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
\it
\bf
\sl
\rm
\font{arial 10 italic}
\fontname{arial}
\fontsize{size}
\fontdelta{size}
\color{color}

Newline is a character kept in a single text item.

*/

typedef struct _SpecialChar {
    const char *name;
    int uniChar;
} SpecialChar;

static SpecialChar specialChars[] = {
    "0",            	0x2205,
    "Delta",		0x0394,
    "Gamma",		0x0393,
    "Im",           	0x2111,
    "Lambda",		0x039B,
    "Leftarrow",	0x21D0,
    "Omega",		0x03A9,
    "Phi",          	0x03A6,
    "Pi",           	0x03A0,
    "Psi",          	0x03A8,
    "Re",           	0x211C,
    "Rightarrow",	0x21D2,
    "Sigma",		0x03A3,
    "Theta",		0x0398,
    "Upsilon",		0x03D2,
    "Xi",           	0x039E,
    "aleph",		0x2135,
    "alpha",		0x03B1,
    "angle",		0x2220,
    "approx",		0x2248,
    "ast",          	0x2217,
    "beta",         	0x03B2,
    "bullet",		0x2219,
    "cap",          	0x2229,
    "cdot",         	0x22C5,
    "chi",          	0x03C7,
    "circ",         	0x2218,
    "clubsuit",		0x2663,
    "cong",         	0x2245,
    "copyright",	0x00A9,
    "cup",          	0x222A,
    "deg",          	0x00B0,
    "delta",		0x03B4,
    "diamondsuit",	0x2666,
    "div",          	0x00F7,
    "downarrow",	0x2193,
    "epsilon",		0x03B5,
    "equiv",		0x2261,
    "eta",          	0x03B7,
    "exists",		0x2203,
    "forall",		0x2200,
    "gamma",		0x03B3,
    "geq",          	0x2265,
    "heartsuit",	0x2665,
    "in",           	0x2208,
    "infty",		0x221E,
    "int",          	0x222B,
    "iota",         	0x03B9,
    "kappa",		0x03BA,
    "lambda",		0x03BB,
    "langle",		0x27E8,
    "lceil",		0x2308,
    "ldots",		0x2026,
    "leftarrow",	0x2190,
    "leftrightarrow",	0x2194,
    "leq",          	0x2264,
    "lfloor",		0x230A,
    "mid",          	0x2223,
    "mu",           	0x03BC,
    "nabla",		0x2207,
    "neg",          	0x00AC,
    "neq",          	0x2260,
    "ni",           	0x220B,
    "nu",           	0x03BD,
    "o",            	0x03BF,
    "omega",		0x03C9,
    "oplus",		0x2295,
    "oslash",		0x2298,
    "otimes",		0x2297,
    "partial",		0x2202,
    "perp",         	0x22A5,
    "phi",          	0x03C6,
    "pi",           	0x03C0,
    "pm",           	0x00B1,
    "prime",		0x2032,
    "propto",		0x221D,
    "psi",          	0x03C8,
    "rangle",		0x27E9,
    "rceil",		0x2309,
    "rfloor",		0x230B,
    "rho",          	0x03C1,
    "rightarrow",	0x2192,
    "sigma",		0x03C3,
    "sim",          	0x223C,
    "spadesuit",	0x2660,
    "subset",		0x2282,
    "subseteq",		0x2286,
    "supset",		0x2283,
    "supseteq",		0x2287,
    "surd",         	0x221A,
    "tau",          	0x03C4,
    "theta",		0x03B8,
    "times",		0x00D7,
    "uparrow",		0x2191,
    "upsilon",		0x03C5,
    "varpi",		0x03D6,
    "varsigma",		0x03C2,
    "vartheta",		0x03D1,
    "vee",          	0x2228,
    "wedge",		0x2227,
    "wp",           	0x2118,
    "xi",           	0x03BE,
    "zeta",         	0x03B6,
};

static int numSpecialChars = sizeof(specialChars) / sizeof(SpecialChar);

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

/* Attribute flags. */
#define NONE       (1<<0)
#define UNDERLINE  (1<<0)
#define STRIKETHROUGH (1<<1)
#define SPECIAL_MASK (UNDERLINE|STRIKETHROUGH)

#define NORMAL     (1<<2)
#define BOLD       (1<<3)
#define WEIGHT_MASK (NORMAL|BOLD)

#define ROMAN      (1<<4)
#define ITALIC     (1<<5)
#define OBLIQUE    (1<<6)
#define SLANT_MASK (ROMAN|ITALIC|OBLIQUE)

#define WRAP_WORD  (1<<7)

#define WRAP_IGNORE  (0)
#define NO_WRAP      (-1)

#define FONT_SIZE_IGNORE  (0)

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
 *
 *      Tags are sets of attributes to be applied to the text.  A range of
 *      text may have 1 or more tags applied to it. The actual GC used will
 *      be a composite of the tags.  If a field is NULL, this indicates to
 *      use this attribute from the previous tag, or the default tag used
 *      by the text object.
 */
typedef struct _Tag {
    unsigned int flags;			/* UNDERLINE, STRIKETHROUGH, ITALIC,
                                         * OBLIQUE, BOLD, */
    /* text Attributes */
    XColor *textColor;			/* If non-NULL, color to draw the
                                         * text. */
    Blt_Bg bg;				/* If non-NULL, background color of
					 * text. */
    Blt_Font font;                      /* If non-NULL, font to use to draw
					 * text. Otherwise use global font. */
    int fontSize;			/* Font size. 0 if none requested. */
    int fontDelta;                      /* Font size delta. */
    int offset;                         /* Offset +/- from baseline. */
    int wrapLength;                     /* # pixels at which to wrap the
                                         * text (i.e. create a new
                                         * line). 0=not set, -1=no wrap, or
                                         * positive value is the # of
                                         * pixels. */
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
NewImageItem(RText *textPtr, Tk_Image tkImage, Blt_HashEntry *hPtr)
{
    ImageItem *itemPtr;

    itemPtr = Blt_AssertCalloc(1, sizeof(ImageItem));
    itemPtr->classPtr = &imageItemClass;
    itemPtr->tkImage = tkImage;
    itemPtr->name = Blt_GetHashKey(&textPtr->imageTable, hPtr);
    itemPtr->hashPtr = hPtr;
    return (Item *)itemPtr;
}

static int
GetTag(Tcl_Interp *interp, RText *textPtr, const char *tagName, Tag **tagPtrPtr)
{
    Blt_HashEntry *hPtr;

    hPtr = Blt_FindHashEntry(&textPtr->tagTable, tagName);
    if (hPtr != NULL) {
        *tagPtrPtr = Blt_GetHashValue(hPtr);
        return TCL_OK;
    }
    if (interp != NULL) {
        Tcl_AppendResult(interp, "can't find a tag \"", tagName, "\"", 
                     (char *)NULL);
    }
    return TCL_ERROR;
}
    
static int
GetTagFromObj(Tcl_Interp *interp, RText *textPtr, Tcl_Obj *objPtr, 
              Tag **tagPtrPtr)
{
    const char *string;

    string = Tcl_GetString(objPtr);
    return GetTag(interp, textPtr, string, tagPtrPtr);
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
 *      the substring is a complete UTF sequence and that the insert
 *      position represents a valid location between UTF characters.
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
 *      last offset is 1 slot after the last deleted character.  Is is
 *      assumed that tags and images have been already deleted from this
 *      range.
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
 *      the substring is a complete UTF sequence and that the insert
 *      position represents a valid location between UTF characters.
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

static int
GetSpecialUnichar(const char *specName, unsigned int *chPtr)
{
    int low, high;

    low = 0;
    high = numSpecialChars - 1;
    while (low <= high) {
        int comp;
        int median;
        
        median = (low + high) >> 1;
        comp = strcmp(specName, specialChars[media].name);
        if (comp == 0) {
            *chPtr = specialChars[median].uniChar;
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
        nextPos = insertPos + length;
        AdjustTagRanges(textPtr, insertPos, length);
        AdjustImageOffsets(textPtr, insertPos, length);
        if ((i+1) < objc) {
            const char *tagName;
            int length;

            tagName = Tcl_GetStringFromObj(objv[i+1], &length);
            if (length > 0) {
                Tag *tagPtr;

                if (GetTag(interp, textPtr, tagName, &tagPtr) != TCL_OK) {
                    return TCL_ERROR;
                }
                AddTagRange(textPtr, tagPtr, insertPos, nextPos); 
            }
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
        nextPos = insertPos + length;
        AdjustTagRanges(textPtr, insertPos, nextPos);
        AdjustImageOffsets(textPtr, insertPos, length);
        if ((i+1) < objc) {
            const char *tagName;
            int length;

            tagName = Tcl_GetStringFromObj(objv[i+1], &length);
            if (length > 0) {
                Tag *tagPtr;

                if (GetTag(interp, textPtr, tagName, &tagPtr) != TCL_OK) {
                    return TCL_ERROR;
                }
                AddTagRange(textPtr, tagPtr, insertPos, nextPos); 
            }
        }
        InsertText(textPtr, string, length, insertPos);
        insertPos = nextPos;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ImageCreateOp --
 *
 *      textName image create tkImageName ?option value ...?
 *
 *---------------------------------------------------------------------------
 */
static int
ImageCreateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
              Tcl_Obj *const *objv)
{
    RText *textPtr = clientData; 
    Tag *tagPtr;
    const char *tagName;
    
    imageName = Tcl_GetString(objv[3]);
    if (GetImageFromObj(NULL, textPtr, objv[i], &tagPtr) == TCL_OK) {
        Tcl_AppendResult(interp, "image \"", imageName, 
			 "\" already exists in text object \"", 
			 textPtr->name, "\"", (char *)NULL);
	return TCL_ERROR;
    }
    imgPtr = CreateTag(interp, textPtr, tagName);
    if (imgPtr == NULL) {
	return TCL_ERROR;
    }
    if (Blt_ConfigureWidgetFromObj(interp, textPtr->tkwin, imgSpecs, 
           objc, objv, (char *)imgPtr, flags) != TCL_OK) {
	DeleteImage(textPtr, imgPtr);
        return TCL_ERROR;
    }
    if (ConfigureTag(textPtr, tagPtr) != TCL_OK) {
	DeleteTag(textPtr, tagPtr);
	return TCL_ERROR;
    }
    Tcl_SetStringObj(Tcl_GetObjResult(interp), imageName, -1);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ImageDeleteOp --
 *
 *      textName image delete ?imageName ...?
 *
 *---------------------------------------------------------------------------
 */
static int
ImageDeleteOp(ClientData clientData, Tcl_Interp *interp, int objc, 
              Tcl_Obj *const *objv)
{
    RText *textPtr = clientData; 
    int i;

    for (i = 3; i < objc; i++) {
        Image *imgPtr;
        
        if (GetImageFromObj(interp, textPtr, objv[i], &imgPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        if (imgPtr == NULL) {
            continue;
        }
	DeleteImage(textPtr, imgPtr);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ImageExistsOp --
 *
 *      Indicates if the given image exists in the text object. 
 *
 *      textName image exists imageName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ImageExistsOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    Image *imgPtr;
    RText *textPtr = clientData; 
    int state;

    state = FALSE;
    if (GetImageFromObj(NULL, textPtr, objv[3], &imgPtr) == TCL_OK) {
	state = TRUE;
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
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
 * ImageNamesOp --
 *
 *      Returns the names of all the images registered (used or unused) in
 *      the text object.  If one of more pattern arguments are provided,
 *      then only the images matching in those patterns are returned.
 *
 *      textName image names ?pattern ...?
 *
 *---------------------------------------------------------------------------
 */
static int
ImageNamesOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    RText *textPtr = clientData; 
    Tcl_Obj *listObjPtr, *objPtr;
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;

    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
    for (hPtr = Blt_FirstHashEntry(&textPtr->imageTable, &iter); hPtr != NULL;
	 hPtr = Blt_NextHashEntry(&iter)) {
	Image *imgPtr;
	int found;

	imgPtr = Blt_GetHashValue(hPtr);
	found = FALSE;
	if (objc == 3) {
	    found = TRUE;
	} else {
	    int i;

	    for (i = 3; i < objc; i++) {
		const char *pattern;

		pattern = Tcl_GetString(objv[i]);
		if (Tcl_StringMatch(imgPtr->name, pattern)) {
		    found = TRUE;
		    break;
		}
	    }
	}
	if (found) {
	    objPtr = Tcl_NewStringObj(imgPtr->name, -1);
	    Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
	}
    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ImageOp --
 *
 *      This procedure handles image operations for the rich text object.
 *
 * Results:
 *      A standard TCL result.
 *
 *	textName image op ...
 *
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec imgOps[] =
{
    {"cget",      2, ImageCgetOp,      5, 5, "imageName option",},
    {"configure", 2, ImageConfigureOp, 4, 0, "imageName ?option value ...?",},
    {"create",    2, ImageCreateOp,    4, 0, "imageName ?option value ...?",},
    {"delete",    1, ImageDeleteOp,    3, 0, "?imageName ...?",},
    {"insert",    1, ImageInsertOp,    4, 0, "imageName ?option value ...?",},
    {"exists",    1, ImageExistsOp,    4, 4, "imageName",},
    {"names",     1, ImageNamesOp,     3, 0, "?pattern ...?",},
};

static int numImgOps = sizeof(imgOps) / sizeof(Blt_OpSpec);

static int
ImageOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numImgOps, imgOps, BLT_OP_ARG2, 
           objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}

/*
 *---------------------------------------------------------------------------
 *
 * SpecialOp --
 *
 *      Inserts text into a text object at a given character
 *      position.  If the text represents a special character to UTF for
 *      that special character is inserted into the text.
 *
 *      textName special insert insertPos text ?tag text...?
 *
 *      textName special insert 0.0 @alpha tag " this is text" tag @beta
 *      textName special names
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
SpecialOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    RText *textPtr = clientData; 
    int i;

    if (GetByteOffset(interp, textPtr, objv[2], &insertPos) != TCL_OK) {
        return TCL_ERROR;
    }
    for (i = 3; i < objc; i += 2) {
        char buf[10];
        const char *string;
        int insertPos, nextPos;
        int ch;

        string = Tcl_GetStringFromObj(objv[i], &length);
        if (string[0] == '@') {
            unsigned int ch;

            if (GetSpecialUnichar(string+1, &ch)) {
                length = Tcl_UniCharToUtf(ch, buf);
                string = buf;
            } else {
                return TCL_ERROR;
            }
        }  
        nextPos = insertPos + length;
        AdjustTagRanges(textPtr, insertPos, length);
        AdjustImageOffsets(textPtr, insertPos, length);
        if ((i+1) < objc) {
            const char *tagName;
            int length;

            tagName = Tcl_GetStringFromObj(objv[i+1], &length);
            if (length > 0) {
                Tag *tagPtr;

                if (GetTag(interp, textPtr, tagName, &tagPtr) != TCL_OK) {
                    return TCL_ERROR;
                }
                AddTagRange(textPtr, tagPtr, insertPos, nextPos); 
            }
        }
        InsertText(textPtr, string, length, insertPos);
        insertPos = nextPos;
    }
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
    return (*proc)(clientData, interp, objc, objv);
}

static Blt_OpSpec textOps[] =
{
    {"cget",      2, CgetOp,           4, 4, "option",},
    {"configure", 2, ConfigureOp,      3, 0, "?option value ...?",},
    {"delete",    1, DeleteOp,         4, 4, "firstPos lastPos",},
    {"get",       1, GetOp,            2, 2, "",},
    {"image",     2, ImageOp,          3, 0, "args...",},
    {"insert",    2, InsertOp,         4, 0, "insertPos text ?tag text ...?",},
    {"symbol",    1, SymbolOp,         4, 0, "insertPos symbolName...",},
    {"tag",       1, TagOp,            3, 0, "args...",},
};

static int numTextOps = sizeof(textOps) / sizeof(Blt_OpSpec);


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

/*
 *---------------------------------------------------------------------------
 *
 * AddTextItem --
 *
 *      We're at the end of line because of a newline, line wrap, or
 *      attribute change. Append a new text item to the list of parsed
 *      text using the current set of attributes.  Compute the width of 
 *      the text substring.
 *
 *---------------------------------------------------------------------------
 */
static void
AddTextItem(RText *textPtr, const char *itemStart, int itemSize, 
            TextParser *parserPtr, TextAttributes *attrPtr)
{
    int flags;

    flags  = 0;
    if (attrPtr->flags & WRAP_WORDS) {
        flags = TK_WHOLE_WORDS;
    } 
    while (itemSize > 0) {
        int numBytes, numPixels, maxPixels;

        if (attrPtr->wrapLength > 0) {
            maxPixels = attrPtr->wrapLength - parserPtr->x;
        } else {
            maxPixels = -1;
        }
        numBytes = Blt_MeasureChars(attrPtr->fontPtr, itemStart, itemSize, 
                                    maxPixels, flags, &numPixels);
        itemPtr = NewTextItem(textPtr, parserPtr, itemStart, numBytes, attrPtr);
        itemPtr->height = fm.lineHeight;
        itemPtr->width = numPixels;
        itemPtr->x = parserPtr->x;
        itemPtr->y = parserPtr->y;
        itemPtr->baseline = -1;
        if (itemSize != numBytes) {
            parserPtr->y += GetLastLineHeight(itemPtr);
        }
        parserPtr->x += numPixels;
        itemSize -= numBytes;
        itemStart += numBytes;
    }
}

static void
ParseText(RText *textPtr, const char *string, int numBytes) 
{
    const char *firstPtr;
    int lineNum;

    parserPtr = NewParser(textPtr);
    parserPtr->itemStart = string;
    parserPtr->itemSize = numBytes;
    parserPtr->lastAttrPtr = GetCombinedAttributes(textPtr, parserPtr, 0);
    first = p = string;
    for (p = string; p < (string + numBytes); /*empty*/) {
        char c;

        size = Tcl_UtfToUniChar(p, &ch);
        p += size;
        c = (unsigned char)(ch & 0xff);
        if (c == '\n') {
            /* 
             * Newline encountered. Add preceding characters to a text item. 
             * Increment the y-coordinate by the height of the line.
             */
            if (parserPtr->itemSize > 0) {
                AddTextItem(textPtr, parserPtr->itemStart, parserPtr->itemSize, 
                            parserPtr->lastAttrPtr);
            }
            parserPtr->y += GetLineHeight(parserPtr);
            parserPtr->itemStart = p;
            parserPtr->itemSize -= 1;
            AdjustBaseline(parserPtr);
            continue;
        } else if (c == '\t') {
            /* Move the x-coordinate to the location of the next tabstop. */
            parserPtr->x = GetNextTabStop(parserPtr);
            continue;
        } else if (parserPtr->wrapLength > 0) {
            length = MeasureCurrentString(parserPtr);
            if (parserPtr->x > parserPtr->wrapLength) {
                /* We're beyond the current wrap length. Start measuring 
                 * the line character by character or word by word to see
                 * where the last break was. */
                p = GetLastBreak(parserPtr);
                if (p != NULL) {
                    AddTextItem(textPtr, first, size - first, parserPtr);
                    AdjustBaseline(parserPtr);
                    first = p;
                    size = p - first;
                    continue;
                }
            }
        }
        insertPos += size;
        currAttrPtr = GetCombinedAttributes(textPtr, parsePtr, insertPos);
        if (parserPtr->lastAttrPtr != currAttrPtr) {
            /* Add preceding text */
            AddTextItem(textPtr, first, size - 1, parserPtr);
            parserPtr->lastAttrPtr = currAttrPtr;
        }
    }
    /* Add any remaining text with the current attributes  */
    if (size > 0) {
        AddTextItem(textPtr, first, size - 1, parserPtr);
        AdjustBaseline(parserPtr);
    }
    /* Clean up actions:
     * 1) For each line adjust baseline 
     * */

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
