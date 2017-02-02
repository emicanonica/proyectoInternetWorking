#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


int main(int argc, char const *argv[]) {
DIR* dir = opendir("$HOME/.heca");
if (dir)
{
  printf("exite\n");
    closedir(dir);
}
else if (ENOENT == errno)
{
  crearCarpeta();
}

  return 0;
}

crearCarpeta() {
  mkdir("$HOME/.heca");
  return 0;
}
