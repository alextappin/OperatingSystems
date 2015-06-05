{"filter":false,"title":"fstest.c","tooltip":"/fileSystem/fstest.c","undoManager":{"stack":[[{"start":{"row":0,"column":0},"end":{"row":61,"column":0},"action":"remove","lines":["#include <stdio.h>","#include <string.h>","","#include \"myfs.h\"","","static const char *MY_FS_NAME = \"my_fs.disk\";","static const int  FS_SIZE = 1000;                      // in blocks","//*************************************","int main()","{","    int status;","","    status = FS_Create(MY_FS_NAME, FS_SIZE);","    if (status != 0)","    {","        printf(\"Create_FS failed\\n\");","        return -1;","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    status = FS_Open(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Open_FS failed\\n\");","        return -1;","    }","","    int block;","    int ii;","    for (ii=0; ii<100; ii++)","    {","        block = FS_Alloc_Block();","        printf(\"Alloc: %d\\n\", block);","    }","","    for (ii=50; ii<100; ii++)","    {","        FS_Free_Block(ii);","    }","","    for (ii=0; ii<100; ii++)","    {","        block = FS_Alloc_Block();","        printf(\"Alloc: %d\\n\", block);","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    return 0;","}",""],"id":2},{"start":{"row":0,"column":0},"end":{"row":74,"column":0},"action":"insert","lines":["#include <stdio.h>","#include <string.h>","","#include \"myfs.h\"","","static const char *MY_FS_NAME = \"pwh_fs.disk\";","static int Num_Blocks = 1000;                      // in blocks","//*************************************","int main()","{","    int status;","","    status = FS_Create(MY_FS_NAME, Num_Blocks);","    if (status != 0)","    {","        printf(\"Create_FS failed\\n\");","        return -1;","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    status = FS_Open(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Open_FS failed\\n\");","        return -1;","    }","","    inode_t inode[20];","    int ii;","    for (ii=0; ii<20; ii++)","    {","        status = FS_Alloc_Inode(&inode[ii]);","        if (status != 0) ","        {","            printf(\"FS_Alloc_Inode failed\\n\");","        }","        printf(\"Alloc: %d\\n\", inode[ii].inode_number);","    }","","    for (ii=5; ii<15; ii++)","    {","        printf(\"Free: %d\\n\", inode[ii].inode_number);","        status = FS_Free_Inode(&inode[ii]);","        if (status != 0) ","        {","            printf(\"FS_Alloc_Inode failed\\n\");","        }","    }","","    for (ii=5; ii<15; ii++)","    {","        status = FS_Alloc_Inode(&inode[ii]);","        if (status != 0) ","        {","            printf(\"FS_Alloc_Inode failed\\n\");","        }","        printf(\"Alloc: %d\\n\", inode[ii].inode_number);","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    return 0;","}",""]}],[{"start":{"row":0,"column":0},"end":{"row":74,"column":0},"action":"remove","lines":["#include <stdio.h>","#include <string.h>","","#include \"myfs.h\"","","static const char *MY_FS_NAME = \"pwh_fs.disk\";","static int Num_Blocks = 1000;                      // in blocks","//*************************************","int main()","{","    int status;","","    status = FS_Create(MY_FS_NAME, Num_Blocks);","    if (status != 0)","    {","        printf(\"Create_FS failed\\n\");","        return -1;","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    status = FS_Open(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Open_FS failed\\n\");","        return -1;","    }","","    inode_t inode[20];","    int ii;","    for (ii=0; ii<20; ii++)","    {","        status = FS_Alloc_Inode(&inode[ii]);","        if (status != 0) ","        {","            printf(\"FS_Alloc_Inode failed\\n\");","        }","        printf(\"Alloc: %d\\n\", inode[ii].inode_number);","    }","","    for (ii=5; ii<15; ii++)","    {","        printf(\"Free: %d\\n\", inode[ii].inode_number);","        status = FS_Free_Inode(&inode[ii]);","        if (status != 0) ","        {","            printf(\"FS_Alloc_Inode failed\\n\");","        }","    }","","    for (ii=5; ii<15; ii++)","    {","        status = FS_Alloc_Inode(&inode[ii]);","        if (status != 0) ","        {","            printf(\"FS_Alloc_Inode failed\\n\");","        }","        printf(\"Alloc: %d\\n\", inode[ii].inode_number);","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    return 0;","}",""],"id":3},{"start":{"row":0,"column":0},"end":{"row":74,"column":0},"action":"insert","lines":["#include <stdio.h>","#include <string.h>","","#include \"myfs.h\"","","static const char *MY_FS_NAME = \"pwh_fs.disk\";","static int Num_Blocks = 1000;                      // in blocks","//*************************************","int main()","{","    int status;","","    status = FS_Create(MY_FS_NAME, Num_Blocks);","    if (status != 0)","    {","        printf(\"Create_FS failed\\n\");","        return -1;","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    status = FS_Open(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Open_FS failed\\n\");","        return -1;","    }","","    inode_t inode;","    char buffer[FS_BLOCK_SIZE];","    int ii;","","    status = FS_Alloc_Inode(&inode);","    if (status != 0) ","    {","        printf(\"FS_Alloc_Inode failed\\n\");","        return -1;","    }","    printf(\"Alloc: %d\\n\", inode.inode_number);","","    for (ii=0; ii<10; ii++)","    {","        sprintf(buffer, \"This is block %d\\n\", ii);","        status = FS_Write_File_Block(&inode, buffer, ii);","        if (status != FS_BLOCK_SIZE) ","        {","            printf(\"FS_Write_File_Block failed\\n\");","        }","    }","","    for (ii=0; ii<10; ii++)","    {","        status = FS_Read_File_Block(&inode, buffer, ii);","        if (status != FS_BLOCK_SIZE) ","        {","            printf(\"FS_Read_File_Block failed\\n\");","        }","        printf(buffer);","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    return 0;","}",""]}],[{"start":{"row":0,"column":0},"end":{"row":74,"column":0},"action":"remove","lines":["#include <stdio.h>","#include <string.h>","","#include \"myfs.h\"","","static const char *MY_FS_NAME = \"pwh_fs.disk\";","static int Num_Blocks = 1000;                      // in blocks","//*************************************","int main()","{","    int status;","","    status = FS_Create(MY_FS_NAME, Num_Blocks);","    if (status != 0)","    {","        printf(\"Create_FS failed\\n\");","        return -1;","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    status = FS_Open(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Open_FS failed\\n\");","        return -1;","    }","","    inode_t inode;","    char buffer[FS_BLOCK_SIZE];","    int ii;","","    status = FS_Alloc_Inode(&inode);","    if (status != 0) ","    {","        printf(\"FS_Alloc_Inode failed\\n\");","        return -1;","    }","    printf(\"Alloc: %d\\n\", inode.inode_number);","","    for (ii=0; ii<10; ii++)","    {","        sprintf(buffer, \"This is block %d\\n\", ii);","        status = FS_Write_File_Block(&inode, buffer, ii);","        if (status != FS_BLOCK_SIZE) ","        {","            printf(\"FS_Write_File_Block failed\\n\");","        }","    }","","    for (ii=0; ii<10; ii++)","    {","        status = FS_Read_File_Block(&inode, buffer, ii);","        if (status != FS_BLOCK_SIZE) ","        {","            printf(\"FS_Read_File_Block failed\\n\");","        }","        printf(buffer);","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    return 0;","}",""],"id":4},{"start":{"row":0,"column":0},"end":{"row":74,"column":0},"action":"insert","lines":["#include <stdio.h>","#include <string.h>","","#include \"myfs.h\"","","static const char *MY_FS_NAME = \"pwh_fs.disk\";","static int Num_Blocks = 1000;                      // in blocks","//*************************************","int main()","{","    int status;","","    status = FS_Create(MY_FS_NAME, Num_Blocks);","    if (status != 0)","    {","        printf(\"Create_FS failed\\n\");","        return -1;","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    status = FS_Open(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Open_FS failed\\n\");","        return -1;","    }","","    inode_t inode;","    char buffer[FS_BLOCK_SIZE];","    int ii;","","    status = FS_Alloc_Inode(&inode);","    if (status != 0) ","    {","        printf(\"FS_Alloc_Inode failed\\n\");","        return -1;","    }","    printf(\"Alloc: %d\\n\", inode.inode_number);","","    for (ii=0; ii<20; ii++)","    {","        sprintf(buffer, \"This is block %d\\n\", ii);","        status = FS_Write_File_Block(&inode, buffer, ii);","        if (status != FS_BLOCK_SIZE) ","        {","            printf(\"FS_Write_File_Block failed\\n\");","        }","    }","","    for (ii=0; ii<20; ii++)","    {","        status = FS_Read_File_Block(&inode, buffer, ii);","        if (status != FS_BLOCK_SIZE) ","        {","            printf(\"FS_Read_File_Block failed\\n\");","        }","        printf(\"block %d %s\", ii, buffer);","    }","","    status = FS_Close(MY_FS_NAME);","    if (status != 0)","    {","        printf(\"Close_FS failed\\n\");","        return -1;","    }","","    return 0;","}",""]}]],"mark":2,"position":2},"ace":{"folds":[],"scrolltop":567,"scrollleft":0,"selection":{"start":{"row":74,"column":0},"end":{"row":74,"column":0},"isBackwards":false},"options":{"guessTabSize":true,"useWrapMode":false,"wrapToView":true},"firstLineState":{"row":32,"mode":"ace/mode/c_cpp"}},"timestamp":1432287116000,"hash":"82fb0d88d63473e2a43922593f15e62bff70d6c6"}