#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define HASHSIZE 101

typedef struct nlist
{
    struct nlist *next;
    char *word;
    char *repl;
} Node;

static Node *hashtab[HASHSIZE];

Node *alloc(char *word, char *repl)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->next = NULL;
    newNode->word = strdup(word);
    newNode->repl = strdup(repl);
    return newNode;
}

unsigned hash(char *s)
{
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
    {
        hashval = *s + 31 * hashval;
    }
    return hashval % HASHSIZE;
}

Node *lookup(char *s)
{
    Node *current = hashtab[hash(s)];
    if (!current)
    {
        return NULL;
    }
    while (current != NULL)
    {
        if (strcmp(current->word, s) == 0)
        {
            return current;
        }
        current = current->next;
    }

    return NULL;
}

Node *install(char *word, char *repl)
{
    Node *np;
    unsigned h = hash(word);
    if ((np = lookup(word)) == NULL)
    {
        np = alloc(word, repl);
        if (hashtab[h] == NULL)
        {
            hashtab[h] = np;
        }
        else
        {
            Node *temp = hashtab[h];
            while (temp->next != NULL)
            {
                temp = temp->next;
            }
            temp->next = np;
        }
    }
    else
    {
        free((void *)np->repl);
        np->repl = strdup(repl);
    }
    return np;
}

void undef(char *word)
{
    Node *np;
    if ((np = lookup(word)) != NULL)
    {
        unsigned h = hash(word);
        Node *temp = hashtab[h];
        if (temp == np){
            hashtab[h] = temp->next;
        }else{
            while(temp->next!=np){
                temp = temp->next;
            }
            temp->next = np->next;
        }
        free((void*)np->word);
        free((void*)np->repl);
        free((void*)np);
        return;
    }
}

int main()
{

    install("kathirvel", "shanthi");
    install("sanjay", "nirmal");

    printf("%s\n%s\n%s\n", lookup("kathirvel")->repl, lookup("sanjay")->repl, (lookup("nirmal") == NULL) ? "NULL" : lookup("nirmal")->repl);
    undef("kathirvel");
    printf("%s\n%s\n%s\n", (lookup("kathirvel") == NULL) ? "NULL" : lookup("kathirvel")->repl, lookup("sanjay")->repl, (lookup("nirmal") == NULL) ? "NULL" : lookup("nirmal")->repl);
    
    return 0;
}