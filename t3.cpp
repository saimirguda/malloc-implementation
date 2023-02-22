
#include <assert.h>
#include <stdio.h>
#include "util.cpp"
char a[] = "Hello there";
char *array[100];




int main()
{
  // Be sure to test your new-operator implementation ;)

  
  for(int i = 0; i < 100; i++)
  {
    array[i] = (char*)malloc(8);
  }

  size_t asa = snp::Memory::malloc_called_count();
  size_t as = snp::Memory::split_called_count();
  printf("%zu\n", as);
  printf("%zu\n", asa);

  return 0;
}
