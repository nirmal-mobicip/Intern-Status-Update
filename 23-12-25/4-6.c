
// Exercise 4-6. Add commands for handling variables. (It's easy to provide twenty-six variables
// with single-letter names.) Add a variable for the most recently printed value.

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#define MAXLINE 100
#define MAXOPS 30

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

int f = 1;
char ch;
Node *stack = NULL;
char ops[MAXOPS][20];
int n;
int stack_length = 0;
double var[26] = {0};
char last_var;

void getLine(char line[])
{
    int ptr = 0;
    while ((ch = getchar()) != '\n' && ch != EOF)
    {
        line[ptr++] = ch;
    }
    line[ptr] = '\0';

    if (ch == EOF)
    {
        f = 0;
        printf("\n");
    }
}

void split(char line[])
{

    int i = 0, no = 0, p = 0;

    while (line[i] != '\0')
    {
        while (line[i] != '\0' && line[i] == ' ')
        {
            i++;
        }
        while (line[i] != '\0' && line[i] != ' ')
        {
            ops[no][p++] = line[i++];
        }
        ops[no][p] = '\0';
        no++;
        p = 0;
    }
    n = no;
}

void evaluate()
{
    int i,assignment;
    for (i = 0; i < n; i++)
    {
        assignment = 0;
        // printf("%s\n",ops[i]);
        if (isdigit(ops[i][0]) || (ops[i][0] == '-' && isdigit(ops[i][1])))
        {
            push(&stack, atof(ops[i]));
            stack_length++;
        }
        else if (ops[i][0] >= 'a' && ops[i][0] <= 'z')
        {
            push(&stack, var[ops[i][0] - 'a']);
            last_var = ops[i][0];
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
                case '=':
                    pop(&stack, &t1);
                    pop(&stack, &t2);
                    var[(int)(last_var - 'a')] = t1;
                    assignment = 1;
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
    if (!assignment)
    {
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
    }

    return 0;
}