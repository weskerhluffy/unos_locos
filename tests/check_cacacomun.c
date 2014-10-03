#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <util.h>
#include <check.h>
#include <zlog.h>
#include "../src/cacacomun.h"

static int *resultado_assestment = NULL;

START_TEST( test_init_grapho)
	{

		const tipo_dato VALORES[3][3] = { { 10, 20, 30 }, { 100, 200, 300 }, {
				1000, 2000, 3000 } };
		int filas = 3;
		int resultado = 0;
		grafo_contexto ctx;

		resultado = init_grafo((void*) VALORES, filas, &ctx, verdadero);

		zlog_fini();

		ck_assert_msg(!resultado, "todo en orden %s", resultado);
	}END_TEST

START_TEST( test_lee_matrix_long_stdin)
	{

		const char EOT[] = { 4, '\0' };
// XXX: http://cboard.cprogramming.com/c-programming/113489-initializing-2d-array-c.html
		const tipo_dato VALORES_ESPERADOS[3][3] = { { 10, 20, 30 },
				{ 100, 0, 0 }, { 1000, 2000, 0 } };

		int ptyfd = 0;
		int pid = 0;
		int caracteres_escritos = 0;
		int num_filas = 0;
		int num_filas_esperado = 0;
		int rc = 0;
		int i = 0, j = 0;
		int nums_escritos = 0;
		tipo_dato numero_actual = 0;

		int num_columnas[MAX_COLUMNAS_INPUT] = { 0 };
		int num_columnas_esperado[MAX_COLUMNAS_INPUT] = { 0 };
		tipo_dato resultados_reales[MAX_COLUMNAS_INPUT][MAX_FILAS_INPUT] = {
				{ 0 } };
		char *apuntador_linea = NULL;
		char cagada[MAX_FILAS_INPUT][TAM_MAX_LINEA] = { { '\0' } };

		zlog_category_t *c;
		zlog_category_t *c_fork;

		rc = zlog_init("/Users/ernesto/workspace/unos_locos/zlog.conf");
		if (rc) {
			printf("init failed\n");
			exit(-1);
		}

		c = zlog_get_category("my_cat");
		c_fork = zlog_get_category("test_fork");
		if (!c) {
			printf("get cat fail\n");
			zlog_fini();
			exit(-2);
		}

		zlog_info(c, "fucccckkkk!!!!");

// XXX: http://stackoverflow.com/questions/13274786/how-to-share-memory-between-process-fork
// XXX: https://code.google.com/p/asmjit/issues/detail?id=1
		resultado_assestment = mmap(NULL, sizeof *resultado_assestment,
				PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANON, -1, 0);

		for (i = 0; i < 3; i++) {
			apuntador_linea = *(cagada + i);
			nums_escritos = 0;
			for (j = 0; j < 3; j++) {
				numero_actual = *(*(VALORES_ESPERADOS + i) + j);
				if (numero_actual) {
					caracteres_escritos =
							sprintf(apuntador_linea, "%ld", numero_actual);
					zlog_debug(c, "en %d %d el num %ld", i, j, numero_actual);
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
			zlog_debug(c, "llllllinea %d: %s", i, *(cagada + i));
			*(num_columnas_esperado + i) = nums_escritos;
		}
		num_filas_esperado = i;

		zlog_debug(c, "El # de filas esperado %d", num_filas_esperado);
		for (i = 0; i < num_filas_esperado; i++) {
			zlog_debug(c, "en fila %d, columnas %d", i,
					*(num_columnas_esperado + i));
		}

		pid = forkpty(&ptyfd, 0, 0, 0);
		if (pid < 0) {
			perror("forkpty"), exit(1);
		}

		zlog_debug(c, "antes de aventar caca %d", pid);

		*resultado_assestment = -1;

		if (!pid) {
			zlog_debug(c_fork, "leyendo cagada");
			lee_matrix_long_stdin(resultados_reales, &num_filas, num_columnas);

			zlog_debug(c_fork, "antes d imprimir resultados reales");
			imprime_matrix(resultados_reales, num_filas, num_columnas);
			zlog_debug(c_fork, "despues d imprimir resultados reales");

			*resultado_assestment = !memcmp(resultados_reales,
					VALORES_ESPERADOS, 9)
					&& !memcmp(num_columnas_esperado, num_columnas, 3)
					&& num_filas == num_filas_esperado;
			zlog_debug(c_fork, "el resultado del assestment %d en child",
					*resultado_assestment);

		} else {
			system("echo 'carajo' > /tmp/shit.txt");
			zlog_info(c, "escribiendo pendejadas");
			for (int i = 0; i < 3; i++) {
				zlog_info(c, "la linea que avienta %s", *(cagada + i));
				write(ptyfd, *(cagada + i), strlen(*(cagada + i)));
			}
			write(ptyfd, EOT, 1);
			zlog_info(c, "escribio pendejadassss");
		}

		if (pid) {
			zlog_debug(c,
					"termino desmadre de mierda en padree %d, esperando %d",
					getpid(), pid);
//waitpid(pid, &i, WUNTRACED | WCONTINUED);
//			wait(&i);
			while (*resultado_assestment < 0) {
				zlog_debug(c, "esperando el resultado del ijo d 1000 putas");
				sleep(5);
			}
			zlog_debug(c, "ya regreso el ijo de 1000 putas %d", i);
			close(ptyfd);
			zlog_debug(c, "cerro el putoy");
			zlog_info(c, "hello, zlog %d ", pid);

			zlog_debug(c, "el resultado del assestment %d en %d",
					*resultado_assestment, pid);
			ck_assert_msg(*resultado_assestment > 0, "Las matrices no son =s");
			zlog_fini();
		} else {
			zlog_debug(c_fork, "termino desmadre de mierda en hijo");
			exit(3);
		}
	}END_TEST

Suite *
cacacomun_suite(void) {
	Suite *s = suite_create("Caca comun");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_set_timeout(tc_core, 600);
//	tcase_add_test(tc_core, test_lee_matrix);
//	tcase_add_test(tc_core, test_lee_matrix_stdin);
//	tcase_add_test(tc_core, test_lee_matrix_long_stdin);
	tcase_add_test(tc_core, test_init_grapho);
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

/*
 START_TEST( test_lee_matrix)
 {

 const char *nombre_archivo =
 "/Users/ernesto/workspace/unos_locos/mierda.txt";
 tipo_dato valor_esperado_0_0 = 1000000000000000;
 tipo_dato valor_esperado_1_2 = 9007199254740992;
 tipo_dato resultado_real_0_0 = 0;
 tipo_dato resultado_real_1_2 = 0;
 int filas_esperadas = 3;
 int resulta_filas = 0;
 tipo_dato matrix[MAX_COLUMNAS][MAX_FILAS];

 lee_matriz_int_archivo(nombre_archivo, matrix, &resulta_filas);
 resultado_real_0_0 = matrix[0][0];
 resultado_real_1_2 = matrix[1][2];

 ck_assert_msg(
 resultado_real_0_0 == valor_esperado_0_0
 && resultado_real_1_2 == valor_esperado_1_2
 && resulta_filas == filas_esperadas,
 "Expecting %ld and %ld, got %ld and %ld", valor_esperado_0_0,
 valor_esperado_1_2, resultado_real_0_0, resultado_real_1_2);
 }END_TEST

 START_TEST( test_lee_matrix_stdin)
 {

 tipo_dato valor_esperado_0_0 = 1000000000000000;
 tipo_dato valor_esperado_1_2 = 9007199254740992;
 tipo_dato resultado_real_0_0 = 0;
 tipo_dato resultado_real_1_2 = 0;
 int filas_esperadas = 3;
 int resulta_filas = 0;
 tipo_dato matrix[MAX_COLUMNAS][MAX_FILAS];
 int ptyfd = 0;
 int pid = 0;
 const char *cagada =
 "1000000000000000 10000000000000000\n2 12\n9007199254740992 9007199254740992\n";

 //XXX: http://stackoverflow.com/questions/5740176/how-do-i-write-a-testing-function-for-another-function-that-uses-stdin-input
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
 "Expecting %ld and %ld, got %ld and %ld",
 valor_esperado_0_0, valor_esperado_1_2, resultado_real_0_0,
 resultado_real_1_2);
 } else {
 write(ptyfd, cagada, strlen(cagada));
 }

 }END_TEST

 */
