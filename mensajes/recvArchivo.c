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

//Inicialización de variables para la localización de los directorios
    char AppDir[200];
    int n;
    char part[30] = "/home/";
    char part2[30] = "/.rockup/";
    char * nombreusu = nombreusuario();
    strcpy(AppDir , part);
    strcat(AppDir,nombreusu);
    strcat(AppDir,part2);
    strcat(AppDir,nombreArchivo);

//Inicialización de variables para la transferencia de archivos
    int sd = 0;
    int conn = 0;
    struct sockaddr_in serv_addr;
    char buffer[256];
    int filedes;

    memset(buffer,'\0',sizeof(buffer));

//Creación del socket
    sd = socket(AF_INET, SOCK_STREAM, 0);

//Habilita SO_REUSEADDR para permitir a multiples instancias de esta aplicacion recibir copias de datagramas multicast
    int reuse = 1;
    if(setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
      perror("Setting SO_REUSEADDR error");
      close(sd);
      exit(1);
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(1235);

//Bind
    bind(sd, (struct sockaddr*)&serv_addr,sizeof(serv_addr));

//Espera que le contesten con un connect
    if (listen(sd,10) < 0) {
      perror("listen error");
    }

//Acepta la solicitud de conexión recivida
    conn = accept(sd, (struct sockaddr*)NULL ,NULL);

//Crea el archivo si no existe y lo abre en modo escritura
    FILE *f;
    f = fopen(AppDir,"wb+");

//Recibe los datos del archivo y los guarda en el recien creado
    filedes = fileno(f);
    while ( (n = recv(conn, buffer, sizeof(buffer), 0)) > 0 ) // es decir mientras del otro lado no hagan close()
    {
        write(filedes, buffer, n);
        //memset(buffer, '\0', sizeof(buffer));
    }

    close(sd);
    fclose(f);
    printf("%s completado\n", nombreArchivo );

    return 0;
}
