#ifndef HTTP_LISTENER_H
#define HTTP_LISTENER_H

#include <QObject>
#include <QTimer>
#include "parameters_datatypes.h"

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

class HttpListener : public QObject
{
    Q_OBJECT
    signals:
        void getResponce(QString);
public:
    explicit HttpListener(QObject *parent = nullptr);
    int init(QString url) noexcept;
    QNetworkRequest createRequest() noexcept;

    QByteArray createPostData(struct  st_parameters controller) noexcept;
    int sendDataToServer(struct  st_parameters controller) noexcept;
    QString errMess() noexcept { return m_error; }

public slots:
    void newControllerData() noexcept;
private slots:
    void m_timeout() noexcept;
private:
    QJsonObject m_parseReply(QNetworkReply *reply) noexcept;
    void replyFinished() noexcept;
    QNetworkAccessManager *m_http = nullptr;
    QTimer m_timer;
    QString m_URL;
    QString m_error;
};

#endif // HTTP_LISTENER_H
