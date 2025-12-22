#include<stdio.h>

void printArray(int arr[],int n){
    for(int i=0;i<n;i++){
        printf("%d ",arr[i]);
    }
    printf("\n");
}

void swap(int* a,int* b){
    int temp = *a;
    *a = *b;
    *b = temp;
}

void shellsort(int arr[],int n){
    for(int gap=(n/2);gap>0;gap/=2){
        for(int j=gap;j<n;j++){
            for(int i=j-gap;i>=0 && arr[i]>arr[i+gap];i-=gap){
                swap(&arr[i+gap],&arr[i]);
            }
        }
    }
}

int main(){
    int n = 10;
    int arr[] = {12,4,77,45,2,6,3,34,1,3};
    shellsort(arr,10);
    printArray(arr,n); 
    return 0;
}