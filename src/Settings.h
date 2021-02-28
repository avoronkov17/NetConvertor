#ifndef SETTINGS_H
#define SETTINGS_H
#include <QTypeInfo>
#include <memory>
#include <QString>
#include <QVariant>

quint64 getTimeInSeconds(const QString time) noexcept;

class Settings
{
public:
    static std::shared_ptr<Settings> instance()
    {
        if (m_instance == nullptr)
            m_instance = std::shared_ptr<Settings>{new Settings()};
        return m_instance;
    }
    ~Settings () {}
    bool isExist(QString fname);
    static QVariant option(QString fname, QString section, QString optName, QVariant defOpt);
    static bool optionBool(QString fname, QString section, QString optName, QVariant defOpt);
    static QString optionStr(QString fname, QString section, QString optName, QVariant defOpt);
    static qint32 optionSInt32(QString fname, QString section, QString optName, QVariant defOpt);
    static quint32 optionUInt32(QString fname, QString section, QString optName, QVariant defOpt);
    static qint64 optionSInt64(QString fname, QString section, QString optName, QVariant defOpt);
    static quint64 optionUInt64(QString fname, QString section, QString optName, QVariant defOpt);
    static double optionDouble(QString fname, QString section, QString optName, QVariant defOpt);
    static int setOptionStr(QString fname, QString section, QString optName, QVariant val);
    static int setOptionUInt32(QString fname, QString section, QString optName, QVariant val);
    static int setOptionBool(QString fname, QString section, QString optName, QVariant val);

private:
    Settings () {}
    Settings (const Settings&) = delete;
    Settings& operator= (Settings&) = delete;
    static std::shared_ptr<Settings> m_instance;
    static int m_setOption(QString fname, QString section, QString optName, QVariant value);
    };

#endif // SETTINGS_H
