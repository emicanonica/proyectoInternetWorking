# Rockup sincronizador de archivos

<b>Proyecto realizado en la materia electiva internetworking en la Universidad Tecnológica Nacional - FRLP</b>

<b>Descripcion:</b>
En este trabajo se plantea el desarrollo de una aplicación que permitirá a múltiples usuarios dentro de una misma red utilizar un repositorio virtual compartido, basándose en la idea de un controlador de versionado descentralizado, el cual contará con su propio protocolo de sincronización entre los distintos repositorios, dando la posibilidad a los usuarios de realizar  modificaciones en un repositorio local y compartirlo con el resto. Esta herramienta será para un uso dentro de una red local y orientado al p2p.

<b>Compilación:</b>

Se debe tener instalado el compilador llamado GCC (the GNU Compiller Collection).

Comando para compilar el servidor ‘escucha’:
gcc escucha.c mensaje/mensaje.c mensaje/mensajeMulticast.c mensaje/enviarArchivo.c mensaje/recvArchivo.c gestTabla.c -o escucha

Comando para compilar el cliente ‘rockup’:
gcc escucha.c mensaje/mensaje.c mensaje/mensajeMulticast.c gestTabla.c -o rockup

<b>Ejecución:</b>
Ejecutar en consola el comando ./escucha para comenzar a interactuar con los demás usuarios de la aplicación.

Las diferentes acciones a realizar se llevan a cabo a través de los parámetros del comando “./rockup”

<b>Uso:</b>
rockup [OPTION]

<b>Opciones de configuración:</b>

 conf		Cargar nombre de usuario y ubicación del repositorio de trabajo
 setId		Ingresa nuevo nombre de usuario
 setDir		Ingresa la nueva ubicación del repositorio de trabajo
 
<b>Opciones de uso:</b>

 hola		Unirse al grupo de proyecto
 version	Consultar versión más reciente
 actualizar	Trae los archivos de la versión más actual en la red
 setVersion	Actualiza el directorio local y le asigna un número de versión

<b>Detener la ejecución:</b>

Si se quiere detener la ejecución del “escucha” uno debe hacer foco sobre la terminal donde se encuentra en ejecución y presionar las teclas Ctrl+C. 

<b>Configuración inicial:</b>

	Durante la primera ejecución del programa se deberá ingresar un par de datos para comenzar a utilizar la aplicación.
	Para acceder a esta configuración se ingresa el siguiente comando ./rockup conf

Los datos son dos:

Nombre de usuario:

	Es el nombre que lo identificará a usted como usuario y puede ser cambiado en cualquier momento

Ubicación del repositorio privado:

	Se deberá ingresar la ubicación de la carpeta en la cual desea trabajar, el contenido de esta carpeta es solo visible para usted.

	Una vez realizado estos dos pasos se creará en segundo plano una carpeta llamada “.rockup” en la carpeta home de su usuario, en la cual se guardará los archivos de configuración y los archivos versionados listos para compartirse.
	
<b>Incorporación a la red de trabajo:</b>

	Una vez terminada la configuración inicial estamos en condiciones de incorporarnos a la red de trabajo, para lograr este paso nos respaldamos en el comando hola.
	Este comando nos permitirá descubrir a todos los participantes que se encuentren en la red local y obtener sus ip y versión del repositorio.
	A su vez los demás participantes son advertidos de nuestra presencia con lo cual proceden a guardar la dirección ip y versión que manejemos a la hora de utilizar el comando hola.


<b>Versionar nuestro repositorio:</b>

	Para poder compartir nuestras modificaciones dentro del repositorio con los demás participantes se deberá versionar nuestros cambios, esto se logra gracias al comando setVersion.
	Este comando va a guardar nuestros cambios en el repositorio dentro de la carpeta .rockup y se le asignará una versión que se mostrará en pantalla.
Una vez ejecutado este comando nuestro repositorio estará listo para compartirse con los demás participantes.


<b>Comprobar vigencia de nuestro repositorio:</b>

	Una de las acciones que podemos realizar es comprobar si nuestro repositorio es el más actual o existe algún otro repositorio más actualizado que el nuestro.
	Para realizar esta acción debemos recurrir al comando version, el cual consultará las versiones que poseen los demás participantes del trabajo, las compara con nuestra versión y determina si estamos o no actualizados.
	Para ambos casos se informará por pantalla el resultado de la comparación de versiones.

<b>Actualizar nuestro repositorio:</b>

	El comando principal de Rockup es el comando actualizar, este comando nos traerá la versión más actual del repositorio existente en todo el grupo de trabajo y nos depositara su contenido en nuestra carpeta privada (definida su ubicación en la configuración inicial).
	Al ejecutarse este comando nuestra versión es cambiada por la más actual y en nuestra carpeta pública se encontrará el contenido del repositorio actualizado.
	Solo podremos ejecutar este comando si nuestro repositorio está desactualizado, en caso de tener una versión actual no se podrá ejecutar.
