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
    ~HttpListener();

    /**
     * @brief init Инициализация модуля взаимодействия с http сервером.
     * @param debugLevel Уровень отладочной печати.
     * @param url url адрес сервера.
     * @return Возвращает 0 при успешной инициализации и -1 в случае ошибки.
     */
    int init(int debugLevel, QString url) noexcept;

    /**
     * @brief sendDataToServer Отправляет параметры контроллера на http сервер.
     * @param controller Структура с параметрами контроллера.
     * @return Код ответа от сервера или 0, если сервер ничего не ответил
     */
    int sendDataToServer(struct  st_parameters controller) noexcept;

    /**
     * @brief errMess Возвращает текст последней ошибки
     * @return Текст последней ошибки
     */
    QString errMess() noexcept { return m_error; }

private slots:
    void m_timeout() noexcept;
private:

    /**
     * @brief m_postDataRaw Создаёт 'сырой' post запрос
     * @param params  - Параметры от микроконтроллера
     * @return массив QbyteArray
     */
    QByteArray m_postDataRaw(st_parameters params) noexcept;

    /**
     * @brief m_postDataJson Создаёт JsonObject post запрос
     * @param params - Параметры от микроконтроллера
     * @return
     */
    QJsonDocument m_postDataJson(st_parameters params) noexcept;
    //void m_replyFinished() noexcept;
    QNetworkRequest m_createRequest() noexcept;
    QByteArray m_parseReply(QNetworkReply *reply) noexcept;

    QNetworkAccessManager *m_http = nullptr;
    QTimer m_timer;
    QString m_URL;
    QString m_error;
    int m_dbgLvl = 0;
};

#endif // HTTP_LISTENER_H
