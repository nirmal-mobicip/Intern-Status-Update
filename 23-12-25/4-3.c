
// Exercise 4-3. Given the basic framework, it's straightforward to extend the calculator. Add
// the modulus (%) operator and provisions for negative numbers.

#include <stdio.h>
#include <ctype.h>
#include<stdlib.h>

#define MAXLINE 100
#define MAXOPS 30



typedef struct node{
    double data;
    struct node* next;
}Node;

typedef enum{
    STACK_OK,
    STACK_EMPTY,
    STACK_MEMORY_ALLOC_FAILED
}Status;



Status push(Node** head, double val){
    Node* newnode = (Node*)malloc(sizeof(Node));
    if(!newnode) return STACK_MEMORY_ALLOC_FAILED;
    newnode->data = val;
    newnode->next = NULL;
    if(*head!=NULL){
        newnode->next = *head;
    }
    *head = newnode;
    return STACK_OK;
}

Status pop(Node** head,double* out){
    if(*head==NULL){
        return STACK_EMPTY;
    }else{
        Node* h = *head;
        double data = h->data;
        *head = (*head)->next;
        free(h);
        *out = data;
        return STACK_OK;
    }
}

Status top(const Node* head,double* out){
    if(head==NULL){
        return STACK_EMPTY;
    }else{
        *out = head->data;
        return STACK_OK;
    }
}

Status empty_stack(Node** head){
    Node* temp;
    while ((*head)!=NULL){
        temp = *head;
        *head = (*head)->next;
        free(temp);
    }
}


int f = 1,n,stack_length = 0;
char ch,ops[MAXOPS][20];
Node *stack = NULL;


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

void evaluate(){
    int i;
        for (i = 0; i < n; i++)
        {
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
    
    }

    return 0;
}