#ifndef gestTabla_H
#define gestTabla_H

void agregarUsuario (char * id_usuario, uint64_t version, uint32_t ip);

char * getConf(int i);

void setConf(int i, char * str);

#endif
