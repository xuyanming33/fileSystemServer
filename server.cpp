#include "server.h"
Server::Server()
{

}

int Server::httpServerStart()
{
    #ifdef _WIN32
        //初始化socket库
        WSADATA wsa;
        WSAStartup(MAKEWORD(2, 2), &wsa);
    #else
        //忽略SIGPIPE信号
        if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
            return 1;
    #endif
        evthread_use_pthreads();

        qDebug() << "test server!\n";
        //创建libevent的上下文
        base = event_base_new();
        if (base)
            qDebug() << "event_base_new success!\n";

        http = evhttp_new(base);

        //2  绑定端口和IP
        struct evhttp_bound_socket* handle;
        handle = evhttp_bind_socket_with_handle(http, "127.0.0.1", 8080);
        if (!handle) {
            qDebug() << "evhttp_bind_socket failed!\n" ;
        }
        //3   设定回调函数
        evhttp_set_gencb(http, http_cb, 0);
        //evhttp_set_bevcb();
        //evhttp_set_cb();
        if (base)
            event_base_dispatch(base);
        if (base)
            event_base_free(base);
        if (http)
            evhttp_free(http);

        #ifdef _WIN32
                WSACleanup();
        #endif
        return 0;

}

void Server::http_cb(evhttp_request *request, void *arg)
{

    //cmd
        string cmdtype;
        switch (evhttp_request_get_command(request))
        {
        case EVHTTP_REQ_GET:
            cmdtype = "GET";
            get_handle_http_cb(request);
            break;
        case EVHTTP_REQ_POST:
            cmdtype = "POST";
            post_handle_http_cb(request);
            break;
        case EVHTTP_REQ_DELETE:
            cmdtype = "DELETE";
            delete_handel_http_cb(request);
            break;
        }
        //cout << "cmdtype:" << cmdtype << endl;
        //cout<<"----------------------------------------------------"<<endl;
        //cout<<" "<<endl;
}


void Server::get_handle_http_cb(evhttp_request *request)
{
    //uri
        const char* uri = evhttp_request_get_uri(request);
        std::cout << "uri:" << uri << std::endl;

    //header
        evkeyvalq *headers = evhttp_request_get_input_headers(request);
        //cout << "====== headers ======" << endl;
        for (evkeyval *p = headers->tqh_first; p != NULL; p = p->next.tqe_next)
        {
            //cout << p->key << ":" << p->value << endl;
        }
    //文件请求
        if(evhttp_find_header(headers,"FileOrDir")==string("File"))
        {
        //打开文件
            string filename=fileRootDir+string(uri);
            cout<<"filename:"<<filename.c_str()<<endl;
            int filefd=open(filename.c_str(),O_RDONLY);
            cout<<"filefd:"<<filefd<<endl;
            struct stat statbuf;
            stat(filename.c_str(), &statbuf);
            size_t filesize = statbuf.st_size;
            cout<<"filesize:"<<filesize<<endl;


            size_t bytes=0;
            size_t offset=0;
            size_t len=0;
        //应答报文的首部行、缓冲区
            evkeyvalq *outhead = evhttp_request_get_output_headers(request);
            evbuffer *outbuf = evhttp_request_get_output_buffer(request);

        //应答第一次请求
            if(!evhttp_find_header(headers,"Bytes"))
            {
                offset=0;
                len=min(BUFFERSIZE,filesize);
                cout<<"len:"<<len<<endl;
                evbuffer_add_file(outbuf,filefd,offset,len);

                bytes=evbuffer_get_length(outbuf);
                cout<<"bytes to buffer:"<<bytes<<endl;
            //小文件
                if(bytes==filesize)
                {
                    evhttp_send_reply(request, HTTP_OK, "OK", outbuf);
                    cout<<"had respond!"<<endl;
                    //close(filefd);
                }
            //大文件
                else
                {
                    string value=std::to_string(filesize)+string(":")+std::to_string(bytes);
                    evhttp_add_header(outhead, "Bytes",value.c_str());
                    evhttp_send_reply(request, HTTP_OK, "OK", outbuf);
                    cout<<"had respond "<<value<<"!"<<endl;
                    //close(filefd);
                }
            }
        //应答后续请求
            else
            {
            //偏移量
                const char* offsetstr=evhttp_find_header(headers,"Bytes");
                sscanf(offsetstr,"%zu",&offset);
                cout<<"bytes_had_get:"<<offset<<endl;
            //文件写入缓冲区
                len=min(BUFFERSIZE,(filesize-offset));
                cout<<"len:"<<len<<endl;
                evbuffer_add_file(outbuf,filefd,offset,len);
                bytes=evbuffer_get_length(outbuf);
                cout<<"bytes to buffer:"<<bytes<<endl;
            //首部信息（进度）
                string value=std::to_string(filesize)+string(":")+std::to_string(offset+bytes);
                evhttp_add_header(outhead, "Bytes",value.c_str());
                evhttp_send_reply(request, HTTP_OK, "OK", outbuf);
                cout<<"had respond "<<value<<"!"<<endl;
                //close(filefd);
            }
        }


    //目录请求
        else if(evhttp_find_header(headers,"FileOrDir")==string("Dir"))
        {
            //cout<<"process Dir!"<<endl;
            QString path=QString(uri);

            const char* fileSystem=fileTreeToString(path);
            //cout<<"fileSystem:"<<fileSystem<<endl;
            evbuffer *outbuf = evhttp_request_get_output_buffer(request);
            evbuffer_add_printf(outbuf,fileSystem);
            evhttp_send_reply(request, HTTP_OK, "OK", outbuf);
        }
        else
            cout<<"headers wrong"<<endl;
}

void Server::post_handle_http_cb(evhttp_request *request)
{
    //uri
        const char* uri = evhttp_request_get_uri(request);
        std::cout << "uri:" << uri << std::endl;

    //header
        evkeyvalq *headers = evhttp_request_get_input_headers(request);
        //cout << "====== headers ======" << endl;
        for (evkeyval *p = headers->tqh_first; p != NULL; p = p->next.tqe_next)
        {
            //cout << p->key << ":" << p->value << endl;
        }
    //inbuf
        evbuffer *inbuf=evhttp_request_get_input_buffer(request);
        evkeyvalq *outhead = evhttp_request_get_output_headers(request);
    //处理文件
        if(evhttp_find_header(headers,"FileOrDir")==string("File"))
        {
        //创建文件
            string tem=fileRootDir+string(uri);
            cout<<"tem:"<<tem<<endl;
            int getfilefd=open(tem.c_str(),O_WRONLY|O_CREAT|O_APPEND,0600);

            cout<<"getfilefd:"<<getfilefd<<endl;
        //写入文件
            evbuffer_write(inbuf,getfilefd);

            const char* recivesize=evhttp_find_header(headers,"Content-Length");


//            struct stat statbuf;
//            stat(tem.c_str(), &statbuf);
//            long long filesize = (statbuf.st_size);
//            QString qstrfileszie=QString("%1").arg(filesize);



        //回传报文
            evhttp_add_header(outhead, "Bytes",recivesize);
            evhttp_send_reply(request, HTTP_OK, "OK", 0);
            //cout<<"had respond!"<<endl;
            close(getfilefd);
        }
    //处理目录
        else if(evhttp_find_header(headers,"FileOrDir")==string("Dir"))
        {
        //创建目录
            string tem=fileRootDir+string(uri);
            cout<<"tem:"<<tem<<endl;
            int dirfd=mkdir(tem.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            cout<<"dirfd: "<<dirfd<<endl;
        //回送报文
            evhttp_send_reply(request, HTTP_OK, "OK", 0);
            cout<<"had respond!"<<endl;
        }
}

void Server::delete_handel_http_cb(evhttp_request *request)
{
    //uri
        const char* uri = evhttp_request_get_uri(request);
        std::cout << "uri:" << uri << std::endl;

    //header
        evkeyvalq *headers = evhttp_request_get_input_headers(request);
        //cout << "====== headers ======" << endl;
        for (evkeyval *p = headers->tqh_first; p != NULL; p = p->next.tqe_next)
        {
            //cout << p->key << ":" << p->value << endl;
        }

        string path=string(fileRootDir)+uri;
        //cout<<"delete: "<<path<<endl;
    //删除文件
        if(evhttp_find_header(headers,"FileOrDir")==string("File"))
        {
            if(remove(path.c_str())==0)
                cout<<"remove sucess!"<<endl;
            else
                cout<<"remove failed!"<<endl;

        }

    //删除目录
        else if(evhttp_find_header(headers,"FileOrDir")==string("Dir"))
        {
            if(QDir(path.c_str()).removeRecursively()==true)
                cout<<"remove sucess!"<<endl;
            else
                cout<<"remove failed!"<<endl;

        }
    //回送报文
        evhttp_send_reply(request, HTTP_OK, "OK", 0);
        cout<<"had respond!"<<endl;
}
