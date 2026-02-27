#include <stdlib.h>
#include <string.h>
typedef struct message
{
    char* matchid;
    char player;
    char* code;
    char* message;
}Message;

Message* parse_json_string(char* string,int len){
    char* ptr;
    Message* res = (Message*)malloc(sizeof(Message));
    ptr = strstr(string,"matchid");
    ptr+=10;
    res->matchid = ptr;
    while(*ptr!='\"'){
        ptr++;
    }
    *ptr = '\0';
    ptr++;

    ptr = strstr(ptr,"player");
    res->player = *(ptr+9);

    ptr = strstr(ptr,"code");
    ptr+=7;
    res->code = ptr;
    while(*ptr!='\"'){
        ptr++;
    }
    *ptr = '\0';
    ptr++;

    ptr = strstr(ptr,"message");
    ptr+=10;
    res->message = ptr;
    while(*ptr!='\"'){
        ptr++;
    }
    *ptr = '\0';
    return res;
}

void printMessage(Message *m){
    printf("Match ID : %s\n",m->matchid);
    printf("Player : %c\n",m->player);
    printf("Code : %s\n",m->code);
    printf("Message : %s\n",m->message);
}
