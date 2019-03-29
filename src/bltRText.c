/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * bltRText.c --
 *
 *	This file implements rich text objects for BLT. It allows BLT
 *	widgets to use Tk text widget-like rich text without replicating
 *	operations in each widget to handle rich text.
 *
 * Copyright 2019 George A. Howlett. All rights reserved.  
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
blt::rtext create ?textName? ?option value ...?
blt::rtext destroy ?textName ...?
blt::rtext exists textName
blt::rtext names ?pattern ...?

textName tag create tagName \
    -size \
    -font \  
    -foreground \
    -justify	\
    -overstrike \
    -offset \
#    -spacing1 
#    -spacing2 \
#    -spacing3 \
#    -tabs \
#    -tabstyle \
    -underline \
    -wrap \
textName tag delete ?tagName ...?
textName tag names ?pattern ...?
textName tag exists tagName
textName tag configure tagName ?option value ...?
	 -font
	 -text
	 -foreground
	 -layout ellipsis|title|normal
	 -wrap none|char|word
	 -wraplength 
textName tag cget tagName option

textName tag add tagName index1 ?index2 index1 index2 ...?
textName tag ranges tagName option seq script
textname tag remove tagName option seq script

textName bbox
textName cget option
textName configure ?option value ...? \
	-background \
	-font \
	-foreground \
	-justify \
	-wrap \
	-wraplength \

textName delete firstIndex lastIndex
textName get firstIndex lastIndex
textName insert index text ?tagName? ?text tagName?

textName image create index -image imageName
textName image configure index -image imageName
textName image cget index -image imageName

textName texinsert text
\frac{a}{b}
\sub{a}
\sup{a}
\fontcolor{blue}
\fontsize{+2}
\font{arial 10}
\theta
\image{imageName}
#\int{a}{b}
{a^b} {a_b}

Newline is a character kept in a single text item.

*/

/*
  textPtr = Blt_CreateRTextObj(string);
  RText_SetText(textPtr, string, length);
  RText_SetForeground(textPtr, color);
  RText_Draw(tkwin, textPtr, x, y, anchor, maxWidth)
 
  Put list of text items into an array for binary searching.
  How does wrap and wraplength work?  How should it work?
  How do tabs and tabstops work?  \t1i 2i 3i  Process tabs.
*/

/* Special characters in input string.  They are not displayed and 
 * indicate that special processing must be performed. */
#define TAB   '\t'
#define NL    '\n'
#define IMAGE '\1'

/*
 * TextLayoutItem --
 *
 *      Forms a list of processed text items for layout. TextItems are
 *      broken into layout when ranges of text using the same font, angle,
 *      color, and attribute.  Dimensions of the text item are computed
 *      and saved.
 */
typedef struct _TextLayoutItem {
    unsigned int flags;			/* UNDERLINE, OVERSTRIKE */
    int numBytes;                       /* Length of text chunk. */
    const char *text;                   /* Pointer to text. */
    short int x, y;                     /* Location of the text item. */
    short int width, height;            /* Dimensions of the item. */
    GC gc;                              /* GC for this chunk. This is reset
                                         * whenever font, foreground color,
                                         * or size change. */
    struct _TextLayoutItem *nextPtr;    /* If non-NULL, points to next text
                                         * layout item. */
} TextLayoutItem;

/*
 * Tag --
 */
typedef struct _Tag {
    unsigned int flags;			/* UNDERLINE, OVERSTRIKE, WRAP */
    /* Text Attributes */
    XColor *fgColor;			/* Color to draw the text. */
    Blt_Bg bg;				/* If non-NULL, background color of
					 * text. */
    Blt_Font font;                      /* If non-NULL, font to use to draw
					 * text. Otherwise use global font. */
    int fontSize;			/* Font size delta. */
    int offset;
    int wrapLength;
    Blt_Pad padX, padY;                 /* # pixels padding of around text
                                         * region. */
    Blt_HashEntry *hashPtr;             /* Pointer to this entry in the
                                         * tag table. */
} Tag;

/*
 * TagRange --
 *
 *      Specifies a range of byte offsets within the input string where a
 *      tag applies.  The same tag can be used in more than one range and
 *      ranges overlap.
 */
typedef struct _TagRange {
    int firstPos, lastPos;              /* Byte offsets of the tag. */
    Tag *tagPtr;
    struct _TagRange *nextPtr;          /* Points to next range. */
} TagRange;


/* TAB, NEWLINE, SPECIAL(s), TEXT, IMAGE */
typedef struct _RText RText;
typedef struct _Item Item;

typedef void (ItemFreeProc)(RText *textPtr, Item *itemPtr);
typedef int (ItemGeometryProc)(RText *textPtr, Item *itemPtr);

typedef struct _ItemClass {
    int type;
    const char *name;
    ItemFreeProc *freeProc;
    ItemGeometryProc *geomProc;
} ItemClass;

struct _Item {
    Item *nextPtr, *prevPtr;
    ItemClass *classPtr;
    unsigned short int x, y;		/* Offset of image or item from
					 * anchor position of parent text
					 * object.  */
    unsigned short int sx, sy;		/* Starting offset of image or text
                                         * using rotated font. */
    unsigned short int width, height;   /* Size of item or image in pixels. */
    unsigned short int lineNum;		/* Line number of start of item. */
    unsigned short int charNum;	        /* Character index of start of
					 * item. */
};

/*
 * TextItem --
 */
typedef struct {
    Item *nextPtr, *prevPtr;
    ItemClass *classPtr;
    unsigned short int x, y;		/* Offset of image or item from
					 * anchor position of parent text
					 * object.  */
    unsigned short int sx, sy;		/* Starting offset of image or text
                                         * using rotated font. */
    unsigned short int width, height;   /* Size of item or image in pixels. */
    unsigned short int lineNum;		/* Line number of start of item. */
    unsigned short int charNum;	        /* Character index of start of
					 * item. */
    /* TextItem specific fields. */
    Tag *tagPtr;			/* If non-NULL, points to
					 * attributes (color, font, etc.)
					 * to use when drawing this text
					 * item. If NULL, we'll use the
					 * global text attributes. */
    const char *text;                   /* Text string to be displayed */
    int numBytes;                       /* # of bytes in text. The actual
                                         * character count may differ
                                         * because of multi-byte UTF
                                         * encodings. */
} TextItem;
    
/*
 * ImageItem --
 */
typedef struct {
    Item *nextPtr, *prevPtr;
    ItemClass *classPtr;
    unsigned short int x, y;		/* Offset of image or item from
					 * anchor position of parent text
					 * object.  */
    unsigned short int sx, sy;		/* Starting offset of image or text
                                         * using rotated font. */
    unsigned short int width, height;   /* Size of item or image in pixels. */
    unsigned short int lineNum;		/* Line number of start of item. */
    unsigned short int charNum;	        /* Character index of start of
					 * item. */
    /* ImageItem specific fields. */
    Tk_Image tkImage;                   /* The Tk image being cached. */
    Blt_HashEntry *hashPtr;             /* Pointer to this entry in the
                                         * image hash table. */
} ImageItem;

/*
 * Blt_RText --
 */
struct _RText {
    unsigned int flags;

    /* Text Attributes */
    unsigned int state;                 /* If non-zero, indicates to draw
                                         * text in the active color */
    XColor *color;                      /* Color to draw the text. */
    Blt_Font font;                      /* Font to use to draw text */
    Blt_Bg bg;                          /* Background color of text.  This
                                         * is also used for drawing
                                         * disabled text. */
    float angle;                        /* Rotation of text in degrees. */
    Tk_Justify justify;                 /* Justification of the text
                                         * string. This only matters if the
                                         * text is composed of multiple
                                         * lines. */
    Tk_Anchor anchor;                   /* Indicates how the text box is
                                         * anchored around its x,y
                                         * coordinates. */
    Blt_Pad padX, padY;                 /* # pixels padding of around text
                                         * region. */
    unsigned short int leader;          /* # pixels spacing between lines
                                         * of text. */
    short int underline;                /* Index of character to be underlined,
                                         * -1 if no underline. */
    int maxLength;                      /* Maximum length in pixels of
                                         * text */
    /* Private fields. */
    unsigned short flags;
    GC gc;                              /* GC used to draw the text */
    TkRegion rgn;

    int width, height;                  /* Dimensions of text bounding
                                         * box */
    int numItems;			/* # of items. */
    Item *firstPtr, *lastPtr;		/* Linked list of items. */
    Blt_HashTable tagTable;
    Blt_HashTable imageTable;
    TagRange *firstRangePtr;            /* List of tag character ranges. */
    TextImage *firstImgPtr;
};

static ItemClass textItemClass = {
    TEXT_ITEM,
    "text",
    FreeTextItem,
    GetTextGeometry,
};

static ItemClass imageItemClass = {
    IMAGE_ITEM,
    "image",
    FreeImageItem,
    GetImageGeometry,
};

static Blt_ConfigSpec textSpecs[] = {
    {BLT_CONFIG_ANCHOR, "-anchor", "anchor", "Anchor", DEF_ANCHOR,
        Blt_Offset(RText, anchor), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_END}
};

static Blt_ConfigSpec tagSpecs[] = {
    {BLT_CONFIG_BACKGROUND, "-background", "background", "Background", 
	DEF_BACKGROUND, Blt_Offset(Tag, bg), 0},
    {BLT_CONFIG_FONT, "-font", "font", "Font", DEF_FONT, 
        Blt_Offset(Tag, font), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_COLOR, "-foreground", "foreground", "Foreground", 
        DEF_FOREGROUND, Blt_Offset(Tag, fgColor), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_SYNONYM, "-bg", "background"},
    {BLT_CONFIG_SYNONYM, "-fg", "foreground"},
    {BLT_CONFIG_JUSTIFY, "-justify", "justify", "Justify", DEF_JUSTIFY, 
	Blt_Offset(Tag, justify), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_BITMASK, "-overstrike", "overstrike", "Overstrike", 
        DEF_OVERSTRIKE, Blt_Offset(Tag, flags), 
        BLT_CONFIG_DONT_SET_DEFAULT, (Blt_CustomOption *)OVERSTRIKE},
    {BLT_CONFIG_BITMASK, "-underline", "underline", "Underline", 
        DEF_UNDERLINE, Blt_Offset(Tag, flags), 
        BLT_CONFIG_DONT_SET_DEFAULT, (Blt_CustomOption *)UNDERLINE},
    {BLT_CONFIG_END}
};

static Blt_ConfigSpec imageSpecs[] = {
    {BLT_CONFIG_CUSTOM, "-align", "align", "Align", DEF_ALIGN, 
        Blt_Offset(Image, align), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_CUSTOM, "-image", "image", "Image", DEF_IMAGE, 
        Blt_Offset(Image, tkImage), 
        BLT_CONFIG_NULL_OK|BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_OBJ, "-name", "name", "Name", DEF_NAME, 
        Blt_Offset(Image, nameObjPtr), 
        BLT_CONFIG_NULL_OK|BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_PADDING, "-padx", "padX", "PadX", 
        DEF_PADX, Blt_Offset(Image, padX), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_PADDING, "-pady", "padY", "PadY", 
        DEF_PADX, Blt_Offset(Image, padY), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_END}
};


static Item *
NewTextItem(RText *textPtr, const char *string, int numBytes)
{
    TextItem *itemPtr;

    itemPtr = Blt_AssertCalloc(1, sizeof(TextItem));
    itemPtr->classPtr = &textItemClass;
    itemPtr->text = text;
    itemPtr->numBytes = numBytes;
    return (Item *)itemPtr;
}

static Item *
NewImageItem(RText *textPtr, Tk_Image tkImage)
{
    ImageItem *itemPtr;

    itemPtr = Blt_AssertCalloc(1, sizeof(ImageItem));
    itemPtr->classPtr = &imageItemClass;
    itemPtr->tkImage = tkImage;
    return (Item *)itemPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * SplitTextItem --
 *
 *      Splits an item at the specified character index relative to the start
 *	of the text in the item.  The current item is modified to contain
 *	the first part of the split (everything before the index). A new
 *	item is inserted after the current item.  It will have the same
 *	tag as the current item.
 *
 *		["abcdef"][tag1] => ["abc"][tag1] -> ["def"][tag1]
 *
 *---------------------------------------------------------------------------
 */
static int 
SplitTextItem(RText *textPtr, TextItem *itemPtr, int charIndex)
{
}

/*
 *---------------------------------------------------------------------------
 *
 * JoinTextItems --
 *
 *      Joins two items together if they both are using the same tag.  The
 *	second item it the next item is removed and the first is marked as
 *	dirty as the geometry needs to be computed .
 *
 *	["abc"][tag1] -> ["def"][tag1] => ["abcdef"][tag1] 
 *
 *	Neither item can be an image or newline.
 *
 *---------------------------------------------------------------------------
 */
static int 
JoinTextItems(RText *textPtr, TextItem *itemPtr)
{
}

/*
 *---------------------------------------------------------------------------
 *
 * InsertImageItem --
 *
 *---------------------------------------------------------------------------
 */
static int 
InsertImageItem(RText *textPtr, Item *beforePtr, Tk_Image *tkImage)
{
    itemPtr = NewImageItem(textPtr, tkImage);
    itemPtr->nextPtr = beforePtr->nextPtr;
    beforePtr->nextPtr = itemPtr;
    textPtr->numItems++;
}

/*
 *---------------------------------------------------------------------------
 *
 * InsertImageText --
 *
 *      Inserts characters at the given byte offset.  It is assumed that
 *      the substring is a complete UTF sequence and that the insert position
 *      represents a valid location between UTF characters.  
 *
 *---------------------------------------------------------------------------
 */
static int 
InsertImageItem(RText *textPtr, Item *beforePtr, const char *text, int length)
{
    itemPtr = NewTextItem(textPtr, text, length);
    itemPtr->nextPtr = beforePtr->nextPtr;
    beforePtr->nextPtr = itemPtr;
    textPtr->numItems++;
}

/*
 *---------------------------------------------------------------------------
 *
 * AppendImageItem --
 *
 *---------------------------------------------------------------------------
 */
static int 
AppendImageItem(RText *textPtr, Tk_Image *tkImage)
{
    Item *itemPtr;

    itemPtr = NewImageItem(textPtr, tkImage);
    if (textPtr->firstPtr == NULL) {
	textPtr->firstPtr = textPtr->lastPtr = itemPtr;
    } else {
	itemPtr->prevPtr = textPtr->lastPtr; 
	textPtr->lastPtr->nextPtr = itemPtr;
    }
    textPtr->numItems++;
}

/*
 *---------------------------------------------------------------------------
 *
 * AppendTextItem --
 *
 *---------------------------------------------------------------------------
 */
static int 
AppendTextItem(RText *textPtr, const char *text, int length)
{
    itemPtr = NewTextItem(textPtr, text, length);
    if (textPtr->firstPtr == NULL) {
	textPtr->firstPtr = textPtr->lastPtr = itemPtr;
    } else {
	itemPtr->prevPtr = textPtr->lastPtr; 
	textPtr->lastPtr->nextPtr = itemPtr;
    }
    textPtr->numItems++;
}

/*
 *---------------------------------------------------------------------------
 *
 * DeleteItem --
 *
 *      Deletes an item.
 *
 *---------------------------------------------------------------------------
 */
static int 
DeleteItem(RText *textPtr, Item *itemPtr)
{
    Item *nextPtr, *prevPtr;

    if (textPtr->firstPtr == itemPtr) {
	textPtr->firstPtr = itemPtr->nextPtr;
    }
    if (textPtr->lastPtr == itemPtr) {
	textPtr->lastPtr = itemPtr->prevPtr;
    }
    nextPtr = itemPtr->nextPtr;
    if (nextPtr != NULL) {
	nextPtr->prevPtr = itemPtr->prevPtr;
    }
    prevPtr = itemPtr->prevPtr;
    if (prevPtr != NULL) {
	prevPtr->nextPtr = itemPtr->nextPtr;
    }
    textPtr->numItems--;
    (*itemPtr->classPtr->freeProc)(textPtr, itemPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * DeleteItem --
 *
 *      Deletes an item.
 *
 *---------------------------------------------------------------------------
 */
static int 
ComputeGeometry(RText *textPtr, Item *itemPtr)
{
    (*itemPtr->classPtr->geomProc)(textPtr, itemPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ParseTextString --
 *
 *      Parses a normal text string, detecting newlines and tabs.
 *
 *---------------------------------------------------------------------------
 */
static int 
ParseTextString(RText *textPtr, TextItem *delItemPtr)
{
}

/*
 *---------------------------------------------------------------------------
 *
 * NewTag --
 *
 *      Creates a new tag structure.  A tag contains information about how
 *	to draw a text item.  These attributes are combined to form the GC
 *      to draw the text.
 *
 * Results:
 *      Returns a pointer to the new tag structure.
 *
 *---------------------------------------------------------------------------
 */
static Tag *
NewTag(Tcl_Interp *interp, RText *textPtr, const char *tagName)
{
    Tag *tagPtr;
    Blt_HashEntry *hPtr;
    int isNew;
    char string[200];

    hPtr = Blt_CreateHashEntry(&textPtr->tagTable, tagName, &isNew);
    if (!isNew) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "a tag \"", tagName, 
                "\" already exists in \"", Tk_PathName(textPtr->name), "\"",
                (char *)NULL);
        }
        return NULL;
    }
    tagPtr = Blt_AssertCalloc(1, sizeof(Tag));
    tagPtr->textPtr = textPtr;
    tagPtr->flags = 0;
    tagPtr->name = Blt_GetHashKey(&textPtr->tagTable, hPtr);
    Blt_SetHashValue(hPtr, tagPtr);
    tagPtr->hashPtr = hPtr;
    return tagPtr;
}

static int
ConfigureTag(RText *textPtr, Tag *tagPtr)
{
    if (Blt_ConfigModified(tagSpecs, "-font", "-*pad*", "-state",
                           "-text", "-window*", (char *)NULL)) {
        textPtr->flags |= (LAYOUT_PENDING | SCROLL_PENDING | REDRAW_ALL);
    }
    NotifyClients(textPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * AddTagRange --
 *
 *      Add a new tag range to the text. If the tag already exists at the
 *      exact given range, don't do anything.  Otherwise append the new
 *      tag range to the end of the list.
 *
 *---------------------------------------------------------------------------
 */
static int
AddTagRange(RText *textPtr, Tag *tagPtr, int firstPos, int lastPos)
{
    TagRange *rangePtr, *lastPtr, *nextPtr;

    lastPtr = NULL;
    for (rangePtr = textPtr->firstRangePtr; rangePtr != NULL;
         rangePtr = nextPtr) {
        
        nextPtr = rangePtr->nextPtr;
        if ((rangePtr->firstPos == firstPos) && 
            (rangePtr->lastPos == lastPos) && (rangePtr->tagPtr == tagPtr)) {
            return TCL_OK;              /* Tag already exists. */
        }
        lastPtr = rangePtr;
    }
    rangePtr = Blt_AssertMalloc(sizeof(TagRange));
    rangePtr->firstPos = firstPos;
    rangePtr->lastPos = lastPos;
    rangePtr->tag = tagPtr;
    rangePtr->nextPtr = NULL;
    if (lastPtr == NULL) {
        textPtr->firstRangePtr = rangePtr;
    } else {
        lastPtr->nextPtr = rangePtr;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * AdjustTagRanges --
 *
 *      For all affected tag ranges, adjust their positions. The new
 *      characters are inserted before the end of the tag range, then the
 *      tag range must be adjusted.  If the insertion occurs within the tag
 *      range, the range is expanded accordingly.
 *
 *---------------------------------------------------------------------------
 */
static int
AdjustTagRanges(RText *textPtr, int insertPos, int numBytes)
{
    TagRange *rangePtr;

    for (rangePtr = textPtr->firstRangePtr; rangePtr != NULL;
         rangePtr = rangePtr->nextPtr) {
        if (insertPos < rangePtr->firstPos) {
            rangePtr->firstPos += numBytes;
            rangePtr->lastPos += numBytes; /* Before tag. */
        } else if (insertPos < rangePtr->lastPos) {
            rangePtr->lastPos += numBytes; /* Inside of tag. */
        }
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * AdjustImageOffset --
 *
 *      For all affected images, adjust their positions. The new
 *      characters are inserted before the insertion point of the image.
 *
 *---------------------------------------------------------------------------
 */
static int
AdjustImageOffsets(RText *textPtr, int insertPos, int numBytes)
{
    Tag *tagPtr;

    for (imgPtr = textPtr->firstImgPtr; imgPtr != NULL; 
         imgPtr = imgPtr->nextPtr) {
        if (insertPos < imgPtr->insertPos) {
            imgPtr->insertPos += numBytes;
        }
    }
}

static int
RemoveImages(RText *textPtr, int firstPos, int lastPos)
{
    Tag *tagPtr;
    TextImage *firstPtr, *lastPtr;

    lastPtr = NULL;
    for (imgPtr = textPtr->firstImgPtr; imgPtr != NULL; imgPtr = nextPtr) {
        nextPtr = imgPtr->nextPtr;
        if ((imgPtr->insertPos >= firstPos) && (imgPtr->insertPos < lastPos)) {
            if (lastPtr == NULL) {
                textPtr->firstPtr = nextPtr;
            } else {
                lastPtr->nextPtr = nextPtr;
            }
            DeleteTextImage(imgPtr);
        }
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * DeleteText --
 *
 *      Deletes characters between the first byte offset and the last.  The
 *      last offset is 1 slot after the last deleted character.  Is is assumed
 *      that tags and images have been already deleted from this range.
 *
 *---------------------------------------------------------------------------
 */
static int
DeleteText(RText *textPtr, int firstPos, int lastPos)
{
    char *newText;
    int length;

    length = lastPos - firstPos;
    newText = Blt_AssertMalloc(textPtr->numBytes - length);
    memcpy(newText, textPtr->text, firstPos);
    memcpy(newText + firstPos, textPtr->text, length);
    Blt_Free(textPtr->text);
    textPtr->text = newText;
    textPtr->numBytes -= length;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * InsertText --
 *
 *      Inserts characters at the given byte offset.  It is assumed that
 *      the substring is a complete UTF sequence and that the insert position
 *      represents a valid location between UTF characters.  
 *
 *---------------------------------------------------------------------------
 */
static int
InsertText(RText *textPtr, const char *string, int length, int insertPos)
{
    char *newText;

    if (insertPos >= textPtr->numBytes) {
        newText = Blt_AssertRealloc(textPtr->text, textPtr->numBytes + length);
        memcpy(newText + textPtr->numBytes, string, length);
    } else {
        newText = Blt_AssertMalloc(textPtr->numBytes + length);
        memcpy(newText, textPtr->text, insertPos);
        memcpy(newText + insertPos, string, length);
        memcpy(newText + insertPos + length, textPtr->text + insertPos, 
               textPtr->numBytes - insertPos);
        Blt_Free(textPtr->text);
    }
    textPtr->text = newText;
    textPtr->numBytes += length;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * GetByteOffset --
 *
 *      Returns the byte offset of the given character index. New lines,
 *      tabs, and images occupy one byte/character.  The returned offsets
 *      returned always end/start valid UTF character sequences.
 * 
 *      Valid character indexes are
 *      "end"           The index after the last character in string.
 *      lineNum,charNum The index of the line/character. It is not an error
 *                      if the lineNum or charNum exceeds the number of 
 *                      lines or characters in the string.
 *
 *---------------------------------------------------------------------------
 */
static int
GetByteOffset(Tcl_Interp *interp, RText *textPtr, Tcl_Obj *objPtr, 
              int *offsetPtr)
{
    char c;
    const char *string;

    string = Tcl_GetString(objPtr);
    c = string[0];
    if ((c == 'e') && (strcmp(string, "end") == 0)) {
        *offsetPtr = textPtr->numBytes;
    } else {
        const char *p;
        char *dot;
        const char *endCharPtr;
        int lineNum, charNum;

        dot = strchr(string, '.');
        if (dot == NULL) {
            return TCL_ERROR;
        }
        *dot = '\0';
        *dot = '.';
        if ((Tcl_GetInt(interp, string, &lineNum) != TCL_OK) ||
            (Tcl_GetInt(interp, dot+1, &charNum) != TCL_OK)) {
            return TCL_ERROR;
        }
        /* Count the number of newlines and then characters in the text
         * string to find the byte offset in the text string. Newlines, tabs
         * and images all count as 1 byte. */
        endCharPtr = textPtr->string + textPtr->numBytes;
        for(p = textPtr->string; p < endCharPtr; p++) {
            if (*p == '\n') {
                lineNum--;
                if (lineNum == 0) {
                    p++;
                    break;
                }
            }
        }
        for(/*empty*/; p < endCharPtr; p++) {
            charNum--;
            if ((charNum == -1) || (*p == '\n')) {
                p++;
                break;
            }
        }
        *offsetPtr = p - textPtr->string;
    }    
    return TCL_OK;
}

static int
GetImageFromObj(Tcl_Interp *interp, RText *textPtr, Tcl_Obj *objPtr,
                TextImage *imgPtrPtr)
{
}

static int
NewTextImage(Tcl_Interp *interp, RText *textPtr, Tcl_Obj *objPtr,
             TextImage *imgPtrPtr)
{
    Blt_HashEntry *hPtr;

    tkImage = Tk_GetImage(interp, textPtr->tkwin, Tcl_GetString(objPtr), 
        ImageChangedProc, textPtr);
    if (tkImage == NULL) {
        return TCL_ERROR;
    }
    hPtr = Blt_CreateEntry(textPtr->imageTable, tkImage, &isNew);
    if (isNew) {
        return TCL_ERROR;
    }
    imgPtr = Blt_AssertCalloc(1, sizeof(TextImage));
    imgPtr->insertPos = -1;             /* Image hasn't been inserted yet. */
    imgPtr->tkImage = tkImage;
    *imgPtrPtr = imgPtr;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * InsertOp --
 *
 *      Insert new text items into a text object.
 *
 *      textName insert index textString ?tagName text ...?
 *
       line.char   Indicates  char’th  character on line line.  Lines are num-
                   bered from 1 for consistency with other UNIX programs  that
                   use  this  numbering scheme.  Within a line, characters are
                   numbered from 0.  If char is end then it refers to the new-
                   line character that ends the line.

       end         Indicates the end of the text (the character just after the
                   last newline).

       mark        Indicates  the  character just after the mark whose name is
                   mark.

       tag.first   Indicates the first character in the  text  that  has  been
                   tagged  with tag.  This form generates an error if no char-
                   acters are currently tagged with tag.

       tag.last    Indicates the character just after the last one in the text
                   that  has  been  tagged  with  tag.  This form generates an
                   error if no characters are currently tagged with tag.

       imageName   Indicates the position of the embedded image whose name  is
                   imageName.   This  form  generates  an error if there is no
                   embedded image by the given name.

       If the base could match more than one of the above  forms,  such  as  a
       mark and imageName both having the same value, then the form earlier in
       the above list takes precedence.  If modifiers follow the  base  index,
       each  one  of  them  must have one of the forms listed below.  Keywords
       such as chars and wordend may be abbreviated as long as  the  abbrevia-
       tion is unambiguous.

       + count ?submodifier? chars
              Adjust  the  index  forward by count characters, moving to later │
              lines in the text if necessary.  If there are fewer  than  count │
              characters  in  the  text  after the current index, then set the │
              index to the last index in the text.  Spaces on either  side  of │
              count are optional.  If the display submodifier is given, elided │
              characters are skipped over without being counted.   If  any  is │
              given, then all characters are counted.  For historical reasons, │
              if neither modifier is given then the count actually takes place │
              in  units  of  index  positions (see indices for details).  This │
              behaviour may be changed in a future major release,  so  if  you │
              need  an  index count, you are encouraged to use indices instead │
              wherever possible.

       - count ?submodifier? chars
              Adjust the index backward by count characters, moving to earlier
              lines  in  the text if necessary.  If there are fewer than count
              characters in the text before the current index,  then  set  the
              index  to  the  first index in the text (1.0).  Spaces on either │
              side of count are  optional.   If  the  display  submodifier  is │
              given, elided characters are skipped over without being counted. │
              If any is given, then all characters are counted.  For  histori- │
              cal  reasons,  if neither modifier is given then the count actu- │
              ally takes place in units of index positions  (see  indices  for │
              details).   This  behaviour  may  be  changed  in a future major │
              release, so if you need an index count, you  are  encouraged  to │
              use indices instead wherever possible.

       + count ?submodifier? indices
              Adjust  the  index  forward  by count index positions, moving to │
              later lines in the text if necessary.  If there are  fewer  than │
              count  index positions in the text after the current index, then │
              set the index to the last index position in the text.  Spaces on │
              either  side of count are optional.  Note that an index position │
              is either a single character  or  a  single  embedded  image  or │
              embedded  window.   If  the display submodifier is given, elided │
              indices are skipped over  without  being  counted.   If  any  is │
              given,  then  all  indices are counted; this is also the default │
              behaviour if no modifier is given.

       - count ?submodifier? indices
              Adjust the index backward by count index  positions,  moving  to │
              earlier lines in the text if necessary.  If there are fewer than │
              count index positions in the text before the current index, then │
              set  the  index  to  the first index position (1.0) in the text. │
              Spaces on either side of count are  optional.   If  the  display │
              submodifier  is  given,  elided indices are skipped over without │
              being counted.  If any is given, then all indices  are  counted; │
              this is also the default behaviour if no modifier is given.

       + count ?submodifier? lines
              Adjust  the  index  forward  by  count lines, retaining the same │
              character position within the line.  If  there  are  fewer  than │
              count  lines  after  the line containing the current index, then │
              set the index to refer to the same  character  position  on  the │
              last  line of the text.  Then, if the line is not long enough to │
              contain a character at the indicated character position,  adjust │
              the  character  position  to  refer to the last character of the │
              line  (the  newline).   Spaces  on  either  side  of  count  are │
              optional.  If the display submodifier is given, then each visual │
              display line is counted separately.  Otherwise, if  any  (or  no │
              modifier)  is  given, then each logical line (no matter how many │
              times it is visually wrapped) counts just once.  If the relevant │
              lines  are  not  wrapped, then these two methods of counting are │
              equivalent.

       - count ?submodifier? lines
              Adjust the index backward by count logical lines, retaining  the │
              same  character  position  within  the line.  If there are fewer │
              than count lines before the line containing the  current  index, │
              then  set  the  index to refer to the same character position on │
              the first line of the text.  Then,  if  the  line  is  not  long │
              enough  to  contain a character at the indicated character posi- │
              tion, adjust the character position to refer to the last charac- │
              ter  of  the line (the newline).  Spaces on either side of count │
              are optional.  If the display submodifier is  given,  then  each │
              visual  display  line  is counted separately.  Otherwise, if any │
              (or no modifier) is given, then each logical line (no matter how │
              many  times  it  is  visually wrapped) counts just once.  If the │
              relevant lines are not wrapped, then these two methods of count- │
              ing are equivalent.

       ?submodifier? linestart
              Adjust  the  index  to refer to the first index on the line.  If │
              the display submodifier is given, this is the first index on the │
              display line, otherwise on the logical line.

       ?submodifier? lineend
              Adjust  the  index  to  refer to the last index on the line (the │
              newline).  If the display submodifier is given, this is the last │
              index on the display line, otherwise on the logical line.

       ?submodifier? wordstart
              Adjust  the  index  to  refer to the first character of the word │
              containing the current index.  A word consists of any number  of │
              adjacent characters that are letters, digits, or underscores, or │
              a single character that is not one of  these.   If  the  display │
              submodifier  is given, this only examines non-elided characters, │
              otherwise all characters (elided or not) are examined.

       ?submodifier? wordend
              Adjust the index to refer to the character just after  the  last │
              one  of  the  word containing the current index.  If the current │
              index refers to the last character of the text then  it  is  not │
              modified.   If the display submodifier is given, this only exam- │
              ines non-elided characters, otherwise all characters (elided  or │
              not) are examined.                                               │

       If  more than one modifier is present then they are applied in left-to- │
       right order.  For example, the index “end -  1  chars”  refers  to  the │
       next-to-last  character in the text and “insert wordstart - 1 c” refers │
       to the character just before the first one in the word  containing  the │
       insertion  cursor.   Modifiers  are  applied one by one in this left to │
       right order, and after each step the resulting index is constrained  to │
       be  a  valid index in the text widget.  So, for example, the index “1.0 │
       -1c +1c” refers to the index “2.0”.                                     │

       Where modifiers result in index changes by display lines, display chars │
       or  display  indices,  and the base refers to an index inside an elided │
       tag, that base index is considered to be equivalent to the  first  fol- │
       lowing non-elided index.

 *      textName insert index textString ?tagName text ...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
InsertOp(ClientData clientData, Tcl_Interp *interp, int objc, 
         Tcl_Obj *const *objv)
{
    RText *textPtr = clientData; 
    int i;

    if (GetByteOffset(interp, textPtr, objv[2], &insertPos) != TCL_OK) {
        return TCL_ERROR;
    }
    for (i = 3; i < objc; i += 2) {
        const char *string;
        int insertPos, nextPos;

        string = Tcl_GetStringFromObj(objv[i], &length);
        nextPos = insertPtr + length;
        AdjustTagRanges(textPtr, insertPos, length);
        AdjustImageOffsets(textPtr, insertPos, length);
        if ((i+1) < objc) {
            Tag *tagPtr;

            if (GetTagFromObj(textPtr, objv[i+1], &tagPtr) != TCL_OK) {
                return TCL_ERROR;
            }
            AddTagRange(textPtr, tagPtr, insertPos, nextPos); 
        }
        InsertText(textPtr, string, length, insertPos);
        insertPos = nextPos;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ImageCgetOp --
 *
 *      textName image cget imageName option
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ImageCgetOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    RText *textPtr = clientData; 
    int i;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ImageConfigureOp --
 *
 *      Configure an image in a text object.
 *
 *      textName image configure imageName ?option value...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ImageConfigureOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    RText *textPtr = clientData; 
    int i;

    if (GetByteOffset(interp, textPtr, objv[2], &insertPos) != TCL_OK) {
        return TCL_ERROR;
    }
    for (i = 3; i < objc; i += 2) {
        const char *string;
        int insertPos, nextPos;

        string = Tcl_GetStringFromObj(objv[i], &length);
        nextPos = insertPtr + length;
        AdjustTagRanges(textPtr, insertPos, nextPos);
        AdjustImageOffsets(textPtr, insertPos, length);
        if ((i+1) < objc) {
            Tag *tagPtr;

            if (GetTagFromObj(textPtr, objv[i+1], &tagPtr) != TCL_OK) {
                return TCL_ERROR;
            }
            AddTagRange(textPtr, tagPtr, insertPos, nextPos); 
        }
        InsertText(textPtr, string, length, insertPos);
        insertPos = nextPos;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ImageInsertOp --
 *
 *      Inserts a Tk image into a text object at a given character
 *      position.  The Tk image cannot already have been inserted into the
 *      input string.  In that case, the character representing the image
 *      must be deleted first.
 *
 *      textName image insert tkImageName insertPos ?option value...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ImageInsertOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    RText *textPtr = clientData; 
    Tk_Image tkImage;
    int insertPos;

    if (GetByteOffset(interp, textPtr, objv[4], &insertPos) != TCL_OK) {
        return TCL_ERROR;
    }
    tkImage = Tk_GetImage(interp, textPtr->tkwin, Tcl_GetString(objv[3]), 
        ImageChangedProc, textPtr);
    if (tkImage == NULL) {
        return TCL_ERROR;
    }
    hPtr = Blt_CreateHashEntry(&textPtr->imageTable, tkImage, &isNew);
    if (!isNew) {
        imgPtr = Blt_GetHashValue(hPtr);
        if (imgPtr->insertPos != -1) { 
            Tcl_AppendResult(interp, "can't insert image \"", 
                         Tcl_GetString(objv[3]), 
                         "\": image is already used text string.", 
                         (char *)NULL);
            return TCL_ERROR;
        }
    } else {
        imgPtr = NewTextImage(interp, textPtr, objv[3]);
        if (imgPtr == NULL) {
            return TCL_ERROR;
        }
    }
    /* An image occupies one character in the input string. */
    nextPos = insertPos + 1;
    AdjustTagRanges(textPtr, insertPos, 1);
    AdjustImageOffsets(textPtr, insertPos, 1);
    InsertText(textPtr, "\1", 1, insertPos);
    imgPtr->insertPos = insertPos;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * TagCgetOp --
 *
 *        pathName tag cget tagName option
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
TagCgetOp(ClientData clientData, Tcl_Interp *interp, int objc, 
          Tcl_Obj *const *objv)
{
    Tag *tagPtr;
    RText *textPtr = clientData; 

    if (GetTagFromObj(interp, textPtr, objv[3], &tagPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    return Blt_ConfigureValueFromObj(interp, textPtr->tkwin, tagSpecs,
        (char *)tagPtr, objv[4], 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * TagConfigureOp --
 *
 *      This procedure is called to process a list of configuration options
 *      database, in order to reconfigure the options for one or more tags
 *      in the widget.
 *
 *        pathName tag configure tagName ?option value ...?
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then interp->result
 *      contains an error message.
 *
 * Side Effects:
 *      Configuration information, such as text string, colors, font, etc. get
 *      set; old resources get freed, if there were any.  The widget is
 *      redisplayed if needed.
 *
 *---------------------------------------------------------------------------
 */
static int
TagConfigureOp(ClientData clientData, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    Tag *tagPtr;
    RText *textPtr = clientData; 
    int result;

    if (GetTagFromObj(interp, textPtr, objv[3], &tagPtr) != TCL_OK) {
	return TCL_ERROR;   /* Can't find tag. */
    }
    if (objc == 4) {
	return Blt_ConfigureInfoFromObj(interp, textPtr->tkwin, tagSpecs, 
                (char *)tagPtr, (Tcl_Obj *)NULL, 0);
    } else if (objc == 5) {
	return Blt_ConfigureInfoFromObj(interp, textPtr->tkwin, tagSpecs, 
                (char *)tagPtr, objv[4], 0);
    }
    Tcl_Preserve(tagPtr);
    result = Blt_ConfigureWidgetFromObj(interp, textPtr->tkwin, tagSpecs, 
                objc - 4, objv + 4, (char *)tagPtr, BLT_CONFIG_OBJV_ONLY);
    Tcl_Release(tagPtr);
    if (result == TCL_ERROR) {
	return TCL_ERROR;
    }
    if (ConfigureTag(textPtr, tagPtr) != TCL_OK) {
	return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * TagCreateOp --
 *
 *      textName tag create tagName ?option value ...?
 *
 *---------------------------------------------------------------------------
 */
static int
TagCreateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
	    Tcl_Obj *const *objv)
{
    RText *textPtr = clientData; 
    Tag *tagPtr;
    const char *tagName;
    
    tagName = Tcl_GetString(objv[3]);
    if (GetTagFromObj(NULL, textPtr, objv[i], &tagPtr) == TCL_OK) {
        Tcl_AppendResult(interp, "tag \"", tagName, 
			 "\" already exists in text object \"", 
			 textPtr->name, "\"", (char *)NULL);
	return TCL_ERROR;
    }
    tagPtr = CreateTag(interp, textPtr, tagName);
    if (tagPtr == NULL) {
	return TCL_ERROR;
    }
    if (Blt_ConfigureWidgetFromObj(interp, textPtr->tkwin, tagSpecs, 
           objc, objv, (char *)tagPtr, flags) != TCL_OK) {
	DeleteTag(textPtr, tagPtr);
        return TCL_ERROR;
    }
    if (ConfigureTag(textPtr, tagPtr) != TCL_OK) {
	DeleteTag(textPtr, tagPtr);
	return TCL_ERROR;
    }
    Tcl_SetStringObj(Tcl_GetObjResult(interp), tagPtr->name, -1);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * TagDeleteOp --
 *
 *      textName tag delete ?tagName ...?
 *
 *---------------------------------------------------------------------------
 */
static int
TagDeleteOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    RText *textPtr = clientData; 
    int i;

    for (i = 3; i < objc; i++) {
        Tag *tagPtr;
        
        if (GetTagFromObj(interp, textPtr, objv[i], &tagPtr) != TCL_OK) {
            return TCL_ERROR;
        }
	DeleteTag(textPtr, tagPtr);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * TagExistsOp --
 *
 *      Indicates if the given tag exists in the text object. 
 *
 *      textName tag exists tagName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
TagExistsOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    Tag *tagPtr;
    RText *textPtr = clientData; 
    int state;

    state = FALSE;
    if (GetTagFromObj(NULL, textPtr, objv[3], &tagPtr) == TCL_OK) {
	state = TRUE;
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * TagNamesOp --
 *
 *      Returns the names of all the tags (used or unused) in the text
 *      object.  If one of more pattern arguments are provided, then only
 *      the tags matching in those patterns are returned.
 *
 *      textName tag names ?pattern ...?
 *
 *---------------------------------------------------------------------------
 */
static int
TagNamesOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    RText *textPtr = clientData; 
    Tcl_Obj *listObjPtr, *objPtr;
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;

    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
    for (hPtr = Blt_FirstHashEntry(&textPtr->tagTable, &iter); hPtr != NULL;
	 hPtr = Blt_NextHashEntry(&iter)) {
	Tag *tagPtr;
	int found;

	tagPtr = Blt_GetHashValue(hPtr);
	found = FALSE;
	if (objc == 3) {
	    found = TRUE;
	} else {
	    int i;

	    for (i = 3; i < objc; i++) {
		const char *pattern;

		pattern = Tcl_GetString(objv[i]);
		if (Tcl_StringMatch(tagPtr->name, pattern)) {
		    found = TRUE;
		    break;
		}
	    }
	}
	if (found) {
	    objPtr = Tcl_NewStringObj(tagPtr->name, -1);
	    Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
	}
    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}


/*
 *---------------------------------------------------------------------------
 *
 * TagOp --
 *
 *      This procedure handles tag operations.
 *
 * Results:
 *      A standard TCL result.
 *
 *	textName tag op ...
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec tagOps[] =
{
    {"cget",      2, TagCgetOp,      5, 5, "tagName option",},
    {"configure", 2, TagConfigureOp, 4, 0, "tagName ?option value ...?",},
    {"create",    2, TagCreateOp,    4, 0, "tagName ?option value ...?",},
    {"delete",    1, TagDeleteOp,    3, 0, "?tagName ...?",},
    {"exists",    1, TagExistsOp,    4, 4, "tagName",},
    {"names",     1, TagNamesOp,     3, 0, "?pattern ...?",},
};

static int numTagOps = sizeof(tagOps) / sizeof(Blt_OpSpec);

static int
TagOp(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numTagOps, tagOps, BLT_OP_ARG2, 
           objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}


void
DrawItem(Tk_Window tkwin, Drawable drawable, int depth, float angle, 
	 int x, int y, Text *textPtr, TextItem *itemPtr, int maxLength)
{
    TextFragment *fragPtr;
    Tag *tagPtr;

    tagPtr = (itemPtr->tagPtr == NULL) ? textPtr->tagPtr : itemPtr->tagPtr;
    Blt_Font_SetClipRegion(tagPtr->font, textPtr->rgn);
    tx = itemPtr->x + itemPtr->rx, ty = itemPtr->y + itemPtr->ry;
    if ((maxLength > 0) && ((itemPtr->width + itemPtr->x) > maxLength)) {
	Blt_DrawWithEllipsis(tkwin, drawable, tagPtr->gc, tagPtr->font, 
	     depth, angle, itemPtr->text, itemPtr->numBytes, tx, ty, 
		 maxLength - itemPtr->x);
        } else {
            Blt_Font_Draw(Tk_Display(tkwin), drawable, tagPtr->gc, font, depth, angle, 
                fp->text, fp->numBytes, tx, ty);
        }
    }
    if (layoutPtr->underlinePtr != NULL) {
        int tx, ty;

        /* Single underlined character. */
        fp = layoutPtr->underlinePtr;
        tx = x + fp->rx, ty = y + fp->ry;
        Blt_Font_UnderlineChars(Tk_Display(tkwin), drawable, gc, font, fp->text,
                fp->numBytes, tx, ty, layoutPtr->underline, 
                layoutPtr->underline + 1, maxLength);
    }
}


/*
 *---------------------------------------------------------------------------
 *
 * Blt_Ts_DrawLayout --
 *
 *      Draw a text string, possibly rotated, using the the given window
 *      coordinates as an anchor for the text bounding box.  If the text is
 *      not rotated, simply use the X text drawing routines. Otherwise,
 *      generate a bitmap of the rotated text.
 *
 * Results:
 *      Returns the x-coordinate to the right of the text.
 *
 * Side Effects:
 *      Text string is drawn using the given font and GC at the the given
 *      window coordinates.
 *
 *      The Stipple, FillStyle, and TSOrigin fields of the GC are modified
 *      for rotated text.  This assumes the GC is private, *not* shared
 *      (via Tk_GetGC)
 *
 *---------------------------------------------------------------------------
 */
void
Blt_Ts_DrawLayout(
    Tk_Window tkwin,
    Drawable drawable,
    RText *textPtr,
    int x, int y)                       /* Window coordinates to draw
                                         * text */
{
    float angle;

    if ((textPtr->gc == NULL) || (textPtr->flags & UPDATE_GC)) {
        TagResetStyle(tkwin, textPtr);
    }
    angle = (float)FMOD(textPtr->angle, 360.0);
    if (angle < 0.0) {
        angle += 360.0;
    }
    Blt_Font_SetClipRegion(textPtr->font, stylePtr->rgn);
    if (angle == 0.0) {
        /*
         * This is the easy case of no rotation. Simply draw the text using
         * the standard drawing routines.  Handle offset printing for
         * engraved (disabled) text.
         */
        DrawStandardLayout(tkwin, drawable, stylePtr, layoutPtr, x, y);
    } else if (Blt_Font_CanRotate(stylePtr->font, angle)) {
        Blt_DrawTextWithRotatedFont(tkwin, drawable, angle, stylePtr, 
                layoutPtr, x, y);
    } else {
        stylePtr->angle = (float)angle;
        Blt_DrawTextWithRotatedBitmap(tkwin, drawable, angle, stylePtr, 
                layoutPtr, x, y);
    }
    Blt_Font_SetClipRegion(stylePtr->font, NULL);
}

static void
ParseChunk(const char *string, int numBytes) {
    const char *firstPtr;
    int lineNum;

    lastAttr = GetCombinedAttributes(textPtr, parserPtr, lineNum, charIndex);
    first = p = string;
    for (p = string; p < (string + numBytes); /*empty*/) {
        char c;

        p += Tcl_UtfToUniChar(p, &ch);
        parsePtr->size = p - first;
        c = (unsigned char)(ch & 0xff);
        if (c == '\n') {
            if (size > 0) {
                AddTextItem(textPtr, first, size - 1, parserPtr);
            }
            parserPtr->y += GetLineHeight(parserPtr);
            parserPtr->lineNum++;
            parserPtr->size = 0;
            continue;
        } else if (c == '\t') {
            parserPtr->x += GetNextTabStop(parserPtr);
            continue;
        } else if (parserPtr->wrapLength > 0) {
            length = GetLastLineLength();
            if (length > parserPtr->wrapLength) {
                p = GetEndofLastWord(parserPtr);
                if (p != NULL) {
                    AddTextItem(textPtr, first, size - first, parserPtr);
                    first = p;
                    size = p - first;
                    continue;
                }
            }
        }
        attrPtr = GetCombinedAttributes(parsePtr);
        if (attrPtr != lastAttrPtr) {
            /* Add previous text */
            AddTextItem(textPtr, first, size - 1, parserPtr);
            lastAttrPtr = attrPtr;
        }
    }
    if (size > 0) {
        AddTextItem(textPtr, first, size - 1, parserPtr);
    }

    if { $gc != $lastGC } {
      add(buf-1, size, lastGC);
      lastGC = gc
    }
            
    }        
        
  foreach char in string {
    if \n  {
      if { $size > 0 } {
	add(buf, size, lastGC) 
      }
      h += GetLineHeight()
      add(nl,-1)
    }
    if \t {
      set currentX [nextTabStop x]
    }
    gc = getCombinedGC(charIndex);
    if { $gc != $lastGC } {
      add(buf-1, size, lastGC);
      lastGC = gc
    }
    if { x > $wrapLength } {
      p = findLastWord(buf)
      add(buf, p - buf, lastGC);
      buf = p+1;
    }
  }
  if (char == \0 {
      add(buf, size, lastGC);
    }
}
