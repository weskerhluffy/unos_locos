/*

 * cacacomun.c
 *
 *  Created on: 07/02/2014
 *      Author: ernesto
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifdef HAVE_EXECINFO_H
#	include <execinfo.h>
#endif
#include <stdarg.h>
#include <zlog.h>
#include <time.h>
#include <inttypes.h>
#include <sys/time.h>
#ifdef __MACH__
#include <mach/clock.h>
#include <mach/mach.h>
#endif
#include <math.h>
#include "cacacomun.h"

int lee_matrix_long_stdin(tipo_dato *matrix, int *num_filas, int *num_columnas,
		int num_max_filas, int num_max_columnas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	long numero = 0;
	char *siguiente_cadena_numero = NULL;
	char *cadena_numero_actual = NULL;
	char linea[TAM_MAX_LINEA];

	while (fgets(linea, TAM_MAX_LINEA, stdin)) {
		indice_columnas = 0;
		cadena_numero_actual = linea;
		for (siguiente_cadena_numero = linea;; siguiente_cadena_numero =
				cadena_numero_actual) {
			numero = strtol(siguiente_cadena_numero, &cadena_numero_actual, 10);
			if (cadena_numero_actual == siguiente_cadena_numero) {
				break;
			}
			*(matrix + indice_filas * num_max_columnas + indice_columnas) =
					numero;
			indice_columnas++;
			if (indice_columnas >= num_max_columnas) {
				perror("se leyeron demasiadas columnas, a la verga");
				abort();
			}
		}
		if (num_columnas) {
			num_columnas[indice_filas] = indice_columnas;
		}
		indice_filas++;
		if (indice_filas >= num_max_filas) {
			perror("se leyeron demasiadas filas, a la verga");
			abort();
		}
	}

	*num_filas = indice_filas;
	return 0;
}

int init_grafo(void *matrix, int num_filas, grafo_contexto *ctx,
		bool usar_hashes, bool relaciones_bidireccionales) {
	int i;
	int contador_nodos = 0;
	tipo_dato indice_origen_actual = 0;
	tipo_dato indice_destino_actual = 0;
	tipo_dato distancia_actual = 0;
	tipo_dato *matrix_int = NULL;
	tipo_dato *matrix_distancias = NULL;
	nodo *nuevo_nodo = NULL;
	tipo_dato nodos_registrados[MAX_NODOS] = { falso };

	memset(ctx, 0, sizeof(grafo_contexto));

	matrix_int = matrix;
	matrix_distancias = (tipo_dato *) ctx->matrix_distancias;
	memset(matrix_distancias, GRAFO_VALOR_INVALIDO,
			sizeof(ctx->matrix_distancias));
	for (i = 0; i < num_filas; i++) {
		indice_origen_actual = *(matrix_int + i * 3 + GRAFO_NODO_ORIGEN);
		indice_destino_actual = *(matrix_int + i * 3 + GRAFO_NODO_DESTINO);
		distancia_actual = *(matrix_int + i * 3 + GRAFO_DISTANCIA_ENTRE_NODOS);

		*(matrix_distancias + indice_origen_actual * MAX_COLUMNAS_NODOS
				+ indice_destino_actual) = distancia_actual;
		caca_log_debug("seteando %ld en %ld,%ld\n",
				*(matrix_distancias + indice_origen_actual * MAX_COLUMNAS_NODOS + indice_destino_actual),
				indice_origen_actual, indice_destino_actual);
		if (relaciones_bidireccionales) {
			caca_log_debug("dando d alta tambien reciproco %ld en %ld,%ld",
					distancia_actual, indice_destino_actual,
					indice_origen_actual);
			*(matrix_distancias + indice_destino_actual * MAX_COLUMNAS_NODOS
					+ indice_origen_actual) = distancia_actual;
		}
		if (!*(nodos_registrados + indice_origen_actual)) {
			nuevo_nodo = grafo_nodo_alloc(ctx, 1);
			nuevo_nodo->indice = indice_origen_actual;
			if (ctx->inicio) {
				ctx->final->siguiente = nuevo_nodo;
			} else {
				ctx->inicio = nuevo_nodo;
			}
			ctx->final = nuevo_nodo;
			*(nodos_registrados + indice_origen_actual) = verdadero;
			contador_nodos++;
		}
		if (!*(nodos_registrados + indice_destino_actual)) {
			nuevo_nodo = grafo_nodo_alloc(ctx, 1);
			nuevo_nodo->indice = indice_destino_actual;
			ctx->final->siguiente = nuevo_nodo;
			ctx->final = nuevo_nodo;
			*(nodos_registrados + indice_destino_actual) = verdadero;
			contador_nodos++;
		}
	}
	return contador_nodos;
}

nodo *grafo_nodo_alloc(grafo_contexto *ctx, int localidades_solicitadas) {
	nodo *inicio_localidades_allocadas = NULL;
	if ((sizeof(ctx->nodos_disponibles) - ctx->localidades_usadas)
			>= localidades_solicitadas) {
		inicio_localidades_allocadas = ctx->nodos_disponibles
				+ ctx->localidades_usadas;
		ctx->localidades_usadas += localidades_solicitadas;
	}
	return inicio_localidades_allocadas;
}

int caca_apuntador_valido(void *p) {
	return (p != NULL );
}

void arbol_avl_init(arbol_binario_contexto *ctx, tipo_dato *indices,
		tipo_dato *datos, int num_datos,
		nodo_arbol_binario **arreglo_referencias_nodos) {
	int i = 0;
	tipo_dato dato_actual = 0;
	nodo_arbol_binario *nodo_actual = NULL;

	memset((void * ) ctx, 0, sizeof(arbol_binario_contexto));
	memset((void * ) ctx->nodos_disponibles, 0, sizeof(ctx->nodos_disponibles));

	for (i = 0; i < num_datos; i++) {
		if (indices && *(indices + i) == ARBOL_AVL_INDICE_INVALIDO) {
			continue;
		}

		if (!ctx->raiz) {
			ctx->raiz = arbol_binario_nodo_allocar(ctx, 1);
			if (indices) {
				ctx->raiz->indice = *(indices + i);
			} else {
				ctx->raiz->indice = 1;
			}
			if (arreglo_referencias_nodos) {
				if (indices) {
					*(arreglo_referencias_nodos + *(indices + i)) = ctx->raiz;

				} else {
					*(arreglo_referencias_nodos + 1) = ctx->raiz;
				}
			}
			ctx->raiz->valor = *(datos + i);
			continue;
		}

		dato_actual = *(datos + i);
		nodo_actual = arbol_binario_nodo_allocar(ctx, 1);

		nodo_actual->valor = dato_actual;
		if (indices) {
			nodo_actual->indice = *(indices + i);
		} else {
			nodo_actual->indice = i + 1;
		}

		arbol_avl_insertar(&ctx->raiz, nodo_actual, ctx->no_indices_repetidos);
		if (arreglo_referencias_nodos) {
			if (indices) {
				*(arreglo_referencias_nodos + *(indices + i)) = nodo_actual;

			} else {
				*(arreglo_referencias_nodos + i + 1) = nodo_actual;
			}
		}
	}
}

void arbol_avl_insertar(nodo_arbol_binario **raiz,
		nodo_arbol_binario *nodo_a_insertar, bool no_indices_repetidos) {
	nodo_arbol_binario *raiz_int = NULL;

	raiz_int = *raiz;

	if (!raiz_int) {
		*raiz = nodo_a_insertar;
		caca_log_debug("se inserto %ld en raiz",
				ARBOL_AVL_GET_INDICE(nodo_a_insertar));
		return;
	}
	caca_log_debug("insertando %ld con ancestro %ld",
			ARBOL_AVL_GET_INDICE(nodo_a_insertar),
			ARBOL_AVL_GET_INDICE(raiz_int));

	switch (arbol_avl_compara_nodos(raiz_int, nodo_a_insertar)) {
	case CACA_COMPARACION_IZQ_MENOR:
		caca_log_debug("cagado a la izq")
		;
		if (raiz_int->hijo_der) {
			caca_log_debug("subcagado a la der");
			arbol_avl_insertar(&raiz_int->hijo_der, nodo_a_insertar,
					no_indices_repetidos);
		} else {
			raiz_int->hijo_der = nodo_a_insertar;
			ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_a_insertar, raiz_int);
		}
		break;
	case CACA_COMPARACION_IZQ_MAYOR:
		caca_log_debug("cagado a la izq")
		;
		if (raiz_int->hijo_izq) {
			caca_log_debug("subcagado a la der");
			arbol_avl_insertar(&raiz_int->hijo_izq, nodo_a_insertar,
					no_indices_repetidos);
		} else {
			raiz_int->hijo_izq = nodo_a_insertar;
			ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_a_insertar, raiz_int);
		}
		break;
	case CACA_COMPARACION_IZQ_IGUAL:
		if (no_indices_repetidos) {
			perror("NO mames, una llave repetida, ahhhhh !");
			exit(1);
		} else {
			if (raiz_int->hijo_der) {
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

	ARBOL_AVL_ACTUALIZAR_ALTURA(raiz_int);

	switch (arbol_avl_diferencia_alturas_subarboles(raiz_int, 1, verdadero)) {
	case ARBOL_AVL_ALTURA_CARGADA_IZQ:
		if (ARBOL_AVL_GET_VALOR(
				raiz_int->hijo_izq) < ARBOL_AVL_GET_VALOR(nodo_a_insertar)) {
			arbol_binario_rotar_izq(&raiz_int->hijo_izq);
		}
		arbol_binario_rotar_der(raiz);
		break;
	case ARBOL_AVL_ALTURA_CARGADA_DER:
		if (ARBOL_AVL_GET_VALOR(
				raiz_int->hijo_der) > ARBOL_AVL_GET_VALOR(nodo_a_insertar)) {
			arbol_binario_rotar_der(&raiz_int->hijo_der);
		}
		arbol_binario_rotar_izq(raiz);
		break;
	case ARBOL_AVL_ALTURA_BALANCEADA:
		break;
	default:
		perror("error en altura indeterminada, verga");
		exit(1);
		break;
	}

	caca_log_debug("Al finalizar insercion de %ld",
			ARBOL_AVL_GET_INDICE(nodo_a_insertar));
}

int arbol_avl_compara_nodos(nodo_arbol_binario *nodo1,
		nodo_arbol_binario *nodo2) {
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

int caca_int_max(int a, int b) {
	return (a > b) ? a : b;
}

void arbol_binario_rotar_izq(nodo_arbol_binario **nodo) {
	nodo_arbol_binario *nodo_int = NULL;
	nodo_arbol_binario *hijo_der = NULL;
	nodo_arbol_binario *hijo_der_subarbol_izq = NULL;

	nodo_int = *nodo;
	caca_log_debug("nodo pivote %ld %p", ARBOL_AVL_GET_INDICE(nodo_int),
			nodo_int);
	hijo_der = nodo_int->hijo_der;
	caca_log_debug("hijo der %ld %p", ARBOL_AVL_GET_INDICE(hijo_der), hijo_der);
	hijo_der_subarbol_izq = hijo_der->hijo_izq;
	caca_log_debug("hijo der sub izq %ld %p",
			ARBOL_AVL_GET_INDICE(hijo_der_subarbol_izq), hijo_der_subarbol_izq);

	nodo_int->hijo_der = hijo_der_subarbol_izq;
	hijo_der->hijo_izq = nodo_int;

	ARBOL_BINARIO_ACTUALIZAR_PADRE(hijo_der, ARBOL_AVL_GET_PADRE(nodo_int));
	ARBOL_BINARIO_ACTUALIZAR_PADRE(hijo_der_subarbol_izq, nodo_int);
	ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_int, hijo_der);

	ARBOL_AVL_ACTUALIZAR_ALTURA(nodo_int);
	ARBOL_AVL_ACTUALIZAR_ALTURA(hijo_der);

	*nodo = hijo_der;

}

nodo_arbol_binario *arbol_binario_nodo_allocar(arbol_binario_contexto *ctx,
		int localidades_solicitadas) {
	nodo_arbol_binario *inicio_localidades_allocadas = NULL;
	if ((sizeof(ctx->nodos_disponibles) - ctx->localidades_usadas)
			>= localidades_solicitadas) {
		inicio_localidades_allocadas = ctx->nodos_disponibles
				+ ctx->localidades_usadas;
		ctx->localidades_usadas += localidades_solicitadas;
	}
	return inicio_localidades_allocadas;
}

nodo_arbol_binario *arbol_binario_get_nodo_minimo_valor(
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

void arbol_avl_borrar(nodo_arbol_binario **raiz, tipo_dato valor_a_borrar) {

	nodo_arbol_binario *raiz_int = NULL;
	nodo_arbol_binario *nodo_min = NULL;

	raiz_int = *raiz;

	if (!raiz_int) {
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

					ARBOL_BINARIO_ACTUALIZAR_HIJO_IZQ(nodo_min,
							raiz_int->hijo_izq);
					ARBOL_BINARIO_ACTUALIZAR_HIJO_DER(nodo_min,
							raiz_int->hijo_der);
					ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_min, raiz_int->padre);
					nodo_min->altura = ARBOL_AVL_GET_ALTURA(raiz_int);
					ARBOL_BINARIO_ACTUALIZAR_PADRE(raiz_int->hijo_izq,
							nodo_min);
					ARBOL_BINARIO_ACTUALIZAR_PADRE(raiz_int->hijo_der,
							nodo_min);
					*raiz = nodo_min;
				}
			}
		}
	}

	if (!*raiz) {
		return;
	}

	ARBOL_AVL_ACTUALIZAR_ALTURA(raiz_int);

	switch (arbol_avl_diferencia_alturas_subarboles(raiz_int, 1, verdadero)) {
	case ARBOL_AVL_ALTURA_CARGADA_IZQ:
		if (arbol_avl_diferencia_alturas_subarboles(raiz_int->hijo_izq, 0,
				falso) == ARBOL_AVL_ALTURA_CARGADA_DER) {
			arbol_binario_rotar_izq(&raiz_int->hijo_izq);
		}
		arbol_binario_rotar_der(raiz);
		break;
	case ARBOL_AVL_ALTURA_CARGADA_DER:
		if (arbol_avl_diferencia_alturas_subarboles(raiz_int->hijo_der, 0,
				verdadero) == ARBOL_AVL_ALTURA_CARGADA_IZQ) {
			arbol_binario_rotar_der(&raiz_int->hijo_der);
		}
		arbol_binario_rotar_izq(raiz);
		break;
	case ARBOL_AVL_ALTURA_BALANCEADA:
		break;
	default:
		break;
	}

}

void arbol_avl_borrar_referencia_directa(nodo_arbol_binario **raiz,
		nodo_arbol_binario *nodo_a_borrar, nodo_arbol_binario *tope) {

	nodo_arbol_binario *ancestro_actual = NULL;
	nodo_arbol_binario *ancestro_actual_tmp = NULL;
	nodo_arbol_binario *nodo_a_borrar_padre = NULL;
	nodo_arbol_binario *nodo_min = NULL;

	nodo_arbol_binario **ancestro_actual_apuntador = NULL;
	nodo_arbol_binario **nodo_a_borrar_ref = NULL;

	if (!nodo_a_borrar) {
		abort();
	}
	caca_log_debug("nodod a borrar %ld %p", ARBOL_AVL_GET_INDICE(nodo_a_borrar),
			nodo_a_borrar);

	if (!(nodo_a_borrar_padre = ARBOL_AVL_GET_PADRE(nodo_a_borrar))) {
		nodo_a_borrar_padre = NULL;
		nodo_a_borrar_ref = raiz;
		caca_log_debug("se esta borrando la razi");
	} else {

		nodo_a_borrar_ref =
				nodo_a_borrar->valor < (nodo_a_borrar_padre)->valor ?
						&nodo_a_borrar_padre->hijo_izq :
				nodo_a_borrar->valor > (nodo_a_borrar_padre)->valor ?
						&nodo_a_borrar_padre->hijo_der :
				nodo_a_borrar->indice
						== ARBOL_AVL_GET_INDICE(nodo_a_borrar_padre->hijo_izq) ?
						&nodo_a_borrar_padre->hijo_izq :
						&nodo_a_borrar_padre->hijo_der;

		caca_log_debug("se borrara ijo %ld de padre %ld",
				ARBOL_AVL_GET_INDICE(*nodo_a_borrar_ref),
				ARBOL_AVL_GET_INDICE(nodo_a_borrar_padre ));
	}

	ancestro_actual = nodo_a_borrar->padre;
	ancestro_actual_apuntador = &nodo_a_borrar->padre;

	if (!nodo_a_borrar->hijo_izq) {
		caca_log_debug("borrando sin ijo izq");
		ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_a_borrar->hijo_der,
				ancestro_actual);
		*nodo_a_borrar_ref = nodo_a_borrar->hijo_der;
	} else {
		if (!nodo_a_borrar->hijo_der) {
			caca_log_debug("borrando sin ijo der");
			ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_a_borrar->hijo_izq,
					ancestro_actual);
			*nodo_a_borrar_ref = nodo_a_borrar->hijo_izq;
		} else {
			nodo_min = arbol_binario_get_nodo_minimo_valor(
					nodo_a_borrar->hijo_der);

			caca_log_debug("%ld,%ld tomara el lugar",
					ARBOL_AVL_GET_INDICE(nodo_min),
					ARBOL_AVL_GET_VALOR(nodo_min));
			arbol_avl_borrar_referencia_directa(raiz, nodo_min, nodo_a_borrar);
			ARBOL_BINARIO_ACTUALIZAR_HIJO_IZQ(nodo_min,
					nodo_a_borrar->hijo_izq);
			ARBOL_BINARIO_ACTUALIZAR_HIJO_DER(nodo_min,
					nodo_a_borrar->hijo_der);
			ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_min, nodo_a_borrar->padre);
			nodo_min->altura = ARBOL_AVL_GET_ALTURA(nodo_a_borrar);

			ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_a_borrar->hijo_izq, nodo_min);
			ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_a_borrar->hijo_der, nodo_min);

			*nodo_a_borrar_ref = nodo_min;
			ancestro_actual = nodo_min;
		}
	}

	do {
		if (!ARBOL_AVL_GET_PADRE(nodo_a_borrar)) {
			caca_log_debug("ya no ay mas padres q recorrer");
			break;
		}
		caca_log_debug("ancestro actual %ld:%p",
				ARBOL_AVL_GET_INDICE(ancestro_actual), ancestro_actual);
		if (tope && ancestro_actual == tope) {
			caca_log_debug("Se alcanzo el tope %ld,%p",
					ARBOL_AVL_GET_INDICE(tope), tope);
			return;
		}
		ancestro_actual_tmp = ancestro_actual->padre;

		if (ARBOL_AVL_GET_PADRE(ancestro_actual)) {
			if (ARBOL_AVL_GET_VALOR(
					ancestro_actual)
					<= ARBOL_AVL_GET_VALOR(ancestro_actual->padre)) {
				ancestro_actual_apuntador = &ancestro_actual->padre->hijo_izq;
				caca_log_debug("tomando referencia izq de ancestro");
			} else {
				ancestro_actual_apuntador = &ancestro_actual->padre->hijo_der;
				caca_log_debug("tomando referencia der de ancestro");
			}
		} else {
			caca_log_debug("tratando con la raiz");
			ancestro_actual_apuntador = raiz;
		};

		ARBOL_AVL_ACTUALIZAR_ALTURA(ancestro_actual);

		switch (arbol_avl_diferencia_alturas_subarboles(ancestro_actual, 1,
				verdadero)) {
		case ARBOL_AVL_ALTURA_CARGADA_IZQ:
			caca_log_debug("altura cagada izq")
			;
			if (arbol_avl_diferencia_alturas_subarboles(
					ancestro_actual->hijo_izq, 0,
					falso) == ARBOL_AVL_ALTURA_CARGADA_DER) {
				caca_log_debug("altura subcagada der");
				arbol_binario_rotar_izq(&ancestro_actual->hijo_izq);
			}
			arbol_binario_rotar_der(ancestro_actual_apuntador);
			break;
		case ARBOL_AVL_ALTURA_CARGADA_DER:
			caca_log_debug("altura cagada der")
			;
			if (arbol_avl_diferencia_alturas_subarboles(
					ancestro_actual->hijo_der, 0,
					verdadero) == ARBOL_AVL_ALTURA_CARGADA_IZQ) {
				caca_log_debug("altura subcagada izq");
				arbol_binario_rotar_der(&ancestro_actual->hijo_der);
			}
			arbol_binario_rotar_izq(ancestro_actual_apuntador);
			break;
		case ARBOL_AVL_ALTURA_BALANCEADA:
			caca_log_debug("altura balanceada")
			;
			break;
		default:
			break;
		}

		ancestro_actual = ancestro_actual_tmp;
	} while (ancestro_actual);
	caca_log_debug("termino de borrar %ld", ARBOL_AVL_GET_INDICE(nodo_a_borrar));
}

void cola_prioridad_modificar_valor_nodo(cola_prioridad_contexto *cpctx,
		tipo_dato indice, tipo_dato nuevo_valor) {
	nodo_arbol_binario *referencia_directa = NULL;
	nodo_arbol_binario **referencias_directas = NULL;

	referencias_directas = cpctx->referencias_directas_por_indice;

	referencia_directa = *(referencias_directas + indice);

	caca_log_debug("borrando x ref dir %ld %ld",
			ARBOL_AVL_GET_INDICE(referencia_directa),
			ARBOL_AVL_GET_VALOR(referencia_directa));
	referencia_directa->valor = nuevo_valor;

}

void dijkstra_relaxar_nodo(grafo_contexto *gctx, cola_prioridad_contexto *cpctx,
		tipo_dato ind_nodo_origen, tipo_dato ind_nodo_destino,
		tipo_dato *antecesores, nodo_cola_prioridad *nodo_origen) {
	tipo_dato dist_origen_dest = 0;

	tipo_dato *matrix_distancias = NULL;

	nodo_arbol_binario *distancia_min_origen = NULL;
	nodo_arbol_binario *distancia_min_destino = NULL;
	nodo_arbol_binario **distancias_minimas = NULL;

	matrix_distancias = (tipo_dato*) gctx->matrix_distancias;
	dist_origen_dest = *(matrix_distancias
			+ ind_nodo_origen * MAX_COLUMNAS_NODOS + ind_nodo_destino);
	caca_log_debug("distancia de %ld a %ld es %ld", ind_nodo_origen,
			ind_nodo_destino, dist_origen_dest);

	distancias_minimas = cpctx->referencias_directas_por_indice;

	distancia_min_origen = *(distancias_minimas + ind_nodo_origen);
	if (nodo_origen) {
		distancia_min_origen = nodo_origen;
	}
	distancia_min_destino = *(distancias_minimas + ind_nodo_destino);

	if (distancia_min_destino->valor
			> distancia_min_origen->valor + dist_origen_dest) {
		caca_log_debug("la distancia nueva %ld fue menor q %ld",
				distancia_min_origen->valor + dist_origen_dest,
				distancia_min_destino->valor);
		cola_prioridad_modificar_valor_nodo(cpctx, ind_nodo_destino,
				distancia_min_origen->valor + dist_origen_dest);
		if (antecesores) {
			*(antecesores + ind_nodo_destino) = ind_nodo_origen;
		}
	}
}
void dijkstra_main(void *matrix_distancias, int num_filas,
		tipo_dato ind_nodo_origen, tipo_dato ind_nodo_destino,
		grafo_contexto *gctx, tipo_dato *distancias_minimas,
		tipo_dato *antecesores) {

	int i, j;

	int contador = 0;
	int num_nodos = 0;

	tipo_dato indice_origen_actual = 0;
	tipo_dato indice_destino_actual = 0;
	tipo_dato distancia_actual = 0;
	tipo_dato max_indice = 0;
	tipo_dato *matrix_distancias_int = NULL;

	grafo_contexto gctx_mem;
	grafo_contexto *gctx_int;
	cola_prioridad_contexto cpctx;

	nodo *nodo_origen_actual = NULL;
	nodo_cola_prioridad *nodo_mas_cercas = NULL;

	bool nodos_distancias_minimas_calculadas[MAX_NODOS] = { falso };
	nodo_cola_prioridad distancias_minimas_nodos[MAX_NODOS];
	nodo_cola_prioridad *distancias_minimas_nodos_calculadas[MAX_NODOS];

	caca_log_debug("fairy tail\n");

	caca_inutiliza_nodo_cola_prioridad(distancias_minimas_nodos, MAX_NODOS);

	if (gctx) {
		gctx_int = gctx;
	} else {
		gctx_int = &gctx_mem;
		init_grafo(matrix_distancias, num_filas, gctx_int, falso, verdadero);
	}
	matrix_distancias_int = (tipo_dato *) gctx_int->matrix_distancias;
	caca_log_debug("initializado matrix distancias %p , %ld\n",
			matrix_distancias_int, *matrix_distancias_int);

	caca_log_debug("initiado grafo tamanio matrix distancias %ld\n",
			sizeof(gctx_int->matrix_distancias));

	nodo_origen_actual = gctx_int->inicio;

	caca_log_debug("antes de recorrer nodos origenes\n");
	while (nodo_origen_actual) {
		if (nodo_origen_actual->indice == ind_nodo_origen) {
			(distancias_minimas_nodos + nodo_origen_actual->indice)->valor = 0;
		} else {
			(distancias_minimas_nodos + nodo_origen_actual->indice)->valor =
					MAX_VALOR;
		}
		if (nodo_origen_actual->indice > max_indice) {
			max_indice = nodo_origen_actual->indice;
			if (max_indice > MAX_NODOS) {
				perror(
						"El indice maximo supera el maximo de nodos permitidos, a la mierda todo");
				abort();
			}
		}
		(distancias_minimas_nodos + nodo_origen_actual->indice)->indice =
				nodo_origen_actual->indice;
		GRAFO_AVANZAR_NODO(nodo_origen_actual, GRAFO_PRINCIPAL, falso);
		contador++;
	}
	num_nodos = contador;

	caca_log_debug("antes de niciar cola\n");
	cola_prioridad_init(&cpctx, distancias_minimas_nodos, NULL, NULL,
			max_indice + 1, NULL, NULL );
	caca_log_debug("empezando algoritmo");

	contador = 0;
	while (!cola_prioridad_es_vacia(&cpctx)) {
		nodo_mas_cercas = cola_prioridad_pop(&cpctx);
		nodos_distancias_minimas_calculadas[nodo_mas_cercas->indice] =
				verdadero;
		*(distancias_minimas_nodos_calculadas + nodo_mas_cercas->indice) =
				nodo_mas_cercas;

		indice_origen_actual = nodo_mas_cercas->indice;
		caca_log_debug("tratando origen %ld\n", indice_origen_actual);

		for (j = 0; j < MAX_COLUMNAS_NODOS; j++) {
			distancia_actual = (tipo_dato) *(matrix_distancias_int
					+ indice_origen_actual * MAX_FILAS_NODOS + j);
			indice_destino_actual = j;
			if (distancia_actual != GRAFO_VALOR_INVALIDO
					&& !(*(nodos_distancias_minimas_calculadas
							+ indice_destino_actual))) {
				caca_log_debug("relaxando destino %ld distancia %ld\n",
						indice_destino_actual, distancia_actual);
				dijkstra_relaxar_nodo(gctx_int, &cpctx, indice_origen_actual,
						indice_destino_actual, antecesores, nodo_mas_cercas);
			}
		}

		contador++;
	}
	caca_log_debug("calculadas chingaderas\n");
	*(antecesores + ind_nodo_origen) = 0;
	for (i = 0; i < max_indice + 1; i++) {
		*(distancias_minimas + i) =
				i == ind_nodo_origen ? 0 :
				caca_apuntador_valido(
						(*(distancias_minimas_nodos_calculadas + i))) ?
						(*(distancias_minimas_nodos_calculadas + i))->valor :
						COLA_PRIORIDAD_VALOR_INVALIDO;
	}
	caca_log_debug("saliendo de la mierda \n");

}

void cola_prioridad_init(cola_prioridad_contexto *ctx,
		nodo_cola_prioridad *nodos, tipo_dato *valores, tipo_dato *indices,
		int num_nodos, arbol_binario_contexto *actx,
		nodo_arbol_binario **referencias_directas) {
	int i;
	int num_nodos_int = 0;
	nodo_cola_prioridad **nodos_int = NULL;

	memset(ctx, 0, sizeof(cola_prioridad_contexto));
	memset(ctx->referencias_directas_por_indice_mem, 0,
			sizeof(ctx->referencias_directas_por_indice_mem));
	nodos_int =
			(nodo_cola_prioridad **) &ctx->referencias_directas_por_indice_mem;

	if (actx) {
		if (!referencias_directas) {
			perror("se paso un contexto de arbol sin referencias directas");
			exit(1);
		}
		ctx->referencias_directas_por_indice = referencias_directas;
		num_nodos_int = num_nodos;
	} else {
		for (i = 0; i < num_nodos; i++) {
			if (nodos) {
				if ((nodos + i)->indice == COLA_PRIORIDAD_VALOR_INVALIDO) {
					continue;
				}
				*(nodos_int + ARBOL_AVL_GET_INDICE((nodos + i))) = (nodos + i);
				caca_log_debug("indice %ld valor %ld con nodos provistos",
						ARBOL_AVL_GET_INDICE(*(nodos_int + (nodos + i)->indice)),
						ARBOL_AVL_GET_VALOR(*(nodos_int + (nodos + i)->indice)));
			} else {
				if (!valores) {
					perror("no se proporcionaron nodos ni valores");
					exit(1);
				}
				if (indices) {
					*(nodos_int + *(indices + i)) = calloc(1,
							sizeof(nodo_cola_prioridad));
					if (!*(nodos_int + *(indices + i))) {
						perror(
								"No se pudo obtener mem para nodo cp nuevo con indices");
					}
					COLA_PRIORIDAD_ASIGNA_VALOR(*(nodos_int + *(indices + i)),
							*(valores + i));
					COLA_PRIORIDAD_ASIGNA_INDICE(*(nodos_int + *(indices + i)),
							*(indices + i));
					caca_log_debug(
							"indice %ld valor %ld con nodos no provistos",
							ARBOL_AVL_GET_INDICE(*(nodos_int +*(indices+i) )),
							ARBOL_AVL_GET_VALOR(*(nodos_int +*(indices+i) )));
				} else {
					*(nodos_int + i + 1) = calloc(1,
							sizeof(nodo_cola_prioridad));
					if (!*(nodos_int + i + 1)) {
						perror(
								"No se pudo obtener mem para nodo cp nuevo sin indices");
					}
					COLA_PRIORIDAD_ASIGNA_VALOR(*(nodos_int + i + 1),
							*(valores + i));
					COLA_PRIORIDAD_ASIGNA_INDICE(*(nodos_int + i + 1), i + 1);
					caca_log_debug(
							"indice %ld valor %ld con nodos ni indices no provistos",
							ARBOL_AVL_GET_INDICE(*(nodos_int + i + 1)),
							ARBOL_AVL_GET_VALOR(*(nodos_int + i + 1)));
				}
			}
			num_nodos_int++;
		}

		ctx->referencias_directas_por_indice =
				ctx->referencias_directas_por_indice_mem;
		ctx->num_nodos = num_nodos_int;

	}

}

nodo_cola_prioridad *cola_prioridad_pop(cola_prioridad_contexto *ctx) {
	bool alguno_no_es_maximo = falso;
	int i = 0;
	tipo_dato indice_min = 0;
	tipo_dato valor_min = MAX_VALOR;
	nodo_cola_prioridad *nodo_encontrado = NULL;
	nodo_cola_prioridad *primer_nodo_no_vacio = NULL;

	for (i = 0; i < MAX_NODOS; i++) {
		nodo_encontrado = *(ctx->referencias_directas_por_indice + i);
		if (nodo_encontrado) {
			caca_log_debug("intentando nodo %ld %ld",
					ARBOL_AVL_GET_INDICE(nodo_encontrado),
					ARBOL_AVL_GET_VALOR(nodo_encontrado ));
			if (!primer_nodo_no_vacio) {
				primer_nodo_no_vacio = nodo_encontrado;
			}
			if (ARBOL_AVL_GET_VALOR(nodo_encontrado) < valor_min) {
				valor_min = ARBOL_AVL_GET_VALOR(nodo_encontrado);
				indice_min = ARBOL_AVL_GET_INDICE(nodo_encontrado);
				alguno_no_es_maximo = verdadero;
			}
		}
	}

	caca_log_debug("al salir del loop indice min %ld valor min %ld", indice_min,
			valor_min);

	if (indice_min < MAX_NODOS && alguno_no_es_maximo) {
		caca_log_debug("nodo indice %ld sera borrado", indice_min);
		nodo_encontrado = *(ctx->referencias_directas_por_indice + indice_min);
		*(ctx->referencias_directas_por_indice + indice_min) = NULL;
		ctx->num_nodos--;
		caca_log_debug("aora los nodos son %d", ctx->num_nodos);
	} else {
		if (primer_nodo_no_vacio) {
			caca_log_debug(
					"x lo visto todos los nodos tienen el maximo,utilizando el primero q c encontro %ld %ld",
					ARBOL_AVL_GET_INDICE(primer_nodo_no_vacio),
					ARBOL_AVL_GET_INDICE(primer_nodo_no_vacio));
			nodo_encontrado = primer_nodo_no_vacio;
			*(ctx->referencias_directas_por_indice
					+ ARBOL_AVL_GET_INDICE(primer_nodo_no_vacio)) = NULL;
			ctx->num_nodos--;
			caca_log_debug("aora los nodos son %d", ctx->num_nodos);
		} else {
			caca_log_debug("q pedo, intantando popear una cola vacia!?");
		}
	}

	return nodo_encontrado;
}

void cola_prioridad_get_valores(cola_prioridad_contexto *ctx,
		tipo_dato *valores, int *num_valores) {
	int i;
	int contador_nodos_validos = 0;
	nodo_cola_prioridad *nodo_actual = NULL;
	for (i = 0; i < MAX_NODOS; i++) {
		nodo_actual = *(ctx->referencias_directas_por_indice + i);
		if (nodo_actual) {
			caca_log_debug("el nodo %d indice %ld tiene valor %ld", i,
					nodo_actual->indice, nodo_actual->valor);
			*(valores + contador_nodos_validos) = nodo_actual->valor;
			contador_nodos_validos++;
		}
	}
	*num_valores = contador_nodos_validos;
}

bool cola_prioridad_es_vacia(cola_prioridad_contexto *ctx) {
	return !ctx->num_nodos;
}

void grafo_copia_profunda(const grafo_contexto *ctx_origen,
		grafo_contexto *ctx_destino, tipo_dato *indices_a_ignorar,
		int tam_indices_a_ignorar) {
	int i, j;
	tipo_dato indice_nodo_origen_actual = 0;

	tipo_dato *matrix_origen = NULL;
	tipo_dato *matrix_destino = NULL;

	nodo *nodo_origen_principal_actual = NULL;
	nodo *nodo_destino_principal_actual = NULL;
	nodo *nodo_destino_principal_previo = NULL;

	caca_log_debug("dentro de copia profunda %p\n", ctx_origen);

	matrix_origen = (tipo_dato*) ctx_origen->matrix_distancias;
	matrix_destino = (tipo_dato *) ctx_destino->matrix_distancias;
	memcpy(matrix_destino, matrix_origen,
			sizeof(ctx_origen->matrix_distancias));

	caca_log_debug("copio matrix %p, %ld\n", matrix_destino, *matrix_destino);

	nodo_origen_principal_actual = ctx_origen->inicio;
	caca_log_debug("naruto %p\n", nodo_origen_principal_actual);
	while (nodo_origen_principal_actual) {
		indice_nodo_origen_actual = nodo_origen_principal_actual->indice;
		caca_log_debug("viendo si copiara %ld\n", indice_nodo_origen_actual);
		if (indices_a_ignorar
				&& caca_arreglo_contiene(indices_a_ignorar,
						tam_indices_a_ignorar, indice_nodo_origen_actual)) {

			caca_log_debug("ignorando %ld\n", indice_nodo_origen_actual);
			for (i = 0; i < MAX_FILAS_NODOS; i++) {
				*(matrix_destino + i * MAX_COLUMNAS_NODOS
						+ indice_nodo_origen_actual) = GRAFO_VALOR_INVALIDO;
			}
			for (j = 0; j < MAX_COLUMNAS_NODOS; j++) {
				*(matrix_destino
						+ indice_nodo_origen_actual * MAX_COLUMNAS_NODOS + j) =
						GRAFO_VALOR_INVALIDO;
			}
			GRAFO_AVANZAR_NODO(nodo_origen_principal_actual, 0, 0);
			caca_log_debug("ignorado %ld\n", indice_nodo_origen_actual);
			continue;
		}
		caca_log_debug("coplando %ld\n", indice_nodo_origen_actual);
		nodo_destino_principal_actual = grafo_nodo_alloc(ctx_destino, 1);
		memset(nodo_destino_principal_actual, 0, sizeof(nodo));
		grafo_copia_nodo(nodo_origen_principal_actual,
				nodo_destino_principal_actual);
		if (nodo_destino_principal_previo) {
			nodo_destino_principal_previo->siguiente =
					nodo_destino_principal_actual;
			nodo_destino_principal_actual->anterior =
					nodo_destino_principal_previo;
		} else {
			ctx_destino->inicio = nodo_destino_principal_actual;
		}
		nodo_destino_principal_previo = nodo_destino_principal_actual;

		GRAFO_AVANZAR_NODO(nodo_origen_principal_actual, 0, 0);
	}

}

#define GRAFO_COPIA_PROPIEDAD(propiedad) nodo_destino->propiedad = nodo_origen->propiedad
void grafo_copia_nodo(const nodo *nodo_origen, nodo *nodo_destino) {
	GRAFO_COPIA_PROPIEDAD(distancia);
	GRAFO_COPIA_PROPIEDAD(indice);
	GRAFO_COPIA_PROPIEDAD(num_nodos_asociados);
	GRAFO_COPIA_PROPIEDAD(num_nodos_asociados_distancia);
	GRAFO_COPIA_PROPIEDAD(num_nodos_asociados_indice);
	GRAFO_COPIA_PROPIEDAD(num_nodos_asociados_valor);
	GRAFO_COPIA_PROPIEDAD(valor);
}

bool caca_arreglo_contiene(tipo_dato *arreglo, int tam_arreglo,
		tipo_dato valor_buscado) {
	int i;
	for (i = 0; i < tam_arreglo; i++) {
		caca_log_debug("comprarando %ld con %ld", *(arreglo + i), valor_buscado);
		if (*(arreglo + i) == valor_buscado) {
			return verdadero;
		}
	}
	return falso;
}

#define GRAFO_GET_APUNTADOR_POSICION_MATRIX(apuntador_destino,posicion_filas,posicion_columnas) \
			if (es_array) { \
				apuntador_destino= (matrix_array + posicion_filas * num_columnas + posicion_columnas); \
			} else { \
				apuntador_destino= (*(matrix_pointer + posicion_filas) + posicion_columnas); \
			}

void caca_inutiliza_nodo_cola_prioridad(nodo_cola_prioridad *nodos,
		int num_nodos) {
	int i;
	for (i = 0; i < num_nodos; i++) {
		COLA_PRIORIDAD_ASIGNA_INDICE((nodos + i),
				COLA_PRIORIDAD_VALOR_INVALIDO);
		COLA_PRIORIDAD_ASIGNA_VALOR((nodos + i), COLA_PRIORIDAD_VALOR_INVALIDO);
	}
}

void caca_realinea_array(tipo_dato *arreglo, int num_filas,
		int alineacion_actual, int alineacion_nueva) {
	int i, j;
	for (i = 0; i < num_filas; i++) {
		for (j = 0; j < alineacion_nueva; j++) {
			*(arreglo + i * alineacion_nueva + j) = *(arreglo
					+ i * alineacion_actual + j);
		}

	}
}

int arbol_avl_diferencia_alturas_subarboles(nodo_arbol_binario *nodo,
		int tolerancia, bool considerar_balanceado_cargado_der) {
	int diferencia_alturas = 0;
	diferencia_alturas = ARBOL_AVL_GET_ALTURA(
			nodo->hijo_izq) - ARBOL_AVL_GET_ALTURA(nodo->hijo_der);
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

void arbol_binario_rotar_der(nodo_arbol_binario **nodo) {
	nodo_arbol_binario *nodo_int = NULL;
	nodo_arbol_binario *hijo_izq = NULL;
	nodo_arbol_binario *hijo_izq_subarbol_der = NULL;

	nodo_int = *nodo;
	caca_log_debug("nodo pivote %ld %p", ARBOL_AVL_GET_INDICE(nodo_int),
			nodo_int);
	hijo_izq = nodo_int->hijo_izq;
	caca_log_debug("hijo izq %ld %p", ARBOL_AVL_GET_INDICE(hijo_izq), hijo_izq);
	hijo_izq_subarbol_der = hijo_izq->hijo_der;
	caca_log_debug("hijo izq sub der %ld %p",
			ARBOL_AVL_GET_INDICE(hijo_izq_subarbol_der), hijo_izq_subarbol_der);

	nodo_int->hijo_izq = hijo_izq_subarbol_der;
	hijo_izq->hijo_der = nodo_int;

	ARBOL_BINARIO_ACTUALIZAR_PADRE(hijo_izq, ARBOL_AVL_GET_PADRE(nodo_int));
	ARBOL_BINARIO_ACTUALIZAR_PADRE(hijo_izq_subarbol_der, nodo_int);
	ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo_int, hijo_izq);

	ARBOL_AVL_ACTUALIZAR_ALTURA(nodo_int);
	ARBOL_AVL_ACTUALIZAR_ALTURA(hijo_izq);

	*nodo = hijo_izq;

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

char *caca_arreglo_a_cadena(tipo_dato *arreglo, int tam_arreglo, char *buffer) {
	int i;
	char *ap_buffer = NULL;
	int characteres_escritos = 0;

	ap_buffer = buffer;

	for (i = 0; i < tam_arreglo; i++) {
		characteres_escritos += sprintf(ap_buffer + characteres_escritos, "%ld",
				*(arreglo + i));
		if (i < tam_arreglo - 1) {
			*(ap_buffer + characteres_escritos++) = ',';
		}
	}
	return ap_buffer;
}

int caca_imprime_matrix(void *matrix, int num_filas, int *num_columnas,
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

	es_array = verdadero;

	caca_log_debug("determinado q es array %d", es_array);

	if (!num_columnas && !num_columnas_fijo) {
		caca_log_debug("No mames no ay nada con q trabajar");
	}

	num_columnas_actual = num_columnas_fijo ? num_columnas_fijo : 0;

	for (i = 0; i < num_filas; i++) {
		caca_log_debug("En la fila %d de %d", i, num_filas);
		if (num_columnas) {
			num_columnas_actual = *(num_columnas + i);
		}
		if (num_columnas_fijo) {
			num_columnas_actual = num_columnas_fijo;
		}
		for (j = 0; j < num_columnas_actual; j++) {
			if (es_array) {
				numero_actual = *(matrix_array + i * num_columnas_actual + j);
			} else {
				caca_log_debug("copiando apuntadores bidimensional");
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

bool from_stack(void *ptr) {
	return verdadero;
}

void grafo_get_representacion_en_matriz_ordenada(grafo_contexto *ctx,
		void *matrix, int num_columnas) {
	bool es_array = falso;
	int contador_filas = 0;
	int contador_columnas = 0;
	void **apuntador_matrix = NULL;
	tipo_dato *matrix_array = NULL;
	tipo_dato **matrix_pointer = NULL;
	tipo_dato *apuntador_a_posicion_en_matrix = NULL;
	nodo *nodo_actual = NULL;
	nodo *nodo_adjacente_actual = NULL;

	nodo_actual = ctx->inicio;
	apuntador_matrix = matrix;
	matrix_array = (tipo_dato *) apuntador_matrix;
	matrix_pointer = (tipo_dato **) apuntador_matrix;

	es_array = verdadero;

	caca_log_debug("Transformando listas en matrix:");

	while (nodo_actual) {
		nodo_adjacente_actual = nodo_actual;
		GRAFO_AVANZAR_NODO(nodo_adjacente_actual, GRAFO_INDICE, verdadero);
		contador_columnas = 0;

		GRAFO_GET_APUNTADOR_POSICION_MATRIX(apuntador_a_posicion_en_matrix,
				contador_filas, contador_columnas);

		caca_log_debug(
				"el apuntador destino del nodo principal %p en %d %d, cuyo valor es %ld",
				apuntador_a_posicion_en_matrix, contador_filas,
				contador_columnas, nodo_actual->indice);

		*apuntador_a_posicion_en_matrix = nodo_actual->indice;

		contador_columnas++;

		while (nodo_adjacente_actual) {

			GRAFO_GET_APUNTADOR_POSICION_MATRIX(apuntador_a_posicion_en_matrix,
					contador_filas, contador_columnas);

			caca_log_debug("en adjacente indice %d %d", contador_filas,
					contador_columnas);
			*apuntador_a_posicion_en_matrix = nodo_adjacente_actual->indice;

			GRAFO_AVANZAR_NODO(nodo_adjacente_actual, GRAFO_INDICE, verdadero);
			contador_columnas++;
		}

		contador_filas++;
		contador_columnas = 0;

		GRAFO_GET_APUNTADOR_POSICION_MATRIX(apuntador_a_posicion_en_matrix,
				contador_filas, contador_columnas);
		*apuntador_a_posicion_en_matrix = nodo_actual->indice;

		contador_columnas++;

		nodo_adjacente_actual = nodo_actual;
		GRAFO_AVANZAR_NODO(nodo_adjacente_actual, GRAFO_DISTANCIA, verdadero);
		while (nodo_adjacente_actual) {
			GRAFO_GET_APUNTADOR_POSICION_MATRIX(apuntador_a_posicion_en_matrix,
					contador_filas, contador_columnas);

			caca_log_debug("en adjacente distancia %d %d", contador_filas,
					contador_columnas);
			*apuntador_a_posicion_en_matrix = nodo_adjacente_actual->indice;

			GRAFO_AVANZAR_NODO(nodo_adjacente_actual, GRAFO_DISTANCIA,
					verdadero);
			contador_columnas++;
		}

		nodo_actual = nodo_actual->siguiente;

		contador_filas++;
	}
	caca_log_debug("termino representacion en matrix");
}

void imprimir_lista_adjacencia(nodo *nodo_inicial) {
	const char *FORMATO_CONCAT_NODO = " --> ";
	int caracteres_escritos = 0;
	char *ap_cadena = NULL;
	char *buffer = NULL;
	nodo *nodo_actual = NULL;
	nodo *nodo_adjacente_actual = NULL;

	nodo_actual = nodo_inicial;

	caca_log_debug("La matrix de adjacencia es:");

	buffer = calloc(1000, sizeof(char));

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

char *grafo_nodo_a_cadena(nodo *node, char *cadena_buffer,
		int *characteres_escritos) {

	if (!node) {
		perror("q pedo, pasaron un nodo vacio!");
		abort();
	}

	if (characteres_escritos) {
		*characteres_escritos = sprintf(cadena_buffer,
				"{valor:%ld,indice:%ld,distancia:%ld} (%p) ", node->valor,
				node->indice, node->distancia, node);
	} else {

		sprintf(cadena_buffer, "{valor:%ld,indice:%ld,distancia:%ld} (%p) ",
				node->valor, node->indice, node->distancia, node);
	}

	return cadena_buffer;
}

void caca_log_debug_func(const char *format, ...) {
	va_list arg;
	va_list arg2;
	const char *PEDAZO_TIMESTAMP_HEADER = "tiempo: %s; ";
	const char *HEADER =
			"archivo: %s; funcion: %s; linea %d; nivel: %zd caca 8====D ";
	char formato[MAX_TAM_CADENA + sizeof(HEADER)
			+ sizeof(PEDAZO_TIMESTAMP_HEADER)] = { '\0' };
	char pedazo_timestamp[sizeof(PEDAZO_TIMESTAMP_HEADER) + 100] = { '\0' };
	char cadena_timestamp[100] = { '\0' };

	timestamp_caca(cadena_timestamp);
	sprintf(pedazo_timestamp, PEDAZO_TIMESTAMP_HEADER, cadena_timestamp);

	strcpy(formato, pedazo_timestamp);
	strcat(formato, HEADER);
	strcat(formato, format);

	if (!zlog_inicializado) {
		init_zlog("/Users/ernesto/workspace/unos_locos/zlog.conf");
	}

	if (!cacategoria) {
		INIT_ZLOG_CATEGORY(cacategoria, "cacacomun");
	}

	va_start(arg, format);
	va_copy(arg2, arg);
	vzlog_debug(cacategoria, formato, arg2);
	va_end(arg2);
	va_end(arg);
}

void init_zlog(const char *arch_conf) {
	int rc = 0;
	rc = zlog_init(arch_conf);
	if (rc) {
		caca_log_debug("init failed\n");
		exit(EXIT_FAILURE);
	}
	zlog_inicializado = verdadero;
}

void timestamp_caca(char *stime) {
	time_t ltime;
	struct tm result;
	long ms;
	struct timespec spec;
	char parte_milisecundos[50];

	ltime = time(NULL );

	localtime_r(&ltime, &result);
	asctime_r(&result, stime);

	*(stime + strlen(stime) - 1) = ' ';

	current_utc_time(&spec);
	ms = round(spec.tv_nsec / 1.0e3);
	sprintf(parte_milisecundos, "%ld", ms);
	strcat(stime, parte_milisecundos);
}

void current_utc_time(struct timespec *ts) {

#ifdef __MACH__
	clock_serv_t cclock;
	mach_timespec_t mts;
	host_get_clock_service(mach_host_self(), CALENDAR_CLOCK, &cclock);
	clock_get_time(cclock, &mts);
	mach_port_deallocate(mach_task_self(), cclock);
	ts->tv_sec = mts.tv_sec;
	ts->tv_nsec = mts.tv_nsec;
#else
	clock_gettime(CLOCK_REALTIME, ts);
#endif

}

char *arbol_binario_nodo_a_cadena(nodo_arbol_binario *node, char *cadena_buffer,
		int *characteres_escritos) {

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

void arbol_binario_recorrido_preoder(nodo_arbol_binario *raiz) {
	char buffer[MAX_TAM_CADENA];
	if (!raiz) {
		return;
	}
	caca_log_debug("%s ", arbol_binario_nodo_a_cadena(raiz,buffer,NULL));
	arbol_binario_recorrido_preoder(raiz->hijo_izq);
	arbol_binario_recorrido_preoder(raiz->hijo_der);
}
