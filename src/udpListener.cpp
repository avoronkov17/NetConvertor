#include <QtNetwork>
#include "udpListener.h"

constexpr quint32 MAX_PORT = 49151;
constexpr quint32 MIN_PORT = 1024;

UdpListener::UdpListener(QObject *parent): QObject(parent)
{}

int UdpListener::init(QString myAddr) noexcept
{
    m_srcAddr = myAddr;
    myIp = m_ip (m_srcAddr);
    myPort = m_port (m_srcAddr);
    if (m_socket == nullptr)
        m_socket = new QUdpSocket(this);

    return m_init();
}

qint32 UdpListener::sendData(char *data, qint32 size, QString ip, quint16 port)
{
    if ( !m_testAddress(ip, port ))
        return -1;

    qint64 len = m_socket->writeDatagram(data, size ,
               QHostAddress(ip), port);
    if (len < 0)
    {
        m_error = "network error: " + m_socket->errorString();
        qWarning()<<m_error;
    }
    return len;
}

void UdpListener::receiveData()
{
    QHostAddress recvIp;
    quint16 recvPort;
    m_inBuf.resize(m_socket->pendingDatagramSize());
    qint64 lastInSize  = m_socket->readDatagram(m_inBuf.data(), m_inBuf.size(),
                         &recvIp, &recvPort);
    QString rcvAddr = recvIp.toString() + ":" + QString::number(recvPort);
    if ( lastInSize < 0 )
    {
        qWarning()<<"Error of receiving from' "<<rcvAddr<<"': "<<m_socket->errorString();
        return;
    }
    //printf("Got %lld bytes from %s\n", lastInSize, rcvAddr.toStdString().c_str() );
    emit getData (m_inBuf.data(), m_inBuf.size());

}

int UdpListener::m_init() noexcept
{
    if (!m_testAddress (m_srcAddr))
    {
        m_error = "wrong address: " + m_srcAddr;
        return -1;
    }
    //qDebug()<<"Ip "<< myIp <<"  Port "<< myPort;
    QHostAddress myHostAddr (myIp);
    if (!m_socket->bind(myHostAddr, myPort))
    {
        m_error = "Bind: " +
                QString::number(myPort) + ": " +m_socket->errorString();
        return -1;
    }
    m_Connector = connect(m_socket, &QUdpSocket::readyRead, this, &UdpListener::receiveData);
    if (!m_Connector)
    {
        m_error = "Error signal slot UDP";
        return -1;
    }
    return 0;
}

bool UdpListener::m_testAddress(QString addr) noexcept
{
    return m_testAddress(m_ip(addr), m_port(addr));
}

bool UdpListener::m_testAddress(QString ip, quint16 port) noexcept
{
    if ( ip == "") {
        m_error = "unsetted IP" ;
        return false;
    }

    QHostAddress testAddr(ip);
    m_error = "";
    if (QAbstractSocket::IPv4Protocol != testAddr.protocol ()) {
        m_error = "unexeptable IP: " + ip;
        return false;
    }
    if ((port <= MIN_PORT) || (port > MAX_PORT )) {
        m_error = "unexeptable port: " + QString::number(port);
        return false;
    }
    return true;
}

QString UdpListener::m_ip(QString Addr) noexcept
{
    if (!Addr.contains(":"))
    {
        qWarning()<<"cannot find ':' in addr string: "<<Addr;
        return "";
    }
    QString ip = Addr.split (":")[0];
    return ip;
}

quint16 UdpListener::m_port(QString Addr) noexcept
{
    if (!Addr.contains(":"))
    {
        qWarning()<<"cannot find ':' in addr string: "<<Addr;
        return 0;
    }
    quint32 t_port = Addr.split (":")[1].toUInt();
    if ((t_port > MAX_PORT ) || (t_port < MIN_PORT))
            t_port = 0;
    quint16 port = static_cast<quint16>(t_port);
    return port;
}
