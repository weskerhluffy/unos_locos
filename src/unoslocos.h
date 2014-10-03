/*
 * unos_locos.h
 *
 *  Created on: 12/01/2014
 *      Author: ernesto
 */

#include "cacacomun.h"

#ifndef UNOSLOCOS_H
#define UNOSLOCOS_H

#define VENTANA_IGUAL_LONGITUD_UNOS 0
#define VENTANA_MENOR_LONGITUD_UNOS 1
#define VENTANA_MAYOR_LONGITUD_UNOS 2

#define EXTREMOS_0_0 0
#define EXTREMOS_0_1 1
#define EXTREMOS_1_0 2
#define EXTREMOS_1_1 3

#define bit(numero,posicion_bit) numero >> posicion_bit & 1

#define mascara_sig_cambio_bit(posicion_bit) (((unsigned long)1 << posicion_bit )-1)
#define mascara_ant_cambio_bit(posicion_bit) ~mascara_sig_cambio_bit(posicion_bit)

unsigned int num_bits_req(unsigned long numero);

unsigned long num_sig_cambio_bit(unsigned long numero, int pos_bit);

unsigned long num_ant_cambio_bit(unsigned long numero, int pos_bit);

unsigned long num_unos(unsigned long numero_1, unsigned long numero_2);

int contar_caca(const char * nombre_archivo,
		unsigned long conteo_unos[MAX_FILAS_INPUT],int *filas_encontradas);

#endif /* UNOSLOCOS_H */
