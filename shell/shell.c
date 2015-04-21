//Alexander Tappin
//Operating Systems Spring 2015
//April 5th 2015
//Philip Howard

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const int INPUT_SIZE = 100;
const int ARGUMENT_SIZE = 10;


int main(int argc, char** argv)
{
    int complete = 0;
    int beenCalled = 0;
    int commandCompare = 0;
    char inputArray [INPUT_SIZE];
    char checkingArray [INPUT_SIZE];
    char* exitCmdString = "exit\n";
    char* goodbyeCmdString = "goodbye\n";
    char* getAgeCmdString = "checkAge\n";
    char* convertWeightCmdString = "convertWeight ";
    char* lineInput;
    char* lineInputSystemCall;
    
    int result;
    int status;
    int count = -1;
    int characterCount;
    int loopCounter = 0;
    //char* arg[ARGUMENT_SIZE];
    //char* arg[] = {"./convertWeight", NULL};
    //int intArg[100];
    char* arg[] = {"./convertWeight", NULL, NULL, NULL, NULL, NULL, NULL, NULL};
    
    while(complete == 0)
    {
        printf("TheTappinShell: ");
        lineInputSystemCall = (char *) malloc(INPUT_SIZE);
        
         lineInput = fgets(inputArray, INPUT_SIZE, stdin);
        
        if (lineInput[strlen(lineInput)-1] != '\n')
        {
            while(lineInput[strlen(lineInput) - 1] != '\n')
            {
                lineInput = fgets(inputArray, INPUT_SIZE, stdin);
                
                if(lineInput[strlen(lineInput) - 1] != '\n')
                    lineInput = fgets(inputArray, INPUT_SIZE, stdin);
            }
            printf("Enter in fewer Characters\n");
        }
        
        else
        {
            //for the standard bash commands
            strcpy(lineInputSystemCall, lineInput);
            
            //exit
            commandCompare = strcmp(lineInput, exitCmdString);
            if (commandCompare == 0)
            {
                printf("Exiting!\n");
                complete = 1;
                beenCalled = 1;
            }
            
            //goodbye
            commandCompare = strcmp(lineInput, goodbyeCmdString);
            if (commandCompare == 0)
            {
                printf("GoodBye Person. Much Love\n");
                complete = 1;
                beenCalled = 1;
            }
            
            //checkYourAge
            commandCompare = strcmp(lineInput, getAgeCmdString);
            if (commandCompare == 0)
            {
                beenCalled = 1;
                result = fork();
                //child
                if (result == 0)
                {
                    execl("./checkAge", "./checkAge", NULL);
                    printf("Error in execl\n");
                    exit(0);
                }
                else
                    waitpid(result, &status, 0);
            }
            
            //convert some weights
            strncpy(checkingArray, lineInput, 14);
            checkingArray[14] = '\0';
            //move pointer so there are only arguements
            lineInput+=13;
            lineInput = strtok(lineInput, " ");
            loopCounter = 1;
            for(lineInput; lineInput != NULL; lineInput = strtok(NULL," "))
            {
                arg[loopCounter] = lineInput;
                loopCounter++;
            }
            commandCompare = strcmp(checkingArray, convertWeightCmdString);
            if (commandCompare == 0)
            {
                beenCalled = 1;
                result = fork();
                //child
                if (result == 0)
                {
                    execvp(arg[0],arg);
                    exit(0);
                }
                else
                    waitpid(result, &status, 0);
            }
            //for standard bash commands. Run if no other commands worked
            if (!beenCalled)
            {
                lineInputSystemCall[sizeof(lineInputSystemCall)] = '\0';
                result = fork();
                if (result == 0)
                {
                    system(lineInputSystemCall);
                    exit(0);
                }
                else
                    waitpid(result, &status, 0);
            }
            //reset it so it reListens for your commands
            free(lineInputSystemCall);
            beenCalled = 0;
        }
    }
    return 0;
}