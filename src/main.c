/* only main should be in this file, to make all other functions in
 the prograble testable by Check.  in order to test main(), use a
 whole program testing framework like Autotest.
 */

#include "unoslocos.h"
#include <stdio.h>

//XXX: http://stackoverflow.com/questions/16345261/how-do-you-add-breakpoint-actions-via-the-lldb-command-line
int main(void) {
	unsigned long numero_1 = 1000000000000000;
	unsigned long numero_2 = 10000000000000000;
	unsigned long resultado_esperado = 239502115812196372;
	unsigned long resultado_real = 0;

	resultado_real = num_unos(numero_1, numero_2);
	printf("You could be mine \n");
	return 0;
}
