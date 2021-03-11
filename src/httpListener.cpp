#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDateTime>
#include <QCoreApplication>
#include <QUrlQuery>

#include "shared_code.h"
#include "httpListener.h"

/*
Пример  POST запроса: HTTP/1.1
Host: 192.168.7.126:3000
Content-Type: application/json

 {"consumData":
     { "dt":"2020-06-28T03:28:00",
       "Eq":"1",
       "Reg":"2",
       "I_A":"1200",
       "I_B":"1200",
       "I_C":"1000",
       "V_A":"1200",
       "V_B":"0",
       "V_C":"0"}}
  */

const QString TELEGRAM_ADDR = "https://api.telegram.org";

constexpr int WAIT_UDP_DAY = ( 10 * 1000); // в течение суток нет данных
//constexpr int WAIT_UDP_DAY = ( 24 * 60 * 60 * 1000); // в течение суток нет данных
constexpr int WAIT_UDP = (10 * 1000); // сколько ждать данных на udp порту (миллисекунд)

constexpr int WAIT_TELEGRAM_SERVER_SEC = 3;
//#define TOKEN "1528549269:AAHnB1Evgi1rL18YTK_9-OK1bo8Ffghkcqc"
//const QString BotURL = "https://web.telegram.org/#/im?p=@avoronkov_test_chanel";
//const QString CHANNEL_ADDR = "@avoronkov_test_chanel111";

HttpListener::HttpListener(QObject *parent) : QObject(parent)
{
    m_httpServer = new QNetworkAccessManager();
    m_httptelegram = new QNetworkAccessManager();
}

HttpListener::~HttpListener()
{
    if ( m_httpServer) {
        delete m_httpServer;
        m_httpServer = nullptr;
    }
}

int HttpListener::init(st_HttpSettings settings) noexcept
{
    m_settings = settings;
    if ( (m_settings.telegram_TOKEN != "") && (m_settings.telegram_ID != "") &&  (m_settings.telegram_waitTime > 0))
        m_isTelegramActive = true;

    if ( m_isTelegramActive)
    {
        QString noData = "Проверка связи с программой. \
                     Уведомления будут прилетать не чаще, чем раз в "
                     + QString::number( m_settings.telegram_waitTime ) + " секунд";
        if ( m_sendTextToBot( noData) == 1 )
        {
            m_telegram.setInterval(m_settings.telegram_waitTime * 1000);
            m_telegram.start();
        }
        else
            Printer_print(LOG_ERR, "Cannot send text to telegram: timeout" );
    }
    else
        Printer_print(LOG_WARNING, "Не заданы параметр(ы) telegram бота. Уведомления не будут отправляться!");

    Printer_print(LOG_INFO, "Http server URL: '%s'", settings.serverURL.toStdString().c_str() );
    connect(&m_waitRegistrator, &QTimer::timeout, this, &HttpListener::m_timeout );

    connect(&m_telegram, &QTimer::timeout, this, [=]()
    {
        QString noData = "Нет соединения с сервером " + QString::number( m_settings.telegram_waitTime) + " секунд";
        m_sendTextToBot(noData);
    } );

    m_waitRegistrator.start(WAIT_UDP);
    return 0;
}

QNetworkRequest HttpListener::m_createRequest() noexcept
{
    QUrl url(m_settings.serverURL);
    QNetworkRequest request;
    request.setUrl(url);
    request.setRawHeader("Content-Type", "application/json");
    return request;
}

QByteArray HttpListener::m_postDataRaw(st_parameters params) noexcept
{
    QDateTime time;
    time = QDateTime::currentDateTime();

    QByteArray timeStr;
    timeStr.append(time.toString(Qt::DateFormat::ISODate));

    QByteArray eq, reg;
    eq.append(QString::number(params.is_eq));
    reg.append(QString::number(params.is_reg));

    QByteArray i_a, i_b, i_c;
    i_a.append(QString::number(params.i_a));
    i_b.append(QString::number(params.i_b));
    i_c.append(QString::number(params.i_c));

    QByteArray v_a, v_b, v_c;
    v_a.append(QString::number(params.v_a));
    v_b.append(QString::number(params.v_b));
    v_c.append(QString::number(params.v_c));

    // Build your JSON string as usual
    QByteArray jsonString = "{\"consumData\": {\"dt\":\"" + timeStr +
                            "\",\"Eq\":\"" + eq +
                            "\",\"Reg\":\"" +  reg +
                            "\",\"I_A\":\"" +  i_a +
                            "\",\"I_B\":\"" +  i_b +
                            "\",\"I_C\":\"" +  i_c +
                            "\",\"V_A\":\"" +  v_a +
                            "\",\"V_B\":\"" +  v_b +
                            "\",\"V_C\":\"" +  v_c +
                            "\"} }";
    return jsonString;
}

QJsonDocument HttpListener::m_postDataJson(st_parameters params) noexcept
{
    QDateTime time;
    time = QDateTime::currentDateTime();

    QByteArray timeStr;
    timeStr.append(time.toString(Qt::DateFormat::ISODate));

    QJsonObject consum;
    QJsonObject obj;
    obj["dt"] =  QString(timeStr);
    obj["Eq"] =  QString::number(params.is_eq);
    obj["Reg"] = QString::number(params.is_reg);
    obj["I_A"] = QString::number(params.i_a);
    obj["I_B"] = QString::number(params.i_b);
    obj["I_C"] = QString::number(params.i_c);
    obj["V_A"] = QString::number(params.v_a);
    obj["V_B"] = QString::number(params.v_b);
    obj["V_C"] = QString::number(params.v_c);
    consum["consumData"] = obj;
    return QJsonDocument(consum);
}


int HttpListener::sendDataToServer(struct st_parameters params) noexcept
{
    QByteArray jsonString = m_postDataRaw(params);
    int status = 0;
    QNetworkRequest request = m_createRequest();
    QNetworkReply *reply = m_httpServer->post (request, jsonString);

    if (m_settings.dbgLevel >= LOG_INFO)
        Printer_print(LOG_INFO, "Sended POST Request:\n %s", jsonString.toStdString().c_str())  ;

    m_waitRegistrator.start(); // этого не должно тут быть!

    connect(reply, &QNetworkReply::finished, this, [this, reply, &status] ()
    {
        QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        status = status_code.toInt();

        if ( reply->error() == QNetworkReply::NoError )
        {
            qWarning()<<"Reply ["<<status << "] "<< m_parseReply(reply);
            m_telegram.start();
        }
        else
            qWarning()<<"Reply error ["<<status <<"] "<< reply->errorString();
    }
    );
    return status;
}


void HttpListener::m_timeout() noexcept
{
    if (m_settings.dbgLevel >= LOG_NOTICE)
        qWarning()<<"No data from Controller";
}

int HttpListener::m_sendTextToBot(QString text) noexcept
{
    int status = 0;
    if ( !m_isTelegramActive ) // Телеграм клиент не настроен
    {
        m_error = "Not active";
        return -1;
    }

    QNetworkRequest request;
    QUrl url(TELEGRAM_ADDR + "/bot" + m_settings.telegram_TOKEN + "/sendMessage");
    request.setUrl(url);
    request.setRawHeader("Content-Type", "application/json");

    QString requestString = "{\"chat_id\": \"" + m_settings.telegram_ID + "\", \"text\": \"" +
            text + "\", \"disable_notification\": true }";

    QNetworkReply *reply = m_httpServer->post (request, requestString.toUtf8());
    if (m_settings.dbgLevel >= LOG_INFO)
        Printer_print(LOG_INFO, "Sended POST Request to Telegram:\n %s", requestString.toStdString().c_str())  ;

    QEventLoop loop;
    connect(reply, &QNetworkReply::finished, this, [this, reply, &status] ()
    {
        QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        status = status_code.toInt();

        if ( reply->error() == QNetworkReply::NoError )
        {
            qWarning()<<"Reply ["<<status << "] "<< m_parseReply(reply);
            m_telegram.start();
            status = 1;
        }
        else
            qWarning()<<"Reply error ["<<status <<"] "<< reply->errorString();
    }
    );

    QTimer timer;
    timer.setSingleShot(true);
    connect( &timer, &QTimer::timeout, &loop, &QEventLoop::quit );
    timer.start(WAIT_TELEGRAM_SERVER_SEC * 1000);
    loop.exec();
    m_waitRegistrator.start();
    if(status == 1)
        return 0;
    else
    {
        m_error = "Server not answer";
        return -1;
    }
}

QByteArray HttpListener::m_parseReply(QNetworkReply *reply) noexcept
{
    QJsonObject jsonObj;
    QJsonDocument jsonDoc;
    QJsonParseError parseError;
    QByteArray replyText = reply->readAll();

    jsonDoc = QJsonDocument::fromJson(replyText, &parseError);
    if (jsonDoc.isEmpty())
    {
        return replyText;
    }

    if(parseError.error != QJsonParseError::NoError)
    {
        qWarning() << "Json parse error:\n" << parseError.errorString() << "offset: "<< parseError.offset;
        qDebug() <<"[Source Reply text START]\n"<< replyText <<"[\n Source Reply text END]";
    }
    else
    {
        if(jsonDoc.isObject())
            jsonObj  = jsonDoc.object();
        else if (jsonDoc.isArray())
            jsonObj["non_field_errors"] = jsonDoc.array();
    }

    return QJsonDocument(jsonObj).toJson();
}
