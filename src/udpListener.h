#ifndef UDP_LISTENER_H
#define UDP_LISTENER_H

#include <QUdpSocket>
#include <QObject>
#include <QHostAddress>

class UdpListener: public QObject
{
    Q_OBJECT
signals:
    void getData(const char* data, qint32 size);

public:
    explicit UdpListener(QObject *parent = nullptr);
    ~UdpListener() {};
    int init(QString myAddr) noexcept;
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
    QUdpSocket *m_socket = nullptr;
    QString m_error;
    QString m_srcAddr;
    QString myIp;
    quint16 myPort;
    QMetaObject::Connection m_Connector;
};

#endif // LISTENCONTROLLER_H
