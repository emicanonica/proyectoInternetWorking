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
    //nombreArchivo = malloc(20);
    /*printf("%s\n", nombreArchivo);
    char * direccion = malloc(300);
    direccion = strcat(getConf(2), nombreArchivo);*/

    char AppDir[100];
    char part[30] = "/home/";
    char part2[30] = "/.NOMBRE/";
    char * nombreusu = nombreusuario();
    strcpy(AppDir , part);
    strcat(AppDir,nombreusu);
    strcat(AppDir,part2);
    strcat(AppDir,nombreArchivo);


    int sd = 0;
    int conn = 0;
    struct sockaddr_in serv_addr;
    char buffer[256];
    int filedes;

    memset(buffer,'\0',sizeof(buffer));

    sd = socket(AF_INET, SOCK_STREAM, 0);

    int reuse = 1;
    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
      perror("Setting SO_REUSEADDR error");
      close(sd);
      exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(1235);

    bind(sd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

    listen(sd,10);


    conn = accept(sd, (struct sockaddr*)NULL ,NULL);


    printf("%s\n", AppDir);

    FILE *f;
    f = fopen(AppDir,"wb+");

    filedes = fileno(f);
    while (recv(conn, buffer, sizeof(buffer), 0) > 0 ) // es decir mientras del otro lado no hagan close()
    {
        printf("%s\n", buffer);
        write(filedes, buffer, strlen(buffer));
    }

    fclose(f);
    close(sd);
    printf("fin recv\n" );

    return 0;
}
