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
