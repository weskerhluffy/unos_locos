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
#define CACA_COMPARACION_IZQ_MENOR -1
#define CACA_COMPARACION_IZQ_IGUAL 0
#define CACA_COMPARACION_IZQ_MAYOR 1
#define MAX_TAM_CADENA 1000

#define MAX_VALOR (tipo_dato) (2<<28)

#define GRAFO_NODO_ORIGEN 0
#define GRAFO_NODO_DESTINO 1
#define GRAFO_DISTANCIA_ENTRE_NODOS 2

#define GRAFO_LIMITE_ORDENACION_LINEAL 2

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


#define GRAFO_AVANZAR_NODO(nodo_apuntador,criterio_busqueda,discriminar_principal) \
		switch (criterio_busqueda) { \
	 	case GRAFO_VALOR: \
			nodo_apuntador = (nodo_apuntador)->siguiente_valor; \
			break; \
		case GRAFO_DISTANCIA: \
			nodo_apuntador = (nodo_apuntador)->siguiente_distancia; \
			break; \
		case GRAFO_INDICE: \
			nodo_apuntador = (nodo_apuntador)->siguiente_indice; \
			break; \
		case GRAFO_PRINCIPAL: \
			if(!discriminar_principal) { \
				nodo_apuntador = (nodo_apuntador)->siguiente; \
			} \
			break; \
		default: \
			perror("en GRAFO_AVANZAR_NODO  hubo un error culero al buscar"); \
			break; \
		}

#define GRAFO_COPIA_NODO_DATOS(nodo_origen,nodo_destino) \
		nodo_destino->valor=nodo_destino->valor; \
		nodo_destino->distancia=nodo_destino->distancia; \
		nodo_destino->indice=nodo_destino->indice;

#define GRAFO_NODO_SIG_ANT(nodo_anterior,nodo_siguiente,nodo_actual,criterio_busqueda) \
		switch (criterio_busqueda) { \
	 	case GRAFO_VALOR: \
			nodo_siguiente= (nodo_actual)->siguiente_valor; \
			nodo_anterior= (nodo_actual)->anterior_valor; \
			break; \
		case GRAFO_DISTANCIA: \
			nodo_siguiente= (nodo_actual)->siguiente_distancia; \
			nodo_anterior= (nodo_actual)->anterior_distancia; \
			break; \
		case GRAFO_INDICE: \
			nodo_siguiente= (nodo_actual)->siguiente_indice; \
			nodo_anterior= (nodo_actual)->anterior_indice; \
			break; \
		case GRAFO_PRINCIPAL: \
			nodo_siguiente= (nodo_actual)->siguiente; \
			nodo_anterior= (nodo_actual)->anterior; \
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

#define ARBOL_AVL_ACTUALIZAR_ALTURA(nodo) \
	(nodo)->altura = ((nodo)->hijo_izq || (nodo)->hijo_der)? \
			caca_int_max(ARBOL_AVL_GET_ALTURA((nodo)->hijo_izq), ARBOL_AVL_GET_ALTURA((nodo)->hijo_der)) + 1: \
			0;

#define ARBOL_AVL_GET_ALTURA(nodo) \
	((nodo)?(nodo)->altura:0)

#define ARBOL_AVL_GET_VALOR(nodo) \
	((nodo)?(nodo)->valor:-1)

#define ARBOL_BINARIO_ACTUALIZAR_PADRE(nodo,nuevo_padre) \
	((nodo)?(nodo)->padre=(nuevo_padre):((void)0))

#define ARBOL_AVL_GET_PADRE(nodo) \
	((nodo)?(nodo)->padre:NULL)

// XXX: http://www.programiz.com/c-programming/c-enumeration
#undef  ADDITEM
#define ADDITEM( criterio_ordenacion, comentario) criterio_ordenacion
typedef enum GRAFO_CRITERIOS_ORDENACION {
#include "tipos_ordenacion.h"
} GRAFO_CRITERIOS_ORDENACION;
#undef  ADDITEM

typedef enum GRAFO_TIPO_RESULTADO_BUSQUEDA {
	GRAFO_NADA_ENCONTRADO,
	GRAFO_NODO_ENCONTRADO,
	GRAFO_NODOS_PRE_POST_ENCONTRADOS
} GRAFO_TIPO_RESULTADO_BUSQUEDA;

typedef struct nodo {
	tipo_dato valor;
	tipo_dato indice;
	tipo_dato distancia;
	int num_nodos_asociados;
	int num_nodos_asociados_indice;
	int num_nodos_asociados_valor;
	int num_nodos_asociados_distancia;
	struct nodo *siguiente;
	struct nodo *anterior;
	struct nodo *siguiente_indice;
	struct nodo *anterior_indice;
	struct nodo *siguiente_valor;
	struct nodo *anterior_valor;
	struct nodo *siguiente_distancia;
	struct nodo *anterior_distancia;
} nodo;

typedef struct nodo_arbol_binario {
	tipo_dato indice;
	tipo_dato valor;
	unsigned int altura;


	struct nodo_arbol_binario *hijo_izq;
	struct nodo_arbol_binario *hijo_der;
	struct nodo_arbol_binario *padre;
} nodo_arbol_binario;

typedef struct grifo_contexto {
	int localidades_usadas;
	nodo *inicio;
	nodo nodos_disponibles[MAX_NODOS];
	tipo_dato matrix_distancias[MAX_COLUMNAS_NODOS][MAX_FILAS_NODOS];
	nodo *referencias_nodos_por_indice[MAX_FILAS_NODOS];
} grafo_contexto;

typedef struct arbol_binario_contexto {
	bool no_indices_repetidos;
	int localidades_usadas;
	nodo_arbol_binario *raiz;
	nodo_arbol_binario nodos_disponibles[MAX_NODOS];
} arbol_binario_contexto;

zlog_category_t *cacategoria = NULL;

#undef ADDITEM
#define ADDITEM( criterio_ordenacion, comentario) [criterio_ordenacion]=comentario
const char* GRAFO_NOMBRES_CRITERIOS_ORDENACION[GRAFO_PRINCIPAL + 1] = {
#include "tipos_ordenacion.h"
		};
#undef ADDITEM
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
		bool usar_hashes, bool relaciones_bidireccionales);

nodo *grafo_nodo_alloc(grafo_contexto *ctx, int localidades_solicitadas);

// XXX: http://stackoverflow.com/questions/742699/returning-an-enum-from-a-function-in-c
GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_lineal(nodo *inicio, nodo *nodo_a_buscar,
		GRAFO_CRITERIOS_ORDENACION criterio_busqueda, void *nodos_encontrados,
		int limite_nodos_busqueda);

GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_binaria(nodo *inicio,
		nodo *nodo_a_buscar, GRAFO_CRITERIOS_ORDENACION criterio_busqueda,
		void *nodos_encontrados);

GRAFO_TIPO_RESULTADO_BUSQUEDA busqueda_binaria_recursiva(nodo *inicio,
		nodo **arreglo_nodos, nodo *nodo_a_buscar,
		GRAFO_CRITERIOS_ORDENACION criterio_busqueda, void *nodos_encontrados,
		int indice_inicial, int indice_final);

void init_zlog(const char *arch_conf);

static inline char *grafo_nodo_a_cadena(nodo *node, char *cadena_buffer,
		int *characteres_escritos);

static inline int grafo_comparar_nodos(nodo *nodo1, nodo *nodo2,
		GRAFO_CRITERIOS_ORDENACION criterio_busqueda);

void caca_log_debug_func(const char *format, ...);

// XXX: https://www.securecoding.cert.org/confluence/display/seccode/MEM10-C.+Define+and+use+a+pointer+validation+function
int apuntador_valido(void *p);

// XXX: http://www.quora.com/Given-a-variable-how-can-you-find-whether-it-was-allocated-from-stack-or-from-heap-memory
bool from_stack(void *ptr);

void grafo_insertar_nodo(nodo *nodo_anterior, nodo *nodo_siguiente,
		nodo *nodo_a_insertar, GRAFO_CRITERIOS_ORDENACION criterio_busqueda);

void grafo_anadir_nodo(nodo *nodo_inicial, nodo *nodo_a_anadir,
		GRAFO_CRITERIOS_ORDENACION criterio_busqueda);

void imprimir_lista_adjacencia(nodo *nodo_inicial);

static inline int *grafo_apuntador_num_nodos_asociados(nodo *nodo,
		GRAFO_CRITERIOS_ORDENACION criterio_busqueda);

void arbol_avl_init(arbol_binario_contexto *ctx, tipo_dato *indices,
		tipo_dato *datos, int num_datos,
		nodo_arbol_binario **arreglo_referencias_nodos);

#define ARBOL_AVL_ALTURA_CARGADA_IZQ -1
#define ARBOL_AVL_ALTURA_CARGADA_DER 1
#define ARBOL_AVL_ALTURA_BALANCEADA 0

static inline int arbol_avl_diferencia_alturas_subarboles(
		nodo_arbol_binario *nodo, int tolerancia,
		bool considerar_balanceado_cargado_der);

static inline char *arbol_binario_nodo_a_cadena(nodo_arbol_binario *node,
		char *cadena_buffer, int *characteres_escritos);

static inline nodo_arbol_binario *arbol_binario_nodo_allocar(
		arbol_binario_contexto *ctx, int localidades_solicitadas);

void arbol_binario_recorrido_preoder(nodo_arbol_binario *raiz);

void arbol_binario_colectar_datos_recorrido_preoder(nodo_arbol_binario *raiz,
		tipo_dato *datos_ordenados, int *num_datos_colectados);

void arbol_avl_insertar(nodo_arbol_binario **raiz,
		nodo_arbol_binario *nodo_a_insertar, bool no_indices_repetidos);

static inline int arbol_avl_compara_nodos(nodo_arbol_binario *nodo1,
		nodo_arbol_binario *nodo2);

static inline int caca_int_max(int a, int b);

static inline void arbol_binario_rotar_izq(nodo_arbol_binario **nodo);
static inline void arbol_binario_rotar_der(nodo_arbol_binario **nodo);

void arbol_binario_borrar_nodo(nodo_arbol_binario **raiz,
		tipo_dato valor_a_borrar);

void arbol_binario_colectar_datos_recorrido_inoder(nodo_arbol_binario *raiz,
		tipo_dato *datos_ordenados, int *num_datos_colectados);

void arbol_binario_recorrido_inoder(nodo_arbol_binario *raiz);

void arbol_avl_borrar(nodo_arbol_binario **raiz, tipo_dato valor_a_borrar);

void arbol_avl_borrar_referencia_directa(nodo_arbol_binario *nodo_a_borrar);

void dijkstra_modificar_valor_nodo(nodo_arbol_binario **raiz,
		nodo_arbol_binario **referencias_directas, tipo_dato indice,
		tipo_dato nuevo_valor);

#endif /* CACACOMUN_H_ */
