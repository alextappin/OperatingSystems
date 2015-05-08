#include "string.h"
#include "syscodes.h"
#include "machine_def.h"
#include "pio_term.h"

char *globalReg;
char *globalString;
char *globalFinish;

int main();

int do_out(int *ptr);
int do_inp(int op, int param);
int do_exec(int op, char *param);
int syscall(int cmd, char *arg);

int printS(char *ptr);
int printI(int num);
int scanS(int addr);
int scanI(int addr);
int exit();
int exec(char *name);

int interrupt();
int systrap(int cmd, char *arg);
int startup__()
{
    // initialize registers
    globalReg = PIO_T_RDR;
    int mainValue;
    mainValue = main();
    exec("main");
    
    exit(mainValue);
    return 0;
}