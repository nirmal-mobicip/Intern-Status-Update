#include<stdio.h>
#include<string.h>

#define MAX_SIZE 1000
#define MAX_LINE 100

int main(int argc,char* argv[]){

    if(argc==2){
        char* pattern = argv[1];
        char string[MAX_SIZE];
        printf("Enter String : ");
        fgets(string,MAX_SIZE,stdin);
        char* res = strstr(string,pattern);
        if(res!=NULL){
            printf("Pattern found at pos : %ld\n",res-string);
            printf("Starting position of that pattern : %s\n",res);
        }else{
            printf("Pattern not found!\n");
        }
    }else if(argc>2){
        char* pattern = argv[1];
        for(int i=2;i<argc;i++){
            printf("File : %s\n",argv[i]);
            FILE *fp = fopen(argv[i],"r");
            char buffer[MAX_LINE];
            int line = 1,f=0;
            char *res;
            while((fgets(buffer,MAX_LINE,fp)!=NULL)){
                if((res = strstr(buffer,pattern))!=NULL){
                    printf("Pattern Found : \n");
                    printf("Line %d : %s\n",line,res);
                    printf("\n");
                    f=1;
                    break;
                }
                line++;
            }
            fclose(fp);
            if(!f){
                printf("Pattern not found in file!\n");
                printf("\n");
            }
        }
    }

    return 0;
}