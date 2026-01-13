#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>

int main(int argc,char* argv[]){

    if(argc>1){
        char buffer[BUFSIZ];
        for(int i=1;i<argc;i++){
            int fd,n;
            if((fd = open(argv[i],O_RDONLY,0))==-1){
                fprintf(stderr,"Can't open file : %s\n",argv[i]);
                continue;
            }
            while((n=read(fd,buffer,BUFSIZ))>0){
                if(write(STDOUT_FILENO,buffer,n)!=n){
                    fprintf(stderr,"Some error occured in file : %s\n",argv[i]);
                    break;
                }
            }
            close(fd);
        }
    }else{
        fprintf(stderr,"Usage : cat file1 file2 ...");
    }

    return 0;
}


// create a bigfile : dd if=/dev/zero of=bigfile bs=1M count=1024
// test command : time ./lowlevel_cat bigfile > /dev/null    time ./standard_cat bigfile > /dev/null
// average speed command : 
// for i in {1..5}; do time ./lowlevel_cat bigfile > /dev/null; done
// for i in {1..5}; do time ./standard_cat bigfile > /dev/null; done


// after testing : the lowlevel_cat is faster comparitively



