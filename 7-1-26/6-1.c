#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define MAXBUFSIZE 1000
#define MAXWORD 100
#define NKEYS 11

// structure

struct key
{
    char *word;
    int count;
} keytab[] = {
    "auto", 0,
    "break", 0,
    "case", 0,
    "char", 0,
    "const", 0,
    "continue", 0,
    "default", 0,
    "float", 0,
    "int", 0,
    "unsigned", 0,
    "void", 0,
    "volatile", 0,
    "while", 0};

// getch() and ungetch()

char buffer[MAXBUFSIZE];
int bufptr = 0;

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


// binsearch()

int binsearch(struct key keys[], char *word, int n)
{
    
    int first = 0;
    int last = n - 1;
    
    while (first <= last)
    {
        int mid = (first + last) / 2;
        int value = strcmp(keys[mid].word, word);
        if (value == 0)
        {
            return mid;
        }
        else if (value < 0)
        {
            first = mid + 1;
        }
        else
        {
            last = mid - 1;
        }
    }
    
    return -1;
}

// getword()

int getword(char *word, int lim)
{
    char *w = word;
    int ch, next;
    while (isspace(ch = getch()))
        ;
    if (ch != EOF)
    {
        *w++ = ch;
    }

    if (ch == '#'){
        while((ch=getch())!='\n' && ch!=EOF){
            *w++ = ch;
        }
        *w = '\0';
        return word[0];
    }

    if(ch == '/'){                                      // for comments

        next = getch();

        if(next=='/'){                                  // for single line comment
            while((ch=getch())!='\n' && ch!=EOF)
                ;
            return getword(word,lim);
        }else if(next == '*'){                          // for multi line comment
            int prev = 0;
            while((ch=getch())!=EOF){
                if(prev=='*' && ch=='/'){
                    break;
                }
                prev = ch;
            }
            return getword(word,lim);
        }

    }


    if(ch=='"'){                            // for string literal
        while(--lim>0){
            int c = getch();
            *w++ = c;
            if(c=='"'){
                break;
            }
        }
        *w = '\0';
        return word[0];
    }
    if (!isalpha(ch) && ch != '_')                      // for underscore
    {
        *w = '\0';
        return ch;
    }
    for (; --lim > 0; w++)
    {
        if (!isalnum(*w = getch()) && *w != '_')        // for underscore
        {
            ungetch(*w);
            break;
        }
    }
    *w = '\0';
    return word[0];
}

int main()
{

    char word[MAXWORD];
    int pos;
    while (getword(word, MAXWORD) != EOF)
    {

        printf("%s\n", word);
        // if((pos = binsearch(keytab,word,NKEYS))!=-1){
        //     keytab[pos].count+=1;
        // }
    }

    // for(int i=0;i<NKEYS;i++){
    //     printf("%8s - %d\n",keytab[i].word,keytab[i].count);
    // }

    return 0;
}