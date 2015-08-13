/*
 * =====================================================================================
 *
 *       Filename:  Client.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  2015年08月05日 08时36分16秒
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:   (wangweihao), 578867817@qq.com
 *        Company:  xiyoulinuxgroup
 *
 * =====================================================================================
 */

#include "Client.h"
#include <string.h>

/* 初始化监控本地负载的client */

static int loadServer_fd;

/* 定时器处理定时事件，发送给负载server自己的负载信息 */
void 
signalHandler(int sig)
{
    char buffer[1024] = "hello world\n";
    switch(sig)
    {
        case SIGALRM:
            printf("caught the SIGALRM signal\n");
            printf("get local load info\n");
            /* send loadserver */
            send(loadServer_fd, buffer, 1024, 0);
            break;
    }
}

ServerClient::
ServerClient(std::string i, int p):
    ip(i), port(p)
{
    /* 初始化socket */
    bzero(&server, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port = htons(port);
    inet_pton(AF_INET, ip.c_str(), &server.sin_addr);

    loadServer_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(loadServer_fd < 0)
    {
        perror("create loadServer socket fd error\n");
        exit(1);
    }
    int con_ret = connect(loadServer_fd, (struct sockaddr*)&server, sizeof(server));
    if(con_ret == -1)
    {
        perror("load Serverconnect error\n");
        exit(1);
    }

    std::thread tid([&](){
            /* 设置定时器，并初始化 */
            signal(SIGALRM, signalHandler);
            new_value.it_value.tv_sec = 0;
            new_value.it_value.tv_usec = 1;
            new_value.it_interval.tv_sec = 10;
            new_value.it_interval.tv_usec = 0;
            setitimer(ITIMER_REAL, &new_value, &old_value);
            while(1);
        }
    );
    tid.detach();
}

