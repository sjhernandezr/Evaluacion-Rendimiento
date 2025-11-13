/*#######################################################################################
#  Fecha: 4 de noviembre de 2025
#  Autor: Andrés Loreto
#  Programa:
#      Multiplicación de Matrices algoritmo clásico
#  Versión:
#      Paralelismo con Hilos Pthreads (POSIX)
######################################################################################*/

#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>

/* Matrices globales para acceso por los hilos */
static double *matrixA, *matrixB, *matrixC;

/* Parámetros por hilo */
typedef struct {
    int idH;       /* id del hilo [0..nH-1] */
    int nH;        /* número total de hilos  */
    int N;         /* dimensión de la matriz */
} parametros_t;

/* Cronometría */
static struct timeval inicio, fin;

static inline void InicioMuestra(void) {
    gettimeofday(&inicio, NULL);
}

static inline void FinMuestra(void) {
    gettimeofday(&fin, NULL);
    long secs  = fin.tv_sec  - inicio.tv_sec;
    long usecs = fin.tv_usec - inicio.tv_usec;
    double ms  = (double)(secs * 1000.0) + (double)usecs / 1000.0;
    /* Formato estable y parseable para el script */
    printf("time_ms=%.3f\n", ms);
}

/* Inicialización de matrices con valores en rango (1..5) y (5..9) */
static void iniMatrix(double *m1, double *m2, int D) {
    for (int i = 0; i < D * D; i++) {
        m1[i] = 1.0 + (double)rand() / RAND_MAX * (5.0 - 1.0);
        m2[i] = 5.0 + (double)rand() / RAND_MAX * (9.0 - 5.0);
    }
}

/* Impresión compacta para tamaños pequeños (útil para verificación) */
static void impMatrix(const double *matriz, int D) {
    if (D < 9) {
        for (int i = 0; i < D * D; i++) {
            if (i % D == 0) puts("");
            printf(" %.2f ", matriz[i]);
        }
        puts("\n>-------------------->");
    }
}

/* Trabajo del hilo: multiplica un bloque de filas de A por B y escribe en C */
static void *multiMatrix(void *arg) {
    parametros_t *par = (parametros_t *)arg;

    const int N  = par->N;
    const int nH = par->nH;
    const int id = par->idH;

    /* Reparto exacto de filas aun cuando N no sea múltiplo de nH */
    const int filaI = (N * id) / nH;        /* inclusive */
    const int filaF = (N * (id + 1)) / nH;  /* exclusive */

    for (int i = filaI; i < filaF; i++) {
        for (int j = 0; j < N; j++) {
            const double *pA = matrixA + (i * N);
            const double *pB = matrixB + j;
            double suma = 0.0;
            for (int k = 0; k < N; k++, pA++, pB += N) {
                suma += (*pA) * (*pB);
            }
            matrixC[i * N + j] = suma;
        }
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Uso:\n  %s tamMatriz numHilos\n", argv[0]);
        return 1;
    }

    const int N         = atoi(argv[1]);
    const int n_threads = atoi(argv[2]);
    if (N <= 0 || n_threads <= 0) {
        fprintf(stderr, "Error: tamMatriz y numHilos deben ser > 0\n");
        return 1;
    }

    /* Reserva de memoria */
    matrixA = (double *)calloc((size_t)N * N, sizeof(double));
    matrixB = (double *)calloc((size_t)N * N, sizeof(double));
    matrixC = (double *)calloc((size_t)N * N, sizeof(double));
    if (!matrixA || !matrixB || !matrixC) {
        fprintf(stderr, "Fallo al reservar memoria\n");
        free(matrixA); free(matrixB); free(matrixC);
        return 1;
    }

    /* Semilla para números aleatorios (determinística por ejecución si lo prefieres) */
    srand((unsigned)time(NULL));

    iniMatrix(matrixA, matrixB, N);
    impMatrix(matrixA, N);
    impMatrix(matrixB, N);

    /* Lanzamiento de hilos */
    pthread_t    *threads = (pthread_t *)malloc(sizeof(pthread_t) * (size_t)n_threads);
    parametros_t *params  = (parametros_t *)malloc(sizeof(parametros_t) * (size_t)n_threads);
    if (!threads || !params) {
        fprintf(stderr, "Fallo al reservar estructuras de hilos\n");
        free(threads); free(params);
        free(matrixA); free(matrixB); free(matrixC);
        return 1;
    }

    InicioMuestra();

    for (int j = 0; j < n_threads; j++) {
        params[j].idH = j;
        params[j].nH  = n_threads;
        params[j].N   = N;
        if (pthread_create(&threads[j], NULL, multiMatrix, (void *)&params[j]) != 0) {
            fprintf(stderr, "Error creando hilo %d\n", j);
            /* Espera los creados y sale */
            for (int k = 0; k < j; k++) pthread_join(threads[k], NULL);
            free(threads); free(params);
            free(matrixA); free(matrixB); free(matrixC);
            return 1;
        }
    }

    for (int j = 0; j < n_threads; j++) {
        pthread_join(threads[j], NULL);
    }

    FinMuestra();  /* <-- fuera del for, explícito y sin warnings */

    impMatrix(matrixC, N);

    /* Limpieza */
    free(threads);
    free(params);
    free(matrixA);
    free(matrixB);
    free(matrixC);

    return 0;
}
