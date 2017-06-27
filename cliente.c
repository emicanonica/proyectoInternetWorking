#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "mensajes/mensaje.h"
#include "mensajes/mensajeMulticast.h"
#include "gestTabla.h"

//declaracion de variables
char str1[50],str2[500];
char *ptr, *confDir, *dir;
unsigned long localVersion;
char *idUsuario, *direccion;
uint32_t IpDestino, localIp;

int main(int argc, char const *argv[]) {

  //Verifica si existe el directorio de la aplicacion y en caso de que no exista lo crea
  crearDir();

  dir = getenv("HOME");
  confDir = dir;
  strcat(confDir,"/.conf");

//Verificación de la existencia de los archivos de configuracion
  if (access(confDir,F_OK) != 0 && strcmp(argv[1], "conf") != 0 ) {
    printf("No es posible encontrar los archivos de configuracion, por favor Corra el comando \033[1m\033[37m ./NOMBRE conf\033[0m \n");
    exit(0);
  }

//verifica que haya un argumento
  if (argc <= 1){
    printf("Uso: NOMBRE [OPTION] \n");
    printf("\nOpciones de configuracion:\n\n conf \t\t cargar nombre de usuario y ubicacion del repositorio de trabajo\n setId \t\t Ingresa nuevo nombre de usuario\n setDir \t Ingresa la nueva ubicación del repositorio de trabajo\n ");
    printf("\nOpciones de uso:\n\n hola \t\t Unirse al grupo de proyecto\n version \t consultar version más reciente\n actualizar \t actualiza el repositorio local\n");
    printf("\n\n Si es la primera vez que utiliza este programa corra en consola el comando './NOMBRE conf' para realizar la configuración inicial\n");
    exit(0);
  }

//Inicializacion de valores de variables
  if(access(confDir,F_OK) == 0){
    localVersion = strtol(getConf(3),&ptr,10);
    idUsuario = getConf(1);
    direccion = getConf(2);
    IpDestino = inet_addr("127.0.0.1"); //obtener de tabla //POR AHORA USARLO ASI
    localIp = inet_addr(getConf(4));   //se utiliza en multicast solamente

  }

//Menu
  if (strcmp(argv[1], "conf") == 0) {

    printf("Ingrese su nombre de usuario: ");
    fgets(str1,sizeof(str1),stdin);
    while (strlen(str1)-1 == 0) {
      printf("\033[1m\033[37m El nombre de usuario no puede ser vacio\033[0m \n");
      printf("Ingrese su nombre de usuario: ");
      fgets(str1,sizeof(str1),stdin);
    }
    str1[strlen(str1)-1] = '\0';
    for (size_t i = 0; i < strlen(str1); i++) {
      if (str1[i] == ' ') {
        str1[i] = '_';
      }
    }
    printf("Ingrese la ubicación de la carpeta de trabajo: ");
    fgets(str2,sizeof(str2),stdin);
    str2[strlen(str2)-1] = '\0';
    if (str2[strlen(str2)-1] == '/')  {
      str2[strlen(str2)-1]='\0';
    }
    while (access(str2,F_OK) != 0) {
      printf("\033[1m\033[37m El directorio especificado no exite\033[0m \n");
      printf("Ingrese la ubicación de la carpeta de trabajo: ");
      fgets(str1,sizeof(str1),stdin);
      if (str2[strlen(str2)-1] == '/')  {
        str2[strlen(str2)-1]='\0';
      }
    }
    strcat(str2,"/");
    FILE *pf;
    pf = fopen(confDir, "w+");
    fprintf(pf, "id de usuario:\nUbicación del repositorio:\nVersion:\nIp:\n");
    fclose(pf);
    setConf(1,str1);
    setConf(2,str2);
    getIpAddr();

//mensaje multicast 'hola'
  } else if (strcmp(argv[1], "hola") == 0) {   //envia un mensaje con COD=1 por multicast para cargar las tablas

      if (mensajeMulticast(1, localVersion, localIp, idUsuario) < 0) {
        perror("Error de envio de mensaje multicast 'hola'");
      }

//mensaje multicast 'version'
  } else if (strcmp(argv[1], "version") == 0) { //envia un mensaje con COD=3 por multicast para verificar las versiones

    if (mensajeMulticast(3, localVersion, localIp, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'version'");
    }

//mensaje unicast 'actualizar'
  } else if (strcmp(argv[1], "actualizar") == 0) { // enviar mensaje "version" y luego enviar mensaje con=6 al mas actualizado en la tabla

    if (mensajeMulticast(3, localVersion, IpDestino, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'version'");
    }
    //BUSCAR LA VERSION MAS ACTUALIZADA EN LA TABLA, TOMAR LA IP Y USARLA PARA ENVIAR UN MENSAJE SOLICITUD (actualizar IpDestino con el mas actual de la tabla)
    //VERIFICAR EN LA TABLA QUE LA VERSION LOCAL SEA MENOR QUE LA MAS ACTUALIZADA, EN ESTE CASO MANDAR UN MENSAJE QUE DIGA QUE SE TIENE LA VERSION MAS ACTUAL
    if (mensaje(6, localVersion, IpDestino, localIp, idUsuario) < 0) {
      perror("Error de envio de mensaje multicast 'solicitud'");
    }

//Cambio de nombre de usuario en el archivo de configuración
  }else if (strcmp(argv[1], "setId") == 0) { //cambiar nombre de usuarion

    printf("Ingrese su nombre de usuario: ");
    fgets(str1,sizeof(str1),stdin);
    while (strlen(str1)-1 == 0) {
      printf("\033[1m\033[37m El nombre de usuario no puede ser vacio\033[0m \n");
      printf("Ingrese su nombre de usuario: ");
      fgets(str1,sizeof(str1),stdin);
    }
    str1[strlen(str1)-1] = '\0';
    for (size_t i = 0; i < strlen(str1); i++) {
      if (str1[i] == ' ') {
        str1[i] = '_';
      }
    }
    setConf(1,str1);

//Cambio de ubicacion del repositorio en el archivo de configuración
  } else if (strcmp(argv[1], "setDir") == 0) { //cambiar repositorio a ser observado

    printf("Ingrese la ubicación de la carpeta de trabajo: ");
    fgets(str2,sizeof(str2),stdin);
    str2[strlen(str2)-1] = '\0';
    if (str2[strlen(str2)-1] == '/')  {
      str2[strlen(str2)-1]='\0';
    }
    while (access(str2,F_OK) != 0) {
      printf("\033[1m\033[37m El directorio especificado no exite\033[0m \n");
      printf("Ingrese la ubicación de la carpeta de trabajo: ");
      fgets(str1,sizeof(str1),stdin);
      if (str2[strlen(str2)-1] == '/')  {
        str2[strlen(str2)-1]='\0';
      }
    }
    strcat(str2,"/");
    setConf(2,str2);

//Establece una version al repositorio local y actualiza los archivos
  } else if (strcmp(argv[1], "setVersion") == 0) { //setea el tiempo local como numero de la version del repositorio

    char vt[16];
    time_t rawtime;
    struct tm *info;
    time( &rawtime );
    info = localtime( &rawtime );
    strftime(vt,16,"%Y%m%d%H%M%S", info);
    setConf(3,vt);
    //COPIAR ARCHIVOS DESDE LA UBICACION ESTABLECIDA A LA CARPETA DE LA APLICACION, LUEGO MANDAR UN MENSAJE HOLA PARA QUE ACTUALIZEN LAS TABLAS LOS OTROS USUARIOS

//ayuda
  }else if (strcmp(argv[1], "help") == 0) { //setea el tiempo local como numero de la version del repositorio

    printf("\nUso: NOMBRE [OPTION] \n");
    printf("\nOpciones de configuracion:\n\n conf \t\t cargar nombre de usuario y ubicacion del repositorio de trabajo\n setId \t\t Ingresa nuevo nombre de usuario\n setDir \t Ingresa la nueva ubicación del repositorio de trabajo\n ");
    printf("\nOpciones de uso:\n\n hola \t\t Unirse al grupo de proyecto\n version \t consultar version más reciente\n actualizar \t actualiza el repositorio local\n");
    printf("\n\n Si es la primera vez que utiliza este programa corra en consola el comando './NOMBRE conf' para realizar la configuración inicial\n");

//En caso de que se ingrese un argumento incorrecto
  } else {

    printf("\033[1m\033[37m%s\033[0m no es un argumento correcto\n", argv[1]);
    printf("\nUso: NOMBRE [OPTION] \n");
    printf("\nOpciones de configuracion:\n\n conf \t\t cargar nombre de usuario y ubicacion del repositorio de trabajo\n setId \t\t Ingresa nuevo nombre de usuario\n setDir \t Ingresa la nueva ubicación del repositorio de trabajo\n ");
    printf("\nOpciones de uso:\n\n hola \t\t Unirse al grupo de proyecto\n version \t consultar version más reciente\n actualizar \t actualiza el repositorio local\n");
    printf("\n\n Si es la primera vez que utiliza este programa corra en consola el comando './NOMBRE conf' para realizar la configuración inicial\n");
  }


  return 0;
}
