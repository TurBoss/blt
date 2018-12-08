/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 *
 * bltCompare.c --
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
#include <bltInt.h>

#ifndef NO_COMPARE

#ifdef HAVE_CTYPE_H
  #include <ctype.h>
#endif /* HAVE_CTYPE_H */

#include "bltAlloc.h"
#include "bltMath.h"
#include "bltString.h"
#include "bltSwitch.h"
#include "bltOp.h"
#include "bltInitCmd.h"

typedef int (StringCompareProc)(const char *s1, const char *s2, size_t len);

#define SORTED_NONE       (0)
#define SORTED_DECREASING (1)
#define SORTED_INCREASING (2)

#define NOCASE          (1<<0)
#define DICTIONARY      (1<<1)
#define ASCII           (1<<2)
#define TRIM_WHITESPACE_NONE       (0)
#define TRIM_WHITESPACE_LEFT       (1)
#define TRIM_WHITESPACE_RIGHT      (2)
#define TRIM_WHITESPACE_BOTH       (3)

typedef struct {
    int flags;
    int sorted;
} NumberSwitches;

typedef struct {
    int flags;
    int trim;
    int sorted;
} StringSwitches;

static Blt_SwitchParseProc SortedSwitchProc;
static Blt_SwitchCustom sortedSwitch = {
    SortedSwitchProc, NULL, NULL, 0,
};

static Blt_SwitchParseProc TrimSwitchProc;
static Blt_SwitchCustom trimSwitch = {
    TrimSwitchProc, NULL, NULL, 0,
};

static Blt_SwitchSpec isMemberNumberSwitches[] = 
{
    {BLT_SWITCH_CUSTOM,  "-sorted",  "sortDirection", (char *)NULL,
        Blt_Offset(NumberSwitches, sorted),  0, 0, &sortedSwitch},
    {BLT_SWITCH_END}
};

static Blt_SwitchSpec stringSwitches[] = 
{
    {BLT_SWITCH_BITS_NOARG, "-nocase", "", (char *)NULL,
        Blt_Offset(StringSwitches, flags), 0, NOCASE},
    {BLT_SWITCH_CUSTOM,  "-trimwhitespace",  "trimName", (char *)NULL,
        Blt_Offset(StringSwitches, trim),    0, 0, &trimSwitch},
    {BLT_SWITCH_END}
};

static Blt_SwitchSpec isBetweenStringSwitches[] = 
{
    {BLT_SWITCH_BITS_NOARG, "-nocase", "", (char *)NULL,
        Blt_Offset(StringSwitches, flags), 0, NOCASE},
    {BLT_SWITCH_END}
};

static Blt_SwitchSpec isMemberStringSwitches[] = 
{
    {BLT_SWITCH_BITS_NOARG, "-nocase", "", (char *)NULL,
        Blt_Offset(StringSwitches, flags), 0, NOCASE},
    {BLT_SWITCH_CUSTOM,  "-sorted",  "sortDirection", (char *)NULL,
        Blt_Offset(StringSwitches, sorted),  0, 0, &sortedSwitch},
    {BLT_SWITCH_CUSTOM,  "-trimwhitespace",  "trimName", (char *)NULL,
        Blt_Offset(StringSwitches, trim),    0, 0, &trimSwitch},
    {BLT_SWITCH_END}
};

/*
 *---------------------------------------------------------------------------
 *
 * SortedSwitch --
 *
 *      Convert a Tcl_Obj representing the sort direction.
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
SortedSwitchProc(ClientData clientData, Tcl_Interp *interp,
                 const char *switchName, Tcl_Obj *objPtr, char *record,
                 int offset, int flags)
{
    int *sortPtr = (int *)(record + offset);
    const char *string;
    char c;
    
    string = Tcl_GetString(objPtr);
    c = string[0];
    if ((c == 'd') && (strcmp(string, "decreasing") == 0)) {
        *sortPtr = SORTED_DECREASING;
    } else if ((c == 'i') && (strcmp(string, "increasing") == 0)) {
        *sortPtr = SORTED_INCREASING;
    } else if ((c == 'n') && (strcmp(string, "none") == 0)) {
        *sortPtr = SORTED_NONE;
    } else {
        Tcl_AppendResult(interp, "bad sorted value \"", string, 
                 "\": should be decreasing, increasing, or none",
                 (char *)NULL);  
        return TCL_ERROR;
    }                     
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * TrimSwitchProc --
 *
 *      Convert a Tcl_Obj representing the trim direction.
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
TrimSwitchProc(ClientData clientData, Tcl_Interp *interp,
               const char *switchName, Tcl_Obj *objPtr, char *record,
               int offset, int flags)
{
    int *trimPtr = (int *)(record + offset);
    const char *string;
    char c;
    
    string = Tcl_GetString(objPtr);
    c = string[0];
    if ((c == 'l') && (strcmp(string, "left") == 0)) {
        *trimPtr = TRIM_WHITESPACE_LEFT;
    } else if ((c == 'r') && (strcmp(string, "right") == 0)) {
        *trimPtr = SORTED_INCREASING;
    } else if ((c == 'b') && (strcmp(string, "both") == 0)) {
        *trimPtr = TRIM_WHITESPACE_BOTH;
    } else if ((c == 'n') && (strcmp(string, "none") == 0)) {
        *trimPtr = TRIM_WHITESPACE_NONE;
    } else {
        Tcl_AppendResult(interp, "bad trim value \"", string, 
                 "\": should be left, right, both, or none", (char *)NULL);      
        return TCL_ERROR;
    }                     
    return TCL_OK;
}

static const char *
TrimString(const char *s, int *lenPtr, int flags)
{
    int len;
    const char *p;

    len = *lenPtr;
    switch (flags) {
    case TRIM_WHITESPACE_LEFT:
        for (p = s; *p != '\0'; p++) {
            if (!isspace(*p)) {
                break;
            }
        }
        len -= p - s;
        s = p;
        break;
    case TRIM_WHITESPACE_RIGHT:
        for (p = s + len - 1; p > s; p--) {
            if (!isspace(*p)) {
                break;
            }
        }
        len = p - s + 1;
        break;
    case TRIM_WHITESPACE_BOTH:
        for (p = s; *p != '\0'; p++) {
            if (!isspace(*p)) {
                break;
            }
        }
        len -= p - s;
        s = p;
        for (p = s + len - 1; p > s; p--) {
            if (!isspace(*p)) {
                break;
            }
        }
        len = p - s + 1;
        break;
    case TRIM_WHITESPACE_NONE:
        break;
    }
    *lenPtr = len;
    return s;
}

static int
LinearNumberSearch(double value, int objc, Tcl_Obj **objv)
{
    int i;
    
    for (i = 0; i < objc; i++) {
        double x;
        
        if ((Blt_GetDoubleFromObj(NULL, objv[i], &x) == TCL_OK) &&
            (Blt_AlmostEquals(value, x))) {
            return TRUE;
        }
    }
    return FALSE;
}

static int
BinaryNumberSearchIncreasing(double value, int objc, Tcl_Obj **objv)
{
    int low, high;

    low = 0;
    high = objc - 1;
    while (low <= high) {
        int median;
        double x;
        
        median = (low + high) >> 1;
        if (Blt_GetDoubleFromObj(NULL, objv[median], &x) != TCL_OK) {
            return FALSE;
        }
        if (Blt_AlmostEquals(value, x)) {
            return TRUE;
        }
        if (value < x) {
            high = median - 1;
        } else if (value > x) {
            low = median + 1;
        }
    }
    return FALSE;                       /* Can't find number. */
}

static int
BinaryNumberSearchDecreasing(double value, int objc, Tcl_Obj **objv)
{
    int low, high;

    low = 0;
    high = objc - 1;
    while (low <= high) {
        int median;
        double x;
        
        median = (low + high) >> 1;
        if (Blt_GetDoubleFromObj(NULL, objv[median], &x) != TCL_OK) {
            return FALSE;
        }
        if (Blt_AlmostEquals(value, x)) {
            return TRUE;
        }
        if (value < x) {
            low = median + 1;
        } else if (value > x) {
            high = median - 1;
        }
    }
    return FALSE;                       /* Can't find number. */
}

static int
LinearStringSearch(const char *str1, int len1, int objc, Tcl_Obj **objv, 
                   int flags)
{
    int i;
    StringCompareProc *proc;

    if (flags & NOCASE) {
        proc = strncasecmp;
    } else {
        proc = strncmp;
    }
    for (i = 0; i < objc; i++) {
        const char *str2;
        int len2;

        str2 = Tcl_GetStringFromObj(objv[i], &len2);
        if ((len1 == len2) && ((*proc)(str1, str2, len1) == 0)) {
            return TRUE;
        }
    }
    return FALSE;
}

static int
BinaryStringSearchIncreasing(const char *str1, int len1, int objc, Tcl_Obj **objv, 
                     int flags)
{
    int low, high;
    StringCompareProc *proc;

    if (flags & NOCASE) {
        proc = strncasecmp;
    } else {
        proc = strncmp;
    }
    low = 0;
    high = objc - 1;
    while (low <= high) {
        int comp;
        int median;
        const char *str2;
        
        median = (low + high) >> 1;
        str2 = Tcl_GetString(objv[median]);
        comp = (*proc)(str1, str2, len1);
        if (comp == 0) {
            return TRUE;
        }
        if (comp < 0) {
            high = median - 1;
        } else if (comp > 0) {
            low = median + 1;
        }
    }
    return FALSE;                       /* Can't find number. */
}

static int
BinaryStringSearchDecreasing(const char *str1, int len1, int objc, Tcl_Obj **objv, 
                       int flags)
{
    int low, high;
    StringCompareProc *proc;

    if (flags & NOCASE) {
        proc = strncasecmp;
    } else {
        proc = strncmp;
    }
    low = 0;
    high = objc - 1;
    while (low <= high) {
        int comp;
        int median;
        const char *str2;
        
        median = (low + high) >> 1;
        str2 = Tcl_GetString(objv[median]);
        comp = (*proc)(str1, str2, len1);
        if (comp == 0) {
            return TRUE;
        }
        if (comp < 0) {
            low = median + 1;
        } else if (comp > 0) {
            high = median - 1;
        }
    }
    return FALSE;                       /* Can't find number. */
}

/*
 *---------------------------------------------------------------------------
 *
 * NumberIsBetweenOp --
 *
 *      blt::numberutils isbetween value first last
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
NumberIsBetweenOp(ClientData clientData, Tcl_Interp *interp, int objc,
                  Tcl_Obj *const *objv)
{
    double value, first, last;
    int state;

    if (Blt_GetDoubleFromObj(interp, objv[2], &value) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Blt_GetDoubleFromObj(interp, objv[3], &first) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Blt_GetDoubleFromObj(interp, objv[4], &last) != TCL_OK) {
        return TCL_ERROR;
    }
    state = FALSE;
    if ((Blt_AlmostEquals(value, first)) || 
        (Blt_AlmostEquals(value, last))) {
        state = TRUE;
    } else {
        state = ((value >= first) && (value <= last));
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * NumberEqualsOp --
 *
 *      blt::numberutils equals value1 value2 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
NumberEqualsOp(ClientData clientData, Tcl_Interp *interp, int objc,
               Tcl_Obj *const *objv)
{
    double value1, value2;
    int state;

    if (Blt_GetDoubleFromObj(interp, objv[2], &value1) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Blt_GetDoubleFromObj(interp, objv[3], &value2) != TCL_OK) {
        return TCL_ERROR;
    }
    state = Blt_AlmostEquals(value1, value2);
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * NumberGreaterThanOrEqualsOp --
 *
 *      blt::numberutils ge value1 value2 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
NumberGreaterThanOrEqualsOp(ClientData clientData, Tcl_Interp *interp, 
                            int objc, Tcl_Obj *const *objv)
{
    double value1, value2;
    int state;

    if (Blt_GetDoubleFromObj(interp, objv[2], &value1) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Blt_GetDoubleFromObj(interp, objv[3], &value2) != TCL_OK) {
        return TCL_ERROR;
    }
    state = Blt_AlmostEquals(value1, value2);
    if (!state) {
        state = (value1 > value2);
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * NumberGreaterThanOp --
 *
 *      blt::numberutils gt value1 value2 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
NumberGreaterThanOp(ClientData clientData, Tcl_Interp *interp, int objc,
                    Tcl_Obj *const *objv)
{
    double value1, value2;
    int state;

    if (Blt_GetDoubleFromObj(interp, objv[2], &value1) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Blt_GetDoubleFromObj(interp, objv[3], &value2) != TCL_OK) {
        return TCL_ERROR;
    }
    state = (value1 > value2);
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * NumberLessThanOrEqualsOp --
 *
 *      blt::numberutils le value1 value2 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
NumberLessThanOrEqualsOp(ClientData clientData, Tcl_Interp *interp, int objc,
                         Tcl_Obj *const *objv)
{
    double value1, value2;
    int state;

    if (Blt_GetDoubleFromObj(interp, objv[2], &value1) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Blt_GetDoubleFromObj(interp, objv[3], &value2) != TCL_OK) {
        return TCL_ERROR;
    }
    state = Blt_AlmostEquals(value1, value2);
    if (!state) {
        state = (value1 < value2);
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * NumberLessThanOp --
 *
 *      blt::numberutils lt value1 value2 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
NumberLessThanOp(ClientData clientData, Tcl_Interp *interp, int objc,
                 Tcl_Obj *const *objv)
{
    double value1, value2;
    int state;

    if (Blt_GetDoubleFromObj(interp, objv[2], &value1) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Blt_GetDoubleFromObj(interp, objv[3], &value2) != TCL_OK) {
        return TCL_ERROR;
    }
    state = (value1 < value2);
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * NumberIsMemberOp --
 *
 *      blt::numberutils ismember value numberList ?switches?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
NumberIsMemberOp(ClientData clientData, Tcl_Interp *interp, int objc,
                 Tcl_Obj *const *objv)
{
    NumberSwitches switches;
    Tcl_Obj **elv;
    double value;
    int elc, state;
    
    if (Blt_GetDoubleFromObj(interp, objv[2], &value) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Tcl_ListObjGetElements(interp, objv[3], &elc, &elv) != TCL_OK) {
        return TCL_ERROR;
    }
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, isMemberNumberSwitches, objc - 4, objv + 4,
                &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    state = FALSE;                      /* Suppress compiler warning. */
    switch (switches.sorted) {
    case SORTED_NONE:
        state = LinearNumberSearch(value, elc, elv);
        break;
    case SORTED_INCREASING:
        state = BinaryNumberSearchIncreasing(value, elc, elv);
        break;
    case SORTED_DECREASING:
        state = BinaryNumberSearchDecreasing(value, elc, elv);
        break;
    }
    Blt_FreeSwitches(isMemberNumberSwitches, (char *)&switches, 0);
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * NumberOp --
 *
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec numberOps[] =
{
    {"<",         1, NumberLessThanOp,     4, 4, "x y",},
    {"<=",        2, NumberLessThanOrEqualsOp, 4, 4, "x y",},
    {"==",        2, NumberEqualsOp,              4, 4, "x y",},
    {">",         1, NumberGreaterThanOp,         4, 4, "x y",},
    {">=",        2, NumberGreaterThanOrEqualsOp, 4, 4, "x y",},
    {"eq",        2, NumberEqualsOp,              4, 4, "x y",},
    {"ge",        2, NumberGreaterThanOrEqualsOp, 4, 4, "x y",},
    {"gt",        2, NumberGreaterThanOp,         4, 4, "x y",},
    {"isbetween", 3, NumberIsBetweenOp, 5, 5, "x first last",},
    {"ismember",  3, NumberIsMemberOp, 4, 0, "x numberList ?switches?",},
    {"le",        2, NumberLessThanOrEqualsOp, 4, 4, "x y",},
    {"lt",        2, NumberLessThanOp,     4, 4, "x y",},
};
int numNumberOps = sizeof(numberOps) / sizeof(Blt_OpSpec);

/*ARGSUSED*/
static int
NumberUtilsObjCmd(ClientData clientData, Tcl_Interp *interp, int objc,
                  Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;
    int result;

    proc = Blt_GetOpFromObj(interp, numNumberOps, numberOps, BLT_OP_ARG1,
                            objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    result = (*proc) (clientData, interp, objc, objv);
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * StringBeginsOp --
 *
 *      Returns if the given string begin with the pattern.
 *
 *      -nocase                 Ignore case.
 *      -trimwhitespace         Trim whitespace from the string.
 *
 *      blt::stringutils begins str pattern ?switches?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StringBeginsOp(ClientData clientData, Tcl_Interp *interp, int objc,
               Tcl_Obj *const *objv)
{
    const char *s, *pattern;
    int len1, len2;
    StringSwitches switches;
    int state;
    
    s = Tcl_GetStringFromObj(objv[2], &len1);
    pattern = Tcl_GetStringFromObj(objv[3], &len2);
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, stringSwitches, objc - 4, objv + 4,
                &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    s = TrimString(s, &len1, switches.trim);
    if (switches.flags & NOCASE) {
        state = (strncasecmp(s, pattern, len2) == 0);
    } else {
        state = (strncmp(s, pattern, len2) == 0);
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    Blt_FreeSwitches(stringSwitches, (char *)&switches, 0);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StringIsBetweenOp --
 *
 *      Returns if the given string is between the first and last 
 *      strings, either using a dictionary or ASCII comparsion.
 *
 *      -nocase         Ignore case.
 *      -dictionary     Use a dictionary comparsion.
 *      -ascii          Use an ASCII comparsion.
 *
 *      blt::stringutils isbetween str first last ?switches?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StringIsBetweenOp(ClientData clientData, Tcl_Interp *interp, int objc,
                  Tcl_Obj *const *objv)
{
    StringCompareProc *proc;
    StringSwitches switches;
    const char *s, *first, *last;
    int len1, len2, len3;
    int state, comp;
    
    s = Tcl_GetStringFromObj(objv[2], &len1);
    first = Tcl_GetStringFromObj(objv[3], &len2);
    last = Tcl_GetStringFromObj(objv[4], &len3);
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, isBetweenStringSwitches, objc - 5, objv + 5,
                &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    if (switches.flags & NOCASE) {
        proc = strncasecmp;
    } else {
        proc = strncmp;
    }
    comp = (*proc)(first, last, len2);
    if (comp < 0) {
        const char *tmp;

        tmp = first;
        first = last;
        last = tmp;
    }
    comp = (*proc)(s, first, len1);
    if (comp == 0) {
        state = TRUE;                   /* Equal to first. */
    } else if (comp > 0) {
        state = FALSE;                  /* Less than first. */
    } else {
        comp = (*proc)(s, last, len1);
        if (comp == 0) {
            state = TRUE;               /* Equal to last. */
        } else if (comp < 0) {
            state = FALSE;              /* Greater than last. */
        } else {
            state = TRUE;               /* Between first and last. */
        }
    }
    Blt_FreeSwitches(isBetweenStringSwitches, (char *)&switches, 0);
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StringContainsOp --
 *
 *      Returns if the pattern is contained within the given string.
 *
 *      -nocase         Ignore case of strings.
 *
 *      blt::stringutils contains str pattern ?switches?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StringContainsOp(ClientData clientData, Tcl_Interp *interp, int objc,
                 Tcl_Obj *const *objv)
{
    const char *s, *pattern;
    StringSwitches switches;
    int state;
    int len1, len2;
    
    s = Tcl_GetStringFromObj(objv[2], &len1);
    pattern = Tcl_GetStringFromObj(objv[3], &len2);
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, stringSwitches, objc - 4, objv + 4,
                &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    state = FALSE;
    if (len1 >= len2) {
        if (switches.flags & NOCASE) {
            state = (strcasestr(s, pattern) != NULL);
        } else {
            state = (strstr(s, pattern) != NULL);
        }
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    Blt_FreeSwitches(stringSwitches, (char *)&switches, 0);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StringDictCompareOp --
 *
 *      Performs a dictionary comparison of two strings.
 *
 *      blt::stringutils dictcompare string1 string2
 *
 *---------------------------------------------------------------------------
 */

/*ARGSUSED*/
static int
StringDictCompareOp(ClientData clientData, Tcl_Interp *interp, int objc,
                    Tcl_Obj *const *objv)
{
    int result;
    const char *s1, *s2;

    s1 = Tcl_GetString(objv[2]);
    s2 = Tcl_GetString(objv[3]);
    result = Blt_DictionaryCompare(s1, s2);
    Tcl_SetIntObj(Tcl_GetObjResult(interp), result);
    return TCL_OK;
}


/*
 *---------------------------------------------------------------------------
 *
 * StringEndsOp --
 *
 *      Returns if the given string ends with the pattern.
 *
 *      -nocase         Ignore case of strings.
 *      -trimwhitespace           Trim whitespace from the string.
 *
 *      blt::stringutils ends str pattern ?switches?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StringEndsOp(ClientData clientData, Tcl_Interp *interp, int objc,
             Tcl_Obj *const *objv)
{
    const char *s, *pattern;
    int len1, len2;
    StringSwitches switches;
    int state;
    
    s = Tcl_GetStringFromObj(objv[2], &len1);
    pattern = Tcl_GetStringFromObj(objv[3], &len2);
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, stringSwitches, objc - 4, objv + 4,
                &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    s = TrimString(s, &len1, switches.trim);
    state = FALSE;
    if (len1 >= len2) {
        if (switches.flags & NOCASE) {
            state = (strncasecmp(s + len1 - len2, pattern, len2) == 0);
        } else {
            state = (strncmp(s + len1 - len2, pattern, len2) == 0);
        }
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    Blt_FreeSwitches(stringSwitches, (char *)&switches, 0);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StringEqualsOp --
 *
 *      Returns if the two strings are equal.
 *
 *      -nocase                 Ignore case of strings.
 *      -trimwhitespace         Trim whitespace from the string.
 *
 *      blt::stringutils equals str1 str2 ?switches?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StringEqualsOp(ClientData clientData, Tcl_Interp *interp, int objc,
               Tcl_Obj *const *objv)
{
    const char *str1, *str2;
    int len1, len2;
    StringSwitches switches;
    int state;
    
    str1 = Tcl_GetStringFromObj(objv[2], &len1);
    str2 = Tcl_GetStringFromObj(objv[3], &len2);
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, stringSwitches, objc - 4, objv + 4,
                &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    str1 = TrimString(str1, &len1, switches.trim);
    state = FALSE;
    if (len1 == len2) {
        if (switches.flags & NOCASE) {
            state = (strncasecmp(str1, str2, len2) == 0);
        } else {
            state = (strncmp(str1, str2, len2) == 0);
        }
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    Blt_FreeSwitches(stringSwitches, (char *)&switches, 0);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StringIsMemberOp --
 *
 *      Returns if the string is a member of the given list.
 *
 *      -nocase                 Ignore case of strings.
 *      -trimwhitespace         Trim whitespace from the string.
 *
 *      blt::stringutils ismember str list ?switches?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StringIsMemberOp(ClientData clientData, Tcl_Interp *interp, int objc,
                 Tcl_Obj *const *objv)
{
    const char *s;
    Tcl_Obj **elv;
    int elc;
    int len;
    StringSwitches switches;
    int state;
    
    s = Tcl_GetStringFromObj(objv[2], &len);
    if (Tcl_ListObjGetElements(interp, objv[3], &elc, &elv) != TCL_OK) {
        return TCL_ERROR;
    }
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, isMemberStringSwitches, objc - 4, objv + 4,
                &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    s = TrimString(s, &len, switches.trim);
    state = FALSE;
    if (switches.flags & NOCASE) {
        switches.sorted = SORTED_NONE;
    }
    switch (switches.sorted) {
    case SORTED_INCREASING:
        state = BinaryStringSearchIncreasing(s, len, elc, elv, switches.flags);
        break;
    case SORTED_DECREASING:
        state = BinaryStringSearchDecreasing(s, len, elc, elv, switches.flags);
        break;
    case SORTED_NONE:
        state = LinearStringSearch(s, len, elc, elv, switches.flags);
        break;
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    Blt_FreeSwitches(isMemberStringSwitches, (char *)&switches, 0);
    return TCL_OK;
}

/*ARGSUSED*/
static Blt_OpSpec stringOps[] =
{
    {"begins",      2, StringBeginsOp,    4, 0, "str pattern ?switches?"},
    {"contains",    1, StringContainsOp,  4, 0, "str pattern ?switches?"},
    {"dictcompare", 1, StringDictCompareOp,  4, 4, "string1 string2"},
    {"ends",        2, StringEndsOp,      4, 0, "str pattern ?switches?"},
    {"equals",      2, StringEqualsOp,    4, 0, "str pattern ?switches?"},
    {"isbetween",   3, StringIsBetweenOp, 5, 0, "str first last ?switches?"},
    {"ismember",    3, StringIsMemberOp,  4, 0, "str list ?switches?"},
};

int numStringOps = sizeof(stringOps) / sizeof(Blt_OpSpec);

static int
StringUtilsObjCmd(ClientData clientData, Tcl_Interp *interp, int objc,
                  Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;
    int result;

    proc = Blt_GetOpFromObj(interp, numStringOps, stringOps, BLT_OP_ARG1,
                            objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    result = (*proc) (clientData, interp, objc, objv);
    return result;
}

/*ARGSUSED*/
static int
ExitCmd(
    ClientData clientData,      /* Not used. */
    Tcl_Interp *interp,
    int objc,                   /* Not used. */
    Tcl_Obj *const *objv)
{
    int code;

    if (Tcl_GetIntFromObj(interp, objv[1], &code) != TCL_OK) {
        return TCL_ERROR;
    }
#ifdef TCL_THREADS
    Tcl_Exit(code);
#else 
    exit(code);
#endif
    /*NOTREACHED*/
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_CompareCmdInitProc --
 *
 *      This procedure is invoked to initialize the "numberutils" and
 *      "stringutils" commands.
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
int
Blt_CompareCmdInitProc(Tcl_Interp *interp)
{
    static Blt_CmdSpec compareSpecs[] = { 
        { "numberutils", NumberUtilsObjCmd, },
        { "stringutils", StringUtilsObjCmd, },
        { "_exit", ExitCmd, }
    };
    if (Blt_InitCmds(interp, "::blt", compareSpecs, 3) != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}

#endif /* NO_COMPARE*/
