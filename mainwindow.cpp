#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QStandardItemModel>
#include "server.h"
#include <iostream>
using namespace std;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Server* server=new Server();
    server->httpServerStart();


    QString s=server->fileTreeToString(QString("xuyanming"));
    qDebug()<<s;

}

MainWindow::~MainWindow()
{
    delete ui;
}

