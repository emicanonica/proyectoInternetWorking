#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "mensajes/mensaje.h"
#include "mensajes/mensajeMulticast.h"
#include "gestTabla.h"

char * setConf(int i){ //TOMA LOS VALORES EN .conf Y LOS PONES EN LAS VARIABLES
  FILE *pf;
  char *line = malloc (1000);
  int len1,len2,c,cont;
  cont = 0;
  pf = fopen(".conf", "r");
  fseek(pf, 0, SEEK_SET);
  for (;;) {
    c = fgetc(pf);
    if (c == EOF)
      break;

    if (c == ':'){
      cont++;
      if (i==cont)
        len1 = ftell(pf);
    }
    if (i == cont) {
      if (c == EOF || c == '\n') {
        len2 = ftell(pf);
        break;
      }
    }

  }
  fseek(pf, len1, SEEK_SET);
  fread(line, (len2-len1-1),1,pf);

  return line;
}

int main(int argc, char const *argv[]) {

  //FILE *pf;

  unsigned long localVersion = 114455661122; //obtener de tabla
  char * idUsuario = setConf(1);
  //printf("%s\n", idUsuario);
  char * direccion = setConf(2);
  //printf("%s\n", direccion);
  uint32_t IpDestino = inet_addr("192.168.0.22"); //obtener de tabla
  uint32_t localIp = inet_addr("192.168.0.17");   //se utiliza en multicast solamente
  //uint32_t MasActualIp = inet_addr("192.168.0.17"); //obtener de tabla !IMPORTANTE!

  //fclose(pf);

  char str1[20],str2[20];

  if (strcmp(argv[1], "hola") == 0) {
    if (mensajeMulticast(1, localVersion, localIp, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'hola'");
    }
  } else if (strcmp(argv[1], "version") == 0) {
    if (mensajeMulticast(3, localVersion, localIp, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'version'");
    }
  } else if (strcmp(argv[1], "conf") == 0) {
    FILE *pf;
    pf = fopen(".conf", "w+");
    printf("Ingrese su nombre de usuario: ");
    scanf("%s",str1);
    printf("Ingrese la direccion de la carpeta donde desea que se realice la copia de archivos: ");
    scanf("%s",str2);
    fprintf(pf, "id de usuario:%s\nUbicación del repositorio:%s\n", str1, str2);
    fclose(pf);
  }else if (strcmp(argv[1], "solicitud") == 0) { // enviar mensaje "version" y luego enviar mensaje con=6 al mas actualizado en la tabla
  /*  if (mensajeMulticast(3, localVersion, IpDestino, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'version'");
    }*/
    //BUSCAR LA VERSION MAS ACTUALIZADA EN LA TABLA, TOMAR LA IP Y USARLA PARA ENVIAR UN MENSAJE SOLICITUD
    if (mensaje(6, localVersion, IpDestino, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'solicitud'");
    }
  }else if (strcmp(argv[1], "") == 0){
    printf("debe ingresarse un argumento\n");
    printf("pruebe con:\n hola <-- Unirse al grupo multicast y al proyecto\n version <-- consultar version más reciente\n solicitud<-- solicitar actualizacion del repositorio local\n conf <-- cargar nombre de usuario y repositorio a utilizar\n");
    printf("\n Si es la primera vez que utiliza este programa corra en consola el comando './cliente conf' para realizar la configuración inicial\n");
  }else {
    printf("\033[1m\033[37m %s\033[0m no es un argumento correcto\n", argv[1]);
    printf("\npruebe con:\n \033[1m\033[37mhola\033[0m <-- Unirse al grupo multicast y al proyecto\n \033[1m\033[37mversion\033[0m <-- consultar version más reciente\n \033[1m\033[37msolicitud\033[0m<-- solicitar actualizacion del repositorio local\n \033[1m\033[37mconf\033[0m <-- cargar nombre de usuario y repositorio a utilizar\n");
    printf("\n Si es la primera vez que utiliza este programa corra en consola el comando './cliente conf' para realizar la configuración inicial\n");
  }


  return 0;
}
