#include <stdio.h>
#include <string.h>

#include "myfs.h"

static const char *MY_FS_NAME = "pwh_fs.disk";
static int Num_Blocks = 1000;                      // in blocks
//*************************************
int main()
{
    int status;

    status = FS_Create(MY_FS_NAME, Num_Blocks);
    if (status != 0)
    {
        printf("Create_FS failed\n");
        return -1;
    }

    status = FS_Close(MY_FS_NAME);
    if (status != 0)
    {
        printf("Close_FS failed\n");
        return -1;
    }

    status = FS_Open(MY_FS_NAME);
    if (status != 0)
    {
        printf("Open_FS failed\n");
        return -1;
    }

    inode_t inode;
    char buffer[FS_BLOCK_SIZE];
    int ii;

    status = FS_Alloc_Inode(&inode);
    if (status != 0) 
    {
        printf("FS_Alloc_Inode failed\n");
        return -1;
    }
    printf("Alloc: %d\n", inode.inode_number);

    for (ii=0; ii<10; ii++)
    {
        sprintf(buffer, "This is block %d\n", ii);
        status = FS_Write_File_Block(&inode, buffer, ii);
        if (status != FS_BLOCK_SIZE) 
        {
            printf("FS_Write_File_Block failed\n");
        }
    }

    for (ii=0; ii<10; ii++)
    {
        status = FS_Read_File_Block(&inode, buffer, ii);
        if (status != FS_BLOCK_SIZE) 
        {
            printf("FS_Read_File_Block failed\n");
        }
        printf(buffer);
    }

    status = FS_Close(MY_FS_NAME);
    if (status != 0)
    {
        printf("Close_FS failed\n");
        return -1;
    }

    return 0;
}
