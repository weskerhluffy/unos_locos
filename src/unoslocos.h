/*
 * unos_locos.h
 *
 *  Created on: 12/01/2014
 *      Author: ernesto
 */

#ifndef UNOSLOCOS_H
#define UNOSLOCOS_H

#define bit(numero,posicion_bit) numero >> posicion_bit & 1

#define mascara_sig_cambio_bit(posicion_bit) ((1 << posicion_bit )-1)
#define mascara_ant_cambio_bit(posicion_bit) ~mascara_sig_cambio_bit(posicion_bit)

unsigned int num_bits_req(unsigned long numero);

unsigned long num_sig_cambio_bit(unsigned long numero, int pos_bit);

unsigned long num_ant_cambio_bit(unsigned long numero, int pos_bit);

#endif /* UNOSLOCOS_H */
