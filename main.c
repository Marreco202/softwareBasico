/*Felipe Maia Meiga 2011460 3WA*/
/*João Victor Godinho Woitschach 2011401 3WB*/

#include <stdio.h>
#include <stdlib.h>
#include "gera.h"

int main(int args, char *argv[]) {
  FILE *myfp;
  funcp funcaoSimples;
  int res;

  /* Abre o arquivo fonte */
  if ((myfp = fopen(argv[1], "r")) == NULL) {
    perror("Falha na abertura do arquivo fonte\n");
    exit(1);
  }
  /* traduz a função Simples */
  funcaoSimples = gera(myfp);
  fclose(myfp);

  /* chama a função */
  res = (*funcaoSimples) (9, 8, 7);  /* passando argumentos apropriados */

  printf("%d\n", res);

  libera(funcaoSimples);
}
