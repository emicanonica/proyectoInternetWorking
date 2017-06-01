#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

#include <dirent.h>

#include "gestTabla.h"


void agregarUsuario(char * id_usuario, uint64_t version, uint32_t ip){

  FILE *pf;
  pf = fopen("tabla.txt", "a+");

  fprintf(pf, "%s %ld\n", id_usuario, version);

  fclose(pf);

}

int buscarusuario(char * id_usuario, uint64_t versionb, uint32_t ipb){
/*buscar un usuario en especifico y actualiza su version e ip*/

int a;
char * usuario;
uint64_t version;
//char versionb[12];
//char ipb[15];
uint32_t ip;
//char usuariob[5];
FILE *pf;
pf = fopen("tabla.txt", "r+");
a = 0;
/*printf ("ingrese usuario a buscar (5 caracteres)\n");
scanf ("%s", &usuariob);
printf ("ingrese nueva version (12 caracteres)\n");
scanf ("%s", &versionb);
printf ("ingrese nuevo ip\n");
scanf ("%s", &ipb);*/

while(!feof(pf)){
fscanf(pf, "%s %ld %d", usuario, version, ip);
 if(!feof(pf)){
 	if(strcmp(usuario, id_usuario) == 0){
	 a = 1;
	}//end if
 		}//end if
 		}//end while
fclose(pf);
	if(a == 1) {
	printf("entro a actualizar");
	 FILE *pf;
	 FILE *auxf;
	 pf = fopen("tabla.txt", "r+");
	 auxf = fopen("tablaaux.txt", "w+");
		//Esto lo hago si encuentro al usuario q quiero agregar

		while(!feof(pf)){
  			fscanf(pf, "%s %ld %d", usuario, version, ip);
  				 if(!feof(pf)){
					 //printf("%s %ld %d", usuario, version, ip);
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
		//remove(pf);
		//rename("tablaaux.txt", "tabla.txt");
			}//end if

	//Agrego el usuario al final de la tabla si no existe

	else{
	FILE *pf;
	pf = fopen("tabla.txt", "a+");
	fprintf(pf, "%s %ld %d\n", id_usuario, versionb, ipb);
	fclose(pf);
	}
	printf("tabla actualizada...\n");
	return (0);
	}

  void print_ip(int ip){
      unsigned char bytes[4];
      bytes[0] = (ip >> 24) & 0xFF;
      bytes[1] = (ip >> 16) & 0xFF;
      bytes[2] = (ip >> 8) & 0xFF;
      bytes[3] = ip & 0xFF;

      syslog(LOG_NOTICE, "el ip es:%d.%d.%d.%d\n", bytes[3], bytes[2], bytes[1], bytes[0]);
      printf("el ip es:%d.%d.%d.%d\n", bytes[3], bytes[2], bytes[1], bytes[0]);
  }

char * getConf(int i) { //TOMA LOS VALORES EN .conf Y LOS PONES EN LAS VARIABLES
  FILE *pf;
  char *line = malloc (500);
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

void setConf(int i, char * str){
  FILE *pf;
  //char buffer[500];
  char *buffer = malloc (100000000);
  int len1,len2,len3,c,cont;
  cont = 0;
  pf = fopen(".conf", "r+");
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
