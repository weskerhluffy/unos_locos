/*
 * cacacomun.h
 *
 *  Created on: 07/02/2014
 *      Author: ernesto
 */

#ifndef CACACOMUN_H_
#define CACACOMUN_H_

//XXX: http://gnupdf.org/manuals/gnupdf-hg-manual/html_node/Using-gdb-to-debug-check-tests.html

#define TAM_MAX_LINEA 1024
#define TAM_MAX_NUMERO 128
#define MAX_FILAS 256
#define MAX_COLUMNAS 256

int lee_matriz_int_archivo(const char * nombre_archivo,
		unsigned long matrix[MAX_COLUMNAS][MAX_FILAS], int *filas);

int lee_matriz_long_stdin(unsigned long matrix[MAX_COLUMNAS][MAX_FILAS],
		int *filas);

// XXX: http://stackoverflow.com/questions/5724171/passing-an-array-by-reference
/**
 *
 * @param matrix
 * @param num_filas Regresa el num de filas .Debe ser inicializado externo.
 * @param num_columnas Regresa de cada fila el num de columnas. Debe ser inicializado externo. Debe ser al menos de tama�o ::MAX_FILAS.
 * @return
 */
int lee_matrix_long_stdin(unsigned long matrix[MAX_COLUMNAS][MAX_FILAS],
		int *num_filas, int *num_columnas);

int imprime_matrix(unsigned long matrix[MAX_COLUMNAS][MAX_FILAS], int num_filas,
		int *num_columnas);

#endif /* CACACOMUN_H_ */
