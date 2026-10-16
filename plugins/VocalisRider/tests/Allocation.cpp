/*
 * Vocalis Audio — Vocalis Rider Live
 *
 * Created and maintained by Enos Takanaka
 * Copyright (C) 2026 Enos Takanaka
 *
 * Developed with AI assistance using OpenAI Codex.
 */
// SPDX-License-Identifier: AGPL-3.0-or-later
#include <cstddef>
#include <atomic>
thread_local bool watchAllocations=false;
thread_local int allocationCount=0;
extern "C" void* __real_malloc(std::size_t);
extern "C" void* __real_calloc(std::size_t,std::size_t);
extern "C" void* __real_realloc(void*,std::size_t);
extern "C" void* __wrap_malloc(std::size_t n){if(watchAllocations)++allocationCount;return __real_malloc(n);}
extern "C" void* __wrap_calloc(std::size_t n,std::size_t s){if(watchAllocations)++allocationCount;return __real_calloc(n,s);}
extern "C" void* __wrap_realloc(void* p,std::size_t s){if(watchAllocations)++allocationCount;return __real_realloc(p,s);}
// Track executable-visible C++ allocation paths as well as direct C allocator calls.
#include <new>
#include <cstdlib>
void* operator new(std::size_t n){if(watchAllocations)++allocationCount;if(void* p=__real_malloc(n?n:1))return p;throw std::bad_alloc();}
void* operator new[](std::size_t n){return ::operator new(n);}
void operator delete(void* p) noexcept {std::free(p);}
void operator delete[](void* p) noexcept {std::free(p);}
void operator delete(void* p,std::size_t) noexcept {std::free(p);}
void operator delete[](void* p,std::size_t) noexcept {std::free(p);}
