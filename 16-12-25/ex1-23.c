/* Exercise 1-24. Write a program to check a C program for rudimentary syntax errors like
unmatched parentheses, brackets and braces. Don't forget about quotes, both single and
double, escape sequences, and comments. (This program is hard if you do it in full
generality.) */


#include <stdio.h>
#include <stdlib.h>

#define OPEN 1
#define CLOSE 0

typedef struct node
{
    char symbol;
    struct node *next;
} Node;

void push(Node **stack, char symbol)
{
    Node *newnode = (Node *)malloc(sizeof(Node));
    newnode->symbol = symbol;
    if (*stack == NULL)
    {
        newnode->next = NULL;
    }
    else
    {
        newnode->next = *stack;
    }
    *stack = newnode;
}

char pop(Node **stack)
{
    if (*stack == NULL)
    {
        return '*';
    }
    else
    {
        Node *curr = *stack;
        *stack = curr->next;
        return curr->symbol;
    }
}

char top(Node *stack)
{
    if (stack == NULL)
    {
        return '*';
    }
    else
    {
        return stack->symbol;
    }
}

void printStack(Node *stack)
{
    Node *temp = stack;
    while (temp != NULL)
    {
        printf("%c\n", temp->symbol);
        temp = temp->next;
    }
}


int main()
{
    Node *s = NULL;

    char ch,prev='*';
    int f = 1;
    int non_comment = 1;

    int sing = CLOSE, doub = CLOSE;

    while ((ch = getchar()) != EOF)
    {
        
        if(ch=='/' && prev=='/'){

            ch = getchar();
            while(ch!='\n' && ch!=EOF){
                ch = getchar();
            }

        }else{
            if (ch == '[' || ch == '{' || ch == '(')
            {
                push(&s, ch);
            }
            else if (ch == '\'' && sing == CLOSE)
            {
                push(&s, ch);
                sing = OPEN;
            }
            else if (ch == '\"' && doub == CLOSE)
            {
                push(&s, ch);
                doub = OPEN;
            }
            else if (ch == ']')
            {
                if (top(s) == '[')
                {
                    pop(&s);
                }
                else
                {
                    f = 0;
                    break;
                }
            }
            else if (ch == ')')
            {
                if (top(s) == '(')
                {
                    pop(&s);
                }
                else
                {
                    f = 0;
                    break;
                }
            }
            else if (ch == '}')
            {
                if (top(s) == '{')
                {
                    pop(&s);
                }
                else
                {
                    f = 0;
                    break;
                }
            }
            else if (ch == '\'')
            {
                if (top(s) == '\'' && sing == OPEN)
                {
                    pop(&s);
                    sing = CLOSE;
                }
                else
                {
                    f = 0;
                    break;
                }
            }
            else if (ch == '\"')
            {
                if (top(s) == '\"' && doub == OPEN)
                {
                    pop(&s);
                    doub = CLOSE;
                }
                else
                {
                    f = 0;
                    break;
                }
            }
            
        }
        prev = ch;
        
    }

    if (f == 1 && top(s) == '*')
    {
        printf("Valid\n");
    }
    else
    {
        printf("Invalid\n");
    }

    return 0;
}