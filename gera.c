/*Felipe Maia Meiga 2011460 3WA*/
/*João Victor Godinho Woitschach 2011401 3WB*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#include "gera.h"

#define MAX 128

#define FALSE 0
#define TRUE 1




static void error (const char *msg, int line) {
    fprintf(stderr, "erro %s na linha %d\n", msg, line);
    exit(EXIT_FAILURE);
}


unsigned char jump_address(int linha[], int n, unsigned char code[],int qg){
 
  int x = qg + 1;
  unsigned char* address_next_jump = (code+x); // endereco da proxima instrucao depois de jump
  int soma = 0;
  int j = 0;

  for(j=0; j<(n-1);j++){
    soma+=linha[j];
  }
  soma++; //garantir o primeiro byte da linha desejada

  unsigned char* address_destiny = (code+ soma); // endereco destino

 
  return (address_destiny - address_next_jump);
}




funcp gera(FILE *arq)
{
    int c, line = 1, j = 8;

    //vetor de endereços de %rbps e cia
   
    unsigned char mpEax[] = {0xfc,0xf8, 0xf4, 0xf0, 0xec}; //move algo do %rbp para o %eax //to fix
    static const unsigned char param[] = {0xf8,0xf0,0xd0}; //array de movl %e?x, %eax

    int linha[30];
    int qgs[30]; // array de indices de code[] a serem trocados
    int destinos[30];

    int z = 0; //index para linha
    int q = 0; //index de qgs
    int d = 0; //index para destinos

    __intptr_t pagesize = sysconf(_SC_PAGE_SIZE);
    unsigned char *code = (unsigned char*)malloc(sizeof(char) * MAX);

#define PAGE_START(p) ((__intptr_t)(p) & ~(pagesize-1))
#define PAGE_END(p) (((__intptr_t)(p) + pagesize-1) & ~(pagesize-1))

    mprotect((void *)PAGE_START(code),
      PAGE_END(code+sizeof(char)*MAX) - PAGE_START(code),
      PROT_READ | PROT_WRITE | PROT_EXEC);

    if(code == NULL)
    {
        printf("Erro na alocação de memoria!\n");
        exit(1);
    }

    //pushq e movq
    code[0] = 0x55;
    code[1] = 0x48;
    code[2] = 0x89;
    code[3] = 0xe5;
    //pilha de tamanaho 16 (0x10)
    code[4] = 0x48;
    code[5] = 0x83;
    code[6] = 0xec;
    code[7] = 0x20;

    while ((c = fgetc(arq)) != EOF)
    {
        switch (c)
        {
            case 'r':
            {
                char var0;
                int idx0;
                int provi = 0;

                if(fscanf(arq, "et %c%d", &var0, &idx0) != 2)
                {
                    error("comando invalido", line);
                }

                printf("%d ret %c%d\n", line, var0, idx0);

                if(var0 == '$')
                {
                    code[j] = 0xb8;
                    j++;

                    *(int*)(code+j) = idx0;
                    j+=4;
                   
                    provi += 5;

                }//DONE

                else if(var0 == 'v')
                {
                   
                    code[j] = 0x89;
                    j++;
                    code[j] = 0x45;
                    j++;
                    code[j] = mpEax[idx0-1];
                    j++;
                    provi += 3;



                }//DONE

                else if(var0 == 'p')
                {
                    error("comando invalido", line);
                }

                code[j] = 0xc9; //leave
                j++;
                code[j] = 0xc3; //ret
                j++;

                provi += 2;

                linha[z] = provi;
                z++;


                break;
            }//DONE

            case 'v':
            {
                int idx0, idx1; //numero depois do "v"
                int provi = 0;

                char var0 = c; //to fix
                char c0, var1;

                if(fscanf(arq,"%d %c", &idx0, &c0) != 2){
               
                    error("comando invalido", line);
                }

                if(c0 == '<') /*Atribuição*/
                {
                 
                  if(fscanf(arq," %c%d", &var1,&idx1)!=2){
                   
                    error("comando invalido", line);


                  }




                    if(var1 == '$')
                    {
                        code[j] = 0xb8;
                        j++;

                        *(int*)(code+j) = idx1; //dessa forma, conseguimos colocar o idx1 na code sem problemas
                        j+=4; //localização da próxima linha vazia

                        provi += 5;
                       

                    }//DONE

                    else if(var1 == 'p'){
                        code[j] = 0x89;
                        j++;

                       
                        code[j] = param[idx1-1];  //atribui o e?x respectivo para a movimentacao para eax
                        j++;

                        provi +=2;
                       



                    }//DONE

                    else if(var1 == 'v'){
                      code[j] = 0x8b;  
                      j++;
                      code[j] = 0x45;
                      j++;

                      code[j] = mpEax[idx1-1]; //movimenta o conteudo da variavel local da stack para eax
                      j++;

                      provi += 3;

                   

                    } //DONE

                    code[j] = 0x89; //comeco do movl do eax para a stack
                    j++;
                    code[j] = 0x45;
                    j++;


                    code[j] = mpEax[idx0-1]; // faremos isso para garantir que os elementos do vetor sejam selecionados de forma correta, ja que o elemento na posição 0 é o -4(%rbp)
                    j++;

                    provi += 3;

                   

                    printf("%d %c%d %c %c%d\n", line, var0, idx0, c0, var1, idx1);
                }//DONE

                else if(c0 == '=') //Operação aritimetica //to fix
                {
                    char var2, op;
                    int idx2, idx1;
                   
                 

                    if(fscanf(arq, " %c%d %c %c%d", &var1, &idx1, &op, &var2, &idx2) != 5)
                    {
                        error("comando invalido", line);
                    }

                    if(var1 == 'v'){

                      code[j] = 0x8b;
                      j++;
                      code[j] = 0x45;
                      j++;

                      code[j] = mpEax[idx1-1];
                      j++;

                      provi += 3;




                     
                    } //var 1 variavel
                    else if(var1 == '$'){ // var1 const
                     
                      code[j] = 0xb8;
                      j++;

                      *(int*)(code+j) = idx1;
                      j+=4;

                      provi +=5;

                    }


                    if(op == '+'){
                      if(var2 == 'v'){
                        code[j] = 0x03;
                        j++;
                        code[j] = 0x45;
                        j++;

                        code[j] = mpEax[idx2-1];
                        j++;
                        provi += 3;

                      }

                      else if(var2 == '$'){
                        code[j] = 0x05;
                        j++;
                       

                        *(int*)(code+j) = idx2;
                        j+=4;

                        provi += 5;

                      }

                    }//to fix/  const

                    else if(op == '-'){
                      if(var2 == 'v'){

                        code[j] = 0x2b;
                        j++;
                        code[j] = 0x45;
                        j++;
                        code[j] = mpEax[idx2-1];
                        j++;

                        provi +=3;
                      }
                      else if(var2 == '$'){

                        code[j] = 0x2d;
                        j++;
                       
                        *(int*)(code+j) = idx2;;
                        j+=4;

                        provi += 5;
                      }//DONE

                     
                    }//to fix/  const

                    else if(op == '*'){
                      if(var2 == 'v'){

                        code[j] = 0x0f;
                        j++;
                        code[j] = 0xaf;
                        j++;
                        code[j] = 0x45;
                        j++;

                        code[j] = mpEax[idx2-1];
                        j++;
                        provi += 4;
                      }

                      else if(var2 == '$'){

                        code[j] = 0x69;
                        j++;
                        code[j] = 0xc0;
                        j++;
                        *(int*)(code+j) = idx2;
                        j+=4;

                        provi += 6;
                      }
                    }//to fix/  const

                    printf("%d %c%d = %c%d %c %c%d\n", line, var0, idx0, var1, idx1, op, var2, idx2);

                    code[j] = 0x89;
                    j++;
                    code[j] = 0x45;
                    j++;
                    code[j] = mpEax[idx0-1];
                    j++;

                    provi+=3;
             
                }//DONE
               
                linha[z] = provi;
                z++;


                break;
            }//DONE

            case 'i':
            {
                int provi = 0;
                char var0;
                int idx0, n;

                if(fscanf(arq, "flez %c%d %d", &var0, &idx0, &n) != 3)
                {
                    error("comando invalido", line);
                }

                printf("%d iflez %c%d %d\n", line, var0, idx0, n);
               
                code[j] = 0xb8;
                j++;
               
                code[j] = 0x00;
                j++;
               
                code[j] = 0x00;
                j++;
               
                code[j] = 0x00;
                j++;
               
                code[j] = 0x00;
                j++;

                code[j] = 0x3b;
                j++;
                code[j] = 0x45;
                j++;
                code[j] = mpEax[idx0-1];
                j++;

                code[j] = 0x7f;
                j++;

                //aqui eu do uns pega no qg
                qgs[q] = j;
                q++;
                destinos[d] = n;
                d++;

                code[j] = 0x00; //não importa o que colocarmos neste endereço, pois posteriormente ele será modificado
                j++;

                provi += 8;
                linha[z] = provi;
                z++;

                break;
            }//to fix

            default: error("comando desconhecido", line);
        }

        line++;

        fscanf(arq, " ");
    }

    int k = 0;
    unsigned char ender;

    for(k=0 ; k<q ;k++){
      ender = jump_address(linha,destinos[k],code,qgs[k]);
      code[qgs[k]] = ender;

    }



    return code;
}

void libera(void *pf)
{
    unsigned char *ptr = (unsigned char*) pf;

    free(ptr);
}