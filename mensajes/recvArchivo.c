#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>

#include "recvArchivo.h"
#include "../gestTabla.h"

int recvArchivo(char * nombreArchivo)
{
    char * direccion = malloc(1000);
    direccion = strcat(getConf(2), nombreArchivo);
    printf("%s\n", direccion);

    int listenfd = 0;
    int connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    int numrv;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    printf("Socket retrieve success\n");

    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(1235);

    bind(listenfd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    if(listen(listenfd, 10) == -1)
    {
        printf("Failed to listen\n");
        return -1;
    }

    //while(1){ //poner condicion de salida


        connfd = accept(listenfd, (struct sockaddr*)NULL ,NULL);

        //printf("paso el listen\n");

        FILE *fp = fopen( direccion, "r+b");
        if(fp==NULL)
        {
            printf("File open error ");
            return 1;
        }

        while(!feof(fp))
        {
            //unsigned char * buff = malloc(1024);
            unsigned char buff[256]={0};
            //int nread = fread(buff,256,1,fp);
            fread(buff,256,1,fp);
            //printf("Bytes read %d \n", nread);
            printf("%s\n", buff );

            //provisorio
            printf("Sending \n");
            write(connfd, buff, sizeof(buff));
            if (feof(fp))
                printf("End of file\n");
            if (ferror(fp))
                printf("Error reading\n");

            /*
            if(nread > 0)
            {
                printf("Sending \n");
                write(connfd, buff, nread);
            }

            if (nread < 256)
            {
                if (feof(fp))
                    printf("End of file\n");
                if (ferror(fp))
                    printf("Error reading\n");
                break;
            }
            */

        }

        close(connfd);
        sleep(1);
    //}


    return 0;
}
