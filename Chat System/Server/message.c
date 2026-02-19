#include <stdlib.h>

typedef struct message
{
    char* to;
    char* from;
    char* message;
    int message_len;
}Message;

Message* parse_json_string(char* string,int len){
    Message* res = (Message*)malloc(sizeof(Message));

    // from
    char* ptr = strstr(string,"\"from\":\"");
    ptr+=8;
    res->from = ptr;
    while(*ptr != '\"'){
        ptr++;
    }
    *ptr = '\0';
    ptr++;
    // to
    ptr = strstr(ptr,"\"to\":\"");
    ptr+=6;
    res->to = ptr;
    while(*ptr != '\"'){
        ptr++;
    }
    *ptr = '\0';
    ptr++;
    // message
    ptr = strstr(ptr,"\"msg\":\"");
    ptr+=7;
    res->message_len = 0;
    res->message = ptr;
    while(*ptr != '\"'){
        ptr++;
        res->message_len++;
    }
    *ptr = '\0';
    return res;
}

void printMessage(Message m){
    printf("FROM : %s\n",m.from);
    printf("TO : %s\n",m.to);
    printf("MESSAGE LENGTH : %d\n",m.message_len);
    printf("MESSAGE CONTENT : %s\n",m.message);
}
