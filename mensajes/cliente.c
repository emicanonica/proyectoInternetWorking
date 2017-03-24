#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>

#include "mensaje.h"
#include "mensajeMulticast.h"


int main(int argc, char const *argv[]) {

  unsigned long localVersion = 114455661122; //obtener de tabla
  char * idUsuario = "uno"; //obtener de tabla
  uint32_t IpDestino = inet_addr("192.168.0.17"); //obtener de tabla
  uint32_t localIp = inet_addr("192.168.0.17");   //se utiliza en multicast solamente
  //uint32_t MasActualIp = inet_addr("192.168.0.17"); //obtener de tabla !IMPORTANTE!

  if (strcmp(argv[1], "hola") == 0) {
    if (mensajeMulticast(1, localVersion, localIp, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'hola'");
    }
  } else if (strcmp(argv[1], "version") == 0) {
    if (mensajeMulticast(3, localVersion, localIp, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'version'");
    }
  } else if (strcmp(argv[1], "solicitud") == 0) { // enviar mensaje "version" y luego enviar mensaje con=6 al mas actualizado en la tabla
    if (mensajeMulticast(3, localVersion, IpDestino, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'version'");
    }
    //BUSCAR LA VERSION MAS ACTUALIZADA EN LA TABLA, TOMAR LA IP Y USARLA PARA ENVIAR UN MENSAJE SOLICITUD
    if (mensaje(6, localVersion, IpDestino, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'version'");
    }
  }else {
    printf("%s no es un argumento correcto\n", argv[1]);
    printf("pruebe con:\n hola\n version\n solicitud\n");
  }


  return 0;
}
