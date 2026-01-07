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
    int lines[100];
    int ptr;
    struct tnode *left, *right;
} Node;

int line = 1;

// getch() and ungetch()

char buffer[MAXBUFSIZE];
int bufptr = 0;

Node *tree_sorted[1000];
int ptr = 0;

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
    if (c == '\n')
    {
        line++;
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
        if (ch == '\n')
            line--;
        buffer[bufptr++] = ch;
    }
}

// allocate Node

Node *alloc(char *str)
{
    Node *newNode = (Node *)malloc(sizeof(Node));
    newNode->count = 0;
    newNode->word = strdup(str);
    newNode->ptr = 0;
    newNode->left = NULL;
    newNode->right = NULL;
    return newNode;
}

// addtree()

Node *addtree(Node **head, char *word)
{
    int comp;
    if (*head == NULL)
    {
        *head = alloc(word);
        (*head)->count = 1;
        return *head;
    }
    else if ((comp = strcmp((*head)->word, word)) == 0)
    {
        (*head)->count++;
        return *head;
    }
    else if (comp < 0)
    {
        return addtree(&(*head)->right, word);
    }
    else if (comp > 0)
    {
        return addtree(&(*head)->left, word);
    }
    return *head;
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

// getword()

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
        if (isalpha(word[0]))
        {
            Node *node = addtree(&tree, word);
            if (node->ptr == 0 || node->lines[node->ptr - 1] != line)
                node->lines[node->ptr++] = line;
        }
    }

    sorted_tree(tree);

    for (int i = 0; i < ptr; i++)
    {
        printf("%4d %s ", tree_sorted[i]->count, tree_sorted[i]->word);
        for (int j = 0; j < tree_sorted[i]->ptr; j++)
        {
            printf("%d ", tree_sorted[i]->lines[j]);
        }
        printf("\n");
    }

    return 0;
}