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
uint64_t localVersion;
char *idUsuario, *direccion;
uint32_t IpDestino, localIp;
char * nombreusu;

int main(int argc, char const *argv[]) {

//Verifica si existe el directorio de la aplicacion y en caso de que no exista lo crea
  crearDir();

  dir = getenv("HOME");
  confDir = dir;
  strcat(confDir,"/.conf"); //variable que guarda la ubicación del archivo de configuración

//Verificación de la existencia de los archivos de configuracion
  if (argc > 1) {
    if (access(confDir,F_OK) != 0 && strcmp(argv[1], "conf") != 0 ) {
      printf("No es posible encontrar los archivos de configuracion, por favor Corra el comando \033[1m\033[37m ./rockup conf\033[0m \n");
      exit(0);
    }
  } else {
    if (access(confDir,F_OK) != 0) {
      printf("No es posible encontrar los archivos de configuracion, por favor Corra el comando \033[1m\033[37m ./rockup conf\033[0m \n");
      exit(0);
    }
  }

//verifica que haya un argumento
  if (argc <= 1){
    printf("Uso: rockup [OPTION] \n");
    printf("\nOpciones de configuracion:\n\n conf \t\t cargar nombre de usuario y ubicacion del repositorio de trabajo\n setId \t\t Ingresa nuevo nombre de usuario\n setDir \t Ingresa la nueva ubicación del repositorio de trabajo\n ");
    printf("\nOpciones de uso:\n\n hola \t\t Unirse al grupo de proyecto\n version \t consultar version más reciente\n actualizar \t trae los archivos de la version más actual en la red\n setVersion \t actualiza el directorio local y le asigna un número de versión\n");
    printf("\n\n Si es la primera vez que utiliza este programa corra en consola el comando './rockup conf' para realizar la configuración inicial\n");
    exit(0);
  }

//Inicializacion de valores de variables
  if(access(confDir,F_OK) == 0){
    localVersion = strtol(getConf(3),&ptr,10);
    idUsuario = getConf(1);
    direccion = getConf(2);
    //IpDestino = inet_addr("127.0.0.1"); //obtener de tabla //POR AHORA USARLO ASI
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
      fgets(str2,sizeof(str2),stdin);
      str2[strlen(str2)-1] = '\0';
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

//Verifica que la tabla de usuarios tenga datos
      char tdir[100];
      char part[30] = "/home/";
      char part2[30] = "/.rockup/";
      char part3[30] = ".tabla";
      char comando[100];
      char cmd1[30] = "find ";
      char * nombreusu = nombreusuario();

      strcpy(tdir , part);
      strcat(tdir,nombreusu);
      strcat(tdir,part2);
      strcpy(comando, cmd1);
      strcat(comando , tdir);
      strcat(comando," ! -name '.conf' ! -name '.tabla' -type f -exec rm -f {} +");
      strcat(tdir,part3);


      FILE *pf =fopen(tdir,"r");
      fseek(pf, 0, SEEK_END);
      if (ftell(pf) == 0){
        printf("No es posible ubicar otros usuarios en la red, por favor corra el comando \033[1m\033[37m./rockup hola\033[0m para actualizar la tabla de usuarios\n");
        exit(0);
      };

      //printf("Tenga en cuenta que debe correr el comando \033[1m\033[37m./rockup version\033[0m\n antes de actualizar\n");

//Obtiene el ip del usuario con la mayor version en la red, en caso de que ese usuario sea el usuario local retorna 0
  		IpDestino = versionmayor(localVersion);

      if (IpDestino != 0) {
        if (mensaje(6, localVersion, IpDestino, localIp, idUsuario) < 0) {
          perror("Error en actualizar\n");
        }
      } else {
          printf("La version del repositorio local es la mas actual\n");
      }

//Borra el contenido de la carpeta de la aplicación
      system( comando );

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
  } else if (strcmp(argv[1], "setVersion") == 0) {

//setea el tiempo local como numero de la version del repositorio
    char vt[16];
    time_t rawtime;
    struct tm *info;
    time( &rawtime );
    info = localtime( &rawtime );
    strftime(vt,16,"%Y%m%d%H%M%S", info);
    setConf(3,vt);

//copia los archivos desde la ubicacion del repositorio establecida por el usuario a el directorio de la aplicación
  	nombreusu = nombreusuario();
  	char * part2 = strcat(direccion,"*"); //direccion de la carpeta donde se guardan los archivos
  	char part[30] = "/home/"; //direccion de la carpeta donde se versiona y comparte los archivos, carpeta oculta la que puse es de ejemplo
  	char part3[30] = "/.rockup/";
  	char comando[100];
  	char cmd1[30] = "find ";
  	char cmd2[30] = "cp -rf ";
  	strcpy(comando , cmd1);
  	strcat(comando , part);
  	strcat(comando , nombreusu);
  	strcat(comando , part3);
  	strcat(comando," ! -name '.conf' ! -name '.tabla' -type f -exec rm -f {} +");
  	system( comando ); // el comando quedaria "find /home/emi/.rockup/ ! -name '.conf' ! -name '.tabla' -type f -exec rm -f {} +"
  	strcpy(comando , cmd2);
  	strcat(comando , part2);
  	strcat(comando , " ");
  	strcat(comando , part);
  	strcat(comando , nombreusu);
  	strcat(comando , part3);
  	system( comando ); // el comando quedaria "cp -rf "carpeta_visible" "carpeta_oculta"
  	printf("Repositorio actualizado a la versión %s\n", vt);

//ayuda
  }else if (strcmp(argv[1], "help") == 0) { //setea el tiempo local como numero de la version del repositorio

    printf("\nUso: rockup [OPTION] \n");
    printf("\nOpciones de configuracion:\n\n conf \t\t cargar nombre de usuario y ubicacion del repositorio de trabajo\n setId \t\t Ingresa nuevo nombre de usuario\n setDir \t Ingresa la nueva ubicación del repositorio de trabajo\n ");
    printf("\nOpciones de uso:\n\n hola \t\t Unirse al grupo de proyecto\n version \t consultar version más reciente\n actualizar \t trae los archivos de la version más actual en la red\n setVersion \t actualiza el directorio local y le asigna un número de versión\n");
    printf("\n\n Si es la primera vez que utiliza este programa corra en consola el comando './rockup conf' para realizar la configuración inicial\n");

//En caso de que se ingrese un argumento incorrecto
  } else {

    printf("\033[1m\033[37m%s\033[0m no es un argumento correcto\n", argv[1]);
    printf("\nUso: rockup [OPTION] \n");
    printf("\nOpciones de configuracion:\n\n conf \t\t cargar nombre de usuario y ubicacion del repositorio de trabajo\n setId \t\t Ingresa nuevo nombre de usuario\n setDir \t Ingresa la nueva ubicación del repositorio de trabajo\n ");
    printf("\nOpciones de uso:\n\n hola \t\t Unirse al grupo de proyecto\n version \t consultar version más reciente\n actualizar \t trae los archivos de la version más actual en la red\n setVersion \t actualiza el directorio local y le asigna un número de versión\n");
    printf("\n\n Si es la primera vez que utiliza este programa corra en consola el comando './rockup conf' para realizar la configuración inicial\n");
  }


  return 0;
}
