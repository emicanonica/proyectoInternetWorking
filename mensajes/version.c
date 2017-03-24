/* Envio de datagrama por multicast para consultar version de nuestro repositorio */
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

struct in_addr localInterface;
struct sockaddr_in groupSock;
int sock;

#define tam sizeof(struct str_data)

struct str_data {																//struct que se enviara
//	int id_mensage[16];
  char id_usuario[16];
//  int Long[8];
  uint8_t cod;
  uint32_t version;
  uint32_t ip;
//  int checksum[16];
};

int main (int argc, char *argv[ ])
{
char buffer[tam];

//data es un puntero al struct str_data.
struct str_data *data;
data = (struct str_data *) buffer;


data->cod = 3;
data->version = 1122334455;  //dd.mm.aa.hh.min
data->ip = inet_addr("192.168.0.17");


int datalen = sizeof(buffer);


//Se crea el socket.
sock = socket(AF_INET, SOCK_DGRAM, 0);
if(sock < 0)
{
  perror("error creando el socket");
  exit(1);
}
else
  printf("creacion de socket --- OK\n");

//Inicializacion y adicion al grupo multicast.

memset((char *) &groupSock, 0, sizeof(groupSock));
groupSock.sin_family = AF_INET;
groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
groupSock.sin_port = htons(4321);


/* Set local interface for outbound multicast datagrams. The IP address specified must be associated with a local, multicast capable interface. */

localInterface.s_addr = inet_addr("127.0.0.1");
if(setsockopt(sock, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
{
  perror("error al setear la interfaz local");
  exit(1);
}
else
  printf("Interfaz local seteada --- OK\n");

/* Send a message to the multicast group specified by the groupSock sockaddr structure.*/

if(sendto(sock, data, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
{perror("error enviando el datagrama");}
else
  printf("Envio de datagrama --- OK\n");


struct sockaddr_in localSock;

sock = socket(AF_INET, SOCK_DGRAM, 0);
if(sock < 0)
{
  perror("error creando el socket");
  exit(1);
}
else
  printf("creacion de socket --- OK\n");

memset((char *) &localSock, 0, sizeof(localSock));
localSock.sin_family = AF_INET;
localSock.sin_port = htons(4322);
localSock.sin_addr.s_addr = INADDR_ANY;

if(bind(sock, (struct sockaddr*)&localSock, sizeof(localSock)))
{
perror("Error realizanzo el Binding");
close(sock);
exit(1);
}
else
printf("Binding socket --- OK\n");

//localSock.sin_addr.s_addr = inet_addr("127.0.0.1");

//RECIBIR ACK
if(read(sock, buffer, datalen) < 0){
  perror("error de lectura");
  close(sock);
  exit(1);
}else{
  if (data->cod == 4) {
    printf("El codido es del datagrama es: \"%i quiere decir que tengo una version vieja\"\n", data->cod);
  }
  else
    printf("El codido es del datagrama es: \"%i quiere decir que mi version es la mas actual\"\n", data->cod);

  printf("La version es: \"%i\"\n", data->version);
  printf("el id es: \"%s\"\n", data->id_usuario);
}

return 0;
}
