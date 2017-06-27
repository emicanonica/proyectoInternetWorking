#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mensajeMulticast.h"


struct sockaddr_in groupSock;
int sock;

#define tam sizeof(struct str_data)

struct str_data {
  char  id_usuario[16];
  uint8_t cod;
  uint64_t version;
  uint32_t ip;
};

int mensajeMulticast(uint8_t cod, uint64_t version, uint32_t ip, char * id_usuario){

  char buffer[tam];

//data es un puntero al struct str_data.
  struct str_data *data;
  data = (struct str_data *) buffer;

//Asignación
  data->cod = cod;
  data->version = version;
  data->ip = ip;
  strcpy(data->id_usuario , id_usuario);

  int datalen = sizeof(buffer);

//Creación del socket
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock < 0){
    perror("error creando el socket");
    exit(1);
  } else
      printf("creacion de socket --- OK\n");

//Inicializacion y adicion al grupo multicast.
  memset((char *) &groupSock, 0, sizeof(groupSock));
  groupSock.sin_family = AF_INET;
  groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
  groupSock.sin_port = htons(4321);

//Envio de mensaje
  if(sendto(sock, data, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
  {perror("error enviando el datagrama");}
  else
    printf("Envio de datagrama --- OK\n");


  return 0;
}
