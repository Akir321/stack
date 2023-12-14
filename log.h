#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

time_t openTime = time(NULL);

FILE *fLogOpen(void);
void flogClose(void);

FILE *fLogOpen(void)
{
    FILE *l = fopen("logfile.log", "a");
    atexit(flogClose);
    if (!l) 
    {
        printf("couldn't open logfile\n");
        return NULL;
    }
    printf("logfile opened\n");

    fprintf(l, "---------------------------------------------------------------\n");
    fprintf(l, "hi, i'm logfile!\ni was created on ");
    fprintf(l, "%s\n", ctime(&openTime));
    return l;
}

FILE *Log = fLogOpen();
#define LOG(...) fprintf(Log, __VA_ARGS__);

void flogClose(void)
{
    fclose(Log);
    printf("logfile successfully closed\n");
}

#endif //LOG_H
