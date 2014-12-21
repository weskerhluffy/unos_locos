/*

 * cacacomun.c
 *
 *  Created on: 07/02/2014
 *      Author: ernesto
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <execinfo.h>
#include <stdarg.h>
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
	char linea[TAM_MAX_LINEA];

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

int imprime_matrix(void *matrix, int num_filas, int *num_columnas,
		int num_columnas_fijo) {
	bool es_array = falso;
	int i = 0, j = 0;
	int num_columnas_actual = 0;
	tipo_dato numero_actual = 0;
	tipo_dato *matrix_array = NULL;
	tipo_dato **matrix_pointer = NULL;

	matrix_array = matrix;
	matrix_pointer = matrix;

	caca_log_debug("Me corto los webos");

	caca_log_debug("pero q mierda void pointer %p", matrix);

	es_array = !from_stack(matrix);

	if (!num_columnas && !num_columnas_fijo) {
		caca_log_debug("No mames no ay nada con q trabajar");
	}

	num_columnas_actual = num_columnas_fijo ? num_columnas_fijo : 0;

	for (i = 0; i < num_filas; i++) {
		if (num_columnas) {
			num_columnas_actual = *(num_columnas + i);
		}
		for (j = 0; j < num_columnas_actual; j++) {
			if (es_array) {
				numero_actual = *(matrix_array + i * num_columnas_actual + j);
			} else {
				numero_actual = *(*(matrix_pointer + i) + j);
			}
			if (numero_actual) {
				caca_log_debug("en %d %d el num %ld", i, j, numero_actual);
			}
		}
	}

	caca_log_debug("El # de filas %d", num_filas);
	if (num_columnas) {
		for (i = 0; i < num_filas; i++) {
			caca_log_debug("en fila %d, columnas %d", i, *(num_columnas + i));
		}
	} else {
		caca_log_debug("num  d columnas para todas las filas %d",
				num_columnas_fijo);
	}

	return 0;
}

/*int init_grafo(tipo_dato[3][MAX_FILAS_INPUT], grafo_contexto *ctx,
 int num_filas, bool
 usar_hashes) */
int init_grafo(void *matrix, int num_filas, grafo_contexto *ctx,
		bool usar_hashes, bool relaciones_bidireccionales) {
	int i = 0;
	GRAFO_TIPO_RESULTADO_BUSQUEDA tipo_resultado = GRAFO_NADA_ENCONTRADO;
	tipo_dato distancia_actual = 0, indice_nodo_origen_actual = 0,
			indice_nodo_destino_actual = 0;
// XXX: http://stackoverflow.com/questions/11454376/cast-void-pointer-to-integer-array
	tipo_dato (*matrix_int)[3] = NULL;
	nodo nodo_tmp;
	nodo *nodo_origen_actual = NULL;
	nodo *nodo_destino_actual = NULL;
	nodo *nodos_encontrados[2] = { NULL };

	matrix_int = (tipo_dato (*)[3]) matrix;
	caca_log_debug("despues de acer el cacast");

	memset((void * )ctx, 0, sizeof(grafo_contexto));
	caca_log_debug("despues de acer el set de ceros");
	memset((void * )ctx->matrix_distancias, MAX_VALOR,
			sizeof(ctx->matrix_distancias));

	memset((void * )&nodo_tmp, 0, sizeof(nodo));

	caca_log_debug("despues de inicializar contexto %s", "mierdadsdsadsa");

	imprime_matrix(matrix, num_filas, NULL, 3);

	for (i = 0; i < num_filas; i++) {
		indice_nodo_origen_actual = *(*(matrix_int + i) + GRAFO_NODO_ORIGEN);
		indice_nodo_destino_actual = *(*(matrix_int + i) + GRAFO_NODO_DESTINO);
		distancia_actual = *(*(matrix_int + i) + GRAFO_DISTANCIA_ENTRE_NODOS);

		caca_log_debug("En fila %d, origen %ld, destino %ld, distancia %ld", i,
				indice_nodo_origen_actual, indice_nodo_destino_actual,
				distancia_actual);

		nodo_tmp.indice = indice_nodo_origen_actual;

		tipo_resultado = busqueda_binaria(ctx->inicio, &nodo_tmp,
				GRAFO_PRINCIPAL, (void *) &nodos_encontrados);

		switch (tipo_resultado) {
		case GRAFO_NODO_ENCONTRADO:
			nodo_origen_actual = *nodos_encontrados;
			break;
		case GRAFO_NODOS_PRE_POST_ENCONTRADOS:
			nodo_origen_actual = grafo_nodo_alloc(ctx, 1);
			nodo_origen_actual->indice = indice_nodo_origen_actual;
			caca_log_debug("anadiendo %p a lista principal, entre %p y %p",
					nodo_origen_actual, *nodos_encontrados,
					*(nodos_encontrados + 1));
			grafo_insertar_nodo(*nodos_encontrados, *(nodos_encontrados + 1),
					nodo_origen_actual, GRAFO_PRINCIPAL);
//			if (*(nodos_encontrados + 1) == ctx->inicio) {
			if (!*nodos_encontrados) {
				*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
						GRAFO_PRINCIPAL) = *grafo_apuntador_num_nodos_asociados(
						ctx->inicio, GRAFO_PRINCIPAL) + 1;
				ctx->inicio = nodo_origen_actual;
			} else {
				(*grafo_apuntador_num_nodos_asociados(ctx->inicio,
						GRAFO_PRINCIPAL))++;
			}
			*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
					GRAFO_INDICE) = -1;
			*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
					GRAFO_DISTANCIA) = -1;
			break;
		case GRAFO_NADA_ENCONTRADO:
			nodo_origen_actual = grafo_nodo_alloc(ctx, 1);
			nodo_origen_actual->indice = indice_nodo_origen_actual;
			ctx->inicio = nodo_origen_actual;
			*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
					GRAFO_PRINCIPAL) = 0;
			*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
					GRAFO_INDICE) = -1;
			*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
					GRAFO_DISTANCIA) = -1;
			caca_log_debug("Se inicia la lista de nodos en ctx");
			break;
		default:
			perror("no se pudo configurar ctx d graph");
			exit(1);
			break;
		}

		nodo_destino_actual = grafo_nodo_alloc(ctx, 1);
		nodo_destino_actual->distancia = distancia_actual;
		nodo_destino_actual->indice = indice_nodo_destino_actual;

		caca_log_debug("insertando nodo %p en lista de indices de %p",
				nodo_destino_actual, nodo_origen_actual);
		grafo_anadir_nodo(nodo_origen_actual, nodo_destino_actual,
				GRAFO_INDICE);
		caca_log_debug("insertando nodo %p en lista de distancias de %p",
				nodo_destino_actual, nodo_origen_actual);
		grafo_anadir_nodo(nodo_origen_actual, nodo_destino_actual,
				GRAFO_DISTANCIA);

		if (usar_hashes) {
			ctx->matrix_distancias[indice_nodo_origen_actual][indice_nodo_destino_actual] =
					distancia_actual;
			*(ctx->referencias_nodos_por_indice + indice_nodo_origen_actual) =
					nodo_origen_actual;
		}

		if (relaciones_bidireccionales) {
			caca_log_debug("Dando de alta relacion espejo-pendejoº");

			nodo_tmp.indice = indice_nodo_destino_actual;

			tipo_resultado = busqueda_binaria(ctx->inicio, &nodo_tmp,
					GRAFO_PRINCIPAL, (void *) &nodos_encontrados);

			switch (tipo_resultado) {
			case GRAFO_NODO_ENCONTRADO:
				nodo_origen_actual = *nodos_encontrados;
				break;
			case GRAFO_NODOS_PRE_POST_ENCONTRADOS:
				nodo_origen_actual = grafo_nodo_alloc(ctx, 1);
				nodo_origen_actual->indice = indice_nodo_destino_actual;
				caca_log_debug("anadiendo %p a lista principal, entre %p y %p",
						nodo_origen_actual, *nodos_encontrados,
						*(nodos_encontrados + 1));
				grafo_insertar_nodo(*nodos_encontrados,
						*(nodos_encontrados + 1), nodo_origen_actual,
						GRAFO_PRINCIPAL);
				if (!*nodos_encontrados) {
					*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
							GRAFO_PRINCIPAL) =
							*grafo_apuntador_num_nodos_asociados(ctx->inicio,
									GRAFO_PRINCIPAL) + 1;
					ctx->inicio = nodo_origen_actual;
				} else {
					(*grafo_apuntador_num_nodos_asociados(ctx->inicio,
							GRAFO_PRINCIPAL))++;
				}
				*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
						GRAFO_INDICE) = -1;
				*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
						GRAFO_DISTANCIA) = -1;
				break;
			case GRAFO_NADA_ENCONTRADO:
				nodo_origen_actual = grafo_nodo_alloc(ctx, 1);
				nodo_origen_actual->indice = indice_nodo_destino_actual;
				ctx->inicio = nodo_origen_actual;
				*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
						GRAFO_PRINCIPAL) = 0;
				*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
						GRAFO_INDICE) = -1;
				*grafo_apuntador_num_nodos_asociados(nodo_origen_actual,
						GRAFO_DISTANCIA) = -1;
				caca_log_debug("Se inicia la lista de nodos en ctx");
				break;
			default:
				perror("no se pudo configurar ctx d graph");
				exit(1);
				break;
			}

			nodo_destino_actual = grafo_nodo_alloc(ctx, 1);
			nodo_destino_actual->distancia = distancia_actual;
			nodo_destino_actual->indice = indice_nodo_origen_actual;

			caca_log_debug("insertando nodo %p en lista de indices de %p",
					nodo_destino_actual, nodo_origen_actual);
			grafo_anadir_nodo(nodo_origen_actual, nodo_destino_actual,
					GRAFO_INDICE);
			caca_log_debug("insertando nodo %p en lista de distancias de %p",
					nodo_destino_actual, nodo_origen_actual);
			grafo_anadir_nodo(nodo_origen_actual, nodo_destino_actual,
					GRAFO_DISTANCIA);

			if (usar_hashes) {
				ctx->matrix_distancias[indice_nodo_destino_actual][indice_nodo_origen_actual] =
						distancia_actual;
				*(ctx->referencias_nodos_por_indice + indice_nodo_destino_actual) =
						nodo_origen_actual;
			}

		}

	}

	return (0);
}

nodo *grafo_nodo_alloc(grafo_contexto *ctx, int localidades_solicitadas) {
	nodo *inicio_localidades_allocadas = NULL;
	if ((sizeof(ctx->nodos_disponibles) - ctx->localidades_usadas)
			>= localidades_solicitadas) {
		inicio_localidades_allocadas = ctx->nodos_disponibles
				+ ctx->localidades_usadas;
		ctx->localidades_usadas += localidades_solicitadas;
		caca_log_debug("el inicio actual es %p",
				ctx->nodos_disponibles + ctx->localidades_usadas);
	}
	caca_log_debug(
			"las localidades usadas son %d, la direccion regresada es %p, cuyo inicio es %p",
			ctx->localidades_usadas, inicio_localidades_allocadas,
			ctx->nodos_disponibles);
	return inicio_localidades_allocadas;
}

GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_lineal(nodo *inicio, nodo *nodo_a_buscar,
		GRAFO_CRITERIOS_ORDENACION criterio_busqueda, void *nodos_encontrados,
		int limite_nodos_busqueda) {
	int num_nodos_recorridos = 0;
	GRAFO_TIPO_RESULTADO_BUSQUEDA tipo_resultado = GRAFO_NADA_ENCONTRADO;
	char buffer[MAX_TAM_CADENA] = { '\0' };
	char buffer1[MAX_TAM_CADENA] = { '\0' };
	nodo *nodo_actual = NULL;
	nodo *nodo_anterior = NULL;
	nodo **nodos_encontrados_int = NULL;

	nodos_encontrados_int = ((nodo **) nodos_encontrados);

	nodo_actual = inicio;
//	GRAFO_AVANZAR_NODO(nodo_actual, criterio_busqueda, verdadero);

	caca_log_debug("nodo inicial %p, nodo a buscar %p, limite de buskeda %d",
			inicio, nodo_a_buscar, limite_nodos_busqueda);

	while (nodo_actual && num_nodos_recorridos < limite_nodos_busqueda
			&& tipo_resultado == GRAFO_NADA_ENCONTRADO) {
		caca_log_debug(
				"Comparando actual %s con el q c busca %s con criterio %s",
				grafo_nodo_a_cadena(nodo_actual,buffer,NULL),
				grafo_nodo_a_cadena(nodo_a_buscar,buffer1,NULL),
				GRAFO_NOMBRES_CRITERIOS_ORDENACION[criterio_busqueda]);
		switch (criterio_busqueda) {
		case GRAFO_VALOR:
			nodo_anterior = nodo_actual;
			if (nodo_actual->valor == nodo_a_buscar->valor) {
				tipo_resultado = GRAFO_NODO_ENCONTRADO;
				continue;
			}
			if (nodo_actual->valor > nodo_a_buscar->valor) {
				tipo_resultado = GRAFO_NODOS_PRE_POST_ENCONTRADOS;
				continue;
			}
			nodo_actual = nodo_actual->siguiente_valor;
			break;
		case GRAFO_DISTANCIA:
			nodo_anterior = nodo_actual;
			caca_log_debug("Se asigno nodo anterior en distancia");
			if (nodo_actual->distancia == nodo_a_buscar->distancia) {
				tipo_resultado = GRAFO_NODO_ENCONTRADO;
				continue;
			}
			if (nodo_actual->distancia > nodo_a_buscar->distancia) {
				tipo_resultado = GRAFO_NODOS_PRE_POST_ENCONTRADOS;
				continue;
			}
			nodo_actual = nodo_actual->siguiente_distancia;
			caca_log_debug("Avanzado en distancia con tipo resultado %d",
					tipo_resultado);
			break;
		case GRAFO_INDICE:
			nodo_anterior = nodo_actual;
			if (nodo_actual->indice == nodo_a_buscar->indice) {
				tipo_resultado = GRAFO_NODO_ENCONTRADO;
				continue;
			}
			if (nodo_actual->indice > nodo_a_buscar->indice) {
				tipo_resultado = GRAFO_NODOS_PRE_POST_ENCONTRADOS;
				continue;
			}
			nodo_actual = nodo_actual->siguiente_indice;
			break;
		case GRAFO_PRINCIPAL:
			nodo_anterior = nodo_actual;
			if (nodo_actual->indice == nodo_a_buscar->indice) {
				tipo_resultado = GRAFO_NODO_ENCONTRADO;
				continue;
			}
			if (nodo_actual->indice > nodo_a_buscar->indice) {
				tipo_resultado = GRAFO_NODOS_PRE_POST_ENCONTRADOS;
				continue;
			}
			nodo_actual = nodo_actual->siguiente;
			break;
		default:
			perror("en busqueda lineal hubo un error culero al buscar");
			exit(1);
			break;
		}
		caca_log_debug("Salio del case con %d", tipo_resultado);
		num_nodos_recorridos++;
	}

	if (tipo_resultado != GRAFO_NADA_ENCONTRADO) {
		num_nodos_recorridos++;
	} else {
		if (nodo_actual) {
			tipo_resultado = GRAFO_NODOS_PRE_POST_ENCONTRADOS;
		}
	}

	if (nodo_actual && num_nodos_recorridos <= limite_nodos_busqueda) {
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
			case GRAFO_PRINCIPAL:
				*nodos_encontrados_int = nodo_actual->anterior;
				*(nodos_encontrados_int + 1) = nodo_actual;
				break;
			default:
				perror(
						"en busqueda lineal hubo un error culero al asignar resultados");
				break;
			}
		}
	} else {
		caca_log_debug("Se reccorrio toda la lista sin encontrar");
		if (nodo_anterior) {
			caca_log_debug(
					"HAy un nodo anterior, lo q significa que al menos c recorrio un nodo");
			tipo_resultado = GRAFO_NODOS_PRE_POST_ENCONTRADOS;
			switch (criterio_busqueda) {
			case GRAFO_VALOR:
				*nodos_encontrados_int = nodo_anterior;
				*(nodos_encontrados_int + 1) = nodo_anterior->siguiente_valor;
				break;
			case GRAFO_DISTANCIA:
				*nodos_encontrados_int = nodo_anterior;
				*(nodos_encontrados_int + 1) =
						nodo_anterior->siguiente_distancia;
				break;
			case GRAFO_INDICE:
				*nodos_encontrados_int = nodo_anterior;
				*(nodos_encontrados_int + 1) = nodo_anterior->siguiente_indice;
				break;
			case GRAFO_PRINCIPAL:
				*nodos_encontrados_int = nodo_anterior;
				*(nodos_encontrados_int + 1) = nodo_anterior->siguiente;
				caca_log_debug("El nodo anterior %p, el nodo siguiente %p",
						*nodos_encontrados_int, *(nodos_encontrados_int + 1));
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

GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_binaria(nodo *inicio,
		nodo *nodo_a_buscar, GRAFO_CRITERIOS_ORDENACION criterio_busqueda,
		void *nodos_encontrados) {
	int i = 0;
	GRAFO_TIPO_RESULTADO_BUSQUEDA tipo_resultado = GRAFO_NADA_ENCONTRADO;
	char buffer[MAX_TAM_CADENA];
	nodo *nodo_actual = NULL;
	nodo *arreglo_nodos_tmp[MAX_NODOS] = { NULL };

	nodo_actual = inicio;

	caca_log_debug("El nodo inicial %p", inicio);

	if (!nodo_actual) {
		return tipo_resultado;
	}

	caca_log_debug("Antes de creaer el arreglo temporal");
	do {
		caca_log_debug("En iteracion %d", i);
		*(arreglo_nodos_tmp + i) = nodo_actual;
		caca_log_debug("Nodo actual %p", nodo_actual);
		caca_log_debug("en la iteracion %d  el nodo es %s", i,
				grafo_nodo_a_cadena(nodo_actual,buffer,NULL));
		GRAFO_AVANZAR_NODO(nodo_actual, criterio_busqueda, falso);
		i++;
	} while (nodo_actual);

	caca_log_debug("Despues de creaer el arreglo temporal");

	tipo_resultado = busqueda_binaria_recursiva(inicio, arreglo_nodos_tmp,
			nodo_a_buscar, criterio_busqueda, nodos_encontrados, 0,
			*grafo_apuntador_num_nodos_asociados(inicio, criterio_busqueda));

	caca_log_debug("Al salir %d", tipo_resultado);

	return tipo_resultado;
}

GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_binaria_recursiva(nodo *inicio,
		nodo **arreglo_nodos, nodo *nodo_a_buscar,
		GRAFO_CRITERIOS_ORDENACION criterio_busqueda, void *nodos_encontrados,
		int indice_inicial, int indice_final) {
	int resultado_comparacion = 0;
	int indice_medio = 0;
	GRAFO_TIPO_RESULTADO_BUSQUEDA tipo_resultado = GRAFO_NADA_ENCONTRADO;
	char cadena1[MAX_TAM_CADENA];
	char cadena2[MAX_TAM_CADENA];
	nodo **nodos_encontrados_int = NULL;
	nodos_encontrados_int = ((nodo **) nodos_encontrados);
	caca_log_debug("entrando con inicio %d y fin %d", indice_inicial,
			indice_final);
	caca_log_debug("nodo inicial %p, nodo a buscar %p", inicio, nodo_a_buscar);

	if ((indice_final - indice_inicial) <= GRAFO_LIMITE_ORDENACION_LINEAL) {
		caca_log_debug("buscando linealmento, ventana menor a %d",
				GRAFO_LIMITE_ORDENACION_LINEAL);
		tipo_resultado = busqueda_lineal(*(arreglo_nodos + indice_inicial),
				nodo_a_buscar, criterio_busqueda, nodos_encontrados,
				indice_final - indice_inicial + 1);
	} else {
		indice_medio = (indice_inicial + indice_final) / 2;
		caca_log_debug("el indice medio pendejo es %d", indice_medio);
		resultado_comparacion = grafo_comparar_nodos(nodo_a_buscar,
				*(arreglo_nodos + indice_medio), criterio_busqueda);
		caca_log_debug("la comparacion se hizo");
		caca_log_debug("comparando nodo %s con %s (posicion %d), resultado %d",
				grafo_nodo_a_cadena(nodo_a_buscar, cadena1,NULL),
				grafo_nodo_a_cadena(*(arreglo_nodos + indice_medio), cadena2,NULL),
				indice_medio, resultado_comparacion);
		if (resultado_comparacion == CACA_COMPARACION_IZQ_IGUAL) {
			tipo_resultado = GRAFO_NODO_ENCONTRADO;
			*nodos_encontrados_int = *(arreglo_nodos + indice_medio);
		} else {
			if (resultado_comparacion == CACA_COMPARACION_IZQ_MENOR) {
				indice_final = indice_medio - 1;
			} else {
				if (resultado_comparacion == CACA_COMPARACION_IZQ_MAYOR) {
					indice_inicial = indice_medio + 1;
				} else {
					perror("What the fuck?? busqueda_binaria_recursiva");
					exit(1);
				}
			}
			caca_log_debug(
					"recursividad arreglo_nodos %d final %d, el medio es %d",
					indice_inicial, indice_final, indice_medio);
			tipo_resultado = busqueda_binaria_recursiva(inicio, arreglo_nodos,
					nodo_a_buscar, criterio_busqueda, nodos_encontrados,
					indice_inicial, indice_final);
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

	caca_log_debug("entro a grafo_comparar_nodos");

	caca_log_debug("comparando %s con %s",
			grafo_nodo_a_cadena(nodo1, cadena1,NULL),
			grafo_nodo_a_cadena(nodo2, cadena2,NULL));

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
	case GRAFO_PRINCIPAL:
		val1 = nodo1->indice;
		val2 = nodo2->indice;
		break;
	default:
		perror("en GRAFO_COMPARAR_NODOS %s un error culero al buscar");
		break;
	}
	if (val1 < val2) {
		resultado_comparacion = CACA_COMPARACION_IZQ_MENOR;
	} else {
		if (val1 > val2) {
			resultado_comparacion = CACA_COMPARACION_IZQ_MAYOR;
		} else {
			resultado_comparacion = CACA_COMPARACION_IZQ_IGUAL;
		}
	}
	caca_log_debug("resultado de comparacion %s con %s %d",
			grafo_nodo_a_cadena(nodo1, cadena1,NULL),
			grafo_nodo_a_cadena(nodo2, cadena2,NULL), resultado_comparacion);
	return resultado_comparacion;
}

// XXX: http://stackoverflow.com/questions/4867229/code-for-printf-function-in-c
void caca_log_debug_func(const char *format, ...) {
	va_list arg;
	va_list arg2;
	const char *HEADER =
			"archivo: %s; funcion: %s; linea %d; nivel: %zd caca 8====D ";
	char formato[MAX_TAM_CADENA + sizeof(HEADER)];

	strcpy(formato, HEADER);
	strcat(formato, format);
//	printf("el formato kedo %s",formato);

	if (!cacategoria) {
		init_zlog("/Users/ernesto/workspace/unos_locos/zlog.conf");
		INIT_ZLOG_CATEGORY(cacategoria, "cacacomun");
	}

	va_start(arg, format);
	va_copy(arg2, arg);
// XXX: http://stackoverflow.com/questions/5977326/call-printf-using-va-list
	vzlog_debug(cacategoria, formato, arg2);
	va_end(arg2);
	va_end(arg);
}

void init_zlog(const char *arch_conf) {
	int rc = 0;
	rc = zlog_init(arch_conf);
	if (rc) {
		printf("init failed\n");
		exit(-1);
	}
}

static inline char *grafo_nodo_a_cadena(nodo *node, char *cadena_buffer,
		int *characteres_escritos) {

	if (characteres_escritos) {
//		caca_log_debug("Se regresaran los characteres escribtos");
		*characteres_escritos =
				sprintf(cadena_buffer, "{valor:%ld,indice:%ld,distancia:%ld} (%p) ", node->valor,
						node->indice, node->distancia,node);
	} else {

		//	caca_log_debug("No se regresaran los characteres escribtos %p", node);
		sprintf(cadena_buffer, "{valor:%ld,indice:%ld,distancia:%ld} (%p) ",
				node->valor, node->indice, node->distancia, node);
	}

//	caca_log_debug("Termina conversion");
	return cadena_buffer;
}

int apuntador_valido(void *p) {
//	extern char _etext;
//	return (p != NULL ) && ((char*) p > &_etext);
	return (p != NULL );
}

bool from_stack(void *ptr) {
	return ptr > sbrk(0);
}

void grafo_insertar_nodo(nodo *nodo_anterior, nodo *nodo_siguiente,
		nodo *nodo_a_insertar, GRAFO_CRITERIOS_ORDENACION criterio_busqueda) {
	nodo **apuntador_nodo_anterior = NULL;
	nodo **apuntador_nodo_siguiente = NULL;
	nodo **nodo_a_insertar_siguente = NULL;
	nodo **nodo_a_insertar_anterior = NULL;

	if (!nodo_anterior && !nodo_siguiente) {
		perror("grafo_insertar_nodo no puede continuar");
		exit(1);
	}

	switch (criterio_busqueda) {
	case GRAFO_VALOR:
		apuntador_nodo_anterior =
				nodo_anterior ? &nodo_anterior->siguiente_valor : NULL;
		apuntador_nodo_siguiente =
				nodo_siguiente ? &nodo_siguiente->anterior_valor : NULL;
		nodo_a_insertar_siguente = &nodo_a_insertar->siguiente_valor;
		nodo_a_insertar_anterior = &nodo_a_insertar->anterior_valor;
		break;
	case GRAFO_DISTANCIA:
		apuntador_nodo_anterior =
				nodo_anterior ? &nodo_anterior->siguiente_distancia : NULL;
		apuntador_nodo_siguiente =
				nodo_siguiente ? &nodo_siguiente->anterior_distancia : NULL;
		nodo_a_insertar_siguente = &nodo_a_insertar->siguiente_distancia;
		nodo_a_insertar_anterior = &nodo_a_insertar->anterior_distancia;
		break;
	case GRAFO_INDICE:
		apuntador_nodo_anterior =
				nodo_anterior ? &nodo_anterior->siguiente_indice : NULL;
		apuntador_nodo_siguiente =
				nodo_siguiente ? &nodo_siguiente->anterior_indice : NULL;
		nodo_a_insertar_siguente = &nodo_a_insertar->siguiente_indice;
		nodo_a_insertar_anterior = &nodo_a_insertar->anterior_indice;
		break;
	case GRAFO_PRINCIPAL:
		apuntador_nodo_anterior =
				nodo_anterior ? &nodo_anterior->siguiente : NULL;
		apuntador_nodo_siguiente =
				nodo_siguiente ? &nodo_siguiente->anterior : NULL;
		nodo_a_insertar_siguente = &nodo_a_insertar->siguiente;
		nodo_a_insertar_anterior = &nodo_a_insertar->anterior;
		caca_log_debug(
				"insertando %p, nodo a insrtar anterior %p, cuyo valor es %p, nodo a insertar siguiente %p, cuyo valor es %p",
				nodo_a_insertar, nodo_a_insertar_anterior,
				*nodo_a_insertar_anterior, nodo_a_insertar_siguente,
				*nodo_a_insertar_siguente);
		break;
	default:
		perror("fallo en grafo_insertar_nodo");
		break;
	}
	if (apuntador_nodo_anterior) {
		*apuntador_nodo_anterior = nodo_a_insertar;
	}
	if (apuntador_nodo_siguiente) {
		*apuntador_nodo_siguiente = nodo_a_insertar;
	}
	*nodo_a_insertar_siguente = nodo_siguiente;
	*nodo_a_insertar_anterior = nodo_anterior;
	caca_log_debug("nodo %p entre %p y %p", nodo_a_insertar, nodo_anterior,
			nodo_siguiente);
}

void grafo_anadir_nodo(nodo *nodo_inicial, nodo *nodo_a_anadir,
		GRAFO_CRITERIOS_ORDENACION criterio_busqueda) {
	GRAFO_TIPO_RESULTADO_BUSQUEDA tipo_resultado;
	nodo *nodo_anterior = NULL;
	nodo *nodo_siguiente = NULL;
	nodo *nodo_inicial_tmp = NULL;
	nodo *nodos_encontrados[2] = { NULL };

	caca_log_debug("Anadiendo nodo %p a lista %p de %s", nodo_a_anadir,
			nodo_inicial,
			GRAFO_NOMBRES_CRITERIOS_ORDENACION[criterio_busqueda]);

	nodo_inicial_tmp = nodo_inicial;

	if (criterio_busqueda != GRAFO_PRINCIPAL) {
		GRAFO_AVANZAR_NODO(nodo_inicial_tmp, criterio_busqueda, verdadero);
		if (nodo_inicial_tmp) {
			*grafo_apuntador_num_nodos_asociados(nodo_inicial_tmp,
					criterio_busqueda) = *grafo_apuntador_num_nodos_asociados(
					nodo_inicial, criterio_busqueda);
		}
	}

	tipo_resultado = busqueda_binaria(nodo_inicial_tmp, nodo_a_anadir,
			criterio_busqueda, (void *) &nodos_encontrados);

	switch (tipo_resultado) {
	case GRAFO_NODOS_PRE_POST_ENCONTRADOS:
		nodo_anterior = *(nodos_encontrados);
		nodo_siguiente = *(nodos_encontrados + 1);
		break;
	case GRAFO_NODO_ENCONTRADO:
		GRAFO_NODO_SIG_ANT(nodo_anterior, nodo_siguiente, *nodos_encontrados,
				criterio_busqueda)
		;
		break;
	case GRAFO_NADA_ENCONTRADO:
		nodo_anterior = nodo_inicial;
		nodo_siguiente = NULL;
		break;
	default:
		perror("init_grafo error");
		exit(1);
		break;
	}

	grafo_insertar_nodo(nodo_anterior, nodo_siguiente, nodo_a_anadir,
			criterio_busqueda);
	(*grafo_apuntador_num_nodos_asociados(nodo_inicial, criterio_busqueda))++;
}

void imprimir_lista_adjacencia(nodo *nodo_inicial) {
	const char *FORMATO_CONCAT_NODO = " --> ";
	int caracteres_escritos = 0;
	char *ap_cadena = NULL;
	char buffer[MAX_TAM_CADENA];
	nodo *nodo_actual = NULL;
	nodo *nodo_adjacente_actual = NULL;

	nodo_actual = nodo_inicial;

	caca_log_debug("La matrix de adjacencia es:");

	while (nodo_actual) {
		nodo_adjacente_actual = nodo_actual;
		ap_cadena = buffer;

		caca_log_debug("Conexiones de nodo %s.",
				grafo_nodo_a_cadena(nodo_adjacente_actual, ap_cadena,
						&caracteres_escritos));

		strcpy(ap_cadena, GRAFO_NOMBRES_CRITERIOS_ORDENACION[GRAFO_DISTANCIA]);
		ap_cadena += strlen(
				GRAFO_NOMBRES_CRITERIOS_ORDENACION[GRAFO_DISTANCIA]);

		nodo_adjacente_actual = nodo_actual->siguiente_distancia;
		while (nodo_adjacente_actual) {

//			caca_log_debug("Direccion nodo adjacente %p", nodo_adjacente_actual);
			strcpy(ap_cadena, FORMATO_CONCAT_NODO);
			ap_cadena += strlen(FORMATO_CONCAT_NODO);
			grafo_nodo_a_cadena(nodo_adjacente_actual, ap_cadena,
					&caracteres_escritos);
			ap_cadena += caracteres_escritos;

			nodo_adjacente_actual = nodo_adjacente_actual->siguiente_distancia;
		}

		caca_log_debug("%s", buffer);

		ap_cadena = buffer;

		strcpy(ap_cadena, GRAFO_NOMBRES_CRITERIOS_ORDENACION[GRAFO_INDICE]);
		ap_cadena += strlen(GRAFO_NOMBRES_CRITERIOS_ORDENACION[GRAFO_INDICE]);

		nodo_adjacente_actual = nodo_actual->siguiente_indice;
		while (nodo_adjacente_actual) {

//			caca_log_debug("Direccion nodo adjacente %p", nodo_adjacente_actual);
			strcpy(ap_cadena, FORMATO_CONCAT_NODO);
			ap_cadena += strlen(FORMATO_CONCAT_NODO);
			grafo_nodo_a_cadena(nodo_adjacente_actual, ap_cadena,
					&caracteres_escritos);
			ap_cadena += caracteres_escritos;

			nodo_adjacente_actual = nodo_adjacente_actual->siguiente_indice;
		}

		caca_log_debug("%s", buffer);

		nodo_actual = nodo_actual->siguiente;

	}
}

static inline int *grafo_apuntador_num_nodos_asociados(nodo *nodo,
		GRAFO_CRITERIOS_ORDENACION criterio_busqueda) {
	int *num_nodos = NULL;
	switch (criterio_busqueda) {
	case GRAFO_VALOR:
		num_nodos = &(nodo)->num_nodos_asociados_valor;
		break;
	case GRAFO_DISTANCIA:
		num_nodos = &(nodo)->num_nodos_asociados_distancia;
		break;
	case GRAFO_INDICE:
		num_nodos = &(nodo)->num_nodos_asociados_indice;
		break;
	case GRAFO_PRINCIPAL:
		num_nodos = &(nodo)->num_nodos_asociados;
		break;
	default:
		perror("en GRAFO_NUM_NODOS_ASOCIADOS hubo un error culero al buscar");
		break;
	}
	return num_nodos;
}

void arbol_avl_init(arbol_binario_contexto *ctx, tipo_dato *indices,
		tipo_dato *datos, int num_datos,
		nodo_arbol_binario **arreglo_referencias_nodos) {
	int i = 0;
	tipo_dato dato_actual = 0;
	nodo_arbol_binario *nodo_actual = NULL;

	memset((void * )ctx, 0, sizeof(arbol_binario_contexto));
	memset((void * )ctx->nodos_disponibles, 0, sizeof(ctx->nodos_disponibles));

	ctx->raiz = arbol_binario_nodo_allocar(ctx, 1);

	ctx->raiz->valor = *datos;

	if (indices) {
		ctx->raiz->indice = *indices;
	} else {
		ctx->raiz->indice = 0;
	}
	if (arreglo_referencias_nodos) {
		if (indices) {
			*(arreglo_referencias_nodos + *(indices + 0)) = ctx->raiz;

		} else {
			*(arreglo_referencias_nodos + 0) = ctx->raiz;
		}
	}

	for (i = 1; i < num_datos; i++) {
		dato_actual = *(datos + i);
		nodo_actual = arbol_binario_nodo_allocar(ctx, 1);

		nodo_actual->valor = dato_actual;
		if (indices) {
			nodo_actual->indice = *(indices + i);
		} else {
			nodo_actual->indice = i;
		}

		arbol_avl_insertar(&ctx->raiz, nodo_actual, ctx->no_indices_repetidos);
		if (arreglo_referencias_nodos) {
			if (indices) {
				*(arreglo_referencias_nodos + *(indices + i)) = nodo_actual;

			} else {
				*(arreglo_referencias_nodos + i) = nodo_actual;
			}
		}
		caca_log_debug("Raiz actual %ld", ctx->raiz->valor);
	}
}

void arbol_avl_insertar(nodo_arbol_binario **raiz,
		nodo_arbol_binario *nodo_a_insertar, bool no_indices_repetidos) {
	nodo_arbol_binario *raiz_int = NULL;

	raiz_int = *raiz;
	char buffer[MAX_TAM_CADENA];
	char buffer1[MAX_TAM_CADENA];

	caca_log_debug("Insertando nodo %s, ancestro actual %s",
			arbol_binario_nodo_a_cadena(nodo_a_insertar,buffer,NULL),
			arbol_binario_nodo_a_cadena(raiz_int,buffer1,NULL));

	switch (arbol_avl_compara_nodos(raiz_int, nodo_a_insertar)) {
	case CACA_COMPARACION_IZQ_MENOR:
		caca_log_debug("Insercion BST a la der");
		if (raiz_int->hijo_der) {
			caca_log_debug("Recursiva");
			arbol_avl_insertar(&raiz_int->hijo_der, nodo_a_insertar,
					no_indices_repetidos);
		} else {
			raiz_int->hijo_der = nodo_a_insertar;
			ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_a_insertar, raiz_int);
		}
		break;
	case CACA_COMPARACION_IZQ_MAYOR:
		caca_log_debug("Insercion BST a la izq");
		if (raiz_int->hijo_izq) {
			caca_log_debug("Recursiva %p", &raiz_int->hijo_izq);
			arbol_avl_insertar(&raiz_int->hijo_izq, nodo_a_insertar,
					no_indices_repetidos);
		} else {
			caca_log_debug("Directa");
			raiz_int->hijo_izq = nodo_a_insertar;
			ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_a_insertar, raiz_int);
		}
		break;
	case CACA_COMPARACION_IZQ_IGUAL:
		if (no_indices_repetidos) {
			perror("NO mames, una llave repetida, ahhhhh !");
			exit(1);
		} else {
			caca_log_debug("Insercion BST a la der aunke las llaves son =s");
			if (raiz_int->hijo_der) {
				caca_log_debug("Recursiva");
				arbol_avl_insertar(&raiz_int->hijo_der, nodo_a_insertar,
						no_indices_repetidos);
			} else {
				raiz_int->hijo_der = nodo_a_insertar;
				ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_a_insertar, raiz_int);
			}
		}
		break;
	default:
		perror("NO mames, resultado increible en arbol_avl_insertar!");
		exit(1);
		break;
	}

	caca_log_debug("Echa la insercion BST de %ld en ancestro %ld",
			ARBOL_AVL_GET_VALOR(nodo_a_insertar),
			ARBOL_AVL_GET_VALOR(raiz_int));
	ARBOL_AVL_ACTUALIZAR_ALTURA(raiz_int);
	caca_log_debug("algura es %d", raiz_int->altura);
	caca_log_debug("hijo der %ld %p %p", ARBOL_AVL_GET_VALOR((*raiz)->hijo_izq),
			(*raiz)->hijo_izq, &(*raiz)->hijo_izq);

	switch (arbol_avl_diferencia_alturas_subarboles(raiz_int, 1, verdadero)) {
	case ARBOL_AVL_ALTURA_CARGADA_IZQ:
		caca_log_debug("Arbol cagado a la izq en %ld", raiz_int->valor);
		if (ARBOL_AVL_GET_VALOR(raiz_int->hijo_izq)
				< ARBOL_AVL_GET_VALOR(nodo_a_insertar)) {
			caca_log_debug("con hijo a la der");
			arbol_binario_rotar_izq(&raiz_int->hijo_izq);
		}
		arbol_binario_rotar_der(raiz);
		caca_log_debug("Ahora la raiz %p %ld, izq %ld, der %ld", raiz,
				ARBOL_AVL_GET_VALOR(*raiz),
				ARBOL_AVL_GET_VALOR((*raiz)->hijo_izq),
				ARBOL_AVL_GET_VALOR((*raiz)->hijo_der));
		break;
	case ARBOL_AVL_ALTURA_CARGADA_DER:
		caca_log_debug("Arbol cagado a la der en %ld", raiz_int->valor);
		if (ARBOL_AVL_GET_VALOR(raiz_int->hijo_der)
				> ARBOL_AVL_GET_VALOR(nodo_a_insertar)) {
			caca_log_debug("con hijo a la izq");
			arbol_binario_rotar_der(&raiz_int->hijo_der);
		}
		arbol_binario_rotar_izq(raiz);
		caca_log_debug("Ahora la raiz %p %ld, izq %ld, der %ld", raiz,
				ARBOL_AVL_GET_VALOR(*raiz),
				ARBOL_AVL_GET_VALOR((*raiz)->hijo_izq),
				ARBOL_AVL_GET_VALOR((*raiz)->hijo_der));
		break;
	case ARBOL_AVL_ALTURA_BALANCEADA:
		caca_log_debug("Excelente, en nodo %s, altura balancelada",
				arbol_binario_nodo_a_cadena(raiz_int,buffer,NULL));
		break;
	default:
		perror("error en altura indeterminada, verga");
		exit(1);
		break;
	}

	caca_log_debug("termino con valor %ld, raiz %p y ap raiz %p",
			ARBOL_AVL_GET_VALOR(*raiz), *raiz, raiz);
	arbol_binario_recorrido_preoder(*raiz);
}

static inline int arbol_avl_compara_nodos(nodo_arbol_binario *nodo1,
		nodo_arbol_binario *nodo2) {
	caca_log_debug("Comparando %ld con %ld", nodo1->valor, nodo2->valor);
	if (nodo1->valor < nodo2->valor) {
		return CACA_COMPARACION_IZQ_MENOR;
	} else {
		if (nodo1->valor > nodo2->valor) {
			return CACA_COMPARACION_IZQ_MAYOR;
		} else {
			return CACA_COMPARACION_IZQ_IGUAL;
		}
	}
}

static inline int caca_int_max(int a, int b) {
	return (a > b) ? a : b;
}

static inline int arbol_avl_diferencia_alturas_subarboles(
		nodo_arbol_binario *nodo, int tolerancia,
		bool considerar_balanceado_cargado_der) {
	int diferencia_alturas = 0;
	diferencia_alturas = ARBOL_AVL_GET_ALTURA(nodo->hijo_izq)
			- ARBOL_AVL_GET_ALTURA(nodo->hijo_der);
	caca_log_debug("La diferecnai resultao %d", diferencia_alturas);
	if (considerar_balanceado_cargado_der) {
		return diferencia_alturas < 0 - tolerancia ?
				ARBOL_AVL_ALTURA_CARGADA_DER :
				diferencia_alturas > tolerancia ?
						ARBOL_AVL_ALTURA_CARGADA_IZQ :
						ARBOL_AVL_ALTURA_BALANCEADA;
	} else {
		return diferencia_alturas < 0 - tolerancia ?
				ARBOL_AVL_ALTURA_CARGADA_DER :
				diferencia_alturas > tolerancia ?
						ARBOL_AVL_ALTURA_CARGADA_IZQ :
						ARBOL_AVL_ALTURA_BALANCEADA;
	}
}

static inline void arbol_binario_rotar_izq(nodo_arbol_binario **nodo) {
	nodo_arbol_binario *nodo_int = NULL;
	nodo_arbol_binario *hijo_der = NULL;
	nodo_arbol_binario *hijo_der_subarbol_izq = NULL;

	caca_log_debug("rotando %ld", ARBOL_AVL_GET_VALOR(*nodo));

	nodo_int = *nodo;
	hijo_der = nodo_int->hijo_der;
	caca_log_debug("Asignado hijo der %ld", ARBOL_AVL_GET_VALOR(hijo_der));
	hijo_der_subarbol_izq = hijo_der->hijo_izq;
	caca_log_debug("Asignado subarbol izq hijo der %ld",
			ARBOL_AVL_GET_VALOR(hijo_der_subarbol_izq));

	caca_log_debug("Antes de la rotacion");

	nodo_int->hijo_der = hijo_der_subarbol_izq;
	hijo_der->hijo_izq = nodo_int;

	ARBOL_BINARIO_ACTUALIZAR_PADRE(hijo_der, ARBOL_AVL_GET_PADRE(nodo_int));
	ARBOL_BINARIO_ACTUALIZAR_PADRE(hijo_der_subarbol_izq, nodo_int);
	ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_int, hijo_der);

	caca_log_debug("Rotados los nodos");
	ARBOL_AVL_ACTUALIZAR_ALTURA(nodo_int);
	ARBOL_AVL_ACTUALIZAR_ALTURA(hijo_der);
	caca_log_debug("Alturas actualizadas");

	*nodo = hijo_der;

}

static inline void arbol_binario_rotar_der(nodo_arbol_binario **nodo) {
	nodo_arbol_binario *nodo_int = NULL;
	nodo_arbol_binario *hijo_izq = NULL;
	nodo_arbol_binario *hijo_izq_subarbol_der = NULL;

	caca_log_debug("rotando %ld hijo izq %ld hijo der %ld",
			ARBOL_AVL_GET_VALOR(*nodo), ARBOL_AVL_GET_VALOR((*nodo)->hijo_izq),
			ARBOL_AVL_GET_VALOR((*nodo)->hijo_der));

	nodo_int = *nodo;
	hijo_izq = nodo_int->hijo_izq;
	caca_log_debug("Asignado hijo izq %ld", ARBOL_AVL_GET_VALOR(hijo_izq));
	hijo_izq_subarbol_der = hijo_izq->hijo_der;
	caca_log_debug("Asignado subarbol der hijo izq %ld",
			ARBOL_AVL_GET_VALOR(hijo_izq_subarbol_der));

	caca_log_debug("Antes de la rotacion");

	nodo_int->hijo_izq = hijo_izq_subarbol_der;
	hijo_izq->hijo_der = nodo_int;

	ARBOL_BINARIO_ACTUALIZAR_PADRE(hijo_izq, ARBOL_AVL_GET_PADRE(nodo_int));
	ARBOL_BINARIO_ACTUALIZAR_PADRE(hijo_izq_subarbol_der, nodo_int);
	ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_int, hijo_izq);

	caca_log_debug("Rotados los nodos");
	ARBOL_AVL_ACTUALIZAR_ALTURA(nodo_int);
	ARBOL_AVL_ACTUALIZAR_ALTURA(hijo_izq);
	caca_log_debug("Alturas actualizadas");

	*nodo = hijo_izq;

}

static inline char *arbol_binario_nodo_a_cadena(nodo_arbol_binario *node,
		char *cadena_buffer, int *characteres_escritos) {

	int characteres_escritos_int = 0;

	characteres_escritos_int =
			sprintf(cadena_buffer,
					"{indice:%ld, valor:%ld, altura %d, direccion %p, hijo izq:%ld (%p), hijo der:%ld (%p), padre:%ld (%p)}  ",
					node->indice, node->valor, ARBOL_AVL_GET_ALTURA(node), node,
					ARBOL_AVL_GET_VALOR(node->hijo_izq), node->hijo_izq,
					ARBOL_AVL_GET_VALOR(node->hijo_der), node->hijo_der,
					ARBOL_AVL_GET_VALOR(node->padre), node->padre);

	if (characteres_escritos) {
		*characteres_escritos = characteres_escritos_int;
	}
	return cadena_buffer;
}

static inline nodo_arbol_binario *arbol_binario_nodo_allocar(
		arbol_binario_contexto *ctx, int localidades_solicitadas) {
	nodo_arbol_binario *inicio_localidades_allocadas = NULL;
	if ((sizeof(ctx->nodos_disponibles) - ctx->localidades_usadas)
			>= localidades_solicitadas) {
		inicio_localidades_allocadas = ctx->nodos_disponibles
				+ ctx->localidades_usadas;
		ctx->localidades_usadas += localidades_solicitadas;
	}
	return inicio_localidades_allocadas;
}

void arbol_binario_recorrido_preoder(nodo_arbol_binario *raiz) {
	char buffer[MAX_TAM_CADENA];
	if (!raiz) {
		return;
	}
	caca_log_debug("%s ", arbol_binario_nodo_a_cadena(raiz,buffer,NULL));
	arbol_binario_recorrido_preoder(raiz->hijo_izq);
	arbol_binario_recorrido_preoder(raiz->hijo_der);
}

void arbol_binario_colectar_datos_recorrido_preoder(nodo_arbol_binario *raiz,
		tipo_dato *datos_ordenados, int *num_datos_colectados) {
	if (!raiz) {
		return;
	}
	*(datos_ordenados + (*num_datos_colectados)++) = raiz->valor;
	arbol_binario_colectar_datos_recorrido_preoder(raiz->hijo_izq,
			datos_ordenados, num_datos_colectados);
	arbol_binario_colectar_datos_recorrido_preoder(raiz->hijo_der,
			datos_ordenados, num_datos_colectados);
}

static inline nodo_arbol_binario *arbol_binario_get_nodo_minimo_valor(
		nodo_arbol_binario *raiz) {
	nodo_arbol_binario *nodo_actual = NULL;
	nodo_actual = raiz;
	while (nodo_actual->hijo_izq) {
		nodo_actual = nodo_actual->hijo_izq;
	}
	return nodo_actual;
}

void arbol_binario_borrar_nodo(nodo_arbol_binario **raiz,
		tipo_dato valor_a_borrar) {
	nodo_arbol_binario *raiz_int = NULL;
	nodo_arbol_binario *nodo_min = NULL;

	raiz_int = *raiz;

	if (!raiz_int) {
		caca_log_debug("Raiz nula, pq la mandan?");
		return;
	}
	if (raiz_int->valor > valor_a_borrar) {
		arbol_binario_borrar_nodo(&raiz_int->hijo_izq, valor_a_borrar);
	} else {
		if (raiz_int->valor < valor_a_borrar) {
			arbol_binario_borrar_nodo(&raiz_int->hijo_der, valor_a_borrar);
		} else {
			if (!raiz_int->hijo_izq) {
				*raiz = raiz_int->hijo_der;
			} else {
				if (!raiz_int->hijo_der) {
					*raiz = raiz_int->hijo_izq;
				} else {
					nodo_min = arbol_binario_get_nodo_minimo_valor(
							raiz_int->hijo_der);
					raiz_int->valor = nodo_min->valor;
					arbol_binario_borrar_nodo(&raiz_int->hijo_der,
							raiz_int->valor);
				}
			}
		}
	}

}

void arbol_binario_recorrido_inoder(nodo_arbol_binario *raiz) {
	char buffer[MAX_TAM_CADENA];
	if (!raiz) {
		return;
	}
	arbol_binario_recorrido_inoder(raiz->hijo_izq);
	caca_log_debug("%s ", arbol_binario_nodo_a_cadena(raiz,buffer,NULL));
	arbol_binario_recorrido_inoder(raiz->hijo_der);
}

void arbol_binario_colectar_datos_recorrido_inoder(nodo_arbol_binario *raiz,
		tipo_dato *datos_ordenados, int *num_datos_colectados) {
	if (!raiz) {
		return;
	}
	arbol_binario_colectar_datos_recorrido_inoder(raiz->hijo_izq,
			datos_ordenados, num_datos_colectados);
	*(datos_ordenados + (*num_datos_colectados)++) = raiz->valor;
	arbol_binario_colectar_datos_recorrido_inoder(raiz->hijo_der,
			datos_ordenados, num_datos_colectados);
}

void arbol_avl_borrar(nodo_arbol_binario **raiz, tipo_dato valor_a_borrar) {

	nodo_arbol_binario *raiz_int = NULL;
	nodo_arbol_binario *nodo_min = NULL;

	raiz_int = *raiz;

	if (!raiz_int) {
		caca_log_debug("Raiz nula, pq la mandan?");
		return;
	}
	if (raiz_int->valor > valor_a_borrar) {
		arbol_avl_borrar(&raiz_int->hijo_izq, valor_a_borrar);
	} else {
		if (raiz_int->valor < valor_a_borrar) {
			arbol_avl_borrar(&raiz_int->hijo_der, valor_a_borrar);
		} else {
			if (!raiz_int->hijo_izq) {
				*raiz = raiz_int->hijo_der;
			} else {
				if (!raiz_int->hijo_der) {
					*raiz = raiz_int->hijo_izq;
				} else {
					nodo_min = arbol_binario_get_nodo_minimo_valor(
							raiz_int->hijo_der);
					raiz_int->valor = nodo_min->valor;
					arbol_avl_borrar(&raiz_int->hijo_der, raiz_int->valor);
				}
			}
		}
	}

	caca_log_debug("Terminado borrado BST");

	if (!*raiz) {
		caca_log_debug("Se borro el ultimo nodo");
		return;
	}

	ARBOL_AVL_ACTUALIZAR_ALTURA(raiz_int);

	switch (arbol_avl_diferencia_alturas_subarboles(raiz_int, 1, verdadero)) {
	case ARBOL_AVL_ALTURA_CARGADA_IZQ:
		caca_log_debug("Arbol cagado a la izq");
		if (arbol_avl_diferencia_alturas_subarboles(raiz_int->hijo_izq, 0,
				falso) == ARBOL_AVL_ALTURA_CARGADA_DER) {
			caca_log_debug("Con ijo cagado a la derecha");
			arbol_binario_rotar_izq(&raiz_int->hijo_izq);
		}
		caca_log_debug("Rotando nodo raiz");
		arbol_binario_rotar_der(raiz);
		caca_log_debug("Ahora la raiz %ld, izq %ld, der %ld",
				ARBOL_AVL_GET_VALOR(*raiz),
				ARBOL_AVL_GET_VALOR((*raiz)->hijo_izq),
				ARBOL_AVL_GET_VALOR((*raiz)->hijo_der));
		break;
	case ARBOL_AVL_ALTURA_CARGADA_DER:
		caca_log_debug("Arbol cagado a la der");
		if (arbol_avl_diferencia_alturas_subarboles(raiz_int->hijo_der, 0,
				verdadero) == ARBOL_AVL_ALTURA_CARGADA_IZQ) {
			caca_log_debug("Con ijo cagado a la izq");
			arbol_binario_rotar_der(&raiz_int->hijo_der);
		}
		caca_log_debug("Rotando nodo raiz");
		arbol_binario_rotar_izq(raiz);
		caca_log_debug("Ahora la raiz %ld, izq %ld, der %ld",
				ARBOL_AVL_GET_VALOR(*raiz),
				ARBOL_AVL_GET_VALOR((*raiz)->hijo_izq),
				ARBOL_AVL_GET_VALOR((*raiz)->hijo_der));
		break;
	case ARBOL_AVL_ALTURA_BALANCEADA:
		caca_log_debug("Arbol balanceado");
		break;
	default:
		caca_log_debug("Mother of fuck arbol_avl_borrar");
		break;
	}

}

void arbol_avl_borrar_referencia_directa(nodo_arbol_binario *nodo_a_borrar) {

	nodo_arbol_binario *ancestro_actual = NULL;
	nodo_arbol_binario *nodo_a_borrar_padre = NULL;
	nodo_arbol_binario *nodo_min = NULL;

	nodo_arbol_binario **ancestro_actual_apuntador = NULL;
	nodo_arbol_binario **nodo_a_borrar_ref = NULL;

	if (!nodo_a_borrar) {
		caca_log_debug("Nodo a borrar nula, pq la mandan?");
		return;
	}

	if (!(nodo_a_borrar_padre = ARBOL_AVL_GET_PADRE(nodo_a_borrar))) {
		caca_log_debug(
				"No se puede borrar x q no ay referencia a nodo padre para encontrar su lugar en el mundo");
		return;
	}

	caca_log_debug("Calculando referencia en arbolazo, padre %p",
			nodo_a_borrar_padre);
	nodo_a_borrar_ref =
			nodo_a_borrar->valor < (nodo_a_borrar_padre)->valor ?
					&nodo_a_borrar_padre->hijo_izq :
					&nodo_a_borrar_padre->hijo_der;

	caca_log_debug("borrando nodo %ld", nodo_a_borrar->valor);

	ancestro_actual = nodo_a_borrar->padre;
	ancestro_actual_apuntador = &nodo_a_borrar->padre;

	if (!nodo_a_borrar->hijo_izq) {
		caca_log_debug("Borrando directamente sin hijo izq");
		*nodo_a_borrar_ref = nodo_a_borrar->hijo_der;
	} else {
		if (!nodo_a_borrar->hijo_der) {
			caca_log_debug("Borrando directamente sin hijo der");
			*nodo_a_borrar_ref = nodo_a_borrar->hijo_izq;
		} else {
			caca_log_debug("Borrando nodo intermedio");
			nodo_min = arbol_binario_get_nodo_minimo_valor(
					nodo_a_borrar->hijo_der);
			nodo_a_borrar->valor = nodo_min->valor;
			arbol_avl_borrar_referencia_directa(nodo_min);
			nodo_min->hijo_der = nodo_min->hijo_izq = NULL;
		}
	}

	caca_log_debug("Terminado borrado BST");

	do {

		if (!*ancestro_actual_apuntador) {
			caca_log_debug("Se borro el ultimo nodo");
			return;
		}

		ARBOL_AVL_ACTUALIZAR_ALTURA(ancestro_actual);

		switch (arbol_avl_diferencia_alturas_subarboles(ancestro_actual, 1,
				verdadero)) {
		case ARBOL_AVL_ALTURA_CARGADA_IZQ:
			caca_log_debug("Arbol cagado a la izq");
			if (arbol_avl_diferencia_alturas_subarboles(
					ancestro_actual->hijo_izq, 0,
					falso) == ARBOL_AVL_ALTURA_CARGADA_DER) {
				caca_log_debug("Con ijo cagado a la derecha");
				arbol_binario_rotar_izq(&ancestro_actual->hijo_izq);
			}
			caca_log_debug("Rotando nodo ancestro_actual_apuntador");
			arbol_binario_rotar_der(ancestro_actual_apuntador);
			caca_log_debug(
					"Ahora la ancestro_actual_apuntador %ld, izq %ld, der %ld",
					ARBOL_AVL_GET_VALOR(*ancestro_actual_apuntador),
					ARBOL_AVL_GET_VALOR((*ancestro_actual_apuntador)->hijo_izq),
					ARBOL_AVL_GET_VALOR((*ancestro_actual_apuntador)->hijo_der));
			break;
		case ARBOL_AVL_ALTURA_CARGADA_DER:
			caca_log_debug("Arbol cagado a la der");
			if (arbol_avl_diferencia_alturas_subarboles(
					ancestro_actual->hijo_der, 0,
					verdadero) == ARBOL_AVL_ALTURA_CARGADA_IZQ) {
				caca_log_debug("Con ijo cagado a la izq");
				arbol_binario_rotar_der(&ancestro_actual->hijo_der);
			}
			caca_log_debug("Rotando nodo ancestro_actual_apuntador");
			arbol_binario_rotar_izq(ancestro_actual_apuntador);
			caca_log_debug(
					"Ahora la ancestro_actual_apuntador %ld, izq %ld, der %ld",
					ARBOL_AVL_GET_VALOR(*ancestro_actual_apuntador),
					ARBOL_AVL_GET_VALOR((*ancestro_actual_apuntador)->hijo_izq),
					ARBOL_AVL_GET_VALOR((*ancestro_actual_apuntador)->hijo_der));
			break;
		case ARBOL_AVL_ALTURA_BALANCEADA:
			caca_log_debug("Arbol balanceado %ld", ancestro_actual->valor);
			break;
		default:
			caca_log_debug("Mother of fuck arbol_avl_borrar");
			break;
		}

		ancestro_actual = ancestro_actual->padre;
		ancestro_actual_apuntador = &ancestro_actual->padre;
	} while (ancestro_actual);
}

void dijkstra_modificar_valor_nodo(nodo_arbol_binario **raiz,
		nodo_arbol_binario **referencias_directas, tipo_dato indice,
		tipo_dato nuevo_valor) {
	nodo_arbol_binario *referencia_directa = NULL;

	referencia_directa = *(referencias_directas + indice);

	caca_log_debug("Modificando valor de %ld, indice %ld",
			referencia_directa->valor, indice);

	arbol_avl_borrar_referencia_directa(referencia_directa);

	referencia_directa->valor = nuevo_valor;

	arbol_avl_insertar(raiz, referencia_directa, falso);
}
