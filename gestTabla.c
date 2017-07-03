#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>
#include <netdb.h>
#include <ifaddrs.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <pwd.h>

#include "gestTabla.h"

//Obtiene el nombre del usuario que esta en uso en la pc
char * nombreusuario(){

  char *login;
  struct passwd *pentry;

  /*login de sesión*/
  if ((login = getlogin()) == NULL){
    perror("getlogin");
    exit(EXIT_FAILURE);
  }

  /*Contraseña de sesión*/
  if ((pentry = getpwnam(login)) == NULL){
    perror("getpwnam");
    exit(EXIT_FAILURE);
  }

  /*Se muestran los datos de la sesión en pantalla*/
  //printf("%s", pentry ->pw_name);
  return(pentry ->pw_name);
}

//Retorna el IP del usuario con mayor version en la tabla de usuarios
uint32_t versionmayor(uint64_t versionb){

  char * usuario;
  uint64_t version;
  uint32_t ip;
  char * usuariomax;
  uint64_t versionmax;
  uint32_t ipmax;
  char tdir[100];
  char part[30] = "/home/";
  char part2[30] = "/.NOMBRE/.tabla";
  char * nombreusu = nombreusuario();
  strcpy(tdir , part);
  strcat(tdir,nombreusu);
  strcat(tdir,part2);
  FILE *pf;
  pf = fopen(tdir, "r+");
  versionmax = 0;

  while(!feof(pf)) {
   fscanf(pf, "%s %ld %d", usuario, &version, &ip);
     if(!feof(pf)){
            if(version > versionmax){
            	versionmax = version;
            	usuariomax = usuario;
            	ipmax = ip;
            }
     }
  }
  fclose(pf);
  if (versionmax <= versionb){
    ipmax = 0;
  	}
  return (ipmax);
}

//Actualiza los datos en la tabla de usuarios
int actualizartabla(char * id_usuario, uint64_t versionb, uint32_t ipb){

  char tdir[100];
  char tauxdir[100];
  char part[30] = "/home/";
  char part2[30] = "/.NOMBRE/.tabla";
  char part3[30] = "/.NOMBRE/.tablaaux";
  char * nombreusu = nombreusuario();
  strcpy(tdir , part);
  strcat(tdir,nombreusu);
  strcpy(tauxdir,tdir);
  strcat(tdir,part2);
  strcat(tauxdir,part3);
  char * usuario;
  uint64_t version;
  uint32_t ip;
  FILE *pf;
  pf = fopen(tdir, "r+");
  FILE *auxf;
  auxf = fopen(tauxdir, "w+");

		while(!feof(pf)){
  			fscanf(pf, "%s %ld %d", usuario, &version, &ip);
  				 if(!feof(pf)){
      					if(strcmp(usuario, id_usuario)== 0){
       					 fprintf(auxf, "%s %ld %d\n", id_usuario, versionb, ipb);
                }
       					else {
          				 fprintf(auxf, "%s %ld %d\n", usuario, version, ip);
            		}//end else
  					}//end if
				}//end while

  fclose(auxf);
  fclose(pf);
  remove(tdir);
  rename(tauxdir, tdir);
}

//Agrega un usuario junto a su version e ip, si ya esta agregado solo actualiza su version e ip
int buscarusuario(char * id_usuario, uint64_t versionb, uint32_t ipb){

  char tdir[100];
  char tauxdir[100];
  char part[30] = "/home/";
  char part2[30] = "/.NOMBRE/.tabla";
  char part3[30] = "/.NOMBRE/.tablaaux";
  char * nombreusu = nombreusuario();
  strcpy(tdir , part);
  strcat(tdir,nombreusu);
  strcpy(tauxdir,tdir);
  strcat(tdir,part2);
  strcat(tauxdir,part3);
  int a;
  char * usuario;
  uint64_t version;
  uint32_t ip;
  FILE *pf;
  pf = fopen(tdir, "r+");
  a = 0;

  while(!feof(pf)){
  fscanf(pf, "%s %ld %d", usuario, &version, &ip);
   if(!feof(pf)){
   	if(strcmp(usuario, id_usuario) == 0){
  	 a = 1;
  		}//end if
   		}//end if
   		}//end while
  fclose(pf);

  	if(a == 1) {
  	 FILE *auxf;
  	 pf = fopen(tdir, "r+");
  	 auxf = fopen(tauxdir, "w+");

  	//Usuario existente, se actualiza version e ip
  		while(!feof(pf)){
    			fscanf(pf, "%s %ld %d", usuario, &version, &ip);
    				 if(!feof(pf)){

        					if(strcmp(usuario, id_usuario)== 0){
         					 fprintf(auxf, "%s %ld %d\n", id_usuario, versionb, ipb);
                  							}
         					else {
            				 fprintf(auxf, "%s %ld %d\n", usuario, version, ip);
              					}//end if
    					}//end if
  				}//end while
  		fclose(auxf);
  		fclose(pf);
  		remove(tdir);
  		rename(tauxdir, tdir);
  			}//end if

  	//Agrego el usuario al final de la tabla si no existe

  	else{
  	pf = fopen(tdir, "a+");
  	fprintf(pf, "%s %ld %d\n", id_usuario, versionb, ipb);
  	fclose(pf);
    printf("La tabla de usuarios se ha actualizado\n");
  	}
}

//Se le pasa un IP en formato net y lo imprime en formato 127.0.0.1
  void print_ip(int ip){
      unsigned char bytes[4];
      bytes[0] = (ip >> 24) & 0xFF;
      bytes[1] = (ip >> 16) & 0xFF;
      bytes[2] = (ip >> 8) & 0xFF;
      bytes[3] = ip & 0xFF;

      syslog(LOG_NOTICE, "el ip es:%d.%d.%d.%d\n", bytes[3], bytes[2], bytes[1], bytes[0]);
  }

//Obtiene los valores de configuracion en el archivo .conf y los asigna
char * getConf(int i) {

  char *confDir = getenv("HOME");
  FILE *pf;
  char *line = malloc (500);
  int len1,len2,c,cont;
  cont = 0;
  pf = fopen(confDir, "r");
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

//Asigna str al valor correspondiente en el archivo de configuracion .conf
void setConf(int i, char * str){

  char *confDir = getenv("HOME");
  FILE *pf;
  char *buffer = malloc (100000000);
  int len1,len2,len3,c,cont;
  cont = 0;
  pf = fopen(confDir, "r+");
  fseek(pf, 0, SEEK_SET);
  for (;;) {
    c = fgetc(pf);
    if (c == EOF){
      len3 = ftell(pf);
      break;
    }
    if (c == ':'){
      cont++;
      if (i==cont){
        len1 = ftell(pf);

      }
    }
    if (i == cont) {
      if (c == EOF || c == '\n') {
        len2 = ftell(pf);
      }
    }
  }
  fseek(pf,len2,SEEK_SET);
  fread(buffer, len3-len2-1,1,pf);
  fseek(pf, len1, SEEK_SET);
  fprintf(pf,"%s\n", str);
  fseek(pf,0,SEEK_CUR);
  fprintf(pf,"%s\n", buffer);
  fclose(pf);
}

//Obtiene el IP actual del usuario y lo guarda en el archivo de configuración .conf
int getIpAddr(){
  FILE *f;
  char line[100] , *p , *c;

  f = fopen("/proc/net/route" , "r");

  while(fgets(line , 100 , f)){
      p = strtok(line , " \t");
      c = strtok(NULL , " \t");

      if(p!=NULL && c!=NULL)
      {
          if(strcmp(c , "00000000") == 0)
          {
              //printf("Default interface is : %s \n" , p);
              break;
          }
      }
  }
    //which family do we require , AF_INET or AF_INET6
    int fm = AF_INET;
    struct ifaddrs *ifaddr, *ifa;
    int family , s;
    char host[NI_MAXHOST];

    if (getifaddrs(&ifaddr) == -1)
    {
        perror("getifaddrs");
        exit(EXIT_FAILURE);
    }

    //Walk through linked list, maintaining head pointer so we can free list later
    for (ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next){
        if (ifa->ifa_addr == NULL){
            continue;
        }

        family = ifa->ifa_addr->sa_family;

        if(strcmp( ifa->ifa_name , p) == 0){
            if (family == fm){
                s = getnameinfo( ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6) , host , NI_MAXHOST , NULL , 0 , NI_NUMERICHOST);

                if (s != 0){
                    printf("getnameinfo() failed: %s\n", gai_strerror(s));
                    exit(EXIT_FAILURE);
                }

                //printf("address: %s", host);
                setConf(4,host);
            }
            //printf("\n");
        }
    }
    freeifaddrs(ifaddr);
    return 0;
}

//En caso de que no exista, crea el directorio y el archivo .tabla de la aplicacion en el home del usuario
int crearDir(){
  char * dir = getenv("HOME");
  strcat(dir,"/.NOMBRE");
  FILE *fp;

  if (access(dir,F_OK) != 0 ) {
    mkdir(dir, 0700);
    strcat(dir,"/.tabla");
    fp = fopen(dir,"w+");
    fclose(fp);
  }

  return 0;
}
