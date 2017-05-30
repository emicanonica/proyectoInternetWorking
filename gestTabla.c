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
