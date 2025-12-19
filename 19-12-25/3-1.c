// Exercise 3-1. Our binary search makes two tests inside the loop, when one would suffice (at
// the price of more tests outside.) Write a version with only one test inside the loop and
// measure the difference in run-time.

#include<stdio.h>

int binsch(int arr[],int n,int x){
    int start = 0, end = n-1,mid;
    while(end!=start){
        int mid = (end+start+1)/2;
        if(arr[mid]<=x){
            start = mid;                            // skipped the condition check arr[mid]>x and arr[mid]==x
        }else{
            end = mid-1;
        }
    }
    return (arr[start]==x)?start:-1;
}

int main(){

    int arr[] = {1,2,3,4,5,6,7,8,9,10};
    int n = 10;
    printf("%d\n",binsch(arr,n,9));

    return 0;
}