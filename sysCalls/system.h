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
int syscall(int op, char * value, int param, char * done_flag);
int send(char * pointer);
int CheckSend();
int printS(char *ptr);
int printI(int num);
int scanS(int addr);
int scanI(int addr);
int exit(int mainValue);
int exitProg();
int exec(char *name);

int interrupt();
int systrap(int cmd, char *arg);
int startup__();
