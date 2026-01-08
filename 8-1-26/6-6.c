// Exercise 6-6. Implement a simple version of the #define processor (i.e., no arguments)
// suitable for use with C programs, based on the routines of this section. You may also find
// getch and ungetch helpful.


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define HASHSIZE 101

#define MAXBUFSIZE 1000
#define MAXWORD 100

typedef struct nlist
{
    struct nlist *next;
    char *word;
    char *repl;
} Node;

static Node *hashtab[HASHSIZE];

char buffer[MAXBUFSIZE];
int bufptr = 0;

int getch()
{
    int c;
    if (bufptr > 0)
    {
        c = buffer[--bufptr];
    }
    else
    {
        c = getchar();
    }
    if(c=='\n'){
        printf("\n");
    }
    return c;
}

void ungetch(char ch)
{
    if (bufptr >= MAXBUFSIZE - 1)
    {
        printf("Buffer Full\n");
    }
    else
    {
        buffer[bufptr++] = ch;
    }
}

int getword(char *word, int lim)
{
    char *w = word;
    int ch, next;
    while (isspace(ch = getch()))
        ;

    if (ch == '"')
    {
        while (--lim > 0)
        {
            int c = getch();
            if (c == '"')
            {
                break;
            }
        }
        *w = '\0';
        return word[0];
    }

    if (ch != EOF)
    {
        *w++ = ch;
    }

    if (!isalpha(ch))
    {
        *w = '\0';
        return ch;
    }
    for (; --lim > 0; w++)
    {
        if (!isalnum(*w = getch()))
        {
            ungetch(*w);
            break;
        }
    }
    *w = '\0';
    return word[0];
}

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
        if (temp == np)
        {
            hashtab[h] = temp->next;
        }
        else
        {
            while (temp->next != np)
            {
                temp = temp->next;
            }
            temp->next = np->next;
        }
        free((void *)np->word);
        free((void *)np->repl);
        free((void *)np);
        return;
    }
}

int main()
{

    char word[MAXWORD];

    while (getword(word, MAXWORD) != EOF)
    {
        if(strcmp(word,"#")==0){
            getword(word,MAXWORD);
            if(strcmp(word,"define")==0){
                char name[MAXWORD],repl[MAXWORD];
                getword(name,MAXWORD);
                getword(repl,MAXWORD);
                install(name,repl);
            }
        }else if(isalpha(word[0])){
            Node* np = lookup(word);
            if(np){
                printf("%s ",np->repl);
            }else{
                printf("%s ",word);
            }
        }else{
            printf("%s ",word);
        }
    }
    printf("\n");

    return 0;
}