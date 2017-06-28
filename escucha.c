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
char *d, *AppDir;
bool a;

struct str_data {
  char  id_usuario[16];
  uint8_t cod;
  uint64_t version;
  uint32_t ip;
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

LOOP:  while(1){

//Inicializacion de variables
  char *direccion = getConf(2);//"/home/emi/git/proyectoInternetWorking/mensajes/";
  char *ptr;
  char ch;
  unsigned long localVersion = strtol(getConf(3),&ptr,10);
  char * idUsuario = getConf(1);
  uint32_t localIp = inet_addr(getConf(4));

  d = getenv("HOME");
  AppDir = d;
  strcat(AppDir,"/.Nombre/");
  a = false;

  //deamon();

  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
  syslog (LOG_NOTICE, "arranco el log.");
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");

  struct str_data *data;
  data = (struct str_data *) buffer;
  datalen = sizeof(buffer);

//guarda la direccion de ip en el archivo .conf
  getIpAddr();

  //Creación del socket
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if(sock < 0){
    perror("error creando el socket");
    exit(1);
  } else
    printf("creacion de socket --- OK\n");

//Habilita SO_REUSEADDR para permitir a multiples instancias de esta aplicacion recibir copias de datagramas multicast

  int reuse = 1;
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *)&reuse, sizeof(reuse)) < 0) {
    perror("Setting SO_REUSEADDR error");
    close(sock);
    exit(1);
  } else
    printf("Setting SO_REUSEADDR...OK.\n");

//Asignación
  memset((char *) &localSock, 0, sizeof(localSock));
  localSock.sin_family = AF_INET;
  localSock.sin_port = htons(4321);
  localSock.sin_addr.s_addr = INADDR_ANY;

//BIND
  if(bind(sock, (struct sockaddr*)&localSock, sizeof(localSock))) {
    perror("Error realizanzo el Binding");
    close(sock);
    exit(1);
  } else
    printf("Binding socket --- OK\n");

//Adheción al grupo multicast
  group.imr_multiaddr.s_addr = inet_addr("226.1.1.1");

  if(setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&group, sizeof(group)) < 0){
    perror("Adding multicast group error");
    close(sock);
    exit(1);
  }else
    printf("Adding multicast group...OK.\n");

//Espera y lectura de los mensajes entrantes
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

//Omite los mensajes propios para no generar redundancia
      if (data->ip == localIp){
        goto LOOP;
      }

      syslog (LOG_NOTICE, "+++++++++++++++++++++++");
      syslog (LOG_NOTICE, "el cod es:%i, la version es:%ld, el id es:%s", data->cod, data->version, data->id_usuario );
      print_ip(data->ip);
      syslog (LOG_NOTICE, "+++++++++++++++++++++++");

      switch (data->cod) {

//Guardado del nuevo usuario y envio de mensaje en respuesta con mi nombre de usuario, el ip local y la version local
        case 1:
          //VRIFICAR QUE NO EXISTA Y GUARDAR USUARIO RESIBIDIO (SI EXISTE PERO TIENE UNA VERSION MAS ACTUAL ACTUALIZAR VERSION)!!!

//verificacion de la existencia del usuario en la tabla y/o actualización de datos
		        buscarusuario(data->id_usuario, data->version, data->ip);

//envio de mensaje en respuesta con los datos locales
            if (mensaje(2, localVersion, data->ip, localIp, idUsuario) < 0) { //2 es el cod para la respuesta al cod 1 del usuario
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
              syslog (LOG_NOTICE, "error en la funcion resp COD=2" );
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            }

            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            syslog (LOG_NOTICE, "sale del case 1" );
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");

//Limpieza de buffer
            memset(buffer, '\0', 1000);
            goto LOOP;

//Guardado de datos del mensaje respuesta en la tabla de usuarios
        case 2:
          //VERIFICAR Y GUARDAR USUARIO RESIBIDIO!!!

          buscarusuario(data->id_usuario, data->version, data->ip);

          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 2" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

//Limpieza de buffer
          memset(buffer, '\0', 1000);
          goto LOOP;

//Respuesta positiva o negativa a consulta de version
        case 3:

//La version local es más actual
          if (data->version < localVersion) {

          if (mensaje(4, localVersion, data->ip, localIp, idUsuario) < 0) {
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            syslog (LOG_NOTICE, "error en la funcion resp COD=3 afirmativa" );
            syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          }

//La version local esta desactualizada
          } else{
            if (mensaje(5, data->version, data->ip, localIp, idUsuario) < 0) {
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
              syslog (LOG_NOTICE, "error en la funcion resp COD=3 negativa" );
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            }
          }

          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "salio del case 3" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

//Limpieza de buffer
          memset(buffer, '\0', 1000);
          goto LOOP;

//Respuesta positiva: Actualización de números de version en la tabla
        case 4:
          //ACTUALIZAR LA TABLA!!
          printf("Se ha encontrado una version más actual del repositorio");
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 4" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

//Limpieza de buffer
          memset(buffer, '\0', 1000);
          goto LOOP;

//Respuesta negativa: responde al mensaje version y si el repositorio donde se desea realizar la copia esta vacio
        case 5:

          if (strcmp(data->id_usuario, "vacio") == 0) {
            printf("El repositorio de donde se desean copiar archivos se encuentra vacio\n Copia de archivos cancelada\n");
          } else {
            printf("Usted se encuentra en la version más actual\n");
          }
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 5" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

//Limpieza de buffer
          memset(buffer, '\0', 1000);
          goto LOOP;

//Solicitud de envio de archivos
        case 6:

//Busqueda de nombre de los archivos
//CAMBIAR "direccion" POR LA UBICACION DEL REPOSITORIO DE LA APLICACION /HOME/USUARIO/.NOMBRE
          if ((dir = opendir (AppDir)) != NULL) {
              while ((ent = readdir (dir)) != NULL) {
                if (strcmp(ent->d_name,".") == 0 || strcmp(ent->d_name,"..") == 0 || strcmp(ent->d_name,".conf") == 0){
                  //no hace nada
                }else{
                  printf ("%s\n", ent->d_name);

//Envio de mensaje con el nombre del archivo a ser enviado posteriormente
                  if (mensaje(7, localVersion, data->ip, localIp, ent->d_name) < 0) { //con cod=7 enviar el nombre del archivo en el idUsuario
                    syslog (LOG_NOTICE, "+++++++++++++++++++++++");
                    syslog (LOG_NOTICE, "error en la funcion resp COD=6" );
                    syslog (LOG_NOTICE, "+++++++++++++++++++++++");
                  }

                  sleep(1); //esto está para asegurarse de que primero se ejecute "recvArchivo" en el escucha del otro lado
                  a = true;

//Crea una conexión TCP y envia el contenido del archivo
                  enviarArchivo(data->ip, data->id_usuario);

                  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
                  syslog (LOG_NOTICE, "salio de enviarArchivo" );
                  syslog (LOG_NOTICE, "+++++++++++++++++++++++");

                }

              }
              closedir (dir);

          } else {
            if (mensaje(5, localVersion, data->ip, localIp, "vacio") < 0) {
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
              syslog (LOG_NOTICE, "error en el envio de mensaje de directorio vacio en case 6" );
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            }
          }

          if (a == true) {
            if (mensaje(5, localVersion, data->ip, localIp, "vacio") < 0) {
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
              syslog (LOG_NOTICE, "error en el envio de mensaje de directorio vacio en case 6" );
              syslog (LOG_NOTICE, "+++++++++++++++++++++++");
            }
          }
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "salio case 6" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

//Limpieza de buffer
          memset(buffer, '\0', 1000);
          goto LOOP;

//Obtención de nombre de archivo y guardado del contenido
        case 7: //recibir y copiar los archivos a mi directorio
          //COPIAR ARCHIVOS A MI REPOSITORIO

          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "llego al case 7" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

          recvArchivo(ent->d_name);

          syslog (LOG_NOTICE, "+++++++++++++++++++++++");
          syslog (LOG_NOTICE, "salio case 7" );
          syslog (LOG_NOTICE, "+++++++++++++++++++++++");

//Limpieza de buffer
          memset(buffer, '\0', 1000);
          goto LOOP;

      } //fin swtich
    } //fin else
  } //fin while

    return 0;
}
