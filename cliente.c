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


int main(int argc, char const *argv[]) {

  unsigned long localVersion = 114455661122; //obtener de tabla
  char * idUsuario = getConf(1);
  //printf("%s\n", idUsuario);
  char * direccion = getConf(2);
  //printf("%s\n", direccion);
  uint32_t IpDestino = inet_addr("192.168.0.18"); //obtener de tabla
  uint32_t localIp = inet_addr(getConf(4));   //se utiliza en multicast solamente
  //uint32_t MasActualIp = inet_addr("192.168.0.17"); //obtener de tabla !IMPORTANTE!

  //fclose(pf);

  char str1[50],str2[500];

  if (strcmp(argv[1], "hola") == 0) {
    if (mensajeMulticast(1, localVersion, localIp, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'hola'");
    }
  } else if (strcmp(argv[1], "version") == 0) {
    if (mensajeMulticast(3, localVersion, localIp, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'version'");
    }
  } else if (strcmp(argv[1], "setId") == 0) {
    printf("Ingrese su nombre de usuario: ");
    scanf("%s",str1);
    setConf(1,str1);
  } else if (strcmp(argv[1], "setDir") == 0) {
    printf("Ingrese la direccion de la carpeta donde desea que se realice la copia de archivos: ");
    scanf("%s",str2);
    setConf(2,str2);
  } else if (strcmp(argv[1], "conf") == 0) {
    FILE *pf;
    pf = fopen(".conf", "w+");
    fprintf(pf, "id de usuario:\nUbicación del repositorio:\nVersion:\nIp:\n");
    fclose(pf);
    printf("Ingrese su nombre de usuario: ");
    scanf("%s",str1);
    setConf(1,str1);
    printf("Ingrese la direccion de la carpeta donde desea que se realice la copia de archivos: ");
    scanf("%s",str2);
    setConf(2,str2);

  }else if (strcmp(argv[1], "solicitud") == 0) { // enviar mensaje "version" y luego enviar mensaje con=6 al mas actualizado en la tabla
  /*  if (mensajeMulticast(3, localVersion, IpDestino, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'version'");
    }*/
    //BUSCAR LA VERSION MAS ACTUALIZADA EN LA TABLA, TOMAR LA IP Y USARLA PARA ENVIAR UN MENSAJE SOLICITUD
    if (mensaje(6, localVersion, IpDestino, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'solicitud'");
    }
  }else if (strcmp(argv[1], "") == 0){ //ESTO NO FUNCIONA
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
