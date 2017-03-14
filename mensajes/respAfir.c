//Envio de datagrama al peer avisando que esta desactualizado
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>

struct in_addr localInterface;
struct sockaddr_in destSock;
int sock;

//#define ADDR "192.168.0.18";

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
strcpy(data->cod , "4");
strcpy(data->version , "123456789");
int datalen = sizeof(buffer);
/* Create a datagram socket on which to send. */
sock = socket(AF_INET, SOCK_DGRAM, 0);
if(sock < 0)
{
  perror("error creando el socket");
  exit(1);
}
else
  printf("creacion de socket --- OK.\n");

memset((char *) &destSock, 0, sizeof(destSock));
destSock.sin_family = AF_INET;
destSock.sin_addr.s_addr = inet_addr("127.0.0.1");
destSock.sin_port = htons(4321);

//Envio de mensaje
if(sendto(sock, data, datalen, 0, (struct sockaddr*)&destSock, sizeof(destSock)) < 0)
{perror("error enviando el datagrama");}
else
  printf("Envio de datagrama --- OK\n");

  //bloque ack
    struct sockaddr_in localSock;

      localSock.sin_addr.s_addr = inet_addr("127.0.0.1");

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

      //prueba de recibir ack
      if(read(sock, buffer, datalen) < 0){
        perror("error de lectura");
        close(sock);
        exit(1);
      }else{
        //printf("Leyendo datagrama --- OK\n");
        printf("------------------------\n");
        printf("El codido es del datagrama es: \"%s\"\n", data->cod);
        printf("La version es: \"%s\"\n", data->version);
      }


return 0;
}
