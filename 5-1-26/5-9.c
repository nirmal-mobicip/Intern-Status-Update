// Exercise 5-9. Rewrite the routines day_of_year and month_day with pointers instead of
// indexing.

#include <stdio.h>

static char daytab[2][13] = {
    {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
    {0, 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}};

int day_of_year(int year, int month, int date)
{
    int leap = year % 4 == 0 && year % 100 != 0 || year % 400 == 0;
    if (month >= 1 && month <= 12 && date >= 1 && date <= daytab[leap][month])
    {
        int day_of_year = date;
        char (*p)[13] = daytab; 
        for (int i = 1; i < month; i++)
        {
            day_of_year += *(*(p+leap)+i);
        }
        return day_of_year;
    }
    else
    {
        printf("Enter valid date!!\n");
        return -1;
    }
}

void month_day(int year, int day_of_year, int *month, int *date)
{
    int leap = year % 4 == 0 && year % 100 != 0 || year % 400 == 0;

    if ((leap && day_of_year >= 1 && day_of_year <= 366) || (!leap && day_of_year >= 1 && day_of_year <= 365))
    {
        int i = 1;
        char (*p)[13] = daytab;
        while (i <= 12 && day_of_year > *(*(p+leap)+i))
        {
            day_of_year -= *(*(p+leap)+i);
            i++;
        }
        *date = day_of_year;
        *month = i;
    }
    else
    {
        printf("Enter valid day!!\n");
    }
}

int main()
{
    // int month, date;
    // month_day(2004, 318, &month, &date);
    // printf("%d-%d\n", date, month);
    printf("%d\n",day_of_year(2004,11,31));
    return 0;
}