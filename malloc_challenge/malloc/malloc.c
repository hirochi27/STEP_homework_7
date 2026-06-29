//
// >>>> malloc challenge! <<<<
//
// Your task is to improve utilization and speed of the following malloc
// implementation.
// Initial implementation is the same as the one implemented in simple_malloc.c.
// For the detailed explanation, please refer to simple_malloc.c.

#include <assert.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//
// Interfaces to get memory pages from OS
//

void *mmap_from_system(size_t size);
void munmap_to_system(void *ptr, size_t size);

//
// Struct definitions
//

typedef struct my_metadata_t {
  size_t size;
  struct my_metadata_t *next;
} my_metadata_t;

// リストの頭を4つ持つ
typedef struct my_heap_t {
  my_metadata_t *free_head[4];
  my_metadata_t dummy[4];
} my_heap_t;

//
// Static variables (DO NOT ADD ANOTHER STATIC VARIABLES!)
//
my_heap_t my_heap;

//
// Helper functions (feel free to add/remove/edit!)
//

// free_listをそれぞれのサイズごとに配列に追加する
void my_add_to_free_list(my_metadata_t *metadata) {
  assert(!metadata->next);
  if (metadata->size <= 1024){
    metadata->next = my_heap.free_head[0];
    my_heap.free_head[0] = metadata;
  } 

  else if (metadata->size > 1024 && metadata->size <= 2048){
    metadata->next = my_heap.free_head[1];
    my_heap.free_head[1] = metadata;
  } 

  else if (metadata->size > 2048 && metadata->size <= 3072){
    metadata->next = my_heap.free_head[2];
    my_heap.free_head[2] = metadata;
  } 

  else if (metadata->size > 3072 && metadata->size <= 4096){
    metadata->next = my_heap.free_head[3];
    my_heap.free_head[3] = metadata;
  } 
}

// 割り当てたfree_listを削除
// metadataのサイズをかくにんして、どこを消すか判断
void my_remove_from_free_list(my_metadata_t *metadata, my_metadata_t *prev) {
  if (metadata->size <= 1024){
    if (prev) {
      prev->next = metadata->next;
    } else {
      my_heap.free_head[0] = metadata->next;
    }
    metadata->next = NULL;
  }
  else if (metadata->size > 1024 && metadata->size <= 2048){
    if (prev) {
      prev->next = metadata->next;
    } else {
      my_heap.free_head[1] = metadata->next;
    }
    metadata->next = NULL;
  }
  else if (metadata->size > 2048 && metadata->size <= 3072){
    if (prev) {
      prev->next = metadata->next;
    } else {
      my_heap.free_head[2] = metadata->next;
    }
    metadata->next = NULL;
  }
  else if (metadata->size > 3072 && metadata->size <= 4096){
    if (prev) {
      prev->next = metadata->next;
    } else {
      my_heap.free_head[3] = metadata->next;
    }
    metadata->next = NULL;
  }

}


// // 右側もfree_listだった場合、結合する
// void my_merge_right(my_metadata_t *metadata) {
//   //metadataのポインタの位置からsize分だけ右に移動したポインタが示すところを見たい
//   if (metadata + metadata->size + 1 == metadata->next){
//     // free_listがアドレス順じゃないので、これだと右隣を調べられていない
//     // アドレス的に見た、右隣を指定しないといけない
//     metadata->size = metadata->size + metadata->next->size;
//     metadata->next = metadata->next->next;
//   }
// }


//
// Interfaces of malloc (DO NOT RENAME FOLLOWING FUNCTIONS!)
//

// This is called at the beginning of each challenge.
void my_initialize() {
  my_heap.free_head[0] = &my_heap.dummy[0];
  my_heap.dummy[0].size = 0;
  my_heap.dummy[0].next = NULL;

  my_heap.free_head[1] = &my_heap.dummy[1];
  my_heap.dummy[1].size = 0;
  my_heap.dummy[1].next = NULL;
  
  my_heap.free_head[2] = &my_heap.dummy[2];
  my_heap.dummy[2].size = 0;
  my_heap.dummy[2].next = NULL;
  
  my_heap.free_head[3] = &my_heap.dummy[3];
  my_heap.dummy[3].size = 0;
  my_heap.dummy[3].next = NULL;
  
}

// my_malloc() is called every time an object is allocated.
// |size| is guaranteed to be a multiple of 8 bytes and meets 8 <= |size| <=
// 4000. You are not allowed to use any library functions other than
// mmap_from_system() / munmap_to_system().
void *my_malloc(size_t size) {
  my_metadata_t *metadata = NULL;
  my_metadata_t *prev = NULL;
  my_metadata_t *best = NULL;
  my_metadata_t *best_prev = NULL;

  if (size <= 1024) {
    metadata = my_heap.free_head[0];
  } else if (size > 1024 && size <= 2048) {
    metadata = my_heap.free_head[1];
  } else if (size > 2048 && size <= 3072) {
    metadata = my_heap.free_head[2];
  } else if (size > 3072 && size <= 4096) {
    metadata = my_heap.free_head[3];
  }

  // First-fit: Find the first free slot the object fits.
  // TODO: Update this logic to Best-fit!

  while (metadata) {
    // 探しているmetadataのサイズが十分大きい場合
    // その中で一番小さいmetadataを保存
    if (metadata->size >= size){
      if (!best || metadata->size < best->size){
        best = metadata;
        best_prev = prev;
      }
    }
    // metadataを最後までみて、一番最適なdataを見つける
    prev = metadata;
    metadata = metadata->next;
  }

  // 最適な値を返す
  metadata = best; 
  prev = best_prev;

  // now, metadata points to the first free slot
  // and prev is the previous entry.



  if (!metadata) {
    // There was no free slot available. We need to request a new memory region
    // from the system by calling mmap_from_system().
    //
    //     | metadata | free slot |
    //     ^
    //     metadata
    //     <---------------------->
    //            buffer_size
    size_t buffer_size = 4096;
    my_metadata_t *metadata = (my_metadata_t *)mmap_from_system(buffer_size);
    metadata->size = buffer_size - sizeof(my_metadata_t);
    metadata->next = NULL;
    // Add the memory region to the free list.
    my_add_to_free_list(metadata);
    // Now, try my_malloc() again. This should succeed.
    return my_malloc(size);
  }

  // |ptr| is the beginning of the allocated object.
  //
  // ... | metadata | object | ...
  //     ^          ^
  //     metadata   ptr
  void *ptr = metadata + 1;
  size_t remaining_size = metadata->size - size;
  // Remove the free slot from the free list.
  my_remove_from_free_list(metadata, prev);

  if (remaining_size > sizeof(my_metadata_t)) {
    // Shrink the metadata for the allocated object
    // to separate the rest of the region corresponding to remaining_size.
    // If the remaining_size is not large enough to make a new metadata,
    // this code path will not be taken and the region will be managed
    // as a part of the allocated object.
    metadata->size = size;
    // Create a new metadata for the remaining free slot.
    //
    // ... | metadata | object | metadata | free slot | ...
    //     ^          ^        ^
    //     metadata   ptr      new_metadata
    //                 <------><---------------------->
    //                   size       remaining size
    my_metadata_t *new_metadata = (my_metadata_t *)((char *)ptr + size);
    new_metadata->size = remaining_size - sizeof(my_metadata_t);
    new_metadata->next = NULL;
    // Add the remaining free slot to the free list.
    my_add_to_free_list(new_metadata);
  }
  return ptr;
}

// This is called every time an object is freed.  You are not allowed to
// use any library functions other than mmap_from_system / munmap_to_system.
void my_free(void *ptr) {
  // Look up the metadata. The metadata is placed just prior to the object.
  //
  // ... | metadata | object | ...
  //     ^          ^
  //     metadata   ptr
  my_metadata_t *metadata = (my_metadata_t *)ptr - 1;
  // Add the free slot to the free list.
  my_add_to_free_list(metadata);
}

// This is called at the end of each challenge.
void my_finalize() {
  // Nothing is here for now.
  // feel free to add something if you want!
}

void test() {
  // Implement here!
  assert(1 == 1); /* 1 is 1. That's always true! (You can remove this.) */
}
