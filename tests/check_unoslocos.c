#include <stdlib.h>
#include <stdio.h>
#include <check.h>
#include "../src/unoslocos.h"

START_TEST( test_bit) {
	ck_assert(bit(4,2));
}
END_TEST

START_TEST( test_num_bits_req) {
	ck_assert_msg(num_bits_req(255) == 8, "Expecting 8, got %d",
			num_bits_req(255));
}
END_TEST

START_TEST( test_num_bits_req_1) {
	ck_assert_msg(num_bits_req(256) == 9, "Expecting 9, got %d",
			num_bits_req(256));
}
END_TEST

START_TEST( test_mask_sig_num) {
	ck_assert_msg(mascara_sig_cambio_bit(4) == 15, "Expecting 15, got %d",
			mascara_sig_cambio_bit(4));
}
END_TEST

START_TEST( test_mask_ant_num) {
	ck_assert_msg(mascara_ant_cambio_bit(4) == ~15, "Expecting %d, got %d", ~15,
			mascara_ant_cambio_bit(4));
}
END_TEST

START_TEST( test_ant_num) {
	int pos_bit = 2;
	unsigned long numero = 14;
	unsigned long resultado_esperado = 12;
	unsigned long resultado_real = num_ant_cambio_bit(numero, pos_bit);
	ck_assert_msg(resultado_real == resultado_esperado, "Expecting %d, got %d",
			resultado_esperado, resultado_real);
}
END_TEST

START_TEST( test_sig_num) {
	int pos_bit = 2;
	unsigned long numero = 5;
	unsigned long resultado_esperado = 7;
	unsigned long resultado_real = num_sig_cambio_bit(numero, pos_bit);
	ck_assert_msg(resultado_real == resultado_esperado, "Expecting %d, got %d",
			resultado_esperado, resultado_real);
}
END_TEST

START_TEST( test_num_unos) {
	ck_assert_msg(num_unos(2, 12) == 21, "Expecting %d, got %d", 21,
			num_unos(2, 12));
}
END_TEST

START_TEST( test_num_unos_1) {
	unsigned long numero_1 = 1000000000000000;
	unsigned long numero_2 = 10000000000000000;
	unsigned long resultado_esperado = 239502115812196372;
	unsigned long resultado_real = 0;

	resultado_real = num_unos(numero_1, numero_2);
	ck_assert_msg(resultado_real == resultado_esperado,
			"Expecting %ld, got %ld", resultado_esperado, resultado_real);
}
END_TEST

START_TEST( test_num_unos_2) {
	unsigned long numero_1 = 9007199254740992;
	unsigned long numero_2 = 9007199254740992;
	unsigned long resultado_esperado = 1;
	unsigned long resultado_real = 0;

	resultado_real = num_unos(numero_1, numero_2);
	ck_assert_msg(resultado_real == resultado_esperado,
			"Expecting %ld, got %ld", resultado_esperado, resultado_real);
}
END_TEST

START_TEST( cuenta_caca) {

	const char *nombre_archivo =
			"/Users/ernesto/workspace/unos_locos/mierda.txt";
	unsigned long valor_esperado_0 = 239502115812196372;
	unsigned long valor_esperado_2 = 1;
	unsigned long resultado_real_0 = 0;
	unsigned long resultado_real_2 = 0;
	unsigned long unos[MAX_FILAS];
	int filas_encontradas = 0;
	int filas_esperadas = 3;

	contar_caca(nombre_archivo, unos, &filas_encontradas);
	resultado_real_0 = unos[0];
	resultado_real_2 = unos[2];

	ck_assert_msg(
			resultado_real_0 == valor_esperado_0
					&& resultado_real_2 == valor_esperado_2
					&& filas_encontradas == filas_esperadas,
			"Expecting %ld and %ld, got %ld and %ld", valor_esperado_0,
			valor_esperado_2, resultado_real_0, resultado_real_2);
}
END_TEST

Suite *
unoslocos_suite(void) {
	Suite *s = suite_create("Unos locos");

	/* Core test case */
	TCase *tc_core = tcase_create("Core");
	tcase_add_test(tc_core, test_bit);
	tcase_add_test(tc_core, test_num_bits_req);
	tcase_add_test(tc_core, test_num_bits_req_1);
	tcase_add_test(tc_core, test_mask_sig_num);
	tcase_add_test(tc_core, test_mask_ant_num);
	tcase_add_test(tc_core, test_sig_num);
	tcase_add_test(tc_core, test_ant_num);
	tcase_add_test(tc_core, test_num_unos);
	tcase_add_test(tc_core, test_num_unos_1);
	tcase_add_test(tc_core, test_num_unos_2);
	tcase_add_test(tc_core, cuenta_caca);
	suite_add_tcase(s, tc_core);

	return s;
}

int main(void) {
	int number_failed;
	Suite *s = unoslocos_suite();
	SRunner *sr = srunner_create(s);
	srunner_run_all(sr, CK_VERBOSE);
	number_failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
