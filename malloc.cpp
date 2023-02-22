// This should have all the imports you need. Please don't import iostream.
#include <pthread.h>
#include "memory.h"
#include <new>
#include "string.h"

#include <semaphore.h>
namespace snp
{
// TODO Student

pthread_mutex_t head_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t count_lock = PTHREAD_MUTEX_INITIALIZER;


block *head = NULL;
size_t malloc_count = 0;
size_t split_count = 0;

inline void Memory::setUsed(block_t *block, bool used) 
{
  if(used)
  {
    block->used = true;
  }
  else
  {
    block->used = false;
  }
}

bool Memory::isUsed(block_t *block) 
{
  return block->used;
}


void Memory::checkCorruption(block* heap_block)
{
  if(heap_block->magic_number != 1313)
  {
    exit(-1);
  }  
}


void Memory::splitBlock( block *ptr_1, size_t required_size ) 
{

  block* new_p = (block *) ((unsigned long)ptr_1 + sizeof(block) + required_size );
  new_p->size = ptr_1->size - (sizeof( block ) + required_size );
  ptr_1->size = required_size;
  new_p->next = ptr_1->next;
  new_p->prev = ptr_1;
  if(new_p->next)
  {
    new_p->next->prev = new_p;
  }
  ptr_1->next = new_p;
  new_p->magic_number = 1313;
  setUsed(new_p, false);
}

void *Memory::malloc(size_t size)
{
  pthread_mutex_lock(&head_lock);
  block* ptr_1 = head;
  if(!size)
  {
    pthread_mutex_unlock(&head_lock);
    return NULL;
  }
  while(ptr_1)
	{
    checkCorruption(ptr_1); 

    if(ptr_1->size >= size && !isUsed(ptr_1))
    {
      
      if(ptr_1->size <= (size + sizeof(block)))
      {
        setUsed(ptr_1, true);
        malloc_count++;
        //split_count++;
        pthread_mutex_unlock(&head_lock);
        return (void *)((unsigned long)ptr_1 + sizeof(block));
      }
      else
      {
        splitBlock(ptr_1, size);
        setUsed(ptr_1, true);
        malloc_count++;
        //split_count++;
        pthread_mutex_unlock(&head_lock);
        return (void *)((unsigned long)ptr_1 + sizeof(block));
      }
 
    }

    if(ptr_1->next == NULL)
      break;
    
		ptr_1 = ptr_1->next;
	}


  size_t total_size;

  if(size < MIN_ALLOC)
  {
    total_size = MIN_ALLOC;
  }
  else
  {
    total_size = size + sizeof(block);
  }

  block* ptr_2 = (block*)sbrk(total_size);
  if(ptr_2 == (void*)-1)
  {
    pthread_mutex_unlock(&head_lock);
    return NULL;
  }
  if(ptr_1 == NULL)
  {
    head = ptr_2;
  }
  ptr_2->prev = ptr_1;
  ptr_2->next = NULL;
  ptr_2->size = total_size - sizeof(block);
  /*
  assert(ptr_2->size == total_size  - sizeof(block));
  assert(total_size == MIN_ALLOC);
  assert(sizeof(block) == 40);

  assert(ptr_2->size == 4056);
  */
  ptr_2->magic_number = 1313;
  setUsed(ptr_2, true);
  if(ptr_2->prev != NULL)
  {
    ptr_2->prev->next = ptr_2;
  }
  if(ptr_2->size > size + sizeof(block))
  {
    /*
    assert(ptr_2->size > size + sizeof(block));
    //split_count++;
    //block* new_p = splitBlock(ptr_2, size);
    assert(size + sizeof(block) < 50);
    assert(sizeof(block) == 40);
    assert(size < 10);
    */
    splitBlock(ptr_2, size);
  }
  malloc_count++;
   /*
  assert(ptr_2->next != NULL);
  assert(ptr_2->next->size > ptr_2->size);
  */
  void *return_pointer = (void *)((unsigned long)ptr_2 + sizeof(block));
  pthread_mutex_unlock(&head_lock);
  return return_pointer;
}

block* Memory::mergeBlocks(block* left_block, block* ptr_to_merge)
{
  left_block->size = left_block->size + ptr_to_merge->size +  sizeof(block);
  left_block->next = ptr_to_merge->next;
  if (left_block->next) 
    left_block->next->prev = left_block;
  return left_block;
}




void Memory::free(void *ptr)
{
  if (!ptr) 
    return;
  pthread_mutex_lock(&head_lock);
  block* ptr_2 = head;

  block* ptr_1 = (block*)((unsigned long)ptr - sizeof(block));
  while(ptr_2 != ptr_1)
  {
    if(ptr_2 == NULL)
    {
      pthread_mutex_unlock(&head_lock);
      exit(-1);
    }

    checkCorruption(ptr_2);
    ptr_2 = ptr_2->next;
  }
  if(!isUsed(ptr_1))
  {
    pthread_mutex_unlock(&head_lock);
    exit(-1);
  }

  setUsed(ptr_1, false);

  if (ptr_1->prev && !isUsed(ptr_1->prev))
  {
    ptr_1 = mergeBlocks(ptr_1->prev, ptr_1);

  }
  if (ptr_1->next && !isUsed(ptr_1->next))
  {
    ptr_1 = mergeBlocks(ptr_1 , ptr_1->next);

  }
  //brk(ptr_3);
  ptr_2 = head;
  while(ptr_2)
  {
    if(ptr_2->next == NULL)
    {
      if(!isUsed(ptr_2))
      {
        if(ptr_2->prev != NULL)
          ptr_2->prev->next = NULL;
        else  
          head = NULL;
        int error = brk(ptr_2);
        if(error == -1)
        {
          exit(-1);
        }
      }
      break;
    }
    checkCorruption(ptr_2);  
    ptr_2 = ptr_2->next;
  }
  pthread_mutex_unlock(&head_lock);
}

void *Memory::calloc(size_t num_memb, size_t size_each)
{
  if(num_memb == 0 || size_each == 0)
    return NULL;
  void* ptr = malloc(num_memb * size_each);
  memset(ptr, 0, num_memb * size_each);

  return ptr;
}



size_t Memory::malloc_called_count() noexcept
{
  return malloc_count;
}

size_t Memory::split_called_count() noexcept
{
  return split_count;
}


size_t Memory::used_blocks_count() noexcept
{
  pthread_mutex_lock(&head_lock);
  block *node = head;
  size_t allocated_blocks_counter = 0;

  while (node) { 
    if( isUsed(node))
    {
      allocated_blocks_counter++;
    }
    checkCorruption(node);  
    node = node->next;
  }

  pthread_mutex_unlock(&head_lock);
  return allocated_blocks_counter;
}

// TODO Student End
}
