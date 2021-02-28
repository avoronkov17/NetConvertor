#include "Settings.h"
#include <QString>
#include <QSettings>
#include <QFile>
#include <QVariant>
#include <QStringList>
#include <QString>
#include <QFile>

std::shared_ptr<Settings> Settings::m_instance;

/**
 * @brief getTimeInSeconds Парсит строку с временем и возвращает размер в секундах
 * @param time строка времени вида: 3_s , 44_M, 2_H
 * @return кол-во секунд или 0, в случае ошибки разбора.
 */
quint64 getTimeInSeconds(const QString time) noexcept
{
    QStringList sizeStrList = time.split ("_");
    quint64 seconds = static_cast<quint64>(sizeStrList[0].toInt());

    if (sizeStrList.length () < 2 ) {
        return 0;
    }
    if ((sizeStrList[1] =="s") || ((sizeStrList[1] == "S") ))
        return seconds;
    if ((sizeStrList[1] =="m") || ((sizeStrList[1] == "M") ))
        seconds *= 60;
    else if ((sizeStrList[1] =="h") || ((sizeStrList[1] == "H") ))
        seconds *= (60 * 60);
    else if ((sizeStrList[1] =="d") || ((sizeStrList[1] == "D") ))
        seconds *= (60 * 60 * 24);
    return seconds;
}

// смотрит, есть ли данный файл
bool Settings::isExist(QString fname)
{
    if (QFile::exists(fname))
        return true;
    return false;
}

/*
 * читает (1), ищет в нём секцию (2) и возвращает значение ключа (3) этой секции или (4) если
 * не удалось получить это значение
*/

QVariant Settings::option(QString fname, QString section, QString optName, QVariant defOpt)
{
    if ( !QFile(fname).exists() ) {
        return defOpt;
    }
    QSettings* settings = new QSettings(fname, QSettings:: IniFormat);
    if (!settings->childGroups().contains (section) ) {
        return defOpt;
    }
    settings->setIniCodec("UTF-8");
    settings->beginGroup( section );
    QVariant option = settings->value (optName, defOpt);
    settings->endGroup();
    delete settings;
    return option;
}

/* обёртки для Settings::option возврата */
bool Settings::optionBool(QString fname, QString section, QString optName, QVariant defOpt)
{
    return option(fname, section, optName, defOpt).toBool();
}

QString Settings::optionStr(QString fname, QString section, QString optName, QVariant defOpt)
{
    return option(fname, section, optName, defOpt).toString();
}

qint32 Settings::optionSInt32(QString fname, QString section, QString optName, QVariant defOpt)
{
    return option(fname, section, optName, defOpt).toInt();
}

quint32 Settings::optionUInt32(QString fname, QString section, QString optName, QVariant defOpt)
{
    return option(fname, section, optName, defOpt).toUInt();
}

qint64 Settings::optionSInt64(QString fname, QString section, QString optName, QVariant defOpt)
{
    return option(fname, section, optName, defOpt).toInt();
}

quint64 Settings::optionUInt64(QString fname, QString section, QString optName, QVariant defOpt)
{
    return option(fname, section, optName, defOpt).toULongLong();
}

double Settings::optionDouble(QString fname, QString section, QString optName, QVariant defOpt)
{
    return option(fname, section, optName, defOpt).toDouble();
}


// запись опций в файл
int Settings::setOptionStr(QString fname, QString section, QString optName, QVariant val)
{
    return m_setOption(fname, section, optName,  val.toString());
}

int Settings::setOptionUInt32(QString fname, QString section, QString optName, QVariant val)
{
    return m_setOption(fname, section, optName,  val.toUInt());
}

int Settings::setOptionBool(QString fname, QString section, QString optName, QVariant val)
{
    return m_setOption(fname, section, optName,  val.toBool());
}

/* записывает опцию в файл
 * Возвращает 0 если опция записалась в файл и -1 в случае ошибки
*/
int Settings::m_setOption(QString fname, QString section, QString optName, QVariant value)
{
    int isApply = -1;
    if ( !QFile(fname).exists() ) {
        return -1;
    }
    QSettings* settings = new QSettings(fname, QSettings:: IniFormat);
    if (!settings->childGroups().contains (section) ) {
        return -2;
    }
    settings->setIniCodec("UTF-8");
    settings->beginGroup( section );
    settings->setValue(optName,value);
    if ( !(settings->value(optName, value) == value))
        isApply = -3;
    else {
        isApply = 0; // опция записана
    }
    settings->endGroup();
    delete settings;
    return  isApply;
}
