/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * bltTreeXml.c --
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

/* -include |a|b|  include |a|b, plus all descendants.
   -include |a|b    include |a|b, no descendants.
   -include |*|b|  include any b off of top, plus all descendants.
   -include |*|b    include any b off of top, no descendants.
   -include b|      include any b, plus all descendants.
   -include b       include any b, no descendants.
   -exclude b       exclude any a, plus descendants.
   -exclude |a|b    exclude |a|b, plus descendants.

   $tree import xml -file file \
   -include |library|device
   -include |library|sector
   -include |library|*_subsystem
   -include |library|esram_type1
   -include |library|sdm_type1
   -include |library|gpio
   -include |library|pio
   -include |library|sip
   -include |library|design|instance

   $tree apply 0 -command getUid


 */
#include <bltInt.h>
#include "config.h"

#ifdef HAVE_CTYPE_H
  #include <ctype.h>
#endif /* HAVE_CTYPE_H */

#ifdef HAVE_STDDEF_H
  #include <stddef.h>
#endif /* HAVE_STDDEF_H */

#include <tcl.h>
#include <bltSwitch.h>
#include <bltAssert.h>
#include <bltAlloc.h>
#include <bltTree.h>
#include <string.h>


#define TRUE    1
#define FALSE   0
/*
 * The macro below is used to modify a "char" value (e.g. by casting
 * it to an unsigned character) so that it can be used safely with
 * macros such as isspace.
 */
#define UCHAR(c) ((unsigned char) (c))

DLLEXPORT extern Tcl_AppInitProc Blt_TreeXmlInit;
DLLEXPORT extern Tcl_AppInitProc Blt_TreeXmlSafeInit;

static Blt_TreeImportProc ImportXmlProc;
static Blt_TreeExportProc ExportXmlProc;

/*
 * Format       Import          Export
 * xml          file/data       file/data
 * html         file/data       file/data
 *
 * $tree import xml $node fileName -data dataString 
 * $table export xml $node -file defaultFileName 
 * $tree import html $node -file fileName -data dataString 
 * $table export html $node -file defaultFileName 
 */


#define SYM_BASEURI       "#baseuri"
#define SYM_BYTEIDX       "#byteindex"
#define SYM_CDATA         "#cdata"
#define SYM_COLNO         "#column"
#define SYM_COMMENT       "#comment"
#define SYM_LINENO        "#line"
#define SYM_NS            "#namespace"
#define SYM_NOTATION      "#notation"
#define SYM_PI            "#pi:"
#define SYM_PUBID         "#publicid"
#define SYM_SYSID         "#systemid"
#define SYM_VERSION       "#version"
#define SYM_ENCODING      "#encoding"
#define SYM_STANDALONE    "#standalone"

static Blt_SwitchFreeProc FreePatternProc;
static Blt_SwitchParseProc PatternSwitchProc;
static Blt_SwitchParseProc TreeNodeSwitchProc;

static Blt_SwitchCustom nodeSwitch = {
    TreeNodeSwitchProc, NULL, NULL, (ClientData)0,
};

static Blt_SwitchCustom patternSwitch = {
    PatternSwitchProc, NULL, FreePatternProc, (ClientData)0,
};

/*
 * XmlWriter --
 */
typedef struct {
    Tcl_Obj *fileObjPtr;
    Tcl_Obj *dataObjPtr;
    Blt_TreeNode root;

    /* Private fields. */
    Tcl_Interp *interp;
    unsigned int flags;
    Tcl_Channel channel;        /* If non-NULL, channel to write output to. */
    Blt_DBuffer dbuffer;
    int indent;
} XmlWriter;

#define LAST_END_TAG           (1<<2)
#define EXPORT_ROOT            (1<<3)
#define EXPORT_DECLARATION     (1<<4)

static Blt_SwitchSpec exportSwitches[] = 
{
    {BLT_SWITCH_OBJ, "-data", "data", (char *)NULL,
        Blt_Offset(XmlWriter, dataObjPtr), 0, 0},
    {BLT_SWITCH_BITS_NOARG, "-declaration", "", (char *)NULL,
        Blt_Offset(XmlWriter, flags),   0, EXPORT_DECLARATION},
    {BLT_SWITCH_OBJ, "-file", "fileName", (char *)NULL,
        Blt_Offset(XmlWriter, fileObjPtr), 0, 0},
    {BLT_SWITCH_INVERT_BITS_NOARG, "-hideroot", "", (char *)NULL,
        Blt_Offset(XmlWriter, flags),   0, EXPORT_ROOT},
    {BLT_SWITCH_INT_POS, "-indent", "number", (char *)NULL,
        Blt_Offset(XmlWriter, indent),  0, 0},
    {BLT_SWITCH_CUSTOM,   "-root",              "node", (char *)NULL,
        Blt_Offset(XmlWriter, root),    0, 0, &nodeSwitch},
    {BLT_SWITCH_END}
};


#ifdef HAVE_LIBEXPAT
#include <expat.h>

/*
 * XmlReader --
 */
typedef struct {
    Tcl_Interp *interp;                 /* Interpreter to report errors
                                         * back to. */
    XML_Parser parser;                  /* Xml parser. */
    Blt_Chain pathStack;                /* List of path elements to track
                                         * where we are in the XML tree. */
    Blt_Tree tree;                      /* BLT tree being populated. */
    Blt_TreeNode node;                  /* Current node. */
    Blt_TreeNode parent;                /* Current parent. */
    Blt_HashTable stringTable;          /* Hash table to map arbitrary
                                         * strings to shared TCL string 
                                         * objects. */
    const char *separator;
    unsigned int flags;
    Blt_TreeNode root;                  /* Root of tree where XML data is
                                         * added. */
    Tcl_Obj *fileObjPtr;                /* Name of file representing the
                                         * channel. */
    Tcl_Obj *dataObjPtr;                /* TCL object containing string of
                                         * XML data. */
    Blt_Chain includeList;              /* If non-NULL, list of include
                                         * patterns. */
    Blt_Chain excludeList;              /* If non-NULL, list of exclude
                                         * patterns. Include and exclude
                                         * patterns are mutually
                                         * exclusive. */
} XmlReader;


#define IMPORT_TRIMCDATA  (1<<0)
#define IMPORT_SINGLECDATA (1<<1)
#define IMPORT_OVERWRITE  (1<<2)

#define IMPORT_ATTRIBUTES (1L<<3)
#define IMPORT_BASEURI    (1L<<4)
#define IMPORT_CDATA      (1L<<5)
#define IMPORT_COMMENTS   (1L<<6)
#define IMPORT_DECL       (1L<<7)
#define IMPORT_DTD        (1L<<8)
#define IMPORT_LOCATION   (1L<<9)
#define IMPORT_PI         (1L<<10)
#define IMPORT_NS         (1L<<11)
#define IMPORT_EXTREF     (1L<<12)
#define IMPORT_ALL        (IMPORT_ATTRIBUTES | IMPORT_COMMENTS | IMPORT_CDATA |\
                           IMPORT_DTD | IMPORT_PI | IMPORT_LOCATION | \
                           IMPORT_BASEURI | IMPORT_DECL | IMPORT_EXTREF)

static Blt_SwitchSpec importSwitches[] = 
{
    {BLT_SWITCH_BITS_NOARG,  "-all",               "", (char *)NULL, 
        Blt_Offset(XmlReader, flags), 0, IMPORT_ALL},
    {BLT_SWITCH_BOOLEAN,  "-attributes",        "bool", (char *)NULL,
        Blt_Offset(XmlReader, flags),      0, IMPORT_ATTRIBUTES},
    {BLT_SWITCH_BOOLEAN,  "-cdata",             "bool", (char *)NULL,
        Blt_Offset(XmlReader, flags),      0, IMPORT_CDATA},
    {BLT_SWITCH_BOOLEAN,  "-comments",          "bool", (char *)NULL, 
        Blt_Offset(XmlReader, flags),      0, IMPORT_COMMENTS},
    {BLT_SWITCH_BOOLEAN,  "-convertcdata",    "bool", (char *)NULL,
        Blt_Offset(XmlReader, flags),      0, IMPORT_SINGLECDATA},
    {BLT_SWITCH_OBJ,      "-data",              "data", (char *)NULL,
        Blt_Offset(XmlReader, dataObjPtr),    0, 0},
    {BLT_SWITCH_BOOLEAN,  "-declaration",       "bool", (char *)NULL,
        Blt_Offset(XmlReader, flags),      0, IMPORT_DECL},
    {BLT_SWITCH_CUSTOM,   "-exclude",      "pattern", (char *)NULL,
        Blt_Offset(XmlReader, excludeList),  0, 0, &patternSwitch},
    {BLT_SWITCH_BOOLEAN,  "-extref",            "bool", (char *)NULL,
        Blt_Offset(XmlReader, flags),      0, IMPORT_EXTREF},
    {BLT_SWITCH_OBJ,      "-file",              "fileName", (char *)NULL,
        Blt_Offset(XmlReader, fileObjPtr),    0, 0},
    {BLT_SWITCH_CUSTOM,   "-include",      "pattern", (char *)NULL,
        Blt_Offset(XmlReader, includeList),  0, 0, &patternSwitch},
    {BLT_SWITCH_BOOLEAN,  "-locations",         "bool", (char *)NULL,
        Blt_Offset(XmlReader, flags),      0, IMPORT_LOCATION},
    {BLT_SWITCH_BOOLEAN,  "-namespace",         "bool", (char *)NULL,
        Blt_Offset(XmlReader, flags),      0, IMPORT_NS},
    {BLT_SWITCH_BOOLEAN,  "-overwrite", "bool", (char *)NULL,
        Blt_Offset(XmlReader, flags),      0, IMPORT_OVERWRITE},
    {BLT_SWITCH_BOOLEAN,  "-processinginstructions",  "bool", (char *)NULL,
        Blt_Offset(XmlReader, flags),      0, IMPORT_PI},
    {BLT_SWITCH_CUSTOM,   "-root",              "node", (char *)NULL,
        Blt_Offset(XmlReader, root),       0, 0, &nodeSwitch},
    {BLT_SWITCH_BOOLEAN,  "-trimwhitespace",    "bool", (char *)NULL,
        Blt_Offset(XmlReader, flags),      0, IMPORT_TRIMCDATA},
    {BLT_SWITCH_END}
};

/*
 * PathElement --
 *      
 *      Path elements form a chain representing a path of an XML element.
 *      Each element indicates the name of the XML element (may be
 *      duplicates), their possible inode in the BLT tree and actual BLT
 *      tree node (if allocated).  They also manage flags that determine 1)
 *      if descendants of the element are also included and 2) if the
 *      various parts of the element (CDATA, attributes, etc) should be
 *      stored.
 */
typedef struct {
    unsigned int flags;                 /* Bit flags: Indicates whether to
                                         * automatically include
                                         * descendants. */
    long inode;                         /* Speculative inode number. */
    Blt_TreeNode node;                  /* If non-NULL, actual node
                                         * created.*/
    char name[1];                       /* Name of the path element. Actual
                                         * size of string is added when
                                         * chain link is allocated. */
} PathElement;    

#define KEEP_DESCENDANTS        (1<<0)  /* Automatically include
                                         * descendants of current path. */
#define KEEP                    (1<<1)  /* Keep the node and its
                                         * descendants. */


/*
 *---------------------------------------------------------------------------
 *
 * SkipSeparators --
 *
 *      Moves the character pointer past one of more separators.
 *
 * Results:
 *      Returns the updates character pointer.
 *
 *---------------------------------------------------------------------------
 */
static const char *
SkipSeparators(const char *path, const char *sep, int length)
{
    while ((*path == *sep) && (strncmp(path, sep, length) == 0)) {
        path += length;
    }
    return path;
}

static const char *
FindNextSeparator(const char *path, const char *sep, int length)
{
    const char *p;

    for (p = path; *p != '\0'; p++) {
        if ((*p == *sep) || (strncmp(p, sep, length) == 0)) {
            return p;
        }
    }
    return NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * SplitPathPattern --
 *
 *      Returns a Tcl_Obj list of the path components.  Trailing and
 *      multiple separators are ignored.
 *
 *---------------------------------------------------------------------------
 */
static Blt_Chain
SplitPathPattern(Tcl_Interp *interp, Tcl_Obj *pathObjPtr, const char *sep)
{
    const char *path, *startPtr, *endPtr;
    int sepLen;
    Blt_Chain chain;
    int lastSeparator;

    chain = Blt_Chain_Create();
    if ((sep == NULL)  || (*sep == '\0')) {
        int numPatterns, i;
        Tcl_Obj **patterns;
        
        /* No separator. Patterns are a TCL list. */
        if (Tcl_ListObjGetElements(interp, pathObjPtr, &numPatterns, &patterns) 
            != TCL_OK) {
        }
        for (i = 0; i < numPatterns; i++) {
            const char *pattern;
            char *entry;
            Blt_ChainLink link;
            int numBytes;
            
            pattern = Tcl_GetStringFromObj(patterns[i], &numBytes);
            link = Blt_Chain_AllocLink(numBytes + 1);
            Blt_Chain_LinkBefore(chain, link, NULL);
            entry = Blt_Chain_GetValue(link);
            strncpy(entry, pattern, numBytes);
            entry[numBytes] = '\0';
        }
        return chain;
    }

    path = Tcl_GetString(pathObjPtr);
    sepLen = strlen(sep);
    /* Skip the first separator. */
    startPtr = SkipSeparators(path, sep, sepLen);
    if (startPtr > path) {
        Blt_ChainLink link;

        /* If we found a starting separator, add an empty entry to the 
         * pattern list. */
        link = Blt_Chain_NewLink();
        Blt_Chain_LinkAfter(chain, link, NULL);
        Blt_Chain_SetValue(link, NULL);
    }
    do {
        char *entry;
        Blt_ChainLink link;
        int numBytes;

        endPtr = FindNextSeparator(startPtr, sep, sepLen); 
        if (endPtr == NULL) {
            /* No more separators, this is the last path element. */
            lastSeparator = FALSE;
            numBytes = strlen(startPtr);
        } else {
            lastSeparator = TRUE;
            numBytes = endPtr - startPtr;
            endPtr = SkipSeparators(endPtr, sep, sepLen);
            if (endPtr == NULL) {
                break;
            }
        }
        link = Blt_Chain_AllocLink(numBytes + 1);
        Blt_Chain_LinkAfter(chain, link, NULL);
        entry = Blt_Chain_GetValue(link);
        strncpy(entry, startPtr, numBytes);
        entry[numBytes] = '\0';
        startPtr = endPtr;
    } while (startPtr != NULL);

    if (lastSeparator) {
        Blt_ChainLink link;

        /* If we found a trailing separator, add an empty entry to the 
         * pattern list. */
        link = Blt_Chain_NewLink();
        Blt_Chain_LinkAfter(chain, link, NULL);
        Blt_Chain_SetValue(link, NULL);
    }
    return chain;
}


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
TreeNodeSwitchProc(ClientData clientData, Tcl_Interp *interp,
                   const char *switchName, Tcl_Obj *objPtr, char *record,
                   int offset, int flags)
{
    Blt_TreeNode *nodePtr = (Blt_TreeNode *)(record + offset);
    Blt_Tree tree  = clientData;

    return Blt_Tree_GetNodeFromObj(interp, tree, objPtr, nodePtr);
}

/*ARGSUSED*/
static void
FreePatternProc(ClientData clientData, char *record, int offset,
                      int flags)
{
    Blt_Chain *chainPtr = (Blt_Chain *)(record + offset);

    if (*chainPtr != NULL) {
        Blt_Chain_Destroy(*chainPtr);
        *chainPtr = NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * PatternSwitchProc --
 *

 *      The return value is a standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
PatternSwitchProc(ClientData clientData, Tcl_Interp *interp,
                  const char *switchName, Tcl_Obj *objPtr, char *record,
                  int offset, int flags)
{
    Blt_Chain *chainPtr = (Blt_Chain *)(record + offset);
    XmlReader *readerPtr = (XmlReader *)record;
    Blt_Chain patternChain;

    if (*chainPtr == NULL) {
        *chainPtr = Blt_Chain_Create();
    }
    patternChain = SplitPathPattern(interp, objPtr, readerPtr->separator);
    Blt_Chain_Append(*chainPtr, patternChain);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * GetStringObj --
 *
 *      Returns a hashed Tcl_Obj from the given string. Many character
 *      strings in the XML tree will be the same.  We generate only one
 *      Tcl_Obj for each unique string.  Returns a reference counted
 *      Tcl_Obj.
 *
 * Results:
 *      The pointer to the string Tcl_Obj.
 *
 *---------------------------------------------------------------------------
 */
static Tcl_Obj *
GetStringObj(XmlReader *readerPtr, const char *string)
{
    Blt_HashEntry *hPtr;
    int isNew;

    hPtr = Blt_CreateHashEntry(&readerPtr->stringTable, string, &isNew);
    if (isNew) {
        Tcl_Obj *objPtr;

        objPtr = Tcl_NewStringObj(string, -1);
        Tcl_IncrRefCount(objPtr);
        Blt_SetHashValue(hPtr, objPtr);
        return objPtr;
    }
    return Blt_GetHashValue(hPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * GetBaseUri --
 *
 *      Searches for the closest SYM_BASEURI data field in the tree to the
 *      given node.  
 *
 * Results:
 *      The base URI is returned if found, otherwise NULL.
 *
 *---------------------------------------------------------------------------
 */
static const char *
GetBaseUri(XmlReader *readerPtr, Blt_TreeNode node)
{
    Blt_TreeNode top;

    top = Blt_Tree_ParentNode(readerPtr->root);
    do {
        if (Blt_Tree_ValueExists(readerPtr->tree, node, SYM_BASEURI)) {
            Tcl_Obj *objPtr;

            if (Blt_Tree_GetValue((Tcl_Interp *)NULL, readerPtr->tree, node, 
                        SYM_BASEURI, &objPtr) == TCL_OK) {
                return Tcl_GetString(objPtr);
            }
        }
        node = Blt_Tree_ParentNode(node);
    } while (node != top);
    return NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * SetLocation --
 *
 *      Adds the line, column, and byte index of the XML to the tree. This
 *      makes it easier to debug in the XML specific information came from.
 *
 *---------------------------------------------------------------------------
 */
static void
SetLocation(XmlReader *readerPtr, Blt_TreeNode node)
{
    Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, node, SYM_LINENO, 
        Tcl_NewIntObj(XML_GetCurrentLineNumber(readerPtr->parser)));
    Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, node, SYM_COLNO, 
        Tcl_NewIntObj(XML_GetCurrentColumnNumber(readerPtr->parser)));
    Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, node, SYM_BYTEIDX, 
        Tcl_NewLongObj(XML_GetCurrentByteIndex(readerPtr->parser)));
}


/*
 *---------------------------------------------------------------------------
 *
 * TrimWhitespace --
 *
 *      Trims leading and trailing whitespace from all the CDATA nodes 
 *      in the tree.  This is done after the entire XML input has been
 *      processed.
 *
 *---------------------------------------------------------------------------
 */
static void
TrimWhitespace(XmlReader *readerPtr)
{
    Blt_TreeNode root, node, next;

    root = readerPtr->root;
    for (node = root; node != NULL; node = next) {
        next = Blt_Tree_NextNode(root, node);
        if (strcmp(Blt_Tree_NodeLabel(node), SYM_CDATA) == 0) {
            Tcl_Obj *objPtr;
            int length;
            const char *first, *last, *pend, *string;

            if (Blt_Tree_GetValue(readerPtr->interp, readerPtr->tree, node,
                        SYM_CDATA, &objPtr) != TCL_OK) {
                continue;
            }
            string = Tcl_GetStringFromObj(objPtr, &length);
            for (first = string, pend = string+length; first < pend; first++) {
                if (!isspace(*first)) {
                    break;
                }
            }
            for (last = pend; last > first; last--) {
                if (!isspace(*(last - 1))) {
                    break;
                }
            }
            if (last > first) {
                Tcl_Obj *newPtr;

                newPtr = Tcl_NewStringObj(first, last - first);
                Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, node,
                                  SYM_CDATA, newPtr);
            } else {
                /* Remove empty CDATA nodes */
                Blt_Tree_DeleteNode(readerPtr->tree, node);
            }
        }
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ConvertSingleCDATA --
 *
 *      Converts a node with a single CDATA child.
 *
 *---------------------------------------------------------------------------
 */
static void
ConvertSingleCDATA(XmlReader *readerPtr)
{
    Blt_TreeNode root, node, next;

    root = readerPtr->root;
    for (node = root; node != NULL; node = next) {
        next = Blt_Tree_NextNode(root, node);
        if (Blt_Tree_NodeDegree(node) == 1) {
            Blt_TreeNode child;
            
            child = Blt_Tree_FirstChild(node);
            /* If the node has only one child and it's a CDATA node, then
             * create a data field in the parent with the label of the node
             * and the CDATA value. */
            if (strcmp(Blt_Tree_NodeLabel(child), SYM_CDATA) == 0) {
                Blt_TreeNode parent;
                Tcl_Obj *objPtr;
                const char *label;

                if (Blt_Tree_GetValue(readerPtr->interp, readerPtr->tree, child,
                                      SYM_CDATA, &objPtr) != TCL_OK) {
                    continue;
                }
                parent = Blt_Tree_ParentNode(node);
                label = Blt_Tree_NodeLabel(node);
                if (!Blt_Tree_ValueExists(readerPtr->tree, parent, label)) {
                    Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree,
                        parent, label, objPtr);
                    next = Blt_Tree_NextNode(root, child);
                    Blt_Tree_DeleteNode(readerPtr->tree, node);
                }
            }
        }
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * DumpStringTable --
 *
 *      Frees the hash table used for tracking unique character strings.
 *      The Tcl_Obj pointer are decremented.  So that Tcl_Obj's no longer
 *      used by the tree are freed.
 *
 *---------------------------------------------------------------------------
 */
static void
DumpStringTable(Blt_HashTable *tablePtr)
{
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;

    for (hPtr = Blt_FirstHashEntry(tablePtr, &iter); hPtr != NULL;
         hPtr = Blt_NextHashEntry(&iter)) {
        Tcl_Obj *objPtr;

        objPtr = Blt_GetHashValue(hPtr);
        Tcl_DecrRefCount(objPtr);
    }
    Blt_DeleteHashTable(tablePtr);
}



static void
PrintPathStack(Blt_Chain pathStack)
{
    Blt_ChainLink link;

    for (link = Blt_Chain_FirstLink(pathStack); link != NULL; 
         link = Blt_Chain_NextLink(link)) {
        PathElement *elemPtr;

        elemPtr = Blt_Chain_GetValue(link);
        fprintf(stderr, "%s/", elemPtr->name);
    }
}

static void
PrintPattern(Blt_Chain patternLst)
{
    Blt_ChainLink link;

    for (link = Blt_Chain_FirstLink(patternLst); link != NULL; 
         link = Blt_Chain_NextLink(link)) {
        const char *pattern;

        pattern = Blt_Chain_GetValue(link);
        if ((pattern == NULL) || (pattern[0] == '\0')) {
            pattern = "empty";
        }
        fprintf(stderr, "%s/", pattern);
    }
}


/*
 *---------------------------------------------------------------------------
 *
 * MatchPattern --
 *
 *---------------------------------------------------------------------------
 */
static int
MatchPattern(Blt_Chain patternList, Blt_Chain pathStack)
{
    Blt_ChainLink patternLink;
    Blt_ChainLink pathLink;
    const char *pattern;
    int flag;

    patternLink = Blt_Chain_LastLink(patternList); 
    if (patternLink == NULL) {
        return FALSE;                   /* No pattern. */
    }
    if (0) {
    fprintf(stderr, "Try to match \"");
    PrintPathStack(pathStack);
    fprintf(stderr, "\" with pattern \""); 
    PrintPattern(patternList);
    fprintf(stderr, "\"\n");
    }
    pattern = Blt_Chain_GetValue(patternLink);
    flag = FALSE;
    if ((pattern == NULL) || (pattern[0] == '\0')) {
        /* Is separator. Set flag. */
        flag = KEEP_DESCENDANTS;
        patternLink = Blt_Chain_PrevLink(patternLink);
    }
    for (pathLink = Blt_Chain_LastLink(pathStack); patternLink != NULL; 
         patternLink = Blt_Chain_PrevLink(patternLink)) {
        PathElement *elemPtr;
        
        pattern = Blt_Chain_GetValue(patternLink);
        if ((pattern == NULL) || (pattern[0] == '\0')) {
            return (pathLink == NULL);
        }
        if (pathLink == NULL) {
            return FALSE;               /* Ran out of path. */
        }
        elemPtr = Blt_Chain_GetValue(pathLink);
        if (!Tcl_StringMatch(elemPtr->name, pattern)) {
            return FALSE;               /* Pattern does not match path
                                         * element. */
        }
        elemPtr->flags |= flag;
        /* Mark the path element to include all descendants.  */
        flag = 0;
        pathLink = Blt_Chain_PrevLink(pathLink);
    }
    return TRUE;
}

/*
 *---------------------------------------------------------------------------
 *
 * TestPatterns --
 *
 *---------------------------------------------------------------------------
 */
static int
TestPatterns(Blt_Chain patternsList, Blt_Chain pathStack)
{
    Blt_ChainLink link;

    for (link = Blt_Chain_FirstLink(patternsList); link != NULL; 
         link = Blt_Chain_NextLink(link)) {
        Blt_Chain pattern;

        pattern = Blt_Chain_GetValue(link);
        if (0) {
            fprintf(stderr, "MatchPattern:\n");
            PrintPattern(pattern);
            fprintf(stderr, " \"");
            PrintPathStack(pathStack);
            fprintf(stderr, "\"\n");
        }
        if (MatchPattern(pattern, pathStack)) {
            return TRUE;
        }
    }
    return FALSE;
}

/*
 *---------------------------------------------------------------------------
 *
 * AppendPathElement --
 *
 *      Creates and pushes the named path element onto the path stack.  
 *      A new node is created using the current node (readerPtr->parent)
 *      as its parent.
 *
 *      The new path element will inherit its parent's KEEP_DESCENDANTS
 *      flag. This means that is an include pattern such as a/ matches then
 *      all its descendants will also be included (a/b, a/b/c, a/b/c/d, a/e,
 *      etc.)
 *
 *      Tests if the current path matches any include or exclude patterns.
 *      If the failes to match an include pattern or matches an exclude
 *      pattern.  The KEEP flag will be removed.
 *
 *      If the path element has a KEEP flag, this means that that node
 *      and all its ancestors are to be retained.
 *
 *---------------------------------------------------------------------------
 */
static PathElement *
AppendPathElement(XmlReader *readerPtr, const char *element)
{
    PathElement *elemPtr;
    Blt_ChainLink lastLink, link;
    size_t numBytes;
    Blt_TreeNode node;

    lastLink = Blt_Chain_LastLink(readerPtr->pathStack);
    numBytes = strlen(element) + sizeof(PathElement);
    link = Blt_Chain_AllocLink(numBytes + 1);

    elemPtr = Blt_Chain_GetValue(link);
    elemPtr->flags = 0;
    elemPtr->node = NULL;
    strcpy(elemPtr->name , element);
    if (lastLink != NULL) {
        PathElement *lastElemPtr;
        
        lastElemPtr = Blt_Chain_GetValue(lastLink);
        elemPtr->flags = lastElemPtr->flags;
        if (lastElemPtr->flags & KEEP_DESCENDANTS) {
            /* Ignore the include patterns. This is descendant of a matching
             * include pattern with a trailing separator. */
            elemPtr->flags |= KEEP_DESCENDANTS;
        }
    }
    Blt_Chain_LinkAfter(readerPtr->pathStack, link, NULL);
    node = Blt_Tree_CreateNode(readerPtr->tree, readerPtr->parent, 
                               elemPtr->name, NULL);
    elemPtr->node = readerPtr->node = node;

    elemPtr->flags |= KEEP;
    if ((elemPtr->flags & KEEP_DESCENDANTS) == 0) {
        if ((readerPtr->includeList != NULL) && 
            (!TestPatterns(readerPtr->includeList, readerPtr->pathStack))) {
            elemPtr->flags &= ~KEEP;    /* No include patterns matched. */
        }
        if ((readerPtr->excludeList != NULL) && 
            (TestPatterns(readerPtr->excludeList, readerPtr->pathStack))) {
            elemPtr->flags &= ~KEEP;    /* At least one exclude pattern
                                         * matched. */
        }
    }
    if (elemPtr->flags & KEEP) {
        Blt_TreeNode parent;

        parent = readerPtr->node;
        while (link != NULL) {
            PathElement *lastPtr;

            lastPtr = Blt_Chain_GetValue(link);
            lastPtr->flags |= KEEP;
            link = Blt_Chain_PrevLink(link);
        }
    }
    return elemPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * StartElementProc --
 *
 *      Pushes the new path element onto the path element stack.  Creates a
 *      new node in the tree, using he current node pointer
 *      (readerPtr->parent) as its parent.  
 *
 *      Tests if the node does not match any include pattern, or matches an
 *      exclude pattern.  The KEEP flag is removed is then removed.  This
 *      lets us at least temporarily retain the element and its attributes,
 *      etc. until we know that the no further path elements will match.
 * 
 *---------------------------------------------------------------------------
 */
static void
StartElementProc(void *userData, const char *element, const char **attr) 
{
    XmlReader *readerPtr = userData;

    AppendPathElement(readerPtr, element);
    if (0) {
    fprintf(stderr, "StartElementProc  element=%s \n", 
            element);
    PrintPathStack(readerPtr->pathStack);
    fprintf(stderr, "\"\n");
    }
    assert(readerPtr->node != NULL);
    if (readerPtr->flags & IMPORT_ATTRIBUTES) {
        const char **p;

        for (p = attr; *p != NULL; p += 2) {
            Tcl_Obj *objPtr;
            
            objPtr = GetStringObj(readerPtr, *(p+1));
            Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, 
                readerPtr->node, *p, objPtr);
        }
    }
    if (readerPtr->flags & IMPORT_LOCATION) {
        SetLocation(readerPtr, readerPtr->node);
    }
    if (readerPtr->flags & IMPORT_BASEURI) {
        const char *oldBase, *newBase;

        newBase = XML_GetBase(readerPtr->parser);
        oldBase = GetBaseUri(readerPtr, readerPtr->parent);
        assert(oldBase != NULL);
        if (strcmp(oldBase, newBase) != 0) {
            Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, 
                              readerPtr->parent, SYM_BASEURI, 
                              Tcl_NewStringObj(newBase, -1));
        }
    }
    readerPtr->parent = readerPtr->node;
}

/*
 *---------------------------------------------------------------------------
 *
 * EndElementProc --
 *
 *      Pops the last element from the path element stack.  Sets the
 *      current node pointer (readerPtr->parent) to its parent.  If the
 *      KEEP flag is not set, this means that the current path did not
 *      match any include pattern, or matched an exclude pattern.  In this
 *      case we delete the the node from the tree.
 * 
 *---------------------------------------------------------------------------
 */
static void
EndElementProc(void *userData, const char *element) 
{
    XmlReader *readerPtr = userData;
    Blt_ChainLink link;
    PathElement *elemPtr;
    Blt_TreeNode parent;

    link = Blt_Chain_LastLink(readerPtr->pathStack);
    assert (link != NULL);
    elemPtr = Blt_Chain_GetValue(link);
    parent = Blt_Tree_ParentNode(readerPtr->parent);
    assert(readerPtr->parent != NULL);
    if ((elemPtr->flags & KEEP) == 0) {
        Blt_Tree_DeleteNode(readerPtr->tree, readerPtr->parent);
    }
    readerPtr->parent = parent;
    Blt_Chain_DeleteLink(readerPtr->pathStack, link);
    assert(readerPtr->parent != NULL);
}

/*
 *---------------------------------------------------------------------------
 *
 * GetDeclProc --
 *
 *      This routine is called from the expat parser when it encounters
 *      an XML declaration.  The version, encoding, standalone flags
 *      are recorded into tree.
 *
 *---------------------------------------------------------------------------
 */
static void
GetDeclProc(void *userData, const XML_Char  *version, const XML_Char  *encoding,
            int standalone)
{
    XmlReader *readerPtr = userData;

    if (version != NULL) {
        Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, readerPtr->node,
                 SYM_VERSION, Tcl_NewStringObj(version, -1));
    } 
    if (encoding != NULL) {
        Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, readerPtr->node,
                SYM_ENCODING, Tcl_NewStringObj(encoding,-1));
    }
    Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, readerPtr->node, 
        SYM_STANDALONE, Tcl_NewIntObj(standalone));
}

/*
 *---------------------------------------------------------------------------
 *
 * GetNotationProc --
 *
 *      This routine is called from the expat parser when it encounters
 *      XML notation.  The public ID, system ID, base URI, and notation
 *      name are recorded into tree.
 *
 *---------------------------------------------------------------------------
 */
static void
GetNotationProc(void *userData, const XML_Char *notationName,
                const XML_Char *base, const XML_Char *systemId,
                const XML_Char *publicId)
{
    XmlReader *readerPtr = userData;

    if (publicId != NULL) {
        Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, readerPtr->node,
                          SYM_PUBID, Tcl_NewStringObj(publicId, -1));
    }
    if (systemId != NULL) {
        Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, readerPtr->node,
                          SYM_SYSID, Tcl_NewStringObj(systemId, -1));
    } 
    if (base != NULL) {
        Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, readerPtr->node,
                          SYM_BASEURI, Tcl_NewStringObj(base, -1));
    }
    if (notationName != NULL) {
        Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, readerPtr->node,
                          SYM_NOTATION, Tcl_NewStringObj(notationName, -1));
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * GetCommentProc --
 *
 *      This routine is called from the expat parser when it encounters
 *      an XML comment.  The comment is recorded into tree.
 *
 *---------------------------------------------------------------------------
 */
static void
GetCommentProc(void *userData, const XML_Char *string) 
{
    XmlReader *readerPtr = userData;

    if ((readerPtr->flags & IMPORT_DTD) == 0) {
        Blt_Tree tree;
        Blt_TreeNode child;
        Tcl_Obj *objPtr;

        tree = readerPtr->tree;
        objPtr = GetStringObj(readerPtr, string);
        child = Blt_Tree_CreateNode(tree, readerPtr->node, SYM_COMMENT, NULL);
        Blt_Tree_SetValue(readerPtr->interp, tree, child, SYM_COMMENT, objPtr);
        if (readerPtr->flags & IMPORT_LOCATION) {
            SetLocation(readerPtr, child);
        }
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * GetProcessingInstructionProc --
 *
 *      This routine is called from the expat parser when it encounters
 *      an XML processing instruction.  The target is recorded into tree.
 *
 *---------------------------------------------------------------------------
 */
static void
GetProcessingInstructionProc(void *userData, const char *target,
                             const char *data)
{
    XmlReader *readerPtr = userData;

    if ((readerPtr->flags & IMPORT_DTD) == 0) {
        Tcl_Obj *objPtr;
        Blt_Tree tree;
        Blt_TreeNode child;

        tree = readerPtr->tree;
        objPtr = GetStringObj(readerPtr, data);
        child = Blt_Tree_CreateNode(tree, readerPtr->node, SYM_PI, NULL);
        Blt_Tree_SetValue(readerPtr->interp, tree, child, target, objPtr);
        if (readerPtr->flags & IMPORT_LOCATION) {
            SetLocation(readerPtr, child);
        }
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * GetCharacterDataProc --
 *
 *      This routine is called from the expat parser when it encounters XML
 *      character data.  The character data is recorded into the tree.
 *
 *      If the last node created was also a CDATA node append data to it.
 *
 *---------------------------------------------------------------------------
 */
static void
GetCharacterDataProc(void *userData, const XML_Char *string, int length) 
{
    XmlReader *readerPtr = userData;
    Blt_TreeNode child;
    Tcl_Obj *objPtr;

    child = Blt_Tree_LastChild(readerPtr->node);
    if ((child != NULL) && (strcmp(Blt_Tree_NodeLabel(child), SYM_CDATA)==0)) {

        /* Last child added was a CDATA node, append new data to it.  */
        
        if (Blt_Tree_GetValue(readerPtr->interp, readerPtr->tree, child, 
                        SYM_CDATA, &objPtr) == TCL_OK) {
            Tcl_AppendToObj(objPtr, string, length);
            return;
        }
    } 
    objPtr = Tcl_NewStringObj(string, length);
    child = Blt_Tree_CreateNode(readerPtr->tree, readerPtr->node, 
                SYM_CDATA, NULL);
    Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, child, SYM_CDATA, 
                objPtr);
    if (readerPtr->flags & IMPORT_LOCATION) {
        SetLocation(readerPtr, child);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * StartDocTypeProc --
 *
 *      This routine is called from the expat parser when it encounters XML
 *      character data.  The character data is recorded into the tree.
 *
 *      If the last node created was also a CDATA node append data to it.
 *
 *---------------------------------------------------------------------------
 */
static void
StartDocTypeProc(void *userData, const char *doctypeName, const char *systemId,
                 const char *publicId, int has_internal_subset)
{   
    XmlReader *readerPtr = userData;

    if (publicId != NULL) {
        Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, readerPtr->root, 
                SYM_PUBID, Tcl_NewStringObj(publicId, -1));
    }
    if (systemId != NULL) {
        Blt_Tree_SetValue(readerPtr->interp, readerPtr->tree, readerPtr->root, 
                SYM_SYSID, Tcl_NewStringObj(systemId, -1));
    } 
    readerPtr->flags |= IMPORT_DTD;
}

static void
EndDocTypeProc(void *userData)
{
    XmlReader *readerPtr = userData;

    readerPtr->flags &= ~IMPORT_DTD;
}

static int
ReadXmlFromFile(Tcl_Interp *interp, XML_Parser parser, const char *fileName)
{
    int closeChannel;
    int done;
    Tcl_Channel channel;

    closeChannel = TRUE;
    if ((fileName[0] == '@') && (fileName[1] != '\0')) {
        int mode;
        
        channel = Tcl_GetChannel(interp, fileName+1, &mode);
        if (channel == NULL) {
            return FALSE;
        }
        if ((mode & TCL_READABLE) == 0) {
            Tcl_AppendResult(interp, "channel \"", fileName, 
                "\" not opened for reading", (char *)NULL);
            return FALSE;
        }
        closeChannel = FALSE;
    } else {
        channel = Tcl_OpenFileChannel(interp, fileName, "r", 0);
        if (channel == NULL) {
            return FALSE;       /* Can't open dump file. */
        }
    }
    done = FALSE;
    while (!done) {
        int length;
#define BUFFSIZE        8191
        char buffer[BUFFSIZE+1];
        
        length = Tcl_Read(channel, buffer, sizeof(char) * BUFFSIZE);
        if (length < 0) {
            Tcl_AppendResult(interp, "\nread error: ", Tcl_PosixError(interp),
                             (char *)NULL);
            if (closeChannel) {
                Tcl_Close(interp, channel);
            }
            return FALSE;
        }
        done = Tcl_Eof(channel);
        if (!XML_Parse(parser, buffer, length, done)) {
            Tcl_AppendResult(interp, "\n", fileName, ":",
                        Blt_Itoa(XML_GetCurrentLineNumber(parser)), ": ",
                        "error: ", 
                        XML_ErrorString(XML_GetErrorCode(parser)), 
                        (char *)NULL);
            Tcl_AppendResult(interp, "\n", fileName, ":",
                        Blt_Itoa(XML_GetCurrentByteIndex(parser)), ": ",
                        "error: ", 
                        XML_ErrorString(XML_GetErrorCode(parser)), 
                        (char *)NULL);
            if (closeChannel) {
                Tcl_Close(interp, channel);
            }
            return FALSE;
        }
    }
    if (closeChannel) {
        Tcl_Close(interp, channel);
    }
    return TRUE;
}


static int
GetExternalEntityRefProc(XML_Parser parser, const XML_Char *context,
                         const XML_Char *base, const XML_Char *systemId,
                         const XML_Char *publicId)
{
    XmlReader *readerPtr;
    Tcl_DString ds;
    Tcl_Interp *interp;
    XML_Parser newParser, oldParser;
    int result;

    readerPtr = XML_GetUserData(parser);
    assert(readerPtr != NULL);
    interp = readerPtr->interp;
    if (strncmp(systemId, "http:", 5) == 0) {
        Tcl_AppendResult(interp, "can't handle external entity reference \"", 
                         systemId, "\"", (char *)NULL);
        return FALSE;
    }
    Tcl_DStringInit(&ds);
    if ((base != NULL) && (Tcl_GetPathType(systemId) == TCL_PATH_RELATIVE)) {
        const char **argv;
        const char **baseNames, **sysIdNames;
        int argc;
        int i, j;
        int numBase, numSysId;

        Tcl_SplitPath(base, &numBase, &baseNames);
        Tcl_SplitPath(systemId, &numSysId, &sysIdNames);
        argc = numBase + numSysId;
        argv = Blt_Malloc(sizeof(char *) * (argc + 1));
        if (argv == NULL) {
            return FALSE;
        }
        for (i = 0; i < numBase; i++) {
            argv[i] = baseNames[i];
        }
        for (j = 0; j < numSysId; j++, i++) {
            argv[i] = sysIdNames[j];
        }
        argv[i] = NULL;
        Tcl_JoinPath(argc, argv, &ds);
        Tcl_Free((char *)baseNames);
        Tcl_Free((char *)sysIdNames);
        Blt_Free(argv);
    } else {
        Tcl_DStringAppend(&ds, systemId, -1);
    }
    newParser = XML_ExternalEntityParserCreate(parser, context, NULL);
    if (newParser == NULL) {
        Tcl_AppendResult(interp, "can't create external entity ref parser", 
                         (char *)NULL);
        return FALSE;
    }
#ifdef notdef
    XML_SetParamEntityParsing(newParser, XML_PARAM_ENTITY_PARSING_ALWAYS);
#endif
    oldParser = readerPtr->parser;
    readerPtr->parser = newParser;
    result = ReadXmlFromFile(interp, newParser, Tcl_DStringValue(&ds));
    readerPtr->parser = oldParser;
    Tcl_DStringFree(&ds);
    XML_ParserFree(newParser);
    return result;
}

static int
ImportXmlFile(Tcl_Interp *interp, const char *fileName, XmlReader *readerPtr)
{
    XML_Parser parser;
    int result;

    if (readerPtr->flags & IMPORT_NS) {
        parser = XML_ParserCreateNS(NULL, ':');
    } else {
        parser = XML_ParserCreate(NULL);
    }
    if (parser == NULL) {
        Tcl_AppendResult(interp, "can't create XML parser", (char *)NULL);
        return TCL_ERROR;
    }
    Blt_InitHashTable(&readerPtr->stringTable, BLT_STRING_KEYS);
    XML_SetUserData(parser, readerPtr);
    /* Set baseURI */
    {
        Tcl_DString ds;
        int argc;
        const char **argv;

        Tcl_DStringInit(&ds);
        Tcl_SplitPath(fileName, &argc, &argv);
        Tcl_JoinPath(argc - 1, argv, &ds);
        XML_SetBase(parser, Tcl_DStringValue(&ds));
        if (readerPtr->flags & IMPORT_BASEURI) {
            Blt_Tree_SetValue(interp, readerPtr->tree, readerPtr->root, 
               SYM_BASEURI, 
               Tcl_NewStringObj(Tcl_DStringValue(&ds), Tcl_DStringLength(&ds)));
        }
        Tcl_Free((char *)argv);
        Tcl_DStringFree(&ds);
    }
    if (readerPtr->flags & IMPORT_EXTREF) {
        XML_SetExternalEntityRefHandler(parser, GetExternalEntityRefProc);
        XML_SetParamEntityParsing(parser, 
                XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE);
    }

    XML_SetElementHandler(parser, StartElementProc, EndElementProc);
    if (readerPtr->flags & IMPORT_CDATA) {
        XML_SetCharacterDataHandler(parser, GetCharacterDataProc);
    }
    if (readerPtr->flags & IMPORT_BASEURI) {
        XML_SetNotationDeclHandler(parser, GetNotationProc);
        XML_SetDoctypeDeclHandler(parser, StartDocTypeProc, EndDocTypeProc);
    }
    if (readerPtr->flags & IMPORT_DECL) {
        XML_SetXmlDeclHandler(parser, GetDeclProc);
    }
    if (readerPtr->flags & IMPORT_PI) {
        XML_SetProcessingInstructionHandler(parser, 
                GetProcessingInstructionProc);
    }
    if (readerPtr->flags & IMPORT_COMMENTS) {
        XML_SetCommentHandler(parser, GetCommentProc);
    }
    result = ReadXmlFromFile(interp, parser, fileName);
    XML_ParserFree(parser);
    if (readerPtr->flags & IMPORT_TRIMCDATA) {
        TrimWhitespace(readerPtr);
    }
    if (readerPtr->flags & IMPORT_SINGLECDATA) {
        ConvertSingleCDATA(readerPtr);
    }
    DumpStringTable(&readerPtr->stringTable);
    return (result) ? TCL_OK : TCL_ERROR;
} 


static int
ImportXmlData(Tcl_Interp *interp, Tcl_Obj *dataObjPtr, XmlReader *readerPtr)
{
    XML_Parser parser;
    char *string;
    int length;
    int result;

    if (readerPtr->flags & IMPORT_NS) {
        parser = XML_ParserCreateNS(NULL, ':');
    } else {
        parser = XML_ParserCreate(NULL);
    }
    if (parser == NULL) {
        Tcl_AppendResult(interp, "can't create parser", (char *)NULL);
        return TCL_ERROR;
    }
    Blt_InitHashTable(&readerPtr->stringTable, BLT_STRING_KEYS);
    XML_SetBase(parser, ".");
    XML_SetUserData(parser, readerPtr);
    if (readerPtr->flags & IMPORT_EXTREF) {
        XML_SetExternalEntityRefHandler(parser, GetExternalEntityRefProc);
        XML_SetParamEntityParsing(parser, 
                XML_PARAM_ENTITY_PARSING_UNLESS_STANDALONE);
    }
    XML_SetElementHandler(parser, StartElementProc, EndElementProc);
    if (readerPtr->flags & IMPORT_DECL) {
        XML_SetXmlDeclHandler(parser, GetDeclProc);
    }
    if (readerPtr->flags & IMPORT_CDATA) {
        XML_SetCharacterDataHandler(parser, GetCharacterDataProc);
    }
    if (readerPtr->flags & IMPORT_BASEURI) {
        XML_SetNotationDeclHandler(parser, GetNotationProc);
        XML_SetDoctypeDeclHandler(parser, StartDocTypeProc, EndDocTypeProc);
    }
    if (readerPtr->flags & IMPORT_PI) {
        XML_SetProcessingInstructionHandler(parser, 
                GetProcessingInstructionProc);
    }
    if (readerPtr->flags & IMPORT_COMMENTS) {
        XML_SetCommentHandler(parser, GetCommentProc);
    }
    string = Tcl_GetStringFromObj(dataObjPtr, &length);
    result = XML_Parse(parser, string, length, 1);
    if (!result) {
        Tcl_AppendResult(interp, "\nparse error at line ",
                Blt_Itoa(XML_GetCurrentLineNumber(parser)), ":  ",
                XML_ErrorString(XML_GetErrorCode(parser)),
                (char *)NULL);
    }
    if (readerPtr->flags & IMPORT_TRIMCDATA) {
        TrimWhitespace(readerPtr);
    }
    XML_ParserFree(parser);
    DumpStringTable(&readerPtr->stringTable);
    return (result) ? TCL_OK : TCL_ERROR;
} 

static int
ImportXmlProc(Tcl_Interp *interp, Blt_Tree tree, int objc, Tcl_Obj *const *objv)
{
    int result;
    XmlReader reader;

    memset(&reader, 0, sizeof(reader));
    nodeSwitch.clientData = tree;
    reader.parent = reader.root = Blt_Tree_RootNode(tree);
    reader.tree = tree;
    reader.flags = IMPORT_ATTRIBUTES | IMPORT_CDATA;
    reader.pathStack = Blt_Chain_Create();
    reader.separator = "/";
    if (Blt_ParseSwitches(interp, importSwitches, objc - 3, objv + 3, 
        &reader, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    reader.parent = reader.root;
    result = TCL_ERROR;
    if ((reader.dataObjPtr != NULL) && (reader.fileObjPtr != NULL)) {
        Tcl_AppendResult(interp, "can't set both -file and -data switches",
                         (char *)NULL);
        goto error;
    }
    if (reader.fileObjPtr != NULL) {
        result = ImportXmlFile(interp, Tcl_GetString(reader.fileObjPtr), 
                               &reader);
    } else if (reader.dataObjPtr != NULL) {
        result = ImportXmlData(interp, reader.dataObjPtr, &reader);
    } else {
        Tcl_AppendResult(interp, "must specify either -file or -data switch",
                (char *)NULL);
        goto error;
    }
 error:
    if (reader.pathStack != NULL) {
        Blt_Chain_Destroy(reader.pathStack);
    }
    Blt_FreeSwitches(importSwitches, (char *)&reader, 0);
    return result;
}

#endif /* HAVE_LIBEXPAT */

/*
 *---------------------------------------------------------------------------
 *
 * XmlExportData --
 *
 *      Appends a string into the buffer storing XML output.
 *
 *---------------------------------------------------------------------------
 */
static INLINE void
XmlExportData(XmlWriter *writerPtr, const char *string, size_t numBytes)
{
    Blt_DBuffer_AppendString(writerPtr->dbuffer, string, numBytes);
}

/*
 *---------------------------------------------------------------------------
 *
 * XmlIndentLine --
 *
 *      Adds a newline and indents to line to the proper indent level based
 *      on the depth of the given node and the indentation increment.  If
 *      we are not exporting the root node 1) don't add an extra newline.
 *      and 2) adjust the depth,
 *
 *---------------------------------------------------------------------------
 */
static void
XmlIndentLine(XmlWriter *writerPtr, Blt_TreeNode node)
{
    long d;
    
    if ((writerPtr->flags & EXPORT_ROOT) || (writerPtr->root != node)) {
        XmlExportData(writerPtr, "\n", 1);
    }
    d = Blt_Tree_NodeDepth(node);
    if ((writerPtr->flags & EXPORT_ROOT) == 0) {
        d--;
    }
    Blt_DBuffer_Format(writerPtr->dbuffer, "%*.s", writerPtr->indent * d, "");
}

/*
 *---------------------------------------------------------------------------
 *
 * XmlFlush --
 *
 *      Writes any data stored in the buffer to the file channel.  If we're
 *      writing XML output to a file, we only store pieces of the XML in the
 *      buffer and write the buffer the file when we encounter an start of
 *      end tag.  The buffer is also reset.
 *
 *---------------------------------------------------------------------------
 */
static int
XmlFlush(XmlWriter *writerPtr) 
{
    size_t length;

    length = Blt_DBuffer_Length(writerPtr->dbuffer);
    if (length > 0) {
        ssize_t numWritten;
        const char *line;

        line = Blt_DBuffer_String(writerPtr->dbuffer);
        numWritten = Tcl_Write(writerPtr->channel, line, length);
        if (numWritten != length) {
            Tcl_AppendResult(writerPtr->interp, "can't write xml element: ",
                Tcl_PosixError(writerPtr->interp), (char *)NULL);
            return TCL_ERROR;
        }
    }
    Blt_DBuffer_SetLength(writerPtr->dbuffer, 0);
    return TCL_OK;
}

static void
XmlPutEscapeString(const char *from, size_t length, XmlWriter *writerPtr)
{
    const char *p, *pend;

    for (p = from, pend = from + length; p < pend; /*empty*/) {
        switch (*p) {
        case '\'': 
            if (p > from) {
                XmlExportData(writerPtr, from, p - from);
            }
            from = ++p;
            XmlExportData(writerPtr, "&apos;", 6);
            break;
        case '&':  
            if (p > from) {
                XmlExportData(writerPtr, from, p - from);
            }
            from = ++p;
            XmlExportData(writerPtr, "&amp;", 5);
            break;
        case '>':  
            if (p > from) {
                XmlExportData(writerPtr, from, p - from);
            }
            from = ++p;
            XmlExportData(writerPtr, "&gt;", 4);
            break; 
        case '<':  
            if (p > from) {
                XmlExportData(writerPtr, from, p - from);
            }
            from = ++p;
            XmlExportData(writerPtr, "&lt;", 4);
            break; 
        case '"':  
            if (p > from) {
                XmlExportData(writerPtr, from, p - from);
            }
            from = ++p;
            XmlExportData(writerPtr, "&quot;", 6);
            break;
        default:  
            p++;
            break;
        }
    }   
    if (p > from) {
        XmlExportData(writerPtr, from, p - from);
    }
}

static void
XmlOpenStartElement(XmlWriter *writerPtr, Blt_TreeNode node)
{
    const char *label;
    
    if (writerPtr->channel != NULL) {
        XmlFlush(writerPtr);
    }
    writerPtr->flags &= ~LAST_END_TAG;
    /* Always indent starting element tags */
    XmlIndentLine(writerPtr, node);
    label = Blt_Tree_NodeLabel(node);
    if (writerPtr->root == node) {
        if (writerPtr->flags & EXPORT_ROOT) {
            if (label[0] == '\0') {
                XmlExportData(writerPtr, "<root", 5);
            } else {
                Blt_DBuffer_Format(writerPtr->dbuffer, "<%s", label);
            }                
        }
    } else {
        Blt_DBuffer_Format(writerPtr->dbuffer, "<%s", label);
    }
}

static int
XmlCloseStartElement(XmlWriter *writerPtr, Blt_TreeNode node)
{
    if ((writerPtr->root != node) || (writerPtr->flags & EXPORT_ROOT)) {
        XmlExportData(writerPtr, ">", 1);
    }
    if (writerPtr->channel != NULL) {
        return XmlFlush(writerPtr);
    }
    return TCL_OK;
}

static int
XmlEndElement(XmlWriter *writerPtr, Blt_TreeNode node)
{
    const char *label;
    
    if (writerPtr->flags & LAST_END_TAG) {
        XmlIndentLine(writerPtr, node);
    }
    writerPtr->flags |= LAST_END_TAG;
    label = Blt_Tree_NodeLabel(node);
    if (writerPtr->root == node) {
        if (writerPtr->flags & EXPORT_ROOT) {
            if (label[0] == '\0') {
                XmlExportData(writerPtr, "</root>\n", 8);
            } else {
                Blt_DBuffer_Format(writerPtr->dbuffer, "</%s>\n", label);
            }
        } else {
            XmlExportData(writerPtr, "\n", 1);
        }
    } else {
        Blt_DBuffer_Format(writerPtr->dbuffer, "</%s>", label);
    }
    if (writerPtr->channel != NULL) {
        return XmlFlush(writerPtr);
    }
    return TCL_OK;
}

static void
XmlAppendAttribute(XmlWriter *writerPtr, const char *attrName,
                   const char *value, int length)
{
    size_t valueLen;

    if (length < 0) {
        valueLen = strlen(value);
    } else {
        valueLen = (size_t)length;
    }
    Blt_DBuffer_Format(writerPtr->dbuffer, " %s=\"", attrName);
    XmlPutEscapeString(value, valueLen, writerPtr);
    XmlExportData(writerPtr, "\"", 1);
}

static void
XmlAppendCharacterData(XmlWriter *writerPtr, const char *string, int length)
{
    if (length < 0) {
        length = strlen(string);
    } 
    XmlPutEscapeString(string, length, writerPtr);
}

static int
XmlExportElement(Blt_Tree tree, Blt_TreeNode parent, XmlWriter *writerPtr)
{
    Blt_TreeUid key;
    Blt_TreeValueIterator iter;
    Blt_TreeNode child;

    if (strcmp(Blt_Tree_NodeLabel(parent), SYM_CDATA) == 0) {
        Tcl_Obj *valueObjPtr;
        const char *value;
        int length;

        /* Just output the CDATA field. */
        if (Blt_Tree_GetValue(writerPtr->interp, tree, parent, SYM_CDATA, 
                &valueObjPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        value = Tcl_GetStringFromObj(valueObjPtr, &length);
        XmlAppendCharacterData(writerPtr, value, length);
        return TCL_OK;
    } 
    XmlOpenStartElement(writerPtr, parent);
    for (key = Blt_Tree_FirstValue(tree, parent, &iter); key != NULL; 
         key = Blt_Tree_NextValue(tree, &iter)) {
        Tcl_Obj *valueObjPtr;
        const char *value;
        int numBytes;

        if (Blt_Tree_GetScalarValueByUid(writerPtr->interp, tree, parent, key,
                &valueObjPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        value = Tcl_GetStringFromObj(valueObjPtr, &numBytes);
        XmlAppendAttribute(writerPtr, key, value, numBytes);
    }
    XmlCloseStartElement(writerPtr, parent);
    for (child = Blt_Tree_FirstChild(parent); child != NULL; 
         child = Blt_Tree_NextSibling(child)) {
        if (XmlExportElement(tree, child, writerPtr) != TCL_OK) {
            return TCL_ERROR;
        }
    }
    XmlEndElement(writerPtr, parent);
    return TCL_OK;
}

static int
XmlExport(Blt_Tree tree, XmlWriter *writerPtr)
{
    if (writerPtr->flags & EXPORT_DECLARATION) {
        XmlExportData(writerPtr, "<?xml version='1.0' encoding='utf-8'?>", 38);
    }
    if (XmlExportElement(tree, writerPtr->root, writerPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (writerPtr->channel != NULL) {
        return XmlFlush(writerPtr);
    }
    return TCL_OK;
}

static int
ExportXmlProc(Tcl_Interp *interp, Blt_Tree tree, int objc, Tcl_Obj *const *objv)
{
    XmlWriter writer;
    Tcl_Channel channel;
    int closeChannel;
    int result;

    closeChannel = FALSE;
    channel = NULL;

    memset(&writer, 0, sizeof(writer));
    nodeSwitch.clientData = tree;
    writer.root = Blt_Tree_RootNode(tree);
    writer.indent = 1;
    writer.flags = IMPORT_ATTRIBUTES | IMPORT_CDATA;
    if (Blt_ParseSwitches(interp, exportSwitches, objc - 3 , objv + 3, 
        &writer, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    result = TCL_ERROR;
    if (writer.fileObjPtr != NULL) {
        char *fileName;

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
    writer.interp = interp;
    writer.dbuffer = Blt_DBuffer_Create();
    writer.channel = channel;
    result = XmlExport(tree, &writer); 
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
    if (closeChannel) {
        Tcl_Close(interp, channel);
    }
    Blt_FreeSwitches(exportSwitches, (char *)&writer, 0);
    return result;
}

int 
Blt_TreeXmlInit(Tcl_Interp *interp)
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
    if (Tcl_PkgProvide(interp, "blt_tree_xml", BLT_VERSION) != TCL_OK) { 
        return TCL_ERROR;
    }
    return Blt_Tree_RegisterFormat(interp,
        "xml",                  /* Name of format. */
#ifdef HAVE_LIBEXPAT
        ImportXmlProc,          /* Import procedure. */
#else
        NULL,                   /* Import procedure. */
#endif  /* HAVE_LIBEXPAT */
        ExportXmlProc);         /* Export procedure. */

}

int 
Blt_TreeXmlSafeInit(Tcl_Interp *interp)
{
    return Blt_TreeXmlInit(interp);
}
