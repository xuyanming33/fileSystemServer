#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStandardItemModel>
#include <string>
#include <string.h>
#include <stack>
#include <QDebug>
#include <iostream>
using namespace  std;
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


    QStandardItemModel *translateStringToModel(QString files)
    {
        const char* symbols=",()";
        string tem=files.toStdString();

        auto s=files.toStdString().c_str();

        char* Files=new char[tem.size()+1];
        strcpy(Files,tem.c_str());
        qDebug()<<files;


        char* item=nullptr;
        char* specialSymbool=nullptr;

        stack<QStandardItem*> itemStack;
        QStandardItemModel* model=new QStandardItemModel();
        QStandardItem *rootItem=model->invisibleRootItem();
        itemStack.push(rootItem);

        item=Files;
        specialSymbool=strpbrk(Files,symbols);
        while(specialSymbool)
        {
            QStandardItem* childItem=nullptr;
            switch (*specialSymbool)
            {
            case ',':
                *specialSymbool='\0';
                if(item!=specialSymbool)
                {
                    childItem=new QStandardItem(QString(item));
                    itemStack.top()->appendRow(childItem);
                    qDebug()<<"add children";
                }
//"xuyanming(a,d),c,b"
                item=specialSymbool+1;
                specialSymbool=strpbrk(item,symbols);
                break;
            case '(':
                *specialSymbool='\0';
//                if(!item)
//                    return nullptr;
                childItem=new QStandardItem(QString(item));
                itemStack.top()->appendRow(childItem);
                qDebug()<<"add children";
                itemStack.push(childItem);

                item=specialSymbool+1;
                specialSymbool=strpbrk(item,symbols);
                break;
            case ')':
                *specialSymbool='\0';
                if(item!=specialSymbool)
                {
                    childItem=new QStandardItem(QString(item));
                    itemStack.top()->appendRow(childItem);
                    qDebug()<<"add children";
                }
                itemStack.pop();
                item=specialSymbool+1;
                specialSymbool=strpbrk(item,symbols);
                break;
            default:
                return nullptr;

            }
        }
        if(item)
        {
            QStandardItem* childItem=new QStandardItem(QString(item));
            itemStack.top()->appendRow(childItem);
        }
        return model;
    }
private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
