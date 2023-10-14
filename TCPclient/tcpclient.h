#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>
#include <QDataStream>
#include <QDateTime>
#include <QHostAddress>
#include <QMessageBox>
#include "structs.h"

class TCPclient : public QObject
{
    Q_OBJECT
public:
    explicit TCPclient(QObject *parent = nullptr);
    void SendRequest(ServiceHeader head);
    void SendData(ServiceHeader head, QString data);
    void ConnectToHost(QHostAddress host, uint16_t port);
    void DisconnectFromHost(void);



private slots:

	void ReadyRead(void);
    void ProcessingData(ServiceHeader header, QDataStream &stream);

private:

QTcpSocket* socket;
ServiceHeader servHeader;

signals:
    void sig_sendFreeSize(uint32_t);
    void sig_sendStat(StatServer);
    void sig_sendTime(QDateTime);
    void sig_SendReplyForSetData(QString);
    void sig_Error(uint16_t);
    void sig_connectStatus(uint16_t);
    void sig_Disconnected( void );
    void sig_Success(uint16_t);

};

#endif // TCPCLIENT_H
