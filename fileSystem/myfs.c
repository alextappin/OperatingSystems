#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include "myfs.h"

static int fs_fd = -1;
static int FS_Num_Blocks = 1000;            // in blocks
static int FS_Num_Inodes = 100;             // Max number of files

#define FS_INODES_PER_BLOCK  (FS_BLOCK_SIZE/sizeof(inode_t))

// the Super Block is kept in memory while FS is open
static super_block_t Super_Block;

// Cache of the last block of inodes read
static inode_t FS_Inode_Cache[FS_INODES_PER_BLOCK + 1];

// block number of the block in the Inode_Cache
static int FS_Inode_Cache_Block = -1;

// How many blocks are in the inode area?
#define FS_NUM_INODE_BLOCKS     (FS_Num_Inodes / FS_INODES_PER_BLOCK + 1)

// Index of first Data Block
#define FS_FIRST_DATA_BLOCK     (FS_NUM_INODE_BLOCKS + 2)

// Number of items in the Super Block's free list
#define FS_FREE_LIST_SIZE       (sizeof(Super_Block.free_list)/sizeof(Super_Block.free_list[0]) - 1)

// Number of items in the Super Block's free inode list
#define FS_FREE_INODE_SIZE      (sizeof(Super_Block.free_inode_list)/sizeof(Super_Block.free_inode_list[0]))

//*************************************
int FS_Close()
{
    int status;

    status = FS_Write(&Super_Block, 1);
    if (status != 0) return status;

    return close(fs_fd);
}

//*************************************
int FS_Open(const char *filename)
{
    int status;

    // indicate no inodes in the cache
    FS_Inode_Cache_Block = -1;

    // file is opened with O_SYNC so all activity gets flushed to disk
    fs_fd = open(filename, O_RDWR | O_SYNC);

    if (fs_fd < 0) return -1;

    // Read the Super Block so we have it in memory
    status = FS_Read(&Super_Block, 1);
    if (status != 0) return status;

    return 0;
}

//*************************************
// NOTE: This will destructively format the file system
int FS_Create(const char *filename, u_int32_t fs_size)
{
    // buffer we can use to initialize all blocks on the virtual drive
    char block[FS_BLOCK_SIZE];
    int ii;
    int status;

    // create the file
    // Truncate the file if it already exists
    // NOTE: we do NOT use O_SYNC so that initialation goes fast
    //       we will reopen after init, so the live FS has O_SYNC
    fs_fd = open(filename, O_CREAT | O_RDWR | O_TRUNC , S_IRUSR | S_IWUSR);
    if (fs_fd < 0) return 1;

    FS_Num_Blocks = fs_size;

    // Fill the FS with zeros
    memset(block, 0, FS_BLOCK_SIZE);
    for (ii=0; ii<FS_Num_Blocks; ii++)
    {
        status = write(fs_fd, block, FS_BLOCK_SIZE);
        if (status != FS_BLOCK_SIZE) return -1;
    }

    // Initialize the Super Block
    memset(&Super_Block, 0, sizeof(Super_Block));

    // Free all data blocks
    for (ii=FS_Num_Blocks-1; ii>=FS_FIRST_DATA_BLOCK; ii--)
    {
        FS_Free_Block(ii);
    }

    // Free all inodes
    inode_t inode;
    memset(&inode, 0, sizeof(inode));
    for (ii=0; ii<FS_Num_Inodes; ii++)
    {
        inode.free = 0;
        inode.inode_number = ii;
        FS_Free_Inode(&inode);
    }

    // Close so everything is flushed to disk
    status = FS_Close();
    if (status) return status;

    // Open the live FS
    status = FS_Open(filename);
    if (status) return status;

    return 0;
}

//*************************************
int FS_Read(void *buff, u_int32_t block)
{
    int status;
    
    status = lseek(fs_fd, block*FS_BLOCK_SIZE, SEEK_SET);
    if (status != block*FS_BLOCK_SIZE) return -1;

    status = read(fs_fd, buff, FS_BLOCK_SIZE);
    if (status == FS_BLOCK_SIZE) return 0;

    return -1;
}

//*************************************
int FS_Write(void *buff, u_int32_t block)
{
    int status;
    
    status = lseek(fs_fd, block*FS_BLOCK_SIZE, SEEK_SET);
    if (status != block*FS_BLOCK_SIZE) return -1;

    status = write(fs_fd, buff, FS_BLOCK_SIZE);
    if (status == FS_BLOCK_SIZE) return 0;

    return -1;
}

//*************************************
int FS_Read_Inode(inode_t *inode, u_int32_t index)
{
    int inode_block;
    int status;
    int offset;

    // what block is the inode stored in?
    // Add 2 to account for boot block and super block
    inode_block = index / FS_INODES_PER_BLOCK + 2;

    // offset of the inode within the block
    offset = index % FS_INODES_PER_BLOCK;

    // if the desired block is NOT already in our cache, read it
    if (inode_block != FS_Inode_Cache_Block)
    {
        status = FS_Read(&FS_Inode_Cache, inode_block);
        if (status != 0)
        {
            FS_Inode_Cache_Block = -1;
            return status;
        }
        FS_Inode_Cache_Block = inode_block;
    }

    // copy the requested inode from the cache into user's buffer
    memcpy(inode, &FS_Inode_Cache[offset], sizeof(inode_t));

    return 0;
}
//*************************************
int FS_Write_Inode(inode_t *inode)
{
    int inode_block;
    u_int32_t index;
    int offset;
    int status;

    //inode number
    index = inode->inode_number;

    //find block for inode
    inode_block = index / FS_INODES_PER_BLOCK + 2;

    offset = index % FS_INODES_PER_BLOCK;

    //cahce it
    if (inode_block != FS_Inode_Cache_Block)
    {
        status = FS_Read(&FS_Inode_Cache, inode_block);
        if (status != 0)
        {
            FS_Inode_Cache_Block = -1;
            return status;
        }
        FS_Inode_Cache_Block = inode_block;
    }

    //copy and write inode
    memcpy(&FS_Inode_Cache[offset], inode, sizeof(inode_t));

    status = FS_Write(&FS_Inode_Cache, inode_block);
    if (status != 0) return status;

    return 0;
}
//*************************************
int FS_Read_File_Block(inode_t *inode, void *buff, u_int32_t block)
{
    //check if double or tiple indirect
    if (block >= 266) return -1;
    
    int status = 0;
    u_int32_t tempDiskBlock;
    
    // Check for end of file
    if ((block + 1) * FS_BLOCK_SIZE > inode->size) return 0;
    // Check if in first 10
    if (block < 10)
        tempDiskBlock = inode->disk_map[block];
    //if its not in first 10 do this
    else
    {
        indirect_block_t i_block;   
        int new_block = block - 10;
        
        //Get Indirect block from the disk map[10]
        tempDiskBlock = inode->disk_map[10];
        if (tempDiskBlock < 0) return -1;
        
        status = FS_Read(&i_block, tempDiskBlock);
        if (status != 0) return status;
        // Set disk block to new indirect block
        tempDiskBlock = i_block.block[new_block];
    }
    //if it has data in it
    if (tempDiskBlock != 0)
    {
        status = FS_Read(buff, tempDiskBlock);
        if (status != 0) return status;
    }
    //fill it with 0000
    else
        memset(buff, 0, FS_BLOCK_SIZE);
    
    return FS_BLOCK_SIZE;
}
//*************************************
int FS_Write_File_Block(inode_t *inode, void *buff, u_int32_t block)
{
    //check if in double indirect or triple first
    if (block >= 266) return -1;
    
    int status = 0;
    u_int32_t tempDiskBlock = inode->disk_map[block];
    
    //if its in first 10 and empty
    if (block < 10 && tempDiskBlock == 0)
    {
        tempDiskBlock = FS_Alloc_Block();
        
        if (tempDiskBlock < 0) return -1;
        
        //status will change
        inode->disk_map[block] = tempDiskBlock;
        status = FS_Write_Inode(inode);
        
        if (status != 0) return status;
    }
    else
    {
        indirect_block_t i_block;  
        //index
        int new_block = block - 10;
        //indirect
        tempDiskBlock = inode->disk_map[10];
        
        //index empty
        if (tempDiskBlock == 0)
        {
            tempDiskBlock = FS_Alloc_Block();
            
            if (tempDiskBlock < 0) return -1;
            
            inode->disk_map[10] = tempDiskBlock;
            //Set it to 0s then update
            memset(&i_block, 0, sizeof(indirect_block_t));
            status = FS_Write_Inode(inode);
        }
        //else just read it
        else
            status = FS_Read(&i_block, tempDiskBlock);
            
        if (status != 0) return status;
          
        //Get block
        tempDiskBlock = i_block.block[new_block];
        
        //If empty
        if (tempDiskBlock == 0)
        {
            tempDiskBlock = FS_Alloc_Block();
            
            if (tempDiskBlock < 0) return -1;
                
            //Pu tin indirect then write it
            i_block.block[new_block] = tempDiskBlock;
            status = FS_Write(&i_block, inode->disk_map[10]);
            
            if (status != 0) return status;
        }
    }
    u_int32_t new_file_size = (block + 1) * FS_BLOCK_SIZE;
        
    //If it increased then update it
    if (inode->size < new_file_size)
    {
        inode->size = new_file_size;
        status = FS_Write_Inode(inode);
        
        if (status != 0) return status;
    }
    
    status = FS_Write(buff, tempDiskBlock);
    
    if (status != 0) return status;
    
    return FS_BLOCK_SIZE;
}
//*************************************
int FS_Alloc_Inode(inode_t *inode)
{
    int status;
    //If the cache is empty then find the inodes
    if (Super_Block.num_free_inodes == 0)
        FindFreeInode(); 
    //still empty
    if (Super_Block.num_free_inodes == 0)
        return -1;
    
    Super_Block.num_free_inodes--;
    FS_Read_Inode(inode,  Super_Block.free_inode_list[Super_Block.num_free_inodes]);
    //working 
    inode->free = 0;
    //Write inode
    status = FS_Write_Inode(inode);
    
    return status;
}
//*************************************
int FS_Free_Inode(inode_t *inode)
{
    int status;

    if (Super_Block.num_free_inodes < FS_FREE_INODE_SIZE)
    {
        Super_Block.free_inode_list[ Super_Block.num_free_inodes] = 
            inode->inode_number;
        Super_Block.num_free_inodes++;

        status = FS_Write(&Super_Block, 1);
        if (status != 0) return status;
    }

    inode->free = 1;

    status = FS_Write_Inode(inode);

    return status;
}
//*************************************
int FS_Alloc_Block()
{
    int status;
    int nextFreeBlock;
    
    if(Super_Block.num_free_blocks < 2)
    {
        super_block_t buffer;
        nextFreeBlock = Super_Block.free_list[0];
        
        status = FS_Read(&buffer, nextFreeBlock);
        if (status != 0) 
            return status;
            
        memcpy(&Super_Block.free_list, &buffer.free_list, sizeof(buffer.free_list));
        
        Super_Block.num_free_blocks = buffer.num_free_blocks;
    }
    else
    {
        Super_Block.num_free_blocks--;
        nextFreeBlock = Super_Block.free_list[Super_Block.num_free_blocks];
    }
    
    status = FS_Write(&Super_Block, 1);
    
    if(status != 0)
        return status;
    
    return nextFreeBlock;
}
//*************************************
int FS_Free_Block(u_int32_t block)
{
    int status;
    
    if(Super_Block.num_free_blocks < FS_FREE_LIST_SIZE)
    {
        Super_Block.free_list[Super_Block.num_free_blocks] = block;
        Super_Block.num_free_blocks++;
        
        status = FS_Write(&Super_Block, 1);
        if (status != 0) 
            return status;
    }
    else
    {
        status = FS_Write(&Super_Block, block);
        if (status != 0) 
            return status;
        
        Super_Block.free_list[0] = block;
        Super_Block.num_free_blocks = 1;
    }
    return 0;
}
//*************************************
int FindFreeInode()
{
    int currentBlock = FS_Inode_Cache_Block;
    int startBlock = currentBlock;
    //for loops
    int i = 0;
    int status = 0;
    // If current block is not set
    if (currentBlock < 0)
    {
        //Make it the first inode block
        currentBlock = 2;
        status = FS_Read(FS_Inode_Cache, currentBlock);
        
        if (status != 0) return status;
    }

    // While the super block free inode list is still empty
    do
    {
        //find the inodes
        for (i = 0; i < FS_INODES_PER_BLOCK &&
                        Super_Block.num_free_inodes < FS_FREE_INODE_SIZE; ++i)
        {
            //Only if the inode is free
            if (FS_Inode_Cache[i].free == 1)
            {
                Super_Block.free_inode_list[Super_Block.num_free_inodes] = 
                    FS_Inode_Cache[i].inode_number;
                    
                //Inodes incremented
                Super_Block.num_free_inodes++;
            }
        }
        
        if (Super_Block.num_free_inodes == 0)
        {
            //if its the last block
            if (currentBlock == FS_NUM_INODE_BLOCKS + 1)
                currentBlock = 2;
            else
                currentBlock++;
                
            // Read in that block from the disk
            status = FS_Read(FS_Inode_Cache, currentBlock);
            
            if (status != 0) return status;
                
            FS_Inode_Cache_Block = currentBlock;
        }
    } while (Super_Block.num_free_inodes == 0 && currentBlock != startBlock);
    
    // Write super block to disk
    status = FS_Write(&Super_Block, 1);
    
    if (status != 0) return status;
    
    return status;
}
