/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * bltRText.h --
 *
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

#ifndef _BLT_RTEXT_H
#define _BLT_RTEXT_H

/* 
 * 1. Replaces Blt_Ts_* and Blt_*Text* routines. Unifies TextLayout and
 *    TextStyle structures.
 * 2. Provides rich text via tex style strings and text-widget style tags.
 * 3. Can be used within blt::tk::button, blt::tk::label, treeview,
 *    tableview, graph, barchart, scale, combomenu, combobutton, listview,
 *    tabset widgets.
 * 
 * canvas label: text, rotation needed, starting text size, scale.
 * combobutton: text
 * combomenu: items
 *+graph, barchart, contour, axis, legend, markers: math, greeks, sub/sup, 
 *	rotation needed
 * listview
 *+scale: tick labels, greeks, sub/sup, rotation needed.
 * tableview column titles, cells: color and font.
 * tabset: tab labels, rotation needed.
 * treeview column titles, cells: color and font.
 * tk::button, tk::label, tk::radiobutton, tk::checkbutton
 *+tooltip: images, color, font, math, greeks
 */

/*
 * RTextTag --
 */
typedef struct {
    unsigned int flags;			/* UNDERLINE, OVERSTRIKE, WRAP */
    int width;                          /* Width of segment in pixels. This
                                         * information is used to draw
                                         * PostScript strings the same
                                         * width as X. (deprecated) */
    /* Text Attributes */
    XColor *fgColor;			/* Color to draw the text. */
    Blt_Bg bg;				/* If non-NULL, background color of
					 * text. */
    Blt_Font font;                      /* If non-NULL, font to use to draw
					 * text. Otherwise use global font. */
    int fontSize;			/* Font size delta. */
    int offset;
    int wrapLength;
    Tk_Anchor anchor;                   /* Indicates how the text box is
                                         * anchored around its x,y
                                         * coordinates. */
    Blt_Pad padX, padY;                 /* # pixels padding of around text
                                         * region. */
} Blt_RTextTag;

/*
 * RTextItem --
 */
typedef struct {
    unsigned short int x, y;		/* Offset of text item from anchor
					 * position of parent text
					 * object.  */
    unsigned short int sx, sy;		/* Starting offset of text using
                                         * rotated font. */
    unsigned short int width, height;   /* Size of text in pixels. */
    unsigned short int numChars;        /* # of UTF characters in string. */
    unsigned short int lineNum;		/* Line number of start of item. */
    unsigned short int charNum;	        /* Character index of start of
					 * item. */
    int numBytes;                       /* # of bytes in text. The actual
                                         * character count may differ
                                         * because of multi-byte UTF
                                         * encodings. */
    const char *text;                   /* Text string to be displayed */
    Blt_RTextTag tag;			/* If non-NULL, points to
					 * attributes (color, font, etc.)
					 * to use when drawing this text
					 * item. If NULL, we'll use the
					 * global text attributes. */
} Blt_RTextItem;
    
/*
 * RTextImage --
 */
typedef struct {
    Tk_Image tkImage;                   /* The Tk image being cached. */
    Blt_HashEntry *hashPtr;             /* Pointer to this entry in the
                                         * image hash table. */
    int refCount;                       /* Reference count for this
                                         * image. */
    short int width, height;            /* Dimensions of the cached
                                         * image. */
} Blt_RTextImage;

typedef struct {
    int type;				/* Type of fragment: IMAGE, TEXT, 
					 * or SPECIAL. */
    union {
	Blt_RTextImage *imgPtr;
	Blt_RTextItem *itemPtr;
    };
    struct _RTextFragment *nextPtr;
    short int x, y;                     /* X-Y offset of the baseline from
                                         * the upper-left corner of the
                                         * bbox. */
} RTextFragment;

/*
 * Blt_RText --
 */
typedef struct _Blt_RText {
    const char *text;
    int textLength;

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

    /* Text layout */
    TextFragment *underlinePtr;
    int width, height;                  /* Dimensions of text bounding
                                         * box */
    int numFragments;                   /* # fragments of text */
    TextFragment *headPtr;		/* List of text fragments */
    Blt_HashTable tagTable;
    Blt_HashTable imageTable;
} *Blt_RText;

BLT_EXTERN void Blt_RText_Draw(Blt_RText text, int x, int y);
BLT_EXTERN void Blt_RText_GetExtents(Blt_RText text, int *widthPtr, 
	int *heightPtr);
BLT_EXTERN Blt_RText Blt_RText_Alloc(Tcl_Interp *interp);
BLT_EXTERN void Blt_RText_Free(Blt_RText text);
BLT_EXTERN Blt_RText Blt_RText_Get(const char *textName);
BLT_EXTERN TkRegion Blt_RText_GetClipRegion(Blt_RText text);
BLT_EXTERN void Blt_RText_GetRegionOffset(Blt_RText, int *xPtr, int *yPtr);
BLT_EXTERN void Blt_RText_SetClipRegion(Blt_RText text, TkRegion rgn);
BLT_EXTERN void Blt_RText_SetRegionOffset(Blt_RText text, int x, int y);
BLT_EXTERN int Blt_RText_Get(Tcl_Interp *interp, Tcl_Obj *objPtr,
	Blt_RText *textPtr);
BLT_EXTERN void Blt_RText_SetJustify(Blt_RText text, int justify);
BLT_EXTERN void Blt_RText_SetTextFromObj(Blt_RText text, Tcl_Obj *objPtr);
BLT_EXTERN void Blt_RText_SetTextFromString(Blt_RText text, const char *text, 
	int textLength);
BLT_EXTERN void Blt_RText_ComputeLayout(Blt_RText text, int how);
BLT_EXTERN void Blt_RText_FreeLayout(Blt_RText text);
BLT_EXTERN int Blt_RText_CreateTag(Blt_RText text, const char *tagName);
BLT_EXTERN int Blt_RText_TagExists(Blt_RText text, const char *tagName);
BLT_EXTERN int Blt_RText_DeleteTag(Blt_RText text, const char *tagName);
BLT_EXTERN int Blt_RText_DeleteTag(Blt_RText text, const char *tagName);
Blt_RText_SetAnchor(textPtr, value);
Blt_RText_SetPad(textPtr, left, right, top, bottom);
Blt_RText_SetLeader(textPtr, value);
Blt_RText_SetMaxLength(textPtr, value);
Blt_RText_SetAngle(textPtr, value);
Blt_RText_SetBackground(textPtr, value);
Blt_RText_SetForeground(textPtr, value);
#Blt_RText_SetState(textPtr, value);
Blt_RText_GetPostScript(textPtr, x, y);

#endif /* _BLT_TEXT_H */
