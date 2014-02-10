#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include <util.h>
#include "../src/cacacomun.h"

START_TEST( test_lee_matrix) {

	const char *nombre_archivo =
			"/Users/ernesto/workspace/unos_locos/mierda.txt";
	unsigned long valor_esperado_0_0 = 1000000000000000;
	unsigned long valor_esperado_1_2 = 9007199254740992;
	unsigned long resultado_real_0_0 = 0;
	unsigned long resultado_real_1_2 = 0;
	int filas_esperadas = 3;
	int resulta_filas = 0;
	unsigned long matrix[MAX_COLUMNAS][MAX_FILAS];

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

	unsigned long valor_esperado_0_0 = 1000000000000000;
	unsigned long valor_esperado_1_2 = 9007199254740992;
	unsigned long resultado_real_0_0 = 0;
	unsigned long resultado_real_1_2 = 0;
	int filas_esperadas = 3;
	int resulta_filas = 0;
	unsigned long matrix[MAX_COLUMNAS][MAX_FILAS];
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
				"Expecting %ld and %ld, got %ld and %ld", valor_esperado_0_0,
				valor_esperado_1_2, resultado_real_0_0, resultado_real_1_2);
	} else {
		write(ptyfd, cagada, strlen(cagada));
	}

}
END_TEST

Suite *
cacacomun_suite(void) {
	Suite *s = suite_create("Caca comun");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_set_timeout(tc_core, 600);
	tcase_add_test(tc_core, test_lee_matrix);
	tcase_add_test(tc_core, test_lee_matrix_stdin);
//	tcase_add_test(tc_core, test_bit);
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
