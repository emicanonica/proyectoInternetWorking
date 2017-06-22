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
    char * direccion = malloc(300);
    direccion = strcat(getConf(2), nombreArchivo);

    int sd = 0;
    struct sockaddr_in serv_addr;
    char buffer[256];
    int filedes;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1235);
    serv_addr.sin_addr.s_addr = ip;//inet_addr("127.0.0.1");

    sd = socket(AF_INET, SOCK_STREAM, 0);

    connect(sd,(struct sockaddr *)&serv_addr, sizeof(serv_addr));

    FILE *f;
    f = fopen(direccion, "rb");
    filedes = fileno(f);

    while( read(filedes, buffer, sizeof(buffer)) > 0 ) // es decir mientras no sea fin de archivo (read devuelve 0 cuando lee fin de archivo)
    {
        printf("%s\n",buffer);
        send(sd, buffer, strlen(buffer), 0);
    }

    close(sd); // Esto hace que del otro lado termine el while por que el read le devuelve 0

    return 0;

}
