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

    char * direccion = malloc(300);
    direccion = strcat(getConf(2), nombreArchivo);

    int sd = 0;
    int conn = 0;
    struct sockaddr_in serv_addr;
    char buffer[256];
    int filedes;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(1235);

    sd = socket(AF_INET, SOCK_STREAM, 0);

    bind(sd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    listen(sd,10);

    conn = accept(sd, (struct sockaddr*)NULL ,NULL);

    FILE *f;
    f = fopen(direccion,"wb+");
    filedes = fileno(f);

    while (recv(conn, buffer, sizeof(buffer), 0) > 0 ) // es decir mientras del otro lado no hagan close()
    {
        printf("%s\n", buffer);
        //fprintf(f, "%s\n", buffer);
        write(filedes, buffer, strlen(buffer));
    }

    fclose(f);

    return 0;
}
