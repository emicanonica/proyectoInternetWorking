#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#define tam sizeof(struct str_data)

struct sockaddr_in localSock;
struct ip_mreq group;
struct sockaddr_in sourceSock;
int sock;
int datalen;


struct str_data {
  //int id_mensage[16];
  //int id_usuario[16];
  //int Long[8];
  int cod[8];
  int version[32];
  //int checksum[16];
};

char buffer[tam];

int main(int argc, char *argv[])
{

pid_t process_id = 0;
pid_t sid = 0;

//crea el proceso hijo
process_id = fork();

// Indication of fork() failure
if (process_id < 0)
{
printf("fork failed!\n");
// Return failure in exit status
exit(1);
}
// PARENT PROCESS. Need to kill it.
if (process_id > 0)
{
printf("process_id of child process %d \n", process_id);
// return success in exit status
exit(0);
}

//unmask the file mode
umask(0);
//set new session
sid = setsid();
if(sid < 0)
{
// Return failure
exit(1);
}

// Change the current working directory to root.
chdir("/");

// Close stdin. stdout and stderr
close(STDIN_FILENO);
close(STDOUT_FILENO);
close(STDERR_FILENO);



//inicio del programa
struct str_data *data;
data = (struct str_data *) buffer;

//Se crea el socket en el que se va a recibir la informacion.
sock = socket(AF_INET, SOCK_DGRAM, 0);
if(sock < 0)
{
perror("error creando el socket");
exit(1);
}
else
printf("creacion de socket --- OK\n");

/* Enable SO_REUSEADDR to allow multiple instances of this application to receive copies of the multicast datagrams. */
{
int reuse = 1;
if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
{
perror("Setting SO_REUSEADDR error");
close(sock);
exit(1);
}
else
printf("Setting SO_REUSEADDR...OK.\n");
}

memset((char *) &localSock, 0, sizeof(localSock));
localSock.sin_family = AF_INET;
localSock.sin_port = htons(4321);
localSock.sin_addr.s_addr = INADDR_ANY;

if(bind(sock, (struct sockaddr*)&localSock, sizeof(localSock)))
{
perror("Error realizanzo el Binding");
close(sock);
exit(1);
}
else
printf("Binding socket --- OK\n");

//Se une al grupo de multicast.

group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");
group.imr_interface.s_addr = inet_addr("127.0.0.1");  //Esta es la dirección IP de la computadora que estoy usando, dentro de mi red local.

sourceSock.sin_addr.s_addr = inet_addr("127.0.0.1");  //Este funciona para recivir monocast.

if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0){
  perror("Adding multicast group error");
  close(sock);
  exit(1);
}else
  printf("Adding multicast group...OK.\n");

while (1){
//Leyendo desde el socket
  datalen = sizeof(buffer);
  if(read(sock, buffer, datalen) < 0){
    perror("error de lectura");
    close(sock);
    exit(1);
  }else{
    /*printf("Leyendo datagrama --- OK\n");
    printf("------------------------\n");
    printf("El codido es del datagrama es: \"%s\"\n", data->cod);
    printf("La version es: \"%s\"\n", data->version);
    */

    memset((char *) &sourceSock, 0, sizeof(sourceSock));
    sourceSock.sin_family = AF_INET;
    sourceSock.sin_addr.s_addr = inet_addr("127.0.0.1");
    sourceSock.sin_port = htons(4322);

    if(sendto(sock, data, datalen, 0, (struct sockaddr*)&sourceSock, sizeof(sourceSock)) < 0)
    {perror("error enviando el datagrama");}
    else
      printf("Envio de datagrama --- OK\n");

  }
}


return 0;
}
