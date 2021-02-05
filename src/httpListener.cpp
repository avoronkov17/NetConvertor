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

constexpr int WAIT_UDP = 5000; // сколько ждать данных на udp порту

HttpListener::HttpListener(QObject *parent) : QObject(parent)
{
}

HttpListener::~HttpListener()
{
    if ( m_http)
        delete m_http;
}

int HttpListener::init(int debugLevel, QString url) noexcept
{
    m_dbgLvl = debugLevel;
    if (m_http == nullptr)
        m_http = new QNetworkAccessManager();
    m_URL = url;
    //connect(m_http, &QNetworkAccessManager::finished,
    //        this, &HttpListener::m_replyFinished);

    connect(&m_timer, &QTimer::timeout, this, &HttpListener::m_timeout );
    m_timer.start(WAIT_UDP);
    return 0;
}

QNetworkRequest HttpListener::m_createRequest() noexcept
{
    QUrl url(m_URL);
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
                            "\",\"reg\":\"" +  reg +
                            "\",\"I_A\":\"" +  i_a +
                            "\",\"I_B\":\"" +  i_b +
                            "\",\"I_C\":\"" +  i_c +
                            "\",\"V_A\":\"" +  v_a +
                            "\",\"V_B\":\"" +  v_b +
                            "\",\"V_C\":\"" +  v_c +
                            " \"} }";
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
    obj["reg"] = QString::number(params.is_reg);
    obj["I_A"] = QString::number(params.i_a);
    obj["I_B"] = QString::number(params.i_b);
    obj["I_C"] = QString::number(params.i_c);
    obj["V_A"] = QString::number(params.v_a);
    obj["V_B"] = QString::number(params.v_b);
    obj["V_C"] = QString::number(params.v_c);

    consum["consumData"] = obj;
    return QJsonDocument(consum);

    /*   QJsonObject recordObject;
       QJsonObject consumObj;
       consumObj.insert("dt", "2021-02-01T12:46:16");
       consumObj.insert("is_eq", "2");
       consumObj.insert("reg", "5");
       consumObj.insert("I_A", "11");
       consumObj.insert("I_B", "12");
       consumObj.insert("I_C", "13");
       consumObj.insert("V_A", "23");
       consumObj.insert("V_B", "23");
       consumObj.insert("V_C", "23");

       recordObject.insert("consumData", consumObj);
       return recordObject;*/
}


int HttpListener::sendDataToServer(struct st_parameters params) noexcept
{
    QByteArray jsonString = m_postDataRaw(params);
    int status = 0;
    QNetworkRequest request = m_createRequest();
    QNetworkReply *reply = m_http->post (request, jsonString);

    if (m_dbgLvl >= LOG_INFO)
        qDebug()<<"String START]"<<jsonString << " [END String]";

    m_timer.start();
    connect(reply, &QNetworkReply::finished, this, [this, reply, &status] ()
    {
        QVariant status_code = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
        status = status_code.toInt();

        if ( reply->error() == QNetworkReply::NoError )
            qWarning()<<"Reply ["<<status << "] "<< m_parseReply(reply);
        else
            qWarning()<<"Reply error ["<<status <<"] "<< reply->errorString();
    }
    );
    return status;
}

void HttpListener::m_timeout() noexcept
{
    if (m_dbgLvl >= LOG_NOTICE)
        qWarning()<<"No data from Controller";
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

/*void HttpListener::m_replyFinished() noexcept
{
    qDebug()<< "========================";
}*/
