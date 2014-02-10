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

#endif /* CACACOMUN_H_ */
