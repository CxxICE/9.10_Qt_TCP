#include "tcpserver.h"

/*!
 * @brief Перегрузка оператора для чтения служебного заголовка
 */
QDataStream &operator >>(QDataStream &out, ServiceHeader &data){

    out >> data.id;
    out >> data.idData;
    out >> data.status;
    out >> data.len;

    return out;
};

QDataStream &operator <<(QDataStream &in, StatServer &data){

    in << data.incBytes;
    in << data.sendBytes;
    in << data.revPck;
    in << data.sendPck;
    in << data.workTime;
    in << data.clients;

    return in;
};

QDataStream &operator <<(QDataStream &in, ServiceHeader &data){

    in << data.id;
    in << data.idData;
    in << data.status;
    in << data.len;

    return in;
};

/*!
 * \brief Конструктор класса запускает сервер
 * \param bindPort - порт который сервер слушаеьт
 */
TCPserver::TCPserver(QObject *parent, uint16_t bindPort)
{

    tcpServer = new QTcpServer(this);

    if(tcpServer->listen(QHostAddress::Any, bindPort)){
        qDebug() << "Server started on port: " << BIND_PORT;
        startTime = QTime::currentTime();
    }
    else{
        qDebug() << "Error start server!";
    }


    //Таймер необходим для статистики. Сохраняет время работы сервера в секундах.
    timer = new QTimer(this);
    timer->start(1000);
    connect(timer, &QTimer::timeout, this, [&]{

        stat.workTime++;
        timer->start(1000);

    });

    //Обработчик нового соединения
    connect(tcpServer, &QTcpServer::newConnection, this, [&]{

        stat.revPck++;

        incSocket = tcpServer->nextPendingConnection();

        connect(incSocket, &QTcpSocket::readyRead, this, &TCPserver::ReadyRead);
        connect(incSocket, &QTcpSocket::disconnected, incSocket, [&]{

            qDebug() << "Socket " <<  incSocket->socketDescriptor() << " disconnected!";
            incSocket->deleteLater();
            sockets.remove(sockets.indexOf(incSocket));

        });

        sockets.push_back(incSocket);

        qDebug() << "Socket " <<  incSocket->socketDescriptor() << " connected!";

    });
}

/*!
 * \brief Метод обрабатывает принятое сообщение
 * \param datastream - в случае если переданы данные используем указатель на поток
 * \param incHeader - заголовок сообщения
 * \param socket - указатель сокет которому нужно ответить
 */
void TCPserver::ProcessingMessage(QDataStream &datastream, ServiceHeader incHeader, QTcpSocket *socket)
{
    trnData.clear();

    QDataStream out(&trnData, QIODevice::WriteOnly);

    ServiceHeader header;
    header.id = ID;
    header.status = STATUS_SUCCES;

    switch (incHeader.idData) {

        case GET_SIZE:{

            header.idData = GET_SIZE;
            header.len = sizeof(freeSpace);
            out << header;
            out << freeSpace;
            break;

        }
        case GET_TIME:{

            header.idData = GET_TIME;
            header.len = sizeof(QDateTime);
            out << header;
            out << QDateTime::currentDateTime();

            break;
        }
        case GET_STAT:{

            header.idData = GET_STAT;
            header.len = sizeof(StatServer);
            out << header;
            stat.clients = sockets.size();
            out << stat;
            break;
        }
        case SET_DATA:{

            header.idData = SET_DATA;

            if(freeSpace < incHeader.len){
                QString str;
                datastream >> str;
                header.status = ERR_NO_FREE_SPACE;
                header.len = 0;
                out << header;
            }
            else if(incHeader.len == 0){

                header.status = ERR_ZERO_LEN;
                header.len = 0;
                out << header;

            }
            else{
                //Читаем строку.
                QString str;
                datastream >> str;
                freeSpace -= str.toUtf8().size();
                header.len += str.toUtf8().size() + sizeof(int);
                out << header;
                out << str;
            }
            break;
        }
    case CLEAR_SPACE:{

        header.idData = CLEAR_SPACE;
        header.len = 0;
        freeSpace = FULL_FREE_SERVER_SPACE;
        out << header;

        break;
    }
        default:
            return;
    }

    socket->write(trnData);
    stat.sendPck++;
    stat.sendBytes+=sizeof(ServiceHeader) + header.len;
}

/*!
 * \brief Метод обрабатывает входящее подключение
 */
void TCPserver::ReadyRead( )
{

    stat.revPck++;

    incSocket = (QTcpSocket*)sender();
    QDataStream incStream(incSocket);

    if(incStream.status() == QDataStream::Ok){
        qDebug() << "Incoming data from client " << incSocket->socketDescriptor();
    }
    else{
        qDebug() << "Error open stream for client " << incSocket->socketDescriptor();
    }

    ServiceHeader header;

    while(1){


		//Проверяем есть ли не считанные данные для этого заголовка
        if(queue.find(incSocket->socketDescriptor()) != queue.end()){

            header = queue.value(incSocket->socketDescriptor( ));

        }
        else{
            //Если данных доступно меньше заголовка, то выходим из функции.
            //Поскольку еще не все сообщение принято
            if(incSocket->bytesAvailable() < sizeof(ServiceHeader)){
                return;
            }
            else{

                //Если пришло достаточно данных для чтения заголовка, читаем его
                incStream >> header;
                stat.incBytes += sizeof(ServiceHeader);

                //Если считаны неверные данные(принимаем решение по правильности заголовка)
                if(header.id != ID){
                    //Отбрасываем все данные до обнаружения следующего загаловка
                    uint16_t hdr = 0;
                    while(incStream.atEnd() == false){

                        incStream >> hdr;
                        stat.incBytes += sizeof(uint16_t);
                        if(hdr == ID){

                            header.id = hdr;
                            incStream >> header.idData;
                            incStream >> header.status;
                            incStream >> header.len;
                            //Добавляем в статистику размер считанных байт, за исключением идентификатора, т.к. мы его считали ранее
                            stat.incBytes += sizeof(ServiceHeader) - sizeof (uint16_t);
                            break;
                        }
                    }
                }
            }
        }

        //Если не все байты доступны, то ставим данный хеадер на считывание и выходим из обработчика
        if(incSocket->bytesAvailable() < header.len){
            //И если этого дискриптора нет в очереди на обработку, то добавляем его в очередь
            if(queue.find(incSocket->socketDescriptor()) == queue.end()){
                queue.insert(incSocket->socketDescriptor(), header);
            }
            return;
        }
        //В противном случае обрабатываем сообщение
        else{
            ProcessingMessage(incStream, header, incSocket);
        }
    }
}



