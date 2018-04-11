// programa en secuencial

#include <stdio.h>
#include <stdlib.h>
#define N 20
#define M 20
#define Tol 0.0001

float **matrix(int m, int n) {
  int i;
  float **ptr;
  ptr = (float **)calloc(m, sizeof(float *));

  for (i = 0; i < m; i++){
    ptr[i] = (float *)calloc(n, sizeof(float));
  }
  return (ptr);
}

float iteration(float **old, float **new, int start, int finish) {
  float diff, maxerr = 0;
  int i, j;

  for (i = start; i < finish; i++){
    for(j = 1; j < N; j++){
      new[i][j] = 0.25 * (old[i+1][j] + old[i-1][j] +
                          old[i][j+1] + old[i][j-1]);
      diff = new[i][j] - old[i][j];
      if (diff < 0)
        diff = -diff;
      if (diff > maxerr)
        maxerr = diff;
    } // for j
  } // for i
  return maxerr;
} // iteration

void matrix2file(float **A, int m, int n, int num){
  //sprintf(str, "Solution%d.Txt", nid);
  FILE *fp;
  char name[20];

  sprintf(name, "Matriz%d.Txt",num);

  fp = fopen(name,"w");

  if (fp == NULL){
    printf("ARCHIVO: NULL\n");
  } else{
  //printf("ARCHIVO ABIERTO\n");
  }
  //printf(str);
  //printf("\n");

  for (int i = 0; i < m; i++){
    for (int j = 0; j < n; j++){
      fprintf(fp, "%6.4f\t", A[i][j]);
      //printf("%6.4f\n", new[i][j]);
    } // for j
    fprintf(fp, "\n");
  } //for i

  //printf("ARCHIVO ESCRITO\n");
  fclose(fp);
  //printf("\nFINALIZADO\n");
}

void main (int argc, char** argv){
  float **new, **old, **tmp, maxerr, err, maxerrG;
  int noprocs, nid, size, i, j;
  char str[20];
  float valor;
  FILE *fp;
  //size = N;
  nid = 1;

  new = matrix(M+1, N+1);
  old = matrix(M+1, N+1);


  /* --------------------------- */
  /* Inicializamos las fronteras */
  /* --------------------------- */

  // Frontera IZQUIERDA
  valor = 1.0;
  for (i = 0; i < M; i++){
    new[i][0] = old[i][0] = valor;
  } // for i

  // Frontera DERECHA
  valor = 3.0;
  for (i = 0; i < M; i++){
    new[i][N] = old[i][N] = valor;
  } // for i

  // Frontera SUPERIOR
  valor = 2.0;
  for (i = 0; i < N + 1; i++){
    new[0][i] = old[0][i] = valor;
  } // for i

  // Frontera INFERIOR
  valor = 4.0;
  for (i = 0; i < N + 1; i++){
    new[M][i] = old[M][i] = valor;
  } // for i

  //sprintf(str, "Inicial.Txt");
  matrix2file(new, M+1, N+1,0);

  maxerrG = iteration(old, new, 2, M);

  printf("%6.4f\n", maxerrG);

  while (maxerrG > Tol){
    tmp = new;
    new = old;
    old = tmp;

    maxerrG = iteration(old, new, 1, M);

  } //while maxerrG

  printf("SOLUCION ALCANZADA\n");


  /* Escritura a archivo */

  sprintf(str, "Szolution%d_sq.Txt", nid);
  fp = fopen(str,"w");

  if (fp == NULL){
    printf("ARCHIVO: NULL\n");
  } else{
  printf("ARCHIVO ABIERTO\n");
  }
  printf(str);
  printf("\n");


  for (i = 0; i < M+1; i++){
    for (j = 0; j < N+1; j++){
      fprintf(fp, "%6.4f\t", new[i][j]);
      //printf("%6.4f\n", new[i][j]);
    } // for j
    fprintf(fp, "\n");
  } //for i

  printf("ARCHIVO ESCRITO\n");
  fclose(fp);
  printf("\nFINALIZADO\n");

} // main
