#include <QCoreApplication>
#include <QThread>
#include <QFileInfo>
#include "shared_code.h"
#include "Settings.h"
#include "netconvertor.h"
#include <signal.h>

int state = 0;

const QString confFile = "settings.ini";

void stop_work (int sigNum)
{
    Printer_print(LOG_DEBUG, "Stopped by signal '%s'", strsignal(sigNum));
    state = 0;
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    struct sigaction sa;
    sa.sa_handler = stop_work;
    sigemptyset(&sa.sa_mask);
    sigaddset(&sa.sa_mask, SIGHUP);
    sa.sa_flags = 0;
    if ( sigaction(SIGTERM, &sa, NULL) < 0)
    {
        Printer_print (LOG_ERR, "невозможно перехватить сигнал SIGTERM: %s", strerror(errno));
        return -1;
    }

    if ( sigaction(SIGINT, &sa, NULL) < 0)
    {
        Printer_print (LOG_ERR, "невозможно перехватить сигнал SIGINT: %s", strerror(errno));
        return -1;
    }

    NetConvertor convertor;
    QFileInfo checkFile(confFile);
    if (! checkFile.isFile())
    {
        Printer_print (LOG_ERR, "Не найден файл настроек '%s'", confFile.toStdString().c_str());
        return -1;
    }

    if ( convertor.init(confFile) == -1 )
    {
        Printer_print (LOG_ERR, "ControllerNet '%s'",
                       convertor.errMess().toStdString().c_str());
        return -1;
    }

    state = 1;
    while (state) // здесь спим до прерывания программы
    {
        QCoreApplication::processEvents();
        QThread::msleep(10);
    }

    convertor.close();
    Printer_print (LOG_DEBUG, "Exit from program");
    a.exit();
    return 0;
}
