/******************************************************************************
* FILE: poisson_paralelo.c
* DESCRIPTION:
*   Programa para la resolución de la ecuación de Poisson en una placa
*   rectangular mediante el método de diferencias finitas, paralelizado con
*   OpenMP.
*   El solver admite soluciones de contorno tipo Dirichlet en las fronteras.
* AUTHOR: Jacobo González Baldonedo
* LAST REVISED: 06/04/18
******************************************************************************/


#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

#define N 20         /* Divisiones eje vertical */
#define M 20         /* Divisiones eje longitudinal */
#define Tol 0.0001    /* Tolerancia en el error de convergencia */
#define NT 3         /* Procesos solicitados */
#define maxit 1000    /* Numero maximo de iteraciones */

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
    for(j = 1; j < M; j++){
      // Actualización de los nodos interiores
      new[i][j] = 0.25 * (old[i+1][j] + old[i-1][j] +
                          old[i][j+1] + old[i][j-1]);
      // Calculo de la diferencia entre iteraciones
      diff = new[i][j] - old[i][j];

      // Actualizacion del maximo
      if (diff < 0)
        diff = -diff;
      if (diff > maxerr)
        maxerr = diff;
    } // for j
  } // for i

  return maxerr;
} // iteration


void main (int argc, char** argv) {

  int nthreads; /* Numero de hilos empleados */
  int tid;      /* Numero de proceso */

  float **new, **old, **tmp;  /* Matrices de valores nodales */
  float maxerr, err, maxerrG; /* Variables de errores */

  float valor; /* Variable para condiciones de contorno */

  int size, remaind; /* Variables de tamaño de subdominios */
  int iter = 0;     /* Numero de iteracion actual */
  int skip = 10;    /* Saltos entre escritura de error */
  FILE *fp; /* Archivo de escritura de solucion */

  int i, j;
  int *starts;  /* Vector con particiones de dominio  */
  int start = 0;
  char str[20];

  printf("Comienzo del solver para la ecuación de Poisson.\n\n");

  // Solicitamos el numero de hilos deseados
  omp_set_num_threads(NT);

  // Alojaomos las matrices de valores nodales
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



  // Generamos el vector con particiones de dominio
  starts = (int *)calloc(NT + 1, sizeof(int));

  starts[0] = 1;
  for (int i = 1; i < NT + 1; i++){
    remaind = (N - 1) % NT;
    size = (N - 1 - remaind) / NT;
    if ((i-1) < remaind)
      size = size + 1;

    starts[i] = size + starts[i-1];
  }


  // Comienzo de la region paralela
  #pragma omp parallel \
  private(start, nthreads, tid) \
  shared(old, new) \
  reduction(max: maxerrG)

  {
    tid = omp_get_thread_num();
    nthreads = omp_get_num_threads();

    if (tid == 0)
      printf("Ejecutando el programa con %d hilos.\n\n", nthreads);

    if (nthreads != NT){
      printf("ERROR: Número de hilos inadecuado.\n");
    }
    maxerrG = iteration(old, new, starts[tid], starts[tid+1]);

  } // Fin region paralela

  printf("Comienzo de las iteraciones: \n");
  printf("---------------------------\n");

  /* Iteraciones hasta convergencia */
  while (maxerrG > Tol){
    maxerrG = 0.0;

    tmp = new;
    new = old;
    old = tmp;


    // Comienzo de la region paralela
    #pragma omp parallel \
    private(nthreads, tid) \
    shared(old, new, iter) \
    reduction(max: maxerrG)
     {

       tid = omp_get_thread_num();

       maxerrG = iteration(old, new, starts[tid], starts[tid+1]);

    } // Fin region paralela

    iter +=1;

    if ((iter % skip) == 0)
      printf("ITERACION: %d    ERROR: %f\n",iter, maxerrG);

    if (iter > maxit){
      printf("Máximo número de iteraciones alcanzado\n");
      break;
    }


  } //while maxerrG

  printf("---------------------------------------------\n");
  printf("Fin de las iteraciones\n");
  printf("---------------------------------------------\n\n");

  if (iter <= maxit){
    printf("SOLUCION ALCANZADA (en %d iteraciones)\n", iter);
  }
  printf("Error final: %f \n\n", maxerrG);


  /* Escritura a archivo */

  sprintf(str, "Solution.Txt");
  fp = fopen(str,"w");

  if (fp == NULL){
    printf("ERROR: ARCHIVO NULL\n");
  } else{
  printf("Escribiendo archivo de salida...\n\n");
  }



  for (i = 0; i < M+1; i++){
    for (j = 0; j < N+1; j++){
      fprintf(fp, "%6.4f\t", new[i][j]);
      //printf("%6.4f\n", new[i][j]);
    } // for j
    fprintf(fp, "\n");
  } //for i

  printf("Solución escrita en ");
  printf(str);
  printf("\n");

  fclose(fp);
  printf("\nFINALIZADO\n");

} // main
