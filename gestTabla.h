#ifndef gestTabla_H
#define gestTabla_H

void agregarUsuario (char * id_usuario, uint64_t version, uint32_t ip);

int buscarusuario(char * id_usuario, uint64_t version, uint32_t ip);

int actualizartabla(char * id_usuario, uint64_t versionb, uint32_t ipb);

uint32_t versionmayor(uint64_t versionb);

char * nombreusuario();

void print_ip(int ip);

char * getConf(int i);

void setConf(int i, char * str);

int getIpAddr();

int crearDir();

#endif
