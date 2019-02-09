/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * bltTreeJson.c --
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

#include <bltInt.h>

#include "config.h"
#ifdef HAVE_CTYPE_H
  #include <ctype.h>
#endif /* HAVE_CTYPE_H */

#include <setjmp.h>
#include <string.h>

#include <tcl.h>

#define TRUE    1
#define FALSE   0
#define DEBUG   0

/*
 * The macro below is used to modify a "char" value (e.g. by casting it to
 * an unsigned character) so that it can be used safely with macros such as
 * isspace.
 */
#define UCHAR(c) ((unsigned char) (c))

static const char *tokens[] = {
    "eof", "???", "string", "number", "boolean", "null",
    "[", "]", "{", "}", ",", ":"
};

enum ParseTokens {
    JSON_EOF=-1, JSON_UNKNOWN, JSON_STRING, JSON_NUMBER, JSON_BOOLEAN, 
    JSON_NULL, JSON_OPEN_ARRAY, JSON_CLOSE_ARRAY, JSON_OPEN_OBJECT, 
    JSON_CLOSE_OBJECT, JSON_COMMA, JSON_COLON,
};
        
DLLEXPORT extern Tcl_AppInitProc Blt_TreeJsonInit;
DLLEXPORT extern Tcl_AppInitProc Blt_TreeJsonSafeInit;

static Blt_TreeImportProc ImportJsonProc;
static Blt_TreeExportProc ExportJsonProc;

/*
 * Format       Import          Export
 * json         file/data       file/data
 * html         file/data       file/data
 *
 * $tree import json $node fileName -data dataString 
 * $table export json $node -file defaultFileName 
 * $tree import html $node -file fileName -data dataString 
 * $table export html $node -file defaultFileName 
 */

#define BUFFER_SIZE             (1<<12)

/*
 * JsonReader --
 */
typedef struct {
    Blt_Tree tree;                      /* Tree where information will be
                                         * stored. */
    Blt_TreeNode root;                  /* Root node where data will be
                                         * imported.  The default is the
                                         * root of the tree. */
    Tcl_Interp *interp;                 /* TCL Interpreter associated with
                                         * command importing data. */
    Tcl_Obj *fileObjPtr;                /* Name of file containing JSON
                                         * data.  Indicates to read data
                                         * from file. */
    Tcl_Obj *dataObjPtr;                /* Data object holding the string
                                         * to be parsed. */
    Tcl_Channel channel;                /* If non-NULL, channel to file. */
    unsigned int flags;                 /* Flags.  */
    int token;                          /* The last token parsed. */
    const char *bufferPtr;              /* (data only) Points to the given
                                         * string contain data. */
    int mark;                           /* Current position in the buffer
                                         * for reading. */
    int fill;                           /* Current position in the buffer
                                         * for writing. */
    char lastChar;                      /* Last character read.  */
    Blt_DBuffer word;                   /* Temporary storage holding the
                                         * contents of the last parsed
                                         * word. */
    char buffer[BUFFER_SIZE];           /* (file only).  Buffer holding
                                         * data from file. */
    int lineNum;
    jmp_buf jmpbuf;
    Tcl_DString errors;
    int numErrors;
} JsonReader;

static Blt_SwitchParseProc TreeNodeSwitchProc;

static Blt_SwitchCustom nodeSwitch = {
    TreeNodeSwitchProc, NULL, NULL, (ClientData)0,
};

static Blt_SwitchSpec importSpecs[] = 
{
    {BLT_SWITCH_OBJ,      "-data",              "data", (char *)NULL,
        Blt_Offset(JsonReader, dataObjPtr),     0, 0},
    {BLT_SWITCH_OBJ,      "-file",              "fileName", (char *)NULL,
        Blt_Offset(JsonReader, fileObjPtr),     0, 0},
    {BLT_SWITCH_CUSTOM,   "-root",              "node", (char *)NULL,
        Blt_Offset(JsonReader, root),   0, 0, &nodeSwitch},
    {BLT_SWITCH_END}
};

/*
 * JsonWriter --
 */
typedef struct {
    Tcl_Obj *fileObjPtr;
    Tcl_Obj *dataObjPtr;
    Blt_TreeNode root;
    Blt_Tree tree;
    int indent;                         /* Current indent level. */

    /* Private fields. */
    Tcl_Interp *interp;
    unsigned int flags;
    Tcl_Channel channel;                /* If non-NULL, channel to write
                                         * output to. */
    Blt_DBuffer dbuffer;
    Tcl_DString dString;                /* Used to hold translated string
                                         * for writing.*/
} JsonWriter;

#define JSON_TREE       (1<<0)          /* Indicates that the tree has
                                         * special characteristics of tree
                                         * that was imported. */

static Blt_SwitchSpec exportSpecs[] = 
{
    {BLT_SWITCH_OBJ,      "-data",              "data",     (char *)NULL,
        Blt_Offset(JsonWriter, dataObjPtr), 0, 0},
    {BLT_SWITCH_OBJ,      "-file",              "fileName", (char *)NULL,
        Blt_Offset(JsonWriter, fileObjPtr), 0, 0},
    {BLT_SWITCH_CUSTOM,   "-root",              "node",     (char *)NULL,
        Blt_Offset(JsonWriter, root),   0, 0, &nodeSwitch},
    {BLT_SWITCH_BITS_NOARG, "-jsontree", "", (char *)NULL,
        Blt_Offset(JsonWriter, flags), 0, JSON_TREE},
    {BLT_SWITCH_END}
};

static void ParseValue(JsonReader *readerPtr, Blt_TreeNode node, 
                       const char *string);
static void ParseArray(JsonReader *readerPtr, Blt_TreeNode node);
static void ParseObject(JsonReader *readerPtr, Blt_TreeNode node);
static int  JsonWriteNode(Tcl_Interp *interp, Blt_Tree tree, 
        Blt_TreeNode parent, JsonWriter *writerPtr, int noLabel);

/*
 *---------------------------------------------------------------------------
 *
 * TreeNodeSwitchProc --
 *
 *      Convert a Tcl_Obj representing a node number into its integer value.
 *
 * Results:
 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
TreeNodeSwitchProc(
    ClientData clientData,              /* Not used. */
    Tcl_Interp *interp,                 /* Interpreter to send results back
                                         * to */
    const char *switchName,             /* Not used. */
    Tcl_Obj *objPtr,                    /* String representation */
    char *record,                       /* Structure record */
    int offset,                         /* Offset to field in structure */
    int flags)                          /* Not used. */
{
    Blt_TreeNode *nodePtr = (Blt_TreeNode *)(record + offset);
    Blt_Tree tree  = clientData;

    return Blt_Tree_GetNodeFromObj(interp, tree, objPtr, nodePtr);
}

static const char *
LastToken(JsonReader *readerPtr)
{
    return tokens[readerPtr->token+1];
}

static void
JsonError(JsonReader *readerPtr, const char *fmt, ...)
{
    char string[BUFSIZ+4];
    int length;
    va_list args;

    va_start(args, fmt);
    Blt_FmtString(string, 200, "line %d: ", readerPtr->lineNum);
    Tcl_DStringAppend(&readerPtr->errors, string, -1);
    length = vsnprintf(string, BUFSIZ, fmt, args);
    if (length > BUFSIZ) {
        strcat(string, "...");
    }
    Tcl_DStringAppend(&readerPtr->errors, string, -1);
    va_end(args);
    longjmp(readerPtr->jmpbuf, 0);
}

static int
GetNextChar(JsonReader *readerPtr)
{
    ssize_t numBytes;
    char c;

    if (readerPtr->mark < readerPtr->fill) {
        c = readerPtr->bufferPtr[readerPtr->mark];
        readerPtr->lastChar = readerPtr->bufferPtr[readerPtr->mark];
        if (c == '\n') {
            readerPtr->lineNum++;
        }
        readerPtr->mark++;
        return c;
    } 
    if (readerPtr->channel == NULL) {
        return 0;                       /* EOF on string data. */
    }
    if (Tcl_Eof(readerPtr->channel)) {
        return 0;                       /* EOF on file data. */
    }
    readerPtr->mark = 0;
    numBytes = Tcl_Read(readerPtr->channel, readerPtr->buffer, BUFFER_SIZE);
    if (numBytes < 0) {
        if (Tcl_Eof(readerPtr->channel)) {
            return 0;
        }
        JsonError(readerPtr, "unexpected EOF on channel");
    }
   readerPtr->fill = numBytes;
   readerPtr->lastChar = readerPtr->buffer[0];
   c = readerPtr->buffer[0];
   if (c == '\n') {
       readerPtr->lineNum++;
   }
   readerPtr->mark++;
   return c;
}

static void
PushBackChar(JsonReader *readerPtr)
{
    readerPtr->mark--;
}

static const char *
GetQuotedString(JsonReader *readerPtr)
{
    Blt_DBuffer_SetLength(readerPtr->word, 0);
    for (;;) {
        char c;

        c = GetNextChar(readerPtr);
        if (c == '\\') {
            c = GetNextChar(readerPtr);
            switch (c) {
            case 'b': 
                c = '\b';  break;
            case 'f': 
                c = '\f';  break;
            case 'n': 
                c = '\n';  break;
            case 't': 
                c = '\t';  break;
            case 'r': 
                c = '\r';  break;
            case '/': 
                c = '/';   break;
            case '\\': 
                c = '\\';  break;
            case '"': 
                c = '"';   break;
            case 'u': 
                {
                    unsigned int value, i;
                    size_t numBytes;
                    char buf[5];

                    value = 0;
                    for (i = 0; i < 4; i++) {
                        c = GetNextChar(readerPtr);
                        if (!isxdigit(c)) {
                            JsonError(readerPtr, 
                                "expected hex digit but got '%c'", c);
                        }
                        value = (value << 4) | c;
                    }
                    numBytes = Tcl_UniCharToUtf(value, buf);
                    Blt_DBuffer_AppendString(readerPtr->word, buf, numBytes);
                    continue;
                }
            default:
                JsonError(readerPtr, "unknown escape character '%c'", c);
            }
        } else if (c == '"') {
            break;
        } 
        if (c == '\0') {
            JsonError(readerPtr, "unclosed quoted string");
        }
        Blt_DBuffer_AppendByte(readerPtr->word, c);
    } 
    Blt_DBuffer_AppendByte(readerPtr->word, 0);
    return (char *)Blt_DBuffer_Bytes(readerPtr->word);
}

static void
NextToken(JsonReader *readerPtr)
{
    char c;

    readerPtr->token = JSON_EOF;
    do {
        c = GetNextChar(readerPtr);
        if (c == '\0') {
            readerPtr->token = JSON_EOF;
            return;
        }
    } while (isspace(c));
    switch (c) {
    case '"': 
        GetQuotedString(readerPtr);
        readerPtr->token = JSON_STRING;         
        break;
    case '[': 
        readerPtr->token = JSON_OPEN_ARRAY;             
        break;
    case ']': 
        readerPtr->token = JSON_CLOSE_ARRAY;            
        break;
    case '{': 
        readerPtr->token = JSON_OPEN_OBJECT;            
        break;
    case '}': 
        readerPtr->token = JSON_CLOSE_OBJECT;           
        break;
    case ',': 
        readerPtr->token = JSON_COMMA;          
        break;
    case ':': 
        readerPtr->token = JSON_COLON;          
        break;
    default: 
        if (c == 'n') {
            Blt_DBuffer_SetLength(readerPtr->word, 0);
            do {
                Blt_DBuffer_AppendByte(readerPtr->word, c);
                c = GetNextChar(readerPtr);
            } while (isalpha(c));
            PushBackChar(readerPtr);
            readerPtr->token = JSON_NULL;
        } else if ((c == 'f') || (c == 't')) {
            Blt_DBuffer_SetLength(readerPtr->word, 0);
            do {
                Blt_DBuffer_AppendByte(readerPtr->word, c);
                c = GetNextChar(readerPtr);
            } while (isalpha(c));
            PushBackChar(readerPtr);
            readerPtr->token = JSON_BOOLEAN;
        } else if ((isdigit(c)) || (c == '-') || (c == '.')) {
            /* Assume that JSON file is correct and allow anything that
             * looks like a number. */
            Blt_DBuffer_SetLength(readerPtr->word, 0);
            do {
                Blt_DBuffer_AppendByte(readerPtr->word, c);
                c = GetNextChar(readerPtr);
            } while ((!isspace(c)) && (c != ',') && (c != ']') && (c != '}'));
            PushBackChar(readerPtr);
            readerPtr->token =  JSON_NUMBER;
        } else {
#if DEBUG
            fprintf(stderr, "unknown token c=%d\n", c);
#endif
            readerPtr->token = JSON_UNKNOWN;
        }
    }
#if DEBUG
    fprintf(stderr, "NextToken %s\n", LastToken(readerPtr));
#endif
}

static void
GetNumberValue(JsonReader *readerPtr, Blt_TreeNode parent, const char *name)
{
    Tcl_Obj *objPtr;
    double d;
    const char *string;
    Blt_TreeNode node;

#if DEBUG
    fprintf(stderr, "Enter GetNumberValue\n");
#endif
    node = Blt_Tree_CreateNode(readerPtr->tree, parent, name, NULL);
    objPtr = Blt_DBuffer_StringObj(readerPtr->word);
    string = Tcl_GetString(objPtr);
    if (Tcl_GetDoubleFromObj(readerPtr->interp, objPtr, &d) != TCL_OK) {
        JsonError(readerPtr, "%s", Tcl_GetStringResult(readerPtr->interp));
    }
    if (Blt_Tree_SetVariable(readerPtr->interp, readerPtr->tree, node, name, 
                objPtr) != TCL_OK) {
        JsonError(readerPtr, "can't set value \"%s\" to %s", name, string);
    }
#if DEBUG
    fprintf(stderr, "Leave GetNumberValue number=%s\n", Tcl_GetString(objPtr));
#endif
}

static void
GetBooleanValue(JsonReader *readerPtr, Blt_TreeNode parent, const char *name)
{
    Tcl_Obj *objPtr;
    int state;
    Blt_TreeNode node;
#if DEBUG
    fprintf(stderr, "Enter GetBooleanValue\n");
#endif
    node = Blt_Tree_CreateNode(readerPtr->tree, parent, name, NULL);
    objPtr = Blt_DBuffer_StringObj(readerPtr->word);
    if (Tcl_GetBooleanFromObj(readerPtr->interp, objPtr, &state) != TCL_OK) {
        JsonError(readerPtr, "%s", Tcl_GetStringResult(readerPtr->interp));
    }
    if (Blt_Tree_SetVariable(readerPtr->interp, readerPtr->tree, node, name, 
                          objPtr) != TCL_OK) {
        JsonError(readerPtr, "can't set value \"%s\" to \"%s\"", name, 
                  Tcl_GetString(objPtr));
    }
#if DEBUG
    fprintf(stderr, "Leave GetBooleanValue boolean=%s\n", 
            Tcl_GetString(objPtr));
#endif
}

static void
GetNullValue(JsonReader *readerPtr, Blt_TreeNode parent, const char *name)
{
    Tcl_Obj *objPtr;
    Blt_TreeNode node;

#if DEBUG
    fprintf(stderr, "Enter GetNullValue\n");
#endif
    node = Blt_Tree_CreateNode(readerPtr->tree, parent, name, NULL);
    objPtr = Blt_DBuffer_StringObj(readerPtr->word);
    Tcl_IncrRefCount(objPtr);
    if (strcmp(Tcl_GetString(objPtr), "null") != 0) {
        JsonError(readerPtr, "can't convert null \"%s\": %s", 
                  Tcl_GetString(objPtr), 
                  Tcl_GetStringResult(readerPtr->interp));
    }
    Tcl_DecrRefCount(objPtr);
    if (Blt_Tree_SetVariable(readerPtr->interp, readerPtr->tree, node, name, 
                          NULL) != TCL_OK) {
        JsonError(readerPtr, "can't set value \"%s\" to NULL", name);
    }
#if DEBUG
    fprintf(stderr, "Leave GetNullValue null=%s\n", Tcl_GetString(objPtr));
#endif
}

static void
GetStringValue(JsonReader *readerPtr, Blt_TreeNode parent, const char *name)
{
    Tcl_Obj *objPtr;
    Blt_TreeNode node;

#if DEBUG
    fprintf(stderr, "Enter GetStringValue\n");
#endif
    node = Blt_Tree_CreateNode(readerPtr->tree, parent, name, NULL);
    objPtr = Blt_DBuffer_StringObj(readerPtr->word);
    if (Blt_Tree_SetVariable(readerPtr->interp, readerPtr->tree, node, name, 
        objPtr) != TCL_OK) {
        JsonError(readerPtr, "can't set value \"%s\" to \"%s\"", name, 
                  Tcl_GetString(objPtr));
    }
#if DEBUG
    fprintf(stderr, "Leave GetStringValue string=%s\n", 
            Tcl_GetString(objPtr));
#endif
}

static void
ParseValue(JsonReader *readerPtr, Blt_TreeNode parent, const char *name)
{
#if DEBUG
    fprintf(stderr, "Enter ParseValue\n");
#endif
    switch (readerPtr->token) {
    case JSON_STRING:
        GetStringValue(readerPtr, parent, name);
        NextToken(readerPtr);           /* Move past string. */
        break;

    case JSON_NUMBER:
        GetNumberValue(readerPtr, parent, name);
        NextToken(readerPtr);           /* Move past number. */
        break;

    case JSON_BOOLEAN:
        GetBooleanValue(readerPtr, parent, name);
        NextToken(readerPtr);           /* Move past boolean. */
        break;

    case JSON_NULL:
        GetNullValue(readerPtr, parent, name);
        NextToken(readerPtr);           /* Move past null. */
        break;

    case JSON_OPEN_OBJECT:
        {
            Blt_TreeNode node;

            if (name != NULL) {
                node = Blt_Tree_CreateNode(readerPtr->tree, parent, name, NULL);
            } else {
                node = parent;
            }
            Blt_Tree_AddTag(readerPtr->tree, node, "json_object");
            ParseObject(readerPtr, node);
        }
        break;

    case JSON_OPEN_ARRAY:
        {
            Blt_TreeNode node;

            if (name != NULL) {
                node = Blt_Tree_CreateNode(readerPtr->tree, parent, name, NULL);
            } else {
                node = parent;
            }
            Blt_Tree_AddTag(readerPtr->tree, node, "json_array");
            ParseArray(readerPtr, node);
        }
        break;

    case JSON_EOF:
        JsonError(readerPtr, "unexpected EOF, expecting array value");
        break;

    default: 
        JsonError(readerPtr, "expected array value but got '%s'", 
                LastToken(readerPtr));
        break;
    }
#if DEBUG
    fprintf(stderr, "Leave ParseValue %s\n", LastToken(readerPtr));
#endif
}

static void
ParseNameValue(JsonReader *readerPtr, Blt_TreeNode parent)
{
    const char *name;
    Tcl_Obj *objPtr;

#if DEBUG
    fprintf(stderr, "Enter ParseNameValue\n");
#endif
    if (readerPtr->token == JSON_EOF) {
        JsonError(readerPtr, "unexpected EOF, should be name of value");
    }
    if (readerPtr->token != JSON_STRING) {
        JsonError(readerPtr, "expected value name but got '%s'", 
                  LastToken(readerPtr));
    }
    objPtr = Blt_DBuffer_StringObj(readerPtr->word);
    name = Tcl_GetString(objPtr);
    Tcl_IncrRefCount(objPtr);
#if DEBUG
    fprintf(stderr, "ParseNameValue: Got name (%s)\n", Tcl_GetString(objPtr));
#endif

    /* Look for colon. */
    NextToken(readerPtr);               /* Move past name. */
    if (readerPtr->token != JSON_COLON) {
        JsonError(readerPtr, "expected colon after name \"%s\" but got '%s'", 
                  name, LastToken(readerPtr));
    }
    NextToken(readerPtr);               /* Move past colon. */
    ParseValue(readerPtr, parent, name);
    Tcl_DecrRefCount(objPtr);
#if DEBUG
    fprintf(stderr, "Leave ParseNameValue %s\n", LastToken(readerPtr));
#endif
}

static void
ParseArray(JsonReader *readerPtr, Blt_TreeNode node)
{
    int count;

#if DEBUG
    fprintf(stderr, "Enter ParseArray %s\n", LastToken(readerPtr));
#endif
    if (readerPtr->token == JSON_EOF) {
        JsonError(readerPtr, "unexpected EOF, should be '['");
    }
    if (readerPtr->token != JSON_OPEN_ARRAY) {
        JsonError(readerPtr, "expected array open bracket but got '%s'", 
                  LastToken(readerPtr));
    }
    count = 0;
    NextToken(readerPtr);               /* Move past open bracket. */
    while (readerPtr->token != JSON_CLOSE_ARRAY) { 
        char string[200];
    
        count++;
        /* Array values must be objects with generated indices (node
         * labels) because you can have an array of objects or arrays. That
         * requires the data to have subnodes. */
        Blt_FmtString(string, 200, "_index%d", count);
        ParseValue(readerPtr, node, string);
        if (readerPtr->token == JSON_CLOSE_ARRAY) {
            break;
        }
        if (readerPtr->token == JSON_EOF) {
            JsonError(readerPtr, "unexpected EOF, should be ',' or ']'");
        }
        if (readerPtr->token != JSON_COMMA) {
            JsonError(readerPtr, 
                "expected comma or array close bracket but got '%s'",
                LastToken(readerPtr));
        }
        NextToken(readerPtr);           /* Move past comma. */
    }
    NextToken(readerPtr);               /* Move past close bracket. */
#if DEBUG
    fprintf(stderr, "Leave ParseArray %s\n", LastToken(readerPtr));
#endif
}

static void
ParseObject(JsonReader *readerPtr, Blt_TreeNode node)
{
#if DEBUG
    fprintf(stderr, "Enter ParseObject\n");
#endif
    if (readerPtr->token == JSON_EOF) {
        JsonError(readerPtr, "unexpected EOF, should be '{'");
    }
    if (readerPtr->token != JSON_OPEN_OBJECT) {
        JsonError(readerPtr, "expected open object brace but got '%s'", 
                  LastToken(readerPtr));
    }
    NextToken(readerPtr);               /* Move past open brace. */
    while (readerPtr->token != JSON_CLOSE_OBJECT) {
        ParseNameValue(readerPtr, node);
        if (readerPtr->token == JSON_CLOSE_OBJECT) {
            break;
        }
        if (readerPtr->token == JSON_EOF) {
            JsonError(readerPtr, "unexpected EOF, should be ',' or '}'");
        }
        if (readerPtr->token != JSON_COMMA) {
            JsonError(readerPtr, 
                "expected comma or close object brace but got '%s'", 
                LastToken(readerPtr));
        }
        NextToken(readerPtr);           /* Move past comma. */
    } 
    NextToken(readerPtr);               /* Move past close brace. */
#if DEBUG
    fprintf(stderr, "Leave ParseObject %s\n", LastToken(readerPtr));
#endif
}

static int
JsonImport(JsonReader *readerPtr, const char *fileName)
{
    Tcl_DStringInit(&readerPtr->errors);
    Tcl_DStringAppend(&readerPtr->errors, "error reading \"", -1);
    Tcl_DStringAppend(&readerPtr->errors, fileName, -1);
    Tcl_DStringAppend(&readerPtr->errors, "\": ", -1);

    if (setjmp(readerPtr->jmpbuf)) {
        Tcl_DStringResult(readerPtr->interp, &readerPtr->errors);
        return TCL_ERROR;
    }
    /* Look for opening curly brace. */
    NextToken(readerPtr);               /* Get first token. */
    ParseValue(readerPtr, readerPtr->root, NULL);

#ifdef notdef
    if (readerPtr->token == JSON_OPEN_OBJECT) {
        ParseObject(readerPtr, readerPtr->root);
    } else if (readerPtr->token == JSON_OPEN_ARRAY) {
        ParseArray(readerPtr, readerPtr->root);
    } else {
        return TCL_ERROR;
    }
#endif
    if (readerPtr->token != JSON_EOF) {
        JsonError(readerPtr, "expected root object or array but got '%s'",
                LastToken(readerPtr));
    }
    /* Find the opening curly brace.  */
    return TCL_OK;
} 


static void
JsonAppend(JsonWriter *writerPtr, const char *s)
{
    Blt_DBuffer_AppendString(writerPtr->dbuffer, s, -1);
}

static void
JsonFormat(JsonWriter *writerPtr, const char *fmt, ...)
{
    char string[BUFSIZ+4];
    int length;
    va_list args;

    va_start(args, fmt);
    length = vsnprintf(string, BUFSIZ, fmt, args);
    if (length > BUFSIZ) {
        strcat(string, "...");
        length += 3;
    }
    va_end(args);
    Blt_DBuffer_AppendString(writerPtr->dbuffer, string, length);
}

static int
JsonFlush(JsonWriter *writerPtr) 
{
    const char *line;
    size_t length;
    ssize_t numWritten;

    line = (const char *)Blt_DBuffer_Bytes(writerPtr->dbuffer);
    length = Blt_DBuffer_Length(writerPtr->dbuffer);
    numWritten = Tcl_Write(writerPtr->channel, line, length);
    if (numWritten != length) {
        Tcl_AppendResult(writerPtr->interp, "can't write json object: ",
                Tcl_PosixError(writerPtr->interp), (char *)NULL);
        return TCL_ERROR;
    }
    Blt_DBuffer_SetLength(writerPtr->dbuffer, 0);
    return TCL_OK;
}

static const char *
JsonTranslateString(JsonWriter *writerPtr, const char *s)
{
    const char *p;
    char *bp;
    int count;

    /* Get a count of the string need to hold the escaped characters. */
    count = 0;
    for (p = s; *p != '\0'; p++) {
        count++;
        switch (*p) {
        case '\n': 
        case '\t': 
        case '\b': 
        case '\f': 
        case '\r': 
        case '\\': 
            count++;
            break;
        }
    }
    Tcl_DStringSetLength(&writerPtr->dString, count + 2);
    bp = Tcl_DStringValue(&writerPtr->dString);
    *bp++ = '\"';
    for (p = s; *p != '\0'; p++) {
        switch (*p) {
        case '\n': 
            *bp++ = '\\';
            *bp++ = 'n';
            break;
        case '\t': 
            *bp++ = '\\';
            *bp++ = 't';
            break;
        case '\b': 
            *bp++ = '\\';
            *bp++ = 'b';
            break;
        case '\f': 
            *bp++ = '\\';
            *bp++ = 'f';
            break;
        case '\r': 
            *bp++ = '\\';
            *bp++ = 'r';
            break;
        case '\\': 
            *bp++ = '\\';
            *bp++ = '\\';
            break;
        default:
            *bp++ = *p;
        }
    }
    *bp++ = '\"';
    return Tcl_DStringValue(&writerPtr->dString);
}

static void
JsonIndent(JsonWriter *writerPtr)
{
    JsonFormat(writerPtr, "%*s", writerPtr->indent * 2, "");
}

static void
JsonWriteKey(JsonWriter *writerPtr, const char *s)
{
    JsonAppend(writerPtr, JsonTranslateString(writerPtr, s));
    JsonFormat(writerPtr, " : ");
}

static void
JsonStartComplexValue(JsonWriter *writerPtr, int isArray)
{
    if (isArray) {
        JsonFormat(writerPtr, "[\n");
    } else {
        JsonFormat(writerPtr, "{\n");
    }
    writerPtr->indent++;
}

static void
JsonEndComplexValue(JsonWriter *writerPtr, int isArray)
{
    writerPtr->indent--;
    JsonIndent(writerPtr);
    if (isArray) {
        JsonFormat(writerPtr, "]");
    } else {
        JsonFormat(writerPtr, "}");
    }
}

static void
JsonWriteNodeLabel(JsonWriter *writerPtr, const char *name)
{
    JsonIndent(writerPtr);
    JsonWriteKey(writerPtr, name);
}

static void
JsonWriteNull(JsonWriter *writerPtr)
{
    JsonFormat(writerPtr, "null");
}

static int
JsonWriteNumber(Tcl_Interp *interp, JsonWriter *writerPtr, Tcl_Obj *objPtr)
{
    double d;
    
    if (Tcl_GetDoubleFromObj(interp, objPtr, &d) != TCL_OK) {
        return TCL_ERROR;
    }
    JsonFormat(writerPtr, "%.15g", d);
    return TCL_OK;
}

static int
JsonWriteString(Tcl_Interp *interp, JsonWriter *writerPtr, 
                Tcl_Obj *objPtr)
{
    const char *string;

    string = Tcl_GetString(objPtr);
    JsonAppend(writerPtr, JsonTranslateString(writerPtr, string));
    return TCL_OK;
}

static int
JsonWriteBoolean(Tcl_Interp *interp, JsonWriter *writerPtr, Tcl_Obj *objPtr)
{
    int b;
                
    if (Tcl_GetBooleanFromObj(interp, objPtr, &b) != TCL_OK) {
        return TCL_ERROR;
    }
    JsonFormat(writerPtr, "%s", (b) ? "true" : "false");
    return TCL_OK;
}

static int
JsonWriteList(Tcl_Interp *interp, JsonWriter *writerPtr, Tcl_Obj *objPtr)
{
    int i;
    Tcl_Obj **objv;
    int objc;
    
    if (Tcl_ListObjGetElements(interp, objPtr, &objc, &objv) != TCL_OK){
        return TCL_ERROR;
    }
    JsonIndent(writerPtr);
    JsonStartComplexValue(writerPtr, TRUE);
    for (i = 0; i < objc; i++) {
        Tcl_Obj *objPtr;

        objPtr = objv[i];
        if (objPtr == NULL) {
            JsonWriteNull(writerPtr);
        } else if (objPtr->typePtr == NULL) {
            if (JsonWriteString(interp, writerPtr, objPtr) != TCL_OK) {
                return TCL_ERROR;
            }
        } else {
            const char *type;
            char c;
            int result;

            type = objPtr->typePtr->name;
            c = type[0];
            if ((c == 's') && (strcmp(type, "string") == 0)) {
                result = JsonWriteString(interp, writerPtr, objPtr);
            } else if ((c == 'l') && (strcmp(type, "long") == 0)) {
                result = JsonWriteNumber(interp, writerPtr, objPtr);
            } else if ((c == 'i') && (strcmp(type, "int") == 0)) {
                result = JsonWriteNumber(interp, writerPtr, objPtr);
            } else if ((c == 'd') && (strcmp(type, "double") == 0)) {
                result = JsonWriteNumber(interp, writerPtr, objPtr);
            } else if ((c == 'b') && (strcmp(type, "boolean") == 0)) {
                result = JsonWriteBoolean(interp, writerPtr, objPtr);
            } else if ((c == 'l') && (strcmp(type, "list") == 0)) {
                result = JsonWriteList(interp, writerPtr, objPtr);
            } else {
                result = JsonWriteString(interp, writerPtr, objPtr);
            }
            if (result != TCL_OK) {
                return TCL_ERROR;
            }
        }
        if (i < (objc - 1)) {
            JsonFormat(writerPtr, ", ");
        }
        JsonFormat(writerPtr, "\n");
    }
    JsonEndComplexValue(writerPtr, TRUE);
    return TCL_OK;
}


static int
JsonWriteValue(Tcl_Interp *interp, JsonWriter *writerPtr, Tcl_Obj *objPtr)
{
    if (objPtr == NULL) {
        JsonWriteNull(writerPtr);
    } else if (objPtr->typePtr == NULL) {
        if (JsonWriteString(interp, writerPtr, objPtr) != TCL_OK) {
            return TCL_ERROR;
        }
    } else {
        char c;
        const char *type;
        int result;

        type = objPtr->typePtr->name;
        c = type[0];
        if ((c == 's') && (strcmp(type, "string") == 0)) {
            result = JsonWriteString(interp, writerPtr, objPtr);
        } else if ((c == 'l') && (strcmp(type, "long") == 0)) {
            result = JsonWriteNumber(interp, writerPtr, objPtr);
        } else if ((c == 'i') && (strcmp(type, "int") == 0)) {
            result = JsonWriteNumber(interp, writerPtr, objPtr);
        } else if ((c == 'd') && (strcmp(type, "double") == 0)) {
            result = JsonWriteNumber(interp, writerPtr, objPtr);
        } else if ((c == 'b') && (strcmp(type, "boolean") == 0)) {
            result = JsonWriteBoolean(interp, writerPtr, objPtr);
        } else if ((c == 'l') && (strcmp(type, "list") == 0)) {
            result = JsonWriteList(interp, writerPtr, objPtr);
        } else {
            result = JsonWriteString(interp, writerPtr, objPtr);
        }
        if (result != TCL_OK) {
            return TCL_ERROR;
        }
    }
    return TCL_OK;
}

static int
JsonWriteNode(Tcl_Interp *interp, Blt_Tree tree, Blt_TreeNode parent, 
              JsonWriter *writerPtr, int noLabel)
{
    Blt_TreeUid key;
    Blt_TreeVariableIterator iter;
    Blt_TreeNode child;
    long count, lastEntry;
    int isArray;
    const char *label;

    isArray = FALSE;
    if (writerPtr->flags & JSON_TREE) {
        isArray = Blt_Tree_HasTag(tree, parent, "json_array");
    }
    label = Blt_Tree_NodeLabel(parent);
    /* 
     * In an imported JSON tree, the following structure is created.
     *  1. Simple values are variable in nodes by the same key. 
     *     This allows array and objects to be mixed in with simple
     *     values (like numbers, strings, booleans) with arrays and 
     *     objects.
     *  2. Arrays are tags by "json_array".  In this case, the child node
     *     labels are ignored and assumed to implied indices for the array.
     *
     *  Here we are detecting these conditions and reversing the
     *  transformation.
     */
    if ((writerPtr->flags & JSON_TREE) && (!isArray) && 
        (Blt_Tree_NodeDegree(parent) == 0) && 
        (Blt_Tree_NodeVariables(parent) == 1)) {
        Blt_TreeVariableIterator iter;
        Blt_TreeUid key;
        Tcl_Obj *valueObjPtr;
        
        key = Blt_Tree_FirstVariable(tree, parent, &iter);
        if (strcmp(key, label) == 0) {
            /* This is a simple value. */
            if (Blt_Tree_GetScalarVariableByUid(interp, tree, parent, key, 
                &valueObjPtr) != TCL_OK) {
                return TCL_ERROR;
            }
            JsonIndent(writerPtr);
            if (!noLabel) {
                JsonWriteKey(writerPtr, key);
            }
            JsonWriteValue(interp, writerPtr, valueObjPtr);
            return TCL_OK;
        }
    }
    if (!noLabel) {
        JsonWriteNodeLabel(writerPtr, label);
    }
    /* Save the current number of entries and count for the parent object. */

    lastEntry = Blt_Tree_NodeDegree(parent) + 
        Blt_Tree_NodeVariables(parent) - 1;
    if ((Blt_Tree_NodeVariables(parent) + Blt_Tree_NodeDegree(parent)) == 0) {
        if (isArray) {
            JsonFormat(writerPtr, "[]");
        } else {
            JsonFormat(writerPtr, "{}");
        }
        return TCL_OK;
    }
    if (noLabel) {
        JsonIndent(writerPtr);
    }
    JsonStartComplexValue(writerPtr, isArray);
    count = 0;                          /* Count the number of value and
                                         * objects */
    for (key = Blt_Tree_FirstVariable(tree, parent, &iter); key != NULL; 
         key = Blt_Tree_NextVariable(tree, &iter)) {
        Tcl_Obj *valueObjPtr;

        if (Blt_Tree_GetScalarVariableByUid(interp, tree, parent, key, 
                &valueObjPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        JsonIndent(writerPtr);
        if (!isArray) {
            JsonWriteKey(writerPtr, key);
        }
        JsonWriteValue(interp, writerPtr, valueObjPtr);
        if (count != lastEntry) {
            JsonFormat(writerPtr, ", ");
        }
        JsonFormat(writerPtr, "\n");
        count++;
    }

    for (child = Blt_Tree_FirstChild(parent); child != NULL; 
         child = Blt_Tree_NextSibling(child)) {
        if (JsonWriteNode(interp, tree, child, writerPtr, isArray) != TCL_OK) {
            return TCL_ERROR;
        }
        if (count != lastEntry) {
            JsonFormat(writerPtr, ", ");
        }
        JsonFormat(writerPtr, "\n");
        count++;
    }
#if DEBUG
    fprintf(stderr, "JsonCloseObject last=%d\n", last);
#endif
    JsonEndComplexValue(writerPtr, isArray);
    return TCL_OK;
}

static int
JsonExport(Tcl_Interp *interp, Blt_Tree tree, JsonWriter *writerPtr)
{
    if (JsonWriteNode(interp, tree, writerPtr->root, writerPtr, TRUE) 
        != TCL_OK) {
        return TCL_ERROR;
    }
    JsonFormat(writerPtr, "\n");
    if (writerPtr->channel != NULL) {
        return JsonFlush(writerPtr);
    }
    return TCL_OK;
}

static int
ImportJsonProc(Tcl_Interp *interp, Blt_Tree tree, int objc, 
               Tcl_Obj *const *objv)
{
    int result;
    JsonReader reader;
    const char *fileName;
    int closeChannel;

    memset(&reader, 0, sizeof(reader));
    closeChannel = FALSE;
    nodeSwitch.clientData = tree;
    reader.root = Blt_Tree_RootNode(tree);
    reader.flags = 0;
    reader.word = Blt_DBuffer_Create();
    if (Blt_ParseSwitches(interp, importSpecs, objc - 3, objv + 3, 
        &reader, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    result = TCL_ERROR;
    if ((reader.dataObjPtr != NULL) && (reader.fileObjPtr != NULL)) {
        Tcl_AppendResult(interp, "can't set both -file and -data switches",
                         (char *)NULL);
        goto error;
    }
    fileName = NULL;                    /* Suppress compiler warning. */
    if (reader.fileObjPtr != NULL) {
        fileName = Tcl_GetString(reader.fileObjPtr);
        if ((fileName[0] == '@') && (fileName[1] != '\0')) {
            int mode;
            
            reader.channel = Tcl_GetChannel(interp, fileName+1, &mode);
            if (reader.channel == NULL) {
                goto error;
            }
            if ((mode & TCL_READABLE) == 0) {
                Tcl_AppendResult(interp, "channel \"", fileName, 
                        "\" not opened for reading", (char *)NULL);
                goto error;
            }
        } else {
            closeChannel = TRUE;
            reader.channel = Tcl_OpenFileChannel(interp, fileName, "r", 0666);
            if (reader.channel == NULL) {
                goto error;             /* Can't open export file. */
            }
        }
        reader.bufferPtr = reader.buffer;
        reader.fill = 0;
        reader.mark = 0;
    } else if (reader.dataObjPtr != NULL) {
        int length;
        const char *string;

        fileName="data";
        string = Tcl_GetStringFromObj(reader.dataObjPtr, &length);
        reader.bufferPtr = string;
        reader.channel = NULL;
        reader.mark = 0;
        reader.fill = length;
    } else {
        Tcl_AppendResult(interp, "must specify either -file or -data switch",
                (char *)NULL);
        goto error;
    }        
    reader.lineNum = 1;
    reader.tree = tree;
    reader.interp = interp;
    result = JsonImport(&reader, fileName); 
 error:
     Blt_DBuffer_Destroy(reader.word);
    if (closeChannel) {
        Tcl_Close(interp, reader.channel);
    }
    Blt_FreeSwitches(importSpecs, (char *)&reader, 0);
    return result;
}


static int
ExportJsonProc(
    Tcl_Interp *interp, 
    Blt_Tree tree, 
    int objc, 
    Tcl_Obj *const *objv)
{
    JsonWriter writer;
    Tcl_Channel channel;
    int closeChannel;
    int result;

    closeChannel = FALSE;
    channel = NULL;

    memset(&writer, 0, sizeof(writer));
    nodeSwitch.clientData = tree;
    writer.root = Blt_Tree_RootNode(tree);
    if (Blt_ParseSwitches(interp, exportSpecs, objc - 3 , objv + 3, 
        &writer, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    result = TCL_ERROR;
    Tcl_DStringInit(&writer.dString);
    if (writer.fileObjPtr != NULL) {
        const char *fileName;

        closeChannel = TRUE;
        fileName = Tcl_GetString(writer.fileObjPtr);
        if ((fileName[0] == '@') && (fileName[1] != '\0')) {
            int mode;
            
            channel = Tcl_GetChannel(interp, fileName+1, &mode);
            if (channel == NULL) {
                goto error;
            }
            if ((mode & TCL_WRITABLE) == 0) {
                Tcl_AppendResult(interp, "channel \"", fileName, 
                        "\" not opened for writing", (char *)NULL);
                goto error;
            }
            closeChannel = FALSE;
        } else {
            channel = Tcl_OpenFileChannel(interp, fileName, "w", 0666);
            if (channel == NULL) {
                goto error;     /* Can't open export file. */
            }
        }
    }
    writer.tree = tree;
    writer.interp = interp;
    writer.dbuffer = Blt_DBuffer_Create();
    writer.channel = channel;
    result = JsonExport(interp, tree, &writer);
    if (result != TCL_OK) {
        goto error;
    }
    if (writer.dataObjPtr != NULL) {
        Tcl_Obj *objPtr;

        /* Write the image into the designated TCL variable. */
        objPtr = Tcl_ObjSetVar2(interp, writer.dataObjPtr, NULL, 
                Blt_DBuffer_ByteArrayObj(writer.dbuffer), TCL_LEAVE_ERR_MSG);
        result = (objPtr == NULL) ? TCL_ERROR : TCL_OK;
    } else if (writer.channel == NULL) {
        Tcl_SetObjResult(interp, Blt_DBuffer_StringObj(writer.dbuffer));
    }
 error:
    if (writer.dbuffer != NULL) {
        Blt_DBuffer_Destroy(writer.dbuffer);
    }
    Tcl_DStringFree(&writer.dString);
    if (closeChannel) {
        Tcl_Close(interp, channel);
    }
    Blt_FreeSwitches(exportSpecs, (char *)&writer, 0);
    return result;
}

int 
Blt_TreeJsonInit(Tcl_Interp *interp)
{
#ifdef USE_TCL_STUBS
    if (Tcl_InitStubs(interp, TCL_VERSION_COMPILED, PKG_ANY) == NULL) {
        return TCL_ERROR;
    };
#endif
#ifdef USE_BLT_STUBS
    if (Blt_InitTclStubs(interp, BLT_VERSION, PKG_EXACT) == NULL) {
        return TCL_ERROR;
    };
#else
    if (Tcl_PkgRequire(interp, "blt_tcl", BLT_VERSION, PKG_EXACT) == NULL) {
        return TCL_ERROR;
    }
#endif    
    if (Tcl_PkgProvide(interp, "blt_tree_json", BLT_VERSION) != TCL_OK) { 
        return TCL_ERROR;
    }
    return Blt_Tree_RegisterFormat(interp,
        "json",                         /* Name of format. */
        ImportJsonProc,                 /* Import procedure. */
        ExportJsonProc);                /* Export procedure. */

}

int 
Blt_TreeJsonSafeInit(Tcl_Interp *interp)
{
    return Blt_TreeJsonInit(interp);
}

