/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * bltParseArgs.c --
 *
 * Copyright 2018 George A. Howlett. All rights reserved.  
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
 * Limitations:
 *
 *      1. No "-long=value" syntax.  Only "-long value".  
 *
 *         I don't know how this works with -nargs > 1.  I don't typically
 *         use arguments with -nargs > 1 (such as "-origin 0 0").  But I
 *         don't typically use -long=value syntax either.
 *      
 *      2. No "-s0" or "-abcde".
 *
 *         Can't distinguish between -de (same as -d -e) and -debug with
 *         abbreviations.  Short switches can be longer than one character
 *         (like -bg).
 *
 */
#define BUILD_BLT_TCL_PROCS 1
#include <bltInt.h>

#ifndef NO_PARSEARGS

#ifdef HAVE_SYS_STAT_H
  #include <sys/stat.h>
#endif  /* HAVE_SYS_STAT_H */

#ifdef HAVE_CTYPE_H
  #include <ctype.h>
#endif /* HAVE_CTYPE_H */

#include "bltAlloc.h"
#include "bltMath.h"
#include "bltString.h"
#include <bltHash.h>
#include <bltList.h>
#include "bltNsUtil.h"
#include "bltSwitch.h"
#include "bltOp.h"
#include "bltInitCmd.h"
#include "tclInterp.h"

#define PARSER_THREAD_KEY "BLT ParseArgs Command Data"
#define PARSER_MAGIC ((unsigned int) 0x46170277)


#define DEF_ABBREVIATIONS           "0"
#define DEF_ARG_ALLOW_PREFIX_CHARS  "0"
#define DEF_ARG_ACTION              "store"
#define DEF_ARG_ARGUMENT            (char *)NULL
#define DEF_ARG_CHOICES             (char *)NULL
#define DEF_ARG_COMMAND             (char *)NULL
#define DEF_ARG_CURRENT             (char *)NULL
#define DEF_ARG_DEFAULT             (char *)NULL             
#define DEF_ARG_EXCLUDE             (char *)NULL
#define DEF_ARG_HELP                (char *)NULL
#define DEF_ARG_LONG_NAME           (char *)NULL
#define DEF_ARG_MAX                 (char *)NULL
#define DEF_ARG_METAVAR             (char *)NULL
#define DEF_ARG_MIN                 (char *)NULL
#define DEF_ARG_NARGS               "1"
#define DEF_ARG_NO_DEFAULT          "0"
#define DEF_ARG_REQUIRED            "0"
#define DEF_ARG_SHORT_NAME          (char *)NULL
#define DEF_ARG_TYPE                "string"
#define DEF_ARG_VALUE               (char *)NULL
#define DEF_ARG_VARIABLE            (char *)NULL
#define DEF_DEFAULT                 ""
#define DEF_DESCRIPTION             (char *)NULL
#define DEF_EPILOG                  (char *)NULL
#define DEF_ERROR                   "badoption"
#define DEF_HELP                    (char *)NULL
#define DEF_NO_DEFAULT_VALUE        "0"
#define DEF_PREFIX_CHARS            "-+"
#define DEF_PROGRAM_NAME            (char *)NULL
#define DEF_ARG_STATE               "normal"
#define DEF_USAGE                   (char *)NULL
#define DEF_VARIABLE                (char *)NULL
#define DEF_USE_QUESTION_MARK       "0"

typedef struct {
    Tcl_Interp *interp;
    Blt_HashTable parserTable;		/* Hash table of parsers keyed by
                                         * address. */
} ParseArgsCmdInterpData;

typedef struct _ArgType {
    const char *name;
    unsigned int mask;
} ArgType;

#define NARGS_ZERO_OR_ONE       (-1)    /* 0 or 1 argument is required. */
#define NARGS_ZERO_OR_MORE      (-2)    /* 0+ arguments are required. */
#define NARGS_ONE_OR_MORE       (-3)    /* 1+ arguments are required. */
#define NARGS_LAST_SWITCH       (-4)    /* When found on the command line,
                                         * the following arguments are not
                                         * interpretered as switches, even
                                         * if the words look like
                                         * switches. */
/* Parser bit fields */
#define ABBREVIATIONS         (1<<1)    /* Allow abbreviations of short and
                                         * long names. */
#define ERROR_ON_EXTRA_ARGS   (1<<3)    /* Generate an error if extra
                                         * arguments are leftover. */
#define ERROR_ON_BAD_SWITCHES (1<<4)    /* Generate an error is invalid
                                         * switches are found. */
#define ERROR_MASK            (ERROR_ON_EXTRA_ARGS|ERROR_ON_BAD_SWITCHES)
#define EXCLUSIONS            (1<<5)    /* Some arguments have exclusions. */
#define UPDATE_VARIABLES      (1<<6)    /* Some arguments have TCL
                                         * variables to be set with the new
                                         * value. */
#define USE_QUESTION_MARK     (1<<7)    /* Use TCL-style ?...? to indicate
                                         * optional arguments instead
                                         * of [...].*/
/* Argument bit fields */
#define TYPE_STRING           (1<<0)    /* Value is a string. */
#define TYPE_INT              (1<<1)    /* Value is an integer. */
#define TYPE_DOUBLE           (1<<2)    /* Value is a real number. */
#define TYPE_BOOLEAN          (1<<3)    /* Value is a boolean. */
#define TYPE_MASK             (TYPE_STRING|TYPE_INT|TYPE_DOUBLE|TYPE_BOOLEAN)

#define ACTION_STORE          (1<<10)   /* Store the value. */
#define ACTION_APPEND         (1<<11)   /* Append the value */
#define ACTION_STORE_FALSE    (1<<12)   /* Store a FALSE value. */
#define ACTION_STORE_TRUE     (1<<13)   /* Store a TRUE value.  */
#define ACTION_HELP           (1<<14)   /* Return help message.  */
#define ACTION_MASK           (ACTION_STORE|ACTION_APPEND|ACTION_STORE_FALSE|\
                               ACTION_STORE_TRUE|ACTION_HELP)

#define ARG_STATE_NORMAL      (0)       /* Normal state. */
#define ARG_STATE_HIDDEN      (1<<15)   /* Hidden argument. */
#define ARG_STATE_DISABLED    (1<<16)   /* Disabled argument. */
#define ARG_STATE_MASK        (ARG_STATE_HIDDEN|ARG_STATE_DISABLED)

#define MODIFIED              (1<<20)   /* Argument was set. */
#define REQUIRED              (1<<21)   /* Argument is required. */
#define NODEFAULT             (1<<22)   /* No default argument. */
#define ALLOW_PREFIX_CHARS    (1<<23)   /* The values of this argument may
                                         * start with prefix chars that
                                         * normally distinguish them from
                                         * options. */
typedef struct {
    unsigned int flags;
    Tcl_Interp *interp;                 /* Interpreter associated with this
                                         * parser. */
    ParseArgsCmdInterpData *dataPtr;    /* Points to global data managing
                                         * argument parsers.*/
    const char *name;                   /* Name of the parser: either
                                         * generated or provided by the
                                         * user. */
    Blt_HashEntry *hashPtr;             /* Pointer to entry in global hash
                                         * table for argument parsers.  *
                                         * The entry is keyed by the name
                                         * of the parser. */
    Tcl_Command cmdToken;               /* Token for parser's TCL command. */
    Blt_HashTable argTable;		/* Table of arguments. Arguments
					 * are keyed by their name. */
    const char *progName;
    const char *usage;
    const char *epilog;
    const char *desc;
    const char *prefixChars;
    Tcl_Obj *defValueObjPtr;		/* Global default value for
                                         * arguments in the parser. This
                                         * can be overridden by the
                                         * argument's default value. */
    Blt_Chain args;			/* Linked list of arguments. */
} Parser;

static Blt_SwitchParseProc ObjToError;
static Blt_SwitchPrintProc ErrorToObj;
static Blt_SwitchCustom errorSwitch = {
    ObjToError, ErrorToObj, NULL, (ClientData)0,
};

static Blt_SwitchSpec cmdSpecs[] = 
{
    {BLT_SWITCH_BITS, "-abbreviations", "bool", DEF_ABBREVIATIONS,
        Blt_Offset(Parser, flags), BLT_SWITCH_DONT_SET_DEFAULT, ABBREVIATIONS},
    {BLT_SWITCH_OBJ, "-default", "string", DEF_DEFAULT,
        Blt_Offset(Parser, defValueObjPtr), 0},
    {BLT_SWITCH_STRING, "-description", "string", DEF_DESCRIPTION,
        Blt_Offset(Parser, desc), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_STRING, "-epilog", "string", DEF_EPILOG,
        Blt_Offset(Parser, epilog), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_CUSTOM, "-error", "errorList", DEF_ERROR,
        Blt_Offset(Parser, flags), BLT_SWITCH_NULL_OK, 0, &errorSwitch},
    {BLT_SWITCH_STRING, "-prefixchars", "string", DEF_PREFIX_CHARS,
        Blt_Offset(Parser, prefixChars), 0},
    {BLT_SWITCH_STRING, "-program", "programName", DEF_PROGRAM_NAME,
        Blt_Offset(Parser, progName), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_STRING, "-usage", "string", DEF_USAGE,
        Blt_Offset(Parser, usage), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_BITS, "-usequestionmark", "bool", DEF_USE_QUESTION_MARK,
        Blt_Offset(Parser, flags), BLT_SWITCH_DONT_SET_DEFAULT, 
        USE_QUESTION_MARK},
    {BLT_SWITCH_END}
};

typedef struct _Argument Argument;

struct _Argument {
    const char *name;			/* Name of the argument. */
    Blt_HashEntry *hashPtr;
    Blt_ChainLink link;
    Parser *parserPtr;                  /* Parser this argument belongs
                                         * to. */
    const char *metaName;		/* Meta variable name. */
    unsigned int flags;
    Tcl_Obj *varNameObjPtr;		/* Name of TCL variable to set with
					 * the arguments value. */
    int numTokens;			/* # of words required. */
    const char *shortName;
    const char *longName;
    Tcl_Obj *defValueObjPtr;		/* Default argument value. String
                                         * does not have to be the correct
                                         * type for the argument. This is
                                         * also used has the defacto value
                                         * for arguments that take no
                                         * values (-nargs 0). */
    Tcl_Obj *valueObjPtr;
    Blt_Chain values;
    Tcl_Obj *minObjPtr, *maxObjPtr;
    int numArgs;
    const char *metaVar;
    const char *help;
    Tcl_Obj *cmdObjPtr;                 /* If non-NULL, a prefix of a TCL
                                         * command to be invoked before the
                                         * argument is set. */
    Tcl_Obj *choicesObjPtr;
    Tcl_Obj *excludeObjPtr;             /* If non-NULL, a TCL list of
                                         * argument names that are mutually
                                         * exclusive with this
                                         * argument.  */
    Tcl_Obj *currentObjPtr;             /* If non-NULL, this is the current
                                         * value of the argument. */
    Argument *destPtr;                  /* If non-NULL, this is the
                                         * argument where to store the
                                         * current value. */
};

static Blt_SwitchParseProc ObjToAction;
static Blt_SwitchPrintProc ActionToObj;
static Blt_SwitchCustom actionSwitch = {
    ObjToAction, ActionToObj, NULL, (ClientData)0,
};

static Blt_SwitchParseProc ObjToDestination;
static Blt_SwitchPrintProc DestinationToObj;
static Blt_SwitchCustom destinationSwitch = {
    ObjToDestination, DestinationToObj, NULL, (ClientData)0,
};

static Blt_SwitchParseProc ObjToNumArgs;
static Blt_SwitchPrintProc NumArgsToObj;
static Blt_SwitchCustom numArgsSwitch = {
    ObjToNumArgs, NumArgsToObj, NULL, (ClientData)0,
};

static Blt_SwitchParseProc ObjToNumber;
static Blt_SwitchPrintProc NumberToObj;
static Blt_SwitchFreeProc FreeNumber;
static Blt_SwitchCustom numberSwitch = {
    ObjToNumber, NumberToObj, FreeNumber, (ClientData)0,
};

static Blt_SwitchParseProc ObjToType;
static Blt_SwitchPrintProc TypeToObj;
static Blt_SwitchCustom typeSwitch = {
    ObjToType, TypeToObj, NULL, (ClientData)0,
};

static Blt_SwitchParseProc ObjToName;
static Blt_SwitchPrintProc NameToObj;
static Blt_SwitchFreeProc FreeName;
static Blt_SwitchCustom longSwitch = {
    ObjToName, NameToObj, FreeName, (ClientData)"long",
};
static Blt_SwitchCustom shortSwitch = {
    ObjToName, NameToObj, FreeName, (ClientData)"short",
};
static Blt_SwitchParseProc ObjToState;
static Blt_SwitchPrintProc StateToObj;
static Blt_SwitchCustom stateSwitch = {
    ObjToState, StateToObj, NULL, (ClientData)0,
};

static Blt_SwitchSpec argSpecs[] = 
{
    {BLT_SWITCH_CUSTOM, "-action", "actionName", DEF_ARG_ACTION,
        Blt_Offset(Argument, flags), BLT_SWITCH_DONT_SET_DEFAULT, 0,
        &actionSwitch},
    {BLT_SWITCH_BITS, "-allowprefixchars", "bool", DEF_ARG_ALLOW_PREFIX_CHARS,
        Blt_Offset(Argument, flags), BLT_SWITCH_DONT_SET_DEFAULT,
        ALLOW_PREFIX_CHARS},
    {BLT_SWITCH_OBJ,    "-command",  "cmdPrefix", DEF_ARG_COMMAND,
        Blt_Offset(Argument, cmdObjPtr), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_OBJ,   "-choices",  "choiceList", DEF_ARG_CHOICES,
        Blt_Offset(Argument, choicesObjPtr), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_OBJ,   "-current",  "value", DEF_ARG_CURRENT,
        Blt_Offset(Argument, currentObjPtr), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_OBJ, "-default", "string", DEF_ARG_DEFAULT,
        Blt_Offset(Argument, defValueObjPtr), 0},
    {BLT_SWITCH_CUSTOM, "-destination", "argName", DEF_ARG_ARGUMENT,
        Blt_Offset(Argument, destPtr), BLT_SWITCH_NULL_OK, 0,
        &destinationSwitch},
    {BLT_SWITCH_OBJ, "-exclude", "excludeList", DEF_ARG_EXCLUDE,
        Blt_Offset(Argument, excludeObjPtr), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_STRING, "-help", "string", DEF_ARG_HELP, 
        Blt_Offset(Argument, help), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_CUSTOM, "-long", "longName", DEF_ARG_LONG_NAME,
        Blt_Offset(Argument, longName), BLT_SWITCH_NULL_OK, 0, &longSwitch},
    {BLT_SWITCH_STRING, "-metavar", "string", DEF_ARG_METAVAR, 
        Blt_Offset(Argument, metaVar), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_CUSTOM, "-max", "maxValue", DEF_ARG_MAX, 
        Blt_Offset(Argument, maxObjPtr),
        BLT_SWITCH_NULL_OK | BLT_SWITCH_DONT_SET_DEFAULT, 0, &numberSwitch},
    {BLT_SWITCH_CUSTOM, "-min", "minValue", DEF_ARG_MIN, 
        Blt_Offset(Argument, minObjPtr),
        BLT_SWITCH_NULL_OK | BLT_SWITCH_DONT_SET_DEFAULT, 0, &numberSwitch},
    {BLT_SWITCH_CUSTOM, "-nargs", "number", DEF_ARG_NARGS,
        Blt_Offset(Argument, numArgs), BLT_SWITCH_DONT_SET_DEFAULT, 0,
        &numArgsSwitch},
    {BLT_SWITCH_BITS, "-nodefault", "bool", DEF_ARG_NO_DEFAULT, 
        Blt_Offset(Argument, flags), BLT_SWITCH_DONT_SET_DEFAULT, 
        NODEFAULT},
    {BLT_SWITCH_BITS, "-required", "bool", DEF_ARG_REQUIRED,
        Blt_Offset(Argument, flags), BLT_SWITCH_DONT_SET_DEFAULT, REQUIRED},
    {BLT_SWITCH_CUSTOM, "-short", "shortName", DEF_ARG_SHORT_NAME,
        Blt_Offset(Argument, shortName), BLT_SWITCH_NULL_OK, 0, &shortSwitch},
    {BLT_SWITCH_CUSTOM, "-state", "stateName", DEF_ARG_STATE,
        Blt_Offset(Argument, flags), BLT_SWITCH_DONT_SET_DEFAULT, 0,
        &stateSwitch},
    {BLT_SWITCH_CUSTOM, "-type", "typeName", DEF_ARG_TYPE,
        Blt_Offset(Argument, flags), BLT_SWITCH_DONT_SET_DEFAULT, 0,
        &typeSwitch},
    {BLT_SWITCH_OBJ,    "-value", "value", DEF_ARG_VALUE,
        Blt_Offset(Argument, valueObjPtr), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_OBJ,    "-variable", "varName", DEF_ARG_VARIABLE,
        Blt_Offset(Argument, varNameObjPtr), BLT_SWITCH_NULL_OK},
    {BLT_SWITCH_END}
};

static Tcl_InterpDeleteProc ParseArgsInterpDeleteProc;
static Tcl_CmdDeleteProc ParserInstDeleteProc;

static Tcl_ObjCmdProc ParseArgsCmd;
static Tcl_ObjCmdProc ParserInstObjCmd;

INLINE static
const char *
SwitchName(Argument *argPtr)
{
    if (argPtr->longName != NULL) {
        return argPtr->longName;
    }
    if (argPtr->shortName != NULL) {
        return argPtr->shortName;
    }
    return argPtr->name;
}

INLINE static Tcl_Obj *
DefaultValue(Argument *argPtr)
{
    if (argPtr->flags & NODEFAULT) {
        return NULL;
    }
    if (argPtr->defValueObjPtr != NULL) {
        return argPtr->defValueObjPtr;
    }
    return argPtr->parserPtr->defValueObjPtr;
}

static void
ProgramName(Tcl_Interp *interp, Parser *parserPtr, Tcl_DString *resultPtr)
{
    Interp *iPtr = (Interp *)interp;
    char *p;
    const char *string;
    int length;
    
    if (parserPtr->progName != NULL) {
        Tcl_DStringAppend(resultPtr, parserPtr->progName, -1);
        return;
    }
    if (iPtr->scriptFile != NULL) {
        string = Tcl_GetStringFromObj(iPtr->scriptFile, &length);
    } else {
        string = Tcl_GetNameOfExecutable();
        length = strlen(string);
    }
    /* Get the tail of the path. */
    p = strrchr(string, '/');
    if (p != NULL) {
        string = p + 1;
        length = strlen(string);
    }
    /* Chop off the extension. */
    p = strrchr(string, '.');
    if (p != NULL) {
        length = p - string;
    }                
    Tcl_DStringAppend(resultPtr, string, length);
}

/*
 *---------------------------------------------------------------------------
 *
 * FindSwitch --
 *
 *      Searches for the given switch in the parser.  Switches are
 *      distinguished by having either a short or long name that
 *      starts with a prefix character. 
 *
 *---------------------------------------------------------------------------
 */
static int
FindSwitch(Tcl_Interp *interp, Parser *parserPtr, Tcl_Obj *objPtr,
           Argument **argPtrPtr)
{
    int length;
    const char *string;
    Blt_ChainLink link;
    Blt_Chain matches;
    Argument *argPtr;
    int numMatches;
    char c;
    
    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    *argPtrPtr = NULL;
    if (strchr(parserPtr->prefixChars, c) == NULL) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "unknown option \"", string, "\"", 
                (char *)NULL);
        }
        return 0;
    }
    matches = Blt_Chain_Create();
    /* Loop through the argument list, storing matches. */
    for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Argument *argPtr;

        argPtr = Blt_Chain_GetValue(link);
        if ((argPtr->shortName == NULL) && (argPtr->longName == NULL)) {
            continue;                   /* Positional argument. */
        }
        if (argPtr->shortName != NULL) {
            int result;

            result = strcmp(string, argPtr->shortName);
            if (result == 0) {
                /* Short name matches exactly. Use it. */
                *argPtrPtr = argPtr;
                Blt_Chain_Destroy(matches);
                return 1;
            }
        }
        if (argPtr->longName != NULL) {
            int result;

            if (parserPtr->flags & ABBREVIATIONS) {
                result = strncmp(string, argPtr->longName, length);
            } else {
                result = strcmp(string, argPtr->longName);
            }
            if (result == 0) {
                Blt_Chain_Append(matches, argPtr);
                continue;
            }
        }
    }
    numMatches = Blt_Chain_GetLength(matches);
    if (numMatches == 1) {
        link = Blt_Chain_FirstLink(matches);
        *argPtrPtr = Blt_Chain_GetValue(link);
        Blt_Chain_Destroy(matches);
        return 1;
    }
    if (numMatches == 0) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "unknown option \"", string, "\"", 
                (char *)NULL);
        }
        Blt_Chain_Destroy(matches);
        return 0;
    }
    if (interp != NULL) {
        Tcl_AppendResult(interp, "switch \"", string, "\":  is ambiguous: "
                         "matches ", (char *)NULL);
        for (link = Blt_Chain_FirstLink(matches); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            argPtr = Blt_Chain_GetValue(link);
            Tcl_AppendResult(interp, argPtr->longName, " ", (char *)NULL);
        }
    }
    Blt_Chain_Destroy(matches);
    return numMatches;
}

static int
GetArgumentFromObj(Tcl_Interp *interp, Parser *parserPtr, Tcl_Obj *objPtr,
                   Argument **argPtrPtr)
{
    Blt_HashEntry *hPtr;
    const char *string;

    string = Tcl_GetString(objPtr);
    hPtr = Blt_FindHashEntry(&parserPtr->argTable, string);
    if (hPtr == NULL) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "can't find argument \"", string,
                             "\" in parser \"", parserPtr->name, "\"",
                             (char *)NULL);
        }
        return TCL_ERROR;
    }
    *argPtrPtr = Blt_GetHashValue(hPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToAction --
 *
 *      Convert a Tcl_Obj representing an argument action to its bit
 *      value.
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToAction(ClientData clientData, Tcl_Interp *interp, const char *switchName,
            Tcl_Obj *objPtr, char *record, int offset,  int flags)
{
    unsigned int *flagsPtr = (unsigned int *)(record + offset);
    int flag;
    const char *string;
    char c;
    int length;

    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if ((c == 's') && (length == 5) &&
        (strncmp(string, "store", length) == 0)) {
        flag = ACTION_STORE;
    } else if ((c == 'a') && (strncmp(string, "append", length) == 0)) {
        flag = ACTION_APPEND;
    } else if ((c == 's') && (length > 6) &&
               (strncmp(string, "store_false", length) == 0)) {
        flag = ACTION_STORE_FALSE;
    } else if ((c == 's') && (length > 6) &&
               (strncmp(string, "store_true", length) == 0)) {
        flag = ACTION_STORE_TRUE;
    } else if ((c == 'h') && (strncmp(string, "help", length) == 0)) {
        flag = ACTION_HELP;
    } else {
        Tcl_AppendResult(interp, "unknown action \"", string, "\": ",
             "should be store, append, store_false, store_true, or help",
             (char *)NULL);
        return TCL_ERROR;
    }
    *flagsPtr &= ~ACTION_MASK;
    *flagsPtr |= flag;
    return TCL_OK;
}

static Tcl_Obj *
ActionToObj(ClientData clientData, Tcl_Interp *interp, char *record, int offset,
            int flags)
{
    unsigned int *flagsPtr = (unsigned int *)(record + offset);
    const char *string;

    switch (*flagsPtr & ACTION_MASK) {
    case ACTION_STORE:
        string = "store";               break;
    case ACTION_STORE_FALSE:
        string = "store_false";         break;
    case ACTION_STORE_TRUE:
        string = "store_true";          break;
    case ACTION_APPEND:
        string = "append";              break;
    case ACTION_HELP:
        string = "help";                break;
    default:
        string = "???";                 break;
    }
    return Tcl_NewStringObj(string, -1);
}
        
/*
 *---------------------------------------------------------------------------
 *
 * ObjToDestination --
 *
 *      Convert a Tcl_Obj representing a destination argument.
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToDestination(ClientData clientData, Tcl_Interp *interp, 
                 const char *switchName, Tcl_Obj *objPtr, char *record, 
                 int offset, int flags)
{
    Argument *argPtr, *destPtr;
    Parser *parserPtr;
    int length;

    argPtr = (Argument *)record;
    parserPtr = argPtr->parserPtr;
    Tcl_GetStringFromObj(objPtr, &length);
    if (length == 0) {
        destPtr = NULL;
    } else {
        if (GetArgumentFromObj(interp, parserPtr, objPtr, &destPtr) != TCL_OK) {
            return TCL_ERROR;
        } 
    }
    /* Free the current value if one exists. */
    if (argPtr->currentObjPtr != NULL) {
        Tcl_DecrRefCount(argPtr->currentObjPtr);
        argPtr->currentObjPtr = NULL;
    }
    /* If the destination argument is this argument, set it to NULL. */
    argPtr->destPtr = (argPtr == destPtr) ? NULL : destPtr;
    return TCL_OK;
}

static Tcl_Obj *
DestinationToObj(ClientData clientData, Tcl_Interp *interp, char *record, 
                 int offset, int flags)
{
    Argument *destPtr = *(Argument **)(record + offset);
    Argument *argPtr;
    Tcl_Obj *objPtr;

    argPtr = (Argument *)record;
    if (destPtr == NULL) {
        objPtr = Tcl_NewStringObj(argPtr->name, -1);
    } else {
        objPtr = Tcl_NewStringObj(destPtr->name, -1);
    }
    return objPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToError --
 *
 *      Convert a Tcl_Obj representing an argument error flag to its bit
 *      value.
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToError(ClientData clientData, Tcl_Interp *interp, const char *switchName,
            Tcl_Obj *objPtr, char *record, int offset,  int flags)
{
    unsigned int *flagsPtr = (unsigned int *)(record + offset);
    Tcl_Obj **objv;
    int objc;
    int mask;
    int i;
    
    if (Tcl_ListObjGetElements(interp, objPtr, &objc, &objv) != TCL_OK) {
        return TCL_ERROR;
    }
    mask = 0;
    for (i = 0; i < objc; i++) {
        const char *string;
        char c;
        int length;
        
        string = Tcl_GetStringFromObj(objv[i], &length);
        c = string[0];

        if ((c == 'b') && (strncmp(string, "badoption", length) == 0)) {
            mask |= ERROR_ON_BAD_SWITCHES;
        } else if ((c == 'e') && (strncmp(string, "extraargs", length) == 0)) {
            mask |= ERROR_ON_EXTRA_ARGS;
        } else {
            Tcl_AppendResult(interp, "unknown error flag \"", string, "\": ",
             "should be badoption or extraargs", (char *)NULL);
            return TCL_ERROR;
        }
    }
    *flagsPtr &= ~ERROR_MASK;
    *flagsPtr |= mask;
    return TCL_OK;
}

static Tcl_Obj *
ErrorToObj(ClientData clientData, Tcl_Interp *interp, char *record, int offset,
          int flags)
{
    unsigned int *flagsPtr = (unsigned int *)(record + offset);
    const char *string;
    
    switch (*flagsPtr & ERROR_MASK) {
    case ERROR_ON_BAD_SWITCHES:
        string = "badoption";           break;
    case ERROR_ON_EXTRA_ARGS:
        string = "extraargs";           break;
    case 0:
        string = "";                    break;
    default:
        string = "???";                 break;
    }
    return Tcl_NewStringObj(string, -1);
}
        
/*
 *---------------------------------------------------------------------------
 *
 * ObjToNumArgs --
 *
 *      Convert a Tcl_Obj representing an argument action to its bit value.
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToNumArgs(ClientData clientData, Tcl_Interp *interp, const char *switchName,
            Tcl_Obj *objPtr, char *record, int offset,  int flags)
{
    int *numArgsPtr = (int *)(record + offset);
    Argument *argPtr = (Argument *)record;
    const char *string;
    char c;
    int length;
    int numArgs;
    
    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if ((c == '?') && (length == 1)) {
        numArgs = NARGS_ZERO_OR_ONE;
    } else if ((c == '*') && (length == 1)) {
        numArgs = NARGS_ZERO_OR_MORE;
    } else if ((c == '+') && (length == 1)) {
        numArgs = NARGS_ONE_OR_MORE;
    } else if ((c == 'l') && (strncmp(string, "last", length) == 0)) {
        numArgs = NARGS_LAST_SWITCH;
    } else if (isdigit(c)) {
        long l;
        
        if (Blt_GetCountFromObj(interp, objPtr, COUNT_NNEG, &l) != TCL_OK) {
            Tcl_AppendResult(interp, ": bad nargs value for \"", 
                             SwitchName(argPtr), "\"", (char *)NULL);
            return TCL_ERROR;
        }
        numArgs = l;
    } else {
        Tcl_AppendResult(interp, "invalid nargs \"", string,
                "\": should be +, ?, *, \"last\" or number", (char *)NULL);
        return TCL_ERROR;
    }
    *numArgsPtr = numArgs;
    return TCL_OK;
}

static Tcl_Obj *
NumArgsToObj(ClientData clientData, Tcl_Interp *interp, char *record,
             int offset, int flags)
{
    int numArgs = *(int *)(record + offset);
    Tcl_Obj *objPtr;
    
    switch (numArgs) {
    case NARGS_ZERO_OR_ONE:     objPtr = Tcl_NewStringObj("?", 1);   break;
    case NARGS_ZERO_OR_MORE:    objPtr = Tcl_NewStringObj("*", 1);   break;
    case NARGS_ONE_OR_MORE:     objPtr = Tcl_NewStringObj("+", 1);   break;
    case NARGS_LAST_SWITCH:     objPtr = Tcl_NewStringObj("last", 4); break;
    default:
        objPtr = Tcl_NewIntObj(numArgs);
    }
    return objPtr;
}
        
static void
FreeNumber(ClientData clientData, char *record, int offset, int flags)
{
    Tcl_Obj **objPtrPtr = (Tcl_Obj **)(record + offset);

    if (*objPtrPtr != NULL) {
        Tcl_DecrRefCount(*objPtrPtr);
        *objPtrPtr = NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToNumber --
 *
 *      Try to convert a Tcl_Obj representing an argument number to a double
 *      precision value. 
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToNumber(ClientData clientData, Tcl_Interp *interp, const char *switchName,
            Tcl_Obj *objPtr, char *record, int offset,  int flags)
{
    Tcl_Obj **objPtrPtr = (Tcl_Obj **)(record + offset);
    double d;
    int length;
    Tcl_Obj *newObjPtr;

    Tcl_GetStringFromObj(objPtr, &length);
    if (length == 0) {
        newObjPtr = NULL;
    } else {
        if (Blt_GetDoubleFromObj(interp, objPtr, &d) != TCL_OK) {
            return TCL_ERROR;
        }
        Tcl_IncrRefCount(objPtr);
        newObjPtr = objPtr;
    }
    if (*objPtrPtr != NULL) {
        Tcl_DecrRefCount(*objPtrPtr);
    }
    *objPtrPtr = newObjPtr;
    return TCL_OK;
}

static Tcl_Obj *
NumberToObj(ClientData clientData, Tcl_Interp *interp, char *record,
             int offset, int flags)
{
    Tcl_Obj *objPtr = *(Tcl_Obj **)(record + offset);

    if (objPtr == NULL) {
        return Tcl_NewStringObj("", -1);
    }
    return objPtr;
}
        

static void
FreeName(ClientData clientData, char *record, int offset, int flags)
{
    const char **namePtr = (const char **)(record + offset);

    if (*namePtr != NULL) {
        Blt_Free(*namePtr);
        *namePtr = NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToName --
 *
 *      Convert a Tcl_Obj representing an argument long or short name.
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToName(ClientData clientData, Tcl_Interp *interp, const char *switchName,
          Tcl_Obj *objPtr, char *record, int offset,  int flags)
{
    Argument *argPtr = (Argument *)record;
    const char *string;
    int length;
    const char **namePtr = (const char **)(record + offset);
    
    string = Tcl_GetStringFromObj(objPtr, &length);
    if (length == 0) {
        FreeName(clientData, record, offset, flags);
    } else {
        Parser *parserPtr;
        int numMatches;
        Argument *aPtr;
        
        parserPtr = argPtr->parserPtr;

        if (strchr(parserPtr->prefixChars, string[0]) == NULL) {
            Tcl_AppendResult(interp, clientData, " name \"", string,
               "\" must start with one the following prefix characters \"",
               parserPtr->prefixChars, "\"", (char *)NULL);
            return TCL_ERROR;
        }
        if ((string[0] == '-') && (isdigit(string[1]))) {
            Tcl_AppendResult(interp, clientData, " name \"", string,
               "\": first character after prefix \"",
               parserPtr->prefixChars, "\" can not be a digit", (char *)NULL);
            return TCL_ERROR;
        }
            
        numMatches = FindSwitch(NULL, parserPtr, objPtr, &aPtr);
        if ((numMatches > 0) && (aPtr != argPtr)) {
            Tcl_AppendResult(interp, clientData, " name \"",
                Tcl_GetString(objPtr),
                "\" for argument \"", argPtr->name, "\" already matches \"",
                aPtr->name, "\"",(char *)NULL);
            return TCL_ERROR;
        }
        FreeName(clientData, record, offset, flags);
        *namePtr = Blt_AssertStrdup(Tcl_GetString(objPtr));
    }
    return TCL_OK;
}

static Tcl_Obj *
NameToObj(ClientData clientData, Tcl_Interp *interp, char *record, int offset,
          int flags)
{
    const char *name = *(const char **)(record + offset);

    return Tcl_NewStringObj(name, -1);
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToState --
 *
 *      Convert a Tcl_Obj representing an argument action to its bit
 *      value.
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToState(ClientData clientData, Tcl_Interp *interp, const char *switchName,
            Tcl_Obj *objPtr, char *record, int offset,  int flags)
{
    unsigned int *flagsPtr = (unsigned int *)(record + offset);
    int flag;
    const char *string;
    char c;
    int length;

    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if ((c == 'n') && (strncmp(string, "normal", length) == 0)) {
        flag = ARG_STATE_NORMAL;
    } else if ((c == 'h') && (strncmp(string, "hidden", length) == 0)) {
        flag = ARG_STATE_HIDDEN;
    } else if ((c == 'd') && (strncmp(string, "disabled", length) == 0)) {
        flag = ARG_STATE_DISABLED;
    } else {
        Tcl_AppendResult(interp, "unknown state \"", string, "\": ",
             "should be normal, hidden, or disabled",
             (char *)NULL);
        return TCL_ERROR;
    }
    *flagsPtr &= ~ARG_STATE_MASK;
    *flagsPtr |= flag;
    return TCL_OK;
}

static Tcl_Obj *
StateToObj(ClientData clientData, Tcl_Interp *interp, char *record, int offset,
            int flags)
{
    unsigned int *flagsPtr = (unsigned int *)(record + offset);
    const char *string;

    switch (*flagsPtr & ARG_STATE_MASK) {
    case ARG_STATE_NORMAL:
        string = "normal";              break;
    case ARG_STATE_HIDDEN:
        string = "hidden";              break;
    case ARG_STATE_DISABLED:
        string = "disabled";            break;
    default:
        string = "???";                 break;
    }
    return Tcl_NewStringObj(string, -1);
}
        
static const char *
NameOfType(unsigned int flags)
{
    const char *string;
    
    switch (flags & TYPE_MASK) {
    case TYPE_INT:
        string = "integer";             break;
    case TYPE_DOUBLE:
        string = "double";              break;
    case TYPE_STRING:
        string = "string";              break;
    case TYPE_BOOLEAN:
        string = "boolean";             break;
    default:
        string = "???";                 break;
    }
    return string;
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToType --
 *
 *      Convert a Tcl_Obj representing an argument type to its bit
 *      value.
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToType(ClientData clientData, Tcl_Interp *interp, const char *switchName,
          Tcl_Obj *objPtr, char *record, int offset,  int flags)
{
    unsigned int *flagsPtr = (unsigned int *)(record + offset);
    int flag;
    const char *string;
    char c;
    int length;

    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if ((c == 'i') && (length > 2) && 
        (strncmp(string, "integer", length) == 0)) {
        flag = TYPE_INT;
    } else if ((c == 'f') && (strncmp(string, "float", length) == 0)) {
        flag = TYPE_DOUBLE;
    } else if ((c == 'd') && (strncmp(string, "double", length) == 0)) {
        flag = TYPE_DOUBLE;
    } else if ((c == 'n') && (strncmp(string, "number", length) == 0)) {
        flag = TYPE_DOUBLE;
    } else if ((c == 's') && (strncmp(string, "string", length) == 0)) {
        flag = TYPE_STRING;
    } else if ((c == 'b') && (strncmp(string, "boolean", length) == 0)) {
        flag = TYPE_BOOLEAN;
    } else {
        Tcl_AppendResult(interp, "unknown argument type \"", string, "\": ",
                         "should be integer, double, string, or boolean",
                         (char *)NULL);
        return TCL_ERROR;
    }
    *flagsPtr &= ~TYPE_MASK;
    *flagsPtr |= flag;
    return TCL_OK;
}

static Tcl_Obj *
TypeToObj(ClientData clientData, Tcl_Interp *interp, char *record, int offset,
          int flags)
{
    unsigned int *flagsPtr = (unsigned int *)(record + offset);

    return Tcl_NewStringObj(NameOfType(*flagsPtr), -1);
}

/*
 *---------------------------------------------------------------------------
 *
 * ParseArgsInterpDeleteProc --
 *
 *      This is called when the interpreter hosting the "parseargs" command
 *      is deleted.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Removes the hash table managing all argument parsers.
 *
 *---------------------------------------------------------------------------
 */
/* ARGSUSED */
static void
ParseArgsInterpDeleteProc(ClientData clientData, Tcl_Interp *interp)
{
    ParseArgsCmdInterpData *dataPtr = clientData;

    /* 
     * All parser instances should already have been destroyed when their
     * respective TCL commands were deleted.
     */
    Blt_DeleteHashTable(&dataPtr->parserTable);
    Tcl_DeleteAssocData(interp, PARSER_THREAD_KEY);
    Blt_Free(dataPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * GetParseArgsCmdInterpData --
 *
 *---------------------------------------------------------------------------
 */
static ParseArgsCmdInterpData *
GetParseArgsCmdInterpData(Tcl_Interp *interp)
{
    ParseArgsCmdInterpData *dataPtr;
    Tcl_InterpDeleteProc *proc;

    dataPtr = (ParseArgsCmdInterpData *)
        Tcl_GetAssocData(interp, PARSER_THREAD_KEY, &proc);
    if (dataPtr == NULL) {
        dataPtr = Blt_AssertMalloc(sizeof(ParseArgsCmdInterpData));
        dataPtr->interp = interp;
        Tcl_SetAssocData(interp, PARSER_THREAD_KEY, ParseArgsInterpDeleteProc,
                 dataPtr);
        Blt_InitHashTable(&dataPtr->parserTable, BLT_STRING_KEYS);
    }
    return dataPtr;
}

static const char *
ArgValue(Argument *argPtr)
{
    if (argPtr->metaVar != NULL) {
        return argPtr->metaVar;
    }
    return NameOfType(argPtr->flags);
}

static Argument *
NewArgument(Tcl_Interp *interp, Parser *parserPtr, Blt_HashEntry *hPtr)
{
    Argument *argPtr;

    argPtr = Blt_AssertCalloc(1, sizeof(Argument));
    argPtr->parserPtr = parserPtr;
    argPtr->hashPtr = hPtr;
    argPtr->flags = ACTION_STORE | TYPE_STRING;
    argPtr->numArgs = 1;
    argPtr->name = Blt_GetHashKey(&parserPtr->argTable, hPtr);
    argPtr->link = Blt_Chain_Append(parserPtr->args, argPtr);
    Blt_SetHashValue(hPtr, argPtr);
    return argPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * DestroyArgument --
 *
 *      Removes the given argument from the parser.
 *
 *---------------------------------------------------------------------------
 */
static void
DestroyArgument(Argument *argPtr)
{
    Parser *parserPtr = argPtr->parserPtr;
    Blt_ChainLink link;

    Blt_FreeSwitches(argSpecs, (char *)argPtr, 0);
    if (argPtr->hashPtr != NULL) {
        Blt_DeleteHashEntry(&parserPtr->argTable, argPtr->hashPtr);
    }
    if (argPtr->link != NULL) {
        /* Remove references to the this argument from other arguments. */
        for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *otherArgPtr;
            
            otherArgPtr = Blt_Chain_GetValue(link);
            if (otherArgPtr->destPtr == argPtr) {
                otherArgPtr->destPtr = NULL;
            }
        }
        Blt_Chain_DeleteLink(parserPtr->args, argPtr->link);
    }
    /* Free the current value if one exists. */
    if (argPtr->currentObjPtr != NULL) {
        Tcl_DecrRefCount(argPtr->currentObjPtr);
        argPtr->currentObjPtr = NULL;
    }
    Blt_Free(argPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * DestroyArguments --
 *
 *      Removes all arguments from the parser.  Used when destroying the
 *      parser.
 *
 *---------------------------------------------------------------------------
 */
static void
DestroyArguments(Parser *parserPtr)
{
    Blt_ChainLink link;
    
    for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Argument *argPtr;

        argPtr = Blt_Chain_GetValue(link);
        argPtr->link = NULL;
        argPtr->hashPtr = NULL;
        DestroyArgument(argPtr);
    }
    Blt_Chain_Destroy(parserPtr->args);
    Blt_DeleteHashTable(&parserPtr->argTable);
}

/*
 *---------------------------------------------------------------------------
 *
 * DestroyParser --
 *
 *      Frees the memory associated with the parser and remove it from the
 *      global hash table of parsers.  This includes the list of arguments.
 *
 *---------------------------------------------------------------------------
 */
static void
DestroyParser(Parser *parserPtr)
{
    Blt_FreeSwitches(cmdSpecs, (char *)parserPtr, 0);
    DestroyArguments(parserPtr);
    if (parserPtr->hashPtr != NULL) {
        Blt_DeleteHashEntry(&parserPtr->dataPtr->parserTable,
                            parserPtr->hashPtr);
    }
    Blt_Free(parserPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * ParserExists --
 *
 *      Returns whether a parser by the given name exists in the global
 *      hash table of parsers.
 *
 *---------------------------------------------------------------------------
 */
static int
ParserExists(Tcl_Interp *interp, const char *name)
{
    ParseArgsCmdInterpData *dataPtr;
    Blt_HashEntry *hPtr;
    
    dataPtr = GetParseArgsCmdInterpData(interp);
    hPtr = Blt_FindHashEntry(&dataPtr->parserTable, name);
    return (hPtr != NULL);
}

static int 
IsChoice(Tcl_Interp *interp, Argument *argPtr, Tcl_Obj *objPtr)
{
    int objc;
    Tcl_Obj **objv;
    
    if (Tcl_ListObjGetElements(interp, argPtr->choicesObjPtr, &objc, &objv)
        != TCL_OK) {
        return TCL_ERROR;
    }
    switch (argPtr->flags & TYPE_MASK) {
    case TYPE_STRING:
        {
            const char *string;
            int i;
            
            string = Tcl_GetString(objPtr);
            for (i = 0; i < objc; i++) {
                if (strcmp(string, Tcl_GetString(objv[i])) == 0) {
                    return TCL_OK;
                }
            }
        }        
        break;

    case TYPE_INT:
        {
            long lval;
            int i;
            
            if (Blt_GetLongFromObj(interp, objPtr, &lval) != TCL_OK) {
                return TCL_ERROR;
            }
            for (i = 0; i < objc; i++) {
                long choice;
                
                if (Blt_GetLongFromObj(NULL, objv[i], &choice) != TCL_OK) {
                    return TCL_ERROR;
                }
                if (lval == choice) {
                    return TCL_OK;
                }
            }
        }
        break;

    case TYPE_DOUBLE:
        {
            double d;
            int i;
            
            if (Blt_GetDoubleFromObj(interp, objPtr, &d) != TCL_OK) {
                return TCL_ERROR;
            }
            for (i = 0; i < objc; i++) {
                double choice;
                
                if (Blt_GetDoubleFromObj(NULL, objv[i], &choice) != TCL_OK) {
                    return TCL_ERROR;
                }
                if (Blt_AlmostEquals(d,choice)) {
                    return TCL_OK;
                }
            }
        }
        break;
                
    case TYPE_BOOLEAN:
        return TCL_OK;
    }
    Tcl_AppendResult(interp, "bad value \"", Tcl_GetString(objPtr), "\"",
        ", must be one of \"", Tcl_GetString(argPtr->choicesObjPtr), "\"",
        (char *)NULL);
    return TCL_ERROR;
}

static int 
InRange(Tcl_Interp *interp, Argument *argPtr, Tcl_Obj *objPtr)
{
    switch (argPtr->flags & TYPE_MASK) {
    case TYPE_STRING:
    case TYPE_BOOLEAN:
        break;
        
    case TYPE_INT:
        {
            long lval, min, max;
            
            if (Blt_GetLongFromObj(interp, objPtr, &lval) != TCL_OK) {
                return TCL_ERROR;
            }
            if ((argPtr->minObjPtr != NULL) &&
                (Blt_GetLongFromObj(NULL, argPtr->minObjPtr, &min) == TCL_OK)) {
                if (lval < min) {
                    if (interp != NULL) {
                        Tcl_AppendResult(interp, "value \"",
                                         Tcl_GetString(objPtr),
                                         "\" is less than minimum \"",
                                         Tcl_GetString(argPtr->minObjPtr),
                                         "\"", (char *)NULL);
                    }
                    return TCL_ERROR;
                }
            }
            if ((argPtr->maxObjPtr != NULL) && 
                (Blt_GetLongFromObj(NULL, argPtr->maxObjPtr, &max) == TCL_OK)) {
                if (lval > max) {
                    if (interp != NULL) {
                        Tcl_AppendResult(interp, "value \"",
                                         Tcl_GetString(objPtr),
                                         "\" is greater than maximium \"",
                                         Tcl_GetString(argPtr->maxObjPtr),
                                         "\"", (char *)NULL);
                    }
                    return TCL_ERROR;
                }
            }
        }
        break;

    case TYPE_DOUBLE:
        {
            double d, min, max;
            
            if (Blt_GetDoubleFromObj(interp, objPtr, &d) != TCL_OK) {
                return TCL_ERROR;
            }
            if ((argPtr->minObjPtr != NULL) &&
                (Blt_GetDoubleFromObj(NULL, argPtr->minObjPtr, &min)==TCL_OK)) {
                if (d < min) {
                    if (interp != NULL) {
                        Tcl_AppendResult(interp, "value \"",
                                         Tcl_GetString(objPtr),
                                         "\" is less than minimum \"",
                                         Tcl_GetString(argPtr->minObjPtr),
                                         "\"", (char *)NULL);
                    }
                    return TCL_ERROR;
                }
            }
            if ((argPtr->maxObjPtr != NULL) &&
                (Blt_GetDoubleFromObj(NULL, argPtr->maxObjPtr, &max)==TCL_OK)) {
                if (d > max) {
                    if (interp != NULL) {
                        Tcl_AppendResult(interp, "value \"",
                                         Tcl_GetString(objPtr),
                                         "\" is greater than maximium \"",
                                         Tcl_GetString(argPtr->maxObjPtr),
                                         "\"", (char *)NULL);
                    }
                    return TCL_ERROR;
                }
            }
        }
        break;
    }
    return TCL_OK;
}

static int 
IsValue(Tcl_Interp *interp, Argument *argPtr, Tcl_Obj *objPtr)
{
    switch (argPtr->flags & TYPE_MASK) {
    case TYPE_STRING:
        break;
        
    case TYPE_INT:
        {
            long lval;
            
            if (Blt_GetLongFromObj(interp, objPtr, &lval) != TCL_OK) {
                return TCL_ERROR;
            }
        }
        break;

    case TYPE_DOUBLE:
        {
            double d;
            
            if (Blt_GetDoubleFromObj(interp, objPtr, &d) != TCL_OK) {
                return TCL_ERROR;
            }
        }
        break;
                
    case TYPE_BOOLEAN:
        {
            int ival;
            
            if (Tcl_GetBooleanFromObj(interp, objPtr, &ival) != TCL_OK) {
                return TCL_ERROR;
            }
        }
        break;
    }
    return TCL_OK;
}

static int 
CheckValue(Tcl_Interp *interp, Argument *argPtr, Tcl_Obj *objPtr)
{
    if (IsValue(interp, argPtr, objPtr) != TCL_OK) {
        goto error;
    }
    if (argPtr->choicesObjPtr != NULL) {
        if (IsChoice(interp, argPtr, objPtr) != TCL_OK) {
            goto error;
        }
    }
    if (argPtr->flags & (TYPE_INT|TYPE_DOUBLE)) {
        if (InRange(interp, argPtr, objPtr) != TCL_OK) {
            goto error;
        }
    }
    return TCL_OK;
 error:
    Tcl_AppendResult(interp, ": bad value for \"", SwitchName(argPtr), "\"", 
                     (char *)NULL);
    return TCL_ERROR;
}

/* 
 * ResetArgument --
 *
 *      Reset the current value of the argument to NULL.  
 *
 */
static int
ResetArgument(Tcl_Interp *interp, Argument *argPtr)
{
    if (argPtr->currentObjPtr != NULL) {
        Tcl_DecrRefCount(argPtr->currentObjPtr);
        argPtr->currentObjPtr = NULL;
    }
    return TCL_OK;
}

static int
ResetArguments(Tcl_Interp *interp, Parser *parserPtr)
{
    Blt_ChainLink link;
    
    for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Argument *argPtr;

        argPtr = Blt_Chain_GetValue(link);
        if (ResetArgument(interp, argPtr) != TCL_OK) {
            return TCL_ERROR;
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * LookLikeSwitch --
 *
 *      Indicates if the given word looks like a switch.  This is used to
 *      detect bad switches and guess whether something is a switch or a
 *      value.
 *
 *      Examples:
 *                "-1"    value. Digits not allowed after prefix char.
 *               "-1d"    value. Digits not allowed after prefix char.
 *              "-1x2"    value. Digits not allowed after prefix char.
 *              "--1d"   switch. Double prefix chars are always a switch.
 *              "-bad"   switch. Detect misspelled or invalid switches.
 *               "abc"    value. No prefix char.
 *             "-good"   switch. Detect possible valid switches.
 *             "- a -"    value. Space after prefix char.
 *              
 *---------------------------------------------------------------------------
 */
static int
LooksLikeSwitch(Parser *parserPtr, Tcl_Obj *objPtr)
{
    const char *string;
    int length;
    
    string = Tcl_GetStringFromObj(objPtr, &length);
    if (length == 0) {
        return FALSE;
    }
    if (strchr(parserPtr->prefixChars, string[0]) == NULL) {
        return FALSE;
    }
    if (length == 1) {
        return FALSE;
    }
    if ((string[0] == '-') && (isdigit(string[1]))) {
        return FALSE;
    }
    if ((string[0] == '-') && (isspace(string[1]))) {
        return FALSE;
    }
    return TRUE;
}

static Tcl_Obj *
InvokeCommand(Tcl_Interp *interp, Argument *argPtr, Tcl_Obj *objPtr)
{
    int result;
    Tcl_Obj *cmdObjPtr;
    
    cmdObjPtr = Tcl_DuplicateObj(argPtr->cmdObjPtr);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    Tcl_IncrRefCount(cmdObjPtr);
    result = Tcl_EvalObjEx(interp, cmdObjPtr, TCL_EVAL_GLOBAL);
    Tcl_DecrRefCount(cmdObjPtr);
    if (result != TCL_OK) {
        return NULL;
    }
    objPtr = Tcl_GetObjResult(interp);
    objPtr = Tcl_DuplicateObj(objPtr);
    Tcl_ResetResult(interp);
    return objPtr;
}

static void
StoreValue(Argument *argPtr, Tcl_Obj *objPtr)
{
    Argument *destPtr;

    destPtr = (argPtr->destPtr != NULL) ? argPtr->destPtr : argPtr;
    if (argPtr->flags & ACTION_STORE) {
        if (objPtr != NULL) {
            Tcl_IncrRefCount(objPtr);
        }
        if (destPtr->currentObjPtr != NULL) {
            Tcl_DecrRefCount(destPtr->currentObjPtr);
            destPtr->currentObjPtr = NULL;
        }
        destPtr->currentObjPtr = objPtr;
    } else if (argPtr->flags & ACTION_APPEND) {
        if (destPtr->currentObjPtr == NULL) {
            destPtr->currentObjPtr = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
            Tcl_IncrRefCount(destPtr->currentObjPtr);
        }
        if (Tcl_IsShared(destPtr->currentObjPtr)) {
            Tcl_DecrRefCount(destPtr->currentObjPtr);
            destPtr->currentObjPtr = Tcl_DuplicateObj(destPtr->currentObjPtr);
            Tcl_IncrRefCount(destPtr->currentObjPtr);
        }
        Tcl_ListObjAppendElement(NULL, destPtr->currentObjPtr, objPtr);
    } else if (argPtr->flags & ACTION_STORE_FALSE) {
        objPtr = Tcl_NewBooleanObj(FALSE);
        Tcl_IncrRefCount(objPtr);
        if (destPtr->currentObjPtr != NULL) {
            Tcl_DecrRefCount(destPtr->currentObjPtr);
            destPtr->currentObjPtr = NULL;
        }
        destPtr->currentObjPtr = objPtr;
    } else if (argPtr->flags & ACTION_STORE_TRUE) {
        objPtr = Tcl_NewBooleanObj(TRUE);
        Tcl_IncrRefCount(objPtr);
        if (destPtr->currentObjPtr != NULL) {
            Tcl_DecrRefCount(destPtr->currentObjPtr);
            destPtr->currentObjPtr = NULL;
        }
        destPtr->currentObjPtr = objPtr;
    }
    argPtr->flags |= MODIFIED;
}


static int 
SetValues(Tcl_Interp *interp, Argument *argPtr, Blt_Chain chain)
{
    Tcl_Obj *listObjPtr;
    Blt_ChainLink link;
    
    /* Handle the one element list as a single value. */
    if (Blt_Chain_GetLength(chain) == 1) {
        Tcl_Obj *objPtr;
        
        link = Blt_Chain_FirstLink(chain);
        objPtr = Blt_Chain_GetValue(link);
        if (CheckValue(interp, argPtr, objPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        StoreValue(argPtr, objPtr);
        return TCL_OK;
    }
    /* Create list of values. */
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
    for (link = Blt_Chain_FirstLink(chain); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Tcl_Obj *objPtr;
        
        objPtr = Blt_Chain_GetValue(link);
        if (CheckValue(interp, argPtr, objPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    }
    StoreValue(argPtr, listObjPtr);
    return TCL_OK;
}

static int
SetValue(Tcl_Interp *interp, Argument *argPtr, Tcl_Obj *objPtr)
{
    if (argPtr->cmdObjPtr != NULL) {
        objPtr = InvokeCommand(interp, argPtr, objPtr);
        if (objPtr == NULL) {
            return TCL_ERROR;
        }
    }
    if ((objPtr != DefaultValue(argPtr)) &&
        (CheckValue(interp, argPtr, objPtr) != TCL_OK)) {
        return TCL_ERROR;
    }
    StoreValue(argPtr, objPtr);
    return TCL_OK;
}

static int 
SetDefaultValue(Tcl_Interp *interp, Argument *argPtr)
{
    if (argPtr->valueObjPtr != NULL) {
        return SetValue(interp, argPtr, argPtr->valueObjPtr);
    }
    return SetValue(interp, argPtr, DefaultValue(argPtr));
}

static int
AddValue(Tcl_Interp *interp, Argument *argPtr, Blt_ChainLink link,
           Blt_Chain chain, Blt_Chain found)
{
    Tcl_Obj *objPtr;

    objPtr = Blt_Chain_GetValue(link);
    if (argPtr->cmdObjPtr != NULL) {
        objPtr = InvokeCommand(interp, argPtr, objPtr);
        if (objPtr == NULL) {
            return TCL_ERROR;
        }
        Blt_Chain_SetValue(link, objPtr);
    }
    if (CheckValue(interp, argPtr, objPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    Blt_Chain_UnlinkLink(chain, link);
    Blt_Chain_LinkAfter(found, link, NULL);
    return TCL_OK;
}

static void
PrintUsageArg(Argument *argPtr, Blt_DBuffer argbuf)
{
    const char *string;
    
    Blt_DBuffer_Format(argbuf, " ");
    if ((argPtr->flags & REQUIRED) == 0) {
        if (argPtr->parserPtr->flags & USE_QUESTION_MARK) {
            Blt_DBuffer_Format(argbuf, "?");
        } else {
            Blt_DBuffer_Format(argbuf, "[");
        }
    }
    if ((argPtr->shortName == NULL) && (argPtr->longName == NULL)) {
        string = argPtr->name;
        switch (argPtr->numArgs) {
        case NARGS_ZERO_OR_MORE:
            if (argPtr->parserPtr->flags & USE_QUESTION_MARK) {
                Blt_DBuffer_Format(argbuf, "?%s ...?", string);
            } else {
                Blt_DBuffer_Format(argbuf, "[%s ...]", string);
            }
            break;
        case NARGS_ONE_OR_MORE:
            Blt_DBuffer_Format(argbuf, "%s ...", string);
            break;
        case NARGS_ZERO_OR_ONE:
            if (argPtr->parserPtr->flags & USE_QUESTION_MARK) {
                Blt_DBuffer_Format(argbuf, "?%s?", string);
            } else {
                Blt_DBuffer_Format(argbuf, "[%s]", string);
            }
            break;
        default:
            {
                int i;
                
                for (i = 0; i < argPtr->numArgs; i++) {
                    Blt_DBuffer_Format(argbuf, "%s%s", string,
                                       (i < (argPtr->numArgs - 1)) ? " " : "");
                }
            }
            break;
        }
    } else {
        if (argPtr->shortName != NULL) {
            Blt_DBuffer_Format(argbuf, "%s", argPtr->shortName);
        } else if (argPtr->longName != NULL) {
            Blt_DBuffer_Format(argbuf, "%s", argPtr->longName);
        }
        string = ArgValue(argPtr);
        switch (argPtr->numArgs) {
        case NARGS_ZERO_OR_MORE:
            if (argPtr->parserPtr->flags & USE_QUESTION_MARK) {
                Blt_DBuffer_Format(argbuf, " ?%s ...?", string);
            } else {
                Blt_DBuffer_Format(argbuf, " [%s ...]", string);
            }
            break;
        case NARGS_ONE_OR_MORE:
            Blt_DBuffer_Format(argbuf, " %s ...", string);
            break;
        case NARGS_ZERO_OR_ONE:
            if (argPtr->parserPtr->flags & USE_QUESTION_MARK) {
                Blt_DBuffer_Format(argbuf, " ?%s?", string);
            } else {
                Blt_DBuffer_Format(argbuf, " [%s]", string);
            }                
            break;
        default:
            {
                int i;
                
                for (i = 0; i < argPtr->numArgs; i++) {
                    Blt_DBuffer_Format(argbuf, " %s", string);
                }
            }
            break;
        }
    }
    if ((argPtr->flags & REQUIRED) == 0) {
        if (argPtr->parserPtr->flags & USE_QUESTION_MARK) {
            Blt_DBuffer_Format(argbuf, "?");
        } else {
            Blt_DBuffer_Format(argbuf, "]");
        }
    }
#ifdef notdef
    fprintf(stderr, "arg=%s\n", Blt_DBuffer_String(argbuf));
#endif
}

static void
PrintUsage(Tcl_Interp *interp, Parser *parserPtr, Blt_Chain positionArgs,
           Blt_Chain switchArgs, Blt_DBuffer dbuffer)
{
    Blt_ChainLink link;
    Blt_DBuffer argbuf;

    argbuf = Blt_DBuffer_Create();
    Blt_DBuffer_Format(dbuffer, "\nusage: ");
    if (parserPtr->usage != NULL) {
        Blt_DBuffer_Format(dbuffer, "%s", parserPtr->usage);
    } else {
        size_t count, indent;
        Tcl_DString ds;

        Tcl_DStringInit(&ds);
        ProgramName(interp, parserPtr, &ds);
        Blt_DBuffer_Format(dbuffer, "%s", Tcl_DStringValue(&ds));
        Tcl_DStringFree(&ds);
        count = Blt_DBuffer_Length(dbuffer);
        indent = count - 1;
        for (link = Blt_Chain_FirstLink(switchArgs); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *argPtr;
            
            argPtr = Blt_Chain_GetValue(link);
            if ((argPtr->flags & REQUIRED) == 0) {
                continue;
            }
            Blt_DBuffer_SetLength(argbuf, 0);
            PrintUsageArg(argPtr, argbuf);
            if ((count + Blt_DBuffer_Length(argbuf)) > 75) {
                Blt_DBuffer_Format(dbuffer, "\n%*.s", indent, "");
                count = indent;
            }
            count += Blt_DBuffer_Length(argbuf);
            Blt_DBuffer_Concat(dbuffer, argbuf);
        }
        for (link = Blt_Chain_FirstLink(switchArgs); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *argPtr;
            
            argPtr = Blt_Chain_GetValue(link);
            if (argPtr->flags & REQUIRED) {
                continue;
            }
            Blt_DBuffer_SetLength(argbuf, 0);
            PrintUsageArg(argPtr, argbuf);
            if ((count + Blt_DBuffer_Length(argbuf)) > 75) {
                Blt_DBuffer_Format(dbuffer, "\n%*.s", indent, "");
                count = indent;
            }
            count += Blt_DBuffer_Length(argbuf);
            Blt_DBuffer_Concat(dbuffer, argbuf);
        }
        for (link = Blt_Chain_FirstLink(positionArgs); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *argPtr;
            
            argPtr = Blt_Chain_GetValue(link);
            if ((argPtr->flags & REQUIRED) == 0) {
                continue;
            }
            Blt_DBuffer_SetLength(argbuf, 0);
            PrintUsageArg(argPtr, argbuf);
            if ((count + Blt_DBuffer_Length(argbuf)) > 75) {
                Blt_DBuffer_Format(dbuffer, "\n%*.s", indent, "");
                count = indent;
            }
            count += Blt_DBuffer_Length(argbuf);
            Blt_DBuffer_Concat(dbuffer, argbuf);
        }
        for (link = Blt_Chain_FirstLink(positionArgs); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *argPtr;
            
            argPtr = Blt_Chain_GetValue(link);
            if (argPtr->flags & REQUIRED) {
                continue;
            }
            Blt_DBuffer_SetLength(argbuf, 0);
            PrintUsageArg(argPtr, argbuf);
            if ((count + Blt_DBuffer_Length(argbuf)) > 75) {
                Blt_DBuffer_Format(dbuffer, "\n%*.s", indent, "");
                count = indent;
            }
            count += Blt_DBuffer_Length(argbuf);
            Blt_DBuffer_Concat(dbuffer, argbuf);
        }
    }
    Blt_DBuffer_Format(dbuffer, "\n");
    Blt_DBuffer_Destroy(argbuf);
}

static void
PrintArgument(Argument *argPtr, Blt_DBuffer dbuffer)
{
    size_t start, finish;

    start = Blt_DBuffer_Length(dbuffer);
    if (argPtr->shortName != NULL) {
        Blt_DBuffer_Format(dbuffer, " %s", argPtr->shortName);
        if (argPtr->longName != NULL) {
            Blt_DBuffer_Format(dbuffer, ",");
        } 
    } else {
        if (argPtr->longName != NULL) {
            Blt_DBuffer_Format(dbuffer, "    ");
        }
    }
    if (argPtr->longName != NULL) {
        Blt_DBuffer_Format(dbuffer, " %s", argPtr->longName);
    }
    if ((argPtr->shortName == NULL) && (argPtr->longName == NULL)) {
        Blt_DBuffer_Format(dbuffer, " %s", argPtr->name);
    } else {
        switch (argPtr->numArgs) {
        case NARGS_ZERO_OR_MORE:
            if (argPtr->parserPtr->flags & USE_QUESTION_MARK) {
                Blt_DBuffer_Format(dbuffer, " ?%s ...?", ArgValue(argPtr));
            } else {
                Blt_DBuffer_Format(dbuffer, " [%s ...]", ArgValue(argPtr));
            }
            break;
        case NARGS_ONE_OR_MORE:
            Blt_DBuffer_Format(dbuffer, " %s ...", ArgValue(argPtr));
            break;
        case NARGS_ZERO_OR_ONE:
            if (argPtr->parserPtr->flags & USE_QUESTION_MARK) {
                Blt_DBuffer_Format(dbuffer, " ?%s?", ArgValue(argPtr));
            } else {
                Blt_DBuffer_Format(dbuffer, " [%s]", ArgValue(argPtr));
            }
            break;
        default:
            {
                int i;
                
                for (i = 0; i < argPtr->numArgs; i++) {
                    Blt_DBuffer_Format(dbuffer, " %s", ArgValue(argPtr));
                }
            }
            break;
        }
    }
    finish = Blt_DBuffer_Length(dbuffer);
    if (argPtr->help != NULL) {
        char *copy;
        size_t count;
        char *p;

        /* Indent to 30 characters. */
        if ((finish - start) >= 30) {
            Blt_DBuffer_Format(dbuffer, "\n%30.s", "");
        } else {
            Blt_DBuffer_Format(dbuffer, "%*.s", 30 - (finish - start), "");
        }
        copy = (char *)Blt_AssertStrdup(argPtr->help);
        count = 30;
        /* Append the word by word, wrapping when we exceed 75 characters. */
        for (p = strtok(copy, " \t\n"); p != NULL; p = strtok(NULL, " \t\n")) {
            int length;

            length = strlen(p);
            if ((length + count) > 75) {
                Blt_DBuffer_Format(dbuffer, "\n%30.s", "");
                count = 30;
            } 
            Blt_DBuffer_Format(dbuffer, "%s ", p);
            count += length + 1;        /* Add back the space. */
        }
        Blt_Free(copy);
    }
    Blt_DBuffer_Format(dbuffer, "\n");
}

static int
CompareSwitches(Blt_ChainLink *link1Ptr, Blt_ChainLink *link2Ptr)
{
    Argument *argPtr1, *argPtr2;
    const char *name1, *name2;

    argPtr1 = Blt_Chain_GetValue(*link1Ptr);
    argPtr2 = Blt_Chain_GetValue(*link2Ptr);
    if (argPtr1->numArgs == NARGS_LAST_SWITCH) {
        return 1;
    }
    if (argPtr2->numArgs == NARGS_LAST_SWITCH) {
        return -1;
    }
    if (argPtr1->shortName != NULL) {
        name1 = argPtr1->shortName;
    } else if (argPtr1->longName != NULL) {
        name1 = argPtr1->longName;
    } else {
        name1 = argPtr1->name;
    }
    if (argPtr2->shortName != NULL) {
        name2 = argPtr2->shortName;
    } else if (argPtr2->longName != NULL) {
        name2 = argPtr2->longName;
    } else {
        name2 = argPtr2->name;
    }
    return Blt_DictionaryCompare(name1, name2);
}

/* 
 *  required vs optional
 *      sort switches.
 */
static void
PrintHelp(Tcl_Interp *interp, Parser *parserPtr)
{
    Blt_ChainLink link;
    int numRequiredArgs;
    Blt_Chain positionArgs, switchArgs;
    Blt_DBuffer dbuffer;

    dbuffer = Blt_DBuffer_Create();
    positionArgs = Blt_Chain_Create();
    switchArgs = Blt_Chain_Create();
    numRequiredArgs = 0;
    for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Argument *argPtr;

        argPtr = Blt_Chain_GetValue(link);
        if ((argPtr->longName == NULL) && (argPtr->shortName == NULL)) {
            Blt_Chain_Append(positionArgs, argPtr);
        } else {
            Blt_Chain_Append(switchArgs, argPtr);
        }
        if (argPtr->flags & REQUIRED) {
            numRequiredArgs++;
        }
    }
    Blt_Chain_Sort(switchArgs, CompareSwitches);
    PrintUsage(interp, parserPtr, positionArgs, switchArgs, dbuffer);

    if (parserPtr->desc != NULL) {
        char *copy;
        size_t count;
        char *p;

        copy = (char *)Blt_AssertStrdup(parserPtr->desc);
        count = 1;
        Blt_DBuffer_Format(dbuffer, "\n ");
        /* Append the word by word, wrapping when we exceed 75 characters. */
        for (p = strtok(copy, " \t\n"); p != NULL; p = strtok(NULL, " \t\n")) {
            int length;

            length = strlen(p);
            if ((length + count) > 75) {
                Blt_DBuffer_Format(dbuffer, "\n ");
                count = 1;
            } 
            Blt_DBuffer_Format(dbuffer, "%s ", p);
            count += length + 1;        /* Add back the space. */
        }
        Blt_Free(copy);
        Blt_DBuffer_Format(dbuffer, "\n");
    }
    /* Step 3. Print the required arguments. Positional arguments before
     *         switches. */
    if (numRequiredArgs > 0) {
        Blt_DBuffer_Format(dbuffer, "\nrequired arguments:\n");
        for (link = Blt_Chain_FirstLink(positionArgs); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *argPtr;
            
            argPtr = Blt_Chain_GetValue(link);
            if ((argPtr->flags & REQUIRED) == 0) {
                continue;
            }
            if ((argPtr->flags & (ARG_STATE_HIDDEN|ARG_STATE_DISABLED)) == 0) {
                PrintArgument(argPtr, dbuffer);
            }
        }
        for (link = Blt_Chain_FirstLink(switchArgs); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *argPtr;
            
            argPtr = Blt_Chain_GetValue(link);
            if ((argPtr->flags & REQUIRED) == 0) {
                continue;
            }
            if ((argPtr->flags & (ARG_STATE_HIDDEN|ARG_STATE_DISABLED)) == 0) {
                PrintArgument(argPtr, dbuffer);
            }
        }
    }
    if (Blt_Chain_GetLength(parserPtr->args) > numRequiredArgs) {
        Blt_DBuffer_Format(dbuffer, "\noptional arguments:\n");
        for (link = Blt_Chain_FirstLink(positionArgs); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *argPtr;
            
            argPtr = Blt_Chain_GetValue(link);
            if (argPtr->flags & REQUIRED) {
                continue;
            }
            if ((argPtr->flags & (ARG_STATE_HIDDEN|ARG_STATE_DISABLED)) == 0) {
                PrintArgument(argPtr, dbuffer);
            }
        }
        for (link = Blt_Chain_FirstLink(switchArgs); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *argPtr;
            
            argPtr = Blt_Chain_GetValue(link);
            if (argPtr->flags & REQUIRED) {
                continue;
            }
            if ((argPtr->flags & (ARG_STATE_HIDDEN|ARG_STATE_DISABLED)) == 0) {
                PrintArgument(argPtr, dbuffer);
            }
        }
    }
    if (parserPtr->epilog != NULL) {
        Blt_DBuffer_Format(dbuffer, "\n%s\n", parserPtr->epilog);
    }
    Blt_Chain_Destroy(positionArgs);
    Blt_Chain_Destroy(switchArgs);
    Tcl_SetObjResult(interp, Blt_DBuffer_StringObj(dbuffer));
    Blt_DBuffer_Destroy(dbuffer);
}

/* parserName add "fred" -short "-f" -long "--fred"  */

static int
ParseArguments(Tcl_Interp *interp, Parser *parserPtr, Blt_Chain chain)
{
    Blt_ChainLink link, link2, next;
    Blt_Chain found;

    found = Blt_Chain_Create();

    /* Step 1.  Process switches and remove them from argument list.  Do
     *          this until we reach the end of the arguments or hit a
     *          special -- argument. */
    for (link = Blt_Chain_FirstLink(chain); link != NULL; link = next) {
        Tcl_Obj *objPtr;
        Argument *argPtr;
        int count;
        int numMatches;
        
        next = Blt_Chain_NextLink(link);
        objPtr = Blt_Chain_GetValue(link);
        numMatches = FindSwitch(interp, parserPtr, objPtr, &argPtr);
        if (numMatches != 1) {
            if ((LooksLikeSwitch(parserPtr, objPtr)) &&
                (parserPtr->flags & ERROR_ON_BAD_SWITCHES)) {
                goto error;
            }
            Tcl_ResetResult(interp);
            continue;
        }
        /* Remove the switch from the list. */
        Blt_Chain_DeleteLink(chain, link);

        /* Process switch arguments. */
        /* Last switch. */
        if (argPtr->numArgs == NARGS_LAST_SWITCH) {
            break;
        }
        if (argPtr->flags & ACTION_HELP) {
            Blt_Chain_Destroy(found);
            PrintHelp(interp, parserPtr);
            return TCL_BREAK;
        }
        /* Zero arguments. */
        if (argPtr->numArgs == 0) {
            if (SetDefaultValue(interp, argPtr) != TCL_OK) {
                goto error;
            }
            continue;
        }
        /* Zero or one argument. */
        if (argPtr->numArgs == NARGS_ZERO_OR_ONE) {
            /* Look ahead. Is next argument a switch? */
            if (next != NULL) {
                objPtr = Blt_Chain_GetValue(next);
            }
            if ((next == NULL) || 
                (((argPtr->flags & ALLOW_PREFIX_CHARS) == 0) &&
                 (LooksLikeSwitch(parserPtr, objPtr)))) {
                if (SetDefaultValue(interp, argPtr) != TCL_OK) {
                    goto error;
                }
                continue;
            }
            if (SetValue(interp, argPtr, objPtr) != TCL_OK) {
                goto error;
            }
            link = next;
            next = Blt_Chain_NextLink(link);
            Blt_Chain_DeleteLink(chain, link);
            continue;
        }
        /* Zero or more arguments. */
        if (argPtr->numArgs == NARGS_ZERO_OR_MORE) {
            Blt_Chain_Reset(found);
            for (link = next; link != NULL; link = next) {
                next = Blt_Chain_NextLink(link);
                objPtr = Blt_Chain_GetValue(link);
                if (((argPtr->flags & ALLOW_PREFIX_CHARS) == 0) &&
                    (LooksLikeSwitch(parserPtr, objPtr))) {
                    next = link;
                    break;              /* Starting next switch. */
                }
                if (AddValue(interp, argPtr, link, chain, found) != TCL_OK) {
                    goto error;
                }
            }
            if (Blt_Chain_GetLength(found) == 0) {
                if (SetDefaultValue(interp, argPtr) != TCL_OK) {
                    goto error;
                }
            } else {
                if (SetValues(interp, argPtr, found) != TCL_OK) {
                    goto error;
                }
            }
            continue;
        }
        /* One or more arguments. */
        if (argPtr->numArgs == NARGS_ONE_OR_MORE) {
            Blt_Chain_Reset(found);
            for (link = next; link != NULL; link = next) {
                next = Blt_Chain_NextLink(link);
                objPtr = Blt_Chain_GetValue(link);
                if (((argPtr->flags & ALLOW_PREFIX_CHARS) == 0) &&
                    (LooksLikeSwitch(parserPtr, objPtr))) {
                    next = link;
                    break;              /* Possibly the next switch. */
                }
                if (AddValue(interp, argPtr, link, chain, found) != TCL_OK) {
                    goto error;
                }
            }
            if (Blt_Chain_GetLength(found) < 1) {
                Tcl_AppendResult(interp, "no values provided for \"",
                        argPtr->name, "\": requires 1 or more values", 
                        (char *)NULL);
                goto error;
            } else {
                if (SetValues(interp, argPtr, found) != TCL_OK) {
                    goto error;
                }
            }
            continue;
        }
        Blt_Chain_Reset(found);
        /* Specified number of arguments. */
        for (count = 0, link = next; link != NULL; link = next) {
            next = Blt_Chain_NextLink(link);
            objPtr = Blt_Chain_GetValue(link);
            if (((argPtr->flags & ALLOW_PREFIX_CHARS) == 0) &&
                (LooksLikeSwitch(parserPtr, objPtr))) {
                next = link;
                break;              /* Possibly the next switch. */
            }
            count++;
            if (AddValue(interp, argPtr, link, chain, found) != TCL_OK) {
                goto error;
            }
            if (count == argPtr->numArgs) {
                break;
            }
        }
        if (Blt_Chain_GetLength(found) != argPtr->numArgs) {
            Tcl_AppendResult(interp, "argument \"", SwitchName(argPtr), 
               "\" requires ", Blt_Itoa(argPtr->numArgs), " value(s), ",
              (char *)NULL);
            Tcl_AppendResult(interp, "found ",  
                Blt_Itoa(Blt_Chain_GetLength(found)), (char *)NULL);
            goto error;
        } else {
            if (SetValues(interp, argPtr, found) != TCL_OK) {
                goto error;
            }
        }
    }
    next = Blt_Chain_FirstLink(chain);
    /* Step 2. Process positional arguments. */
    for (link2 = Blt_Chain_FirstLink(parserPtr->args); link2 != NULL;
         link2 = Blt_Chain_NextLink(link2)) {
        Argument *argPtr;
        int count;
        
        argPtr = Blt_Chain_GetValue(link2);
        if ((argPtr->shortName != NULL) || (argPtr->longName != NULL)) {
            continue;
        }
        /* Process argument */
        /* Zero aruments. */
        if (argPtr->numArgs == 0) {
            SetDefaultValue(interp, argPtr);
            continue;
        }
        /* Zero or one argument. */
        if (argPtr->numArgs == NARGS_ZERO_OR_ONE) {
            Tcl_Obj *objPtr;
            
            /* Look ahead. Is next argument a switch? */
            objPtr = Blt_Chain_GetValue(next);
            /* Can't be a switch. */
            if (((argPtr->flags & ALLOW_PREFIX_CHARS) == 0) &&
                (LooksLikeSwitch(parserPtr, objPtr))) {
                if (SetDefaultValue(interp, argPtr) != TCL_OK) {
                    goto error;
                }
            } else {
                if (SetValue(interp, argPtr, objPtr) != TCL_OK) {
                    goto error;
                }
                link = next;
                next = Blt_Chain_NextLink(link);
                Blt_Chain_DeleteLink(chain, link);
            }
            continue;
        }
        /* Zero or more arguments. */
        if (argPtr->numArgs == NARGS_ZERO_OR_MORE) {
            Blt_Chain_Reset(found);
            for (link = next; link != NULL; link = next) {
                next = Blt_Chain_NextLink(link);
                if (AddValue(interp, argPtr, link, chain, found) != TCL_OK) {
                    goto error;
                }
            }
            if (Blt_Chain_GetLength(found) == 0) {
                if (SetDefaultValue(interp, argPtr) != TCL_OK) {
                    goto error;
                }
            } else {
                if (SetValues(interp, argPtr, found) != TCL_OK) {
                    goto error;
                }
            }
            continue;
        } 
        /* One or more arguments. */
        if (argPtr->numArgs == NARGS_ONE_OR_MORE) {
            Blt_Chain_Reset(found);
            for (link = next; link != NULL; link = next) {
                Tcl_Obj *objPtr;

                next = Blt_Chain_NextLink(link);
                objPtr = Blt_Chain_GetValue(link);
                if (((argPtr->flags & ALLOW_PREFIX_CHARS) == 0) &&
                    (LooksLikeSwitch(parserPtr, objPtr))) {
                    break;              /* Possibly the next switch. */
                }
                if (AddValue(interp, argPtr, link, chain, found) != TCL_OK) {
                    goto error;
                }
            }
            if (Blt_Chain_GetLength(found) < 1) {
                Tcl_AppendResult(interp, "no values provided for \"",
                        argPtr->name, "\": requires 1 or more values", 
                        (char *)NULL);
                goto error;
            } else {
                SetValues(interp, argPtr, found);
            }
        }
        Blt_Chain_Reset(found);
        /* Specified number of arguments. */
        for (count = 0, link = next; link != NULL; link = next) {
            next = Blt_Chain_NextLink(link);
            if (AddValue(interp, argPtr, link, chain, found) != TCL_OK) {
                goto error;
            }
            count++;
            if (count == argPtr->numArgs) {
                break;
            }
        }
        if (count == 0) {
            continue;
        }
        if (count != argPtr->numArgs) {
            Tcl_AppendResult(interp, "found ", Blt_Itoa(count),
                 " value(s) for \"", SwitchName(argPtr), "\"", (char *)NULL);
            Tcl_AppendResult(interp, ", requires ", Blt_Itoa(argPtr->numArgs), 
                             "value(s)",(char *)NULL);
            goto error;
        } else {
            if (SetValues(interp, argPtr, found) != TCL_OK) {
                goto error;
            }
        }
    }

    /* Check for leftover arguments. */
    if ((Blt_Chain_GetLength(chain) > 0) &&
        (parserPtr->flags & ERROR_ON_EXTRA_ARGS)) {
        Tcl_AppendResult(interp, "unknown arguments found: ",
                         (char *)NULL);
        for (link = Blt_Chain_FirstLink(chain); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Tcl_Obj *objPtr;

            objPtr = Blt_Chain_GetValue(link);
            Tcl_AppendResult(interp, "\"", Tcl_GetString(objPtr), "\" ", (char *)NULL);
        }
        goto error;
    }

    /* Look for missing required arguments and fill in unset current values
     * from the default. */
    for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Argument *argPtr;
        Argument *destPtr;
        
        argPtr = Blt_Chain_GetValue(link);
        destPtr = (argPtr->destPtr != NULL) ? argPtr->destPtr : argPtr;
        
        if (destPtr->currentObjPtr == NULL) {
            if (argPtr->flags & REQUIRED) {
                Tcl_AppendResult(interp, "missing required argument \"",
                     SwitchName(argPtr), "\"", (char *)NULL);
                goto error;
            }
            /* The default argument value is the current value. */
            destPtr->currentObjPtr = DefaultValue(argPtr);
            if (destPtr->currentObjPtr != NULL) {
                Tcl_IncrRefCount(destPtr->currentObjPtr);
            }
        }
    }
    return TCL_OK;
 error:
    Blt_Chain_Destroy(found);
    return TCL_ERROR;
}

/*
 *---------------------------------------------------------------------------
 *
 * GetParser --
 *
 *      Find the parser command associated with the TCL command "string".
 *      
 *      We have to do multiple lookups to get this right.  
 *
 *      The first step is to generate a canonical command name.  If an
 *      unqualified command name (i.e. no namespace qualifier) is given, we
 *      should search first the current namespace and then the global one.
 *      Most TCL commands (like Tcl_GetCmdInfo) look only at the global
 *      namespace.
 *
 *      Next check if the string is 
 *              a) a TCL command and 
 *              b) really is a command for a parser object.  
 *      Tcl_GetCommandInfo will get us the objClientData field that should
 *      be a parserPtr.  We can verify that by searching our hashtable of
 *      parserPtr addresses.
 *
 * Results:
 *      A pointer to the parser command.  If no associated parser command
 *      can be found, NULL is returned.  It's up to the calling routines to
 *      generate an error message.
 *
 *---------------------------------------------------------------------------
 */
static Parser *
GetParser(ParseArgsCmdInterpData *dataPtr, Tcl_Interp *interp, 
             const char *string)
{
    Blt_HashEntry *hPtr;
    Blt_ObjectName objName;
    Tcl_CmdInfo cmdInfo;
    Tcl_DString ds;
    const char *parserName;
    int result;

    /* Pull apart the parser name and put it back together in a standard
     * format. */
    if (!Blt_ParseObjectName(interp, string, &objName, BLT_NO_ERROR_MSG)) {
        return NULL;                    /* No such parent namespace. */
    }
    /* Rebuild the fully qualified name. */
    parserName = Blt_MakeQualifiedName(&objName, &ds);
    result = Tcl_GetCommandInfo(interp, parserName, &cmdInfo);
    if (!result) {
        Tcl_DStringFree(&ds);
        return NULL;
    }
    hPtr = Blt_FindHashEntry(&dataPtr->parserTable, parserName);
    Tcl_DStringFree(&ds);
    if (hPtr == NULL) {
        return NULL;
    }
    return Blt_GetHashValue(hPtr);
}

static int
InitParser(Tcl_Interp *interp, Parser *parserPtr)
{
    Blt_ChainLink link;
    
    parserPtr->flags &= ~(UPDATE_VARIABLES|EXCLUSIONS);
    for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Argument *argPtr;
        
        argPtr = Blt_Chain_GetValue(link);
        argPtr->flags &= ~MODIFIED;
        if (argPtr->varNameObjPtr != NULL) {
            parserPtr->flags |= UPDATE_VARIABLES;
        }
        if (argPtr->excludeObjPtr != NULL) {
            parserPtr->flags |= EXCLUSIONS;
        }
        /* Check if min/max are valid for the designated type. */
        switch (argPtr->flags & TYPE_MASK) {
        case TYPE_STRING:
        case TYPE_BOOLEAN:
            break;
            
        case TYPE_INT:
            {
                long l;

                if ((argPtr->minObjPtr != NULL) &&
                    (Blt_GetLongFromObj(interp, argPtr->minObjPtr, &l)
                     != TCL_OK)) {
                    return TCL_ERROR;
                }
                if ((argPtr->maxObjPtr != NULL) &&
                    (Blt_GetLongFromObj(interp, argPtr->maxObjPtr, &l)
                     != TCL_OK)) {
                    return TCL_ERROR;
                }
            }
            break;

        case TYPE_DOUBLE:
            {
                double d;
            
                if ((argPtr->minObjPtr != NULL) &&
                    (Blt_GetDoubleFromObj(interp, argPtr->minObjPtr, &d)
                     != TCL_OK)) {
                    return TCL_ERROR;
                }
                if ((argPtr->maxObjPtr != NULL) &&
                    (Blt_GetDoubleFromObj(interp, argPtr->maxObjPtr, &d)
                     != TCL_OK)) {
                    return TCL_ERROR;
                }
            }
            break;
        }
    }
    return TCL_OK;
}

static int
CheckExclusions(Tcl_Interp *interp, Parser *parserPtr)
{
    Blt_ChainLink link;
    
    for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Argument *argPtr;
        
        argPtr = Blt_Chain_GetValue(link);
        /* Check for mutually exclusive arguments. */
        if ((argPtr->flags & MODIFIED) && (argPtr->excludeObjPtr != NULL)) {
            int objc;
            Tcl_Obj **objv;
            int i;
            
            if (Tcl_ListObjGetElements(interp, argPtr->excludeObjPtr,
                                       &objc, &objv) != TCL_OK) {
                return TCL_ERROR;
            }
            for (i = 0; i < objc; i++) {
                Argument *excludePtr;
                
                if (GetArgumentFromObj(NULL, parserPtr, objv[i], &excludePtr)
                    != TCL_OK) {
                    continue;           /* Ignore bad arguments. */
                } 
                if (excludePtr->flags & MODIFIED) {
                    Tcl_AppendResult(interp, "can't set both \"",
                        SwitchName(argPtr), "\" and \"", SwitchName(excludePtr),
                        "\"", (char *)NULL);
                    return TCL_ERROR;
                }
            }
        }            
    }
    return TCL_OK;
}

static int
UpdateVariables(Tcl_Interp *interp, Parser *parserPtr, Tcl_Obj *varNameObjPtr)
{
    Blt_ChainLink link;

    for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Argument *argPtr, *destPtr;
        
        argPtr = Blt_Chain_GetValue(link);
        destPtr = (argPtr->destPtr != NULL) ? argPtr->destPtr : argPtr;
        if (destPtr->currentObjPtr == NULL) {
             continue;                   /* The argument was not set. */
        }
        /* Set the global array variable if one was specified. */
        if (varNameObjPtr != NULL) {
            const char *varName;

            varName = Tcl_GetString(varNameObjPtr);
            if (Tcl_SetVar2Ex(interp, varName, argPtr->name,
                    destPtr->currentObjPtr, TCL_LEAVE_ERR_MSG) == NULL) {
                return TCL_ERROR;
            }
        }
        /* Set the argument's variable if one was specified. */
        if (argPtr->varNameObjPtr != NULL) {
            if (Tcl_ObjSetVar2(interp, argPtr->varNameObjPtr, NULL,
                destPtr->currentObjPtr, TCL_LEAVE_ERR_MSG) == NULL) {
                return TCL_ERROR;
            }
        }
    }        
    return TCL_OK;
}

static Blt_Chain
CreateArgumentChain(Tcl_Interp *interp, Tcl_Obj *objPtr)
{
    int objc;
    Tcl_Obj **objv;
    Blt_Chain chain;
    int i;
    
    if (Tcl_ListObjGetElements(interp, objPtr, &objc, &objv) != TCL_OK) {
        return NULL;
    }
    chain = Blt_Chain_Create();
    for (i = 0; i < objc; i++) {
        Blt_Chain_Append(chain, objv[i]);
    }
    return chain;
}

/*
 *---------------------------------------------------------------------------
 *
 * GenerateName --
 *
 *      Generates an unique parser command name.  Parser names are in the
 *      form "parseargsN", where N is a non-negative integer. Check each
 *      name generated to see if it is already a parser. We want to recycle
 *      names if possible.
 *      
 * Results:
 *      Returns the unique name.  The string itself is stored in the
 *      dynamic string passed into the routine.
 *
 *---------------------------------------------------------------------------
 */
static const char *
GenerateName(Tcl_Interp *interp, const char *prefix, const char *suffix,
             Tcl_DString *resultPtr)
{

    int i;
    const char *name;

    /* 
     * Parse the command and put back so that it's in a consistent
     * format.  
     *
     *  t1         <current namespace>::t1
     *  n1::t1     <current namespace>::n1::t1
     *  ::t1       ::t1
     *  ::n1::t1   ::n1::t1
     */
    name = NULL;            /* Suppress compiler warning. */
    for (i = 0; i < INT_MAX; i++) {
        Blt_ObjectName objName;
        Tcl_DString ds;
        char string[200];

        Tcl_DStringInit(&ds);
        Tcl_DStringAppend(&ds, prefix, -1);
        Blt_FmtString(string, 200, "parseargs%d", i);
        Tcl_DStringAppend(&ds, string, -1);
        Tcl_DStringAppend(&ds, suffix, -1);
        if (!Blt_ParseObjectName(interp, Tcl_DStringValue(&ds), &objName, 0)) {
            Tcl_DStringFree(&ds);
            return NULL;
        }
        name = Blt_MakeQualifiedName(&objName, resultPtr);
        if (ParserExists(interp, name)) {
            continue;
        }
        if (Blt_CommandExists(interp, name)) {
            continue;           /* A command by this name already exists. */
        }
        Tcl_DStringFree(&ds);
        break;
    }
    return name;
}

static Parser *
NewParserObj(ParseArgsCmdInterpData *dataPtr, const char *name)
{
    int isNew;
    Blt_HashEntry *hPtr;
    Parser *parserPtr;
    
    parserPtr = Blt_AssertCalloc(1, sizeof(Parser));
    parserPtr->dataPtr = dataPtr;
    parserPtr->interp = dataPtr->interp;
    parserPtr->flags = ERROR_ON_BAD_SWITCHES;
    Blt_InitHashTable(&parserPtr->argTable, BLT_STRING_KEYS);
    parserPtr->args = Blt_Chain_Create();
    parserPtr->cmdToken = Tcl_CreateObjCommand(dataPtr->interp, (char *)name, 
         ParserInstObjCmd, parserPtr, ParserInstDeleteProc);
    hPtr = Blt_CreateHashEntry(&dataPtr->parserTable, name, &isNew);
    parserPtr->name = Blt_GetHashKey(&dataPtr->parserTable, hPtr);
    parserPtr->hashPtr = hPtr;
    Blt_SetHashValue(hPtr, parserPtr);
    return parserPtr;
}


static Parser *
NewParser(ClientData clientData, Tcl_Interp *interp, const char *name)
{
    ParseArgsCmdInterpData *dataPtr = clientData;
    Tcl_DString ds;

    Tcl_DStringInit(&ds);
    if (name == NULL) {
        name = GenerateName(interp, "", "", &ds);
    } else {
        char *p;

        p = strstr(name, "#auto");
        if (p != NULL) {
            *p = '\0';
            name = GenerateName(interp, name, p + 5, &ds);
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
                return NULL;
            }
            name = Blt_MakeQualifiedName(&objName, &ds);
            /* 
             * Check if the command already exists. 
             */
            if (Blt_CommandExists(interp, name)) {
                Tcl_AppendResult(interp, "a command \"", name,
                                 "\" already exists", (char *)NULL);
                goto error;
            }
            if (ParserExists(interp, name)) {
                Tcl_AppendResult(interp, "an argument parser \"", name, 
                        "\" already exists", (char *)NULL);
                goto error;
            }
        } 
    } 
    if (name != NULL) {
        Parser *parserPtr;

        parserPtr = NewParserObj(dataPtr, name);
        Tcl_DStringFree(&ds);
        return parserPtr;
    }
 error:
    Tcl_DStringFree(&ds);
    return NULL;
}

static int
ConfigureArg(Argument *argPtr, Tcl_Interp *interp, int objc,
             Tcl_Obj *const *objv, int flags)
{
    if (Blt_ParseSwitches(interp, argSpecs, objc, objv, argPtr, flags) < 0) {
        return TCL_ERROR;
    }
    if (argPtr->flags & TYPE_INT) {
        long l;
        
        if ((argPtr->minObjPtr != NULL) &&
            (Blt_GetLongFromObj(interp, argPtr->minObjPtr, &l) != TCL_OK)) {
            Tcl_AppendResult(interp, ": bad minimum value for \"", 
                             SwitchName(argPtr), "\"", (char *)NULL);
            return TCL_ERROR;
        }
        if ((argPtr->maxObjPtr != NULL) &&
            (Blt_GetLongFromObj(interp, argPtr->maxObjPtr, &l) != TCL_OK)) {
            Tcl_AppendResult(interp, ": bad maximum value for \"", 
                             SwitchName(argPtr), "\"", (char *)NULL);
            return TCL_ERROR;
        }
    }
    if (argPtr->flags & TYPE_DOUBLE) {
        double d;
        
        if ((argPtr->minObjPtr != NULL) &&
            (Blt_GetDoubleFromObj(interp, argPtr->minObjPtr, &d) != TCL_OK)) {
            Tcl_AppendResult(interp, ": bad minimum value for \"", 
                             SwitchName(argPtr), "\"", (char *)NULL);
            return TCL_ERROR;
        }
        if ((argPtr->maxObjPtr != NULL) &&
            (Blt_GetDoubleFromObj(interp, argPtr->maxObjPtr, &d) != TCL_OK)) {
            Tcl_AppendResult(interp, ": bad maximum value for \"", 
                             SwitchName(argPtr), "\"", (char *)NULL);
            return TCL_ERROR;
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * AddOp --
 *
 *      parserName add argName ?switches...?
 *
 *---------------------------------------------------------------------------
 */
static int
AddOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;
    Argument *argPtr;
    Blt_HashEntry *hPtr;
    const char *name;
    int isNew;
    
    name = Tcl_GetString(objv[2]);
    hPtr = Blt_CreateHashEntry(&parserPtr->argTable, name, &isNew);
    if (!isNew) {
        Tcl_AppendResult(interp, "argument \"", name, "\" already exists in \"",
                         parserPtr->name, "\"", (char *)NULL);
        return TCL_ERROR;
    }
    argPtr = NewArgument(interp, parserPtr, hPtr);
    if (argPtr == NULL) {
        return TCL_ERROR;
    }
    if (ConfigureArg(argPtr, interp, objc - 3, objv + 3, BLT_SWITCH_INITIALIZE) 
        != TCL_OK) {
        DestroyArgument(argPtr);
        return TCL_ERROR;
    }
    Tcl_SetStringObj(Tcl_GetObjResult(interp), argPtr->name, -1);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ArgCgetOp --
 *
 *      parserName argument cget argName option
 *
 *---------------------------------------------------------------------------
 */
static int
ArgCgetOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Argument *argPtr;
    Parser *parserPtr = clientData;
    
    if (GetArgumentFromObj(interp, parserPtr, objv[3], &argPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    return Blt_SwitchValue(interp, argSpecs, argPtr, objv[4], 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * ArgConfigureOp --
 *
 *      parserName argument configure argName ?option value...?
 *
 *---------------------------------------------------------------------------
 */
static int
ArgConfigureOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Argument *argPtr;
    Parser *parserPtr = clientData;

    if (GetArgumentFromObj(interp, parserPtr, objv[3], &argPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (objc == 4) {
        return Blt_SwitchInfo(interp, argSpecs, argPtr, (Tcl_Obj *)NULL, 0);
    } else if (objc == 5) {
        return Blt_SwitchInfo(interp, argSpecs, argPtr, objv[4], 0);
    }
    if (ConfigureArg(argPtr, interp, objc - 4, objv + 4, BLT_SWITCH_DEFAULTS) 
        != TCL_OK) {
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ArgumentOp --
 *
 *      This procedure is invoked to process commands on behalf of the
 *	argument parser object.
 *
 * Results:
 *      A standard TCL result.
 *
 * Side effects:
 *      See the user documentation.
 *
 *	parserName argument operName argName 
 *
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec argOps[] =
{
    {"cget",        2, ArgCgetOp,        5, 5, "argName option",},
    {"configure",   2, ArgConfigureOp,   4, 0, "argName ?value ...?",},
};

static int numArgOps = sizeof(argOps) / sizeof(Blt_OpSpec);

static int
ArgumentOp(ClientData clientData, Tcl_Interp *interp, int objc,
           Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numArgOps, argOps, BLT_OP_ARG2,
                            objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}

/*
 *---------------------------------------------------------------------------
 *
 * CgetOp --
 *
 *      parserName cget option
 *
 *---------------------------------------------------------------------------
 */
static int
CgetOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;

    return Blt_SwitchValue(interp, cmdSpecs, parserPtr, objv[2], 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * ConfigureOp --
 *
 *      parserName configure ?option value...?
 *
 *---------------------------------------------------------------------------
 */
static int
ConfigureOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;
    
    if (objc == 2) {
        return Blt_SwitchInfo(interp, cmdSpecs, parserPtr, (Tcl_Obj *)NULL, 0);
    } else if (objc == 3) {
        return Blt_SwitchInfo(interp, cmdSpecs, parserPtr, objv[2], 0);
    }
    if (Blt_ParseSwitches(interp, cmdSpecs, objc - 2 , objv + 2, parserPtr,
                          BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CurrentOp --
 *
 *      parserName configure ?option value...?
 *
 *---------------------------------------------------------------------------
 */
static int
CurrentOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * DeleteOp --
 *
 *      parserName delete ?argName...?
 *
 *---------------------------------------------------------------------------
 */
static int
DeleteOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;
    int i;
    
    for (i = 2; i < objc; i++) {
        Argument *argPtr;

        if (GetArgumentFromObj(interp, parserPtr, objv[i], &argPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        DestroyArgument(argPtr);
    } 
    return TCL_OK;
}


/*
 *---------------------------------------------------------------------------
 *
 * ExistsOp --
 *
 *      parserName exists argName
 *
 *---------------------------------------------------------------------------
 */
static int
ExistsOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;
    Argument *argPtr;
    int bool;
    
    bool = TRUE;
    if (GetArgumentFromObj(NULL, parserPtr, objv[2], &argPtr) != TCL_OK) {
        bool = FALSE;
    } 
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), bool);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * GetOp --
 *
 *      parserName get ?argName? ?defValue?
 *
 *---------------------------------------------------------------------------
 */
static int
GetOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;

    if (objc == 2) {
        Blt_ChainLink link;
        Tcl_Obj *listObjPtr;

        listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
        for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *argPtr, *destPtr;
            Tcl_Obj *nameObjPtr, *valueObjPtr;

            argPtr = Blt_Chain_GetValue(link);
            destPtr = (argPtr->destPtr != NULL) ? argPtr->destPtr : argPtr;
            valueObjPtr = destPtr->currentObjPtr;
            if (valueObjPtr == NULL) {
                valueObjPtr = DefaultValue(argPtr);
            } 
            if (valueObjPtr == NULL) {
                continue;               /* No default value. */
            }
            nameObjPtr = Tcl_NewStringObj(argPtr->name, -1);
            Tcl_ListObjAppendElement(interp, listObjPtr, nameObjPtr);
            Tcl_ListObjAppendElement(interp, listObjPtr, valueObjPtr);
        }
        Tcl_SetObjResult(interp, listObjPtr);
        return TCL_OK;
    } else {
        Argument *argPtr, *destPtr;
        Tcl_Obj *valueObjPtr;

        if (GetArgumentFromObj(interp, parserPtr, objv[2], &argPtr) != TCL_OK) {
            if (objc == 4) {
                Tcl_ResetResult(interp);
                Tcl_SetObjResult(interp, objv[3]);
                return TCL_OK;
            }
            return TCL_ERROR;
        } 
        destPtr = (argPtr->destPtr != NULL) ? argPtr->destPtr : argPtr;
        valueObjPtr = destPtr->currentObjPtr;
        if (valueObjPtr == NULL) {
            valueObjPtr = (objc == 4) ? objv[3] : DefaultValue(argPtr); 
        }
        if (valueObjPtr == NULL) {
            /* It's an error if there's no default value. */
            Tcl_AppendResult(interp, "no value was specified for argument \"",
                             argPtr->name, "\"", (char *)NULL);
            return TCL_ERROR;
        }
        Tcl_SetObjResult(interp, valueObjPtr);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * HelpOp --
 *
 *      parserName help
 *
 *---------------------------------------------------------------------------
 */
static int
HelpOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;

    PrintHelp(interp, parserPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * IsChangedOp --
 *
 *      parserName ischanged argName
 *
 *---------------------------------------------------------------------------
 */
static int
IsChangedOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Argument *argPtr;
    Parser *parserPtr = clientData;

    if (GetArgumentFromObj(interp, parserPtr, objv[2], &argPtr) != TCL_OK) {
        return TCL_ERROR;
    } 
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), 
                      (argPtr->flags & MODIFIED) ? 1 : 0);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * NamesOp --
 *
 *        pathName names ?pattern ...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
NamesOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)     
{
    Parser *parserPtr = clientData;
    Tcl_Obj *listObjPtr;

    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    if (objc == 2) {
        Blt_ChainLink link;

        for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *argPtr;
            Tcl_Obj *objPtr;
            
            argPtr = Blt_Chain_GetValue(link);
            objPtr = Tcl_NewStringObj(argPtr->name, -1);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
        }
    } else {
        Blt_ChainLink link;

        for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Argument *argPtr;
            int i;

            argPtr = Blt_Chain_GetValue(link);
            for (i = 2; i < objc; i++) {
                if (Tcl_StringMatch(argPtr->name, Tcl_GetString(objv[i]))) {
                    Tcl_Obj *objPtr;

                    objPtr = Tcl_NewStringObj(argPtr->name, -1);
                    Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
                    break;
                }
            }
        }
    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ParseOp --
 *
 *      parserName parse argList ?varName?
 *
 *---------------------------------------------------------------------------
 */
static int
ParseOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;
    Blt_Chain argChain;
    Tcl_Obj *listObjPtr;
    Blt_ChainLink link;
    int result;

    InitParser(interp, parserPtr);
    argChain = CreateArgumentChain(interp, objv[2]);
    result = ParseArguments(interp, parserPtr, argChain);
    if (result != TCL_OK) {
        goto error;
    }
    result = TCL_ERROR;
    if ((parserPtr->flags & EXCLUSIONS) &&
        (CheckExclusions(interp, parserPtr) != TCL_OK)) {
        goto error;
    }
    /* Return a list of any leftover arguments. */
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
    for (link = Blt_Chain_FirstLink(argChain); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Tcl_Obj *objPtr;
        
        objPtr = Blt_Chain_GetValue(link);
        Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    }
    if ((objc == 4) || (parserPtr->flags & UPDATE_VARIABLES)) {
        Tcl_Obj *objPtr;

        objPtr = (objc == 4) ? objv[3] : NULL;
        if (UpdateVariables(interp, parserPtr, objPtr) != TCL_OK) {
            goto error;
        }
    }
    Blt_Chain_Destroy(argChain);
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
 error:
    Blt_Chain_Destroy(argChain);
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * ResetOp --
 *
 *      parserName reset ?argName ...?
 *
 *---------------------------------------------------------------------------
 */
static int
ResetOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;
    int i;

    if (objc == 2) {
        return ResetArguments(interp, parserPtr);
    }
    for (i = 2; i < objc; i++) {
        Argument *argPtr;
        if (GetArgumentFromObj(interp, parserPtr, objv[2], &argPtr) != TCL_OK) {
            return TCL_ERROR;
        } 
        if (ResetArgument(interp, argPtr) != TCL_OK) {
            return TCL_ERROR;
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RestoreOp --
 *
 *      Restores the current values for the named arguments.
 *
 *      parserName restore list
 *
 *---------------------------------------------------------------------------
 */
static int
RestoreOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;
    int elc;
    Tcl_Obj **elv;
    int i;

    if (Tcl_ListObjGetElements(interp, objv[2], &elc, &elv) != TCL_OK) {
        return TCL_ERROR;
    }
    for (i = 0; i < elc; i += 2) {
        Argument *argPtr;

        if (GetArgumentFromObj(interp, parserPtr, elv[i], &argPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        if ((i + 1) == objc) {
            Tcl_AppendResult(interp, "missing value for argument \"",
                             SwitchName(argPtr), "\"", (char *)NULL);
            return TCL_ERROR;
        }
        if (SetValue(interp, argPtr, elv[i]) != TCL_OK) {
            return TCL_ERROR;
        }
    } 
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SaveOp --
 *
 *      Returns a list of name-value pairs of argument names and their
 *      current value.
 *
 *      parserName save 
 *
 *---------------------------------------------------------------------------
 */
static int
SaveOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Blt_ChainLink link;
    Parser *parserPtr = clientData;
    Tcl_Obj *listObjPtr;
    
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
    for (link = Blt_Chain_FirstLink(parserPtr->args); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Argument *argPtr, *destPtr;
        Tcl_Obj *objPtr;
        
        argPtr = Blt_Chain_GetValue(link);
        objPtr = Tcl_NewStringObj(argPtr->name, -1);
        Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
        destPtr = (argPtr->destPtr != NULL) ? argPtr->destPtr : argPtr;
        objPtr = destPtr->currentObjPtr;
        if (objPtr == NULL) {
            objPtr = DefaultValue(argPtr);
        }
        Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SetOp --
 *
 *      parserName set ?argName value ...?
 *
 *---------------------------------------------------------------------------
 */
static int
SetOp(ClientData clientData, Tcl_Interp *interp, int objc,
         Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;
    int i;

    for (i = 2; i < objc; i += 2) {
        Argument *argPtr;

        if (GetArgumentFromObj(interp, parserPtr, objv[i], &argPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        if ((i + 1) == objc) {
            Tcl_AppendResult(interp, "missing value for argument \"",
                             SwitchName(argPtr), "\"", (char *)NULL);
            return TCL_ERROR;
        }
        if (SetValue(interp, argPtr, objv[i]) != TCL_OK) {
            return TCL_ERROR;
        }
    } 
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ParserInstObjCmd --
 *
 *      This procedure is invoked to process commands on behalf of the
 *	argument parser object.
 *
 * Results:
 *      A standard TCL result.
 *
 * Side effects:
 *      See the user documentation.
 *
 *	parserName add argName 
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec parserInstOps[] =
{
    {"add",         2, AddOp,         3, 0, "argName ?switches ...?",},
    {"argument",    2, ArgumentOp,    3, 0, "argName args...",},
    {"cget",        2, CgetOp,        3, 3, "option",},
    {"configure",   2, ConfigureOp,   2, 0, "?value ...?",},
    {"currentdb",   1, CurrentOp,     3, 3, "argName",},
    {"delete",      1, DeleteOp,      2, 0, "?argName ...?",},
    {"exists",      1, ExistsOp,      3, 3, "argName",},
    {"get",         1, GetOp,         2, 4, "?argName? ?defValue?",},
    {"help",        1, HelpOp,        2, 0, "",},
    {"ischanged",   1, IsChangedOp,   3, 3, "argName",},
    {"names",       1, NamesOp,       2, 0, "?pattern ...?",},
    {"parse",       1, ParseOp,       3, 4, "argList ?varName?",},
    {"reset",	    1, ResetOp,	      2, 0, "?argName ...?",},
    {"restore",	    1, RestoreOp,     3, 3, "list",},
    {"save",	    1, SaveOp,        2, 2, "",},
    {"set",         1, SetOp,         2, 0, "?argName value ...?",},
};

static int numParserInstOps = sizeof(parserInstOps) / sizeof(Blt_OpSpec);

static int
ParserInstObjCmd(ClientData clientData, Tcl_Interp *interp, int objc,
		 Tcl_Obj *const *objv)
{
    Parser *parserPtr = clientData;
    Tcl_ObjCmdProc *proc;
    int result;

    proc = Blt_GetOpFromObj(interp, numParserInstOps, parserInstOps,
			    BLT_OP_ARG1, objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    Tcl_Preserve(parserPtr);
    result = (*proc) (clientData, interp, objc, objv);
    Tcl_Release(parserPtr);
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * ParserInstDeleteProc --
 *
 *      Deletes the command associated with the parser.  This is called
 *      only when the command associated with the parser is destroyed.
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static void
ParserInstDeleteProc(ClientData clientData)
{
    Parser *parserPtr = clientData;
    
    DestroyParser(parserPtr);
}


/*
 *---------------------------------------------------------------------------
 *
 * ParserCreateOp --
 *
 *      blt::parseargs create ?parserName? ?switches...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ParserCreateOp(ClientData clientData, Tcl_Interp *interp, int objc,
	       Tcl_Obj *const *objv)
{
    const char *string;
    Parser *parserPtr;

    string = NULL;
    if (objc >= 3) {
        const char *thirdArg;

        thirdArg = Tcl_GetString(objv[2]);
        if (thirdArg[0] != '-') {
            objc--, objv++;             /* Remove the string  */
            string = thirdArg;
        }
    }
    parserPtr = NewParser(clientData, interp, string);
    if (parserPtr == NULL) {
        return TCL_ERROR;
    }
    if (Blt_ParseSwitches(interp, cmdSpecs, objc - 2 , objv + 2, 
        parserPtr, BLT_SWITCH_INITIALIZE) < 0) {
        DestroyParser(parserPtr);
        return TCL_ERROR;
    }
    Tcl_SetStringObj(Tcl_GetObjResult(interp), parserPtr->name, -1);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ParserDestroyOp --
 *
 *      blt::parseargs destroy ?parserName...? 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ParserDestroyOp(ClientData clientData, Tcl_Interp *interp, int objc,
		Tcl_Obj *const *objv)
{
    ParseArgsCmdInterpData *dataPtr = clientData;
    int i;

    for (i = 2; i < objc; i++) {
        Parser *parserPtr;
        const char *string;

        string = Tcl_GetString(objv[i]);
        parserPtr = GetParser(dataPtr, interp, string);
        if (parserPtr == NULL) {
            Tcl_AppendResult(interp, "can't find a parser named \"", string,
                             "\"", (char *)NULL);
            return TCL_ERROR;
        }
        Tcl_DeleteCommandFromToken(interp, parserPtr->cmdToken);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ParserExistsOp --
 *
 *      blt::parseargs exists parserName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ParserExistsOp(ClientData clientData, Tcl_Interp *interp, int objc,
             Tcl_Obj *const *objv)
{
    Parser *parserPtr;
    ParseArgsCmdInterpData *dataPtr = clientData;
    const char *string;
    int state;
    
    string = Tcl_GetString(objv[3]);
    parserPtr = GetParser(dataPtr, interp, string);
    state = (parserPtr != NULL);
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ParserNamesOp --
 *
 *      blt::parseargs names ?pattern ...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ParserNamesOp(ClientData clientData, Tcl_Interp *interp, int objc,
            Tcl_Obj *const *objv)
{
    ParseArgsCmdInterpData *dataPtr = clientData;
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;
    Tcl_Obj *listObjPtr;
    Tcl_DString ds;

    Tcl_DStringInit(&ds);
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
    for (hPtr = Blt_FirstHashEntry(&dataPtr->parserTable, &iter); hPtr != NULL; 
        hPtr = Blt_NextHashEntry(&iter)) {
        Blt_ObjectName objName;
        Parser *parserPtr;
        const char *qualName;
        Tcl_Obj *objPtr;
        int match;
        int i;
        
        parserPtr = Blt_GetHashValue(hPtr);
        objName.name = Tcl_GetCommandName(interp, parserPtr->cmdToken);
        objName.nsPtr = Blt_GetCommandNamespace(parserPtr->cmdToken);
        qualName = Blt_MakeQualifiedName(&objName, &ds);
        match = (objc == 2);
        for (i = 2; i < objc; i++) {
            if (Tcl_StringMatch(qualName, Tcl_GetString(objv[i]))) {
                match = TRUE;
                break;
            }
        }
        if (match) {
            objPtr = Tcl_NewStringObj(qualName, -1);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
        }
    }
    Tcl_SetObjResult(interp, listObjPtr);
    Tcl_DStringFree(&ds);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ParserArgsCmd --
 *
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec parserCmdOps[] =
{
    {"create",  1, ParserCreateOp,  2, 0, "?parserName?",},
    {"destroy", 1, ParserDestroyOp, 2, 0, "?parserName ...?",},
    {"exists",  1, ParserExistsOp,  3, 3, "parserName",},
    {"names",   1, ParserNamesOp,   2, 3, "?pattern ...?",},
};

static int numParserCmdOps = sizeof(parserCmdOps) / sizeof(Blt_OpSpec);

/*ARGSUSED*/
static int
ParseArgsCmd(ClientData clientData, Tcl_Interp *interp, int objc,
           Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numParserCmdOps, parserCmdOps, BLT_OP_ARG1,
			    objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_ParseArgsCmdInitProc --
 *
 *      This procedure is invoked to initialize the "parseargs" command.
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
Blt_ParseArgsCmdInitProc(Tcl_Interp *interp)
{
    static Blt_CmdSpec cmdSpec = { 
        "parseargs", ParseArgsCmd, 
    };
    cmdSpec.clientData = GetParseArgsCmdInterpData(interp);
    return Blt_InitCmd(interp, "::blt", &cmdSpec);
}

#endif /* NO_PARSEARGS */
