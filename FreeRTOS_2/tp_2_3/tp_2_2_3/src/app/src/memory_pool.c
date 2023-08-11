/*
 * Copyright (c) YEAR NOMBRE <MAIL>.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING
 * IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 *
 * @file   : memory_pool.c
 * @date   : Mar 16, 2023
 * @author : NOMBRE <MAIL>
 * @version	v1.0.0
 */

/********************** inclusions *******************************************/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "memory_pool.h"
#include "driver.h"

/********************** macros and definitions *******************************/

/********************** internal data declaration ****************************/

/********************** internal functions declaration ***********************/

/********************** internal data definition *****************************/

/********************** external data definition *****************************/

/********************** internal functions definition ************************/

/********************** external functions definition ************************/

size_t memory_pool_init(memory_pool_t* hmp, void* pmemory, size_t memory_size, size_t block_size)
{
  hmp->pmemory = pmemory;
  hmp->memory_size = memory_size;
  hmp->block_size = block_size;
  hmp->nblocks = memory_size / block_size;
  hmp->nblocks_free = 0;

  linked_list_t* hlist = &(hmp->block_list);
  linked_list_init(hlist);

  for(size_t i = 0; i < hmp->nblocks; ++i)
  {
    memory_pool_block_t* pblock = (memory_pool_block_t*)(hmp->pmemory + i*block_size);
    linked_list_node_init(pblock, NULL);
    linked_list_node_add(hlist, pblock);
    hmp->nblocks_free++;
  }

  return hmp->nblocks;
}

size_t memory_pool_blocks(memory_pool_t* hmp)
{
  return hmp->nblocks;
}

size_t memory_pool_blocks_free(memory_pool_t* hmp)
{
  return hmp->nblocks_free;
}

void* memory_pool_block_get(memory_pool_t* hmp)
{
  portENTER_CRITICAL(); //Enter on critical section
  linked_list_t* hlist = &(hmp->block_list);
  void* pblock = (void*)linked_list_node_remove(hlist);
  if(NULL != pblock)
  {
    hmp->nblocks_free--;
  }
  portEXIT_CRITICAL(); //Exit from critical section
  return pblock;
}

void memory_pool_block_put(memory_pool_t* hmp, void* pblock)
{
  portENTER_CRITICAL(); //Enter on critical section
  if(NULL != pblock)
  {
    hmp->nblocks_free++;
    linked_list_t* hlist = &(hmp->block_list);
    linked_list_node_init((memory_pool_block_t*)pblock, NULL);
    linked_list_node_add(hlist, pblock);
  }
  portEXIT_CRITICAL(); //Exit from critical section
}

/********************** end of file ******************************************/
