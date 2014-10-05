/*
 * cacacomun.h
 *
 *  Created on: 07/02/2014
 *      Author: ernesto
 */

#include <zlog.h>
#include <stdarg.h> /* for va_list */
#include <unistd.h>

#ifndef CACACOMUN_H_
#define CACACOMUN_H_

//XXX: http://gnupdf.org/manuals/gnupdf-hg-manual/html_node/Using-gdb-to-debug-check-tests.html

#if __STDC_VERSION__ < 199901L
#	if __GNUC__ >= 2
#		define __func__ __FUNCTION__
#	else
#		define __func__ "<unknown>"
#	endif
#endif

#define tipo_dato unsigned long

#define MAX_NODOS 100000
#define TAM_MAX_LINEA 1024
#define TAM_MAX_NUMERO 128
#define MAX_FILAS_INPUT 10000
#define MAX_COLUMNAS_INPUT 256
#define MAX_FILAS_NODOS 1000
#define MAX_COLUMNAS_NODOS 1000
#define COMPARACION_MENOR -1
#define COMPARACION_IGUAL 0
#define COMPARACION_MAYOR 1
#define MAX_TAM_CADENA 1000

#define MAX_VALOR (tipo_dato) (2<<28)

typedef enum BOOLEANOS {
	falso = 0, verdadero
} bool;

#define INIT_ZLOG_CATEGORY(cacategoria,nombre_cacategoria) \
	cacategoria = zlog_get_category(nombre_cacategoria); \
	if (!cacategoria) { \
		printf("get cacategoria %s fail\n",nombre_cacategoria); \
		zlog_fini(); \
		exit(-2); \
	} \


#define GRAFO_AVANZAR_NODO(nodo_apuntador,criterio_busqueda) \
		switch (criterio_busqueda) { \
	 	case GRAFO_VALOR: \
			nodo_actual = nodo_actual->siguiente_valor; \
			break; \
		case GRAFO_DISTANCIA: \
			nodo_actual = nodo_actual->siguiente_distancia; \
			break; \
		case GRAFO_INDICE: \
			nodo_actual = nodo_actual->siguiente_indice; \
			break; \
		default: \
			perror("en GRAFO_AVANZAR_NODO  hubo un error culero al buscar"); \
			break; \
		}

#define caca_log_debug(formato, args...) \
		do \
		{ \
			size_t profundidad = 0; \
			void *array[MAX_TAM_CADENA]; \
			profundidad = backtrace(array, MAX_TAM_CADENA); \
			caca_log_debug_func(formato,__FILE__, __func__, __LINE__,profundidad,##args); \
		} \
		while(0)

// XXX: http://www.programiz.com/c-programming/c-enumeration
typedef enum GRAFO_CRITERIOS_ORDENACION {
	GRAFO_INDICE, GRAFO_VALOR, GRAFO_DISTANCIA
} GRAFO_CRITERIOS_ORDENACION;
typedef enum GRAFO_TIPO_RESULTADO_BUSQUEDA {
	GRAFO_NADA_ENCONTRADO,
	GRAFO_NODO_ENCONTRADO,
	GRAFO_NODOS_PRE_POST_ENCONTRADOS
} GRAFO_TIPO_RESULTADO_BUSQUEDA;

typedef struct nodo {
	tipo_dato valor;
	tipo_dato indice;
	tipo_dato distancia;
	struct nodo *siguiente;
	struct nodo *anterior;
	struct nodo *siguiente_indice;
	struct nodo *anterior_indice;
	struct nodo *siguiente_valor;
	struct nodo *anterior_valor;
	struct nodo *siguiente_distancia;
	struct nodo *anterior_distancia;
} nodo;

typedef struct grifo_contexto {
	int localidades_usadas;
	nodo *inicio;
	nodo nodos_disponibles[MAX_NODOS];
	tipo_dato matrix_distancias[MAX_COLUMNAS_NODOS][MAX_FILAS_NODOS];
	tipo_dato referencias_nodos_por_indice[MAX_FILAS_NODOS];
} grafo_contexto;

zlog_category_t *cacategoria = NULL;

int lee_matriz_int_archivo(const char * nombre_archivo,
		tipo_dato matrix[MAX_COLUMNAS_INPUT][MAX_FILAS_INPUT], int *filas);

int lee_matriz_long_stdin(tipo_dato matrix[MAX_COLUMNAS_INPUT][MAX_FILAS_INPUT],
		int *filas);

// XXX: http://stackoverflow.com/questions/5724171/passing-an-array-by-reference
/**
 *
 * @param matrix
 * @param num_filas Regresa el num de filas .Debe ser inicializado externo.
 * @param num_columnas Regresa de cada fila el num de columnas. Debe ser inicializado externo. Debe ser al menos de tama�o ::MAX_FILAS_INPUT.
 * @return
 */
int lee_matrix_long_stdin(tipo_dato matrix[MAX_COLUMNAS_INPUT][MAX_FILAS_INPUT],
		int *num_filas, int *num_columnas);

int imprime_matrix(void *matrix, int num_filas, int *num_columnas,
		int num_columnas_fijo);

int init_grafo(void *matrix, int num_filas, grafo_contexto *ctx,
		bool usar_hashes);

nodo *grafo_alloc(grafo_contexto *ctx, int localidades_solicitadas);

// XXX: http://stackoverflow.com/questions/742699/returning-an-enum-from-a-function-in-c
GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_lineal(grafo_contexto *ctx,
		nodo *nodo_a_buscar, GRAFO_CRITERIOS_ORDENACION criterio_busqueda,
		void *nodos_encontrados);

GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_binaria(grafo_contexto *ctx,
		nodo *nodo_a_buscar, GRAFO_CRITERIOS_ORDENACION criterio_busqueda,
		void *nodos_encontrados);

GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_lineal_lista_nodos(nodo *inicio,
		nodo *nodo_a_buscar, GRAFO_CRITERIOS_ORDENACION criterio_busqueda,
		void *nodos_encontrados, int limite_nodos_busqueda);

void init_zlog(const char *arch_conf);

static inline char *nodo_a_cadena(nodo *node, char *cadena_buffer);

static inline int grafo_comparar_nodos(nodo *nodo1, nodo *nodo2,
		GRAFO_CRITERIOS_ORDENACION criterio_busqueda);

void caca_log_debug_func(const char *format, ...);

// XXX: https://www.securecoding.cert.org/confluence/display/seccode/MEM10-C.+Define+and+use+a+pointer+validation+function
int apuntador_valido(void *p);

// XXX: http://www.quora.com/Given-a-variable-how-can-you-find-whether-it-was-allocated-from-stack-or-from-heap-memory
bool from_stack(void *ptr);

#endif /* CACACOMUN_H_ */
