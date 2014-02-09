/* only main should be in this file, to make all other functions in
 the prograble testable by Check.  in order to test main(), use a
 whole program testing framework like Autotest.
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "cacacomun.h"
#include "unoslocos.h"

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
