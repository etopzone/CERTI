/*
 * Two Levels Segregate Fit memory allocator (TLSF)
 * Version 2.4.6
 *
 * Written by Miguel Masmano Tello <mimastel@doctor.upv.es>
 *
 * Thanks to Ismael Ripoll for his suggestions and reviews
 *
 * Copyright (C) 2008, 2007, 2006, 2005, 2004
 *
 * This code is released using a dual license strategy: GPL/LGPL
 * You can choose the licence that better fits your requirements.
 *
 * Released under the terms of the GNU General Public License Version 2.0
 * Released under the terms of the GNU Lesser General Public License Version 2.1
 *
 */

#ifndef _TLSF_H_
#define _TLSF_H_

#include <stddef.h>

extern size_t init_memory_pool(size_t mem_pool_size, void* mem_pool);
extern size_t get_used_size(void* mem_pool);
extern size_t get_max_size(void* mem_pool);
extern void destroy_memory_pool(void* mem_pool);
extern size_t add_new_area(void* area, size_t area_size, void* mem_pool);
extern void* malloc_ex(size_t size, void* mem_pool);
extern void free_ex(void* ptr, void* mem_pool);
extern void* realloc_ex(void* ptr, size_t new_size, void* mem_pool);
extern void* calloc_ex(size_t nelem, size_t elem_size, void* mem_pool);

extern void* tlsf_malloc(size_t size);
extern void tlsf_free(void* ptr);
extern void* tlsf_realloc(void* ptr, size_t size);
extern void* tlsf_calloc(size_t nelem, size_t elem_size);

#endif
