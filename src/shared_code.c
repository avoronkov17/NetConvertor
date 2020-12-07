#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#include <arpa/inet.h>

#include "shared_code.h"


int errorRet(int code, char* errorStr, const char *format, ...)
{
    va_list ptr;
    va_start(ptr, format);
    /* сохранение сообщения */
    vsprintf(errorStr, format, ptr);
    va_end(ptr);
    return code;
}

void  Printer_print(int dbgLevel, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    if (dbgLevel <= LOG_ERR)
        printf("%s [%s] ", RED_COL, "NetListener");
    else if (dbgLevel <= LOG_WARNING)
        printf("%s [%s] ", YELLOW_COL, "NetListener");
    else if (dbgLevel <= LOG_INFO)
        printf("%s [%s] ", CYAN_COL, "NetListener");
    else
        printf("%s [%s] ", ORANGE_COL, "NetListener [debug]");
    vprintf(format, args);
    printf("%s\n", RESET_COL);
    fflush(stdout);
    va_end(args);

}

void  Printer_error(const char *format, ...)
{
    va_list args;
    va_start(args, format);
    printf("%s [%s] ", RED_COL, "NetListener");
    vprintf(format, args);
    printf("%s %s\n", strerror(errno), RESET_COL);
    fflush(stdout);
    va_end(args);

}
