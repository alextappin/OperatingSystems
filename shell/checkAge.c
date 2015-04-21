

#include <stdio.h>

int main()
{
    int age;
    printf("Enter your age: ");
    scanf("%d", &age);
    if(age > 20)
        printf("You Can Legally Drink Alcohol In The USA\n");
    else
        printf("You Cannot Legally Drink Alcohol In The USA\n");
    return 0;
}