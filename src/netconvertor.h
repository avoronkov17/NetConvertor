#ifndef NETCONVERTOT_H
#define NETCONVERTOT_H

#include <QObject>
#include "udpListener.h"
#include "httpListener.h"
#include "parameters_datatypes.h"

class NetConvertor : public QObject
{
    Q_OBJECT
public:
    explicit NetConvertor(QObject *parent = nullptr);
    virtual ~NetConvertor() {}
    qint32 init(QString confFile) noexcept;
    void close() noexcept;
    QString errMess() noexcept { return m_error; }
public slots:
    void receiveUdp(const char *data, qint32 dataSize);
private slots:
    void receiveResponce(QString resp) noexcept;

private:
    QString m_error;
    UdpListener *m_nano = nullptr;
    HttpListener *m_http_server = nullptr;
    struct  st_parameters m_platData;
    int m_imitPlat = 0; // имитация работы платы
    int m_dbgLvl = 0;
signals:

};

#endif // NETCONVERTOT_H
