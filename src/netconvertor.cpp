#include "netconvertor.h"
#include "shared_code.h"
#include "Settings.h"

NetConvertor::NetConvertor(QObject *parent) : QObject(parent)
{
    m_nano = new UdpListener(this);
    m_http_server = new HttpListener(this);
}

qint32 NetConvertor::init(QString confFile) noexcept
{
    m_imitPlat = Settings::optionSInt32(confFile, "debug", "imit_plat", 0);

    m_dbgLvl = Settings::optionSInt32(confFile, "debug", "debug_level", 0);

    if ( m_nano->init(m_dbgLvl, Settings::optionStr(confFile, "main", "udp_addr", "0.0.0.0:3000"), m_imitPlat)  == -1)
    {
        m_error = m_nano->errMess();
        return -1;
    }
    if (m_http_server->init(m_dbgLvl, Settings::optionStr(confFile, "main", "http_addr", "127.0.0.1:3000"))  == -1)
    {
        m_error = m_http_server->errMess();
        return -1;
    }

    connect(m_nano, &UdpListener::getData, this, &NetConvertor::receiveUdp);
    connect(m_http_server, &HttpListener::getResponce, this, &NetConvertor::receiveResponce);
    return 0;
}

void NetConvertor::close() noexcept
{

}

void NetConvertor::receiveUdp(const char *data, qint32 dataSize)
{
    if (dataSize != sizeof(m_platData))
    {
        Printer_print(LOG_WARNING, "Wrong size: %d, must %llu", dataSize, sizeof (m_platData));
        return;
    }
    memcpy( static_cast<void*>(&m_platData), data, sizeof(m_platData));
    m_http_server->sendDataToServer(m_platData);
}

void NetConvertor::receiveResponce(QString resp) noexcept
{

}
