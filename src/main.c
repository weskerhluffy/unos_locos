/* only main should be in this file, to make all other functions in
 the prograble testable by Check.  in order to test main(), use a
 whole program testing framework like Autotest.
 */

#include <stdio.h>
#include "unoslocos.h"
#include "cacacomun.h"

//XXX: http://stackoverflow.com/questions/16345261/how-do-you-add-breakpoint-actions-via-the-lldb-command-line
int main(void) {
	const char *nombre_archivo =
			"/Users/ernesto/workspace/unos_locos/mierda.txt";
	unsigned long unos[MAX_FILAS];
	int filas_encontradas = 0;
	int i = 0;
	unsigned long conteo_unos = 0;

	contar_caca(nombre_archivo, unos, &filas_encontradas);

	for (i = 0; i < filas_encontradas; i++) {
		conteo_unos = unos[i];
		printf("%ld\n", conteo_unos);
	}

	return 0;
}
