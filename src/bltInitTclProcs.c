/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#define BUILD_BLT_TCL_PROCS 1
#include "bltInt.h"
#include "bltTclProcs.h"

extern char *Blt_InitTclStubs(Tcl_Interp *interp, const char *version, 
        int wantExact);

Blt_TclProcs *bltTclProcsPtr = NULL;

const char *
Blt_InitTclStubs(Tcl_Interp *interp, const char *version, int wantExact)
{
    ClientData clientData;

    clientData = NULL;
    if (Tcl_PkgRequireEx(interp, "blt_tcl", version, wantExact, &clientData) 
        == NULL) {
        return NULL;
    }
    bltTclProcsPtr = clientData;
    bltTclIntProcsPtr = bltTclProcsPtr->hooks->bltTclIntProcs;
    return version;
}
