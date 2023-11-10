#include "mainwindow.h"
#include "server.h"
#include <QApplication>
#include <iostream>
using namespace std ;
#include <QDebug>
#include <QStandardItemModel>
int main(int argc, char *argv[])
{
    //QApplication a(argc, argv);

    Server* server=new Server();
    server->httpServerStart();

//    MainWindow w;
//    w.show();
//    return a.exec();
}
