/*
 * cacacomun.c
 *
 *  Created on: 07/02/2014
 *      Author: ernesto
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zlog.h>
#include "cacacomun.h"

int lee_matriz_int_archivo(const char * nombre_archivo,
		unsigned long matrix[MAX_COLUMNAS][MAX_FILAS], int *filas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	long numero = 0;
	char *inicio_cadena_num = NULL;
	char linea[TAM_MAX_LINEA];
	FILE *fp;

	fp = fopen(nombre_archivo, "r");
	if (fp == NULL ) {
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}
	while (fgets(linea, TAM_MAX_LINEA, fp) != NULL ) {
		/* writing content to stdout */
		//puts(str);
		inicio_cadena_num = strtok(linea, " ");
		indice_columnas = 0;
		while (inicio_cadena_num != NULL ) {
			numero = strtol(inicio_cadena_num, NULL, 10);
			matrix[indice_columnas][indice_filas] = (unsigned long) numero;
			inicio_cadena_num = strtok(NULL, " ");
			indice_columnas++;
		}
		indice_filas++;
	}
	*filas = indice_filas;
	fclose(fp);
	return 0;
}

int lee_matriz_long_stdin(unsigned long matrix[MAX_COLUMNAS][MAX_FILAS],
		int *filas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	long numero = 0;
	char *inicio_cadena_num = NULL;
	char linea[TAM_MAX_LINEA];
	char input_usuario[TAM_MAX_LINEA][MAX_FILAS];

	fgets(linea, TAM_MAX_LINEA, stdin);
	sscanf(linea, "%ld %ld", &matrix[0][indice_filas],
			&matrix[1][indice_filas]);
	indice_filas++;
	while (fgets(linea, TAM_MAX_LINEA, stdin)
			&& sscanf(linea, "%ld %ld", &matrix[0][indice_filas],
					&matrix[1][indice_filas]) == 2) {
		indice_filas++;
	}

	*filas = indice_filas;
	return 0;
}

// XXX: http://www.eclipse.org/forums/index.php/t/26571/
// XXX: http://stackoverflow.com/questions/3537542/a-doxygen-eclipse-plugin-with-automatic-code-completion
int lee_matrix_long_stdin(unsigned long matrix[MAX_COLUMNAS][MAX_FILAS],
		int *num_filas, int *num_columnas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	int rc = 0;
	long numero = 0;
	char *siguiente_cadena_numero = NULL;
	char *cadena_numero_actual = NULL;
	char linea[TAM_MAX_LINEA];
	zlog_category_t *c;

	c = zlog_get_category("cacacomun");
	if (!c) {
		printf("get cat fail\n");
		zlog_fini();
		exit(-2);
	}
	zlog_debug(c, "DDDDDDDDDDDentro de leer mierda");
// XXX: http://stackoverflow.com/questions/2195823/reading-unknown-number-of-integers-from-stdin-c
	while (fgets(linea, TAM_MAX_LINEA, stdin)) {
		indice_columnas = 0;
		zlog_debug(c, "transformando linea '%s'", linea);
		cadena_numero_actual = linea;
		for (siguiente_cadena_numero = linea;; siguiente_cadena_numero =
				cadena_numero_actual) {
			zlog_debug(c, "la cadena restante '%s'", cadena_numero_actual);
			numero = strtol(siguiente_cadena_numero, &cadena_numero_actual, 10);
			zlog_debug(c, "el num actual %ld", numero);
			if (cadena_numero_actual == siguiente_cadena_numero) {
				zlog_debug(c, "se llego al final de la cadena");
				break;
			}
			zlog_debug(c, "guardando el numero %ld en fil %d, col %d", numero,
					indice_filas, indice_columnas);
			*(*(matrix + indice_filas) + indice_columnas) = numero;
			indice_columnas++;
		}
		zlog_debug(c, "salio del ciclo de linea");
//		*(*num_columnas + indice_filas) = indice_columnas;
		num_columnas[indice_filas] = indice_columnas;
		zlog_debug(c, "c asigno el numero de columnas");
		indice_filas++;
		zlog_debug(c, "terminada de procesar linea '%s'", linea);
	}

	*num_filas = indice_filas;
	return 0;
}

int imprime_matrix(unsigned long matrix[MAX_COLUMNAS][MAX_FILAS], int num_filas,
		int *num_columnas) {
	int i = 0, j = 0;
	int rc = 0;
	unsigned long numero_actual = 0;
	zlog_category_t *c;

	system("echo 'mierda ' > /tmp/volar.txt");

	c = zlog_get_category("cacacomun");
	if (!c) {
		printf("get cat fail\n");
		zlog_fini();
		exit(-2);
	}
	system("echo 'mierda ' > /tmp/volar1.txt");
	zlog_debug(c, "Me corto los webos");
	system("echo 'mierda ' > /tmp/volar2.txt");

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			numero_actual = *(*(matrix + i) + j);
			if (numero_actual) {
				zlog_debug(c, "en %d %d el num %ld", i, j, numero_actual);
			}
		}
	}

	zlog_debug(c, "El # de filas %d", num_filas);
	for (i = 0; i < num_filas; i++) {
		zlog_debug(c, "en fila %d, columnas %d", i, *(num_columnas + i));
	}

	return 0;
}
