#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mensaje.h"


struct sockaddr_in destSock;
int sock;

#define tam sizeof(struct str_data)

struct str_data {
  uint8_t cod;
  uint64_t version;
  uint32_t ip;
  char id_usuario[16];
};

int mensaje(uint8_t cod, uint64_t version, uint32_t ip, uint32_t ipOrigen, char * id_usuario){

  char buffer[tam];

//data es un puntero al struct str_data.
  struct str_data *data;
  data = (struct str_data *) buffer;

//Asignación
  data->cod = cod;
  data->version = version;
  data->ip = ipOrigen;
  strcpy(data->id_usuario , id_usuario);

  int datalen = sizeof(buffer);

//Creación del socket
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock < 0){
    perror("error creando el socket");
    exit(1);
  }

//Asignación de datos al socket
  memset((char *) &destSock, 0, sizeof(destSock));
  destSock.sin_family = AF_INET;
  destSock.sin_addr.s_addr = ip;
  if (cod == 1 || cod == 3) {
    destSock.sin_port = htons(4322); //puerto donde la aplicación escucha mensajes multicast
  } else {
    destSock.sin_port = htons(4321); //puerto donde la aplicación escucha mensajes unicast
  }

//Envio de mensaje
  if(sendto(sock, data, datalen, 0, (struct sockaddr*)&destSock, sizeof(destSock)) < 0) {
    perror("error enviando el datagrama");
  }


  return 0;
}
