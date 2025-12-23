// Exercise 4-5. Add access to library functions like sin, exp, and pow. See <math.h> in
// Appendix B, Section 4.

#include<stdio.h>
#include<math.h>


int main(){
    double a = 15,b=4;
    printf("sin(%4.0lf) = %10.2lf\n",a,sin(a));
    printf("exp(%4.0lf) = %10.2lf\n",b,exp(b));
    printf("pow(%2.0lf,%1.0lf) = %10.2lf\n",a,b,pow(a,b));
    printf("sqrt(%3.0lf) = %10.2lf\n",b,sqrt(b));
    
    
    return 0;
}