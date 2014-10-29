#include<unistd.h>
#include <stdio.h>
#include <stdlib.h>

int Stack_or_heap(void* ptr)
 {
int dummy;
return ptr > &dummy;
}

int main(int argc, char** argv) 
{
int* i = (int*)malloc(sizeof(int));
int x, y, z;
printf("%d %d %d %d", Stack_or_heap(&x), Stack_or_heap(&y), Stack_or_heap(&z), Stack_or_heap(i));
}
