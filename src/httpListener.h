#ifndef HTTP_LISTENER_H
#define HTTP_LISTENER_H

#include <QObject>
#include <QTimer>
#include "parameters_datatypes.h"
//#include "qttelegrambot.h"

class QNetworkAccessManager;
class QNetworkRequest;
class QNetworkReply;

/**
 * @brief The st_HttpSettings struct параметры работы http модуля
 */
struct st_HttpSettings
{
  qint32 dbgLevel = 0; // Уровень отладки
  QString serverURL = "";  // Адрес сервера с базой

  QString telegram_TOKEN = ""; // Имя телеграм бота
  QString telegram_ID = ""; // Имя телеграм канала
  quint64 telegram_waitTime;
};

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
     * @param debugLevel Структура с параметрами работы.
     * @return Возвращает 0 при успешной инициализации и -1 в случае ошибки.
     */
    int init(st_HttpSettings settings) noexcept;

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
    int m_sendTextToBot(QString text) noexcept;
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

    QNetworkRequest m_createRequest() noexcept;
    QByteArray m_parseReply(QNetworkReply *reply) noexcept;

    QNetworkAccessManager *m_httpServer = nullptr;
    QNetworkAccessManager *m_httptelegram = nullptr;
    //Telegram::Bot *m_bot = nullptr;
    QTimer m_waitRegistrator;
    QTimer m_telegram;
    st_HttpSettings m_settings;
    QString m_error;
    bool m_isTelegramActive = false;
};

#endif // HTTP_LISTENER_H
