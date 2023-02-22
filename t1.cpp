
#include <assert.h>
#include <stdio.h>
#include <cstdlib>
#include "util.cpp"
char a[] = "Hello there";
char *array[100];
int main()
{
  // Be sure to test your new-operator implementation ;)



  char *e = (char*)malloc( 1024 );

    size_t m_count = snp::Memory::malloc_called_count();
  
  size_t alloc_count = snp::Memory::used_blocks_count();
  printf("%zu\n", alloc_count);
  printf("%zu\n", m_count);



  char *p = (char*)malloc( 1024 );

  char *q = (char*)malloc( 1024 );

  char *s = (char*)malloc( 1024 );

  free(p);

  free(s);


  char *v = (char*)malloc( 1024 );

  free(e);
  free(v);
  free(q);







  return 0;
}
