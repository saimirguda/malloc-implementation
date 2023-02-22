#include <new> 
#include <stdlib.h> 
#include "memory.h"

namespace snp
{




// TODO Student
void *Memory::_new(size_t size) 
{ 
  void *ptr = malloc(size);
  if(ptr)
    return ptr;
  else
    throw(std::bad_alloc){};
} 

// TODO Student
void Memory::_delete(void * p) 
{ 
  free(p);
} 





}
