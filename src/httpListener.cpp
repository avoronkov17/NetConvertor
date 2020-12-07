#include <QDebug>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDateTime>

#include "httpListener.h"


HttpListener::HttpListener(QObject *parent) : QObject(parent)
{
    m_http = new QNetworkAccessManager(this);
}

int HttpListener::init(QString url) noexcept
{
    m_URL = url;
    connect(m_http, &QNetworkAccessManager::finished,
            this, &HttpListener::replyFinished);

    connect(&m_timer, &QTimer::timeout, this, &HttpListener::m_timeout );
    m_timer.start(5000);
    return 0;
}

QNetworkRequest HttpListener::createRequest() noexcept
{
    QUrl url(m_URL);
    QNetworkRequest request;
    request.setUrl(url);

    // Add the headers specifying their names and their values with the following method : void QNetworkRequest::setRawHeader(const QByteArray & headerName, const QByteArray & headerValue);
    request.setRawHeader("User-Agent", "Net Convertor  v0.1");
    request.setRawHeader("Content-Type", "application/json");
    return request;
}

QByteArray HttpListener::createPostData(st_parameters params) noexcept
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
                            "\",\"\nEq\":\"" + eq +
                            "\",\"\nreg\":\"" +  reg +
                            "\",\"\nI_A\":\"" +  i_a +
                            "\",\"I_B\":\"" +  i_b +
                            "\",\"I_C\":\"" +  i_c +
                            "\",\"V_A\":\"" +  v_a +
                            "\",\"V_B\":\"" +  v_b +
                            "\",\"V_C\":\"" +  v_c +
                            " \"} }";

    return jsonString;
}

/*
POST /consum HTTP/1.1
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
 sscanf_P(str, (const char *)F("%*s %d-%d-%d %d:%d:%d"), &y, &m, &d, &hh, &mm, &ss)
  */


int HttpListener::sendDataToServer(struct st_parameters params) noexcept
{
    //qWarning()<<"Sending POST to "<<m_URL;
    QByteArray jsonString = createPostData(params);
    QByteArray postDataSize = QByteArray::number(jsonString.size());

    QNetworkRequest request = createRequest();
    request.setRawHeader("Content-Length", postDataSize);
    QNetworkReply *reply = m_http->post(request, jsonString);

    int retCode = 0;
    m_timer.start();
    connect(reply, &QNetworkReply::finished, this, [this, reply, &retCode] ()
    {

        if ( reply->error() == QNetworkReply::NoError )
        {
            QJsonObject obj = m_parseReply(reply);
            qWarning()<<"Got some reply: "<< obj;
            retCode = 1;
        }
        else
        {
            qWarning()<<"Got reply error: "<< reply->errorString();
            retCode  = -1;
        }
    }
    );

    return retCode;
}

void HttpListener::newControllerData() noexcept
{

}

void HttpListener::m_timeout() noexcept
{
    qWarning()<<"No data from Controller";
}

QJsonObject HttpListener::m_parseReply(QNetworkReply *reply) noexcept
{
    QJsonObject jsonObj;
    QJsonDocument jsonDoc;
    QJsonParseError parseError;
    auto replyText = reply->readAll();
    jsonDoc = QJsonDocument::fromJson(replyText, &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
        qDebug() << replyText;
        qWarning() << "Json parse error: " << parseError.errorString();
    }
    else
    {
        if(jsonDoc.isObject())
            jsonObj  = jsonDoc.object();
        else if (jsonDoc.isArray())
            jsonObj["non_field_errors"] = jsonDoc.array();
    }
    return jsonObj;
}

void HttpListener::replyFinished() noexcept
{

}
