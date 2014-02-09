#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define TAM_MAX_LINEA 1024
#define TAM_MAX_NUMERO 128
#define MAX_FILAS 256
#define MAX_COLUMNAS 256

int lee_matriz_int_archivo(const char * nombre_archivo,
		unsigned long matrix[MAX_COLUMNAS][MAX_FILAS], int *filas);

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
		unsigned long conteo_unos[MAX_FILAS], int *filas_encontradas);

int lee_matriz_int_archivo(const char * nombre_archivo,
		unsigned long matrix[MAX_COLUMNAS][MAX_FILAS], int *filas) {
	int indice_filas = 0;
	int indice_columnas = 0;
	long numero = 0;
	char *inicio_cadena_num = NULL;
	char linea[TAM_MAX_LINEA];
	FILE *fp;

	fp = fopen(nombre_archivo, "r");
	if (fp == NULL ) {
		perror("Error while opening the file.\n");
		exit(EXIT_FAILURE);
	}
	while (fgets(linea, TAM_MAX_LINEA, fp) != NULL ) {
		/* writing content to stdout */
		inicio_cadena_num = strtok(linea, " ");
		indice_columnas = 0;
		while (inicio_cadena_num != NULL ) {
			numero = strtol(inicio_cadena_num, NULL, 10);
			matrix[indice_columnas][indice_filas] = (unsigned long) numero;
			inicio_cadena_num = strtok(NULL, " ");
			indice_columnas++;
		}
		indice_filas++;
	}
	*filas = indice_filas;
	fclose(fp);
	return 0;
}

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

	num_ant_cambio = numero & mask_ant_num;

	return num_ant_cambio;
}

unsigned long num_unos(unsigned long numero_1, unsigned long numero_2) {
	char bit_extremo_1 = 0;
	char bit_extremo_2 = 0;
	int num_bits = 0;
	int pos_bit = 0;
	int caso_extremos = -1;
	int relacion_ventana_longitud = -1;
	unsigned long residuo = 0;
	unsigned long tam_ventana = 0;
	unsigned long unos_bit_actual = 0;
	unsigned long unos_totales = 0;
	unsigned long periodo_binario = 0;
	unsigned long longitud_unos = 0;
	unsigned long num_crestas_unos = 0;
	unsigned long num_unos_en_crestas = 0;
	unsigned long num_unos_en_extremos = 0;
	unsigned long num_cambio_bit = 0;

	num_bits = num_bits_req(numero_2);
	tam_ventana = numero_2 - numero_1 + 1;

	for (pos_bit = (num_bits - 1); pos_bit >= 0; pos_bit--) {
		unos_bit_actual = 0;
		num_unos_en_crestas = 0;
		num_unos_en_extremos = 0;
		longitud_unos = (unsigned long) 1 << pos_bit;
		periodo_binario = longitud_unos << 1;

		bit_extremo_1 = bit(numero_1,pos_bit);
		bit_extremo_2 = bit(numero_2,pos_bit);

		if (!bit_extremo_1 && !bit_extremo_2) {
			caso_extremos = EXTREMOS_0_0;
		} else {
			if (!bit_extremo_1 && bit_extremo_2) {
				caso_extremos = EXTREMOS_0_1;
			} else {
				if (bit_extremo_1 && !bit_extremo_2) {
					caso_extremos = EXTREMOS_1_0;
				} else {
					if (bit_extremo_1 && bit_extremo_2) {
						caso_extremos = EXTREMOS_1_1;
					} else {
						printf("imposible");
						exit(1);
					}
				}
			}
		}

		if (tam_ventana < longitud_unos) {
			relacion_ventana_longitud = VENTANA_MENOR_LONGITUD_UNOS;
		} else {
			if (tam_ventana > longitud_unos) {
				relacion_ventana_longitud = VENTANA_MAYOR_LONGITUD_UNOS;
			} else {
				relacion_ventana_longitud = VENTANA_IGUAL_LONGITUD_UNOS;
			}
		}

		num_crestas_unos = tam_ventana / periodo_binario;
		residuo = tam_ventana % periodo_binario;
		num_unos_en_crestas =
				num_crestas_unos ? num_crestas_unos * longitud_unos : 0;
		switch (caso_extremos) {
		case EXTREMOS_0_0:
			if (relacion_ventana_longitud == VENTANA_IGUAL_LONGITUD_UNOS
					|| relacion_ventana_longitud == VENTANA_MENOR_LONGITUD_UNOS) {
				num_unos_en_crestas = 0;
			} else {
				if (residuo > longitud_unos) {
					num_unos_en_extremos += longitud_unos;
				}
			}
			break;
		case EXTREMOS_0_1:
		case EXTREMOS_1_0:
			if (residuo) {
				if (caso_extremos == EXTREMOS_0_1) {
					num_cambio_bit = num_ant_cambio_bit(numero_2, pos_bit);
					num_unos_en_extremos = numero_2 - num_cambio_bit + 1;
				} else {
					num_cambio_bit = num_sig_cambio_bit(numero_1, pos_bit);
					num_unos_en_extremos = num_cambio_bit - numero_1 + 1;
				}
			}

			break;
		case EXTREMOS_1_1:
			if (relacion_ventana_longitud == VENTANA_IGUAL_LONGITUD_UNOS
					|| relacion_ventana_longitud == VENTANA_MENOR_LONGITUD_UNOS) {
				num_unos_en_crestas = numero_2 - numero_1 + 1;
			} else {
				num_unos_en_extremos += residuo;
				if (residuo > longitud_unos) {
					num_unos_en_extremos -= longitud_unos;
				}
			}
			break;
		default:
			printf("WTF!?");
			exit(1);
		}

		unos_bit_actual = num_unos_en_crestas + num_unos_en_extremos;

		unos_totales += unos_bit_actual;
	}

	return unos_totales;
}

int contar_caca(const char * nombre_archivo,
		unsigned long conteo_unos[MAX_FILAS], int *filas_encontradas) {
	unsigned long matrix[MAX_COLUMNAS][MAX_FILAS];
	unsigned long num_unos_encontrados = 0;
	int filas_encontradas_tmp = 0;
	int indice_filas = 0;

	lee_matriz_int_archivo(nombre_archivo, matrix, &filas_encontradas_tmp);
	for (indice_filas = 0; indice_filas < filas_encontradas_tmp;
			indice_filas++) {
		num_unos_encontrados = num_unos(matrix[0][indice_filas],
				matrix[1][indice_filas]);
		conteo_unos[indice_filas] = num_unos_encontrados;
	}

	*filas_encontradas = filas_encontradas_tmp;
	return 0;
}

int main(int argc, char *argv[]) {
	char nombre_archivo[TAM_MAX_LINEA];
	unsigned long unos[MAX_FILAS];
	int filas_encontradas = 0;
	int i = 0;
	unsigned long conteo_unos = 0;

	strcpy(nombre_archivo, argv[1]);
	contar_caca(nombre_archivo, unos, &filas_encontradas);

	for (i = 0; i < filas_encontradas; i++) {
		conteo_unos = unos[i];
		printf("%ld\n", conteo_unos);
	}

	return 0;
}

