/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * bltAlloc.h --
 *
 * Copyright 2015 George A. Howlett. All rights reserved.  
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions are
 *   met:
 *
 *   1) Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *   2) Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the
 *      distribution.
 *   3) Neither the name of the authors nor the names of its contributors
 *      may be used to endorse or promote products derived from this
 *      software without specific prior written permission.
 *   4) Products derived from this software may not be called "BLT" nor may
 *      "BLT" appear in their names without specific prior written
 *      permission from the author.
 *
 *   THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED
 *   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *   DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 *   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *   BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 *   WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 *   OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 *   IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

#ifndef _BLT_ALLOC_H
#define _BLT_ALLOC_H

/*
 *      Memory allocation/deallocation within BLT can be set to use
 *      specific memory allocators using Blt_AllocInit. The routine
 *      Blt_AllocInit allows you to specify your own memory
 *      allocation/deallocation routines for BLT on a library-wide basis.
 */
typedef void *(Blt_MallocProc) (size_t size);
typedef void *(Blt_ReallocProc) (void *ptr, size_t size);
typedef void (Blt_FreeProc) (void *ptr);

BLT_EXTERN void Blt_AllocInit(Blt_MallocProc *mallocProc, 
        Blt_ReallocProc *reallocProc, Blt_FreeProc *freeProc);
BLT_EXTERN void *Blt_Malloc(size_t size);
BLT_EXTERN void Blt_Free(const void *mem);
BLT_EXTERN void *Blt_Realloc(void *ptr, size_t size);
BLT_EXTERN void *Blt_Calloc(size_t numElem, size_t elemSize);
BLT_EXTERN void *Blt_MallocAbortOnError(size_t size, const char *fileName, 
        int lineNum);
BLT_EXTERN void *Blt_CallocAbortOnError(size_t numElem, size_t elemSize, 
        const char *fileName, int lineNum);
BLT_EXTERN void *Blt_ReallocAbortOnError(void *ptr, size_t size, 
        const char *fileName, int lineNum);
BLT_EXTERN const char *Blt_Strndup(const char *string, size_t size);
BLT_EXTERN const char *Blt_Strdup(const char *string);
BLT_EXTERN const char *Blt_StrdupAbortOnError(const char *string, 
        const char *fileName, int lineNum);
BLT_EXTERN const char *Blt_StrndupAbortOnError(const char *string, size_t size,
        const char *fileName, int lineNum);

#endif /* _BLT_ALLOC_H */

