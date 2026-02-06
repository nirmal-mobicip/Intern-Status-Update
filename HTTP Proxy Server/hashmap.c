#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TABLE_SIZE 1024

typedef struct HashNode
{
    char *key;
    char *value;
    struct HashNode *next;
} HashNode;

HashNode *Table[MAX_TABLE_SIZE];

unsigned int hash(const char *key)
{
    const char *str = key;
    size_t hash = 5381;
    int c;
    while ((c = *str++))
        hash = ((hash << 5) + hash) + c;
    return hash % MAX_TABLE_SIZE;
}

HashNode *createNode(char *key, char *value)
{
    HashNode *node = (HashNode *)malloc(sizeof(HashNode));
    node->key = strdup(key);
    node->value = strdup(value);
    node->next = NULL;
    return node;
}

int isSameKeys(char *k1, char *k2)
{
    return strcmp(k1, k2) == 0;
}

void put(char *key, char *value)
{
    unsigned int idx = hash(key);
    if (Table[idx] == NULL)
    {
        Table[idx] = createNode(key, value);
    }
    else
    {
        HashNode *temp = Table[idx];
        if (isSameKeys(temp->key, key))
        {
            free(temp->value);
            temp->value = strdup(value);
            return;
        }
        while (temp->next != NULL)
        {
            temp = temp->next;
            if (isSameKeys(temp->key, key))
            {
                free(temp->value);
                temp->value = strdup(value);
                return;
            }
        }
        temp->next = createNode(key, value);
    }
}
int isAvailable(char *key)
{
    unsigned int idx = hash(key);
    HashNode *temp = Table[idx];
    if (temp != NULL)
    {
        if (isSameKeys(temp->key, key))
        {
            return 1;
        }
        else
        {
            while (temp->next != NULL)
            {
                temp = temp->next;
                if (isSameKeys(temp->key, key))
                {
                    return 1;
                }
            }
        }
    }
    return 0;
}

char *get(char *key)
{
    unsigned int idx = hash(key);
    HashNode *temp = Table[idx];
    if (temp != NULL)
    {
        if (isSameKeys(temp->key, key))
        {
            return temp->value;
        }
        while (temp->next != NULL)
        {
            temp = temp->next;
            if (isSameKeys(temp->key, key))
            {
                return temp->value;
            }
        }
    }
    return NULL;
}

void freeTable()
{
    for (int i = 0; i < MAX_TABLE_SIZE; i++)
    {
        HashNode *temp = Table[i];
        while (temp)
        {
            HashNode *next = temp->next;
            free(temp->key);
            free(temp->value);
            free(temp);
            temp = next;
        }
        Table[i] = NULL;
    }
}

void initTable()
{
    for (int i = 0; i < MAX_TABLE_SIZE; i++)
        Table[i] = NULL;
}

void removeKey(char *key)
{
    int idx = hash(key);
    HashNode *prev = Table[idx];
    if(prev!=NULL){
        HashNode *curr = Table[idx]->next;
        if (isSameKeys(prev->key, key))
        {
            HashNode* temp = Table[idx];
            Table[idx] = Table[idx]->next;
            free(temp->key);
            free(temp->value);
            free(temp);
            return;
        }
        else
        {
            while (curr != NULL)
            {
                if (isSameKeys(curr->key, key))
                {
                    prev->next = curr->next;
                    free(curr->key);
                    free(curr->value);
                    free(curr);
                    return;
                }
                prev = curr;
                curr = curr->next;
            }
        }
    }
}
