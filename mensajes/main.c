#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int agregarusuario(){
/*agrega un usuario al final de la tabla*/
int i;
unsigned int usuario;
unsigned long version;
char ip[15];
FILE *pf;
pf = fopen("tabla.txt", "at");

printf("ingrese id de usuario:(5 caracteres)\n");
scanf("%d", &usuario);
printf("ingrese version(12 caracteres):\n" );
scanf("%lu", &version);
printf("ingrese ip\n");
scanf("%s", &ip);
printf("%d , %lu , %s\n", usuario, version, ip);
fprintf(pf, "\n%d %lu %s", usuario, version, ip);

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
while ((c=fgetc(pf)) != EOF) {
fscanf(pf, "%d %lu %s", &usuario, &version, &ip);
if(version >= versionmax){
	versionmax = version;
	usuariomax = usuario;
	strcpy(ipmax,ip);
}

printf("usuario: %d  version: %lu  ip: %s \n", usuario, version, ip);
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
	printf("ingrese accion: 1->obtener mayor version, 2->agregar usuario, 100 -> salir\n");
	scanf("%d", &i);
	switch (i)
	{
	case 1:
		listarversionmayor();
		break;
	case 2:
		agregarusuario();
		break;
	default:
		printf("error\n");
		break;
	case 100:
		printf("saliendo..\n");
		exit(0);
}
}
fclose(pf);
return(0);
}
