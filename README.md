Descripción General del Proyecto Este laboratorio implementa y compara distintas versiones del algoritmo clásico de multiplicación de matrices, evaluando su comportamiento bajo distintos modelos de paralelismo: Pthreads (mmClasicaPosix.c)
OpenMP clásico (mmClasicaOpenMP.c)

OpenMP por bloques de filas (mmFilasOpenMP.c)

Procesos con fork() (mmClasicaFork.c) El objetivo es estudiar el rendimiento, speedup, eficiencia y escalabilidad de cada enfoque en dos entornos: Máquina Virtual Linux (Intel Xeon virtualizado)

MacBook Air M2 (ARM64, ejecución nativa) Para garantizar mediciones consistentes se emplea además un script de automatización en Perl (Lanzador.pl) que ejecuta múltiples repeticiones, captura tiempos, genera CSV y calcula estadísticas. 2. Estructura del Repositorio ├── mmClasicaPosix.c ├── mmClasicaOpenMP.c ├── mmClasicaFork.c ├── mmFilasOpenMP.c ├── Lanzador.pl ├── Makefile ├── data/
└── README.md

Requisitos del Sistema Linux (Ubuntu 22.04 o similar) gcc con soporte para OpenMP
sudo apt install build-essential gcc g++ make sudo apt install libomp-dev

Perl

sudo apt install perl

macOS (Apple Silicon / ARM64) El compilador Clang de Apple NO soporta OpenMP. Es necesario instalar LLVM: brew install llvm brew install libomp

Y agregar al PATH (si no lo hace automáticamente): export PATH="/opt/homebrew/opt/llvm/bin:$PATH" export LDFLAGS="-L/opt/homebrew/opt/llvm/lib" export CPPFLAGS="-I/opt/homebrew/opt/llvm/include" 4. Compilación del Proyecto Todo se compila con el Makefile incluido. En Linux make

En macOS M2 El Makefile ya debe incluir estas variables: CC = /opt/homebrew/opt/llvm/bin/clang CFLAGS = -O3 -Wall -Wextra -std=c11 -I/opt/homebrew/opt/llvm/include LDFLAGS = -L/opt/homebrew/opt/llvm/lib -fopenmp -lm

Luego simplemente ejecutas: make

Ejecutar los Programas Manualmente Formato general ./ejecutable N nHilos
Ejemplos: ./mmClasicaPosix 512 4 ./mmClasicaOpenMP 1024 8 ./mmClasicaFork 256 2 ./mmFilasOpenMP 512 4

Cada ejecución imprime un tiempo en formato parseable: time_ms=123.456 6. Ejecución Automática y Generación de Datos (Uso del script Lanzador.pl) El script ejecuta todas las combinaciones de: tamaños de matriz

número de hilos

ejecutables

repeticiones

y guarda los resultados en archivos CSV dentro de /data

6.1. Dar permisos: chmod +x Lanzador.pl

6.2. Ejecutarlo ./Lanzador.pl

6.3. Output del script Se creará automáticamente: data/ ├── mmClasicaPosix_128_4.csv ├── mmClasicaOpenMP_1024_8.csv ├── ...

Cada archivo contiene: tamaño de matriz

número de hilos

número de repetición

tiempo en ms

promedio

desviación estándar

Ejemplo de línea final del CSV:

avg_ms=247.383

std_ms=14.912

Replicar el Experimento Completo

Clonar el repositorio git clone <URL_DEL_REPO> cd

Instalar dependencias Linux → gcc + libomp

macOS → brew + llvm

Compilar: make

Verificar ejecutables: ls | grep mm

Ejecutar pruebas manuales o automáticas: ./mmClasicaOpenMP 256 4 ./Lanzador.pl

Analizar resultados: En /data, abrir archivos CSV y revisar: tiempo promedio

desviación estándar

speedup = T1 / Tp

eficiencia = speedup / p 8. Resultados y Observaciones Clave El paralelismo NO mejora el rendimiento en matrices pequeñas Porque el costo de crear hilos/procesos supera la carga útil.

A partir de 256×256 el paralelismo empieza a ser útil, especialmente con OpenMP.

OpenMP escaló mejor que Pthreads y mucho mejor que fork() fork() tiene alto overhead y poca eficiencia.

MacBook Air M2 superó claramente a la VM Linux en la mayoría de pruebas Debido a ausencia de virtualización y mejor jerarquía de memoria.

La VM Linux solo superó a macOS en algunos escenarios con OpenMP por filas, donde el host físico asigna más recursos reales por detrás del hipervisor. 9. Autores Andrés Loreto Quiros Santiago Hernández Curso: Sistemas Operativos Profesor: John Corredor
