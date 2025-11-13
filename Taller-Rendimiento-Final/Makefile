# Makefile para macOS ARM (Apple M2) con soporte OpenMP
CC = /opt/homebrew/opt/llvm/bin/clang
CFLAGS = -O3 -Wall -Wextra -std=c11 -I/opt/homebrew/opt/llvm/include
LDFLAGS = -L/opt/homebrew/opt/llvm/lib -fopenmp -lm

PROGRAMAS = mmClasicaOpenMP mmClasicaPosix mmClasicaFork mmFilasOpenMP

all: $(PROGRAMAS)

mmClasicaOpenMP: mmClasicaOpenMP.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

mmClasicaPosix: mmClasicaPosix.c
	$(CC) $(CFLAGS) $< -o $@ -lpthread -lm

mmClasicaFork: mmClasicaFork.c
	$(CC) $(CFLAGS) $< -o $@ -lm

mmFilasOpenMP: mmFilasOpenMP.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

clean:
	rm -f $(PROGRAMAS)

