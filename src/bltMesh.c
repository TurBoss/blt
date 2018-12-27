/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * bltMesh.c --
 *
 * This module implements 2D meshes.
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

#define BUILD_BLT_TCL_PROCS 1
#include "bltInt.h"

#ifdef HAVE_STDLIB_H
#  include <stdlib.h>
#endif /* HAVE_STDLIB_H */

#ifdef HAVE_STRING_H
#  include <string.h>
#endif /* HAVE_STRING_H */

#include "bltAlloc.h"
#include "bltMath.h"
#include "bltChain.h"
#include "bltGeomUtil.h"
#include "bltHash.h"
#include "bltVector.h"
#include "bltNsUtil.h"
#include "bltDataTable.h"
#include "bltMesh.h"
#include "bltSwitch.h"
#include "bltOp.h"
#include "bltInitCmd.h"

#define MESH_THREAD_KEY "BLT Mesh Command Interface"

#define DELETED                 (1<<1) /* 0x0002 */
#define CONFIG_PENDING          (1<<2) /* 0x0004 */

/*
 * MeshCmdInterpData --
 *
 *      Structure containing global data, used on a interpreter by
 *      interpreter basis.
 *
 *      This structure holds the hash table of instances of datatable
 *      commands associated with a particular interpreter.
 */
struct _MeshCmdInterpData {
    Blt_HashTable meshTable;            /* Tracks tables in use. */
    Tcl_Interp *interp;
    int nextMeshId;
};

typedef struct _DataSource DataSource;
typedef struct _MeshClass MeshClass;
typedef struct _MeshCmdInterpData MeshCmdInterpData;

typedef struct _DataSourceResult DataSourceResult;

typedef int (DataSourceGetProc)(Tcl_Interp *interp, DataSource *srcPtr, 
        DataSourceResult *resultPtr);
typedef void (DataSourceFreeProc)(DataSource *srcPtr);
typedef Tcl_Obj * (DataSourcePrintProc)(DataSource *srcPtr);

typedef int (DataSourceChangedProc)(DataSource *srcPtr);

typedef enum SourceTypes {
    SOURCE_LIST, SOURCE_VECTOR, SOURCE_TABLE, SOURCE_NONE
} SourceType;

typedef struct {
    enum SourceTypes type;              /* Selects the type of data
                                         * populating this data source:
                                         * SOURCE_VECTOR, SOURCE_TABLE, or
                                         * SOURCE_LIST */
    const char *name;
    DataSourceGetProc *getProc;
    DataSourceFreeProc *freeProc;
    DataSourcePrintProc *printProc;
} DataSourceClass;

struct _DataSourceResult {
    double min, max;
    double *values;
    int numValues;
};

struct _DataSource {
    ClientData clientData;
    DataSourceClass *classPtr;
};

typedef struct {
    ClientData clientData;
    DataSourceClass *classPtr;

    /* Vector-specific fields. */
    Blt_VectorToken token;
} VectorDataSource;

typedef struct {
    ClientData clientData;
    DataSourceClass *classPtr;

    /* List-specific fields. */
    double *values;
    int numValues;
} ListDataSource;

typedef struct {
    BLT_TABLE table;
    int refCount;
} TableClient;

typedef struct {
    ClientData clientData;
    DataSourceClass *classPtr;

    /* Table-specific fields. */
    BLT_TABLE table;                    /* Data table. */ 
    BLT_TABLE_COLUMN column;            /* Column of data used. */
    BLT_TABLE_NOTIFIER notifier;        /* Notifier used for column
                                         * (destroy). */
    BLT_TABLE_TRACE trace;              /* Trace used for column
                                         * (set/get/unset). */
    Blt_HashEntry *hashPtr;             /* Pointer to entry of source in
                                         * meshes' hash table of
                                         * meshes. One mesh may use
                                         * multiple columns from the same
                                         * data table. */
} TableDataSource;

typedef int (MeshConfigureProc)(Tcl_Interp *interp, Blt_Mesh mesh);

typedef enum MeshTypes {
    MESH_CLOUD, MESH_REGULAR, MESH_IRREGULAR, MESH_TRIANGLE
} MeshType;

typedef struct {
    double x, y;
} MeshKey;

struct _MeshClass {
    enum MeshTypes type;                /* Type of mesh. */
    const char *name;                   /* Name of mesh class. */
    Blt_SwitchSpec *specs;              /* Mesh configuration
                                         * specifications. */
    MeshConfigureProc *configProc;      /* Configure procedure. */
};

typedef struct _Blt_Mesh {
    const char *name;                   /* Mesh identifier. */
    MeshClass *classPtr;
    MeshCmdInterpData *dataPtr;
    Tcl_Interp *interp;
    int refCount;                       /* Reference count of mesh. */
    unsigned int flags;                 /* Indicates if the mesh element is
                                         * active or normal */
    Blt_HashEntry *hashPtr;
    DataSource *x, *y;

    /* Resulting mesh is a triangular grid. */
    Point2d *vertices;                  /* x */
    int numVertices;                    /* x */
    int *hull;                          /* x Array of indices pointing into
                                         * the mesh representing the convex
                                         * hull of the mesh. */
    int numHullPts;                     /* x */
    float xMin, yMin, xMax, yMax;       /* x */
    Blt_MeshTriangle *triangles;        /* Array of triangles. */
    Blt_MeshTriangle *reqTriangles;     /* User-requested triangles. */
    int numReqTriangles;
    int numTriangles;                   /* x # of triangles in array. */
    Blt_HashTable hideTable;
    Blt_HashTable tableTable;
    Blt_Chain notifiers;                /* List of client notifiers. */
} MeshObject;

typedef struct _Blt_MeshNotifier {
    const char *name;                   /* Token id for notifier. */
    Blt_MeshChangedProc *proc;          /* Procedure to be called when the
                                         * mesh changes. */
    ClientData clientData;              /* Data to be passed on notifier
                                         * callbacks.  */
} MeshNotifier;

static MeshConfigureProc CloudMeshConfigureProc;
static MeshConfigureProc IrregularMeshConfigureProc;
static MeshConfigureProc RegularMeshConfigureProc;
static MeshConfigureProc TriangleMeshConfigureProc;

static DataSourceGetProc     ListDataSourceGetProc;
static DataSourcePrintProc   ListDataSourcePrintProc;
static DataSourceFreeProc ListDataSourceFreeProc;

static DataSourceGetProc     TableDataSourceGetProc;
static DataSourcePrintProc   TableDataSourcePrintProc;
static DataSourceFreeProc TableDataSourceFreeProc;

static DataSourceGetProc     VectorDataSourceGetProc;
static DataSourcePrintProc   VectorDataSourcePrintProc;
static DataSourceFreeProc VectorDataSourceFreeProc;

static DataSourceClass listDataSourceClass = {
    SOURCE_LIST, 
    "List",
    ListDataSourceGetProc,
    ListDataSourceFreeProc,
    ListDataSourcePrintProc
};

static DataSourceClass vectorDataSourceClass = {
    SOURCE_VECTOR, "Vector",
    VectorDataSourceGetProc,
    VectorDataSourceFreeProc,
    VectorDataSourcePrintProc
};

static DataSourceClass tableDataSourceClass = {
    SOURCE_TABLE, "Table",
    TableDataSourceGetProc,
    TableDataSourceFreeProc,
    TableDataSourcePrintProc
};

static Blt_SwitchFreeProc FreeTrianglesProc;
static Blt_SwitchParseProc ObjToTriangles;
static Blt_SwitchPrintProc TrianglesToObj;
static Blt_SwitchCustom trianglesSwitch = {
    ObjToTriangles, TrianglesToObj, FreeTrianglesProc, (ClientData)0
};

static Blt_SwitchFreeProc FreeDataSourceProc;
static Blt_SwitchParseProc ObjToDataSource;
static Blt_SwitchPrintProc DataSourceToObj;
Blt_SwitchCustom bltDataSourceSwitch = {
    ObjToDataSource, DataSourceToObj, FreeDataSourceProc, (ClientData)0
};

static Blt_SwitchSpec cloudMeshSpecs[] = {
    {BLT_SWITCH_CUSTOM, "-x", (char *)NULL, (char *)NULL, 
        Blt_Offset(MeshObject, x), 0, 0, &bltDataSourceSwitch},
    {BLT_SWITCH_CUSTOM, "-y", (char *)NULL, (char *)NULL, 
        Blt_Offset(MeshObject, y), 0, 0, &bltDataSourceSwitch},
    {BLT_SWITCH_END}
};

static Blt_SwitchSpec regularMeshSpecs[] = {
    {BLT_SWITCH_CUSTOM, "-x", (char *)NULL, (char *)NULL, 
        Blt_Offset(MeshObject, x), 0, 0, &bltDataSourceSwitch},
    {BLT_SWITCH_CUSTOM, "-y", (char *)NULL, (char *)NULL, 
        Blt_Offset(MeshObject, y), 0, 0, &bltDataSourceSwitch},
    {BLT_SWITCH_END}
};

static Blt_SwitchSpec irregularMeshSpecs[] = {
    {BLT_SWITCH_CUSTOM, "-x", (char *)NULL, (char *)NULL, 
        Blt_Offset(MeshObject, x), 0, 0, &bltDataSourceSwitch},
    {BLT_SWITCH_CUSTOM, "-y", (char *)NULL, (char *)NULL, 
        Blt_Offset(MeshObject, y), 0, 0, &bltDataSourceSwitch},
    {BLT_SWITCH_END}
};

static Blt_SwitchSpec triangleMeshSpecs[] = {
    {BLT_SWITCH_CUSTOM, "-x",  (char *)NULL, (char *)NULL, 
        Blt_Offset(MeshObject, x), 0, 0, &bltDataSourceSwitch},
    {BLT_SWITCH_CUSTOM, "-y", (char *)NULL, (char *)NULL, 
        Blt_Offset(MeshObject, y), 0, 0, &bltDataSourceSwitch},
    {BLT_SWITCH_CUSTOM, "-triangles", (char *)NULL, (char *)NULL, 
        Blt_Offset(MeshObject, triangles), 0, 0, &trianglesSwitch},
    {BLT_SWITCH_END}
};

static MeshClass cloudMeshClass = {
    MESH_CLOUD, 
    "cloud",
    cloudMeshSpecs,
    CloudMeshConfigureProc,

};

static MeshClass regularMeshClass = {
    MESH_REGULAR, 
    "regular", 
    regularMeshSpecs,
    RegularMeshConfigureProc,
};

static MeshClass triangleMeshClass = {
    MESH_TRIANGLE, 
    "triangle",
    triangleMeshSpecs,
    TriangleMeshConfigureProc,
};

static MeshClass irregularMeshClass = {
    MESH_IRREGULAR, 
    "irregular",
    irregularMeshSpecs,
    IrregularMeshConfigureProc,
};

static MeshCmdInterpData *GetMeshCmdInterpData(Tcl_Interp *interp);
static int ComputeMesh(MeshObject *meshObjPtr);


/*
 *---------------------------------------------------------------------------
 *
 * ObjToTriangles --
 *
 *      Given a string representation of a data source, this routine
 *      converts it into its equivalent data source.  A data source may be
 *      a list of numbers, a vector name, or a two element list of table
 *      name and column.
 *
 * Results:
 *      The return value is a standard TCL result.  The data source is
 *      passed back via the srcPtr.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToTriangles(ClientData clientData, Tcl_Interp *interp,
               const char *switchName, Tcl_Obj *objPtr, char *record,
               int offset, int flags)
{
    MeshObject *meshObjPtr = (MeshObject *)record;
    Tcl_Obj **objv;
    int objc;
    Blt_MeshTriangle *t, *reqTriangles;
    int i, numReqTriangles;

    if (Tcl_ListObjGetElements(interp, objPtr, &objc, &objv) != TCL_OK) {
        return TCL_ERROR;
    }
    if (objc == 0) {
        if (meshObjPtr->reqTriangles != NULL) {
            Blt_Free(meshObjPtr->reqTriangles);
        }
        meshObjPtr->reqTriangles = NULL;
        meshObjPtr->numReqTriangles = 0;
        return TCL_OK;
    }
    if ((objc % 3) != 0) {
        Tcl_AppendResult(interp, "wrong # of elements in triangle list: ",
                         "must be have 3 indices for each triangle",
                         (char *)NULL);
        return TCL_ERROR;
    }
    numReqTriangles = objc / 3;
    reqTriangles = Blt_Malloc(sizeof(Blt_MeshTriangle) * numReqTriangles);
    if (reqTriangles == NULL) {
        Tcl_AppendResult(interp, "can't allocate array of ",
                Blt_Itoa(numReqTriangles), " triangles", (char *)NULL);
        return TCL_ERROR;
    }
    t = reqTriangles;
    for (i = 0; i < objc; i += 3) {
        long a, b, c;

        /* We assume that user-defined triangle indices start from 1. */
        if ((Blt_GetLongFromObj(interp, objv[i], &a) != TCL_OK) || (a <= 0)) {
            Tcl_AppendResult(interp, "bad triangle index \"", 
                Tcl_GetString(objv[i]), "\"", (char *)NULL);
            goto error;
        }
        if ((Blt_GetLongFromObj(interp, objv[i+1], &b) != TCL_OK) || (b <= 0)) {
            Tcl_AppendResult(interp, "bad triangle index \"", 
                Tcl_GetString(objv[i+1]), "\"", (char *)NULL);
            goto error;
        }
        if ((Blt_GetLongFromObj(interp, objv[i+2], &c) != TCL_OK) || (c <= 0)) {
            Tcl_AppendResult(interp, "bad triangle index \"", 
                Tcl_GetString(objv[i+2]), "\"", (char *)NULL);
            goto error;
        }
        /* But our triangle indices start from 0. */
        t->a = a - 1;
        t->b = b - 1;
        t->c = c - 1;
        t++;
    }
    if (meshObjPtr->reqTriangles != NULL) {
        Blt_Free(meshObjPtr->reqTriangles);
    }
    meshObjPtr->reqTriangles = reqTriangles;
    meshObjPtr->numReqTriangles = numReqTriangles;
    return TCL_OK;
 error:
    if (reqTriangles != NULL) {
        Blt_Free(reqTriangles);
    }
    meshObjPtr->numReqTriangles = 0;
    return TCL_ERROR;
}

static Tcl_Obj *
ObjOfTriangle(Tcl_Interp *interp, Blt_MeshTriangle *t)
{
    Tcl_Obj *listObjPtr;
    
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(t->a));
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(t->b));
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(t->c));
    return listObjPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * TrianglesToObj --
 *
 *      Converts the data source to its equivalent string representation.
 *      The data source may be a table, vector, or list.
 *
 * Results:
 *      The string representation of the data source is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
TrianglesToObj(ClientData clientData, Tcl_Interp *interp, char *record, 
               int offset, int flags)
{
    MeshObject *meshObjPtr = (MeshObject *)record;
    Tcl_Obj *listObjPtr;
    int i;

    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    for (i = 0; i < meshObjPtr->numReqTriangles; i++) {
        Tcl_Obj *objPtr;
        
        objPtr = ObjOfTriangle(interp, meshObjPtr->reqTriangles + i);
        Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    }
    return listObjPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * FreeTrianglesProc --
 *
 *      Free the table used.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static void
FreeTrianglesProc(ClientData clientData, char *record, int offset, int flags)
{
    MeshObject *meshObjPtr = (MeshObject *)record;

    if (meshObjPtr->reqTriangles != NULL) {
        Blt_Free(meshObjPtr->reqTriangles);
        meshObjPtr->reqTriangles = NULL;
        meshObjPtr->numReqTriangles = 0;
    }
}

static void
NotifyClients(MeshObject *meshObjPtr, unsigned int flags)
{
     Blt_ChainLink link;

     for (link = Blt_Chain_FirstLink(meshObjPtr->notifiers); link != NULL;
        link = Blt_Chain_NextLink(link)) {
         MeshNotifier *notifyPtr;

         /* Notify each client that the mesh has changed. */
         notifyPtr = Blt_Chain_GetValue(link);
        if (notifyPtr->proc != NULL) {
            (*notifyPtr->proc)(meshObjPtr, notifyPtr->clientData, flags);
        }
    }
}

static void
ConfigureMesh(ClientData clientData)
{
    MeshObject *meshObjPtr = clientData;

    if ((*meshObjPtr->classPtr->configProc)(meshObjPtr->interp, meshObjPtr) 
        != TCL_OK) {
        Tcl_BackgroundError(meshObjPtr->interp);
        return;                         /* Failed to configure element */
    }
    if ((meshObjPtr->numVertices == 0) || (meshObjPtr->vertices == NULL)) {
        return;
    }
    NotifyClients(meshObjPtr, MESH_CHANGE_NOTIFY);
}

static void
EventuallyConfigureMeshObject(ClientData clientData)
{
    MeshObject *meshObjPtr = clientData;

    if ((meshObjPtr->flags & CONFIG_PENDING) == 0) {
        meshObjPtr->flags |= CONFIG_PENDING;
        Tcl_DoWhenIdle(ConfigureMesh, meshObjPtr);
    }
}

static void
FreeDataSource(DataSource *srcPtr)
{
    MeshObject *meshObjPtr = srcPtr->clientData;

    if ((srcPtr->classPtr != NULL) && (srcPtr->classPtr->freeProc != NULL)) {
        (*srcPtr->classPtr->freeProc)(srcPtr);
    }
    if (meshObjPtr->x == srcPtr) {
        meshObjPtr->x = NULL;
    } else if (meshObjPtr->y == srcPtr) {
        meshObjPtr->y = NULL;
    }
    memset(srcPtr, 0, sizeof(*srcPtr));
    Blt_Free(srcPtr);
}

static Tcl_Obj *
VectorDataSourcePrintProc(DataSource *basePtr)
{
    VectorDataSource *srcPtr = (VectorDataSource *)basePtr;
            
    return Tcl_NewStringObj(Blt_NameOfVectorFromToken(srcPtr->token), -1);
}

static void
VectorDataSourceFreeProc(DataSource *basePtr)
{
    VectorDataSource *srcPtr = (VectorDataSource *)basePtr;

    if (srcPtr->token != NULL) { 
        Blt_FreeVectorToken(srcPtr->token); 
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * VectorChangedProc --
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static void
VectorChangedProc(Tcl_Interp *interp, ClientData clientData, 
                  Blt_VectorNotify notify)
{
    VectorDataSource *srcPtr = clientData;
    MeshObject *meshObjPtr;

    meshObjPtr = srcPtr->clientData;
    /* Reconfigure the mesh now that one of its vectors has changed. */
    EventuallyConfigureMeshObject(meshObjPtr);
}

static DataSource *
NewVectorDataSource(Tcl_Interp *interp, MeshObject *meshObjPtr, 
                    const char *vecName)
{
    Blt_Vector *vecPtr;
    VectorDataSource *srcPtr;
    
    srcPtr = Blt_AssertCalloc(1, sizeof(VectorDataSource));
    srcPtr->classPtr = &vectorDataSourceClass;
    srcPtr->token = Blt_GetVectorToken(interp, vecName);
    if (Blt_GetVectorFromToken(interp, srcPtr->token, &vecPtr) != TCL_OK) {
        Blt_Free(srcPtr);
        return NULL;
    }
    Blt_SetVectorChangedProc(srcPtr->token, VectorChangedProc, srcPtr);
    return (DataSource *)srcPtr;
}

static int
VectorDataSourceGetProc(Tcl_Interp *interp, DataSource *basePtr, 
                        DataSourceResult *resultPtr)
{
    VectorDataSource *srcPtr = (VectorDataSource *)basePtr;
    size_t numBytes;
    Blt_Vector *vector;
    double *values;

    if (Blt_GetVectorFromToken(interp, srcPtr->token, &vector) != TCL_OK) {
        return TCL_ERROR;
    }
    numBytes = Blt_VecLength(vector) * sizeof(double);
    values = Blt_Malloc(numBytes);
    if (values == NULL) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "can't allocate new vector", (char *)NULL);
        }
        return TCL_ERROR;
    }
    {
        double min, max;
        double *p; 
        int i;

        p = Blt_VecData(vector);
        min = max = *p;
        for (i = 0; i < Blt_VecLength(vector); i++, p++) {
            values[i] = *p;
            if (*p > max) {
                max = (double)*p;
            } else if (*p < min) {
                min = (double)*p;
            }
        }
        resultPtr->min = min;
        resultPtr->max = max;
    }
    resultPtr->values = values;
    resultPtr->numValues = Blt_VecLength(vector);
    return TCL_OK;
}

static Tcl_Obj *
ListDataSourcePrintProc(DataSource *basePtr)
{
    ListDataSource *srcPtr = (ListDataSource *)basePtr;
    Tcl_Interp *interp;
    Tcl_Obj *listObjPtr;
    int i;
    MeshObject *meshObjPtr;

    meshObjPtr = srcPtr->clientData;
    interp = meshObjPtr->interp;
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    for (i = 0; i < srcPtr->numValues; i++) {
        Tcl_Obj *objPtr;

        objPtr = Tcl_NewDoubleObj(srcPtr->values[i]);
        Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    }
    return listObjPtr;
}

static void
ListDataSourceFreeProc(DataSource *basePtr)
{
    ListDataSource *srcPtr = (ListDataSource *)basePtr;

    if (srcPtr->values != NULL) {
        Blt_Free(srcPtr->values);
        srcPtr->values = NULL;
    }
}

static DataSource *
NewListDataSource(Tcl_Interp *interp, MeshObject *meshObjPtr, int objc, 
                  Tcl_Obj **objv)
{
    double *values;
    ListDataSource *srcPtr;

    srcPtr = Blt_AssertMalloc(sizeof(ListDataSource));
    srcPtr->classPtr = &listDataSourceClass;
    values = NULL;
    if (objc > 0) {
        int i;

        values = Blt_Malloc(sizeof(double) * objc);
        if (values == NULL) {
            Tcl_AppendResult(interp, "can't allocate new vector", (char *)NULL);
            Blt_Free(srcPtr);
            return NULL;
        }
        for (i = 0; i < objc; i++) {
            if (Blt_ExprDoubleFromObj(interp, objv[i], values + i) != TCL_OK) {
                Blt_Free(values);
                Blt_Free(srcPtr);
                return NULL;
            }
        }
        srcPtr->values = values;
        srcPtr->numValues = objc;
    }
    return (DataSource *)srcPtr;
}

static int
ListDataSourceGetProc(Tcl_Interp *interp, DataSource *basePtr, 
                      DataSourceResult *resultPtr)
{
    int i;
    double *values;
    double min, max;
    ListDataSource *srcPtr = (ListDataSource *)basePtr;

    values = Blt_Malloc(sizeof(double) * srcPtr->numValues);
    if (values == NULL) {
        return TCL_ERROR;
    }
    min = max = srcPtr->values[0];
    for (i = 0; i < srcPtr->numValues; i++) {
        values[i] = srcPtr->values[i];
        if (values[i] > max) {
            max = values[i];
        } else if (values[i] < min) {
            min = values[i];
        }
    }
    resultPtr->min = min;
    resultPtr->max = max;
    resultPtr->values = values;
    resultPtr->numValues = srcPtr->numValues;
    return TCL_OK;
}


static void
TableDataSourceFreeProc(DataSource *basePtr)
{
    TableDataSource *srcPtr = (TableDataSource *)basePtr;

    /* Shouldn't be here if column was destroyed. Need to mark source
     * as deleted. */
    if (srcPtr->trace != NULL) {
        blt_table_delete_trace(srcPtr->table, srcPtr->trace);
    }
    if (srcPtr->notifier != NULL) {
        blt_table_delete_notifier(srcPtr->table, srcPtr->notifier);
    }
    if (srcPtr->hashPtr != NULL) {
        TableClient *clientPtr;

        clientPtr = Blt_GetHashValue(srcPtr->hashPtr);
        clientPtr->refCount--;
        if (clientPtr->refCount == 0) {
            MeshObject *meshObjPtr;

            meshObjPtr = srcPtr->clientData;
            if (srcPtr->table != NULL) {
                blt_table_close(srcPtr->table);
            }
            Blt_Free(clientPtr);
            Blt_DeleteHashEntry(&meshObjPtr->tableTable, srcPtr->hashPtr);
        }
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * TableNotifyProc --
 *
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static int
TableNotifyProc(ClientData clientData, BLT_TABLE_NOTIFY_EVENT *eventPtr)
{
    DataSource *srcPtr = clientData;
    MeshObject *meshObjPtr;

    meshObjPtr = srcPtr->clientData;
    if (eventPtr->type == TABLE_NOTIFY_COLUMNS_DELETED) {
        FreeDataSource(srcPtr);
        if (meshObjPtr->x == srcPtr) {
            meshObjPtr->x = NULL;
        } else if (meshObjPtr->y == srcPtr) {
            meshObjPtr->y = NULL;
        }
    } 
    EventuallyConfigureMeshObject(meshObjPtr);
    return TCL_OK;
}
 
/*
 *---------------------------------------------------------------------------
 *
 * TableTraceProc --
 *
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static int
TableTraceProc(ClientData clientData, BLT_TABLE_TRACE_EVENT *eventPtr)
{
    TableDataSource *srcPtr = clientData;
    MeshObject *meshObjPtr;

    meshObjPtr = srcPtr->clientData;
    assert(eventPtr->column == srcPtr->column);
    EventuallyConfigureMeshObject(meshObjPtr);
    return TCL_OK;
}


static DataSource *
NewTableDataSource(Tcl_Interp *interp, MeshObject *meshObjPtr, const char *name,
                   Tcl_Obj *colObjPtr)
{
    TableDataSource *srcPtr;
    TableClient *clientPtr;
    int isNew;

    srcPtr = Blt_AssertMalloc(sizeof(TableDataSource));
    srcPtr->classPtr = &tableDataSourceClass;

    /* See if the mesh is already using this table. */
    srcPtr->hashPtr = Blt_CreateHashEntry(&meshObjPtr->tableTable, name, 
                                          &isNew);
    if (isNew) {
        if (blt_table_open(interp, name, &srcPtr->table) != TCL_OK) {
            return NULL;
        }
        clientPtr = Blt_AssertMalloc(sizeof(TableClient));
        clientPtr->table = srcPtr->table;
        clientPtr->refCount = 1;
        Blt_SetHashValue(srcPtr->hashPtr, clientPtr);
    } else {
        clientPtr = Blt_GetHashValue(srcPtr->hashPtr);
        srcPtr->table = clientPtr->table;
        clientPtr->refCount++;
    }
    srcPtr->column = blt_table_get_column(interp, srcPtr->table, colObjPtr);
    if (srcPtr->column == NULL) {
        goto error;
    }
    srcPtr->notifier = blt_table_create_column_notifier(interp, srcPtr->table,
        srcPtr->column, TABLE_NOTIFY_COLUMN_CHANGED, TableNotifyProc, 
        (BLT_TABLE_NOTIFIER_DELETE_PROC *)NULL, srcPtr);
    srcPtr->trace = blt_table_create_column_trace(srcPtr->table, srcPtr->column,
        TABLE_TRACE_WCU, TableTraceProc, (BLT_TABLE_TRACE_DELETE_PROC *)NULL, 
        srcPtr);
    return (DataSource *)srcPtr;
 error:
    FreeDataSource((DataSource *)srcPtr);
    return NULL;
}

static Tcl_Obj *
TableDataSourcePrintProc(DataSource *basePtr)
{
    TableDataSource *srcPtr = (TableDataSource *)basePtr;
    Tcl_Obj *listObjPtr;
    const char *name;
    long index;                         /* Column index. */
    Tcl_Interp *interp;
    MeshObject *meshObjPtr;

    meshObjPtr = srcPtr->clientData;
    interp = meshObjPtr->interp;
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);

    name = blt_table_name(srcPtr->table);
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewStringObj(name, -1));
    
    index = blt_table_column_index(srcPtr->table, srcPtr->column);
    Tcl_ListObjAppendElement(interp, listObjPtr, Blt_NewLongObj(index));
    return listObjPtr;
}

static int
TableDataSourceGetProc(Tcl_Interp *interp, DataSource *basePtr, 
                       DataSourceResult *resultPtr)
{
    BLT_TABLE table;
    BLT_TABLE_ROW row;
    TableDataSource *srcPtr = (TableDataSource *)basePtr;
    double *values;
    double minValue, maxValue;
    int i;

    table = srcPtr->table;
    values = Blt_Malloc(sizeof(double) * blt_table_num_rows(table));
    if (values == NULL) {
        return TCL_ERROR;
    }
    minValue = FLT_MAX, maxValue = -FLT_MAX;
    for (i = 0, row = blt_table_first_row(table); row != NULL; 
         row = blt_table_next_row(row), i++) {
        values[i] = blt_table_get_double(interp, table, row, srcPtr->column);
        if (values[i] < minValue) {
            minValue = values[i];
        }
        if (values[i] > maxValue) {
            maxValue = values[i];
        }
    }
    resultPtr->min = minValue;
    resultPtr->max = maxValue;
    resultPtr->values = values;
    resultPtr->numValues = i;
    return TCL_OK;
}

/*ARGSUSED*/
static void
FreeDataSourceProc(ClientData clientData, char *record, int offset, int flags)
{
    DataSource **srcPtrPtr = (DataSource **)(record + offset);

    if (*srcPtrPtr != NULL) {
        FreeDataSource(*srcPtrPtr);
    }
    *srcPtrPtr = NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToDataSource --
 *
 *      Given a string representation of a data source, converts it into
 *      its equivalent data source.  A data source may be a list of
 *      numbers, a vector name, or a two element list of table name and
 *      column.
 *
 * Results:
 *      The return value is a standard TCL result.  The data source is
 *      passed back via the srcPtr.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToDataSource(ClientData clientData, Tcl_Interp *interp,
                const char *switchName, Tcl_Obj *objPtr, char *record,
                int offset, int flags)
{
    MeshObject *meshObjPtr = (MeshObject *)record;
    DataSource *srcPtr;
    DataSource **srcPtrPtr = (DataSource **)(record + offset);
    Tcl_Obj **objv;
    int objc;
    const char *string;
    
    if (Tcl_ListObjGetElements(interp, objPtr, &objc, &objv) != TCL_OK) {
        return TCL_ERROR;
    }
    if (objc == 0) {
        if (*srcPtrPtr != NULL) {
            FreeDataSource(*srcPtrPtr);
        }
        *srcPtrPtr = NULL;
        return TCL_OK;
    }
    string = Tcl_GetString(objv[0]);
    if ((objc == 1) && (Blt_VectorExists2(interp, string))) {
        srcPtr = NewVectorDataSource(interp, meshObjPtr, string);
    } else if ((objc == 2) && (blt_table_exists(interp, string))) {
        srcPtr = NewTableDataSource(interp, meshObjPtr, string, objv[1]);
    } else {
        srcPtr = NewListDataSource(interp, meshObjPtr, objc, objv);
    }
    srcPtr->clientData = meshObjPtr;
    *srcPtrPtr = srcPtr;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * DataSourceToObj --
 *
 *      Converts the data source to its equivalent string representation.
 *      The data source may be a table, vector, or list.
 *
 * Results:
 *      The string representation of the data source is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
DataSourceToObj(ClientData clientData, Tcl_Interp *interp, char *record, 
                int offset, int flags)
{
    DataSource *srcPtr = *(DataSource **)(record + offset);

    if ((srcPtr != NULL) && (srcPtr->classPtr != NULL)) {
        return (srcPtr->classPtr->printProc)(srcPtr);
    }
    return Tcl_NewStringObj("", -1);
}

static void
DestroyMeshObject(MeshObject *meshObjPtr)
{
    MeshCmdInterpData *dataPtr;

    dataPtr = meshObjPtr->dataPtr;
    if (meshObjPtr->hashPtr != NULL) {
        Blt_DeleteHashEntry(&dataPtr->meshTable, meshObjPtr->hashPtr);
    }
    Blt_FreeSwitches(meshObjPtr->classPtr->specs, (char *)meshObjPtr, 0);
    if (meshObjPtr->triangles != NULL) {
        Blt_Free(meshObjPtr->triangles);
    }
    if (meshObjPtr->vertices != NULL) {
        Blt_Free(meshObjPtr->vertices);
    }
    if (meshObjPtr->hull != NULL) {
        Blt_Free(meshObjPtr->hull);
    }
    if (meshObjPtr->notifiers != NULL) {
        Blt_Chain_Destroy(meshObjPtr->notifiers);
    }
    Blt_DeleteHashTable(&meshObjPtr->hideTable);
    Blt_Free(meshObjPtr);
}

static int
GetMeshObject(Tcl_Interp *interp, MeshCmdInterpData *dataPtr, Tcl_Obj *objPtr, 
              MeshObject **meshObjPtrPtr)
{
    Blt_HashEntry *hPtr;
    Blt_ObjectName objName;
    Tcl_DString ds;
    const char *name;
    MeshObject *meshObjPtr;
    const char *string;

    string = Tcl_GetString(objPtr);
    /* 
     * Parse the command and put back so that it's in a consistent
     * format.
     *
     *  t1         <current namespace>::t1
     *  n1::t1     <current namespace>::n1::t1
     *  ::t1       ::t1
     *  ::n1::t1   ::n1::t1
     */
    if (!Blt_ParseObjectName(interp, string, &objName, 0)) {
        return TCL_ERROR;
    }
    name = Blt_MakeQualifiedName(&objName, &ds);
    hPtr = Blt_FindHashEntry(&dataPtr->meshTable, name);
    Tcl_DStringFree(&ds);
    if (hPtr == NULL) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "can't find a mesh \"", string, "\"",
                         (char *)NULL);
        }
        return TCL_ERROR;
    }
    meshObjPtr = Blt_GetHashValue(hPtr);
    meshObjPtr->refCount++;
    *meshObjPtrPtr = meshObjPtr;
    return TCL_OK;
}

static int 
ComputeMesh(MeshObject *meshObjPtr)
{
    Blt_MeshTriangle *triangles;
    int numTriangles;
    int i, count;

    triangles = NULL;
    numTriangles = 0;
    if (meshObjPtr->numVertices > 0) {
        int *hull;
        int numPoints;
        
        /* Compute the convex hull first, this will provide an estimate for
         * the boundary vertices and therefore the number of triangles. */
        hull = Blt_ConvexHull(meshObjPtr->numVertices, meshObjPtr->vertices,
                              &numPoints);
        if (hull == NULL) {
            Tcl_AppendResult(meshObjPtr->interp, "can't allocate convex hull", 
                             (char *)NULL);
            goto error;
        }
        if (meshObjPtr->hull != NULL) {
            Blt_Free(meshObjPtr->hull);
        }
        meshObjPtr->hull = hull;
        meshObjPtr->numHullPts = numPoints;
        /* Determine the number of triangles. */
        numTriangles = 2 * meshObjPtr->numVertices;
        triangles = Blt_Malloc(numTriangles * sizeof(Blt_MeshTriangle));
        if (triangles == NULL) {
            Tcl_AppendResult(meshObjPtr->interp, "can't allocate ", 
                Blt_Itoa(numTriangles), " triangles", (char *)NULL);
            goto error;
        }
        numTriangles = Blt_Triangulate(meshObjPtr->interp, 
                meshObjPtr->numVertices, meshObjPtr->vertices, FALSE, 
                triangles);
        if (numTriangles == 0) {
            Tcl_AppendResult(meshObjPtr->interp, "error triangulating mesh", 
                             (char *)NULL);
            goto error;
        }
    }
    /* Compress the triangle array. This is because there are hidden
     * triangles designated or we over-allocated the initial array of
     * triangles. */
    count = 0;
    for (i = 0; i < numTriangles; i++) {
        if (Blt_FindHashEntry(&meshObjPtr->hideTable, (intptr_t)i)) {
            continue;
        }
        if (i > count) {
            triangles[count] = triangles[i];
        }
        count++;
    }   
    if (count > 0) {
        triangles = Blt_Realloc(triangles, count * sizeof(Blt_MeshTriangle));
    }
    if (meshObjPtr->triangles != NULL) {
        Blt_Free(meshObjPtr->triangles);
    }
    meshObjPtr->numTriangles = numTriangles;
    meshObjPtr->triangles = triangles;
    return TCL_OK;
 error:
    if (triangles != NULL) {
        Blt_Free(triangles);
    }
    return TCL_ERROR;
}

static int 
ComputeRegularMesh(MeshObject *meshObjPtr, int xNum, int yNum)
{
    int x, y;
    int i, numTriangles, numVertices, count;
    Blt_MeshTriangle *t, *triangles;
    int *hull;

    assert(xNum > 1);
    assert(yNum > 1);
    numTriangles = ((xNum - 1) * 2) * (yNum - 1);
    triangles = Blt_Malloc(numTriangles * sizeof(Blt_MeshTriangle));
    if (triangles == NULL) {
        Tcl_AppendResult(meshObjPtr->interp, "can't allocate ", 
                         Blt_Itoa(numTriangles), " triangles", (char *)NULL);
        return TCL_ERROR;
    }
    t = triangles;
    for (y = 0; y < ((yNum - 1) * xNum); y += xNum) {
        int upper, lower;

        upper = y;
        lower = y + xNum;
        for (x = 0; x < (xNum - 1); x++) {
            t->a = upper + x, t->b = upper + x + 1, t->c = lower + x;
            t++;
            t->a = upper + x + 1, t->b = lower + x + 1, t->c = lower + x;
            t++;
        }
    }
    /* Compute the convex hull. */
    numVertices = 4;
    hull = Blt_AssertMalloc(4 * sizeof(int));
    hull[0] = 0, hull[1] = xNum - 1;
    hull[2] = (yNum * xNum) - 1;
    hull[3] = xNum * (yNum - 1);
    if (meshObjPtr->hull != NULL) {
        Blt_Free(meshObjPtr->hull);
    }
    meshObjPtr->hull = hull;
    meshObjPtr->numHullPts = numVertices;

    /* Compress the triangle array. This is because there are hidden triangles
     * designated or we over-allocated the initial array of triangles. */
    count = 0;
    for (i = 0; i < numTriangles; i++) {
        if (Blt_FindHashEntry(&meshObjPtr->hideTable, (intptr_t)i)) {
            continue;
        }
        if (i > count) {
            triangles[count] = triangles[i];
        }
        count++;
    }   
    if (count > 0) {
        triangles = Blt_Realloc(triangles, count * sizeof(Blt_MeshTriangle));
    }
    if (meshObjPtr->triangles != NULL) {
        Blt_Free(meshObjPtr->triangles);
    }
    meshObjPtr->numTriangles = numTriangles;
    meshObjPtr->triangles = triangles;
    return TCL_OK;
}

static int
RegularMeshConfigureProc(Tcl_Interp *interp, MeshObject *meshObjPtr)
{
    double xStep, yStep;
    DataSourceResult x, y;
    double xMin, xMax, xNum, yNum, yMin, yMax;
    Point2d *vertices;
    int numVertices;

    interp = meshObjPtr->interp;
    if ((meshObjPtr->x == NULL) || (meshObjPtr->y == NULL)) {
        return TCL_OK;                  /* Missing x or y vectors. */
    }
    if ((*meshObjPtr->x->classPtr->getProc)(interp, meshObjPtr->x, &x) 
        != TCL_OK) {
        return TCL_ERROR;
    }
    if (x.numValues != 3) {
        Tcl_AppendResult(interp, 
                "wrong # of elements for x regular mesh description",
                (char *)NULL);
        return TCL_ERROR;
    }
    if ((*meshObjPtr->y->classPtr->getProc)(interp, meshObjPtr->y, &y) 
        != TCL_OK) {
        return TCL_ERROR;
    }
    if (y.numValues != 3) {
        Tcl_AppendResult(interp, 
                "wrong # of elements for y rectangular mesh description",
                (char *)NULL);
        return TCL_ERROR;
    }
    xMin = x.values[0];
    xMax = x.values[1];
    xNum = (int)x.values[2];
    yMin = y.values[0];
    yMax = y.values[1];
    yNum = (int)y.values[2];
    Blt_Free(x.values);
    Blt_Free(y.values);
    
    if (xNum < 2) {
        Tcl_AppendResult(interp, "too few x-values (", Blt_Itoa(xNum), 
                ") for rectangular mesh", (char *)NULL);
        return TCL_ERROR;
    }
    if (yNum < 2) {
        Tcl_AppendResult(interp, "too few y-values  (", Blt_Itoa(xNum), 
                ") for rectangular mesh", (char *)NULL);
        return TCL_ERROR;
    }
    if (xMin == xMax) {
        return TCL_ERROR;
    }
    if (yMin == yMax) {
        return TCL_ERROR;
    }
    numVertices = xNum * yNum;
    vertices = Blt_Malloc(numVertices * sizeof(Point2d));
    if (vertices == NULL) {
        Tcl_AppendResult(interp, "can't allocate ", Blt_Itoa(numVertices), 
                " vertices", (char *)NULL);
        return TCL_ERROR;
    }
    xStep = (xMax - xMin) / (double)(xNum - 1);
    yStep = (yMax - yMin) / (double)(yNum - 1);
    {
        Point2d *p;
        int i;

        p = vertices;
        for (i = 0; i < yNum; i++) {
            double y0;
            int j;
            
            y0 = yMin + (yStep * i);
            for (j = 0; j < xNum; j++) {
                p->x = xMin + (xStep * j);
                p->y = y0;
                p++;
            }
        }
    }
    if (meshObjPtr->vertices != NULL) {
        Blt_Free(meshObjPtr->vertices);
    }
    meshObjPtr->xMin = xMin, meshObjPtr->xMax = xMax;
    meshObjPtr->yMin = yMin, meshObjPtr->yMax = yMax;
    meshObjPtr->vertices = vertices;
    meshObjPtr->numVertices = numVertices;
    return ComputeRegularMesh(meshObjPtr, xNum, yNum);
}

static int
IrregularMeshConfigureProc(Tcl_Interp *interp, MeshObject *meshObjPtr)
{
    DataSourceResult x, y;
    Point2d *vertices;
    int numVertices;

    if ((meshObjPtr->x == NULL) || (meshObjPtr->y == NULL)) {
        return TCL_OK;
    }
    if ((meshObjPtr->x->classPtr == NULL) || 
        (meshObjPtr->y->classPtr == NULL)) {
        return TCL_OK;
    }
    if ((*meshObjPtr->x->classPtr->getProc)(interp, meshObjPtr->x, &x) 
        != TCL_OK) {
        return TCL_ERROR;
    }
    if (x.numValues < 2) {
        Tcl_AppendResult(interp, "wrong # of x-values (", Blt_Ltoa(x.numValues),
                 ") for irregular mesh description", (char *)NULL);
        return TCL_ERROR;
    }
    meshObjPtr->xMin = x.min;
    meshObjPtr->xMax = x.max;
    if ((*meshObjPtr->y->classPtr->getProc)(meshObjPtr->interp, meshObjPtr->y, 
                                            &y) != TCL_OK) {
        return TCL_ERROR;
    }
    if (y.numValues < 2) {
        Tcl_AppendResult(interp, "wrong # of y-values (", Blt_Ltoa(y.numValues),
                 ") for irregular mesh description", (char *)NULL);
        return TCL_ERROR;
    }
    meshObjPtr->yMin = y.min;
    meshObjPtr->yMax = y.max;
    numVertices = x.numValues * y.numValues;
    vertices = Blt_Malloc(numVertices * sizeof(Point2d));
    if (vertices == NULL) {
        Tcl_AppendResult(interp, "can't allocate ", Blt_Ltoa(numVertices), 
                " vertices", (char *)NULL);
        return TCL_ERROR;
    }
    {
        int i;
        Point2d *p;

        p = vertices;
        for (i = 0; i < y.numValues; i++) {
            int j;
            
            for (j = 0; j < x.numValues; j++) {
                p->x = x.values[j];
                p->y = y.values[i];
                p++;
            }
        }
    }
    Blt_Free(x.values);
    Blt_Free(y.values);
    if (meshObjPtr->vertices != NULL) {
        Blt_Free(meshObjPtr->vertices);
    }
    meshObjPtr->xMin = x.min, meshObjPtr->xMax = x.max;
    meshObjPtr->yMin = y.min, meshObjPtr->yMax = y.max;
    meshObjPtr->vertices = vertices;
    meshObjPtr->numVertices = numVertices;
    return ComputeMesh(meshObjPtr);
}

static int
CloudMeshConfigureProc(Tcl_Interp *interp, MeshObject *meshObjPtr)
{
    Blt_HashTable table;
    DataSourceResult x, y;
    Point2d *vertices;
    int i, numVertices, count;

    if ((meshObjPtr->x == NULL) || (meshObjPtr->y == NULL)) {
        return TCL_OK;
    }
    if ((meshObjPtr->x->classPtr == NULL) || 
        (meshObjPtr->y->classPtr == NULL)) {
        return TCL_OK;
    }
    if ((*meshObjPtr->x->classPtr->getProc)(interp, meshObjPtr->x, &x) 
        != TCL_OK) {
        return TCL_ERROR;
    }
    if (x.numValues < 3) {
        Tcl_AppendResult(interp, "bad cloud mesh: too few x-coordinates \"",
                Blt_Itoa(x.numValues), "\"", (char *)NULL);
        return TCL_ERROR;
    }
    if ((*meshObjPtr->y->classPtr->getProc)(interp, meshObjPtr->y, &y) 
        != TCL_OK) {
        return TCL_ERROR;
    }
    if (y.numValues < 3) {
        Tcl_AppendResult(interp, "bad cloud mesh: too few y-coordinates \"",
                Blt_Itoa(y.numValues), "\"", (char *)NULL);
        return TCL_ERROR;
    }
    if (x.numValues != y.numValues) {
        Tcl_AppendResult(interp, 
        "bad cloud mesh: # of values for x and y coordinates do not match",
                (char *)NULL);
        return TCL_ERROR;
    }
    numVertices = x.numValues;
    vertices = Blt_Malloc(numVertices * sizeof(Point2d));
    if (vertices == NULL) {
        Tcl_AppendResult(interp, "can't allocate ", Blt_Itoa(numVertices), 
                         " vertices", (char *)NULL);
        return TCL_ERROR;
    }
    Blt_InitHashTable(&table, sizeof(MeshKey) / sizeof(int));
    count = 0;
    for (i = 0; i < numVertices; i++) {
        Blt_HashEntry *hPtr;
        int isNew;
        MeshKey key;

        key.x = x.values[i];
        key.y = y.values[i];
        hPtr = Blt_CreateHashEntry(&table, &key, &isNew);
        assert(hPtr != NULL);
        if (!isNew) {
            int index;

            index = (int)(intptr_t)Blt_GetHashValue(hPtr);
            fprintf(stderr,
                    "duplicate point %d x=%g y=%g, old=%d x=%g y=%g\n",
                    i, x.values[i], y.values[i], index, x.values[index], 
                    y.values[index]);
            continue;
        }
        Blt_SetHashValue(hPtr, (intptr_t)i);
        vertices[count].x = x.values[i];
        vertices[count].y = y.values[i];
        count++;
    }
    Blt_DeleteHashTable(&table);
    Blt_Free(x.values);
    Blt_Free(y.values);
    if (meshObjPtr->vertices != NULL) {
        Blt_Free(meshObjPtr->vertices);
    }
    meshObjPtr->xMin = x.min, meshObjPtr->xMax = x.max;
    meshObjPtr->yMin = y.min, meshObjPtr->yMax = y.max;
    meshObjPtr->vertices = vertices;
    meshObjPtr->numVertices = count;
    return ComputeMesh(meshObjPtr);
}

static int
TriangleMeshConfigureProc(Tcl_Interp *interp, MeshObject *meshObjPtr)
{
    DataSourceResult x, y;
    Point2d *vertices;
    int i, numVertices, numTriangles, count;
    Blt_MeshTriangle *triangles;
    int *hull;
    int numPoints;
    
    if ((meshObjPtr->x == NULL) || (meshObjPtr->y == NULL)) {
        return TCL_OK;
    }
    if (meshObjPtr->numReqTriangles == 0) {
        return TCL_OK;
    }
    if ((meshObjPtr->x->classPtr == NULL) || 
        (meshObjPtr->y->classPtr == NULL)) {
        return TCL_OK;
    }
    if ((*meshObjPtr->x->classPtr->getProc)(interp, meshObjPtr->x, &x) 
        != TCL_OK) {
        return TCL_ERROR;
    }
    if (x.numValues < 2) {
        Tcl_AppendResult(interp, "wrong # of x-values (", Blt_Itoa(x.numValues),
                 ") for irregular mesh description", (char *)NULL);
        return TCL_ERROR;
    }
    if ((*meshObjPtr->y->classPtr->getProc)(interp, meshObjPtr->y, &y) 
        != TCL_OK) {
        return TCL_ERROR;
    }
    if (y.numValues < 2) {
        Tcl_AppendResult(interp, "wrong # of y-values (", Blt_Itoa(y.numValues),
                 ") for irregular mesh description", (char *)NULL);
        return TCL_ERROR;
    }
    if (x.numValues != y.numValues) {
        Tcl_AppendResult(interp, " # of values for x and y do not match",
                (char *)NULL);
        return TCL_ERROR;
    }
    numVertices = x.numValues;
    vertices = Blt_Malloc(numVertices * sizeof(Point2d));
    if (vertices == NULL) {
        Tcl_AppendResult(interp, "can't allocate ", Blt_Itoa(numVertices), 
                " vertices", (char *)NULL);
        return TCL_ERROR;
    }
   /* Fill the vertices array with the sorted x-y coordinates. */
    for (i = 0; i < numVertices; i++) {
        vertices[i].x = x.values[i];
        vertices[i].y = y.values[i];
    }
    Blt_Free(x.values);
    Blt_Free(y.values);
    triangles = NULL;
    hull = Blt_ConvexHull(numVertices, vertices, &numPoints);
    if (hull == NULL) {
        Tcl_AppendResult(interp, "can't allocate convex hull", (char *)NULL);
        goto error;
    }
    if (meshObjPtr->hull != NULL) {
        Blt_Free(meshObjPtr->hull);
    }
    meshObjPtr->hull = hull;
    meshObjPtr->numHullPts = numPoints;
    numTriangles = meshObjPtr->numReqTriangles;
    /* Fill the triangles array with the sorted indices of the vertices. */
    triangles = Blt_AssertCalloc(numTriangles, sizeof(Blt_MeshTriangle));
    for (i = 0; i < numTriangles; i++) {
        Blt_MeshTriangle *t;

        t = meshObjPtr->reqTriangles + i;
        if ((t->a < 0) || (t->a >= numVertices)) {
            Tcl_AppendResult(interp, "first index on triangle ",
                             Blt_Ltoa(i), " is out of range",
                             (char *)NULL);
            goto error;
        }

        if ((t->b < 0) || (t->b >= numVertices)) {
            Tcl_AppendResult(interp, "second index on triangle ",
                             Blt_Ltoa(i), " is out of range",
                             (char *)NULL);
            goto error;
        }
        if ((t->c < 0) || (t->c >= numVertices)) {
            Tcl_AppendResult(interp, "third index on triangle ",
                             Blt_Itoa(i), " is out of range",
                             (char *)NULL);
            goto error;
        }
        triangles[i].a = t->a;
        triangles[i].b = t->b;
        triangles[i].c = t->c;
    }
    /* Compress the triangle array. */
    count = 0;
    for (i = 0; i < numTriangles; i++) {
        if (Blt_FindHashEntry(&meshObjPtr->hideTable, (intptr_t)i)) {
            continue;
        }
        if (i > count) {
            meshObjPtr->triangles[count] = meshObjPtr->triangles[i];
        }
        count++;
    }   
    if ((count > 0) && (count != numTriangles)) {
        triangles = Blt_Realloc(triangles, count * sizeof(Blt_MeshTriangle));
        if (triangles == NULL) {
            Tcl_AppendResult(interp, 
                "can't reallocate triangle array for mesh \"", meshObjPtr->name,
                "\"", (char *)NULL);
            goto error;
        }
        numTriangles = count;
    }
    if (meshObjPtr->vertices != NULL) {
        Blt_Free(meshObjPtr->vertices);
    }
    meshObjPtr->vertices = vertices;
    meshObjPtr->numVertices = numVertices;
    if (meshObjPtr->triangles != NULL) {
        Blt_Free(meshObjPtr->triangles);
    }
    meshObjPtr->xMin = x.min, meshObjPtr->xMax = x.max;
    meshObjPtr->yMin = y.min, meshObjPtr->yMax = y.max;
    meshObjPtr->numTriangles = numTriangles;
    meshObjPtr->triangles = triangles;
    return TCL_OK;
 error:
    if (vertices != NULL) {
        Blt_Free(vertices);
    }
    if (triangles != NULL) {
        Blt_Free(triangles);
    }
    return TCL_ERROR;
}

static MeshObject *
NewMeshObject(Tcl_Interp *interp, MeshCmdInterpData *dataPtr, int type, 
              Blt_HashEntry *hPtr)
{
    MeshObject *meshObjPtr;

    /* Allocate memory for the new mesh. */
    meshObjPtr = Blt_AssertCalloc(1, sizeof(MeshObject));
    switch (type) {
    case MESH_IRREGULAR:
        meshObjPtr->classPtr = &irregularMeshClass;
        break;
    case MESH_REGULAR:
        meshObjPtr->classPtr = &regularMeshClass;
        break;
    case MESH_TRIANGLE:
        meshObjPtr->classPtr = &triangleMeshClass;
        break;
    case MESH_CLOUD:
        meshObjPtr->classPtr = &cloudMeshClass;
        break;
    default:
        return NULL;
    }
    meshObjPtr->name = Blt_GetHashKey(&dataPtr->meshTable, hPtr);
    meshObjPtr->hashPtr = hPtr;
    meshObjPtr->interp = interp;
    meshObjPtr->dataPtr = dataPtr;
    meshObjPtr->refCount = 1;
    Blt_SetHashValue(hPtr, meshObjPtr);
    Blt_InitHashTable(&meshObjPtr->tableTable, BLT_STRING_KEYS);
    Blt_InitHashTable(&meshObjPtr->hideTable, BLT_ONE_WORD_KEYS);
    return meshObjPtr;
}

/* ARGSUSED*/
static int
CgetOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    MeshCmdInterpData *dataPtr = clientData;
    MeshObject *meshObjPtr;


    if (GetMeshObject(interp, dataPtr, objv[2], &meshObjPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    return Blt_SwitchValue(interp, meshObjPtr->classPtr->specs, 
                           (char *)meshObjPtr, objv[3], 0);
}

static int
ConfigureOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    MeshObject *meshObjPtr;
    MeshCmdInterpData *dataPtr = clientData;

    if (GetMeshObject(interp, dataPtr, objv[2], &meshObjPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (objc == 3) {
        return Blt_SwitchInfo(interp, meshObjPtr->classPtr->specs, meshObjPtr,
                (Tcl_Obj *)NULL, 0);
    } else if (objc == 4) {
        return Blt_SwitchInfo(interp, meshObjPtr->classPtr->specs, meshObjPtr, 
                objv[3], 0);
    }
    bltDataSourceSwitch.clientData = meshObjPtr;
    if (Blt_ParseSwitches(interp, meshObjPtr->classPtr->specs, objc - 3, 
        objv + 3, (char *)meshObjPtr, 0) < 0) {
        return TCL_ERROR;
    }
    ConfigureMesh(meshObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CreateOp --
 *
 *      Creates a mesh.
 *
 * Results:
 *      The return value is a standard TCL result. The interpreter
 *      result will contain a TCL list of the element names.
 *
 *      blt::mesh create meshType meshName? ?option value ... ?
 *
 *---------------------------------------------------------------------------
 */
static int
CreateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
         Tcl_Obj *const *objv)
{
    Blt_HashEntry *hPtr;
    MeshObject *meshObjPtr;
    MeshCmdInterpData *dataPtr = clientData;
    Tcl_DString ds;
    char c;
    const char *name, *string;
    int isNew, length, type;

    meshObjPtr = NULL;                     /* Suppress compiler warning. */
    string = Tcl_GetString(objv[2]);
    c = string[0];
    length = strlen(string);
    if ((c == 't') && (strncmp(string, "triangle", length) == 0)) {
        type = MESH_TRIANGLE;
    } else if ((c == 'r') && (strncmp(string, "regular", length) == 0)) {
        type = MESH_REGULAR;
    } else if ((c == 'i') && (strncmp(string, "irregular", length) == 0)) {
        type = MESH_IRREGULAR;
    } else if ((c == 'c') && (strncmp(string, "cloud", length) == 0)) {
        type = MESH_CLOUD;
    } else {
        Tcl_AppendResult(interp, "unknown mesh type \"", string, "\"",
                         (char *)NULL);
        return TCL_ERROR;
    }
    name = NULL;
    Tcl_DStringInit(&ds);
    if (objc > 3) {
        string = Tcl_GetString(objv[3]);
        if (string[0] != '-') {
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
            if (!Blt_ParseObjectName(interp, string, &objName, 0)) {
                return TCL_ERROR;
            }
            name = Blt_MakeQualifiedName(&objName, &ds);
            if (Blt_FindHashEntry(&dataPtr->meshTable, name) != NULL) {
                Tcl_AppendResult(interp, "mesh \"", name, 
                        "\" already exists", (char *)NULL);
                return TCL_ERROR;
            }
            objc--, objv++;
        }
    }
    /* If no name was given for the marker, make up one. */
    if (name == NULL) {
        Blt_ObjectName objName;
        char ident[256];

        Blt_FmtString(ident, 255, "mesh%d", dataPtr->nextMeshId++);
        if (!Blt_ParseObjectName(interp, ident, &objName, 0)) {
            return TCL_ERROR;
        }
        name = Blt_MakeQualifiedName(&objName, &ds);
    }
    hPtr = Blt_CreateHashEntry(&dataPtr->meshTable, name, &isNew);
    Tcl_DStringFree(&ds);
    if (!isNew) {
        Tcl_AppendResult(interp, "mesh \"", name, "\" already exists",
                         (char *)NULL);
        return TCL_ERROR;
    }
    switch (type) {
    case MESH_TRIANGLE:
        meshObjPtr = NewMeshObject(interp, dataPtr, MESH_TRIANGLE, hPtr);
        break;
    case MESH_REGULAR:
        meshObjPtr = NewMeshObject(interp, dataPtr, MESH_REGULAR, hPtr);
        break;
    case MESH_IRREGULAR:
        meshObjPtr = NewMeshObject(interp, dataPtr, MESH_IRREGULAR, hPtr);
        break;
    case MESH_CLOUD:
        meshObjPtr = NewMeshObject(interp, dataPtr, MESH_CLOUD, hPtr);
        break;
    }
    /* Parse the configuration options. */
    if (Blt_ParseSwitches(interp, meshObjPtr->classPtr->specs, objc - 3, 
        objv + 3, (char *)meshObjPtr, BLT_SWITCH_INITIALIZE) < 0) {
        DestroyMeshObject(meshObjPtr);
        return TCL_ERROR;
    }
    if (!isNew) {
        MeshObject *oldMeshPtr;

        oldMeshPtr = Blt_GetHashValue(hPtr);
        if ((oldMeshPtr->flags & DELETED) == 0) {
            Tcl_AppendResult(interp, "mesh \"", meshObjPtr->name,
                "\" already exists", (char *)NULL);
            DestroyMeshObject(meshObjPtr);
            return TCL_ERROR;
        }
        oldMeshPtr->hashPtr = NULL;     /* Remove the mesh from the table. */
    }
    if ((meshObjPtr->classPtr->configProc)(interp, meshObjPtr) != TCL_OK) {
        DestroyMeshObject(meshObjPtr);
        return TCL_ERROR;
    }
    Tcl_SetStringObj(Tcl_GetObjResult(interp), meshObjPtr->name, -1);
    return TCL_OK;
}

static int
DeleteOp(ClientData clientData, Tcl_Interp *interp, int objc, 
         Tcl_Obj *const *objv)
{
    MeshCmdInterpData *dataPtr = clientData;
    int i;

    for (i = 2; i < objc; i++) {
        MeshObject *meshObjPtr;

        if (GetMeshObject(interp, dataPtr, objv[i], &meshObjPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        if (meshObjPtr->hashPtr != NULL) {
            MeshCmdInterpData *dataPtr;
            
            /* Remove the mesh from the hash table. */
            dataPtr = meshObjPtr->dataPtr;
            Blt_DeleteHashEntry(&dataPtr->meshTable, meshObjPtr->hashPtr);
            meshObjPtr->hashPtr = NULL;
        }
        Blt_FreeMesh(meshObjPtr);
    }
    return TCL_OK;
}

static int
HullOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    MeshObject *meshObjPtr;
    MeshCmdInterpData *dataPtr = clientData;
    Tcl_Obj *listObjPtr;
    int i;
    int wantPoints;

    if (GetMeshObject(interp, dataPtr, objv[2], &meshObjPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    wantPoints = FALSE;
    if (objc > 3) {
        const char *string;

        string = Tcl_GetString(objv[3]);
        if (strcmp(string, "-vertices") == 0) {
            wantPoints = TRUE;
        }
    }
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    if (wantPoints) {
        for (i = 0; i < meshObjPtr->numHullPts; i++) {
            Tcl_Obj *objPtr;
            Point2d *p;

            p = meshObjPtr->vertices + meshObjPtr->hull[i];
            objPtr = Tcl_NewDoubleObj(p->x);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
            objPtr = Tcl_NewDoubleObj(p->y);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
        }
    } else {
        for (i = 0; i < meshObjPtr->numHullPts; i++) {
            Tcl_Obj *objPtr;
            
            objPtr = Tcl_NewIntObj(meshObjPtr->hull[i]);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
        }
    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * NamesOp --
 *
 *      Returns a list of marker identifiers in interp->result;
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
static int
NamesOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    MeshCmdInterpData *dataPtr = clientData;
    Tcl_Obj *listObjPtr;

    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    if (objc == 2) {
        Blt_HashEntry *hPtr;
        Blt_HashSearch iter;

        for (hPtr = Blt_FirstHashEntry(&dataPtr->meshTable, &iter); 
             hPtr != NULL; hPtr = Blt_NextHashEntry(&iter)) {
            MeshObject *meshObjPtr;

            meshObjPtr = Blt_GetHashValue(hPtr);
            Tcl_ListObjAppendElement(interp, listObjPtr, 
                Tcl_NewStringObj(meshObjPtr->name, -1));
        }
    } else {
        Blt_HashEntry *hPtr;
        Blt_HashSearch iter;

        for (hPtr = Blt_FirstHashEntry(&dataPtr->meshTable, &iter); 
             hPtr != NULL; hPtr = Blt_NextHashEntry(&iter)) {
            MeshObject *meshObjPtr;
            int i;

            meshObjPtr = Blt_GetHashValue(hPtr);
            for (i = 2; i < objc; i++) {
                const char *pattern;

                pattern = Tcl_GetString(objv[i]);
                if (Tcl_StringMatch(meshObjPtr->name, pattern)) {
                    Tcl_ListObjAppendElement(interp, listObjPtr,
                        Tcl_NewStringObj(meshObjPtr->name, -1));
                    break;
                }
            }
        }
    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

static int
VerticesOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    MeshCmdInterpData *dataPtr = clientData;
    MeshObject *meshObjPtr;
    int i;
    Tcl_Obj *listObjPtr;

    if (GetMeshObject(interp, dataPtr, objv[2], &meshObjPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    for (i = 0; i <  meshObjPtr->numVertices; i++) {
        Tcl_Obj *subListObjPtr, *objPtr;
        Point2d *p;

        p = meshObjPtr->vertices + i;
        subListObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);

        objPtr = Tcl_NewIntObj(i);
        Tcl_ListObjAppendElement(interp, subListObjPtr, objPtr);
        objPtr = Tcl_NewDoubleObj(p->x);
        Tcl_ListObjAppendElement (interp, subListObjPtr, objPtr);
        objPtr = Tcl_NewDoubleObj(p->y);
        Tcl_ListObjAppendElement(interp, subListObjPtr, objPtr);

        Tcl_ListObjAppendElement(interp, listObjPtr, subListObjPtr);

    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

static int
TrianglesOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    MeshObject *meshObjPtr;
    MeshCmdInterpData *dataPtr = clientData;
    Tcl_Obj *listObjPtr;
    int i;

    if (GetMeshObject(interp, dataPtr, objv[2], &meshObjPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    for (i = 0; i < meshObjPtr->numTriangles; i++) {
        Tcl_Obj *objPtr;
        
        objPtr = ObjOfTriangle(interp, meshObjPtr->triangles + i);
        Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);

    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * TypeOp --
 *
 *      Returns the type of the mesh.
 *
 *---------------------------------------------------------------------------
 */
static int
TypeOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    MeshCmdInterpData *dataPtr = clientData;
    MeshObject *meshObjPtr;

    if (GetMeshObject(interp, dataPtr, objv[2], &meshObjPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    Tcl_SetStringObj(Tcl_GetObjResult(interp), meshObjPtr->classPtr->name, -1);
    return TCL_OK;
}

static int
HideOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    MeshCmdInterpData *dataPtr = clientData;
    MeshObject *meshObjPtr;
    int i;

    if (GetMeshObject(interp, dataPtr, objv[2], &meshObjPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (meshObjPtr->hideTable.numEntries > 0) {
        Blt_DeleteHashTable(&meshObjPtr->hideTable);
    }
    Blt_InitHashTable(&meshObjPtr->hideTable, BLT_ONE_WORD_KEYS);
    for (i = 3; i < objc; i++) {
        long index;
        Blt_HashEntry *hPtr;
        int isNew;

        if (Blt_GetCountFromObj(interp, objv[i], COUNT_NNEG, &index)!=TCL_OK) {
            return TCL_ERROR;
        }
        hPtr = Blt_CreateHashEntry(&meshObjPtr->hideTable,(intptr_t)index, &isNew);
        Blt_SetHashValue(hPtr, (intptr_t)index);
    }
    if (meshObjPtr->classPtr->type != MESH_TRIANGLE) {
        ComputeMesh(meshObjPtr);
    }
    NotifyClients(meshObjPtr, MESH_CHANGE_NOTIFY);
    return TCL_OK;
}

static void
DestroyMeshObjects(MeshCmdInterpData *dataPtr)
{
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;

    for (hPtr = Blt_FirstHashEntry(&dataPtr->meshTable, &iter); hPtr != NULL;
         hPtr = Blt_NextHashEntry(&iter)) {
        MeshObject *meshObjPtr;

        meshObjPtr = Blt_GetHashValue(hPtr);
        meshObjPtr->hashPtr = NULL;
        DestroyMeshObject(meshObjPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * MeshCmd --
 *
 *      blt::mesh create regular ?meshName? -x {x0 xN n} -y {y0 yN n}
 *      blt::mesh create irregular ?meshName? -x $xvalues -y $yvalues 
 *      blt::mesh create triangle ?meshName? -x x -y y -triangles $triangles
 *      blt::mesh create cloud ?meshName? -x x -y y 
 *      blt::mesh type meshName
 *      blt::mesh names pattern
 *      blt::mesh delete meshName
 *      blt::mesh configure meshName -hide no -linewidth 1 -color black
 *
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec meshOps[] =
{
    {"cget",        2, CgetOp,       3, 4,"meshName option"},
    {"configure",   2, ConfigureOp,  2, 0,"meshName ?option value ...?"},
    {"create",      2, CreateOp,     3, 0,"type ?meshName? ?option value ...?"},
    {"delete",      1, DeleteOp,     2, 0,"?meshName ...?",},
    {"hide",        2, HideOp,       3, 4,"meshName ?indices ...?",},
    {"hull",        2, HullOp,       3, 4,"meshName ?-vertices?",},
    {"names",       1, NamesOp,      2, 0,"?pattern ...?",},
    {"triangles",   2, TrianglesOp,  3, 3,"meshName",},
    {"type",        2, TypeOp,       3, 3,"meshName",},
    {"vertices",    1, VerticesOp,   3, 3,"meshName",},
};
static int numMeshOps = sizeof(meshOps) / sizeof(Blt_OpSpec);

static int
MeshCmd(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numMeshOps, meshOps, BLT_OP_ARG1, 
        objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}

/*
 *---------------------------------------------------------------------------
 *
 * MeshInterpDeleteProc --
 *
 *      This is called when the interpreter registering the "mesh" command
 *      is deleted.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      Removes the hash table managing all table names.
 *
 *---------------------------------------------------------------------------
 */
/* ARGSUSED */
static void
MeshInterpDeleteProc(ClientData clientData, Tcl_Interp *interp)
{
    MeshCmdInterpData *dataPtr = clientData;

    /* All table instances should already have been destroyed when their
     * respective TCL commands were deleted. */
    DestroyMeshObjects(dataPtr);
    Blt_DeleteHashTable(&dataPtr->meshTable);
    Tcl_DeleteAssocData(interp, MESH_THREAD_KEY);
    Blt_Free(dataPtr);
}

/*
 *
 * GetMeshCmdInterpData --
 *
 */
static MeshCmdInterpData *
GetMeshCmdInterpData(Tcl_Interp *interp)
{
    MeshCmdInterpData *dataPtr;
    Tcl_InterpDeleteProc *proc;

    dataPtr = (MeshCmdInterpData *)
        Tcl_GetAssocData(interp, MESH_THREAD_KEY, &proc);
    if (dataPtr == NULL) {
        dataPtr = Blt_AssertMalloc(sizeof(MeshCmdInterpData));
        dataPtr->interp = interp;
        Tcl_SetAssocData(interp, MESH_THREAD_KEY, MeshInterpDeleteProc, 
                dataPtr);
        Blt_InitHashTable(&dataPtr->meshTable, BLT_STRING_KEYS);
        dataPtr->nextMeshId = 0;
    }
    return dataPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_MeshCmdInitProc --
 *
 *      This procedure is invoked to initialize the "mesh" command.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      Creates the new command and adds a new entry into a global Tcl
 *      associative array.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_MeshCmdInitProc(Tcl_Interp *interp)
{
    static Blt_CmdSpec cmdSpec = { "mesh", MeshCmd, };

    cmdSpec.clientData = GetMeshCmdInterpData(interp);
    if (Blt_InitCmd(interp, "::blt", &cmdSpec) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}

/* Public routines. */

/*
 *---------------------------------------------------------------------------
 *
 * Blt_FreeMesh --
 *
 *      Destroys the mesh.
 *
 *---------------------------------------------------------------------------
 */
void
Blt_FreeMesh(Blt_Mesh mesh)
{
    MeshObject *meshObjPtr = mesh;

    if (meshObjPtr == NULL) {
        return;
    }
    meshObjPtr->refCount--;
    if (meshObjPtr->refCount <= 0) {
        if (meshObjPtr->hashPtr != NULL) {
            MeshCmdInterpData *dataPtr;
            
            /* Remove the mesh from the hash table. */
            dataPtr = meshObjPtr->dataPtr;
            Blt_DeleteHashEntry(&dataPtr->meshTable, meshObjPtr->hashPtr);
            meshObjPtr->hashPtr = NULL;
        }
        DestroyMeshObject(meshObjPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_GetMeshFromObj --
 *
 *      Returns the names mesh.  The mesh must already exist.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_GetMeshFromObj(Tcl_Interp *interp, Tcl_Obj *objPtr, Blt_Mesh *meshPtr)
{
    MeshObject *meshObjPtr;
    MeshCmdInterpData *dataPtr;

    dataPtr = GetMeshCmdInterpData(interp);
    if (GetMeshObject(interp, dataPtr, objPtr, &meshObjPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    meshObjPtr->refCount++;
    *meshPtr = meshObjPtr;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_GetMesh --
 *
 *      Returns the named mesh. The mesh must already exist.  
 *
 *---------------------------------------------------------------------------
 */
int
Blt_GetMesh(Tcl_Interp *interp, const char *string, Blt_Mesh *meshPtr)
{
    Blt_HashEntry *hPtr;
    MeshCmdInterpData *dataPtr;
    MeshObject *meshObjPtr;

    dataPtr = GetMeshCmdInterpData(interp);
    hPtr = Blt_FindHashEntry(&dataPtr->meshTable, string);
    if (hPtr == NULL) {
        Tcl_AppendResult(interp, "can't find a mesh \"", string, "\"", 
                (char *)NULL);
        return TCL_ERROR;
    }
    meshObjPtr = Blt_GetHashValue(hPtr);
    *meshPtr = meshObjPtr;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_Mesh_CreateNotifier --
 *
 *      Creates a notifier callback for the mesh.  Notifiers are uniquely
 *      recognized by their callback procedure and clientdata.
 *
 *---------------------------------------------------------------------------
 */
void
Blt_Mesh_CreateNotifier(Blt_Mesh mesh, Blt_MeshChangedProc *notifyProc, 
                       ClientData clientData)
{
    MeshObject *meshObjPtr = mesh;
    MeshNotifier *notifyPtr;
    Blt_ChainLink link;
    
    if (meshObjPtr->notifiers == NULL) {
        meshObjPtr->notifiers = Blt_Chain_Create();
    }
     for (link = Blt_Chain_FirstLink(meshObjPtr->notifiers); link != NULL;
        link = Blt_Chain_NextLink(link)) {
         MeshNotifier *notifyPtr;

         notifyPtr = Blt_Chain_GetValue(link);
         if ((notifyPtr->proc == notifyProc) &&
             (notifyPtr->clientData == clientData)) {
             notifyPtr->clientData = clientData;
             return;                    /* Notifier already exists. */
         }
     }
    link = Blt_Chain_AllocLink(sizeof(MeshNotifier));
    notifyPtr = Blt_Chain_GetValue(link);
    notifyPtr->proc = notifyProc;
    notifyPtr->clientData = clientData;
    Blt_Chain_LinkAfter(meshObjPtr->notifiers, link, NULL);
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_Mesh_DeleteNotifier --
 *
 *      Destroys the notifier callback for the mesh.
 *
 *---------------------------------------------------------------------------
 */
void
Blt_Mesh_DeleteNotifier(Blt_Mesh mesh, Blt_MeshChangedProc *notifyProc,
                        ClientData clientData)
{
    MeshObject *meshObjPtr = mesh;
    Blt_ChainLink link;
    
     for (link = Blt_Chain_FirstLink(meshObjPtr->notifiers); link != NULL;
          link = Blt_Chain_NextLink(link)) {
         MeshNotifier *notifyPtr;

         notifyPtr = Blt_Chain_GetValue(link);
         if ((notifyPtr->proc == notifyProc) &&
             (notifyPtr->clientData == clientData)) {
             Blt_Chain_DeleteLink(meshObjPtr->notifiers, link);
             return;
         }
     }
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_Mesh_Name --
 *
 *      Returns the name of the mesh.
 *
 *---------------------------------------------------------------------------
 */
const char *
Blt_Mesh_Name(Blt_Mesh mesh)
{
    MeshObject *meshObjPtr = mesh;

    return meshObjPtr->name;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_Mesh_Type --
 *
 *      Returns the type of the mesh.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_Mesh_Type(Blt_Mesh mesh)
{
    MeshObject *meshObjPtr = mesh;

    return meshObjPtr->classPtr->type;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_Mesh_GetVertices --
 *
 *      Returns the vertices of the mesh.  
 *
 *---------------------------------------------------------------------------
 */
Point2d *
Blt_Mesh_GetVertices(Blt_Mesh mesh, int *numVerticesPtr)
{
    MeshObject *meshObjPtr = mesh;

    *numVerticesPtr = meshObjPtr->numVertices;

    return meshObjPtr->vertices;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_Mesh_GetHull --
 *
 *      Returns the indices of the vertices the make up the convex
 *      hull of the mesh.
 *
 *---------------------------------------------------------------------------
 */
int *
Blt_Mesh_GetHull(Blt_Mesh mesh, int *numHullPtsPtr)
{
    MeshObject *meshObjPtr = mesh;

    *numHullPtsPtr = meshObjPtr->numHullPts;

    return meshObjPtr->hull;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_Mesh_GetExtents --
 *
 *      Returns the minimum and maximum dimensions of the mesh.
 *
 *---------------------------------------------------------------------------
 */
void
Blt_Mesh_GetExtents(Blt_Mesh mesh, float *x1Ptr, float *y1Ptr, float *x2Ptr,
                    float *y2Ptr)
{
    MeshObject *meshObjPtr = mesh;

    *x1Ptr = meshObjPtr->xMin;
    *x2Ptr = meshObjPtr->xMax;
    *y1Ptr = meshObjPtr->yMin;
    *y2Ptr = meshObjPtr->yMax;
}
    
/*
 *---------------------------------------------------------------------------
 *
 * Blt_Mesh_GetTriangles --
 *
 *      Returns the triangles of the mesh.
 *
 *---------------------------------------------------------------------------
 */
Blt_MeshTriangle *
Blt_Mesh_GetTriangles(Blt_Mesh mesh, int *numTrianglesPtr)
{
    MeshObject *meshObjPtr = mesh;

    *numTrianglesPtr = meshObjPtr->numTriangles;

    return meshObjPtr->triangles;
}

#ifdef notdef
/* 
 * RegularMeshFindProc 
 */
static Blt_MeshTriangle *
RegularMeshFindProc(Blt_Mesh mesh, double x, double y)
{
    MeshObject *meshObjPtr = mesh;
    double xStep, yStep;
    int xLoc, yLoc;

    if ((x < meshObjPtr->xMin) || (x > meshObjPtr->xMax) ||
        (y < meshObjPtr->yMin) || (y > meshObjPtr->yMax)) {
        return NULL;                    /* Point is outside mesh. */
    }
    xStep = (meshObjPtr->xMax - meshObjPtr->xMin) / 
        (double)(meshObjPtr->xNum - 1);
    yStep = (meshObjPtr->yMax - meshObjPtr->yMin) / 
        (double)(meshObjPtr->yNum - 1);

    xLoc = floor((x - meshObjPtr->xMin) / xStep);
    yLoc = floor((y - meshObjPtr->yMin) / yStep);
    t = meshObjPtr->triangles + 
        ((yLoc * meshObjPtr->xNum) + xLoc) * 2;
    if (PointInTriangle(t, x, y) {
       return t;
    }
    if (PointInTriangle(t+1, x, y)) {
       return t+1;
   } 
   return NULL;
}
#endif
