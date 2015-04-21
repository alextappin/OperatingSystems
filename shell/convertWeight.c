//Alexander Tappin
//Operating Systems Spring 2015
//April 5th 2015
//Philip Howard

#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[])
{
    int count;
    float convertingConstant = .45;
    if(argc > 2)
    {
        for (count = 1; count < argc; count++)
	    {
	        if (atoi(argv[count]) == 0 && argv[count] != NULL && argv[count] != '\0')
                printf("Please enter a nonZero value/Correct Value\n\n");
            else
            {
                printf("The weight in lb's is %d\n", atoi(argv[count]));
                printf("The weight in kg's is %f\n\n", atof(argv[count]) * convertingConstant);
            }
	    }
	    return 0;
    }
    
    printf("\nToo Few Parameters Passed To convertWeight.c\n");
    //Multiple return statements because writing else is too complicated
    return 0;
}