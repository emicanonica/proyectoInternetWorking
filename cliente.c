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

char str1[50],str2[500];

int main(int argc, char const *argv[]) {

  if (access(".conf",F_OK) != 0) {
    printf("No es posible encontrar la configuracion, por favor ingrese los siguientes datos:\n");
    FILE *pf;
    pf = fopen(".conf", "w+");
    fprintf(pf, "id de usuario:\nUbicación del repositorio:\nVersion:\nIp:\n");
    fclose(pf);
    printf("Ingrese su nombre de usuario: ");
    scanf("%s",str1);
    setConf(1,str1);
    printf("Ingrese la direccion de la carpeta donde desea que se realice la copia de archivos: ");
    //COMPROBAR QUE LA CARPETA SEA VALIDA, EN CASO CONTRARIO PEDIR OTRA VEZ
    //VERIFICAR QUE LA DIRECCION TERMINE CON "/"
    scanf("%s",str2);
    setConf(2,str2);
    getIpAddr();
  }

  char *ptr;
  unsigned long localVersion = strtol(getConf(3),&ptr,10); //obtener de tabla
  char * idUsuario = getConf(1);
  char * direccion = getConf(2);
  uint32_t IpDestino = inet_addr("IP DESTINO"); //obtener de tabla //POR AHORA USARLO ASI
  uint32_t localIp = inet_addr(getConf(4));   //se utiliza en multicast solamente
  //uint32_t MasActualIp = inet_addr("192.168.0.17"); //obtener de tabla !IMPORTANTE!

  if (strcmp(argv[1], "hola") == 0) {   //envia un mensaje con COD=1 por multicast para cargar las tablas
    if (mensajeMulticast(1, localVersion, localIp, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'hola'");
    }
    //SI ENVIO HOLA Y NO HAY RESPUESTA ENTONCES MI REPOSITORIO ES LA UNICA VERSION.
    //EN CASO DE QUE OCURRA LO DE ARRIBA LO UNICO QUE HAY QUE HACER ES: VERIFICAR SI LA TABLA ESTA VACIA, SI LO ESTÁ IMPRIMIR UN MENSAJE QUE DIGA QUE NO HAY MAS USUARIOS, SINO, IMPRIMIR "LA TABLA SE HA ACTUALIZADO CON EXITO"

  } else if (strcmp(argv[1], "version") == 0) { //envia un mensaje con COD=3 por multicast para verificar las versiones
    if (mensajeMulticast(3, localVersion, localIp, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'version'");
    }

  } else if (strcmp(argv[1], "setId") == 0) { //cambiar nombre de usuarion IMPORTANTE:NO SE PUEDEN INGRESAR ESPACIOS (ARREGLAR)
    printf("Ingrese su nombre de usuario: ");
    scanf("%s",str1);
    setConf(1,str1);

  } else if (strcmp(argv[1], "setDir") == 0) { //cambiar repositorio a ser observado
    printf("Ingrese la direccion de la carpeta donde desea que se realice la copia de archivos: ");
    scanf("%s",str2);
    setConf(2,str2);

  } else if (strcmp(argv[1], "conf") == 0) { //reiniciar la configuracion, pregunta nuevo id y repositorio y reinicia el ip y la version
    FILE *pf;
    pf = fopen(".conf", "w+");
    fprintf(pf, "id de usuario:\nUbicación del repositorio:\nVersion:\nIp:\n");
    fclose(pf);
    printf("Ingrese su nombre de usuario: ");
    scanf("%s",str1);
    setConf(1,str1);
    printf("Ingrese la direccion de la carpeta donde desea que se realice la copia de archivos: ");
    //comprobar que la carpeta sea valida, en caso de ser invalida, pedir otra vez
    //verificar que la direccion termine con "/" sino agregarla
    scanf("%s",str2);
    setConf(2,str2);
    getIpAddr();

  }else if (strcmp(argv[1], "actualizar") == 0) { // enviar mensaje "version" y luego enviar mensaje con=6 al mas actualizado en la tabla
    /*if (mensajeMulticast(3, localVersion, IpDestino, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'version'");
    }*/
    //BUSCAR LA VERSION MAS ACTUALIZADA EN LA TABLA, TOMAR LA IP Y USARLA PARA ENVIAR UN MENSAJE SOLICITUD (actualizar IpDestino con el mas actual de la tabla)
    if (mensaje(6, localVersion, IpDestino, localIp, idUsuario) < 0) {
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

  //FALTA OPCION PARA AGREGAR ARCHIVOS
  //CREAR CARPETA PARALELA A LA UBICACION DEL REOPSITRIO ESPECIFICADO POR EL USUARIO Y USAR ESTA PARA ENVIAR LOS ARCHIVOS


  return 0;
}
