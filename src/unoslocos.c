/*
 ============================================================================
 Name        : unos_locos.c
 Author      : ernesto
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdlib.h>
#include <stdio.h>
#include "unoslocos.h"
unsigned int num_bits_req(unsigned long numero) {
	int num_bits_long = sizeof(unsigned long) * 8;
	int i = 0;
	for (i = num_bits_long - 1; i >= 0; i--) {
		if (bit(numero,i)) {
			return i + 1;
		}
	}
	return 1;
}

unsigned long num_sig_cambio_bit(unsigned long numero, int pos_bit) {
	unsigned long num_sig_cambio = 0;
	unsigned long mask_sig_num = 0;

	mask_sig_num = mascara_sig_cambio_bit(pos_bit);

	num_sig_cambio = numero | mask_sig_num;

	return num_sig_cambio;
}

unsigned long num_ant_cambio_bit(unsigned long numero, int pos_bit) {
	unsigned long num_ant_cambio = 0;
	unsigned long mask_ant_num = 0;

	mask_ant_num = mascara_ant_cambio_bit(pos_bit);

	num_ant_cambio=numero & mask_ant_num;

	return num_ant_cambio;
}
