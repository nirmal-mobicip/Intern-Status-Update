// Exercise 6-2. Write a program that reads a C program and prints in alphabetical order each
// group of variable names that are identical in the first 6 characters, but different somewhere
// thereafter. Don't count words within strings and comments. Make 6 a parameter that can be
// set from the command line.

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define MAXBUFSIZE 1000
#define MAXWORD 100
#define NKEYS 11

// structure

typedef struct tnode
{
    char *word;
    int count;
    struct tnode *left, *right;
} Node;

// getch() and ungetch()

char buffer[MAXBUFSIZE];
int bufptr = 0;

Node *tree_sorted[1000];
int ptr = 0;

int getch()
{
    if (bufptr > 0)
    {
        return buffer[--bufptr];
    }
    else
    {
        return getchar();
    }
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

// allocate Node

Node *alloc(char *str)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->count = 0;
    newNode->word = strdup(str);
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// addtree()

void addtree(Node **head, char *word)
{
    int comp;
    if (*head == NULL)
    {
        *head = alloc(word);
        (*head)->count = 1;
    }
    else if ((comp = strcmp((*head)->word, word)) == 0)
    {
        (*head)->count++;
    }
    else if (comp < 0)
    {
        addtree(&(*head)->right, word);
    }
    else if (comp > 0)
    {
        addtree(&(*head)->left, word);
    }
}

char *slice(char *str, int start, int end)
{
    char *res = (char *)malloc((end - start + 2) * sizeof(char));
    int j = 0;
    for (int i = start; i <= end; i++)
    {
        res[j++] = str[i];
    }
    res[j] = '\0';
    return res;
}

// printtree

void sorted_tree(struct tnode *p) // inorder traversal for alphabetical order
{
    if (p != NULL)
    {
        sorted_tree(p->left);
        // printf("%4d %s\n", p->count, p->word);
        tree_sorted[ptr++] = p;
        sorted_tree(p->right);
    }
}

int iskeyword(char *word)
{
    static char *keywords[] = {
        "auto","break","case","char","const","continue",
        "default","do","double","else","enum","extern",
        "float","for","goto","if","int","long","register",
        "return","short","signed","sizeof","static",
        "struct","switch","typedef","union","unsigned",
        "void","volatile","while"
    };
    int n = sizeof(keywords) / sizeof(keywords[0]);

    for (int i = 0; i < n; i++)
        if (strcmp(word, keywords[i]) == 0)
            return 1;
    return 0;
}

// getword()

int getword(char *word, int lim)
{
    char *w = word;
    int ch, next;
    while (isspace(ch = getch()))
        ;

    if (ch == '"')
    { // for string literal
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

    if (ch == '#')
    {
        while ((ch = getch()) != '\n' && ch != EOF)
        {
            *w++ = ch;
        }
        *w = '\0';
        return word[0];
    }

    if (ch == '/')
    { // for comments

        next = getch();

        if (next == '/')
        { // for single line comment
            while ((ch = getch()) != '\n' && ch != EOF)
                ;
            return getword(word, lim);
        }
        else if (next == '*')
        { // for multi line comment
            int prev = 0;
            while ((ch = getch()) != EOF)
            {
                if (prev == '*' && ch == '/')
                {
                    break;
                }
                prev = ch;
            }
            return getword(word, lim);
        }
    }

    if (!isalpha(ch) && ch != '_') // for underscore
    {
        *w = '\0';
        return ch;
    }
    for (; --lim > 0; w++)
    {
        if (!isalnum(*w = getch()) && *w != '_') // for underscore
        {
            ungetch(*w);
            break;
        }
    }
    *w = '\0';
    return word[0];
}

int main(int argc, char *argv[])
{
    int n = 6;
    if (argc > 1)
    {
        n = atoi(argv[1]);
    }

    Node *tree = NULL;

    char word[MAXWORD];
    int pos;
    while (getword(word, MAXWORD) != EOF)
    {
        if ((isalpha(word[0]) || word[0] == '_') && !iskeyword(word))
            addtree(&tree, word);
    }

    sorted_tree(tree);

    char *current = slice(tree_sorted[0]->word, 0, n - 1);
    for (int i = 0; i < ptr; i++)
    {
        if (strncmp(current, tree_sorted[i]->word, n) != 0)
        {
            current = slice(tree_sorted[i]->word, 0, n - 1);
            printf("\n");
        }
        printf("%4d %s\n", tree_sorted[i]->count, tree_sorted[i]->word);
    }

    return 0;
}