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

    inode_t inode[20];
    int ii;
    for (ii=0; ii<20; ii++)
    {
        status = FS_Alloc_Inode(&inode[ii]);
        if (status != 0) 
        {
            printf("FS_Alloc_Inode failed\n");
        }
        printf("Alloc: %d\n", inode[ii].inode_number);
    }

    for (ii=5; ii<15; ii++)
    {
        printf("Free: %d\n", inode[ii].inode_number);
        status = FS_Free_Inode(&inode[ii]);
        if (status != 0) 
        {
            printf("FS_Alloc_Inode failed\n");
        }
    }

    for (ii=5; ii<15; ii++)
    {
        status = FS_Alloc_Inode(&inode[ii]);
        if (status != 0) 
        {
            printf("FS_Alloc_Inode failed\n");
        }
        printf("Alloc: %d\n", inode[ii].inode_number);
    }

    status = FS_Close(MY_FS_NAME);
    if (status != 0)
    {
        printf("Close_FS failed\n");
        return -1;
    }

    return 0;
}
