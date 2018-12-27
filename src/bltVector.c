/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * bltVector.c --
 *
 * This module implements vector data objects.
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

/*
 * TODO:
 *      o Add H. Kirsch's vector binary read operation
 *              x binread file0
 *              x binread -file file0
 *
 *      o Add ASCII/binary file reader
 *              x read fileName
 *
 *      o Allow Tcl-based client notifications.
 *              vector x
 *              x notify call Display
 *              x notify delete Display
 *              x notify reorder #1 #2
 */

#include "bltVecInt.h"

#ifdef HAVE_STDLIB_H
  #include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#ifdef HAVE_STRING_H
  #include <string.h>
#endif /* HAVE_STRING_H */

#ifdef HAVE_CTYPE_H
  #include <ctype.h>
#endif /* HAVE_CTYPE_H */

#ifdef TIME_WITH_SYS_TIME
  #include <sys/time.h>
  #include <time.h>
#else
  #ifdef HAVE_SYS_TIME_H
    #include <sys/time.h>
  #else
    #include <time.h>
  #endif /* HAVE_SYS_TIME_H */
#endif /* TIME_WITH_SYS_TIME */

#include "bltAlloc.h"
#include <bltMath.h>
#include "bltNsUtil.h"
#include "bltSwitch.h"
#include "bltOp.h"
#include "bltInitCmd.h"

#ifndef TCL_NAMESPACE_ONLY
#define TCL_NAMESPACE_ONLY TCL_GLOBAL_ONLY
#endif

#define DEF_ARRAY_SIZE          64
#define TRACE_ALL  (TCL_TRACE_WRITES | TCL_TRACE_READS | TCL_TRACE_UNSETS)


#define VECTOR_CHAR(c)  ((isalnum(UCHAR(c))) || \
        (c == '_') || (c == ':') || (c == '@') || (c == '.'))


/*
 * VectorClient --
 *
 *      A vector can be shared by several clients.  Each client allocates
 *      this structure that acts as its key for using the vector.  Clients
 *      can also designate a callback routine that is executed whenever the
 *      vector is updated or destroyed.
 *
 */
typedef struct {
    unsigned int magic;                 /* Magic value designating whether
                                         * this really is a vector token or
                                         * not */
    VectorObject *serverPtr;            /* Pointer to the master record of
                                         * the vector.  If NULL, indicates
                                         * that the vector has been
                                         * destroyed but as of yet, this
                                         * client hasn't recognized it. */
    Blt_VectorChangedProc *proc;        /* Routine to call when the
                                         * contents of the vector change or
                                         * the vector is deleted. */
    ClientData clientData;              /* Data passed whenever the vector
                                         * change procedure is called. */
    Blt_ChainLink link;                 /* Used to quickly remove this
                                         * entry from its server's client
                                         * chain. */
} VectorClient;

static Tcl_CmdDeleteProc VectorInstDeleteProc;
static Tcl_ObjCmdProc VectorCmd;
static Tcl_InterpDeleteProc VectorInterpDeleteProc;

typedef struct {
    const char *varName;                /* Requested variable name. */
    const char *cmdName;                /* Requested command name. */
    int flush;                          /* Flush */
    int watchUnset;                     /* Watch when variable is unset. */
    int size;
    int first, last;
} CreateSwitches;

static Blt_SwitchSpec createSwitches[] = 
{
    {BLT_SWITCH_STRING, "-variable", "varName", (char *)NULL,
        Blt_Offset(CreateSwitches, varName), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_STRING, "-command", "command", (char *)NULL,
        Blt_Offset(CreateSwitches, cmdName), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_BOOLEAN, "-watchunset", "bool", (char *)NULL,
        Blt_Offset(CreateSwitches, watchUnset), 0},
    {BLT_SWITCH_BOOLEAN, "-flush", "bool", (char *)NULL,
        Blt_Offset(CreateSwitches, flush), 0},
    {BLT_SWITCH_LONG_POS, "-length", "length", (char *)NULL,
        Blt_Offset(CreateSwitches, size), 0},
    {BLT_SWITCH_END}
};

typedef int (VectorCmdProc)(VectorObject *vecObjPtr, Tcl_Interp *interp, 
        int objc, Tcl_Obj *const *objv);

static VectorObject *
FindVectorInNamespace(VectorCmdInterpData *dataPtr, Blt_ObjectName *objNamePtr)
{
    Tcl_DString ds;
    const char *name;
    Blt_HashEntry *hPtr;

    name = Blt_MakeQualifiedName(objNamePtr, &ds);
    hPtr = Blt_FindHashEntry(&dataPtr->vectorTable, name);
    Tcl_DStringFree(&ds);
    if (hPtr != NULL) {
        return Blt_GetHashValue(hPtr);
    }
    return NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * GetVectorObject --
 *
 *      Searches for the vector associated with the name given.  Allow for
 *      a range specification.
 *
 * Results:
 *      Returns a pointer to the vector if found, otherwise NULL.
 *
 *---------------------------------------------------------------------------
 */
static VectorObject *
GetVectorObject(
    VectorCmdInterpData *dataPtr,       /* Interpreter-specific data. */
    const char *name,
    int flags)
{
    Blt_ObjectName objName;
    VectorObject *vecObjPtr;
    Tcl_Interp *interp;

    interp = dataPtr->interp;
    if (!Blt_ParseObjectName(interp, name, &objName, 
                BLT_NO_ERROR_MSG | BLT_NO_DEFAULT_NS)) {
        return NULL;                    /* Can't find namespace. */
    } 
    vecObjPtr = NULL;
    if (objName.nsPtr != NULL) {
        vecObjPtr = FindVectorInNamespace(dataPtr, &objName);
    } else {
        if (flags & NS_SEARCH_CURRENT) {
            objName.nsPtr = Tcl_GetCurrentNamespace(interp);
            vecObjPtr = FindVectorInNamespace(dataPtr, &objName);
        }
        if ((vecObjPtr == NULL) && (flags & NS_SEARCH_GLOBAL)) {
            objName.nsPtr = Tcl_GetGlobalNamespace(interp);
            vecObjPtr = FindVectorInNamespace(dataPtr, &objName);
        }
    }
    return vecObjPtr;
}

void
Blt_VecObj_UpdateRange(VectorObject *vecObjPtr)
{
    double min, max;
    double *vp, *vend;

    vp = vecObjPtr->valueArr + vecObjPtr->first;
    vend = vecObjPtr->valueArr + vecObjPtr->last;
    min = max = *vp++;
    for (/* empty */; vp < vend; vp++) {
        if (min > *vp) {
            min = *vp; 
        } else if (max < *vp) { 
            max = *vp; 
        } 
    } 
    vecObjPtr->min = min;
    vecObjPtr->max = max;
    vecObjPtr->notifyFlags &= ~UPDATE_RANGE;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_GetSpecialIndex --
 *
 *      Converts the string representing an index in the vector, to its
 *      numeric value.  A valid index may be an numeric string of the
 *      string "end" (indicating the last element in the string).
 *
 * Results:
 *      A standard TCL result.  If the string is a valid index, TCL_OK is
 *      returned.  Otherwise TCL_ERROR is returned and interp->result will
 *      contain an error message.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_VecObj_GetSpecialIndex(Tcl_Interp *interp, VectorObject *vecObjPtr, 
                           const char *string, Blt_VectorIndexProc **procPtrPtr)
{
    Blt_HashEntry *hPtr;
    
    hPtr = Blt_FindHashEntry(&vecObjPtr->dataPtr->indexProcTable, string);
    if (hPtr == NULL) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "bad index \"", string, "\"", 
                             (char *)NULL);
        }
        return TCL_ERROR;
    }
    *procPtrPtr = Blt_GetHashValue(hPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_GetIndex --
 *
 *      Converts the string representing an index in the vector, to its
 *      numeric value.  A valid index may be an numeric string of the
 *      string "end" (indicating the last element in the string).
 *
 * Results:
 *      A standard TCL result.  If the string is a valid index, TCL_OK is
 *      returned.  Otherwise TCL_ERROR is returned and interp->result will
 *      contain an error message.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_VecObj_GetIndex(Tcl_Interp *interp, VectorObject *vecObjPtr, 
                    const char *string, int *indexPtr)
{
    char c;
    int index;
    
    c = string[0];

    /* Treat the index "end" like a numeric index.  */

    if ((c == 'e') && (strcmp(string, "end") == 0)) {
        if (vecObjPtr->length == 0) {
            if (interp != NULL) {
                Tcl_AppendResult(interp, "bad index \"end\": vector is empty", 
                                 (char *)NULL);
            }
            return TCL_ERROR;
        }
        *indexPtr = vecObjPtr->length - 1;
        return TCL_OK;
    }
    /* Try to process the index as a number. May be negative. */
    if (Tcl_GetInt(interp, string, &index) != TCL_OK) {
        long lval;
        /*   
         * Try to process the index as a numeric expression. 
         *
         * Unlike Tcl_GetInt, Tcl_ExprLong needs a valid interpreter, but
         * the interp passed in may be NULL.  So we have to use
         * vecObjPtr->interp and then reset the result.
         */
        if (Tcl_ExprLong(vecObjPtr->interp, (char *)string, &lval) != TCL_OK) {
            Tcl_ResetResult(vecObjPtr->interp);
            if (interp != NULL) {
                Tcl_AppendResult(interp, "bad index \"", string, "\"", 
                                 (char *)NULL);
            }
            return TCL_ERROR;
        }
        index = lval;
    }
    /*
     * Correct the index by the current value of the offset. This makes all
     * the numeric indices non-negative.
     */
    if ((vecObjPtr->offset > index) || 
        ((index - vecObjPtr->offset) >= vecObjPtr->length))  {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "index \"", string, "\" is out of range", 
                         (char *)NULL);
        }
        return TCL_ERROR;
    }
    *indexPtr = index - vecObjPtr->offset;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_GetRange --
 *
 *      Converts the string representing an index in the vector, to its
 *      numeric value.  A valid index may be an numeric string of the
 *      string "end" (indicating the last element in the string).
 *
 *      "all"
 *      "end"
 *      n:m, n:"end"
 *
 * Results:
 *      A standard TCL result.  If the string is a valid index, TCL_OK is
 *      returned.  Otherwise TCL_ERROR is returned and interp->result will
 *      contain an error message.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_VecObj_GetRange(Tcl_Interp *interp, VectorObject *vecObjPtr, 
                    const char *string)
{
    char *colon;
    char c;

    c = string[0];
    colon = strchr(string, ':');
    if (colon != NULL) {
        if (string == colon) {
            vecObjPtr->first = 0;       /* Colon is first character.  First
                                         * index is defaults to 0. */
        } else {
            int result;
            int index;

            *colon = '\0';
            result = Blt_VecObj_GetIndex(interp, vecObjPtr, string, &index);
            *colon = ':';
            if (result != TCL_OK) {
                return TCL_ERROR;
            }
            vecObjPtr->first = index;
        }
        if (*(colon + 1) == '\0') {
            /* Default to the last index */
            vecObjPtr->last = vecObjPtr->length;
        } else {
            int index;

            if (Blt_VecObj_GetIndex(interp, vecObjPtr, colon + 1, &index) 
                != TCL_OK) {
                return TCL_ERROR;
            }
            vecObjPtr->last = index + 1;
        }
        if (vecObjPtr->first >= vecObjPtr->last) {
            if (interp != NULL) {
                Tcl_AppendResult(interp, "bad range \"", string,
                         "\" (first >= last)", (char *)NULL);
            }
            return TCL_ERROR;
        }
    } else if ((c == 'a') && (strcmp(string, "all") == 0)) {
        vecObjPtr->first = 0;
        vecObjPtr->last = vecObjPtr->length;
    } else {
        int index;

        if (Blt_VecObj_GetIndex(interp, vecObjPtr, string, &index) != TCL_OK) {
            return TCL_ERROR;
        }
        vecObjPtr->first = index;
        vecObjPtr->last = index + 1;
    }
    return TCL_OK;
}

VectorObject *
Blt_VecObj_ParseElement(
    Tcl_Interp *interp,
    VectorCmdInterpData *dataPtr,       /* Interpreter-specific data. */
    const char *start,
    const char **endPtr,
    int flags)
{
    char *p;
    char saved;
    VectorObject *vecObjPtr;

    p = (char *)start;
    /* Find the end of the vector name */
    while (VECTOR_CHAR(*p)) {
        p++;
    }
    saved = *p;
    *p = '\0';

    vecObjPtr = GetVectorObject(dataPtr, start, flags);
    if (vecObjPtr == NULL) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "can't find a vector named \"", 
                start, "\"", (char *)NULL);
        }
        *p = saved;
        return NULL;
    }
    *p = saved;
    vecObjPtr->first = 0;
    vecObjPtr->last = vecObjPtr->length;
    if (*p == '(') {
        int count, result;

        start = p + 1;
        p++;

        /* Find the matching right parenthesis */
        count = 1;
        while (*p != '\0') {
            if (*p == ')') {
                count--;
                if (count == 0) {
                    break;
                }
            } else if (*p == '(') {
                count++;
            }
            p++;
        }
        if (count > 0) {
            if (interp != NULL) {
                Tcl_AppendResult(interp, "unbalanced parentheses \"", start, 
                        "\"", (char *)NULL);
            }
            return NULL;
        }
        *p = '\0';
        result = Blt_VecObj_GetRange(interp, vecObjPtr, start);
        *p = ')';
        if (result != TCL_OK) {
            return NULL;
        }
        p++;
    }
    if (endPtr != NULL) {
      *endPtr = p;
    }
    return vecObjPtr;
}


/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_NotifyClients --
 *
 *      Notifies each client of the vector that the vector has changed
 *      (updated or destroyed) by calling the provided function back.  The
 *      function pointer may be NULL, in that case the client is not
 *      notified.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The results depend upon what actions the client callbacks
 *      take.
 *
 *---------------------------------------------------------------------------
 */
void
Blt_VecObj_NotifyClients(ClientData clientData)
{
    VectorObject *vecObjPtr = clientData;
    Blt_ChainLink link, next;
    Blt_VectorNotify notify;

    notify = (vecObjPtr->notifyFlags & NOTIFY_DESTROYED)
        ? BLT_VECTOR_NOTIFY_DESTROY : BLT_VECTOR_NOTIFY_UPDATE;
    vecObjPtr->notifyFlags &= 
        ~(NOTIFY_UPDATED | NOTIFY_DESTROYED | NOTIFY_PENDING);
    for (link = Blt_Chain_FirstLink(vecObjPtr->chain); link != NULL; 
         link = next) {
        VectorClient *clientPtr;

        next = Blt_Chain_NextLink(link);
        clientPtr = Blt_Chain_GetValue(link);
        if ((clientPtr->proc != NULL) && (clientPtr->serverPtr != NULL)) {
            (*clientPtr->proc)(vecObjPtr->interp, clientPtr->clientData, 
                               notify);
        }
    }
    /*
     * Some clients may not handle the "destroy" callback properly (they
     * should call Blt_FreeVectorToken to release the client identifier), so
     * mark any remaining clients to indicate that vector's server has gone
     * away.
     */
    if (notify == BLT_VECTOR_NOTIFY_DESTROY) {
        for (link = Blt_Chain_FirstLink(vecObjPtr->chain); link != NULL;
            link = Blt_Chain_NextLink(link)) {
            VectorClient *clientPtr;

            clientPtr = Blt_Chain_GetValue(link);
            clientPtr->serverPtr = NULL;
        }
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_UpdateClients --
 *
 *      Notifies each client of the vector that the vector has changed
 *      (updated or destroyed) by calling the provided function back.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The individual client callbacks are eventually invoked.
 *
 *---------------------------------------------------------------------------
 */
void
Blt_VecObj_UpdateClients(VectorObject *vecObjPtr)
{
    vecObjPtr->dirty++;
    vecObjPtr->max = vecObjPtr->min = Blt_NaN();
    if (vecObjPtr->notifyFlags & NOTIFY_NEVER) {
        return;
    }
    vecObjPtr->notifyFlags |= NOTIFY_UPDATED;
    if (vecObjPtr->notifyFlags & NOTIFY_ALWAYS) {
        Blt_VecObj_NotifyClients(vecObjPtr);
        return;
    }
    if (!(vecObjPtr->notifyFlags & NOTIFY_PENDING)) {
        vecObjPtr->notifyFlags |= NOTIFY_PENDING;
        Tcl_DoWhenIdle(Blt_VecObj_NotifyClients, vecObjPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_FlushCache --
 *
 *      Unsets all the elements of the TCL array variable associated with
 *      the vector, freeing memory associated with the variable.  This
 *      includes both the hash table and the hash keys.  The down side is
 *      that this effectively flushes the caching of vector elements in the
 *      array.  This means that the subsequent reads of the array will
 *      require a decimal to string conversion.
 *
 *      This is needed when the vector changes its values, making the array
 *      variable out-of-sync.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      All elements of array variable (except one) are unset, freeing the
 *      memory associated with the variable.
 *
 *---------------------------------------------------------------------------
 */
void
Blt_VecObj_FlushCache(VectorObject *vecObjPtr)
{
    Tcl_Interp *interp = vecObjPtr->interp;

    if (vecObjPtr->arrayName == NULL) {
        return;                 /* Doesn't use the variable API */
    }
    /* Turn off the trace temporarily so that we can unset all the elements
     * in the array.  */

    Tcl_UntraceVar2(interp, vecObjPtr->arrayName, (char *)NULL,
        TRACE_ALL | vecObjPtr->varFlags, Blt_VecObj_VarTrace, vecObjPtr);

    /* Clear all the element entries from the entire array */
    Tcl_UnsetVar2(interp, vecObjPtr->arrayName, (char *)NULL, 
                  vecObjPtr->varFlags);

    /* Restore the "end" index by default and the trace on the entire
     * array */
    Tcl_SetVar2(interp, vecObjPtr->arrayName, "end", "", vecObjPtr->varFlags);
    Tcl_TraceVar2(interp, vecObjPtr->arrayName, (char *)NULL,
        TRACE_ALL | vecObjPtr->varFlags, Blt_VecObj_VarTrace, vecObjPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_Find --
 *
 *      Searches for the vector associated with the name given.  Allow for
 *      a range specification.
 *
 * Results:
 *      Returns a pointer to the vector if found, otherwise NULL.  If the
 *      name is not associated with a vector and the TCL_LEAVE_ERR_MSG flag
 *      is set, and interp->result will contain an error message.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_VecObj_Find(Tcl_Interp *interp, VectorCmdInterpData *dataPtr,
             const char *vecName, VectorObject **vecObjPtrPtr)
{
    VectorObject *vecObjPtr;
    const char *endPtr;

    vecObjPtr = Blt_VecObj_ParseElement(interp, dataPtr, vecName, &endPtr,
          NS_SEARCH_BOTH);
    if (vecObjPtr == NULL) {
        return TCL_ERROR;
    }
    if (*endPtr != '\0') {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "extra characters after vector name",
                             (char *)NULL);
        }
        return TCL_ERROR;
    }
    *vecObjPtrPtr = vecObjPtr;
    return TCL_OK;
}

double
Blt_VecObj_Min(VectorObject *vecObjPtr)
{
    long i;
    double min;

    for (i = vecObjPtr->first; i < vecObjPtr->last; i++) {
        if (FINITE(vecObjPtr->valueArr[i])) {
            break;
        }
    }
    if (i == vecObjPtr->last) {
        return Blt_NaN();
    }
    min = vecObjPtr->valueArr[i];
    for (/* empty */; i < vecObjPtr->last; i++) {
        if (!FINITE(vecObjPtr->valueArr[i])) {
            continue;
        }
        if (min > vecObjPtr->valueArr[i]) {
            min = vecObjPtr->valueArr[i]; 
        } 
    } 
    vecObjPtr->min = min;
    return vecObjPtr->min;
}

double
Blt_VecObj_Max(VectorObject *vecObjPtr)
{
    long i;
    double max;

    for (i = vecObjPtr->first; i < vecObjPtr->last; i++) {
        if (FINITE(vecObjPtr->valueArr[i])) {
            break;
        }
    }
    if (i == vecObjPtr->last) {
        return Blt_NaN();
    }
    max = vecObjPtr->valueArr[i];
    for (/* empty */; i < vecObjPtr->last; i++) {
        if (!FINITE(vecObjPtr->valueArr[i])) {
            continue;
        }
        if (max < vecObjPtr->valueArr[i]) {
            max = vecObjPtr->valueArr[i]; 
        } 
    } 
    vecObjPtr->max = max;
    return vecObjPtr->max;
}

/*
 *---------------------------------------------------------------------------
 *
 * DeleteCommand --
 *
 *      Deletes the TCL command associated with the vector, without
 *      triggering a callback to "VectorInstDeleteProc".
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static void
DeleteCommand(VectorObject *vecObjPtr)   
{
    Blt_ObjectName objName;
    Tcl_CmdInfo cmdInfo;
    Tcl_DString ds;
    Tcl_Interp *interp = vecObjPtr->interp;
    const char *qualName;               /* Name of TCL command. */

    Tcl_DStringInit(&ds);
    objName.name = Tcl_GetCommandName(interp, vecObjPtr->cmdToken);
    objName.nsPtr = Blt_GetCommandNamespace(vecObjPtr->cmdToken);
    qualName = Blt_MakeQualifiedName(&objName, &ds);
    if (Tcl_GetCommandInfo(interp, qualName, &cmdInfo)) {
        /* Disable the callback before deleting the TCL command.*/      
        cmdInfo.deleteProc = NULL;      
        Tcl_SetCommandInfo(interp, qualName, &cmdInfo);
        Tcl_DeleteCommandFromToken(interp, vecObjPtr->cmdToken);
    }
    Tcl_DStringFree(&ds);
    vecObjPtr->cmdToken = 0;
}

/*
 *---------------------------------------------------------------------------
 *
 * UnmapVariable --
 *
 *      Destroys the trace on the current TCL variable designated to access
 *      the vector.
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static void
UnmapVariable(VectorObject *vecObjPtr)
{
    Tcl_Interp *interp = vecObjPtr->interp;

    /* Unset the entire array */
    Tcl_UntraceVar2(interp, vecObjPtr->arrayName, (char *)NULL,
        (TRACE_ALL | vecObjPtr->varFlags), Blt_VecObj_VarTrace, vecObjPtr);
    Tcl_UnsetVar2(interp, vecObjPtr->arrayName, (char *)NULL, 
                  vecObjPtr->varFlags);

    if (vecObjPtr->arrayName != NULL) {
        Blt_Free(vecObjPtr->arrayName);
        vecObjPtr->arrayName = NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_MapVariable --
 *
 *      Sets up traces on a TCL variable to access the vector.
 *
 *      If another variable is already mapped, it's first untraced and
 *      removed.  Don't do anything else for variables named "" (even
 *      though TCL allows this pathology). Saves the name of the new array
 *      variable.
 *
 * Results:
 *      A standard TCL result. If an error occurs setting the variable
 *      TCL_ERROR is returned and an error message is left in the
 *      interpreter.
 *
 * Side effects:
 *      Traces are set for the new variable. The new variable name is saved
 *      in a malloc'ed string in vecObjPtr->arrayName.  If this variable is
 *      non-NULL, it indicates that a TCL variable has been mapped to this
 *      vector.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_VecObj_MapVariable(
    Tcl_Interp *interp, 
    VectorObject *vecObjPtr, 
    const char *path)
{
    Blt_ObjectName objName;
    const char *newPath;
    const char *result;
    Tcl_DString ds;

    if (vecObjPtr->arrayName != NULL) {
        UnmapVariable(vecObjPtr);
    }
    if ((path == NULL) || (path[0] == '\0')) {
        return TCL_OK;                  /* If the variable pathname is the
                                         * empty string, simply return
                                         * after removing any existing
                                         * variable. */
    }
    /* Get the variable name (without the namespace qualifier). */
    if (!Blt_ParseObjectName(interp, path, &objName, BLT_NO_DEFAULT_NS)) {
        return TCL_ERROR;
    }
    if (objName.nsPtr == NULL) {
        /* 
         * If there was no namespace qualifier, try harder to see if the
         * variable is non-local.
         */
        objName.nsPtr = Blt_GetVariableNamespace(interp, objName.name);
    } 
    Tcl_DStringInit(&ds);
    vecObjPtr->varFlags = 0;
    if (objName.nsPtr != NULL) {        /* Global or namespace variable. */
        newPath = Blt_MakeQualifiedName(&objName, &ds);
        vecObjPtr->varFlags |= (TCL_NAMESPACE_ONLY | TCL_GLOBAL_ONLY);
    } else {                            /* Local variable. */
        newPath = (char *)objName.name;
    }

    /*
     * To play it safe, delete the variable first.  This has the benefical
     * side-effect of unmapping the variable from another vector that may
     * be currently associated with it.
     */
    Tcl_UnsetVar2(interp, newPath, (char *)NULL, 0);

    /* 
     * Set the index "end" in the array.  This will create the variable
     * immediately so that we can check its namespace context.
     */
    result = Tcl_SetVar2(interp, newPath, "end", "", TCL_LEAVE_ERR_MSG);
    if (result == NULL) {
        Tcl_DStringFree(&ds);
        return TCL_ERROR;
    }
    /* Create a full-array trace on reads, writes, and unsets. */
    Tcl_TraceVar2(interp, newPath, (char *)NULL, TRACE_ALL, Blt_VecObj_VarTrace,
        vecObjPtr);
    vecObjPtr->arrayName = Blt_AssertStrdup(newPath);
    Tcl_DStringFree(&ds);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_SetSize --
 *
 *      Resizes the vector to the designated new size.
 *
 *      If the new size is the same as the old, simply return.  Otherwise
 *      we're copying the data from one memory location to another.
 *
 *      If the storage changed memory locations, free up the old location
 *      if it was dynamically allocated.
 *
 * Results:
 *      A standard TCL result.  If the reallocation is successful, TCL_OK
 *      is returned, otherwise TCL_ERROR.
 *
 * Side effects:
 *      Memory for the array is reallocated.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_VecObj_SetSize(Tcl_Interp *interp, VectorObject *vecObjPtr, int newSize)
{
    if (newSize == 0) {
        newSize = DEF_ARRAY_SIZE;
    }
    if (newSize == vecObjPtr->size) {
        /* Same size, use the current array. */
        return TCL_OK;
    } 
    if (vecObjPtr->freeProc == TCL_DYNAMIC) {
        double *newArr;

        /* Old memory was dynamically allocated, so use realloc. */
        newArr = Blt_Realloc(vecObjPtr->valueArr, newSize * sizeof(double));
        if (newArr == NULL) {
            if (interp != NULL) {
                Tcl_AppendResult(interp, "can't reallocate ", 
                                 Blt_Ltoa(newSize), " elements for vector \"", 
                                 vecObjPtr->name, "\"", (char *)NULL); 
            }
            return TCL_ERROR;
        }
        vecObjPtr->size = newSize;
        vecObjPtr->valueArr = newArr;
        return TCL_OK;
    }

    {
        double *newArr;

        /* Old memory was created specially (static or special allocator).
         * Replace with dynamically allocated memory (malloc-ed). */

        newArr = Blt_Calloc(newSize, sizeof(double));
        if (newArr == NULL) {
            if (interp != NULL) {
                Tcl_AppendResult(interp, "can't allocate ", 
                                 Blt_Ltoa(newSize), " elements for vector \"", 
                                 vecObjPtr->name, "\"", (char *)NULL); 
            }
            return TCL_ERROR;
        }
        {
            int used, wanted;
            
            /* Copy the contents of the old memory into the new. */
            used = vecObjPtr->length;
            wanted = newSize;
            
            if (used > wanted) {
                used = wanted;
            }
            /* Copy any previous data */
            if (used > 0) {
                memcpy(newArr, vecObjPtr->valueArr, used * sizeof(double));
            }
        }
        
        assert(vecObjPtr->valueArr != NULL);
        
        /* 
         * We're not using the old storage anymore, so free it if it's not
         * TCL_STATIC.  It's static because the user previously reset the
         * vector with a statically allocated array (setting freeProc to
         * TCL_STATIC).
         */
        if (vecObjPtr->freeProc != TCL_STATIC) {
            if (vecObjPtr->freeProc == TCL_DYNAMIC) {
                Blt_Free(vecObjPtr->valueArr);
            } else {
                (*vecObjPtr->freeProc) ((char *)vecObjPtr->valueArr);
            }
        }
        vecObjPtr->freeProc = TCL_DYNAMIC;   /* Set the type of the new
                                         * storage */
        vecObjPtr->valueArr = newArr;
        vecObjPtr->size = newSize;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_SetSize --
 *
 *      Set the length (the number of elements currently in use) of the
 *      vector.  If the new length is greater than the size (total number
 *      of slots), then the vector is grown.
 *
 * Results:
 *      A standard TCL result.  If the reallocation is successful, TCL_OK
 *      is returned, otherwise TCL_ERROR.
 *
 * Side effects:
 *      Memory for the array is possibly reallocated.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_VecObj_SetLength(Tcl_Interp *interp, VectorObject *vecObjPtr, int newLength)
{
    if (vecObjPtr->size < newLength) {
        if (Blt_VecObj_SetSize(interp, vecObjPtr, newLength) != TCL_OK) {
            return TCL_ERROR;
        }
    }
    if (newLength > vecObjPtr->length) {
        double emptyValue;
        long i;

        emptyValue = Blt_NaN();
        for (i = vecObjPtr->length; i < newLength; i++) {
            vecObjPtr->valueArr[i] = emptyValue;
        }
    }
    vecObjPtr->length = newLength;
    vecObjPtr->first = 0;
    vecObjPtr->last = newLength;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_ChangeLength --
 *
 *      Resizes the vector to the new size.
 *
 *      The new size of the vector is computed by doubling the size of the
 *      vector until it fits the number of slots needed (designated by
 *      *length*).
 *
 *      If the new size is the same as the old, simply adjust the length of
 *      the vector.  Otherwise we're copying the data from one memory
 *      location to another. The trailing elements of the vector need to be
 *      reset to zero.
 *
 *      If the storage changed memory locations, free up the old location
 *      if it was dynamically allocated.
 *
 * Results:
 *      A standard TCL result.  If the reallocation is successful, TCL_OK
 *      is returned, otherwise TCL_ERROR.
 *
 * Side effects:
 *      Memory for the array is reallocated.
 *
 *---------------------------------------------------------------------------
 */

int
Blt_VecObj_ChangeLength(Tcl_Interp *interp, VectorObject *vecObjPtr, 
                        int newLength)
{
    double emptyValue;
    long i;

    if (newLength > vecObjPtr->size) {
        int newSize;                    /* Size of array in elements */
    
        /* Compute the new size of the array.  It's a multiple of
         * DEF_ARRAY_SIZE. */
        newSize = DEF_ARRAY_SIZE;
        while (newSize < newLength) {
            newSize += newSize;
        }
        if (newSize != vecObjPtr->size) {
            if (Blt_VecObj_SetSize(interp, vecObjPtr, newSize) != TCL_OK) {
                return TCL_ERROR;
            }
        }
    }
    emptyValue = Blt_NaN();
    for (i = vecObjPtr->length; i < newLength; i++) {
        vecObjPtr->valueArr[i] = emptyValue;
    }
    vecObjPtr->length = newLength;
    vecObjPtr->first = 0;
    vecObjPtr->last = newLength;
    return TCL_OK;
    
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_Reset --
 *
 *      Resets the vector data.  This is called by a client to indicate
 *      that the vector data has changed.  The vector does not need to
 *      point to different memory.  Any clients of the vector will be
 *      notified of the change.
 *
 * Results:
 *      A standard TCL result.  If the new array size is invalid, TCL_ERROR
 *      is returned.  Otherwise TCL_OK is returned and the new vector data
 *      is recorded.
 *
 * Side Effects:
 *      Any client designated callbacks will be posted.  Memory may be
 *      changed for the vector array.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_VecObj_Reset(
    VectorObject *vecObjPtr,
    double *valueArr,                   /* Array containing the elements of
                                         * the vector. If NULL, indicates
                                         * to reset the vector size to the
                                         * default. */
    int length,                         /* # of elements that the vector
                                         * currently holds. */
    int size,                           /* The # of elements that the
                                         * array can hold. */
    Tcl_FreeProc *freeProc)             /* Address of memory deallocation
                                         * routine for the array of values.
                                         * Can also be TCL_STATIC,
                                         * TCL_DYNAMIC, or TCL_VOLATILE. */
{
    if (vecObjPtr->valueArr != valueArr) {   
        /* New array of values resides in different memory than the current
         * vector.  */
        if ((valueArr == NULL) || (size == 0)) {
            /* Empty array. Set up default values */
            valueArr = Blt_Malloc(sizeof(double) * DEF_ARRAY_SIZE);
            size = DEF_ARRAY_SIZE;
            if (valueArr == NULL) {
                Tcl_AppendResult(vecObjPtr->interp, "can't allocate ", 
                        Blt_Ltoa(size), " elements for vector \"", 
                        vecObjPtr->name, "\"", (char *)NULL);
                return TCL_ERROR;
            }
            freeProc = TCL_DYNAMIC;
            length = 0;
        } else if (freeProc == TCL_VOLATILE) {
            double *newArr;

            /* Data is volatile. Make a copy of the value array.  */
            newArr = Blt_Malloc(size * sizeof(double));
            if (newArr == NULL) {
                Tcl_AppendResult(vecObjPtr->interp, "can't allocate ", 
                        Blt_Ltoa(size), " elements for vector \"", 
                        vecObjPtr->name, "\"", (char *)NULL);
                return TCL_ERROR;
            }
            memcpy((char *)newArr, (char *)valueArr, 
                   sizeof(double) * length);
            valueArr = newArr;
            freeProc = TCL_DYNAMIC;
        } 

        if (vecObjPtr->freeProc != TCL_STATIC) {
            /* Old data was dynamically allocated. Free it before attaching
             * new data.  */
            if (vecObjPtr->freeProc == TCL_DYNAMIC) {
                Blt_Free(vecObjPtr->valueArr);
            } else {
                (*freeProc) ((char *)vecObjPtr->valueArr);
            }
        }
        vecObjPtr->freeProc = freeProc;
        vecObjPtr->valueArr = valueArr;
    }
    vecObjPtr->size = size;
    vecObjPtr->length = length;
    if (vecObjPtr->flush) {
        Blt_VecObj_FlushCache(vecObjPtr);
    }
    Blt_VecObj_UpdateClients(vecObjPtr);
    return TCL_OK;
}

VectorObject *
Blt_VecObj_New(VectorCmdInterpData *dataPtr)
{
    VectorObject *vecPtr;

    vecPtr = Blt_AssertCalloc(1, sizeof(VectorObject));
    vecPtr->valueArr = Blt_Malloc(sizeof(double) * DEF_ARRAY_SIZE);
    if (vecPtr->valueArr == NULL) {
        Blt_Free(vecPtr);
        return NULL;
    }
    vecPtr->refCount = 1;
    vecPtr->size = DEF_ARRAY_SIZE;
    vecPtr->freeProc = TCL_DYNAMIC;
    vecPtr->interp = dataPtr->interp;
    vecPtr->chain = Blt_Chain_Create();
    vecPtr->min = vecPtr->max = Blt_NaN();
    vecPtr->notifyFlags = NOTIFY_WHENIDLE;
    vecPtr->dataPtr = dataPtr;
    return vecPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_Free --
 *
 *      Removes the memory and frees resources associated with the vector.
 *
 *      o Removes the trace and the TCL array variable and unsets
 *        the variable.
 *      o Notifies clients of the vector that the vector is being
 *        destroyed.
 *      o Removes any clients that are left after notification.
 *      o Frees the memory (if necessary) allocated for the array.
 *      o Removes the entry from the hash table of vectors.
 *      o Frees the memory allocated for the name.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *
 *---------------------------------------------------------------------------
 */
void
Blt_VecObj_Free(VectorObject *vecObjPtr)
{
    if (vecObjPtr->notifyFlags & NOTIFY_PENDING) {
        vecObjPtr->notifyFlags &= ~NOTIFY_PENDING;
        Tcl_CancelIdleCall(Blt_VecObj_NotifyClients, (ClientData)vecObjPtr);
    }
    if (vecObjPtr->cmdToken != 0) {
        DeleteCommand(vecObjPtr);
    }
    if (vecObjPtr->arrayName != NULL) {
        UnmapVariable(vecObjPtr);
    }
    if (vecObjPtr->hashPtr != NULL) {
        Blt_DeleteHashEntry(&vecObjPtr->dataPtr->vectorTable, 
                            vecObjPtr->hashPtr);
    }
    vecObjPtr->length = 0;

    assert(Blt_Chain_GetLength(vecObjPtr->chain) == 0);
    Blt_Chain_Destroy(vecObjPtr->chain);
    if ((vecObjPtr->valueArr != NULL) && (vecObjPtr->freeProc != TCL_STATIC)) {
        if (vecObjPtr->freeProc == TCL_DYNAMIC) {
            Blt_Free(vecObjPtr->valueArr);
        } else {
            (*vecObjPtr->freeProc) ((char *)vecObjPtr->valueArr);
        }
    }
    Blt_Free(vecObjPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * VectorInstDeleteProc --
 *
 *      Deletes the command associated with the vector.  This is called
 *      only when the command associated with the vector is destroyed.
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static void
VectorInstDeleteProc(ClientData clientData)
{
    VectorObject *vecObjPtr = clientData;

    vecObjPtr->cmdToken = 0;
    Blt_VecObj_Free(vecObjPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VecObj_Create --
 *
 *      Creates a vector structure and the following items:
 *
 *      o TCL command
 *      o TCL array variable and establishes traces on the variable
 *      o Adds a  new entry in the vector hash table
 *
 * Results:
 *      A pointer to the new vector structure.  If an error occurred NULL
 *      is returned and an error message is left in interp->result.
 *
 * Side effects:
 *      A new TCL command and array variable is added to the interpreter.
 *
 * ---------------------------------------------------------------------- 
 */
VectorObject *
Blt_VecObj_Create(
    VectorCmdInterpData *dataPtr,       /* Interpreter-specific data. */
    const char *vecName,                /* Namespace-qualified name of the
                                         * vector */
    const char *cmdName,                /* Name of the TCL command mapped
                                         * to the vector */
    const char *varName,                /* Name of the TCL array mapped to
                                         * the vector */
    int *isNewPtr)
{
    Tcl_DString ds;
    VectorObject *vecObjPtr;
    int isNew;
    Blt_ObjectName objName;
    const char *qualName;
    Blt_HashEntry *hPtr;
    Tcl_Interp *interp = dataPtr->interp;

    isNew = 0;
    vecObjPtr = NULL;

    if (!Blt_ParseObjectName(interp, vecName, &objName, 0)) {
        return NULL;
    }
    Tcl_DStringInit(&ds);
    if ((objName.name[0] == '#') && (strcmp(objName.name, "#auto") == 0)) {
        do {                            /* Generate a unique vector name. */
            char string[200];

            Blt_FmtString(string, 200, "vector%d", dataPtr->nextId);
            dataPtr->nextId++;
            objName.name = string;
            qualName = Blt_MakeQualifiedName(&objName, &ds);
            hPtr = Blt_FindHashEntry(&dataPtr->vectorTable, qualName);
        } while (hPtr != NULL);
    } else {
        const char *p;

        for (p = objName.name; *p != '\0'; p++) {
            if (!VECTOR_CHAR(*p)) {
                Tcl_AppendResult(interp, "bad vector name \"", objName.name,
                "\": must contain only digits, letters, underscore, or period",
                    (char *)NULL);
                goto error;
            }
        }
        qualName = Blt_MakeQualifiedName(&objName, &ds);
        vecObjPtr = Blt_VecObj_ParseElement((Tcl_Interp *)NULL, dataPtr, 
                                            qualName, NULL, NS_SEARCH_CURRENT);
    }
    if (vecObjPtr == NULL) {
        hPtr = Blt_CreateHashEntry(&dataPtr->vectorTable, qualName, &isNew);
        if (!isNew) {
            Tcl_AppendResult(interp, "a vector \"", qualName,
                        "\" already exists", (char *)NULL);
            goto error;
        }
        vecObjPtr = Blt_VecObj_New(dataPtr);
        vecObjPtr->hashPtr = hPtr;
        vecObjPtr->nsPtr = objName.nsPtr;

        vecObjPtr->name = Blt_GetHashKey(&dataPtr->vectorTable, hPtr);
        Blt_SetHashValue(hPtr, vecObjPtr);
    } 
    if (cmdName != NULL) {
        Tcl_CmdInfo cmdInfo;

        if ((cmdName == vecName) ||
            ((cmdName[0] == '#') && (strcmp(cmdName, "#auto")==0))) {
            cmdName = qualName;
        } 
        if (Tcl_GetCommandInfo(interp, (char *)cmdName, &cmdInfo)) {
            if (vecObjPtr != cmdInfo.objClientData) {
                Tcl_AppendResult(interp, "a command \"", cmdName,
                         "\" already exists", (char *)NULL);
                goto error;
            }
            /* We get here only if the old name is the same as the new. */
            goto checkVariable;
        }
    }
    if (vecObjPtr->cmdToken != 0) {
        DeleteCommand(vecObjPtr);            /* Command already exists, delete
                                         * old first */
    }
    if (cmdName != NULL) {
        Tcl_DString ds2;
        
        Tcl_DStringInit(&ds2);
        if (cmdName != qualName) {
            if (!Blt_ParseObjectName(interp, cmdName, &objName, 0)) {
                goto error;
            }
            cmdName = Blt_MakeQualifiedName(&objName, &ds2);
        }
        vecObjPtr->cmdToken = Tcl_CreateObjCommand(interp, (char *)cmdName, 
                Blt_VecObj_InstCmd, vecObjPtr, VectorInstDeleteProc);
        Tcl_DStringFree(&ds2);
    }
  checkVariable:
    if (varName != NULL) {
        if ((varName[0] == '#') && (strcmp(varName, "#auto") == 0)) {
            varName = qualName;
        }
        if (Blt_VecObj_MapVariable(interp, vecObjPtr, varName) != TCL_OK) {
            goto error;
        }
    }

    Tcl_DStringFree(&ds);
    *isNewPtr = isNew;
    return vecObjPtr;

  error:
    Tcl_DStringFree(&ds);
    if (vecObjPtr != NULL) {
        Blt_VecObj_Free(vecObjPtr);
    }
    return NULL;
}

int
Blt_VecObj_Duplicate(VectorObject *destPtr, VectorObject *srcPtr)
{
    int numBytes;
    int length;
    
    if (destPtr == srcPtr) {
        /* Copying the same vector. */
    }
    length = srcPtr->last - srcPtr->first;
    if (Blt_VecObj_ChangeLength(destPtr->interp, destPtr, length) != TCL_OK) {
        return TCL_ERROR;
    }
    numBytes = length * sizeof(double);
    memcpy(destPtr->valueArr, srcPtr->valueArr + srcPtr->first, numBytes);
    destPtr->offset = srcPtr->offset;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * VectorNamesOp --
 *
 *      Reports the names of all the current vectors in the interpreter.
 *
 * Results:
 *      A standard TCL result.  interp->result will contain a list of
 *      all the names of the vector instances.
 *
 *      blt::vector names ?pattern ...?
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
VectorNamesOp(ClientData clientData, Tcl_Interp *interp, int objc,
              Tcl_Obj *const *objv)
{
    VectorCmdInterpData *dataPtr = clientData;
    Tcl_Obj *listObjPtr;
    Blt_HashEntry *hPtr;
    Blt_HashSearch cursor;

    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
    for (hPtr = Blt_FirstHashEntry(&dataPtr->vectorTable, &cursor);
         hPtr != NULL; hPtr = Blt_NextHashEntry(&cursor)) {
        const char *name;
        int i;
        int match;
        
        name = Blt_GetHashKey(&dataPtr->vectorTable, hPtr);
        match = (objc == 2);
        for (i = 2; i < objc; i++) {
            char *pattern;

            pattern = Tcl_GetString(objv[i]);
            if (Tcl_StringMatch(name, pattern)) {
                match = TRUE;
                break;
            }
        }
        if (match) {
            Tcl_Obj *objPtr;

            objPtr = Tcl_NewStringObj(name, -1);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
        }
    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * OldVectorCreate --
 *
 *      Creates a TCL command, and array variable representing an instance
 *      of a vector.
 *
 *      vector a b c 
 *      vector a(1) b(20) c(20)
 *      vector c(-5:14)
 *
 *      blt::vector create ?name? -size 10 -command a -variable b -first 1 \
 *              -last 15
 *      blt::vector create #auto
 *      blt::vector create 

 * Results:
 *      A standard TCL result.
 *
 * Side effects:
 *      See the user documentation.
 *
 *---------------------------------------------------------------------------
 */

/*ARGSUSED*/
static int
OldVectorCreate(ClientData clientData, Tcl_Interp *interp, int argStart,
                int objc, Tcl_Obj *const *objv)
{
    VectorCmdInterpData *dataPtr = clientData;
    VectorObject *vecObjPtr;
    long count, i;
    CreateSwitches switches;

    /*
     * Handle switches to the vector command and collect the vector name
     * arguments into an array.
     */
    vecObjPtr = NULL;
    for (i = argStart; i < objc; i++) {
        char *string;

        string = Tcl_GetString(objv[i]);
        if (string[0] == '-') {
            break;
        }
    }
    count = i - argStart;
    if (count == 0) {
        Tcl_AppendResult(interp, "no vector names supplied", (char *)NULL);
        return TCL_ERROR;
    }
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, createSwitches, objc - i, objv + i, 
        &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    if (count > 1) {
        if (switches.cmdName != NULL) {
            Tcl_AppendResult(interp, 
                "can't specify more than one vector with \"-command\" switch",
                (char *)NULL);
            goto error;
        }
        if (switches.varName != NULL) {
            Tcl_AppendResult(interp,
                "can't specify more than one vector with \"-variable\" switch",
                (char *)NULL);
            goto error;
        }
    }
    for (i = 0; i < count; i++) {
        char *leftParen, *rightParen;
        char *string;
        int isNew;
        int size, first, last;

        size = first = last = 0;
        string = Tcl_GetString(objv[i + argStart]);
        leftParen = strchr(string, '(');
        rightParen = strchr(string, ')');
        if (((leftParen != NULL) && (rightParen == NULL)) ||
            ((leftParen == NULL) && (rightParen != NULL)) ||
            (leftParen > rightParen)) {
            Tcl_AppendResult(interp, "bad vector specification \"", string,
                "\"", (char *)NULL);
            goto error;
        }
        if (leftParen != NULL) {
            int result;
            char *colon;

            *rightParen = '\0';
            colon = strchr(leftParen + 1, ':');
            if (colon != NULL) {

                /* Specification is in the form vecName(first:last) */
                *colon = '\0';
                result = Tcl_GetInt(interp, leftParen+1, &first);
                if ((*(colon + 1) != '\0') && (result == TCL_OK)) {
                    result = Tcl_GetInt(interp, colon + 1, &last);
                    if (first >= last) {
                        Tcl_AppendResult(interp, "bad vector range \"",
                            string, "\"", (char *)NULL);
                        result = TCL_ERROR;
                    }
                    size = last - first;
                }
                *colon = ':';
            } else {
                /* Specification is in the form vecName(size) */
                result = Tcl_GetInt(interp, leftParen + 1, &size);
            }
            *rightParen = ')';
            if (result != TCL_OK) {
                goto error;
            }
        }
        if (leftParen != NULL) {
            *leftParen = '\0';
        }
        /* By default, the TCL command is the name of the vector. */
        vecObjPtr = Blt_VecObj_Create(dataPtr, string,
            (switches.cmdName == NULL) ? string : switches.cmdName,
            (switches.varName == NULL) ? string : switches.varName, &isNew);
        if (leftParen != NULL) {
            *leftParen = '(';
        }
        if (vecObjPtr == NULL) {
            goto error;
        }
        vecObjPtr->freeOnUnset = switches.watchUnset;
        vecObjPtr->flush = switches.flush;
        vecObjPtr->offset = first;
        if (size > 0) {
            if (Blt_VecObj_ChangeLength(interp, vecObjPtr, size) != TCL_OK) {
                goto error;
            }
        }
        if (!isNew) {
            if (vecObjPtr->flush) {
                Blt_VecObj_FlushCache(vecObjPtr);
            }
            Blt_VecObj_UpdateClients(vecObjPtr);
        }
    }
    Blt_FreeSwitches(createSwitches, (char *)&switches, 0);
    if (vecObjPtr != NULL) {
        /* Return the name of the last vector created  */
        Tcl_SetStringObj(Tcl_GetObjResult(interp), vecObjPtr->name, -1);
    }
    return TCL_OK;
  error:
    Blt_FreeSwitches(createSwitches, (char *)&switches, 0);
    return TCL_ERROR;
}

/*
 *---------------------------------------------------------------------------
 *
 * GenerateName --
 *
 *      Generates an unique vector command name.  
 *      
 * Results:
 *      Returns the unique name.  The string itself is stored in the
 *      dynamic string passed into the routine.
 *
 *---------------------------------------------------------------------------
 */
static const char *
GenerateName(VectorCmdInterpData *dataPtr, Tcl_Interp *interp, 
        const char *prefix, const char *suffix, Tcl_DString *resultPtr)
{

    const char *vecName;

    /* 
     * Parse the command and put back so that it's in a consistent
     * format.  
     *
     *  t1         <current namespace>::t1
     *  n1::t1     <current namespace>::n1::t1
     *  ::t1       ::t1
     *  ::n1::t1   ::n1::t1
     */
    vecName = NULL;                     /* Suppress compiler warning. */
    while (dataPtr->nextId < INT_MAX) {
        Blt_ObjectName objName;
        Tcl_DString ds;
        char string[200];

        Tcl_DStringInit(&ds);
        Tcl_DStringAppend(&ds, prefix, -1);
        Blt_FmtString(string, 200, "vector%d", dataPtr->nextId);
        dataPtr->nextId++;
        Tcl_DStringAppend(&ds, string, -1);
        Tcl_DStringAppend(&ds, suffix, -1);
        if (!Blt_ParseObjectName(interp, Tcl_DStringValue(&ds), &objName, 0)) {
            Tcl_DStringFree(&ds);
            return NULL;
        }
        vecName = Blt_MakeQualifiedName(&objName, resultPtr);
        Tcl_DStringFree(&ds);

        if (Blt_VectorExists2(interp, vecName)) {
            continue;                   /* A vector by this name already
                                         * exists. */
        }
        if (Blt_CommandExists(interp, vecName)) {
            continue;                   /* A command by this name already
                                         * exists. */
        }
        break;
    }
    return vecName;
}

/*
 *---------------------------------------------------------------------------
 *
 * VectorCreateOp --
 *
 *      Creates a TCL command, and array variable representing an instance
 *      of a vector.
 *
 *      blt::vector create ?name? -size 10 -command a -variable b -first 1 \
 *              -last 15
 *      blt::vector create #auto
 *      blt::vector create 
 *
 * Results:
 *      A standard TCL result.
 *
 * Side effects:
 *      See the user documentation.
 *
 *---------------------------------------------------------------------------
 */

/*ARGSUSED*/
static int
VectorCreateOp(ClientData clientData, Tcl_Interp *interp, int objc,
               Tcl_Obj *const *objv)
{
    VectorCmdInterpData *dataPtr = clientData;
    VectorObject *vecObjPtr;
    CreateSwitches switches;
    int isNew;
    Blt_HashEntry *hPtr;
    const char *qualName, *varName;
    Blt_ObjectName objName;
    const char *name;
    Tcl_DString ds, ds2, ds3;
        
    Tcl_DStringInit(&ds2);
    Tcl_DStringInit(&ds3);
    name = NULL;
    varName = NULL;
    if (objc >= 3) {
        const char *string;

        string = Tcl_GetString(objv[2]);
        if (string[0] != '-') {
            name = string;
            objc--, objv++;
        }
    }
    Tcl_DStringInit(&ds);
    if (name == NULL) {
        qualName = GenerateName(dataPtr, interp, "", "", &ds);
    } else {
        char *p;

        p = strstr(name, "#auto");
        if (p != NULL) {
            *p = '\0';
            qualName = GenerateName(dataPtr, interp, name, p + 5, &ds);
            *p = '#';
        } else {
            Blt_ObjectName objName;

            /* 
             * Parse the command and put back so that it's in a consistent
             * format.
             *
             *  t1         <current namespace>::t1
             *  n1::t1     <current namespace>::n1::t1
             *  ::t1       ::t1
             *  ::n1::t1   ::n1::t1
             */
            if (!Blt_ParseObjectName(interp, name, &objName, 0)) {
                return TCL_ERROR;
            }
            qualName = Blt_MakeQualifiedName(&objName, &ds);
            if (Blt_VectorExists2(interp, qualName)) {
                Tcl_AppendResult(interp, "a vector \"", qualName, 
                        "\" already exists", (char *)NULL);
                Tcl_DStringFree(&ds);
                return TCL_ERROR;
            }
            /* 
             * Check if the command already exists. 
             */
            if (Blt_CommandExists(interp, qualName)) {
                Tcl_AppendResult(interp, "a command \"", qualName,
                                 "\" already exists", (char *)NULL);
                Tcl_DStringFree(&ds);
                return TCL_ERROR;
            }
        } 
    } 
    if (qualName == NULL) {
        goto error;
    }
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, createSwitches, objc - 2, objv + 2, 
        &switches, BLT_SWITCH_DEFAULTS) < 0) {
        Tcl_DStringFree(&ds);
        return TCL_ERROR;
    }
    varName = NULL;
    if (switches.varName == NULL) {
        varName = qualName;
    } else if (switches.varName[0] != '\0') {
        if (!Blt_ParseObjectName(interp, switches.varName, &objName, 0)) {
            goto error;
        }
        varName = Blt_MakeQualifiedName(&objName, &ds3);
    } 
    /* By default, the TCL command is the name of the vector. */
    hPtr = Blt_CreateHashEntry(&dataPtr->vectorTable, qualName, &isNew);
    assert(isNew);
    vecObjPtr = Blt_VecObj_New(dataPtr);
    vecObjPtr->hashPtr = hPtr;
    vecObjPtr->nsPtr = objName.nsPtr;

    vecObjPtr->name = Blt_GetHashKey(&dataPtr->vectorTable, hPtr);
    Blt_SetHashValue(hPtr, vecObjPtr);

    vecObjPtr->cmdToken = Tcl_CreateObjCommand(interp, (char *)qualName, 
        Blt_VecObj_InstCmd, vecObjPtr, VectorInstDeleteProc);

    if (varName != NULL) {
        if (Blt_VecObj_MapVariable(interp, vecObjPtr, varName) != TCL_OK) {
            goto error;
        }
    }
    vecObjPtr->freeOnUnset = switches.watchUnset;
    vecObjPtr->flush = switches.flush;
    vecObjPtr->offset = 0;
    if (switches.size > 0) {
        if (Blt_VecObj_ChangeLength(interp, vecObjPtr, switches.size) != TCL_OK) {
            goto error;
        }
    }
    Tcl_DStringFree(&ds2);
    Tcl_DStringFree(&ds3);
    Tcl_DStringFree(&ds);
    Blt_FreeSwitches(createSwitches, (char *)&switches, 0);
    /* Return the name of the last vector created  */
    Tcl_SetStringObj(Tcl_GetObjResult(interp), vecObjPtr->name, -1);
    return TCL_OK;
 error:
    Blt_FreeSwitches(createSwitches, (char *)&switches, 0);
    Tcl_DStringFree(&ds2);
    Tcl_DStringFree(&ds3);
    Tcl_DStringFree(&ds);
    return TCL_ERROR;
}

/*
 *---------------------------------------------------------------------------
 *
 * VectorDestroyOp --
 *
 *      Destroys the vector and its related TCL command and array variable
 *      (if they exist).
 *
 * Results:
 *      A standard TCL result.
 *
 * Side effects:
 *      Deletes the vector.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
VectorDestroyOp(ClientData clientData, Tcl_Interp *interp, int objc,
                Tcl_Obj *const *objv)
{
    VectorCmdInterpData *dataPtr = clientData;
    int i;

    for (i = 2; i < objc; i++) {
        VectorObject *vecObjPtr;

        if (Blt_VecObj_Find(interp, dataPtr, Tcl_GetString(objv[i]), &vecObjPtr)
            != TCL_OK) {
            return TCL_ERROR;
        }
        vecObjPtr->refCount--;
        if (vecObjPtr->refCount <= 0) {
            Blt_VecObj_Free(vecObjPtr);
        } else {
            if (vecObjPtr->cmdToken != 0) {
                DeleteCommand(vecObjPtr);
            }
            if (vecObjPtr->arrayName != NULL) {
                UnmapVariable(vecObjPtr);
            }
            if (vecObjPtr->hashPtr != NULL) {
                Blt_DeleteHashEntry(&vecObjPtr->dataPtr->vectorTable, 
                                    vecObjPtr->hashPtr);
                vecObjPtr->hashPtr = NULL;
            }
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * VectorExprOp --
 *
 *      Computes the result of the expression which may be either a scalar
 *      (single value) or vector (list of values).
 *
 * Results:
 *      A standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
VectorExprOp(ClientData clientData, Tcl_Interp *interp, int objc,
             Tcl_Obj *const *objv)
{
    return Blt_ExprVector(interp, Tcl_GetString(objv[2]), (Blt_Vector *)NULL);
}

static Blt_OpSpec vectorCmdOps[] =
{
    {"create",  1, VectorCreateOp,  2, 0, "?vecName? ?switches...?",},
    {"destroy", 1, VectorDestroyOp, 2, 0, "?vecName...?",},
    {"expr",    1, VectorExprOp,    3, 3, "expression",},
    {"names",   1, VectorNamesOp,   2, 3, "?pattern...?",},
};

static int numCmdOps = sizeof(vectorCmdOps) / sizeof(Blt_OpSpec);

/*ARGSUSED*/
static int
VectorCmd(ClientData clientData, Tcl_Interp *interp, int objc,
          Tcl_Obj *const *objv)
{
    VectorCmdProc *proc;

    /* Try to replicate the old vector command's behavior: */
    if (objc > 1) {
        char *string;
        char c;
        int i;
        Blt_OpSpec *specPtr;

        string = Tcl_GetString(objv[1]);
        c = string[0];
        for (specPtr = vectorCmdOps, i = 0; i < numCmdOps; i++, specPtr++) {
            if ((c == specPtr->name[0]) &&
                (strcmp(string, specPtr->name) == 0)) {
                goto doOp;
            }
        }
        /*
         * The first argument is not an operation, so assume that its
         * actually the name of a vector to be created.
         */
        return OldVectorCreate(clientData, interp, 1, objc, objv);
    }
  doOp:
    /* Do the usual vector operation lookup now. */
    proc = Blt_GetOpFromObj(interp, numCmdOps, vectorCmdOps, BLT_OP_ARG1, 
        objc, objv,0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}

/*
 *---------------------------------------------------------------------------
 *
 * VectorInterpDeleteProc --
 *
 *      This is called when the interpreter hosting the "vector" command is
 *      deleted.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Destroys the math and index hash tables.  In addition removes the
 *      hash table managing all vector names.
 *
 *---------------------------------------------------------------------------
 */
/* ARGSUSED */
static void
VectorInterpDeleteProc(ClientData clientData, Tcl_Interp *interp)
{
    VectorCmdInterpData *dataPtr = clientData;
    Blt_HashEntry *hPtr;
    Blt_HashSearch cursor;
    
    for (hPtr = Blt_FirstHashEntry(&dataPtr->vectorTable, &cursor);
         hPtr != NULL; hPtr = Blt_NextHashEntry(&cursor)) {
        VectorObject *vecObjPtr;

        vecObjPtr = Blt_GetHashValue(hPtr);
        vecObjPtr->hashPtr = NULL;
        Blt_VecObj_Free(vecObjPtr);
    }
    Blt_DeleteHashTable(&dataPtr->vectorTable);

    /* If any user-defined math functions were installed, remove them.  */
    Blt_VecObj_UninstallMathFunctions(&dataPtr->mathProcTable);
    Blt_DeleteHashTable(&dataPtr->mathProcTable);

    Blt_DeleteHashTable(&dataPtr->indexProcTable);
    Tcl_DeleteAssocData(interp, VECTOR_THREAD_KEY);
    Blt_Free(dataPtr);
}

VectorCmdInterpData *
Blt_VecObj_GetInterpData(Tcl_Interp *interp)
{
    VectorCmdInterpData *dataPtr;
    Tcl_InterpDeleteProc *proc;

    dataPtr = (VectorCmdInterpData *)
        Tcl_GetAssocData(interp, VECTOR_THREAD_KEY, &proc);
    if (dataPtr == NULL) {
        dataPtr = Blt_AssertMalloc(sizeof(VectorCmdInterpData));
        dataPtr->interp = interp;
        dataPtr->nextId = 1;
        Tcl_SetAssocData(interp, VECTOR_THREAD_KEY, VectorInterpDeleteProc,
                 dataPtr);
        Blt_InitHashTable(&dataPtr->vectorTable, BLT_STRING_KEYS);
        Blt_InitHashTable(&dataPtr->mathProcTable, BLT_STRING_KEYS);
        Blt_InitHashTable(&dataPtr->indexProcTable, BLT_STRING_KEYS);
        Blt_VecObj_InstallMathFunctions(&dataPtr->mathProcTable);
        Blt_VecObj_InstallSpecialIndices(&dataPtr->indexProcTable);
#ifdef HAVE_SRAND48
        srand48(time((time_t *) NULL));
#endif
    }
    return dataPtr;
}

/*ARGSUSED*/
static int
SimplifyCmd(ClientData clientData, Tcl_Interp *interp, int objc,
            Tcl_Obj *const *objv)
{
    Blt_Vector *x, *y, *xr, *yr;
    Point2d *origPts;
    double *xArr, *yArr;
    long *indices;
    long i;
    long numOrigPts, numReducedPts;
    double tolerance = 0.05;
    
    if ((objc < 5) || (objc > 6)) {
        Tcl_AppendResult(interp, "wrong # arguments: should be \"",
                Tcl_GetString(objv[0]), " simplify x y rx ry ?tol?",
                (char *)NULL);
        return TCL_ERROR;
    }
    if ((Blt_GetVectorFromObj(interp, objv[1], &x) != TCL_OK) ||
        (Blt_GetVectorFromObj(interp, objv[2], &y) != TCL_OK) ||
        (Blt_GetVectorFromObj(interp, objv[3], &xr) != TCL_OK) ||
        (Blt_GetVectorFromObj(interp, objv[4], &yr) != TCL_OK)) {
        return TCL_ERROR;
    }
    if ((objc > 5) &&
        (Tcl_GetDoubleFromObj(interp, objv[5], &tolerance) != TCL_OK)) {
        return TCL_ERROR;
    }
    numOrigPts = Blt_VecLength(x);
    if (numOrigPts < 3) {
        Tcl_AppendResult(interp, "length of vector \"", Tcl_GetString(objv[2]),
                         "\" is < 3", (char *)NULL);
        return TCL_ERROR;
    }
    if (numOrigPts != Blt_VecLength(y)) {
        Tcl_AppendResult(interp, "vectors \"", Tcl_GetString(objv[1]), 
                         "\" and \"", Tcl_GetString(objv[2]),
                         " have different lengths", (char *)NULL);
        return TCL_ERROR;
    }

    origPts = Blt_Malloc(sizeof(Point2d) * numOrigPts);
    if (origPts == NULL) {
        Tcl_AppendResult(interp, "can't allocate \"", Blt_Ltoa(numOrigPts), 
                "\" points", (char *)NULL);
        return TCL_ERROR;
    }
    xArr = Blt_VecData(x);
    yArr = Blt_VecData(y);
    for (i = 0; i < numOrigPts; i++) {
        origPts[i].x = xArr[i];
        origPts[i].y = yArr[i];
    }
    indices = Blt_Malloc(sizeof(long) * numOrigPts);
    if (indices == NULL) {
        Tcl_AppendResult(interp, "can't allocate \"", Blt_Ltoa(numOrigPts), 
                "\" indices for simplication array", (char *)NULL);
        Blt_Free(origPts);
        return TCL_ERROR;
    }
    numReducedPts = Blt_SimplifyLine(origPts, 0, numOrigPts - 1, tolerance,
                indices);
    if ((Blt_ResizeVector(xr, numReducedPts) != TCL_OK) ||
        (Blt_ResizeVector(yr, numReducedPts) != TCL_OK)) {
        return TCL_ERROR;
    }
    xArr = Blt_VecData(xr);
    yArr = Blt_VecData(yr);
    for (i = 0; i < numReducedPts; i++) {
        xArr[i] = origPts[indices[i]].x;
        yArr[i] = origPts[indices[i]].y;
    }
    Blt_Free(origPts);
    Blt_Free(indices);

    /* Finally update the vector. The size of the vector hasn't changed,
     * just the data. Reset the vector using TCL_STATIC to indicate
     * this. */
    if (Blt_ResetVector(xr, Blt_VecData(xr), Blt_VecLength(xr),
            Blt_VecSize(xr), TCL_STATIC) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Blt_ResetVector(yr, Blt_VecData(yr), Blt_VecLength(yr),
            Blt_VecSize(yr), TCL_STATIC) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}


/*
 *---------------------------------------------------------------------------
 *
 * Blt_VectorCmdInitProc --
 *
 *      This procedure is invoked to initialize the "vector" command.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Creates the new command and adds a new entry into a global Tcl
 *      associative array.
 *
 *---------------------------------------------------------------------------
 */

int
Blt_VectorCmdInitProc(Tcl_Interp *interp)
{
    static Blt_CmdSpec cmdSpecs[2] = {
        {"vector", VectorCmd },
        {"simplify", SimplifyCmd }
    };
    cmdSpecs[0].clientData = Blt_VecObj_GetInterpData(interp);
    return Blt_InitCmds(interp, "::blt", cmdSpecs, 2);
}



/* C Application interface to vectors */

/*
 *---------------------------------------------------------------------------
 *
 * Blt_CreateVector --
 *
 *      Creates a new vector by the name and size.
 *
 * Results:
 *      A standard TCL result.  If the new array size is invalid or a
 *      vector already exists by that name, TCL_ERROR is returned.
 *      Otherwise TCL_OK is returned and the new vector is created.
 *
 * Side Effects:
 *      Memory will be allocated for the new vector.  A new TCL command and
 *      TCL array variable will be created.
 *
 *---------------------------------------------------------------------------
 */

/*LINTLIBRARY*/
int
Blt_CreateVector2(
    Tcl_Interp *interp,
    const char *vecName, const char *cmdName, const char *varName,
    int initialSize,
    Blt_Vector **vecPtrPtr)
{
    VectorCmdInterpData *dataPtr;       /* Interpreter-specific data. */
    VectorObject *vecObjPtr;
    int isNew;
    const char *nameCopy;

    dataPtr = Blt_VecObj_GetInterpData(interp);

    nameCopy = Blt_AssertStrdup(vecName);
    vecObjPtr = Blt_VecObj_Create(dataPtr, nameCopy, cmdName, varName, &isNew);
    Blt_Free(nameCopy);

    if (vecObjPtr == NULL) {
        return TCL_ERROR;
    }
    if (initialSize > 0) {
        if (Blt_VecObj_ChangeLength(interp, vecObjPtr, initialSize) != TCL_OK) {
            return TCL_ERROR;
        }
    }
    if (vecPtrPtr != NULL) {
        *vecPtrPtr = (Blt_Vector *) vecObjPtr;
    }
    return TCL_OK;
}

int
Blt_CreateVector(Tcl_Interp *interp, const char *name, int size,
                 Blt_Vector **vecPtrPtr)
{
    return Blt_CreateVector2(interp, name, name, name, size, vecPtrPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_DeleteVector --
 *
 *      Deletes the vector of the given name.  All clients with designated
 *      callback routines will be notified.
 *
 * Results:
 *      A standard TCL result.  If no vector exists by that name, TCL_ERROR
 *      is returned.  Otherwise TCL_OK is returned and vector is deleted.
 *
 * Side Effects:
 *      Memory will be released for the new vector.  Both the TCL command
 *      and array variable will be deleted.  All clients which set call
 *      back procedures will be notified.
 *
 *---------------------------------------------------------------------------
 */
/*LINTLIBRARY*/
int
Blt_DeleteVector(Blt_Vector *vecPtr)
{
    VectorObject *vecObjPtr = (VectorObject *)vecPtr;

    Blt_VecObj_Free(vecObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_DeleteVectorByName --
 *
 *      Deletes the vector of the given name.  All clients with designated
 *      callback routines will be notified.
 *
 * Results:
 *      A standard TCL result.  If no vector exists by that name, TCL_ERROR
 *      is returned.  Otherwise TCL_OK is returned and vector is deleted.
 *
 * Side Effects:
 *      Memory will be released for the new vector.  Both the TCL command
 *      and array variable will be deleted.  All clients which set call
 *      back procedures will be notified.
 *
 *---------------------------------------------------------------------------
 */
/*LINTLIBRARY*/
int
Blt_DeleteVectorByName(Tcl_Interp *interp, const char *name)
{
    VectorCmdInterpData *dataPtr;       /* Interpreter-specific data. */
    VectorObject *vecObjPtr;
    const char *nameCopy;
    int result;

    /*
     * If the vector name was passed via a read-only string (e.g. "x"), the
     * Blt_VecObj_ParseElement routine will segfault when it tries to write
     * into the string.  Therefore make a writable copy and free it when
     * we're done.
     */
    nameCopy = Blt_AssertStrdup(name);
    dataPtr = Blt_VecObj_GetInterpData(interp);
    result = Blt_VecObj_Find(interp, dataPtr, nameCopy, &vecObjPtr);
    Blt_Free(nameCopy);

    if (result != TCL_OK) {
        return TCL_ERROR;
    }
    Blt_VecObj_Free(vecObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VectorExists2 --
 *
 *      Returns whether the vector associated with the client token still
 *      exists.
 *
 * Results:
 *      Returns 1 is the vector still exists, 0 otherwise.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_VectorExists2(Tcl_Interp *interp, const char *vecName)
{
    VectorCmdInterpData *dataPtr;       /* Interpreter-specific data. */

    dataPtr = Blt_VecObj_GetInterpData(interp);
    if (GetVectorObject(dataPtr, vecName, NS_SEARCH_BOTH) != NULL) {
        return TRUE;
    }
    return FALSE;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VectorExists --
 *
 *      Returns whether the vector associated with the client token still
 *      exists.
 *
 * Results:
 *      Returns 1 is the vector still exists, 0 otherwise.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_VectorExists(Tcl_Interp *interp, const char *vecName)
{
    const char *nameCopy;
    int result;

    /*
     * If the vector name was passed via a read-only string (e.g. "x"), the
     * Blt_VectorParseName routine will segfault when it tries to write
     * into the string.  Therefore make a writable copy and free it when
     * we're done.
     */
    nameCopy = Blt_AssertStrdup(vecName);
    result = Blt_VectorExists2(interp, nameCopy);
    Blt_Free(nameCopy);
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_GetVector --
 *
 *      Returns a pointer to the vector associated with the given name.
 *
 * Results:
 *      A standard TCL result.  If there is no vector "name", TCL_ERROR is
 *      returned.  Otherwise TCL_OK is returned and vecPtrPtr will point to
 *      the vector.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_GetVector(Tcl_Interp *interp, const char *name, Blt_Vector **vecPtrPtr)
{
    VectorCmdInterpData *dataPtr;       /* Interpreter-specific data. */
    VectorObject *vecObjPtr;
    const char *nameCopy;
    int result;

    dataPtr = Blt_VecObj_GetInterpData(interp);
    /*
     * If the vector name was passed via a read-only string (e.g. "x"), the
     * Blt_VectorParseName routine will segfault when it tries to write
     * into the string.  Therefore make a writable copy and free it when
     * we're done.
     */
    nameCopy = Blt_AssertStrdup(name);
    result = Blt_VecObj_Find(interp, dataPtr, nameCopy, &vecObjPtr);
    Blt_Free(nameCopy);
    if (result != TCL_OK) {
        return TCL_ERROR;
    }
    Blt_VecObj_UpdateRange(vecObjPtr);
    *vecPtrPtr = (Blt_Vector *) vecObjPtr;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_GetVectorFromObj --
 *
 *      Returns a pointer to the vector associated with the given name.
 *
 * Results:
 *      A standard TCL result.  If there is no vector "name", TCL_ERROR is
 *      returned.  Otherwise TCL_OK is returned and vecPtrPtr will point to
 *      the vector.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_GetVectorFromObj(Tcl_Interp *interp, Tcl_Obj *objPtr,
                     Blt_Vector **vecPtrPtr)
{
    VectorCmdInterpData *dataPtr;       /* Interpreter-specific data. */
    VectorObject *vecObjPtr;

    dataPtr = Blt_VecObj_GetInterpData(interp);
    if (Blt_VecObj_Find(interp, dataPtr, Tcl_GetString(objPtr), &vecObjPtr) 
        != TCL_OK) {
        return TCL_ERROR;
    }
    Blt_VecObj_UpdateRange(vecObjPtr);
    *vecPtrPtr = (Blt_Vector *) vecObjPtr;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_ResetVector --
 *
 *      Resets the vector data.  This is called by a client to indicate
 *      that the vector data has changed.  The vector does not need to
 *      point to different memory.  Any clients of the vector will be
 *      notified of the change.
 *
 * Results:
 *      A standard TCL result.  If the new array size is invalid, TCL_ERROR
 *      is returned.  Otherwise TCL_OK is returned and the new vector data
 *      is recorded.
 *
 * Side Effects:
 *      Any client designated callbacks will be posted.  Memory may be
 *      changed for the vector array.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_ResetVector(
    Blt_Vector *vecPtr,
    double *valueArr,                   /* Array containing the elements of
                                         * the vector. If NULL, indicates
                                         * to reset the vector.*/
    int length,                         /* The number of elements that the
                                         * vector currently holds. */
    int size,                           /* The maximum number of elements
                                         * that the array can hold. */
    Tcl_FreeProc *freeProc)             /* Address of memory deallocation
                                         * routine for the array of values.
                                         * Can also be TCL_STATIC,
                                         * TCL_DYNAMIC, or TCL_VOLATILE. */
{
    VectorObject *vecObjPtr = (VectorObject *)vecPtr;

    return Blt_VecObj_Reset(vecObjPtr, valueArr, length, size, freeProc);
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_ResizeVector --
 *
 *      Changes the size of the vector.  All clients with designated
 *      callback routines will be notified of the size change.
 *
 * Results:
 *      A standard TCL result.  If no vector exists by that name, TCL_ERROR
 *      is returned.  Otherwise TCL_OK is returned and vector is resized.
 *
 * Side Effects:
 *      Memory may be reallocated for the new vector size.  All clients
 *      which set call back procedures will be notified.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_ResizeVector(Blt_Vector *vecPtr, int length)
{
    VectorObject *vecObjPtr = (VectorObject *)vecPtr;

    if (Blt_VecObj_ChangeLength((Tcl_Interp *)NULL, vecObjPtr, length) 
        != TCL_OK) {
        Tcl_AppendResult(vecObjPtr->interp, "can't resize vector \"", 
                         vecObjPtr->name, "\"", (char *)NULL);
        return TCL_ERROR;
    }
    if (vecObjPtr->flush) {
        Blt_VecObj_FlushCache(vecObjPtr);
    }
    Blt_VecObj_UpdateClients(vecObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_GetVectorToken --
 *
 *      Creates an identifier token for an existing vector.  The identifier
 *      is used by the client routines to get call backs when (and if) the
 *      vector changes.
 *
 * Results:
 *      A standard TCL result.  If "vecName" is not associated with a
 *      vector, TCL_ERROR is returned and interp->result is filled with an
 *      error message.
 *
 *---------------------------------------------------------------------------
 */
Blt_VectorToken
Blt_GetVectorToken(Tcl_Interp *interp, const char *name)
{
    VectorCmdInterpData *dataPtr;       /* Interpreter-specific data. */
    VectorObject *vecObjPtr;
    VectorClient *clientPtr;
    int result;
    const char *nameCopy;

    dataPtr = Blt_VecObj_GetInterpData(interp);
    /*
     * If the vector name was passed via a read-only string (e.g. "x"), the
     * Blt_VectorParseName routine will segfault when it tries to write
     * into the string.  Therefore make a writable copy and free it when
     * we're done.
     */
    nameCopy = Blt_AssertStrdup(name);
    result = Blt_VecObj_Find(interp, dataPtr, nameCopy, &vecObjPtr);
    Blt_Free(nameCopy);

    if (result != TCL_OK) {
        return (Blt_VectorToken) 0;
    }
    /* Allocate a new client structure */
    clientPtr = Blt_AssertCalloc(1, sizeof(VectorClient));
    clientPtr->magic = VECTOR_MAGIC;

    /* Add the new client to the server's list of clients */
    clientPtr->link = Blt_Chain_Append(vecObjPtr->chain, clientPtr);
    clientPtr->serverPtr = vecObjPtr;
    vecObjPtr->refCount++;
    return (Blt_VectorToken)clientPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_SetVectorChangedProc --
 *
 *      Sets the routine to be called back when the vector is changed or
 *      deleted.  *clientData* will be provided as an argument. If *proc*
 *      is NULL, no callback will be made.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      The designated routine will be called when the vector is changed
 *      or deleted.
 *
 *---------------------------------------------------------------------------
 */
void
Blt_SetVectorChangedProc(
    Blt_VectorToken token,              /* Client token identifying the
                                         * vector */
    Blt_VectorChangedProc *proc,        /* Address of routine to call when
                                         * the contents of the vector
                                         * change. If NULL, no routine will
                                         * be called */
    ClientData clientData)              /* One word of information to pass
                                         * along when the above routine is
                                         * called */
{
    VectorClient *clientPtr = (VectorClient *)token;

    if (clientPtr->magic != VECTOR_MAGIC) {
        return;                 /* Not a valid token */
    }
    clientPtr->clientData = clientData;
    clientPtr->proc = proc;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_FreeVectorToken --
 *
 *      Releases the token for an existing vector.  This indicates that the
 *      client is no longer interested the vector.  Any previously
 *      specified callback routine will no longer be invoked when (and if)
 *      the vector changes.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      Any previously specified callback routine will no longer be invoked
 *      when (and if) the vector changes.
 *
 *---------------------------------------------------------------------------
 */
void
Blt_FreeVectorToken(Blt_VectorToken token)
{
    VectorClient *clientPtr = (VectorClient *)token;
    VectorObject *vecObjPtr;

    if (clientPtr->magic != VECTOR_MAGIC) {
        return;                         /* Not a valid token */
    }
    vecObjPtr = clientPtr->serverPtr;
    if (vecObjPtr != NULL) {
        /* Remove the client from the server's list */
        Blt_Chain_DeleteLink(vecObjPtr->chain, clientPtr->link);
    }
    vecObjPtr->refCount--;
    if (vecObjPtr->refCount <= 0) {
        Blt_VecObj_Free(vecObjPtr);
    } else {
        Blt_Free(clientPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_NameOfVectorFromToken --
 *
 *      Returns the name of the vector (and array variable).
 *
 * Results:
 *      The name of the array variable is returned.
 *
 *---------------------------------------------------------------------------
 */
const char *
Blt_NameOfVectorFromToken(Blt_VectorToken token) 
{
    VectorClient *clientPtr = (VectorClient *)token;

    if ((clientPtr->magic != VECTOR_MAGIC) || (clientPtr->serverPtr == NULL)) {
        return NULL;
    }
    return clientPtr->serverPtr->name;
}

const char *
Blt_NameOfVector(Blt_Vector *vecPtr)    /* Vector to query. */
{
    VectorObject *vecObjPtr = (VectorObject *)vecPtr;
    return vecObjPtr->name;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_VectorNotifyPending --
 *
 *      Returns the name of the vector (and array variable).
 *
 * Results:
 *      The name of the array variable is returned.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_VectorNotifyPending(Blt_VectorToken token)
{
    VectorClient *clientPtr = (VectorClient *)token;

    if ((clientPtr == NULL) || (clientPtr->magic != VECTOR_MAGIC) || 
        (clientPtr->serverPtr == NULL)) {
        return 0;
    }
    return (clientPtr->serverPtr->notifyFlags & NOTIFY_PENDING);
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_GetVectorFromToken --
 *
 *      Returns a pointer to the vector associated with the client token.
 *
 * Results:
 *      A standard TCL result.  If the client token is not associated with
 *      a vector any longer, TCL_ERROR is returned. Otherwise, TCL_OK is
 *      returned and vecPtrPtr will point to vector.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_GetVectorFromToken(
    Tcl_Interp *interp,
    Blt_VectorToken token,              /* Client token identifying the
                                         * vector */
    Blt_Vector **vecPtrPtr)
{
    VectorClient *clientPtr = (VectorClient *)token;

    if (clientPtr->magic != VECTOR_MAGIC) {
        Tcl_AppendResult(interp, "bad vector token", (char *)NULL);
        return TCL_ERROR;
    }
    if (clientPtr->serverPtr == NULL) {
        Tcl_AppendResult(interp, "vector no longer exists", (char *)NULL);
        return TCL_ERROR;
    }
    Blt_VecObj_UpdateRange(clientPtr->serverPtr);
    *vecPtrPtr = (Blt_Vector *) clientPtr->serverPtr;
    return TCL_OK;
}

/*LINTLIBRARY*/
void
Blt_InstallIndexProc(Tcl_Interp *interp, const char *string, 
                     Blt_VectorIndexProc *procPtr) 
{
    VectorCmdInterpData *dataPtr;       /* Interpreter-specific data. */
    Blt_HashEntry *hPtr;
    int isNew;

    dataPtr = Blt_VecObj_GetInterpData(interp);
    hPtr = Blt_CreateHashEntry(&dataPtr->indexProcTable, string, &isNew);
    if (procPtr == NULL) {
        Blt_DeleteHashEntry(&dataPtr->indexProcTable, hPtr);
    } else {
        Blt_SetHashValue(hPtr, procPtr);
    }
}
