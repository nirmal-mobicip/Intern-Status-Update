#include<stdio.h>

int my_isupper_1(char c){                       // very simple logic
    return (c>='A' && c<='Z');                  // takes 2 comparisons
}                                               // doesnt use space

int my_isupper_2(char c){                       // simple logic
    return ((unsigned)(c)-'A' < 26);            // one arithmetic operation and only one comparision 
}                                               // doesnt use space

unsigned const char table[256] = {              // very fast
    ['A'] = 1, ['B'] = 1, ['C'] = 1, ['D'] = 1, // but uses 256 byte space
    ['E'] = 1, ['F'] = 1, ['G'] = 1, ['H'] = 1, // constant time for lookup
    ['I'] = 1, ['J'] = 1, ['K'] = 1, ['L'] = 1, // this can be used when frequent checks are needed
    ['M'] = 1, ['N'] = 1, ['O'] = 1, ['P'] = 1,
    ['Q'] = 1, ['R'] = 1, ['S'] = 1, ['T'] = 1,
    ['U'] = 1, ['V'] = 1, ['W'] = 1, ['X'] = 1,
    ['Y'] = 1, ['Z'] = 1
};

int my_isupper_3(char c){
    return table[(unsigned)c];
}

int main(){
    printf("%d\n",my_isupper_3('A'));
    return 0;
}