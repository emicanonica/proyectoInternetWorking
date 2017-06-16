#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "enviarArchivo.h"
#include "../gestTabla.h"

int enviarArchivo(uint32_t ip , char * nombreArchivo)
{
    char * direccion = malloc(1000);
    direccion = strcat(getConf(2), nombreArchivo);

    int sockfd = 0;
    int bytesReceived = 0;
    char recvBuff[1024]; //ver si se puede poner ilimitado
    memset(recvBuff, '0', sizeof(recvBuff));
    struct sockaddr_in serv_addr;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0))< 0)
    {
        printf("\n Error : Could not create socket \n");
        return 1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1235);
    serv_addr.sin_addr.s_addr = ip; //inet_addr("127.0.0.1"); //ip al que lo envio

    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
    {
        printf("\n Error : Connect Failed \n");
        return 1;
    }

    FILE *fp;
    fp = fopen( direccion, "wb+");
    if(NULL == fp)
    {
        printf("Error opening file");
        return 1;
    }

    while((bytesReceived = read(sockfd, recvBuff, sizeof(recvBuff))) > 0)
    {
        printf("Bytes received %d\n",bytesReceived);

        fwrite(recvBuff, 1,bytesReceived,fp);
        printf("%s \n", recvBuff);
        //FILE *pf;
        //pf = fopen(direccion, "wb");
        //fprintf(fp, "%s\n",recvBuff);
    }

    if(bytesReceived < 0)
    {
        printf("\n Read Error \n");
    }


    return 0;
}
