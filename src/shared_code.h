#ifndef SHARED_CODE_H
#define SHARED_CODE_H

#include <stdio.h>
#include <stdint.h>
#include <stdarg.h>
#include <netinet/in.h>
#include <signal.h>

#define	LOG_EMERG	0	/* system is unusable */
#define	LOG_ALERT	1	/* action must be taken immediately */
#define	LOG_CRIT	2	/* critical conditions */
#define	LOG_ERR		3	/* error conditions */
#define	LOG_WARNING	4	/* warning conditions */
#define	LOG_NOTICE	5	/* normal but significant condition */
#define	LOG_INFO	6	/* informational */
#define	LOG_DEBUG	7	/* debug-level messages */


// для цветной печати
#define   BLACK_COL       "\033[1;30m"
#define   RED_COL         "\033[1;31m"
#define   GREEN_COL       "\033[1;32m"
#define   YELLOW_COL      "\033[1;33m"
#define   ORANGE_COL      "\033[2;33m"
#define   BLUE_COL        "\033[1;34m"
#define   MAGENTA_COL     "\033[1;35m"
#define   CYAN_COL        "\033[1;36m"
#define   WHITE_COL       "\033[1;37m"
#define   RESET_COL       "\033[1;0m"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief errorRet  Сохраняет текст ошибки и возвращает заданный код возврата.
 * @param code Код возврата, который нужно вернуть
 * @param errorStr Указатель на строку для сохранения ошибки
 * @param format Строка формата printf
 * @return Всегда возвращает заданный код возврата.
 */
int errorRet(int code, char* errorStr, const char *format, ...);

/**
  * @brief Аналог printf. Печатает текст в зависимости от уровня отладки.
  * @param При каком уровне отладки печатать.
  * @param Строка формата printf для печати.
  */
void  Printer_print(int dbgLevel, const char *format, ...);

/**
 * @brief Printer_error Аналог функции perror. Печатает текст ошибки и выводит текст переменной errno.
 * @param format Строка формата printf для печати.
 */
void  Printer_error(const char *format, ...);

#ifdef __cplusplus
}
#endif

#endif // SHARED_CODE_H
