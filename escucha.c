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
#include <dirent.h>

#include "mensajes/mensaje.h"
#include "mensajes/mensajeMulticast.h"
#include "gestTabla.h"
#include "mensajes/recvArchivo.h"
#include "mensajes/enviarArchivo.h"


#define tam sizeof(struct str_data)

struct sockaddr_in localSock;
struct ip_mreq group;
struct sockaddr_in sourceSock;
int sock;
int datalen;

//para el envio de archivos
DIR *dir;
struct dirent *ent;


struct str_data {
  //int id_mensage[16];
  char  id_usuario[16];
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

int main(int argc, char *argv[]){

  char *direccion = getConf(2);//"/home/emi/git/proyectoInternetWorking/mensajes/";

  char *ptr;
  unsigned long localVersion = strtol(getConf(3),&ptr,10); //obtener de tabla
  char * idUsuario = getConf(1); //obtener de tabla
  //uint32_t localIp = inet_addr("192.168.0.17"); //obtener de tabla
  uint32_t localIp = inet_addr(getConf(4)); //obtener de tabla
  uint32_t IpDestino = inet_addr("192.168.0.12"); //provisorio

  //deamon();

  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
  syslog (LOG_NOTICE, "arranco el log.");
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");

LOOP:  while(1){

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

  int reuse = 1;
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0)
  {
    perror("Setting SO_REUSEADDR error");
    close(sock);
    exit(1);
  }
  else
    printf("Setting SO_REUSEADDR...OK.\n");



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

  if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0){
    perror("Adding multicast group error");
    close(sock);
    exit(1);
  }else
    printf("Adding multicast group...OK.\n");


    char ch;
    //Leyendo desde el socket

    datalen = sizeof(buffer);



    syslog (LOG_NOTICE, "+++++++++++++++++++++++");
    syslog (LOG_NOTICE, "entro al while" );
    syslog (LOG_NOTICE, "+++++++++++++++++++++++");

    if(read(sock, buffer, datalen) < 0){
      perror("error de lectura");
      syslog (LOG_NOTICE, "+++++++++++++++++++++++");
      syslog (LOG_NOTICE, "error de lectura" );
      syslog (LOG_NOTICE, "+++++++++++++++++++++++");
      close(sock);
      exit(1);
    }else{

      syslog (LOG_NOTICE, "+++++++++++++++++++++++");
      syslog (LOG_NOTICE, "esta leyendo" );
      syslog (LOG_NOTICE, "+++++++++++++++++++++++");

      //para hacer que no lea el mensaje de si mismo. COMENTAR PARA QUE SE LEA SUS PROPIOS MENSAJES
      if (data->ip == localIp){
        goto LOOP;
      }

      syslog (LOG_NOTICE, "+++++++++++++++++++++++");
      syslog (LOG_NOTICE, "el cod es:%i, la version es:%ld, el id es:%s", data->cod, data->version, data->id_usuario );
      syslog (LOG_NOTICE, "+++++++++++++++++++++++");
      print_ip(data->ip);
      syslog (LOG_NOTICE, "+++++++++++++++++++++++");

      switch (data->cod) {
        case 1: //guardado del nuevo usuario y envio de mensaje con cod=2 con mi id_usuario, mi ip y mi version
          //VRIFICAR QUE NO EXISTA Y GUARDAR USUARIO RESIBIDIO (SI EXISTE PERO TIENE UNA VERSION MAS ACTUAL ACTUALIZAR VERSION)!!!
          //if (data->ip != localIp) {

            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            syslog (LOG_NOTICE, "llego al case 1" );
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");

            //agregarUsuario(data->id_usuario, data->version, data->ip);
		        buscarusuario(data->id_usuario, data->version, data->ip);

            if (mensaje(2, localVersion, data->ip, localIp, idUsuario) < 0) { //2 es el cod para la respuesta al cod 1 del usuario
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
              syslog (LOG_NOTICE, "error en la funcion resp COD=2" );
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            }

          //}
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            syslog (LOG_NOTICE, "sale del case 1" );
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");

            //VACIA BUFFER
            //while ((ch = getchar()) != '\n' && ch != EOF) { }
            //memset(buffer, '\0', 1000);
            goto LOOP;
            //break;
        case 2://resibo id_usuario y guardo en tabla
          //VERIFICAR Y GUARDAR USUARIO RESIBIDIO!!!
          //agregarUsuario(data->id_usuario, data->version, data->ip);

          buscarusuario(data->id_usuario, data->version, data->ip);

          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 2" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          //VACIA BUFFER
          //while ((ch = getchar()) != '\n' && ch != EOF) { }
          memset(buffer, '\0', 1000);
          goto LOOP;
          //break;
        case 3: //respuesta positiva o negativa a consulta de version version
          if (data->version < localVersion) {

          if (mensaje(4, localVersion, data->ip, localIp, idUsuario) < 0) { //4 es el cod para la respuesta afirmativa
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            syslog (LOG_NOTICE, "error en la funcion resp COD=3 afirmativa" );
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          }

          }else{
            if (mensaje(5, data->version, data->ip, localIp, idUsuario) < 0) { //5 es el cod para la respuesta negativa
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
              syslog (LOG_NOTICE, "error en la funcion resp COD=3 negativa" );
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            }

          }

          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "salio del case 3" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          //VACIA BUFFER
          //while ((ch = getchar()) != '\n' && ch != EOF) { }
          memset(buffer, '\0', 1000);
          goto LOOP;
          //break;
        case 4:
          //ACTUALIZAR LA TABLA
          printf("La version que posee actualmente se encuentra desactualizada, para actualizarla corra el comando ./cliente actualizar\n");
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 4" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          //VACIA BUFFER
          //while ((ch = getchar()) != '\n' && ch != EOF) { }
          memset(buffer, '\0', 1000);
          goto LOOP;
          //break;
        case 5:
          //SI ES RESPUESTA A 3 ENTONCES NO HAGO NADA Y IMPRIMO QUE EL USUARIO ESTA ACTUALIZADO
          printf("Usted se encuentra en la version más actual\n");
          //SI ES RESPUESTA A 6 IMPRIMIR MENSAJE DE ERROR DE ENVIO DE ARCHIVOS(POR AHORA SOLO FALLA EN ESO);
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 5" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          //VACIA BUFFER
          //while ((ch = getchar()) != '\n' && ch != EOF) { }
          memset(buffer, '\0', 1000);
          goto LOOP;
          //break;
        case 6://solicitud de recibida de archivos, envio de mensaje con cod=7 y los archivos
          //ENVIAR LOS ARCHIVOS CON UN MENSAJE CON COD=7

          if ((dir = opendir (direccion)) != NULL) {
              while ((ent = readdir (dir)) != NULL) {
                if (strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0){
                  //no hago nada
                }else{

                  printf ("%s\n", ent->d_name);
                  printf("%ld\n", localVersion);
                  print_ip(localIp);
                  print_ip(data->ip);
                  if (mensaje(7, localVersion, data->ip, localIp, ent->d_name) < 0) { //con cod=7 enviar el nombre del archivo en el idUsuario
                    syslog (LOG_NOTICE, "+++++++++++++++++++++++");
                    syslog (LOG_NOTICE, "error en la funcion resp COD=6" );
                    syslog (LOG_NOTICE, "+++++++++++++++++++++++");
                  }
                  sleep(1);
                  enviarArchivo(data->ip, data->id_usuario);


                  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
                  syslog (LOG_NOTICE, "salio de recvArchivo" );
                  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
                }

                syslog (LOG_NOTICE, "+++++++++++++++++++++++");
                syslog (LOG_NOTICE, "salio de while" );
                syslog (LOG_NOTICE, "+++++++++++++++++++++++");

              }
              closedir (dir);

          }else {
            perror ("");
          }
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "salio case 6" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          //VACIA BUFFER
          //while ((ch = getchar()) != '\n' && ch != EOF) { }
          memset(buffer, '\0', 1000);
          goto LOOP;
          //break;

        case 7: //recibir y copiar los archivos a mi directorio
          //COPIAR ARCHIVOS A MI REPOSITORIO
          //¿QUE PASA SI ESTA VACIO?

          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 7" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          //enviarArchivo(data->ip, data->id_usuario); //el nombre del archivo va guardado en id_usuario

          recvArchivo(ent->d_name);

          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "salio case 7" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          //VACIA BUFFER
          //while ((ch = getchar()) != '\n' && ch != EOF) { }
          memset(buffer, '\0', 1000);
          goto LOOP;
          //break;
      } //fin swtich
    } //fin else
  } //fin while

    return 0;
}
