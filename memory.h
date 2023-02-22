#ifndef SNP_MEMORY_H_
#define SNP_MEMORY_H_

#include "unistd.h"
#include "stdlib.h"
namespace snp {

    typedef struct block {
    size_t		size;
    struct block   *next;
    struct block   *prev;
    size_t magic_number;
    bool used;
  }	block_t;

 
  #define MIN_ALLOC 2*4096

  class Memory
  {
  public:
    // DO NOT MODIFY
    static void *calloc(size_t num_memb, size_t size_each);
    static void *malloc(size_t size);
    static void free(void *ptr);

    // DO NOT MODIFY
    static void *_new(size_t size);
    static void _delete(void *ptr);
    static size_t malloc_called_count() noexcept;
    static size_t used_blocks_count() noexcept;
    static size_t split_called_count() noexcept;

    // Feel free to add new members to this class.
    static inline void setUsed(block_t *block, bool used);
    static bool isUsed(block_t *block) ;
    static void removeFromFreeList(block * b);
    static void splitBlock( block *b, size_t size );
    static void addToFreeList( block *freeBlock );
    static void scanAndCoalesce();
    static block* mergeBlocks(block* left_block, block* ptr_to_merge);
    static void checkCorruption(block* heap_block);

  };

  // DO NOT MODIFY
  void* sbrk(intptr_t __delta);
  int brk(void* __addr);
}




// DO NOT CHANGE ANYTHING BELOW THIS LINE
void *malloc(size_t size) noexcept;
void *calloc(size_t num_memb, size_t size_each) noexcept;
void free(void *ptr) noexcept;
void* operator new(size_t size);
void operator delete(void *address ) noexcept;
void* operator new[] ( size_t size );
void operator delete[] ( void* address ) noexcept;

#endif /* SNP_MEMORY_H_ */
