#include "system.h"

int do_out(int *ptr)
{
    //transmit
    globalReg[1] = PIO_T_IE_XMIT;
    while(ptr[0])
    {
        globalReg[0] = ptr[0];
        ptr++;
        while (globalReg[2] & PIO_T_IE_XMIT == 0)
        {}
    }
}
int do_inp(int op, int param)
{
    typedef struct
    {
        int op;
        int addr;
        int status;
    } io_blk_t;
    io_blk_t io_blk;
    
    io_blk.op = op;
    io_blk.addr = param;
    io_blk.status = 0;
    
    asm(INP_OP, &io_blk);
    
    while (io_blk.op >= 0)
        asm(NOP);
}
int do_exec(int op, char *param)
{
    typedef struct
    {
        int op;
        int addr;
        int status;
    } io_blk_t;
    io_blk_t io_blk;
    int start_addr;
    
    asm(PUSHREG_OP);
    asm(SP_REG);
    start_addr = asm(NOP);
    asm(POPREG_OP, start_addr + 1000);
    asm(BP_REG);
    
    io_blk.op = op;
    io_blk.addr = param;
    io_blk.status = 0;
    
    asm(INP_OP, &io_blk);
    
    while (io_blk.op >= 0)
        asm(NOP);
    
    //clear interrupt
    asm(SETMODE_OP);
    asm(0);
    asm(DUP_OP, io_blk.status);
    asm(POPREG_OP);
    //Frame Pointer
    asm(FP_REG);            
    //Pop
    asm(POPREG_OP);
    //Stack Pointer
    asm(SP_REG);
    asm(JMPUSER_OP);
    //GoTo mem address 8
    asm(8); 
    return 0;
}
int CheckSend()
{
    if (globalReg[2] & PIO_T_IID_XMIT)
    {
        //if there is still characters left in the input
        if (globalString[0] != 0)
        {
            globalReg[0] = globalString[0];
            globalString++;
        }
        else
        {
            globalReg[1] = 0;
            *globalFinish = 1;
        }
    }
}
int send(char * pointer, char * done)
{
    globalString = pointer + 1;
    globalFinish = done;
    //transit
    globalReg[1] = PIO_T_IE_XMIT;
    globalReg[0] = pointer[0];
}
int interrupt()
{
    CheckSend();
    asm(RTI_OP);
}
int systrap(int cmd, char *arg)
{
    //Do Output
    if (cmd == PRINTS_CALL)
        asm(OUTS_OP, arg);
    //do Input
    else if (cmd == GETL_CALL || cmd == GETI_CALL)
    {
        do_inp(cmd, arg);
    }
    else if (cmd == EXIT_CALL)
    {
        asm(HALT_OP);
    }
    //Do exec
    else if (cmd == EXEC_CALL)
    {
        do_exec(cmd, arg);
    }
        
    asm(RTI_OP);
}
int syscall(int op, char * value, int param, char * done_flag)
{
    // Push base pointer
    asm(PUSHREG_OP);
    asm(BP_REG);
    
    int baseP;
    
    baseP = asm(NOP);
    
    // Push the flag register
    asm(PUSHREG_OP);
    asm(FLAG_REG);
    
    int flag_reg;
    char globalFinish;
    
    // Start out not done
    globalFinish = 0;
    
    done_flag = &globalFinish;
    
    // Grab flag
    flag_reg = asm(NOP);
    
    // Check to see if we are in user mode
    if (flag_reg & FL_USER_MODE)
    {
        // Move value to user's address
        value += baseP;
        done_flag += baseP;
    }
    asm(TRAP_OP);
    if (op == PRINTI_CALL || op == PRINTS_CALL)
    {
        while (globalFinish == 0)
        {}
    }
        
    return 0;
}
int printS(char *ptr)
{
    //Prints the String
    syscall(PRINTS_CALL, ptr);
}
int printI(int num)
{
    //Prints the Number
    char buffer[150];
    itostr(num, buffer);
    
    printS(buffer);
}
int scanS(int addr)
{
    //Take String Input
    syscall(GETL_CALL, addr);
}
int scanI(int addr)
{
    //take Integer Input
    syscall(GETI_CALL, addr);
}
int exit(int mainValue)
{
    syscall(EXIT_CALL, "", mainValue);
    
    return 0;
}
int exitProg()
{
    syscall(EXIT_CALL);
    
    return 0;
}
int exec(char * file)
{
    syscall(EXEC_CALL, file, 0, 0);
    
    return 0;
}
int startup__()
{
    // initialize registers
    globalReg = PIO_T_RDR;
    //int mainValue;
    //mainValue = main();
    //exec("main");
    //syscall(EXIT_CALL);
    //exit(mainValue);
    char buffer[150];
    int number;
    printS("Enter in a string: ");
    scanS(buffer);
    printS("'");
    printS(buffer);
    printS("' was the string that was read in\nNow enter in a number: ");
    scanI(&number);
    printS("'");
    printI(number);
    printS("' was the number that was read in\nExiting\n");
    return 0;
}