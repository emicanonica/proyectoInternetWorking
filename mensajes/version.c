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
//  int id_usuario[16];
//  int Long[8];
  int cod[8];
  int version[32];
//  int checksum[16];
};

int main (int argc, char *argv[ ])
{
char buffer[tam];

//data es un puntero al struct str_data.
struct str_data *data;

data = (struct str_data *) buffer;
strcpy(data->cod , "3");
strcpy(data->version , "123456789");
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

return 0;
}
