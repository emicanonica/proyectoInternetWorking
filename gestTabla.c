#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <syslog.h>

#include <dirent.h>

#include "gestTabla.h"


void agregarUsuario(char * id_usuario, uint64_t version, uint32_t ip){
  /*agrega un usuario al final de la tabla*/
  /*syslog (LOG_NOTICE, "+++++++++++++++++++++++");
  syslog (LOG_NOTICE, "entro a la funcion");
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");

  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
  syslog (LOG_NOTICE, " la version es:%ld, el id es:%s", version, id_usuario );
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");*/

  FILE *pf;
  pf = fopen("tabla.txt", "a+");
  /*syslog (LOG_NOTICE, "+++++++++++++++++++++++");
  syslog (LOG_NOTICE, "siguio");
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
*/
  fprintf(pf, "%s %ld\n", id_usuario, version);

  fclose(pf);
/*
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
  syslog (LOG_NOTICE, "salio");
  syslog (LOG_NOTICE, "+++++++++++++++++++++++");
*/
}
