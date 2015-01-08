#include <time.h>
#include <stdio.h>


void timestamp_caca()
{
time_t ltime;
struct tm result;
char stime[32];

ltime = time(NULL);
localtime_r(&ltime, &result);
asctime_r(&result, stime);
    printf("%s",stime );
/*
 // calendar time
    time_t ltime;
 // get current cal time
    ltime=time(NULL);
    printf("%s",asctime( localtime(&ltime) ) );
*/
}

int main()
{
timestamp_caca();
}
