/*
 * cacacomun.c
 *
 *  Created on: 07/02/2014
 *      Author: ernesto
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cacacomun.h"

int lee_matriz_int_archivo(const char * nombre_archivo,
		unsigned long matrix[MAX_COLUMNAS][MAX_FILAS]) {
	int indice_filas = 0;
	int indice_columnas = 0;
	long numero = 0;
	char *inicio_cadena_num = NULL;
	char linea[TAM_MAX_LINEA];
	FILE *fp;

	fp = fopen("/Users/ernesto/workspace/unos_locos/mierda.txt", "r");
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
	fclose(fp);
	return 0;
}
