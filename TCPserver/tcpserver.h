#ifndef TCPSERVER_H
#define TCPSERVER_H

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QDataStream>
#include <QDebug>
#include <QDateTime>
#include <QTimer>
#include "structs.h"


#define BIND_PORT 12345
#define FULL_FREE_SERVER_SPACE 1000

class TCPserver : public QTcpServer
{
    Q_OBJECT
public:
    TCPserver(QObject *parent = nullptr, uint16_t bindPort = 12345);

private:
    QTcpServer* tcpServer;
    QTcpSocket* incSocket;
    QVector<QTcpSocket*> sockets;

    //Статистика сервера
    uint32_t freeSpace = FULL_FREE_SERVER_SPACE;
    QByteArray trnData;
    StatServer stat;
    QTime startTime;
    QTimer* timer;
    QTime workTime;

    //Очередь запросов
    QMap<uint16_t, ServiceHeader> queue;

    void ProcessingMessage(QDataStream &datastream, ServiceHeader incHeader, QTcpSocket *socket);

private slots:

    void ReadyRead( void );


};



#endif // TCPSERVER_H
