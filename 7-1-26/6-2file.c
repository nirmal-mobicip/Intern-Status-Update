#include <stdio.h>

/* 
   This comment mentions variables like counterAlpha
   and counterBeta but they should NOT be counted.
*/

int counterAlpha;
int counterBeta;
int counterGamma;

int counter;          // same prefix, but identical name → ignored
int counter;          // duplicate

int countDown;        // different prefix (countD ≠ counte)

char *message = "counterAlpha counterBeta";  // inside string → ignored

int _hiddenValueOne;
int _hiddenValueTwo;
int _hiddenValueThree;

/* multiline comment
   with fake variables:
   _hiddenValueFour
*/

int computeResult;
int computeRescue;
int computeRestore;

int main(void)
{
    int localCounterAlpha;
    int localCounterBeta;

    printf("computeResult = %d\n", computeResult);
    return 0;
}
