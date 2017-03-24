//Envio de datagrama al peer avisando que esta desactualizado
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mensaje.h"


struct in_addr localInterface;
struct sockaddr_in destSock;
int sock;


#define tam sizeof(struct str_data)

struct str_data {																//struct que se enviara
//	int id_mensage[16];
  char id_usuario[16];
//  int Long[8];
  uint8_t cod;
  uint64_t version;
  uint32_t ip;
//  int checksum[16];
};

int mensaje(uint8_t cod, uint64_t version, uint32_t ip, char * id_usuario){

  char buffer[tam];

  //data es un puntero al struct str_data.
  struct str_data *data;

  data->cod = cod;
  data->version = version;  //dd.mm.aa.hh.min
  data->ip = ip;            //ip al que voy a enviar, esto esta mal, aca debo poner MI IP .
  strcpy(data->id_usuario , id_usuario);
  //obtener mi ip, este voy a enviarlo, el otro es la direccion al que voy a enviar el mensaje


  int datalen = sizeof(buffer);


  /* Create a datagram socket on which to send. */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock < 0){
    perror("error creando el socket");
    exit(1);
  }
  else
    printf("creacion de socket --- OK.\n");


  memset((char *) &destSock, 0, sizeof(destSock));
  destSock.sin_family = AF_INET;
  destSock.sin_addr.s_addr = ip; //ACA DEBE IR "IP" EL IP A DONDE LO ESTOY ENVIANDO
  destSock.sin_port = htons(4322);

  //Envio de mensaje
  if(sendto(sock, data, datalen, 0, (struct sockaddr*)&destSock, sizeof(destSock)) < 0)
    {perror("error enviando el datagrama");}
  else
    printf("Envio de datagrama --- OK\n");


  return 0;
}
