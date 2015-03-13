#include <config.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#ifdef HAVE_UTIL_H
#	include <util.h>
#endif
#ifdef HAVE_EXECINFO_H
#	include <execinfo.h>
#endif
#include <check.h>
#include <zlog.h>
#include "../src/cacacomun.h"

static int *resultado_assestment = NULL;

START_TEST( test_lee_matrix) {

	const char *nombre_archivo =
			"/Users/ernesto/workspace/unos_locos/mierda.txt";
	tipo_dato valor_esperado_0_0 = 1000000000000000;
	tipo_dato valor_esperado_1_2 = 9007199254740992;
	tipo_dato resultado_real_0_0 = 0;
	tipo_dato resultado_real_1_2 = 0;
	int filas_esperadas = 3;
	int resulta_filas = 0;
	tipo_dato matrix[MAX_COLUMNAS_INPUT][MAX_FILAS_INPUT];

	lee_matriz_int_archivo(nombre_archivo, matrix, &resulta_filas);
	resultado_real_0_0 = matrix[0][0];
	resultado_real_1_2 = matrix[1][2];

	ck_assert_msg(
			resultado_real_0_0 == valor_esperado_0_0
					&& resultado_real_1_2 == valor_esperado_1_2
					&& resulta_filas == filas_esperadas,
			"Expecting %ld and %ld, got %ld and %ld", valor_esperado_0_0,
			valor_esperado_1_2, resultado_real_0_0, resultado_real_1_2);
}
END_TEST

START_TEST( test_lee_matrix_stdin) {

	tipo_dato valor_esperado_0_0 = 1000000000000000;
	tipo_dato valor_esperado_1_2 = 9007199254740992;
	tipo_dato resultado_real_0_0 = 0;
	tipo_dato resultado_real_1_2 = 0;
	int filas_esperadas = 3;
	int resulta_filas = 0;
	tipo_dato matrix[MAX_COLUMNAS_INPUT][MAX_FILAS_INPUT];
	int ptyfd = 0;
	int pid = 0;
	const char *cagada =
			"1000000000000000 10000000000000000\n2 12\n9007199254740992 9007199254740992\n";

	/* XXX: http://stackoverflow.com/questions/5740176/how-do-i-write-a-testing-function-for-another-function-that-uses-stdin-input*/
	pid = forkpty(&ptyfd, 0, 0, 0);
	if (pid < 0)
		perror("forkpty"), exit(1);
	if (!pid) {
		lee_matriz_long_stdin(matrix, &resulta_filas);
		resultado_real_0_0 = matrix[0][0];
		resultado_real_1_2 = matrix[1][2];

		ck_assert_msg(
				resultado_real_0_0 == valor_esperado_0_0
						&& resultado_real_1_2 == valor_esperado_1_2
						&& resulta_filas == filas_esperadas,
				"Expecting %ld and %ld, got %ld and %ld", valor_esperado_0_0,
				valor_esperado_1_2, resultado_real_0_0, resultado_real_1_2);
	} else {
		write(ptyfd, cagada, strlen(cagada));
	}

}
END_TEST

START_TEST( test_lee_matrix_long_stdin) {

	const char EOT[] = { 4, '\0' };
	/*  XXX: http://cboard.cprogramming.com/c-programming/113489-initializing-2d-array-c.html */
	const tipo_dato VALORES_ESPERADOS[3][3] = { { 10, 20, 30 }, { 100, 0, 0 }, {
			1000, 2000, 0 } };

	int ptyfd = 0;
	int pid = 0;
	int caracteres_escritos = 0;
	int num_filas = 0;
	int num_filas_esperado = 0;
	int i = 0, j = 0;
	int nums_escritos = 0;
	tipo_dato numero_actual = 0;

	int num_columnas[MAX_COLUMNAS_INPUT] = { 0 };
	int num_columnas_esperado[MAX_COLUMNAS_INPUT] = { 0 };
	tipo_dato resultados_reales[MAX_COLUMNAS_INPUT][MAX_FILAS_INPUT] = { { 0 } };
	char *apuntador_linea = NULL;
	char cagada[MAX_FILAS_INPUT][TAM_MAX_LINEA] = { { '\0' } };

	/*  XXX: http://stackoverflow.com/questions/13274786/how-to-share-memory-between-process-fork */
	/*  XXX: https://code.google.com/p/asmjit/issues/detail?id=1 */
	resultado_assestment = mmap(NULL, sizeof *resultado_assestment,
			PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

	for (i = 0; i < 3; i++) {
		apuntador_linea = *(cagada + i);
		nums_escritos = 0;
		for (j = 0; j < 3; j++) {
			numero_actual = *(*(VALORES_ESPERADOS + i) + j);
			if (numero_actual) {
				caracteres_escritos = sprintf(apuntador_linea, "%ld",
						numero_actual);
				if (caracteres_escritos < 0) {
					fprintf(stderr,
							"El numero no se pudo convertir, error fatal \n");
					exit(EXIT_FAILURE); /* indicate failure.*/
				}
				apuntador_linea += caracteres_escritos;
				*apuntador_linea++ = ' ';
				nums_escritos++;
			}
		}
		*apuntador_linea++ = '\n';
		*(num_columnas_esperado + i) = nums_escritos;
	}
	num_filas_esperado = i;

	pid = forkpty(&ptyfd, 0, 0, 0);
	if (pid < 0) {
		perror("forkpty"), exit(1);
	}

	*resultado_assestment = -1;

	if (!pid) {
		lee_matrix_long_stdin((tipo_dato *) resultados_reales, &num_filas,
				num_columnas, MAX_FILAS_INPUT, MAX_COLUMNAS_INPUT);

		caca_imprime_matrix(resultados_reales, num_filas, num_columnas, 0);

		*resultado_assestment = !memcmp(resultados_reales, VALORES_ESPERADOS, 9)
				&& !memcmp(num_columnas_esperado, num_columnas, 3)
				&& num_filas == num_filas_esperado;

	} else {
		for (int i = 0; i < 3; i++) {
			write(ptyfd, *(cagada + i), strlen(*(cagada + i)));
		}
		write(ptyfd, EOT, 1);
	}

	if (pid) {
		while (*resultado_assestment < 0) {
			sleep(5);
		}
		close(ptyfd);
		ck_assert_msg(*resultado_assestment > 0, "Las matrices no son =s");
		zlog_fini();
	}

	/*else {
	 exit(3);
	 }
	 */
}

END_TEST

START_TEST( test_apuntador_allocado) {

	tipo_dato **valores = NULL;

	valores = (tipo_dato **) malloc(3 * sizeof(tipo_dato *));

	for (int i = 0; i < 3; i++) {
		*(valores + i) = (tipo_dato *) malloc(10 * sizeof(tipo_dato));
	}
	caca_log_debug("o o");

	zlog_fini();

	ck_assert_msg(from_stack(*valores), "El valor %p no es apuntador valido",
			*valores);
}
END_TEST

START_TEST( test_imprime_apuntador) {

	tipo_dato **valores = NULL;

	valores = (tipo_dato **) malloc(3 * sizeof(tipo_dato *));

	for (int i = 0; i < 3; i++) {
		*(valores + i) = (tipo_dato *) malloc(10 * sizeof(tipo_dato));
		for (int j = 0; j < 10; j++) {
			*(*(valores + i) + j) = i * j;
		}
	}

	zlog_fini();

	ck_assert_msg(!caca_imprime_matrix(valores, 3, NULL, 10),
			"El valor %p no se pudo imprimir como arreglo", valores);
}
END_TEST

START_TEST( test_imprime_array) {

	const tipo_dato VALORES[3][3] = { { 10, 20, 30 }, { 100, 200, 300 }, { 1000,
			2000, 3000 } };
	void *ptr = NULL;

	ptr = (void *) VALORES;

	zlog_fini();

	ck_assert_msg(!caca_imprime_matrix(ptr, 3, NULL, 3),
			"El valor %p no es apuntador valido", ptr);
}
END_TEST

START_TEST( test_init_arbol_avl) {

	const tipo_dato VALORES[6] = { 10, 20, 30, 40, 50, 25 };
	const tipo_dato VALORES_PREORDER[6] = { 30, 20, 10, 25, 40, 50 };

	int resultado = 0;
	tipo_dato valores_preorder_resultado[6] = { 0 };
	arbol_binario_contexto ctx;

	arbol_avl_init(&ctx, NULL, (unsigned long *) VALORES, 6, NULL );

	arbol_binario_colectar_datos_recorrido_preoder(ctx.raiz,
			valores_preorder_resultado, &resultado);

	resultado = memcmp(VALORES_PREORDER, valores_preorder_resultado, 6);

	caca_log_debug("La secuencia es:");

	arbol_binario_recorrido_preoder(ctx.raiz);

	zlog_fini();

	ck_assert_msg(!resultado, "todo en orden %d", resultado);
}
END_TEST

START_TEST( test_borrar_arbol_binario) {

	const tipo_dato VALORES[7] = { 20, 30, 40, 50, 60, 70, 80 };
	const tipo_dato VALORES_FINALES_INORDER[4] = { 40, 60, 70, 80 };

	int resultado = 0;
	tipo_dato valores_preorder_resultado[4] = { 0 };
	arbol_binario_contexto ctx;

	arbol_avl_init(&ctx, NULL, (unsigned long *) VALORES, 7, NULL );

	arbol_binario_borrar_nodo(&ctx.raiz, 20);
	arbol_binario_borrar_nodo(&ctx.raiz, 30);
	arbol_binario_borrar_nodo(&ctx.raiz, 50);

	arbol_binario_colectar_datos_recorrido_inoder(ctx.raiz,
			valores_preorder_resultado, &resultado);

	resultado = memcmp(VALORES_FINALES_INORDER, valores_preorder_resultado, 4);

	caca_log_debug("La secuencia es:");

	arbol_binario_recorrido_inoder(ctx.raiz);

	zlog_fini();

	ck_assert_msg(!resultado, "todo en orden %d", resultado);
}
END_TEST

START_TEST( test_borrar_arbol_avl) {

	const tipo_dato VALORES[9] = { 9, 5, 10, 1, 6, 11, 0, 2, 3 };
	const tipo_dato VALORES_FINALES_PREORDER[8] = { 2, 1, 0, 9, 5, 3, 6, 11 };

	int resultado = 0;
	tipo_dato valores_preorder_resultado[8] = { 0 };
	arbol_binario_contexto ctx;

	arbol_avl_init(&ctx, NULL, (unsigned long *) VALORES, 9, NULL );

	caca_log_debug("La secuencia inicial es:");
	arbol_binario_recorrido_preoder(ctx.raiz);

	arbol_avl_borrar(&ctx.raiz, 10);

	arbol_binario_colectar_datos_recorrido_preoder(ctx.raiz,
			valores_preorder_resultado, &resultado);

	resultado = memcmp(VALORES_FINALES_PREORDER, valores_preorder_resultado, 8);

	caca_log_debug("La secuencia final es:");
	arbol_binario_recorrido_preoder(ctx.raiz);

	zlog_fini();

	ck_assert_msg(!resultado, "todo en orden %d", resultado);
}
END_TEST

START_TEST( test_borrar_arbol_avl_ref_dir) {

	const tipo_dato VALORES[9] = { 9, 5, 10, 1, 6, 11, 0, 2, 3 };
	const tipo_dato VALORES_FINALES_INORDER[8] = { 1, 2, 3, 5, 6, 9, 10, 11 };

	int resultado = 0;
	arbol_binario_contexto ctx;
	tipo_dato valores_preorder_resultado[8] = { 0 };
	nodo_arbol_binario *referencias_directas[10] = { NULL };

	arbol_avl_init(&ctx, NULL, (unsigned long *) VALORES, 9,
			referencias_directas);

	caca_log_debug("La secuencia inicial es:");
	arbol_binario_recorrido_inoder(ctx.raiz);

	caca_log_debug("borrando nodo %ld",
			ARBOL_AVL_GET_VALOR(*(referencias_directas + 7)));

	arbol_avl_borrar_referencia_directa(&ctx.raiz, *(referencias_directas + 7),
			NULL );

	arbol_binario_colectar_datos_recorrido_inoder(ctx.raiz,
			valores_preorder_resultado, &resultado);

	for (int i = 0; i < 8; i++) {
		caca_log_debug("eien pressure %ld", *(valores_preorder_resultado + i));
	}

	resultado = memcmp(VALORES_FINALES_INORDER, valores_preorder_resultado, 8);

	caca_log_debug("La secuencia final es:");
	arbol_binario_recorrido_inoder(ctx.raiz);

	zlog_fini();

	ck_assert_msg(!resultado, "todo en orden %d", resultado);
}
END_TEST

START_TEST( test_dijkstra_modificar_valor_nodo) {

	const tipo_dato VALORES[9] = { 9, 5, 10, 1, 6, 11, 0, 2, 3 };
	const tipo_dato VALORES_FINALES_INORDER[10] = { 0, 1, 2, 3, 4, 5, 6, 9, 10 };

	int resultado = 0;
	int num_datos_colectados = 0;
	arbol_binario_contexto ctx;
	cola_prioridad_contexto cpctx;
	tipo_dato valores_preorder_resultado[10] = { 0 };
	nodo_arbol_binario *referencias_directas[11] = { NULL };

	arbol_avl_init(&ctx, NULL, (unsigned long *) VALORES, 9,
			referencias_directas);

	for (int i = 0; i < 11; i++) {
		caca_log_debug("referencias directas indice %d  valor %ld", i,
				ARBOL_AVL_GET_VALOR(*(referencias_directas + i)));
	}

	caca_log_debug("La secuencia inicial es:");
	arbol_binario_recorrido_inoder(ctx.raiz);

	cola_prioridad_init(&cpctx, NULL, NULL, NULL, 0, &ctx,
			referencias_directas);

	cola_prioridad_modificar_valor_nodo(&cpctx, 6, 4);

	arbol_binario_colectar_datos_recorrido_inoder(ctx.raiz,
			valores_preorder_resultado, &num_datos_colectados);

	for (int i = 0; i < 11; i++) {
		caca_log_debug("eien pressure %d %ld", i,
				*(valores_preorder_resultado + i));
		caca_log_debug("referencias directas indice %ld, valor %ld",
				ARBOL_AVL_GET_INDICE(*(referencias_directas + i)),
				ARBOL_AVL_GET_VALOR(*(referencias_directas + i)));
	}

	resultado = num_datos_colectados == 9
			&& !memcmp(VALORES_FINALES_INORDER, valores_preorder_resultado, 9)
			&& (*(referencias_directas + 6))->valor == 4;

	caca_log_debug("La secuencia final es:");
	arbol_binario_recorrido_inoder(ctx.raiz);

	zlog_fini();

	ck_assert_msg(resultado,
			"los datos colectados %d, el valor en el indice 5 ",
			num_datos_colectados);
}
END_TEST

#define NUM_VALORES  9
START_TEST( test_cola_prioridad_pop) {

	const tipo_dato VALORES[NUM_VALORES] = { 9, 5, 10, 1, 6, 11, 0, 2, 3 };
	const tipo_dato VALORES_FINALES_INORDER[NUM_VALORES - 2] = { 9, 5, 10, 6,
			11, 2, 3 };

	int resultado = 0;
	int num_datos_colectados = 0;
	cola_prioridad_contexto ctx;
	tipo_dato valores_inorder_resultado[NUM_VALORES] = { 0 };
	nodo_cola_prioridad **referencias_directas = NULL;
	nodo_cola_prioridad *nodo_pop_1 = NULL;
	nodo_cola_prioridad *nodo_pop_2 = NULL;

//	sleep(5);

	caca_log_debug("empezando el año");

	cola_prioridad_init(&ctx, NULL, (tipo_dato *) VALORES, NULL, NUM_VALORES,
			NULL, NULL );

	referencias_directas = ctx.referencias_directas_por_indice;

	for (int i = 0; i < NUM_VALORES + 1; i++) {
		caca_log_debug("referencias directas indice %d  valor %ld", i,
				ARBOL_AVL_GET_VALOR(*(referencias_directas + i)));
	}

	nodo_pop_1 = cola_prioridad_pop(&ctx);
	nodo_pop_2 = cola_prioridad_pop(&ctx);

	cola_prioridad_get_valores(&ctx, valores_inorder_resultado,
			&num_datos_colectados);

	for (int i = 0; i < NUM_VALORES; i++) {
		caca_log_debug("eien pressure %d %ld", i,
				*(valores_inorder_resultado + i));
	}
	caca_log_debug("los datos colectados %d", num_datos_colectados);
	caca_log_debug("el popeado 1 %ld", nodo_pop_1->valor);
	caca_log_debug("el popeado 2 %ld", nodo_pop_2->valor);

	resultado = num_datos_colectados == (NUM_VALORES - 2)
			&& !memcmp(VALORES_FINALES_INORDER, valores_inorder_resultado,
					NUM_VALORES - 2) && nodo_pop_1->valor == 0
			&& nodo_pop_2->valor == 1;

	zlog_fini();

	ck_assert_msg(resultado, "los datos colectados %d", num_datos_colectados);
}
END_TEST

#define NUM_VERTICES  14
START_TEST( test_dijkstra) {

	const tipo_dato VERTICES[NUM_VERTICES][3] = { { 0, 1, 4 }, { 0, 7, 8 }, { 1,
			7, 11 }, { 1, 2, 8 }, { 7, 8, 7 }, { 7, 6, 1 }, { 2, 8, 2 }, { 8, 6,
			6 }, { 2, 3, 7 }, { 6, 5, 2 }, { 2, 5, 4 }, { 3, 4, 9 },
			{ 5, 4, 10 }, { 3, 5, 14 }, };
	const tipo_dato DISTANCIAS_FINALES[NUM_VERTICES] = { 0, 4, 12, 19, 21, 11,
			9, 8, 14 };
	const tipo_dato ANTECESORES[NUM_VERTICES] = { 0, 0, 1, 2, 5, 6, 7, 0, 2 };

	int resultado = 0;

	char buffer[MAX_TAM_CADENA] = { '\0' };

	tipo_dato distancias_minimas_calculadas[NUM_VERTICES] = { 0 };
	tipo_dato antecesores[NUM_VERTICES] = { 0 };

	caca_log_debug("yo no olvido al año viejo");

	dijkstra_main((void *) VERTICES, NUM_VERTICES, 0, 0, NULL,
			distancias_minimas_calculadas, antecesores);

	resultado = !memcmp(DISTANCIAS_FINALES, distancias_minimas_calculadas, 9)
			&& !memcmp(ANTECESORES, antecesores, 99);

	caca_log_debug("los antecesores kedaron: %s",
			caca_arreglo_a_cadena(antecesores, NUM_VERTICES, buffer));

	for (int i = 1; i < NUM_VERTICES; i++) {
		caca_log_debug("antecesor de %ld es %ld", i, *(antecesores + i - 1));
	}

	zlog_fini();

	ck_assert_msg(resultado, "las distancias minimas %s",
			caca_arreglo_a_cadena(distancias_minimas_calculadas, NUM_VERTICES,
					buffer));
}
END_TEST

Suite *
cacacomun_suite(void) {

	Suite *s = suite_create("Caca comun");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_set_timeout(tc_core, 600);
	tcase_add_test(tc_core, test_lee_matrix_long_stdin);
	tcase_add_test(tc_core, test_lee_matrix);
	tcase_add_test(tc_core, test_lee_matrix_stdin);
	tcase_add_test(tc_core, test_apuntador_allocado);
	tcase_add_test(tc_core, test_imprime_apuntador);
	tcase_add_test(tc_core, test_imprime_array);
	tcase_add_test(tc_core, test_init_arbol_avl);
	tcase_add_test(tc_core, test_borrar_arbol_binario);
	tcase_add_test(tc_core, test_borrar_arbol_avl);
	tcase_add_test(tc_core, test_borrar_arbol_avl_ref_dir);
	tcase_add_test(tc_core, test_dijkstra_modificar_valor_nodo);
	tcase_add_test(tc_core, test_cola_prioridad_pop);
	/*
	 */
	tcase_add_test(tc_core, test_dijkstra);
	/*
	 */
	/*
	 */
	suite_add_tcase(s, tc_core);

	return s;
}

int main(void) {
	int number_failed;
	Suite *s = cacacomun_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
