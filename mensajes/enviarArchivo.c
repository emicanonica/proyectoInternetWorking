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

    //nombreArchivo = malloc(20);
    //char * direccion = malloc(300);
    //direccion = strcat(getConf(2), nombreArchivo);

    char AppDir[100];
    char part[30] = "/home/";
    char part2[30] = "/.NOMBRE/";
    char * nombreusu = nombreusuario();
    strcpy(AppDir , part);
    strcat(AppDir,nombreusu);
    strcat(AppDir,part2);
    strcat(AppDir,nombreArchivo);
    printf("%s\n", AppDir);

    int sd = 0;
    struct sockaddr_in serv_addr;
    char buffer[256];
    int filedes=0;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(1235);
    serv_addr.sin_addr.s_addr = ip;//inet_addr("127.0.0.1");

    //memset(buffer,0,sizeof(buffer));

    sd = socket(AF_INET, SOCK_STREAM, 0);


RETRY:    if (connect(sd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0){
      perror("connect failed error");
      goto RETRY;
      return 1;
    }

    //sleep(1);


    FILE *f;
    f = fopen(AppDir, "rb");
    //printf("%s\n", direccion);
    filedes = fileno(f);
    printf("aca\n");
    printf("%d\n", filedes);


    while( read(filedes, buffer, sizeof(buffer)) > 0 ) // es decir mientras no sea fin de archivo (read devuelve 0 cuando lee fin de archivo)
    {
        printf("%s\n",buffer);
        send(sd, buffer, strlen(buffer), 0);
    }

    close(sd); // Esto hace que del otro lado termine el while por que el read le devuelve 0
    printf("fin enviarArchivo\n" );

    return 0;

}
