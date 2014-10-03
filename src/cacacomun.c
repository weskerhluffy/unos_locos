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
		tipo_dato matrix[MAX_COLUMNAS_INPUT][MAX_FILAS_INPUT], int *filas) {
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
			matrix[indice_columnas][indice_filas] = (tipo_dato) numero;
			inicio_cadena_num = strtok(NULL, " ");
			indice_columnas++;
		}
		indice_filas++;
	}
	*filas = indice_filas;
	fclose(fp);
	return 0;
}

int lee_matriz_long_stdin(tipo_dato matrix[MAX_COLUMNAS_INPUT][MAX_FILAS_INPUT],
		int *filas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	long numero = 0;
	char *inicio_cadena_num = NULL;
	char linea[TAM_MAX_LINEA];
	char input_usuario[TAM_MAX_LINEA][MAX_FILAS_INPUT];

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
int lee_matrix_long_stdin(tipo_dato matrix[MAX_COLUMNAS_INPUT][MAX_FILAS_INPUT],
		int *num_filas, int *num_columnas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	int rc = 0;
	long numero = 0;
	char *siguiente_cadena_numero = NULL;
	char *cadena_numero_actual = NULL;
	char linea[TAM_MAX_LINEA];

	caca_log_debug("DDDDDDDDDDDentro de leer mierda");
// XXX: http://stackoverflow.com/questions/2195823/reading-unknown-number-of-integers-from-stdin-c
	while (fgets(linea, TAM_MAX_LINEA, stdin)) {
		indice_columnas = 0;
		caca_log_debug("transformando linea '%s'", linea);
		cadena_numero_actual = linea;
		for (siguiente_cadena_numero = linea;; siguiente_cadena_numero =
				cadena_numero_actual) {
			caca_log_debug("la cadena restante '%s'", cadena_numero_actual);
			numero = strtol(siguiente_cadena_numero, &cadena_numero_actual, 10);
			caca_log_debug("el num actual %ld", numero);
			if (cadena_numero_actual == siguiente_cadena_numero) {
				caca_log_debug("se llego al final de la cadena");
				break;
			}
			caca_log_debug("guardando el numero %ld en fil %d, col %d", numero,
					indice_filas, indice_columnas);
			*(*(matrix + indice_filas) + indice_columnas) = numero;
			indice_columnas++;
		}
		caca_log_debug("salio del ciclo de linea");
//		*(*num_columnas + indice_filas) = indice_columnas;
		num_columnas[indice_filas] = indice_columnas;
		caca_log_debug("c asigno el numero de columnas");
		indice_filas++;
		caca_log_debug("terminada de procesar linea '%s'", linea);
	}

	*num_filas = indice_filas;
	return 0;
}

int imprime_matrix(tipo_dato matrix[MAX_COLUMNAS_INPUT][MAX_FILAS_INPUT],
		int num_filas, int *num_columnas) {
	int i = 0, j = 0;
	int rc = 0;
	tipo_dato numero_actual = 0;

	caca_log_debug("Me corto los webos");

	for (i = 0; i < 3; i++) {
		for (j = 0; j < 3; j++) {
			numero_actual = *(*(matrix + i) + j);
			if (numero_actual) {
				caca_log_debug("en %d %d el num %ld", i, j, numero_actual);
			}
		}
	}

	caca_log_debug("El # de filas %d", num_filas);
	for (i = 0; i < num_filas; i++) {
		caca_log_debug("en fila %d, columnas %d", i, *(num_columnas + i));
	}

	return 0;
}

int init_grafo(tipo_dato[3][MAX_FILAS_INPUT], bool
usar_hashes) {

	return (0);
}

nodo *grafo_alloc(grafo_contexto *ctx, int localidades_solicitadas) {
	nodo *inicio_localidades_allocadas = NULL;
	if ((sizeof(ctx->nodos_disponibles) - ctx->localidades_usadas)
			>= localidades_solicitadas) {
		inicio_localidades_allocadas = ctx->nodos_disponibles
				+ localidades_solicitadas;
		ctx->localidades_usadas += localidades_solicitadas;
	}
	return inicio_localidades_allocadas;
}

GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_lineal(grafo_contexto *ctx,
		nodo *nodo_a_buscar, GRAFO_CRITERIOS_ORDENACION criterio_busqueda,
		void *nodos_encontrados) {
	GRAFO_TIPO_RESULTADO_BUSQUEDA tipo_resultado = GRAFO_NADA_ENCONTRADO;

	tipo_resultado = busqueda_lineal_lista_nodos(ctx->inicio, nodo_a_buscar,
			criterio_busqueda, nodos_encontrados, MAX_NODOS);

	return tipo_resultado;
}

GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_lineal_lista_nodos(nodo *inicio,
		nodo *nodo_a_buscar, GRAFO_CRITERIOS_ORDENACION criterio_busqueda,
		void *nodos_encontrados, int limite_nodos_busqueda) {
	int num_nodos_recorridos = 0;
	GRAFO_TIPO_RESULTADO_BUSQUEDA tipo_resultado = GRAFO_NADA_ENCONTRADO;
	nodo *nodo_actual = NULL;
	nodo *nodo_anterior = NULL;
	nodo **nodos_encontrados_int = NULL;

	nodos_encontrados_int = ((nodo **) *nodos_encontrados);

	nodo_actual = inicio;

	while (nodo_actual && num_nodos_recorridos < limite_nodos_busqueda) {
		switch (criterio_busqueda) {
		case GRAFO_VALOR:
			nodo_anterior = nodo_actual;
			if (nodo_actual->valor == nodo_a_buscar->valor) {
				tipo_resultado = GRAFO_NODO_ENCONTRADO;
				break;
			}
			if (nodo_actual->valor >= nodo_a_buscar->valor) {
				tipo_resultado = GRAFO_NODOS_PRE_POST_ENCONTRADOS;
				break;
			}
			nodo_actual = nodo_actual->siguiente_valor;
			break;
		case GRAFO_DISTANCIA:
			nodo_anterior = nodo_actual;
			if (nodo_actual->distancia == nodo_a_buscar->distancia) {
				tipo_resultado = GRAFO_NODO_ENCONTRADO;
				break;
			}
			if (nodo_actual->distancia >= nodo_a_buscar->distancia) {
				tipo_resultado = GRAFO_NODOS_PRE_POST_ENCONTRADOS;
				break;
			}
			nodo_actual = nodo_actual->siguiente_distancia;
			break;
		case GRAFO_INDICE:
			nodo_anterior = nodo_actual;
			if (nodo_actual->indice == nodo_a_buscar->indice) {
				tipo_resultado = GRAFO_NODO_ENCONTRADO;
				break;
			}
			if (nodo_actual->indice >= nodo_a_buscar->indice) {
				tipo_resultado = GRAFO_NODOS_PRE_POST_ENCONTRADOS;
				break;
			}
			nodo_actual = nodo_actual->siguiente_indice;
			break;
		default:
			perror("en busqueda lineal hubo un error culero al buscar");
			break;
		}
		num_nodos_recorridos++;
	}

	if (nodo_actual && num_nodos_recorridos < limite_nodos_busqueda) {
		if (tipo_resultado == GRAFO_NODO_ENCONTRADO) {
			*nodos_encontrados_int = nodo_actual;
		}
		if (tipo_resultado == GRAFO_NODOS_PRE_POST_ENCONTRADOS) {
			switch (criterio_busqueda) {
			case GRAFO_VALOR:
				*nodos_encontrados_int = nodo_actual->anterior_valor;
				*(nodos_encontrados_int + 1) = nodo_actual;
				break;
			case GRAFO_DISTANCIA:
				*nodos_encontrados_int = nodo_actual->anterior_distancia;
				*(nodos_encontrados_int + 1) = nodo_actual;
				break;
			case GRAFO_INDICE:
				*nodos_encontrados_int = nodo_actual->anterior_indice;
				*(nodos_encontrados_int + 1) = nodo_actual;
				break;
			default:
				perror(
						"en busqueda lineal hubo un error culero al asignar resultados");
				break;
			}
		}
	} else {
		if (nodo_anterior) {
			tipo_resultado = GRAFO_NODOS_PRE_POST_ENCONTRADOS;
			switch (criterio_busqueda) {
			case GRAFO_VALOR:
				*nodos_encontrados_int = nodo_anterior->anterior_valor;
				*(nodos_encontrados_int + 1) = nodo_anterior;
				break;
			case GRAFO_DISTANCIA:
				*nodos_encontrados_int = nodo_anterior->anterior_distancia;
				*(nodos_encontrados_int + 1) = nodo_anterior;
				break;
			case GRAFO_INDICE:
				*nodos_encontrados_int = nodo_anterior->anterior_indice;
				*(nodos_encontrados_int + 1) = nodo_anterior;
				break;
			default:
				perror(
						"en busqueda lineal hubo un error culero al asignar resultados sin nodo actual");
				break;
			}
		}
	}
	return tipo_resultado;
}

GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_binaria(grafo_contexto *ctx,
		nodo *nodo_a_buscar, GRAFO_CRITERIOS_ORDENACION criterio_busqueda,
		void *nodos_encontrados) {
	int i = 0;
	GRAFO_TIPO_RESULTADO_BUSQUEDA tipo_resultado = GRAFO_NADA_ENCONTRADO;
	nodo *nodo_actual = NULL;
	nodo *arreglo_nodos_tmp[MAX_NODOS] = { NULL };

	nodo_actual = ctx->inicio;

	do {
		*(arreglo_nodos_tmp + i) = nodo_actual;
		GRAFO_AVANZAR_NODO(nodo_actual, criterio_busqueda);
		i++;
	} while (nodo_actual);

	return tipo_resultado;
}

GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_binaria_recursiva(nodo *inicio,
		nodo *nodo_a_buscar, GRAFO_CRITERIOS_ORDENACION criterio_busqueda,
		void *nodos_encontrados, int indice_inicial, int indice_final) {
	int resultado_comparacion = 0;
	int indice_medio = 0;
	GRAFO_TIPO_RESULTADO_BUSQUEDA tipo_resultado = GRAFO_NADA_ENCONTRADO;
	char cadena1[MAX_TAM_CADENA];
	char cadena2[MAX_TAM_CADENA];
	nodo **nodos_encontrados_int = NULL;
	nodos_encontrados_int = ((nodo **) *nodos_encontrados);
	caca_log_debug("entrando con inicio %d y fin %d", indice_inicial,
			indice_final);

	if ((indice_final - indice_inicial) < 2) {
		caca_log_debug("buscando linealmento, ventana menor a donas");
		tipo_resultado = busqueda_lineal_lista_nodos(inicio, nodo_a_buscar,
				criterio_busqueda, nodos_encontrados,
				indice_final - indice_inicial + 1);
	} else {
		indice_medio = (indice_inicial + indice_final) / 2;
		resultado_comparacion = grafo_comparar_nodos(nodo_a_buscar,
				*(inicio + indice_medio), criterio_busqueda) == 0;
		caca_log_debug("comparando nodo %s con %s (posicion %d), resultado %d",
				nodo_a_cadena(nodo_a_buscar, cadena1),
				nodo_a_cadena(*(inicio + indice_medio)), indice_medio,
				resultado_comparacion);
		if (resultado_comparacion == 0) {
			tipo_resultado = GRAFO_NODO_ENCONTRADO;
			*nodos_encontrados = *(inicio + indice_medio);
		} else {
			if (resultado_comparacion == -1) {
				indice_final = indice_medio - 1;
			} else {
				if (resultado_comparacion == 1) {
					indice_inicial = indice_medio + 1;
				} else {
					perror("What the fuck?? busqueda_binaria_recursiva");
				}
			}
			caca_log_debug("recursividad inicio %d final %d", indice_inicial,
					indice_final);
			tipo_resultado = busqueda_binaria_recursiva(inicio, nodo_a_buscar,
					criterio_busqueda, nodos_encontrados, indice_inicial,
					indice_final);
			caca_log_debug("recursividad resultado %d ", tipo_resultado);
		}
	}

	caca_log_debug("al salir d la funcion %d", tipo_resultado);

	return tipo_resultado;
}

// XXX: http://stackoverflow.com/questions/2679182/have-macro-return-a-value
static inline int grafo_comparar_nodos(nodo *nodo1, nodo *nodo2,
		GRAFO_CRITERIOS_ORDENACION criterio_busqueda) {
	tipo_dato val1 = 0, val2 = 0;
	int resultado_comparacion;
	char cadena1[MAX_TAM_CADENA];
	char cadena2[MAX_TAM_CADENA];

	caca_log_debug("comparando %s con %s", nodo_a_cadena(nodo1, cadena1),
			nodo_a_cadena(nodo2, cadena2));

	switch (criterio_busqueda) {
	case GRAFO_VALOR:
		val1 = nodo1->valor;
		val2 = nodo2->valor;
		break;
	case GRAFO_DISTANCIA:
		val1 = nodo1->distancia;
		val2 = nodo2->distancia;
		break;
	case GRAFO_INDICE:
		val1 = nodo1->indice;
		val2 = nodo2->indice;
		break;
	default:
		perror("en GRAFO_COMPARAR_NODOS %s un error culero al buscar",
				__func__);
		break;
	}
	if (val1 < val2) {
		resultado_comparacion = COMPARACION_MENOR;
	} else {
		if (val1 > val2) {
			resultado_comparacion = COMPARACION_MAYOR;
		} else {
			resultado_comparacion = COMPARACION_IGUAL;
		}
	}
	caca_log_debug("resultado de comparacion %s con %s %d",
			nodo_a_cadena(nodo1, cadena1), nodo_a_cadena(nodo2, cadena2),
			resultado_comparacion);
	return resultado_comparacion;
}

// XXX: http://stackoverflow.com/questions/4867229/code-for-printf-function-in-c
void caca_log_debug(const char *format, ...) {
	va_list arg;
	size_t size;
	size_t i;
	const char *HEADER = "funcion: %s; nivel: %ld 8====D ";
	char formato[MAX_TAM_CADENA + sizeof(HEADER)];
	void *array[MAX_TAM_CADENA];

	size = backtrace(array, MAX_TAM_CADENA);

	strcat(formato, format);

	if (!cacategoria) {
		INIT_ZLOG_CATEGORY(cacategoria, "cacacomun");
	}

	va_start(arg, format);
//	   vfprintf (stdout, format, arg);
	zlog_debug(cacategoria, formato, __func__, size, arg);
	va_end(arg);
}

void init_zlog() {
	int rc = 0;
	rc = zlog_init("/Users/ernesto/workspace/unos_locos/zlog.conf");
	if (rc) {
		printf("init failed\n");
		exit(-1);
	}
}

static inline char *nodo_a_cadena(nodo *node, char *cadena_buffer) {
	sprintf(cadena_buffer, "{valor:%ld,indice:%ld,distancia:%ld}", node->valor,
			node->indice, node->distancia);
	return cadena_buffer;
}
