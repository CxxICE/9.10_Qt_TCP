#include <QCoreApplication>
#include "tcpserver.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    TCPserver server(nullptr, BIND_PORT);

    return a.exec();
}
