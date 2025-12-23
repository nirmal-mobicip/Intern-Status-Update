
// Exercise 4-4. Add the commands to print the top elements of the stack without popping, to
// duplicate it, and to swap the top two elements. Add a command to clear the stack


#include <stdio.h>
#include <ctype.h>
#include<stdlib.h>

typedef struct node{
    int data;
    struct node* next;
}Node;

typedef enum{
    STACK_OK,
    STACK_EMPTY,
    STACK_MEMORY_ALLOC_FAILED
}Status;



Status push(Node** head, int val){
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

Status pop(Node** head,int* out){
    if(*head==NULL){
        return STACK_EMPTY;
    }else{
        Node* h = *head;
        int data = h->data;
        *head = (*head)->next;
        free(h);
        *out = data;
        return STACK_OK;
    }
}

Status top(const Node* head,int* out){
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
    return STACK_OK;
}

Node* duplicate(Node* stack){
    Node* s = NULL;
    Node* curr = s;
    Node* temp = stack;
    while(temp!=NULL){
        Node* newnode = (Node*)malloc(sizeof(Node));
        newnode->data = temp->data;
        newnode->next = NULL;
        if(curr==NULL){
            s = newnode;
            curr = newnode;
        }else{
            curr->next = newnode;
            curr = newnode;
        }
        temp = temp->next;
    }
    return s;
}

void swap(Node** stk){
    int v1,v2;
    if(pop(stk,&v1)!=STACK_EMPTY && pop(stk,&v2)!=STACK_EMPTY){
        // printf("%d-%d\n",v1,v2);
        push(stk,v1);
        push(stk,v2);
    }else{
        printf("Cannot perform Swap in stack containing elements < 2.\n");
    }

}

int main(){
    Node* stack = NULL;
    push(&stack,13);
    push(&stack,14);
    push(&stack,15);
    int val;
    if(top(stack,&val)!=STACK_EMPTY){
        printf("Top Element : %d\n",val);
    }
    /*----------------------print top elements---------------------------*/
    Node *s1 = NULL;
    push(&s1,13);
    push(&s1,14);
    push(&s1,15);

    Node* s2 = duplicate(s1);
    printf("Duplicate Values : \n");
    while(pop(&s2,&val)!=STACK_EMPTY){
        printf("%d\n",val);
    }
    /*-----------------------duplicate function----------------------*/    

    empty_stack(&s2);
    
    if(top(s2,&val)!=STACK_EMPTY){
        printf("%d\n",val);
    }else{
        printf("Empty Stack\n");
    }

    /*-----------------------empty stack-----------------------------*/

    Node* stk = NULL;
    push(&stk,13);
    push(&stk,14);
    push(&stk,15);

    
    swap(&stk);
    printf("After Swap : \n");
    while(pop(&stk,&val)!=STACK_EMPTY){
        printf("%d\n",val);
    }


    /*-----------------------swap top 2 elements---------------------*/

}