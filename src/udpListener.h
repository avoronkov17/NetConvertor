#ifndef UDP_LISTENER_H
#define UDP_LISTENER_H

#include <QUdpSocket>
#include <QObject>
#include <QHostAddress>
#include <QTimer>

class UdpListener: public QObject
{
    Q_OBJECT
signals:
    void getData(const char* data, qint32 size);

public:
    explicit UdpListener(QObject *parent = nullptr);
    ~UdpListener() {};
    int init(int debugLevel, QString myAddr, qint16 nano_port, int imitTime) noexcept;
    QString errMess() noexcept { return m_error; }

public slots:
    qint32 sendData(char* data, qint32 size, QString ip, quint16 port);
    void receiveData();

private:
    int m_init() noexcept;
    bool m_testAddress(QString addr) noexcept;
    bool m_testAddress(QString ip, quint16 port) noexcept;
    static QString m_ip(QString Addr) noexcept;
    static quint16 m_port(QString Addr) noexcept;

    QByteArray m_inBuf;
    QTimer m_imitTimer;
    QUdpSocket *m_socket = nullptr;
    QString m_error;
    QString m_srcAddr;
    QString m_myIp;
    quint16 m_myPort;
    qint16 m_nanoPort;
    int m_imitTime = 0;
    int m_dbgLvl = 0;
    QMetaObject::Connection m_Connector;
    QMetaObject::Connection m_UdpImitator;
};

#endif // LISTENCONTROLLER_H
