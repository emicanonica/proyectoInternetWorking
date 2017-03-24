#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <syslog.h>
#include <stdbool.h>

#include "mensaje.h"
#include "mensajeMulticast.h"
#include "gestTabla.h"


#define tam sizeof(struct str_data)

struct sockaddr_in localSock;
struct ip_mreq group;
struct sockaddr_in sourceSock;
int sock;
int datalen;


struct str_data {
  //int id_mensage[16];
  char id_usuario[16];
  //int Long[8];
  uint8_t cod;
  uint64_t version;
  uint32_t ip;
  //int checksum[16];
};

char buffer[tam];



//FUNCTIONS
static void deamon(){
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

  openlog ("firstdaemon", LOG_PID, LOG_DAEMON);
}

void print_ip(int ip){
    unsigned char bytes[4];
    bytes[0] = (ip >> 24) & 0xFF;
    bytes[1] = (ip >> 16) & 0xFF;
    bytes[2] = (ip >> 8) & 0xFF;
    bytes[3] = ip & 0xFF;

    syslog(LOG_NOTICE, "el ip es:%d.%d.%d.%d\n", bytes[3], bytes[2], bytes[1], bytes[0]);
}

/*void agregarUsuario(char * id_usuario, uint64_t version, uint32_t ip){
  /*agrega un usuario al final de la tabla*//*
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
  syslog (LOG_NOTICE, "entro a la funcion");
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");

  FILE *pf;
  pf = fopen("tabla.txt", "a+");
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
  syslog (LOG_NOTICE, "siguio");
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");

  fprintf(pf, "%s %ld\n", id_usuario, version);

  fclose(pf);

  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
  syslog (LOG_NOTICE, "salio");
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");

}
*/


int main(int argc, char *argv[]){


  unsigned long localVersion = 1122556611; //obtener de tabla
  char * idUsuario = "guachin"; //obtener de tabla
  uint32_t localIp = inet_addr("192.168.0.17"); //obtener de tabla
  uint32_t IpDestino = inet_addr("192.168.0.17");

  //deamon();

  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
  syslog (LOG_NOTICE, "arranco el log.");
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");

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
  group.imr_interface.s_addr = inet_addr("127.0.0.1");


  if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0){
    perror("Adding multicast group error");
    close(sock);
    exit(1);
  }else
    printf("Adding multicast group...OK.\n");


  while (1) {

  //Leyendo desde el socket
    datalen = sizeof(buffer);
    if(read(sock, buffer, datalen) < 0){
      perror("error de lectura");
      syslog (LOG_NOTICE, "+++++++++++++++++++++++");
      syslog (LOG_NOTICE, "error de lectura" );
      syslog (LOG_NOTICE, "+++++++++++++++++++++++");
      close(sock);
      exit(1);
    }else{

      syslog (LOG_NOTICE, "+++++++++++++++++++++++");
      syslog (LOG_NOTICE, "el cod es:%i, la version es:%ld, el id es:%s", data->cod, data->version, data->id_usuario );
      syslog (LOG_NOTICE, "+++++++++++++++++++++++");
      print_ip(data->ip);
      syslog (LOG_NOTICE, "+++++++++++++++++++++++");
/*
      if (data->cod == 1) {
        //guardado del nuevo usuario y envio de mensaje con cod=2 con mi id_usuario, mi ip y mi version
          //GUARDAR USUARIO RESIBIDIO!!!
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 1" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          //agregarUsuario(data->id_usuario, data->version, data->ip);

          if (mensaje(2, localVersion, data->ip, idUsuario) < 0) { //2 es el cod para la respuesta al cod 1 del usuario
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            syslog (LOG_NOTICE, "error en la funcion resp COD=2" );
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          }
          continue;

      }else if (data->cod == 2) {
        //resibo id_usuario y guardo en tabla
          //GUARDAR USUARIO RESIBIDIO!!!
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 2" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          continue;
      }else if (data->cod == 3) {
        if (data->version < localVersion) {
        if (mensaje(4, data->version, data->ip, idUsuario) < 0) { //4 es el cod para la respuesta afirmativa
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "error en la funcion resp COD=3" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
        }

        }else{
          if (mensaje(5, data->version, data->ip, idUsuario) < 0) { //5 es el cod para la respuesta negativa
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            syslog (LOG_NOTICE, "error en la funcion resp COD=3" );
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          }
        }

        syslog (LOG_NOTICE, "+++++++++++++++++++++++");
        syslog (LOG_NOTICE, "llego al case 3" );
        syslog (LOG_NOTICE, "+++++++++++++++++++++++");
        continue;
      }else if (data->cod == 4) {
        //ACTUALIZAR LA TABLA
        syslog (LOG_NOTICE, "+++++++++++++++++++++++");
        syslog (LOG_NOTICE, "llego al case 4" );
        syslog (LOG_NOTICE, "+++++++++++++++++++++++");
        continue;
      }else if (data->cod == 6) {
        //solicitud de recibida de archivos, envio de mensaje con cod=7 y los archivos
        //ENVIAR LOS ARCHIVOS CON UN MENSAJE CON COD=7
        syslog (LOG_NOTICE, "+++++++++++++++++++++++");
        syslog (LOG_NOTICE, "llego al case 6" );
        syslog (LOG_NOTICE, "+++++++++++++++++++++++");
        continue;
      }else if (data->cod == 7) {
        //recibir y copiar los archivos a mi directorio
        //COPIAR ARCHIVOS A MI REPOSITORIO
        syslog (LOG_NOTICE, "+++++++++++++++++++++++");
        syslog (LOG_NOTICE, "llego al case 7" );
        syslog (LOG_NOTICE, "+++++++++++++++++++++++");
        continue;
      }else {
        syslog (LOG_NOTICE, "+++++++++++++++++++++++");
        syslog (LOG_NOTICE, "error: codigo no correcto" );
        syslog (LOG_NOTICE, "+++++++++++++++++++++++");
        continue;
      }
*/
      switch (data->cod) {
        case 1: //guardado del nuevo usuario y envio de mensaje con cod=2 con mi id_usuario, mi ip y mi version
          //GUARDAR USUARIO RESIBIDIO!!!

          agregarUsuario(data->id_usuario, data->version, data->ip);

          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 1" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          //agregarUsuario(data->id_usuario, data->version, data->ip);
          /*
          if (mensaje(2, localVersion, data->ip, idUsuario) < 0) { //2 es el cod para la respuesta al cod 1 del usuario
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            syslog (LOG_NOTICE, "error en la funcion resp COD=2" );
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          }*/

        case 2://resibo id_usuario y guardo en tabla
          //GUARDAR USUARIO RESIBIDIO!!!
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 2" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          break;
        case 3: //respuesta positiva o negativa a consulta de version version
          if (data->version < localVersion) {

          if (mensaje(4, localVersion, data->ip, idUsuario) < 0) { //4 es el cod para la respuesta afirmativa
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            syslog (LOG_NOTICE, "error en la funcion resp COD=3" );
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          }

          }else{
            if (mensaje(5, data->version, data->ip, idUsuario) < 0) { //5 es el cod para la respuesta negativa
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
              syslog (LOG_NOTICE, "error en la funcion resp COD=3" );
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            }
          }
          break;
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 3" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

        case 4:
          //ACTUALIZAR LA TABLA
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 4" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          break;
        case 6://solicitud de recibida de archivos, envio de mensaje con cod=7 y los archivos
          //ENVIAR LOS ARCHIVOS CON UN MENSAJE CON COD=7
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 6" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          break;
        case 7://recibir y copiar los archivos a mi directorio
          //COPIAR ARCHIVOS A MI REPOSITORIO
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 7" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          break;
      }
    }
  }

    return 0;
}
