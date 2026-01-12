#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <limits.h>
#include <string.h>

double stack[100];
int top = -1;

void push(double val)
{
    top++;
    stack[top] = val;
}

double pop()
{
    if (top > -1)
    {
        top--;
        return stack[top + 1];
    }
    else
    {
        return INT_MIN;
    }
}

void calc()
{

    char operator;
    double operand;

    char arg[11];

    while (scanf("%10s", arg) == 1)
    {
        if(strcmp(arg,"=")==0){
            break;
        }
        if (isdigit(arg[0]))
        {
            operand = atof(arg);
            push(operand);
        }
        else
        {
            if (top > 0)
            {
                double arg2 = pop();
                double arg1 = pop();

                if (arg[0] == '+')
                {
                    push(arg1 + arg2);
                }
                else if (arg[0] == '-')
                {
                    push(arg1 - arg2);
                }
                else if (arg[0] == '*')
                {
                    push(arg1 * arg2);
                }
                else if (arg[0] == '/')
                {
                    if (arg2 == 0)
                    {
                        printf("Divide by zero error\n");
                        return;
                    }
                    else
                    {
                        push(arg1 / arg2);
                    }
                }
                else
                {
                    printf("Unrecognised input operator\n");
                    return;
                }
            }
        }
    }
    if (top == 0)
    {
        double res = pop();
        printf("Result : %lf\n", res);
    }
    else
    {
        printf("Some error has occured\n");
        return;
    }
}

int main()
{

    calc();


    // 15 7 1 1 + - / 3 * 2 1 1 + + -
    // =

    return 0;
}