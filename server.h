#ifndef SERVER_H
#define SERVER_H
#include <event2/event.h>
#include <event2/listener.h>
#include <event2/http.h>
#include <event2/keyvalq_struct.h>
#include <event2/buffer.h>
#include <event2/thread.h>

#include <string.h>
#ifndef _WIN32
#include <signal.h>
#endif
#include <QString>
#include <iostream>
#include <string>
#include <QDebug>
#include <QDir>
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
static const char*fileRootDir="/home/xuyanming33/qttest/fileSystemServer/fileSystem";
static const size_t BUFFERSIZE=4096*4*4*4;
using namespace std;

//文件目录树结构
//struct fileNote
//{
//    const char* fileOrDirName;
//    vector<fileNote*> chileDirsOrFlies;
//};



class Server
{
public:
    Server();
    int httpServerStart();
private:
    static const int FILESIZE=1024;
    //reactor base
    event_base * base;
    //http server
    evhttp *http;
public:
    static void http_cb(struct evhttp_request *request, void *arg);

    static void get_handle_http_cb(struct evhttp_request *request);

    static void post_handle_http_cb(struct evhttp_request *request);

    static void delete_handel_http_cb(struct evhttp_request *request);

    //返回文件树
    static const char* fileTreeToString(QString path)
    {
        QString files;
        QDir dir(QString(fileRootDir)+path);

        //列出子目录
        QStringList strDirList=dir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);

        //列出所有文件
        QStringList strFileList=dir.entryList(QDir::Files);

        if(strDirList.count()==0&&strFileList.count()==0)
            return "";

        for(int i=0;i<strDirList.count();i++)
        {
            files+=strDirList.at(i);
            files+="(";

            files+=fileTreeToString(path+"/"+strDirList.at(i));
            files+=")";
            if(i==(strDirList.count()-1)&&strFileList.count()==0)
                files+="";
            else
                files+=",";
        }

        for(int i=0;i<strFileList.count();i++)
        {
            files+=strFileList.at(i);
            if(i==strFileList.count()-1)
                files+="";
            else
                files+=",";
        }

        string tem=files.toStdString();
        char* Files=new char[tem.size()+1];
        strcpy(Files,tem.c_str());

        return Files;
    }
};




#endif // SERVER_H
