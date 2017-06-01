#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int eliminar(){
/*elimina un usuario*/
int a;
char usuario[5];
char usuariob[5];
char version[12];
char ip[15];
FILE *pf;
FILE *auxf;

a = 0;
pf = fopen("tabla.txt", "r+");
auxf = fopen("tablaaux.txt", "w+");
printf("ingrese el usuario a eliminar (5 caracteres)\n");
scanf ("%s", &usuariob);

while(!feof(pf))
{
  fscanf(pf, "%s %s %s", &usuario, &version, &ip);
   if(!feof(pf)){
     a = strcmp(usuario, usuariob);
      if(a != 0){
       fprintf(auxf, "%s %s %s\n", usuario, version, ip);
                }
  }
}

fclose(auxf);
fclose(pf);
int remove(pf);
rename("tablaaux.txt", "tabla.txt");
printf("usuario eliminado...\n");
return (0);
}




int buscarusuario(){
/*buscar un usuario en especifico y actualiza su version e ip*/

int a;
char usuario[5];
char version[12];
char versionb[12];
char ipb[15];
char ip[15];
char usuariob[5];
FILE *pf;
pf = fopen("tabla.txt", "r+");
a = 0;
printf ("ingrese usuario a buscar (5 caracteres)\n");
scanf ("%s", &usuariob);
printf ("ingrese nueva version (12 caracteres)\n");
scanf ("%s", &versionb);
printf ("ingrese nuevo ip\n");
scanf ("%s", &ipb);

while(!feof(pf)){
fscanf(pf, "%s %s %s", &usuario, &version, &ip);
 if(!feof(pf)){
 	if(strcmp(usuario, usuariob) == 0){
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
  			fscanf(pf, "%s %s %s", &usuario, &version, &ip);
  				 if(!feof(pf)){
					 printf("%s %s %s", usuario, version, ip);
      					if(strcmp(usuario, usuariob)== 0){
       					 fprintf(auxf, "%s %s %s\n", usuariob, versionb, ipb);
                							}
       					else {
          				 fprintf(auxf, "%s %s %s\n", usuario, version, ip);
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
	fprintf(pf, "%s %s %s\n", usuariob, versionb, ipb);
	fclose(pf);
	}
	printf("tabla actualizada...\n");
	return (0);
	}



int agregarusuario(){
/*agrega un usuario al final de la tabla*/
int i;
unsigned int usuario;
unsigned long version;
char ip[15];
FILE *pf;
pf = fopen("tabla.txt", "a+");

printf("ingrese id de usuario:(5 caracteres)\n");
scanf("%d", &usuario);
printf("ingrese version(12 caracteres):\n" );
scanf("%lu", &version);
printf("ingrese ip\n");
scanf("%s", &ip);
printf("%d , %lu , %s\n", usuario, version, ip);
fprintf(pf, "%d %lu %s\n", usuario, version, ip);

fclose(pf);
return(0);

}


int listarversionmayor(){
/*Lista toda la tabla y encuentra el usuario con la version mas grande*/
int i;
char c;
char ip[15];
unsigned int usuario;
unsigned long version;
unsigned int usuariomax;
unsigned long versionmax;
char ipmax[15];
FILE *pf;
pf = fopen("tabla.txt", "rt");
usuario = 0;
version = 0;
usuariomax = 0;
versionmax = 0;
i=0;
while(!feof(pf))
{
 fscanf(pf, "%d %lu %s", &usuario, &version, &ip);
 if(!feof(pf)){
        if(version >= versionmax){
        	versionmax = version;
        	usuariomax = usuario;
        	strcpy(ipmax,ip);
             }
printf("usuario: %d  version: %lu  ip: %s \n", usuario, version, ip);
}
}
printf("usuario con version mas reciente: %d, version: %lu, ip: %s\n", usuariomax, versionmax, ipmax);


fclose(pf);
return (0);
}






int main() {
int i;
i = 0;
FILE *pf;
pf = fopen("tabla.txt", "at");
while(i!=100){
	printf("ingrese acción:\n 1--> Obtener mayor version\n 2--> Agregar usuario\n 3--> Buscar usuario y actualizar version e ip\n 4--> Eliminar un usuario\n 100--> Salir\n");
	scanf("%d", &i);
	switch (i)
	{
	case 1:
		listarversionmayor();
		break;
	case 2:
		agregarusuario();
		break;
	case 3:
		buscarusuario();
		break;
	case 4:
		eliminar();
		break;
	default:
		printf("error\n");
		main();
		break;
	case 100:
		printf("saliendo..\n");
		exit(0);
}
}
fclose(pf);
return(0);
}
