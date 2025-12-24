
// Exercise 4-10. An alternate organization uses getline to read an entire input line; this makes
// getch and ungetch unnecessary. Revise the calculator to use this approach.

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#define MAXLINE 100


typedef struct node
{
    double data;
    struct node *next;
} Node;

typedef enum
{
    STACK_OK,
    STACK_EMPTY,
    STACK_MEMORY_ALLOC_FAILED
} Status;

Status push(Node **head, double val)
{
    Node *newnode = (Node *)malloc(sizeof(Node));
    if (!newnode)
        return STACK_MEMORY_ALLOC_FAILED;
    newnode->data = val;
    newnode->next = NULL;
    if (*head != NULL)
    {
        newnode->next = *head;
    }
    *head = newnode;
    return STACK_OK;
}

Status pop(Node **head, double *out)
{
    if (*head == NULL)
    {
        return STACK_EMPTY;
    }
    else
    {
        Node *h = *head;
        double data = h->data;
        *head = (*head)->next;
        free(h);
        *out = data;
        return STACK_OK;
    }
}

Status top(const Node *head, double *out)
{
    if (head == NULL)
    {
        return STACK_EMPTY;
    }
    else
    {
        *out = head->data;
        return STACK_OK;
    }
}

Status empty_stack(Node **head)
{
    Node *temp;
    while ((*head) != NULL)
    {
        temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}

int n = 0;
int f=1;
char *ops[MAXLINE];
char ch;
Node *stack = NULL;
int stack_length = 0;
char last_var;

void getLine(char line[])
{
    int ptr = 0, ch;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        line[ptr++] = ch;
    }
    line[ptr] = '\0';
}

void split(char str[])
{
    char *s = strtok(str, " ");
    while (s != NULL)
    {
        ops[n++] = s;
        s = strtok(NULL, " ");
    }
}

void evaluate()
{
    int i;
    for (i = 0; i < n; i++)
    {
        // printf("%s\n",ops[i]);
        if (isdigit(ops[i][0]) || (ops[i][0] == '-' && isdigit(ops[i][1])))
        {
            push(&stack, atof(ops[i]));
            stack_length++;
        }
        else
        {
            if (stack_length >= 2)
            {
                double t1, t2;
                switch (ops[i][0])
                {
                case '+':
                    pop(&stack, &t1);
                    pop(&stack, &t2);
                    push(&stack, t2 + t1);
                    stack_length--;
                    break;
                case '-':
                    pop(&stack, &t1);
                    pop(&stack, &t2);
                    push(&stack, t2 - t1);
                    stack_length--;
                    break;
                case '*':
                    pop(&stack, &t1);
                    pop(&stack, &t2);
                    push(&stack, t2 * t1);
                    stack_length--;
                    break;
                case '/':
                    pop(&stack, &t1);
                    pop(&stack, &t2);
                    stack_length -= 2;
                    if (t1 == 0)
                    {
                        printf("Divide by zero is Invalid\n");
                    }
                    else
                    {
                        push(&stack, t2 / t1);
                        stack_length++;
                    }
                    break;
                case '%':
                    pop(&stack, &t1);
                    pop(&stack, &t2);
                    push(&stack, (double)((int)t2 % (int)t1));
                    stack_length--;
                    break;
                default:
                    printf("Unknown Symbol\n");
                    break;
                }
            }
            else
            {
                printf("Input Invlid\n");
                break;
            }
        }
    }
    
        if (stack_length == 1 && i == n)
        {
            double ans;
            pop(&stack, &ans);
            printf("%lf\n", ans);
        }
        else
        {
            printf("Input Structure is Invalid\n");
        }
    
    empty_stack(&stack);
    stack_length = 0;
}


int main()
{

    while (f)
    {
        char line[MAXLINE];

        getLine(line);

        split(line);

        evaluate();
        n=0;
    }

    return 0;
}
