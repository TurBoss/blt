/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * bltGrBar2.c --
 *
 * This module implements barchart elements for the BLT graph widget.
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

#define BUILD_BLT_TK_PROCS 1
#include "bltInt.h"

#ifdef HAVE_STRING_H
  #include <string.h>
#endif /* HAVE_STRING_H */

#include <X11/Xutil.h>
#include "bltMath.h"
#include "bltBind.h"
#include "bltPs.h"
#include "bltBg.h"
#include "bltPalette.h"
#include "bltPicture.h"
#include "bltGraph.h"
#include "bltGrAxis.h"
#include "bltGrLegd.h"
#include "bltGrElem.h"

#define CLAMP(x,l,h)    ((x) = (((x)<(l))? (l) : ((x)>(h)) ? (h) : (x)))

typedef struct {
    float x1, y1, x2, y2;
} BarRegion;

#define PLAYING(g,i) \
    ((!(g)->play.enabled) || (((i) >= (g)->play.t1) && ((i) <= (g)->play.t2)))

#define PointInBarSegment(s,x,y) \
        (((x) <= (int)((s)->x2 - 1)) && ((x) >= (int)(s)->x1) && \
         ((y) <= (int)((s)->y2 - 1)) && ((y) >= (int)(s)->y1))

/* Flags for trace's point and segments. */
#define VISIBLE         (1<<0)          /* Segment is (at least partially)
                                         * on visible on screen. */
#define CLIPPED         (1<<1)          /* Segment is clipped. */
#define INVERT          (1<<2)          /* Segment value is negative. */
#ifdef notdef
#define ACTIVE          (1<<6)          /* Segment is active. This is only
                                         * used when numActiveIndices is
                                         * greater than zero. */
#endif

typedef struct {
    const char *name;                   /* Pen style identifier.  If NULL,
                                         * pen was statically allocated. */
    ClassId classId;                    /* Type of pen */
    const char *typeId;                 /* String token identifying the
                                         * type of pen */
    unsigned int flags;                 /* Indicates if the pen element is
                                         * active or normal */
    int refCount;                       /* Reference count for elements
                                         * using this pen. */
    Blt_HashEntry *hashPtr;
    Blt_ConfigSpec *configSpecs;        /* Configuration specifications */
    PenConfigureProc *configProc;
    PenDestroyProc *destroyProc;
    Graph *graphPtr;                    /* Graph that the pen is associated
                                         * with. */

     /* Barchart-specific pen fields start here. */

    XColor *outlineColor;               /* Outline (foreground) color of
                                         * bar */
    Blt_Bg fillBg;                      /* 3D border and fill (background)
                                         * color */
    Blt_PaintBrush brush;               /* 3D border and fill (background)
                                         * color */
    double opacity;                     /* Opacity of fill background. */
    int borderWidth;                    /* 3D border width of bar */
    int relief;                         /* Relief of the bar */
    Pixmap stipple;                     /* Stipple */
    GC fillGC;                          /* Graphics context */
    GC outlineGC;                       /* Outline (foreground) color of
                                         * bar */

    int showValues;
    int showErrorBars;
    /* Error bar attributes. */
    int errorBarLineWidth;              /* Width of the error bar
                                         * segments. */
    int reqErrorBarCapWidth;
    XColor *errorBarColor;              /* Color of the error bar. */
    GC errorBarGC;                      /* Error bar graphics context. */
    /* Show value attributes. */
    const char *valueFormat;            /* A printf format string. */
    TextStyle valueStyle;               /* Text attributes (color, font,
                                         * rotation, etc.) of the value. */
    int errorBarCapWidth;
} BarPen;

typedef struct {
    Weight weight;                      /* Weight range where this pen is
                                         * valid. */
    BarPen *penPtr;                     /* Pen to use. */
} BarStyle;

typedef struct _BarSegment {
    struct _BarSegment *next;           /* Pointer to next bar. */
    float x1, y1, x2, y2;               /* Screen coordinates of the
                                         * rectangular segment (opposite
                                         * corners of the rectangle). x1,y1
                                         * is the upper left corner for
                                         * positive values and the lower
                                         * left coordinate for negative
                                         * values. */
    int index;                          /* Index of this coordinate
                                         * pointing back to the raw world
                                         * values in the individual data
                                         * arrays. */
    unsigned int flags;                 /* Flags associated with the
                                         * segment are described below. */
    BarPen *penPtr;                     /* Pen associated with this
                                         * segment. */
} BarSegment;

typedef struct {
    GraphObj obj;                       /* Must be first field in element. */
    unsigned int flags;         
    Blt_HashEntry *hashPtr;

    /* Fields specific to elements. */
    Blt_ChainLink link;                 /* Element's link in display
                                         * list. */
    const char *label;                  /* Label displayed in legend */
    unsigned short row, col;            /* Position of the entry in the
                                         * legend. */
    int legendRelief;                   /* Relief of label in legend. */
    Axis2d axes;                        /* X-axis and Y-axis mapping the
                                         * element */
    ElemValues x, y, w;                 /* Contains array of floating point
                                         * graph coordinate values. Also
                                         * holds min/max and the number of
                                         * coordinates */
    Blt_HashTable activeTable;          /* Table of indices which indicate
                                         * which data points are active
                                         * (drawn with "active" colors). */
    int numActiveIndices;               /* Number of active data points.
                                         * Special case: if
                                         * numActiveIndices < 0 and the
                                         * active bit is set in "flags",
                                         * then all data points are drawn
                                         * active. */
    ElementProcs *procsPtr;
    Blt_ConfigSpec *configSpecs;        /* Configuration specifications. */
    BarPen *activePenPtr;               /* Standard Pens */
    BarPen *normalPenPtr;
    BarPen *builtinPenPtr;
    Blt_Chain styles;                   /* Palette of pens. */

    /* Symbol scaling */
    int scaleSymbols;                   /* If non-zero, the symbols will
                                         * scale in size as the graph is
                                         * zoomed in/out.  */
    double xRange, yRange;              /* Initial X-axis and Y-axis
                                         * ranges: used to scale the size
                                         * of element's symbol. */
    int state;
    Blt_HashTable isoTable;             /* Table of isolines to be
                                         * displayed. */

    /* Barchart-specific fields. */
    int numActive;                      /* Number of active bar segments.
                                         * Special case: if
                                         * numActiveIndices < 0 and the
                                         * active bit is set in "flags",
                                         * then all data * points are drawn
                                         * active. */
    Blt_Painter painter;
    BarSegment *headPtr, *tailPtr;      /* Linked list of all bar segments
                                         * for the element. */
    int numSegments;                    /* # of bar segments in above
                                         * linked list. */
    int numVisibleSegments;             /* # of visible bar segments. */
    int numActiveSegments;              /* # of active bar segments. */
    Blt_Pool segPool;                   /* Memory pool for bar segments. */

    float barWidth;

    int padX;                           /* Spacing on either side of bar */
    ElemValues xError;                  /* Relative/symmetric X error
                                         * values. */
    ElemValues yError;                  /* Relative/symmetric Y error
                                         * values. */
    ElemValues xHigh, xLow;             /* Absolute/asymmetric X-coordinate
                                         * high/low error values. */
    ElemValues yHigh, yLow;             /* Absolute/asymmetric Y-coordinate
                                         * high/low error values. */
    BarPen builtinPen;

    int errorBarCapWidth;               /* Length of cap on error bars */
    Axis *zAxisPtr;
} BarElement;

BLT_EXTERN Blt_CustomOption bltAxisOption;
BLT_EXTERN Blt_CustomOption bltBarPenOption;
BLT_EXTERN Blt_CustomOption bltBarStylesOption;
BLT_EXTERN Blt_CustomOption bltElementTagsOption;
BLT_EXTERN Blt_CustomOption bltValuePairsOption;
BLT_EXTERN Blt_CustomOption bltValuesOption;
BLT_EXTERN Blt_CustomOption bltXAxisOption;
BLT_EXTERN Blt_CustomOption bltYAxisOption;

static Blt_OptionParseProc ObjToBarMode;
static Blt_OptionPrintProc BarModeToObj;
Blt_CustomOption bltBarModeOption = {
    ObjToBarMode, BarModeToObj, NULL, (ClientData)0
};

static Blt_OptionParseProc ObjToPenColors;
static Blt_OptionPrintProc PenColorsToObj;
static Blt_CustomOption penColorsOption = {
    ObjToPenColors, PenColorsToObj, NULL, (ClientData)0
};

static Blt_OptionFreeProc FreeBackground;
static Blt_OptionParseProc ObjToBackground;
static Blt_OptionPrintProc BackgroundToObj;
static Blt_CustomOption backgroundOption = {
    ObjToBackground, BackgroundToObj, FreeBackground, (ClientData)0
};

static Blt_OptionParseProc ObjToShow;
static Blt_OptionPrintProc ShowToObj;
Blt_CustomOption showOption = {
    ObjToShow, ShowToObj, NULL, (ClientData)0
};

#define DEF_ACTIVE_PEN          "activeBar"
#define DEF_AXIS_X              "x"
#define DEF_AXIS_Y              "y"
#define DEF_BORDERWIDTH         "1"
#define DEF_COLORMAP            (char *)NULL
#define DEF_ERRORBAR_LINE_WIDTH "1"
#define DEF_ERRORBAR_CAP_WIDTH  "1"
#define DEF_HIDE                "no"
#define DEF_LABEL_RELIEF        "flat"
#define DEF_NORMAL_STIPPLE      ""
#define DEF_RELIEF              "raised"
#define DEF_SHOW                "errorbars"
#define DEF_HIDEX               "values"
#define DEF_STATE               "normal"
#define DEF_STACK               (char *)NULL
#define DEF_STYLES              ""
#define DEF_TAGS                "all"
#define DEF_WIDTH               "0.0"

#define DEF_PEN_ACTIVE_COLOR            "red"
#define DEF_PEN_ACTIVE_ERRORBAR_COLOR   "red"
#define DEF_PEN_ACTIVE_FILL_COLOR       "red"
#define DEF_PEN_ACTIVE_OUTLINE_COLOR    "pink"
#define DEF_PEN_BORDERWIDTH             "1"
#define DEF_PEN_NORMAL_COLOR            "blue"
#define DEF_PEN_NORMAL_ERRORBAR_COLOR   "blue"
#define DEF_PEN_NORMAL_FILL_COLOR       "blue"
#define DEF_PEN_NORMAL_OUTLINE_COLOR    "navyblue"
#define DEF_PEN_RELIEF                  "raised"
#define DEF_PEN_SHOW_VALUES             "no"
#define DEF_PEN_STIPPLE                 ""
#define DEF_PEN_TYPE                    "bar"
#define DEF_PEN_VALUE_ANCHOR            "s"
#define DEF_PEN_VALUE_COLOR             RGB_BLACK
#define DEF_PEN_VALUE_FONT              STD_FONT_SMALL
#define DEF_PEN_VALUE_FORMAT            "%g"
#define DEF_PEN_ERRORBARS               (char *)NULL
#define DEF_PEN_VALUES                  (char *)NULL

static Blt_ConfigSpec penSpecs[] =
{
    {BLT_CONFIG_CUSTOM, "-color", "color", "Color", DEF_PEN_ACTIVE_COLOR, 
        0,  BLT_CONFIG_DONT_SET_DEFAULT | ACTIVE_PEN, &penColorsOption},
    {BLT_CONFIG_CUSTOM, "-color", "color", "Color", DEF_PEN_NORMAL_COLOR, 
        0, BLT_CONFIG_DONT_SET_DEFAULT | NORMAL_PEN,  &penColorsOption},
    {BLT_CONFIG_CUSTOM, "-background", "background", "Background",
        DEF_PEN_ACTIVE_FILL_COLOR, 0, BLT_CONFIG_NULL_OK | ACTIVE_PEN,
        &backgroundOption},
    {BLT_CONFIG_CUSTOM, "-background", "background", "Background",
        DEF_PEN_NORMAL_FILL_COLOR, 0, BLT_CONFIG_NULL_OK | NORMAL_PEN,
        &backgroundOption},
    {BLT_CONFIG_SYNONYM, "-bd", "borderWidth", (char *)NULL,
        (char *)NULL, 0, ALL_PENS},
    {BLT_CONFIG_SYNONYM, "-bg", "background", (char *)NULL,
        (char *)NULL, 0, ALL_PENS},
    {BLT_CONFIG_PIXELS_NNEG, "-borderwidth", "borderWidth", "BorderWidth",
        DEF_PEN_BORDERWIDTH, Blt_Offset(BarPen, borderWidth), ALL_PENS},
    {BLT_CONFIG_COLOR, "-errorbarcolor", "errorBarColor", "ErrorBarColor",
        DEF_PEN_ACTIVE_ERRORBAR_COLOR, Blt_Offset(BarPen, errorBarColor), 
        ACTIVE_PEN},
    {BLT_CONFIG_COLOR, "-errorbarcolor", "errorBarColor", "ErrorBarColor",
        DEF_PEN_NORMAL_ERRORBAR_COLOR, Blt_Offset(BarPen, errorBarColor), 
        NORMAL_PEN},
    {BLT_CONFIG_CUSTOM, "-errorbars", "errorBars", "ErrorBars", 
        DEF_PEN_ERRORBARS, Blt_Offset(BarPen, showErrorBars), 
        ALL_PENS | BLT_CONFIG_DONT_SET_DEFAULT, &showOption},
    {BLT_CONFIG_PIXELS_NNEG, "-errorbarwidth", "errorBarWidth","ErrorBarWidth",
        DEF_ERRORBAR_LINE_WIDTH, Blt_Offset(BarPen, errorBarLineWidth),
        ALL_PENS | BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_PIXELS_NNEG, "-errorbarcap", "errorBarCap", "ErrorBarCap", 
        DEF_ERRORBAR_CAP_WIDTH, Blt_Offset(BarPen, reqErrorBarCapWidth),
        ALL_PENS | BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_SYNONYM, "-fg", "foreground", (char *)NULL,
        (char *)NULL, 0, ALL_PENS},
    {BLT_CONFIG_SYNONYM, "-fill", "background", (char *)NULL,
        (char *)NULL, 0, ALL_PENS},
    {BLT_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
        DEF_PEN_ACTIVE_OUTLINE_COLOR, Blt_Offset(BarPen, outlineColor),
        ACTIVE_PEN | BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
        DEF_PEN_NORMAL_OUTLINE_COLOR, Blt_Offset(BarPen, outlineColor),
        NORMAL_PEN |  BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_SYNONYM, "-outline", "foreground", (char *)NULL,
        (char *)NULL, 0, ALL_PENS},
    {BLT_CONFIG_RELIEF, "-relief", "relief", "Relief",
        DEF_PEN_RELIEF, Blt_Offset(BarPen, relief), ALL_PENS},
    {BLT_CONFIG_BITMAP, "-stipple", "stipple", "Stipple", DEF_PEN_STIPPLE, 
        Blt_Offset(BarPen, stipple), ALL_PENS | BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_STRING, "-type", (char *)NULL, (char *)NULL, DEF_PEN_TYPE, 
        Blt_Offset(BarPen, typeId), ALL_PENS | BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_ANCHOR, "-valueanchor", "valueAnchor", "ValueAnchor",
        DEF_PEN_VALUE_ANCHOR, Blt_Offset(BarPen, valueStyle.anchor), 
        ALL_PENS},
    {BLT_CONFIG_COLOR, "-valuecolor", "valueColor", "ValueColor",
        DEF_PEN_VALUE_COLOR, Blt_Offset(BarPen, valueStyle.color), 
        ALL_PENS},
    {BLT_CONFIG_FONT, "-valuefont", "valueFont", "ValueFont",
        DEF_PEN_VALUE_FONT, Blt_Offset(BarPen, valueStyle.font), 
        ALL_PENS},
    {BLT_CONFIG_STRING, "-valueformat", "valueFormat", "ValueFormat",
        DEF_PEN_VALUE_FORMAT, Blt_Offset(BarPen, valueFormat),
        ALL_PENS | BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_FLOAT, "-valuerotate", "valueRotate", "ValueRotate",
        (char *)NULL, Blt_Offset(BarPen, valueStyle.angle), ALL_PENS},
    {BLT_CONFIG_CUSTOM, "-values", "values", "Values", DEF_PEN_VALUES, 
        Blt_Offset(BarPen, showValues), ALL_PENS | BLT_CONFIG_DONT_SET_DEFAULT,
        &showOption},
    {BLT_CONFIG_END, NULL, NULL, NULL, NULL, 0, 0}
};

static Blt_ConfigSpec barElemConfigSpecs[] = {
    {BLT_CONFIG_CUSTOM, "-color", "color", "Color", DEF_PEN_ACTIVE_COLOR, 
        Blt_Offset(BarElement, builtinPen), BLT_CONFIG_DONT_SET_DEFAULT, 
        &penColorsOption},
    {BLT_CONFIG_CUSTOM, "-activepen", "activePen", "ActivePen",
        DEF_ACTIVE_PEN, Blt_Offset(BarElement, activePenPtr), 
        BLT_CONFIG_NULL_OK, &bltBarPenOption},
    {BLT_CONFIG_CUSTOM, "-background", "background", "Background",
        DEF_PEN_NORMAL_FILL_COLOR, Blt_Offset(BarElement, builtinPen),
        BLT_CONFIG_NULL_OK, &backgroundOption},
    {BLT_CONFIG_FLOAT, "-barwidth", "barWidth", "BarWidth",
        DEF_WIDTH, Blt_Offset(BarElement, barWidth),
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_SYNONYM, "-bd", "borderWidth"},
    {BLT_CONFIG_SYNONYM, "-bg", "background"},
    {BLT_CONFIG_SYNONYM, "-bindtags", "tags"},
    {BLT_CONFIG_PIXELS_NNEG, "-borderwidth", "borderWidth", "BorderWidth",
        DEF_BORDERWIDTH, Blt_Offset(BarElement, builtinPen.borderWidth), 0},
    {BLT_CONFIG_COLOR, "-errorbarcolor", "errorBarColor", "ErrorBarColor",
        DEF_PEN_NORMAL_ERRORBAR_COLOR, 
        Blt_Offset(BarElement, builtinPen.errorBarColor), 0},
    {BLT_CONFIG_CUSTOM, "-errorbars", "errorBars", "ErrorBars", 
        DEF_PEN_ERRORBARS, Blt_Offset(BarElement, builtinPen.showErrorBars), 
        BLT_CONFIG_DONT_SET_DEFAULT, &showOption},
    {BLT_CONFIG_PIXELS_NNEG,"-errorbarwidth", "errorBarWidth", "ErrorBarWidth",
        DEF_ERRORBAR_LINE_WIDTH, 
        Blt_Offset(BarElement, builtinPen.errorBarLineWidth), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_PIXELS_NNEG, "-errorbarcap", "errorBarCap", "ErrorBarCap", 
        DEF_ERRORBAR_CAP_WIDTH, 
        Blt_Offset(BarElement, builtinPen.reqErrorBarCapWidth),
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_SYNONYM, "-fg", "foreground"},
    {BLT_CONFIG_CUSTOM, "-data", "data", "Data", (char *)NULL, 0, 0, 
        &bltValuePairsOption},
    {BLT_CONFIG_SYNONYM, "-fill", "background"},
    {BLT_CONFIG_COLOR, "-foreground", "foreground", "Foreground",
        DEF_PEN_NORMAL_OUTLINE_COLOR, 
        Blt_Offset(BarElement, builtinPen.outlineColor), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_STRING, "-label", "label", "Label", (char *)NULL, 
        Blt_Offset(BarElement, label), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_RELIEF, "-legendrelief", "legendRelief", "LegendRelief",
        DEF_LABEL_RELIEF, Blt_Offset(BarElement, legendRelief),
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_BITMASK, "-hide", "hide", "Hide", DEF_HIDE, 
         Blt_Offset(BarElement, flags), BLT_CONFIG_DONT_SET_DEFAULT,
        (Blt_CustomOption *)HIDDEN},
    {BLT_CONFIG_CUSTOM, "-mapx", "mapX", "MapX", DEF_AXIS_X, 
        Blt_Offset(BarElement, axes.x), 0, &bltXAxisOption},
    {BLT_CONFIG_CUSTOM, "-mapy", "mapY", "MapY", DEF_AXIS_Y, 
        Blt_Offset(BarElement, axes.y), 0, &bltYAxisOption},
    {BLT_CONFIG_SYNONYM, "-outline", "foreground"},
    {BLT_CONFIG_CUSTOM, "-colormap", "colormap", "Colormap", DEF_COLORMAP, 
        Blt_Offset(BarElement, zAxisPtr), 0, &bltAxisOption},
    {BLT_CONFIG_CUSTOM, "-pen", "pen", "Pen", (char *)NULL, 
        Blt_Offset(BarElement, normalPenPtr), BLT_CONFIG_NULL_OK, 
        &bltBarPenOption},
    {BLT_CONFIG_RELIEF, "-relief", "relief", "Relief",
        DEF_RELIEF, Blt_Offset(BarElement, builtinPen.relief), 0},
    {BLT_CONFIG_STATE, "-state", "state", "State", DEF_STATE, 
        Blt_Offset(BarElement, state), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_BITMAP, "-stipple", "stipple", "Stipple",
        DEF_NORMAL_STIPPLE, Blt_Offset(BarElement, builtinPen.stipple),
        BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_CUSTOM, "-styles", "styles", "Styles", DEF_STYLES, 
        Blt_Offset(BarElement, styles), 0, &bltBarStylesOption},
    {BLT_CONFIG_CUSTOM, "-tags", "tags", "Tags", DEF_TAGS, 0,
        BLT_CONFIG_NULL_OK, &bltElementTagsOption},
    {BLT_CONFIG_ANCHOR, "-valueanchor", "valueAnchor", "ValueAnchor",
        DEF_PEN_VALUE_ANCHOR, 
        Blt_Offset(BarElement, builtinPen.valueStyle.anchor), 0},
    {BLT_CONFIG_COLOR, "-valuecolor", "valueColor", "ValueColor",
        DEF_PEN_VALUE_COLOR, 
        Blt_Offset(BarElement, builtinPen.valueStyle.color), 0},
    {BLT_CONFIG_FONT, "-valuefont", "valueFont", "ValueFont",
        DEF_PEN_VALUE_FONT, 
        Blt_Offset(BarElement, builtinPen.valueStyle.font), 0},
    {BLT_CONFIG_STRING, "-valueformat", "valueFormat", "ValueFormat",
        DEF_PEN_VALUE_FORMAT, Blt_Offset(BarElement, builtinPen.valueFormat),
        BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_FLOAT, "-valuerotate", "valueRotate", "ValueRotate",
        (char *)NULL, Blt_Offset(BarElement, builtinPen.valueStyle.angle), 0},
    {BLT_CONFIG_CUSTOM, "-values", "values", "Values",  DEF_PEN_VALUES, 
        Blt_Offset(BarElement, builtinPen.showValues), 
        BLT_CONFIG_DONT_SET_DEFAULT, &showOption},
    {BLT_CONFIG_CUSTOM, "-weights", "weights", "Weights", (char *)NULL, 
        Blt_Offset(BarElement, w), 0, &bltValuesOption},
    {BLT_CONFIG_CUSTOM, "-x", "xdata", "Xdata", (char *)NULL, 
        Blt_Offset(BarElement, x), 0, &bltValuesOption},
    {BLT_CONFIG_CUSTOM, "-y", "ydata", "Ydata", (char *)NULL, 
        Blt_Offset(BarElement, y), 0, &bltValuesOption},
    {BLT_CONFIG_CUSTOM, "-xdata", "xdata", "Xdata", (char *)NULL, 
        Blt_Offset(BarElement, x), 0, &bltValuesOption},
    {BLT_CONFIG_CUSTOM, "-ydata", "ydata", "Ydata", (char *)NULL, 
        Blt_Offset(BarElement, y), 0, &bltValuesOption},
    {BLT_CONFIG_CUSTOM, "-xerror", "xError", "XError", (char *)NULL, 
        Blt_Offset(BarElement, xError), 0, &bltValuesOption},
    {BLT_CONFIG_CUSTOM, "-xhigh", "xHigh", "XHigh", (char *)NULL, 
        Blt_Offset(BarElement, xHigh), 0, &bltValuesOption},
    {BLT_CONFIG_CUSTOM, "-xlow", "xLow", "XLow", (char *)NULL, 
        Blt_Offset(BarElement, xLow), 0, &bltValuesOption},
    {BLT_CONFIG_CUSTOM, "-yerror", "yError", "YError", (char *)NULL, 
        Blt_Offset(BarElement, yError), 0, &bltValuesOption},
    {BLT_CONFIG_CUSTOM, "-yhigh", "yHigh", "YHigh", (char *)NULL, 
        Blt_Offset(BarElement, yHigh), 0, &bltValuesOption},
    {BLT_CONFIG_CUSTOM, "-ylow", "yLow", "YLow", (char *)NULL, 
        Blt_Offset(BarElement, yLow), 0, &bltValuesOption},
    {BLT_CONFIG_END, NULL, NULL, NULL, NULL, 0, 0}
};

/* Forward declarations */
static PenConfigureProc ConfigurePenProc;
static PenDestroyProc DestroyPenProc;
static ElementNearestProc NearestProc;
static ElementConfigProc ConfigureProc;
static ElementDestroyProc DestroyProc;
static ElementDrawProc DrawActiveProc;
static ElementDrawProc DrawNormalProc;
static ElementDrawSymbolProc DrawSymbolProc;
/* static ElementFindProc *FindProc; */ 
static ElementExtentsProc ExtentsProc;
static ElementToPostScriptProc ActiveToPostScriptProc;
static ElementToPostScriptProc NormalToPostScriptProc;
static ElementSymbolToPostScriptProc SymbolToPostScriptProc;
static ElementMapProc MapProc;

/*
 *---------------------------------------------------------------------------
 * Custom option parse and print procedures
 *---------------------------------------------------------------------------
 */

/*
 *---------------------------------------------------------------------------
 *
 * NameOfBarMode --
 *
 *      Converts the integer representing the mode style into a string.
 *
 *---------------------------------------------------------------------------
 */
static const char *
NameOfBarMode(BarMode mode)
{
    switch (mode) {
    case BARS_INFRONT:
        return "infront";
    case BARS_OVERLAP:
        return "overlap";
    case BARS_STACKED:
        return "stacked";
    case BARS_ALIGNED:
        return "aligned";
    default:
        return "unknown mode value";
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToBarMode --
 *
 *      Converts the mode string into its numeric representation.
 *
 *      Valid mode strings are:
 *
 *      "infront"   Draw a full bar at each point in the element.
 *
 *      "stacked"   Stack bar segments vertically. Each stack is defined
 *                  by each ordinate at a particular abscissa. The height
 *                  of each segment is represented by the sum the previous
 *                  ordinates.
 *
 *      "aligned"   Align bar segments as smaller slices one next to
 *                  the other.  Like "stacks", aligned segments are
 *                  defined by each ordinate at a particular abscissa.
 *
 * Results:
 *      A standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToBarMode(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
             Tcl_Obj *objPtr, char *widgRec, int offset, int flags)
{
    BarMode *modePtr = (BarMode *)(widgRec + offset);
    int length;
    char c;
    char *string;
    
    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if ((c == 'n') && (strncmp(string, "normal", length) == 0)) {
        *modePtr = BARS_INFRONT;
    } else if ((c == 'i') && (strncmp(string, "infront", length) == 0)) {
        *modePtr = BARS_INFRONT;
    } else if ((c == 's') && (strncmp(string, "stacked", length) == 0)) {
        *modePtr = BARS_STACKED;
    } else if ((c == 'a') && (strncmp(string, "aligned", length) == 0)) {
        *modePtr = BARS_ALIGNED;
    } else if ((c == 'o') && (strncmp(string, "overlap", length) == 0)) {
        *modePtr = BARS_OVERLAP;
    } else {
        Tcl_AppendResult(interp, "bad mode argument \"", string, "\": should"
                "be \"infront\", \"stacked\", \"overlap\", or \"aligned\"",
                (char *)NULL);
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * BarModeToObj --
 *
 *      Returns the mode style string based upon the mode flags.
 *
 * Results:
 *      The mode style string is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
BarModeToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
             char *widgRec, int offset, int flags)
{
    BarMode mode = *(BarMode *)(widgRec + offset);

    return Tcl_NewStringObj(NameOfBarMode(mode), -1);
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToPenColors --
 *
 *      Convert the string representation of a color into a XColor pointer
 *      and sets all the pen's colors to that color.
 *
 * Results:
 *      The return value is a standard TCL result.  The color pointer is
 *      written into the widget record.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToPenColors(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
               Tcl_Obj *objPtr, char *widgRec, int offset, int flags)
{
    BarPen *penPtr = (BarPen *)(widgRec + offset);
    XColor *colorPtr;
    Blt_Bg bg;
    
    colorPtr = Tk_AllocColorFromObj(interp, tkwin, objPtr);
    if (colorPtr == NULL) {
        return TCL_ERROR;
    }
    if (penPtr->outlineColor != NULL) {
        Tk_FreeColor(penPtr->outlineColor);
    }
    penPtr->outlineColor = colorPtr;
    colorPtr = Tk_AllocColorFromObj(interp, tkwin, objPtr);
    if (penPtr->errorBarColor != NULL) {
        Tk_FreeColor(penPtr->errorBarColor);
    }
    penPtr->errorBarColor = colorPtr;

    Blt_GetBgFromObj(interp, tkwin, objPtr, &bg);
    Tcl_ResetResult(interp);
    if (penPtr->fillBg != NULL) {
        Blt_Bg_Free(penPtr->fillBg);
    }
    penPtr->fillBg = bg;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * PenColorsToObj --
 *
 *      Convert the color value into a string.
 *
 * Results:
 *      The string representing the symbol color is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
PenColorsToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
               char *widgRec, int offset, int flags)
{
    BarPen *penPtr = (BarPen *)(widgRec + offset);

    return Tcl_NewStringObj(Tk_NameOfColor(penPtr->errorBarColor), -1);
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToShow --
 *
 * Results:
 *      The return value is a standard TCL result.  The color pointer is
 *      written into the widget record.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToShow(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
          Tcl_Obj *objPtr, char *widgRec, int offset, int flags)
{
    unsigned int *flagsPtr = (unsigned int *)(widgRec + offset);
    const char *string;
    int length;
    char c;

    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if ((c == 'x') && (length == 1)) {
        *flagsPtr = SHOW_X;
    } else if ((c == 'y') && (length == 1)) {
        *flagsPtr = SHOW_Y;
    } else if ((c == 'b') && (strncmp(string, "both", length) == 0)) {
        *flagsPtr = SHOW_BOTH;
    } else if ((c == 'n') && (strncmp(string, "none", length) == 0)) {
        *flagsPtr = SHOW_NONE;
    } else {
        Tcl_AppendResult(interp, "bad value \"", string, 
                         "\": should be x, y, both, or none", (char *)NULL);
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ShowToObj --
 *
 *      Convert the color value into a string.
 *
 * Results:
 *      The string representing the symbol color is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
ShowToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
               char *widgRec, int offset, int flags)
{
    unsigned int *flagsPtr = (unsigned int *)(widgRec + offset);
    Tcl_Obj *objPtr;

    if (*flagsPtr & SHOW_X) {
        if (*flagsPtr & SHOW_Y) {
            objPtr = Tcl_NewStringObj("both", 4);
        } else {
            objPtr = Tcl_NewStringObj("x", 1);
        }
    } else if (*flagsPtr & SHOW_Y) {
        objPtr = Tcl_NewStringObj("y", 1);
    } else {
        objPtr = Tcl_NewStringObj("none", 4);
    }
    return objPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * FreeBackground --
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static void
FreeBackground(ClientData clientData, Display *display, char *widgRec,
               int offset)
{
    BarPen *penPtr = (BarPen *)(widgRec + offset);

    if (penPtr->brush != NULL) {
        Blt_FreeBrush(penPtr->brush);
        penPtr->brush = NULL;
    }
    if (penPtr->fillBg != NULL) {
        Blt_Bg_Free(penPtr->fillBg);
        penPtr->fillBg = NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToBackground --
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToBackground(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
                Tcl_Obj *objPtr, char *widgRec, int offset, int flags)
{
    BarPen *penPtr = (BarPen *)(widgRec + offset);
    Blt_Bg bg;
    Blt_PaintBrush brush;
    const char *string;
    int length;

    string = Tcl_GetStringFromObj(objPtr, &length);
    /* Handle NULL string.  */
    if (length == 0) {
        FreeBackground(clientData, Tk_Display(tkwin), widgRec, offset);
        return TCL_OK;
    }
    /* First try as a background */
    if (Blt_GetBgFromObj(interp, tkwin, objPtr, &bg) == TCL_OK) {
        FreeBackground(clientData, Tk_Display(tkwin), widgRec, offset);
        penPtr->fillBg = bg;
    } else if (Blt_GetPaintBrushFromObj(interp, objPtr, &brush) == TCL_OK) {
        FreeBackground(clientData, Tk_Display(tkwin), widgRec, offset);
        penPtr->brush = brush;
    } else {
        Tcl_ResetResult(interp);
        Tcl_AppendResult(interp, "bad color argument \"", string,
                "\": should be a color name, background, or paintbrush",
                (char *)NULL);
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * BackgroundToObj --
 *
 *      Returns the mode style string based upon the mode flags.
 *
 * Results:
 *      The mode style string is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
BackgroundToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
                char *widgRec, int offset, int flags)
{
    BarPen *penPtr = (BarPen *)(widgRec + offset);
    const char *string;
    
    if (penPtr->fillBg != NULL) {
        string = Blt_Bg_Name(penPtr->fillBg);
    } else if (penPtr->brush != NULL) {
        string = Blt_GetBrushName(penPtr->brush);
    } else {
        string = "";
    }
    return Tcl_NewStringObj(string, -1);
}

static int
ConfigurePen(Graph *graphPtr, BarPen *penPtr)
{
    XGCValues gcValues;
    unsigned long gcMask;
    GC newGC;
    int screenNum;

    screenNum = Tk_ScreenNumber(graphPtr->tkwin);

    newGC = NULL;
    gcMask = GCForeground | GCBackground;
    gcValues.foreground = BlackPixel(graphPtr->display, screenNum);
    gcValues.background = WhitePixel(graphPtr->display, screenNum);
    if (((penPtr->fillBg != NULL) || (penPtr->outlineColor != NULL)) &&
        (penPtr->stipple != None)) {

	gcValues.fill_style = FillStippled;
        if (penPtr->fillBg != NULL) {
            gcValues.foreground = Blt_Bg_BorderColor(penPtr->fillBg)->pixel;
            if (penPtr->outlineColor != NULL) {
                gcValues.fill_style = FillOpaqueStippled;
            }
        } 
        if (penPtr->outlineColor != NULL) {
            gcValues.background = penPtr->outlineColor->pixel;
        }
        /* Handle old-style -stipple specially. */
        gcValues.stipple = penPtr->stipple;
        gcMask |= (GCStipple | GCFillStyle);
    }
    newGC = Tk_GetGC(graphPtr->tkwin, gcMask, &gcValues);
    if (penPtr->fillGC != NULL) {
        Tk_FreeGC(graphPtr->display, penPtr->fillGC);
    }
    penPtr->fillGC = newGC;

    if (penPtr->outlineColor != NULL) {
        gcValues.foreground = penPtr->outlineColor->pixel;
        gcValues.line_width = penPtr->borderWidth;
        gcMask = (GCForeground | GCLineWidth);
    }
    newGC = Tk_GetGC(graphPtr->tkwin, gcMask, &gcValues);
    if (penPtr->outlineGC != NULL) {
        Tk_FreeGC(graphPtr->display, penPtr->outlineGC);
    }
    penPtr->outlineGC = newGC;

    gcMask = GCLineWidth;
    gcValues.line_width = LineWidth(penPtr->errorBarLineWidth);
    if (penPtr->errorBarColor != NULL) {
        gcMask |= GCForeground;
        gcValues.foreground = penPtr->errorBarColor->pixel;
    }
    newGC = Tk_GetGC(graphPtr->tkwin, gcMask, &gcValues);
    if (penPtr->errorBarGC != NULL) {
        Tk_FreeGC(graphPtr->display, penPtr->errorBarGC);
    }
    penPtr->errorBarGC = newGC;
    return TCL_OK;
}

static void
DestroyPen(Graph *graphPtr, BarPen *penPtr)
{
    Blt_Ts_FreeStyle(graphPtr->display, &penPtr->valueStyle);
    if (penPtr->fillGC != NULL) {
        Tk_FreeGC(graphPtr->display, penPtr->fillGC);
    }
    if (penPtr->outlineGC != NULL) {
        Tk_FreeGC(graphPtr->display, penPtr->outlineGC);
    }
    if (penPtr->errorBarGC != NULL) {
        Tk_FreeGC(graphPtr->display, penPtr->errorBarGC);
    }
}

static int
ConfigurePenProc(Graph *graphPtr, Pen *basePtr)
{
    return ConfigurePen(graphPtr, (BarPen *)basePtr);
}

static void
DestroyPenProc(Graph *graphPtr, Pen *basePtr)
{
    DestroyPen(graphPtr, (BarPen *)basePtr);
}


static void
InitPen(BarPen *penPtr)
{
    /* Generic fields common to all pen types. */
    penPtr->configProc = ConfigurePenProc;
    penPtr->destroyProc = DestroyPenProc;
    penPtr->flags = NORMAL_PEN;
    penPtr->configSpecs = penSpecs;

    /* Initialize fields specific to bar pens. */
    Blt_Ts_InitStyle(penPtr->valueStyle);
    penPtr->relief = TK_RELIEF_RAISED;
    penPtr->showErrorBars = penPtr->showValues = 0;
    penPtr->borderWidth = 1;
}

Pen *
Blt_CreateBarPen(Graph *graphPtr, Blt_HashEntry *hPtr)
{
    BarPen *penPtr;

    penPtr = Blt_AssertCalloc(1, sizeof(BarPen));
    InitPen(penPtr);
    penPtr->opacity = 100.0;
    penPtr->name = Blt_GetHashKey(&graphPtr->penTable, hPtr);
    penPtr->graphPtr = graphPtr;
    penPtr->classId = CID_ELEM_BAR;
    penPtr->hashPtr = hPtr;
    if (strcmp(penPtr->name, "activeBar") == 0) {
        penPtr->flags = ACTIVE_PEN;
    }
    Blt_SetHashValue(hPtr, penPtr);
    return (Pen *)penPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * CheckStacks --
 *
 *      Check that the data limits are not superseded by the heights of
 *      stacked bar segments.  The heights are calculated by
 *      Blt_ComputeStacks.
 *
 * Results:
 *      If the y-axis limits need to be adjusted for stacked segments,
 *      *minPtr* or *maxPtr* are updated.
 *
 * Side effects:
 *      Autoscaling of the y-axis is affected.
 *
 *---------------------------------------------------------------------------
 */
static void
CheckStacks(Graph *graphPtr, Axis2d *pairPtr, double *minPtr, double *maxPtr)
{
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;
    
    if ((graphPtr->mode != BARS_STACKED) || (graphPtr->numBarGroups == 0)) {
        return;
    }
    for (hPtr = Blt_FirstHashEntry(&graphPtr->groupTable, &iter); 
         hPtr != NULL; hPtr = Blt_NextHashEntry(&iter)) {
        BarGroup *groupPtr;

        groupPtr = Blt_GetHashValue(hPtr);
        if ((groupPtr->axes.x == pairPtr->x) && 
            (groupPtr->axes.y == pairPtr->y)) {
            /*
             * Check if any of the y-values (because of stacking) are
             * greater than the current limits of the graph.
             */
            if (groupPtr->max < 0.0) {
                if (*minPtr > groupPtr->sum) {
                    *minPtr = groupPtr->sum;
                }
            } else {
                if (*maxPtr < groupPtr->sum) {
                    *maxPtr = groupPtr->sum;
                }
            }
        }
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ConfigureProc --
 *
 *      Sets up the appropriate configuration parameters in the GC.  It is
 *      assumed the parameters have been previously set by a call to
 *      Blt_ConfigureWidget.
 *
 * Results:
 *      The return value is a standard TCL result.  If TCL_ERROR is
 *      returned, then interp->result contains an error message.
 *
 * Side effects:
 *      Configuration information such as bar foreground/background color
 *      and stipple etc. get set in a new GC.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ConfigureProc(Graph *graphPtr, Element *basePtr)
{
    BarElement *elemPtr = (BarElement *)basePtr;
    Blt_ChainLink link;
    BarStyle *stylePtr;

    if (ConfigurePen(graphPtr, elemPtr->builtinPenPtr)!= TCL_OK) {
        return TCL_ERROR;
    }
    /*
     * Point to the static normal pen if no external pens have been
     * selected.
     */
    link = Blt_Chain_FirstLink(elemPtr->styles);
    if (link == NULL) {
        link = Blt_Chain_AllocLink(sizeof(BarStyle));
        Blt_Chain_LinkAfter(elemPtr->styles, link, NULL);
    }
    stylePtr = Blt_Chain_GetValue(link);
    stylePtr->penPtr = NORMALPEN(elemPtr);

    if (Blt_ConfigModified(elemPtr->configSpecs, "-barwidth", "-*data",
            "-map*", "-label", "-hide", "-x", "-y", (char *)NULL)) {
        elemPtr->flags |= MAP_ITEM;
        graphPtr->flags |= RESET_AXES;
        Blt_EventuallyRedrawGraph(graphPtr);
    }
    if (((elemPtr->xHigh.numValues > 0) && (elemPtr->xLow.numValues > 0)) ||
        (elemPtr->xError.numValues > 0)) {
        elemPtr->flags |= HAVE_XERROR;
    } else {
        elemPtr->flags &= ~HAVE_XERROR;
    }
    if (((elemPtr->yHigh.numValues > 0) && (elemPtr->yLow.numValues > 0)) ||
        (elemPtr->yError.numValues > 0)) {
        elemPtr->flags |= HAVE_YERROR;
    } else {
        elemPtr->flags &= ~HAVE_YERROR;
    }
    return TCL_OK;
}

static void
ExtentsProc(Element *basePtr)
{
    BarElement *elemPtr = (BarElement *)basePtr;
    Graph *graphPtr;
    double barWidth, middle;
    int numPoints;
    Region2d exts;

    graphPtr = elemPtr->obj.graphPtr;
    exts.top = exts.left = DBL_MAX;
    exts.bottom = exts.right = -DBL_MAX;

    numPoints = NUMBEROFPOINTS(elemPtr);
    if (numPoints < 1) {
        return;                         /* No data points */
    }
    barWidth = (elemPtr->barWidth > 0.0f) 
        ? elemPtr->barWidth : graphPtr->barWidth;

    middle = barWidth * 0.5;
    exts.left = elemPtr->x.min - middle;
    exts.right = elemPtr->x.max + middle;

    exts.top = elemPtr->y.min;
    exts.bottom = elemPtr->y.max;
    if (exts.bottom < graphPtr->baseline) {
        exts.bottom = graphPtr->baseline;
    }
    /*
     * Handle stacked bar elements specially.
     *
     * If element is stacked, the sum of its ordinates may be outside the
     * minimum/maximum limits of the element's data points.
     */
    if ((graphPtr->mode == BARS_STACKED) && (graphPtr->numBarGroups > 0)) {
        CheckStacks(graphPtr, &elemPtr->axes, &exts.top, &exts.bottom);
    }
    /* Warning: You get what you deserve if the x-axis is logScale */
    if (IsLogScale(elemPtr->axes.x)) {
        exts.left = Blt_FindElemValuesMinimum(&elemPtr->x, DBL_MIN) + 
            middle;
    }
    /* Fix y-min limits for barchart */
    if (IsLogScale(elemPtr->axes.y)) {
        if ((exts.top <= 0.0) || (exts.top > 1.0)) {
            exts.top = 1.0;
        }
    } else {
        if (exts.top > 0.0) {
            exts.top = 0.0;
        }
    }

    /* Correct the extents for error bars if they exist. */
    if (elemPtr->xError.numValues > 0) {
        int i;
        
        /* Correct the data limits for error bars */
        numPoints = MIN(elemPtr->xError.numValues, numPoints);
        for (i = 0; i < numPoints; i++) {
            double x;

            x = elemPtr->x.values[i] + elemPtr->xError.values[i];
            if (x > exts.right) {
                exts.right = x;
            }
            x = elemPtr->x.values[i] - elemPtr->xError.values[i];
            if (IsLogScale(elemPtr->axes.x)) {
                if (x < 0.0) {
                    x = -x;             /* Mirror negative values, instead
                                         * of ignoring them. */
                }
                if ((x > DBL_MIN) && (x < exts.left)) {
                    exts.left = x;
                }
            } else if (x < exts.left) {
                exts.left = x;
            }
        }                    
    } else {
        if ((elemPtr->xHigh.numValues > 0) && 
            (elemPtr->xHigh.max > exts.right)) {
            exts.right = elemPtr->xHigh.max;
        }
        if (elemPtr->xLow.numValues > 0) {
            double left;
            
            if ((elemPtr->xLow.min <= 0.0) && (IsLogScale(elemPtr->axes.x))) {
                left = Blt_FindElemValuesMinimum(&elemPtr->xLow, DBL_MIN);
            } else {
                left = elemPtr->xLow.min;
            }
            if (left < exts.left) {
                exts.left = left;
            }
        }
    }
    if (elemPtr->yError.numValues > 0) {
        int i;
        
        numPoints = MIN(elemPtr->yError.numValues, numPoints);
        for (i = 0; i < numPoints; i++) {
            double y;

            y = elemPtr->y.values[i] + elemPtr->yError.values[i];
            if (y > exts.bottom) {
                exts.bottom = y;
            }
            y = elemPtr->y.values[i] - elemPtr->yError.values[i];
            if (IsLogScale(elemPtr->axes.y)) {
                if (y < 0.0) {
                    y = -y;             /* Mirror negative values, instead
                                         * of ignoring them. */
                }
                if ((y > DBL_MIN) && (y < exts.left)) {
                    exts.top = y;
                }
            } else if (y < exts.top) {
                exts.top = y;
            }
        }                    
    } else {
        if ((elemPtr->yHigh.numValues > 0) && 
            (elemPtr->yHigh.max > exts.bottom)) {
            exts.bottom = elemPtr->yHigh.max;
        }
        if (elemPtr->yLow.numValues > 0) {
            double top;
            
            if ((elemPtr->yLow.min <= 0.0) && (IsLogScale(elemPtr->axes.y))) {
                top = Blt_FindElemValuesMinimum(&elemPtr->yLow, DBL_MIN);
            } else {
                top = elemPtr->yLow.min;
            }
            if (top < exts.top) {
                exts.top = top;
            }
        }
    }
    if (elemPtr->axes.x->dataRange.min > exts.left) {
        elemPtr->axes.x->dataRange.min = exts.left;
    }
    if (elemPtr->axes.x->dataRange.max < exts.right) {
        elemPtr->axes.x->dataRange.max = exts.right;
    }
    if (elemPtr->axes.y->dataRange.min > exts.top) {
        elemPtr->axes.y->dataRange.min = exts.top;
    }
    if (elemPtr->axes.y->dataRange.max < exts.bottom) {
        elemPtr->axes.y->dataRange.max = exts.bottom;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * NearestProc --
 *
 *      Find the bar segment nearest to the window coordinates point
 *      specified.
 *
 *      Note:  This does not return the height of the stacked segment
 *             (in graph coordinates) properly.
 *
 * Results:
 *      Returns 1 if the point is width any bar segment, otherwise 0.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static void
NearestProc(
    Graph *graphPtr,                    /* Not used. */
    Element *basePtr,                   /* Bar element */
    NearestElement *nearestPtr)         /* Information about nearest point
                                         * in element */
{
    BarElement *elemPtr = (BarElement *)basePtr;
    BarSegment *segPtr;

    for (segPtr = elemPtr->headPtr; segPtr != NULL; segPtr = segPtr->next) {
        Point2d *pp, *pend;
        Point2d outline[5];
        double left, right, top, bottom;

        if (PointInBarSegment(segPtr, nearestPtr->x, nearestPtr->y)) {
            nearestPtr->index = segPtr->index;
            nearestPtr->distance = 0.0;
            nearestPtr->item = elemPtr;
            nearestPtr->point.x = elemPtr->x.values[segPtr->index];
            nearestPtr->point.y = elemPtr->y.values[segPtr->index];
            break;
        }
        left = segPtr->x1, top = segPtr->y1;
        right = (double)(segPtr->x2 - 1);
        bottom = (double)(segPtr->y2 - 1);
        outline[4].x = outline[3].x = outline[0].x = left;
        outline[4].y = outline[1].y = outline[0].y = top;
        outline[2].x = outline[1].x = right;
        outline[3].y = outline[2].y = bottom;

        for (pp = outline, pend = outline + 4; pp < pend; pp++) {
            Point2d t;
            double d;

            t = Blt_GetProjection(nearestPtr->x, nearestPtr->y, pp, pp + 1);
            if (t.x > right) {
                t.x = right;
            } else if (t.x < left) {
                t.x = left;
            }
            if (t.y > bottom) {
                t.y = bottom;
            } else if (t.y < top) {
                t.y = top;
            }
            d = hypot((t.x - nearestPtr->x), (t.y - nearestPtr->y));
            if (d < nearestPtr->distance) {
                nearestPtr->item = elemPtr;
                nearestPtr->distance = d;
                nearestPtr->index = segPtr->index;
                nearestPtr->point.x = elemPtr->x.values[segPtr->index];
                nearestPtr->point.y = elemPtr->y.values[segPtr->index];
            }
        }
    }
}


static void
ResetElement(BarElement *elemPtr)
{
    if (elemPtr->segPool != NULL) {
        Blt_Pool_Destroy(elemPtr->segPool);
        elemPtr->segPool = NULL;
    }
    elemPtr->headPtr = elemPtr->tailPtr = NULL;
    elemPtr->segPool = Blt_Pool_Create(BLT_FIXED_SIZE_ITEMS);
    elemPtr->numSegments = elemPtr->numActive = 0;
    elemPtr->numActive = 0;
}

/*
 *---------------------------------------------------------------------------
 *
 * BoxesDontOverlap --
 *
 *      Tests if the bounding box of a marker overlaps the plotting area in
 *      any way.  If so, the marker will be drawn.  Just do a min/max test
 *      on the extents of both boxes.
 *
 *      Note: It's assumed that the extents of the bounding box lie within
 *            the area.  So for a 10x10 rectangle, bottom and left would be
 *            9.
 *
 * Results:
 *      Returns 0 is the marker is visible in the plotting area, and 1
 *      otherwise (marker is off screen).
 *
 *---------------------------------------------------------------------------
 */
static INLINE int
BoxesDontOverlap(Graph *graphPtr, double x1, double y1, double x2, double y2)
{
    return (((double)graphPtr->x2 < x1) || ((double)graphPtr->y2 < y1) ||
            (x2 < (double)graphPtr->x1) || (y2 < (double)graphPtr->y1));
}

static BarPen *
WeightToPen(BarElement *elemPtr, double weight)
{
    Blt_ChainLink link;

    for (link = Blt_Chain_LastLink(elemPtr->styles); link != NULL;  
         link = Blt_Chain_PrevLink(link)) {
        BarStyle *stylePtr;
        
        stylePtr = Blt_Chain_GetValue(link);
        if (stylePtr->weight.range > 0.0) {
            double norm;
            
            norm = (weight - stylePtr->weight.min) / stylePtr->weight.range;
            if (((norm - 1.0) <= DBL_EPSILON) && 
                (((1.0 - norm) - 1.0) <= DBL_EPSILON)) {
                return stylePtr->penPtr;
            }
        }
    }
    return NORMALPEN(elemPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * GraphExtents --
 *
 *      Generates a bounding box representing the plotting area of the
 *      graph. This data structure is used to clip the points and line
 *      segments of the line element.
 *
 *      The clip region is the plotting area plus such arbitrary extra
 *      space.  The reason we clip with a bounding box larger than the plot
 *      area is so that symbols will be drawn even if their center point
 *      isn't in the plotting area.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      The bounding box is filled with the dimensions of the plotting
 *      area.
 *
 *---------------------------------------------------------------------------
 */
static void
GraphExtents(Graph *graphPtr, Region2d *regionPtr)
{
    regionPtr->left = (double)(graphPtr->hOffset - graphPtr->padX.side1);
    regionPtr->top = (double)(graphPtr->vOffset - graphPtr->padY.side1);
    regionPtr->right = (double)(graphPtr->hOffset + graphPtr->hRange + 
        graphPtr->padX.side2);
    regionPtr->bottom = (double)(graphPtr->vOffset + graphPtr->vRange + 
        graphPtr->padY.side2);
}

/*
 *---------------------------------------------------------------------------
 *
 * NewSegment --
 *
 *      Creates a new bar segment.
 *
 * Results:
 *      Returns a pointer to the new bar segment.
 *
 *---------------------------------------------------------------------------
 */
static INLINE BarSegment *
NewSegment(BarElement *elemPtr, Point2d *p, Point2d *q, int index, int flags)
{
    BarSegment *segPtr;
    Region2d exts;
    int clipped;
    Blt_HashEntry *hPtr;

    segPtr = Blt_Pool_AllocItem(elemPtr->segPool, sizeof(BarSegment));
    if (elemPtr->headPtr == NULL) {
        elemPtr->headPtr = segPtr;
    }
    if (elemPtr->tailPtr == NULL) {
        elemPtr->tailPtr = segPtr;
    } else {
        elemPtr->tailPtr->next = segPtr;
        elemPtr->tailPtr = segPtr;
    }
    segPtr->next = NULL;
    segPtr->flags = 0;
    segPtr->penPtr = NORMALPEN(elemPtr);
    if (p->x < q->x) {
        segPtr->x1 = p->x;
        segPtr->x2 = q->x;
    } else {
        segPtr->x1 = q->x;
        segPtr->x2 = p->x;
    }
    if (p->y < q->y) {
        segPtr->y1 = p->y;
        segPtr->y2 = q->y;
    } else {
        segPtr->y1 = q->y;
        segPtr->y2 = p->y;
    }
    if (flags) {
        segPtr->flags |= INVERT;
    }
    segPtr->index = index;                  /* Save the data index
                                         * corresponding to the
                                         * rectangle */

    elemPtr->numSegments++;             /* All bar segments. */
    GraphExtents(elemPtr->obj.graphPtr, &exts);
    clipped = BoxesDontOverlap(elemPtr->obj.graphPtr, segPtr->x1, segPtr->y1, 
                               segPtr->x2, segPtr->y2);
    if (!clipped) {
        segPtr->flags |= VISIBLE;
        elemPtr->numVisibleSegments++;  /* Visible segments. */
    }
    hPtr = Blt_FindHashEntry(&elemPtr->activeTable, (char *)(intptr_t)index);
    if (hPtr != NULL) {
        segPtr->flags |= ACTIVE;
        elemPtr->numActiveSegments++;   /* Active bar segments. */
    }
    segPtr->index = index;
    if (elemPtr->w.numValues > segPtr->index) {
        segPtr->penPtr = WeightToPen(elemPtr, elemPtr->w.values[segPtr->index]);
    } else {
        segPtr->penPtr = NORMALPEN(elemPtr);
    }
    return segPtr;
}


/*
 *---------------------------------------------------------------------------
 *
 * MapStyles --
 *
 *      Splits traces based on the pen used.  May create many more traces
 *      if the traces change pens frequently.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      New traces may be created.  Traces may be split.
 *
 *---------------------------------------------------------------------------
 */
static void
MapStyles(BarElement *elemPtr)
{
    BarSegment *segPtr;

    for (segPtr = elemPtr->headPtr; segPtr != NULL; segPtr = segPtr->next) {
        BarPen *penPtr;

        if (elemPtr->w.numValues > segPtr->index) {
            penPtr = WeightToPen(elemPtr, elemPtr->w.values[segPtr->index]);
        } else {
            penPtr = NORMALPEN(elemPtr);
        }
        segPtr->penPtr = penPtr;
    }
}


/*
 *---------------------------------------------------------------------------
 *
 * MapProc --
 *
 *      Calculates the actual window coordinates of the bar element.  The
 *      window coordinates are saved in the bar element structure.
 *
 * Results:
 *      None.
 *
 * Notes:
 *      A bar can have multiple segments (more than one x,y pairs).  In
 *      this case, the bar can be represented as either a set of
 *      non-contiguous bars or a single multi-segmented (stacked) bar.
 *
 *      The x-axis layout for a barchart may be presented in one of two
 *      ways.  If abscissas are used, the bars are placed at those
 *      coordinates.  Otherwise, the range will represent the number of
 *      values.
 *
 *---------------------------------------------------------------------------
 */
static void
MapProc(Graph *graphPtr, Element *basePtr)
{
    BarElement *elemPtr = (BarElement *)basePtr;
    double *x, *y;
    double barWidth, barOffset;
    double baseline, ybot;
    int invertBar;
    int numPoints;
    int i;
    int size;

    ResetElement(elemPtr);
    numPoints = NUMBEROFPOINTS(elemPtr);
    if (numPoints < 1) {
        return;                         /* No data points. */
    }
    barWidth = (elemPtr->barWidth > 0.0) 
        ? elemPtr->barWidth : graphPtr->barWidth;
    baseline = (IsLogScale(elemPtr->axes.y)) ? 1.0 : graphPtr->baseline;
    barOffset = (IsTimeScale(elemPtr->axes.x)) ? 0.0 : barWidth * 0.5;

    x = elemPtr->x.values, y = elemPtr->y.values;
    for (i = 0; i < numPoints; i++) {
        Point2d c1, c2;                 /* Two opposite corners of the
                                         * rectangle in graph
                                         * coordinates. */
        double dx, dy;
        double right, left, top, bottom;

        if (((x[i] - barWidth) > elemPtr->axes.x->tickRange.max) ||
            ((x[i] + barWidth) < elemPtr->axes.x->tickRange.min)) {
            continue;                   /* Abscissa is out of range of the
                                         * x-axis. */
        }

        c1.x = x[i] - barOffset;
        c1.y = y[i];
        c2.x = c1.x + barWidth;
        c2.y = baseline;

        /*
         * If the mode is "aligned" or "stacked" we need to adjust the x or
         * y coordinates of the two corners.
         */

        if ((graphPtr->numBarGroups > 0) && (graphPtr->mode != BARS_INFRONT) && 
            ((graphPtr->flags & STACK_AXES) == 0)) {
            Blt_HashEntry *hPtr;
            BarGroupKey key;

            memset(&key, 0, sizeof(key));
            key.value = (float)x[i];
            key.axes = elemPtr->axes;
            key.axes.y = NULL;
            hPtr = Blt_FindHashEntry(&graphPtr->groupTable, (char *)&key);
            if (hPtr != NULL) {
                BarGroup *groupPtr;
                double slice, width, offset;
                
                groupPtr = Blt_GetHashValue(hPtr);
                slice = barWidth / (double)graphPtr->maxBarGroupSize;
                offset = (slice * groupPtr->count);
                if (graphPtr->maxBarGroupSize > 1) {
                    offset += slice * 0.05;
                    slice *= 0.90;
                }
                switch (graphPtr->mode) {
                case BARS_STACKED:
                    /* Draw segments bottom-to-top */
                    groupPtr->count++;
                    c2.y = groupPtr->lastY; /* Raise the bottom */
                    c1.y += c2.y;           /* Raise the top. */
                    groupPtr->lastY = c1.y; /* Last y is new top. */
                    break;
                        
                case BARS_ALIGNED:
                    /* Order segments left-to-right, bottom-to-top */
                    c1.x = x[i] - barOffset + (groupPtr->count * slice);
                    c2.x = c1.x + slice;
                    groupPtr->count++;
                    break;
                        
                case BARS_OVERLAP:
                    /* Order segments left-to-right, bottom-to-top */
                    offset = (slice * groupPtr->count);
                    width = slice + slice;
                    groupPtr->count++;
                    c1.x += offset;
                    c2.x = c1.x + width;
                    break;
                        
                case BARS_INFRONT:
                    break;
                }
            }
        }
        invertBar = FALSE;
        if (c1.y < c2.y) {
            double temp;

            /* Handle negative bar values by swapping ordinates */
            temp = c1.y, c1.y = c2.y, c2.y = temp;
            invertBar = TRUE;
        }
        /*
         * Get the two corners of the bar segment and compute the rectangle.
         */
        ybot = c2.y;
        c1 = Blt_Map2D(graphPtr, c1.x, c1.y, &elemPtr->axes);
        c2 = Blt_Map2D(graphPtr, c2.x, c2.y, &elemPtr->axes);
        if ((ybot == 0.0) && (IsLogScale(elemPtr->axes.y))) {
            c2.y = graphPtr->y2;
        }
            
        if (c2.y < c1.y) {
            double t;
            t = c1.y, c1.y = c2.y, c2.y = t;
        }
        if (c2.x < c1.x) {
            double t;
            t = c1.x, c1.x = c2.x, c2.x = t;
        }
        if ((c1.x > graphPtr->x2) || (c2.x < graphPtr->x1) || 
            (c1.y > graphPtr->y2) || (c2.y < graphPtr->y1)) {
            continue;
        }
        /* Bound the bars horizontally by the width of the graph window */
        /* Bound the bars vertically by the position of the axis. */
        if (graphPtr->flags & STACK_AXES) {
            top = elemPtr->axes.y->screenMin;
            bottom = elemPtr->axes.y->screenMin + elemPtr->axes.y->screenRange;
            left = graphPtr->x1;
            right = graphPtr->x2;
        } else {
            left = top = 0;
            bottom = right = 10000;
            /* Shouldn't really have a call to Tk_Width or Tk_Height in
             * mapping routine.  We only want to clamp the bar segment to
             * the size of the window if we're actually mapped onscreen. */
            if (Tk_Height(graphPtr->tkwin) > 1) {
                bottom = Tk_Height(graphPtr->tkwin);
            }
            if (Tk_Width(graphPtr->tkwin) > 1) {
                right = Tk_Width(graphPtr->tkwin);
            }
        }
        CLAMP(c1.y, top, bottom);
        CLAMP(c2.y, top, bottom);
        CLAMP(c1.x, left, right);
        CLAMP(c2.x, left, right);
        dx = FABS(c1.x - c2.x);
        dy = FABS(c1.y - c2.y);
        if ((dx == 0) || (dy == 0)) {
            continue;
        }
        NewSegment(elemPtr, &c1, &c2, i, invertBar);
    }
    size = 20;
    if (elemPtr->headPtr != NULL) {
        size = elemPtr->headPtr->x2 - elemPtr->headPtr->x1;
    }
    {
        Blt_ChainLink link;

        /* Set the symbol size of all the pen styles. */
        for (link = Blt_Chain_FirstLink(elemPtr->styles); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            BarStyle *stylePtr;
            BarPen *penPtr;

            stylePtr = Blt_Chain_GetValue(link);
            penPtr = stylePtr->penPtr;
            penPtr->errorBarCapWidth = 
                (penPtr->reqErrorBarCapWidth > 0) 
                ? penPtr->reqErrorBarCapWidth : (size * 66666) / 100000;
            penPtr->errorBarCapWidth /= 2;
        }
    }
}


/*
 *---------------------------------------------------------------------------
 *
 * DrawOutline --
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static void
DrawOutline(Graph *graphPtr, Drawable drawable, BarElement *elemPtr, 
            BarPen *penPtr, float x1, float y1, float x2, float y2)
{
    int numSegments;
    XSegment xSegments[4];
    Region2d reg;
    Point2d p, q;
    float bw2;

    bw2 = penPtr->borderWidth * 0.5;
    numSegments = 0;
    GraphExtents(graphPtr, &reg);

    /* Top line */
    p.x = x1, p.y = y1;
    q.x = x2, q.y = y1;
    if (Blt_LineRectClip(&reg, &p, &q)) {
        xSegments[numSegments].x1 = (int)p.x;
        xSegments[numSegments].x2 = (int)q.x;
        xSegments[numSegments].y1 = (int)p.y;
        xSegments[numSegments].y2 = (int)q.y;
        numSegments++;
    }
    /* Left line */
    p.x = x1, p.y = y1;
    q.x = x1, q.y = y2;
    if (Blt_LineRectClip(&reg, &p, &q)) {
        xSegments[numSegments].x1 = (int)p.x;
        xSegments[numSegments].x2 = (int)q.x;
        xSegments[numSegments].y1 = (int)p.y;
        xSegments[numSegments].y2 = (int)q.y;
        numSegments++;
    }
    /* Right line */
    p.x = x2, p.y = y1;
    q.x = x2, q.y = y2;
    if (Blt_LineRectClip(&reg, &p, &q)) {
        xSegments[numSegments].x1 = (int)p.x;
        xSegments[numSegments].x2 = (int)q.x;
        xSegments[numSegments].y1 = (int)p.y;
        xSegments[numSegments].y2 = (int)q.y;
        numSegments++;
    }
    /* Bottom line */
    p.x = x1, p.y = y2;
    q.x = x2, q.y = y2;
    if (Blt_LineRectClip(&reg, &p, &q)) {
        xSegments[numSegments].x1 = (int)p.x;
        xSegments[numSegments].x2 = (int)q.x;
        xSegments[numSegments].y1 = (int)p.y;
        xSegments[numSegments].y2 = (int)q.y;
        numSegments++;
    }
    XDrawSegments(graphPtr->display, drawable, penPtr->outlineGC, xSegments, 
        numSegments);
}

/*
 *---------------------------------------------------------------------------
 *
 * DrawSymbolProc --
 *
 *      Draw a symbol centered at the given x,y window coordinate based
 *      upon the element symbol type and size.
 *
 * Results:
 *      None.
 *
 * Problems:
 *      Most notable is the round-off errors generated when calculating the
 *      centered position of the symbol.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static void
DrawSymbolProc(Graph *graphPtr, Drawable drawable, Element *basePtr, 
               int x, int y, int size)
{
    BarElement *elemPtr = (BarElement *)basePtr;
    BarPen *penPtr;
    int radius;

    penPtr = NORMALPEN(elemPtr);
    if ((penPtr->fillBg == NULL) && (penPtr->outlineColor == NULL)) {
        return;
    }
    radius = (size / 2);
    size--;

    x -= radius;
    y -= radius;
    if (penPtr->fillBg != NULL) {
        XSetTSOrigin(graphPtr->display, penPtr->fillGC, x, y);
        if (penPtr->stipple != None) {
            XFillRectangle(graphPtr->display, drawable, penPtr->fillGC, x, y, 
                           size, size);
        } else {
            Blt_Bg_FillRectangle(graphPtr->tkwin, drawable, penPtr->fillBg, 
                x, y, size, size, 0, TK_RELIEF_FLAT);
        }
        XSetTSOrigin(graphPtr->display, penPtr->fillGC, 0, 0);
    }
    if ((penPtr->outlineColor != NULL) && (penPtr->borderWidth > 0)) {
         XDrawRectangle(graphPtr->display, drawable, penPtr->outlineGC, 
                       x, y, size - 1, size - 1);
    }
}

static int
GradientCalcProc(ClientData clientData, int x, int y, double *valuePtr)
{
    BarElement *elemPtr = clientData;
    Graph *graphPtr;
    Point2d point;
    double value;
    AxisRange *rangePtr;
    
    graphPtr = elemPtr->obj.graphPtr;
    point = Blt_InvMap2D(graphPtr, x, y, &elemPtr->axes);
    if (elemPtr->zAxisPtr == elemPtr->axes.y) {
        value = point.y;
    } else if (elemPtr->zAxisPtr == elemPtr->axes.x) {
        value = point.x;
    } else {
        return TCL_ERROR;
    }
    rangePtr = &elemPtr->zAxisPtr->tickRange;
    *valuePtr = (value - rangePtr->min) / rangePtr->range;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * DrawGradientRectangle --
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static void
DrawGradientRectangle(Graph *graphPtr, Drawable drawable, BarElement *elemPtr, 
                      float x1, float y1, float x2, float y2, 
                      BarSegment *segPtr)
{
    Blt_PaintBrush brush;
    Blt_Picture picture;
    int w, h;

    w = (int)(x2 - x1) + 1;
    h = (int)(y2 - y1) + 1;
    
    if ((elemPtr->zAxisPtr == NULL) || (elemPtr->zAxisPtr->palette == NULL)) {
        return;                         /* No palette defined. */
    }
    picture = Blt_CreatePicture(w, h);
    if (picture == NULL) {
        return;                         /* Background is obscured. */
    }
    Blt_BlankPicture(picture, 0x0);
    brush = Blt_NewLinearGradientBrush();
    Blt_SetBrushOrigin(brush, -segPtr->x1, -segPtr->y1); 
    Blt_SetLinearGradientBrushPalette(brush, elemPtr->zAxisPtr->palette);
    Blt_SetLinearGradientBrushCalcProc(brush, GradientCalcProc, elemPtr);
    Blt_PaintRectangle(picture, 0, 0, w, h, 0, 0, brush, TRUE);
    Blt_FreeBrush(brush);
    Blt_PaintPicture(elemPtr->painter, drawable, picture, 0, 0, w, h, (int)x1, 
                     (int)y1);
    Blt_FreePicture(picture);
}

/*
 *---------------------------------------------------------------------------
 *
 * DrawColorRectangle --
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static void
DrawColorRectangle(Graph *graphPtr, Drawable drawable, Blt_Painter painter,
                   Blt_PaintBrush brush, float x1, float y1, float x2, float y2,
                   BarSegment *segPtr)
{
    Blt_Picture picture;
    int w, h;

    w = (int)(x2 - x1) + 1;
    h = (int)(y2 - y1) + 1;
    picture = Blt_CreatePicture(w, h);
    if (picture == NULL) {
        return;                         /* Can't allocate picture. */
    }
    Blt_BlankPicture(picture, 0x0);
    Blt_SetBrushOrigin(brush, -segPtr->x1, -segPtr->y1); 
    Blt_PaintRectangle(picture, 0, 0, w, h, 0, 0, brush, TRUE);
    Blt_PaintPicture(painter, drawable, picture, 0, 0, w, h, (int)x1, (int)y1);
    Blt_FreePicture(picture);
}

static TkRegion
SetClipRegion(Graph *graphPtr, BarElement *elemPtr)
{
    Blt_ChainLink link;
    TkRegion rgn;
    XRectangle clip;

    /* Setup clip region. */
    clip.x = graphPtr->x1;
    clip.y = graphPtr->y1;
    clip.width  = graphPtr->x2 - graphPtr->x1 + 1;
    clip.height = graphPtr->y2 - graphPtr->y1 + 1;
    rgn = TkCreateRegion();
    TkUnionRectWithRegion(&clip, rgn, rgn);

    for (link = Blt_Chain_FirstLink(elemPtr->styles); link != NULL; 
         link = Blt_Chain_NextLink(link)) {
        BarStyle *stylePtr;
        BarPen *penPtr;

        stylePtr = Blt_Chain_GetValue(link);
        penPtr = stylePtr->penPtr;
        if (penPtr->fillBg != NULL) {
            if (penPtr->stipple != None) {
                TkSetRegion(graphPtr->display, penPtr->fillGC, rgn);
            }
            Blt_Bg_SetClipRegion(graphPtr->tkwin, penPtr->fillBg, rgn);
        }
        if (penPtr->errorBarGC != None) {
            TkSetRegion(graphPtr->display, penPtr->errorBarGC, rgn);
        }
        if (penPtr->outlineGC != NULL) {
            TkSetRegion(graphPtr->display, penPtr->outlineGC, rgn);
        }
        if (penPtr->brush != NULL) {
            Blt_SetPainterClipRegion(elemPtr->painter, rgn);
        }
    }
    return rgn;
}

static void
UnsetClipRegion(Graph *graphPtr, BarElement *elemPtr, TkRegion rgn)
{
    Blt_ChainLink link;

    for (link = Blt_Chain_FirstLink(elemPtr->styles); link != NULL; 
         link = Blt_Chain_NextLink(link)) {
        BarStyle *stylePtr;
        BarPen *penPtr;
        
        stylePtr = Blt_Chain_GetValue(link);
        penPtr = stylePtr->penPtr;
        if (penPtr->brush != NULL) {
            Blt_UnsetPainterClipRegion(elemPtr->painter);
        }
        if (penPtr->fillBg) {
            Blt_Bg_UnsetClipRegion(graphPtr->tkwin, penPtr->fillBg);
        }
        if (penPtr->stipple != None) {
            XSetClipMask(graphPtr->display, penPtr->fillGC, None);
        }
        if (penPtr->outlineGC != None) {
            XSetClipMask(graphPtr->display, penPtr->outlineGC, None);
        }
        if (penPtr->errorBarGC != None) {
            XSetClipMask(graphPtr->display, penPtr->errorBarGC, None);
        }
    }
    TkDestroyRegion(rgn);
}

static void
DrawRectangle(Graph *graphPtr, Drawable drawable, BarElement *elemPtr, 
              BarPen *penPtr, BarSegment *segPtr)
{
    XRectangle r;
    Region2d reg;
    float x1, x2, y1, y2;

    GraphExtents(graphPtr, &reg);

    x1 = MAX(reg.left, segPtr->x1);
    y1 = MAX(reg.top, segPtr->y1);
    x2 = MIN(reg.right, segPtr->x2);
    y2 = MIN(reg.bottom, segPtr->y2);
        
    r.x = (short int)x1;
    r.y = (short int)y1;
    r.width = (int)(x2 - x1) + 1;
    r.height = (int)(y2 - y1) + 1;

    if (elemPtr->zAxisPtr != NULL) {
        DrawGradientRectangle(graphPtr, drawable, elemPtr, 
                              x1, y1, x2, y2, segPtr);
    } else if (penPtr->stipple != None) {
        XFillRectangle(graphPtr->display, drawable, penPtr->fillGC, 
                       r.x, r.y, r.width, r.height);
    } else if (penPtr->brush != NULL) {
        DrawColorRectangle(graphPtr, drawable, elemPtr->painter, penPtr->brush,
                           x1, y1, x2, y2, segPtr);
    } else if (penPtr->fillBg != NULL) {
        Blt_Bg_FillRectangle(graphPtr->tkwin, drawable, penPtr->fillBg,
          r.x, r.y, r.width, r.height, 0, TK_RELIEF_FLAT);
    }
    if ((penPtr->outlineColor != NULL) && (penPtr->borderWidth > 0)) {
        DrawOutline(graphPtr, drawable, elemPtr, penPtr, segPtr->x1, segPtr->y1,
                    segPtr->x2, segPtr->y2);
    }
}

static void
DrawXErrorBar(Graph *graphPtr, Drawable drawable, BarElement *elemPtr, 
              BarPen *penPtr, BarSegment *segPtr)
{
    double high, low;
    double x, y;
    XSegment xSegments[3];
    int numSegments;
    Region2d reg;

    GraphExtents(graphPtr, &reg);

    x = elemPtr->x.values[segPtr->index];
    y = elemPtr->y.values[segPtr->index];

    if ((!FINITE(x)) || (!FINITE(y))) {
        return;
    }
    if (elemPtr->xError.numValues > 0) {
        high = x + elemPtr->xError.values[segPtr->index];
        low  = x - elemPtr->xError.values[segPtr->index];
    } else {
        high = elemPtr->xHigh.values[segPtr->index];
        low  = elemPtr->xLow.values[segPtr->index];
    }
    numSegments = 0;
    if ((FINITE(high)) && (FINITE(low)))  {
        Point2d p, q;
        Point2d minErr, maxErr;

        p = maxErr = Blt_Map2D(graphPtr, high, y, &elemPtr->axes);
        q = minErr = Blt_Map2D(graphPtr, low, y, &elemPtr->axes);
        if (Blt_LineRectClip(&reg, &p, &q)) {
            xSegments[numSegments].x1 = p.x;
            xSegments[numSegments].x2 = q.x;
            xSegments[numSegments].y1 = p.y;
            xSegments[numSegments].y2 = q.y;
            numSegments++;
        }
        /* Left cap */
        p.x = q.x = minErr.x;
        p.y = maxErr.y - penPtr->errorBarCapWidth;
        q.y = minErr.y + penPtr->errorBarCapWidth;
        if (Blt_LineRectClip(&reg, &p, &q)) {
            xSegments[numSegments].x1 = p.x;
            xSegments[numSegments].x2 = q.x;
            xSegments[numSegments].y1 = p.y;
            xSegments[numSegments].y2 = q.y;
            numSegments++;
        }
        /* Right cap */
        p.x = q.x = maxErr.x;
        p.y = maxErr.y - penPtr->errorBarCapWidth;
        q.y = minErr.y + penPtr->errorBarCapWidth;
        if (Blt_LineRectClip(&reg, &p, &q)) {
            xSegments[numSegments].x1 = p.x;
            xSegments[numSegments].x2 = q.x;
            xSegments[numSegments].y1 = p.y;
            xSegments[numSegments].y2 = q.y;
            numSegments++;
        }
    }
    XDrawSegments(graphPtr->display, drawable, penPtr->errorBarGC, xSegments, 
        numSegments);
}

static void
DrawYErrorBar(Graph *graphPtr, Drawable drawable, BarElement *elemPtr, 
              BarPen *penPtr, BarSegment *segPtr)
{
    double high, low;
    double x, y;
    XSegment xSegments[3];
    int numSegments;
    Region2d reg;

    GraphExtents(graphPtr, &reg);

    x = elemPtr->x.values[segPtr->index];
    y = elemPtr->y.values[segPtr->index];

    if ((!FINITE(x)) || (!FINITE(y))) {
        return;
    }
    if (elemPtr->yError.numValues > 0) {
        high = y + elemPtr->yError.values[segPtr->index];
        low  = y - elemPtr->yError.values[segPtr->index];
    } else {
        high = elemPtr->yHigh.values[segPtr->index];
        low  = elemPtr->yLow.values[segPtr->index];
    }
    numSegments = 0;
    if ((FINITE(high)) && (FINITE(low)))  {
        Point2d p, q;
        Point2d minErr, maxErr;

        p = maxErr = Blt_Map2D(graphPtr, x, high, &elemPtr->axes);
        q = minErr = Blt_Map2D(graphPtr, x, low, &elemPtr->axes);
        if (Blt_LineRectClip(&reg, &p, &q)) {
            xSegments[numSegments].x1 = p.x;
            xSegments[numSegments].x2 = q.x;
            xSegments[numSegments].y1 = p.y;
            xSegments[numSegments].y2 = q.y;
            numSegments++;
        }
        /* Bottom cap */
        p.y = q.y = minErr.y;
        p.x = maxErr.x - penPtr->errorBarCapWidth;
        q.x = minErr.x + penPtr->errorBarCapWidth;
        if (Blt_LineRectClip(&reg, &p, &q)) {
            xSegments[numSegments].x1 = p.x;
            xSegments[numSegments].x2 = q.x;
            xSegments[numSegments].y1 = p.y;
            xSegments[numSegments].y2 = q.y;
            numSegments++;
        }
        /* Top cap */
        p.y = q.y = maxErr.y;
        p.x = maxErr.x - penPtr->errorBarCapWidth;
        q.x = minErr.x + penPtr->errorBarCapWidth;
        if (Blt_LineRectClip(&reg, &p, &q)) {
            xSegments[numSegments].x1 = p.x;
            xSegments[numSegments].x2 = q.x;
            xSegments[numSegments].y1 = p.y;
            xSegments[numSegments].y2 = q.y;
            numSegments++;
        }
    }
    XDrawSegments(graphPtr->display, drawable, penPtr->errorBarGC, xSegments, 
        numSegments);
}

/*
 *---------------------------------------------------------------------------
 *
 * DrawValue --
 *
 *      Draws the numeric value of the bar.
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static void
DrawValue(Graph *graphPtr, Drawable drawable, BarElement *elemPtr,
          BarPen *penPtr, BarSegment *segPtr)
{
    const char *fmt;
    double x, y;
    Point2d anchorPos;
    char string[TCL_DOUBLE_SPACE * 2 + 2];

    fmt = penPtr->valueFormat;
    if (fmt == NULL) {
        fmt = "%g";
    }

    x = elemPtr->x.values[segPtr->index];
    y = elemPtr->y.values[segPtr->index];
    if (penPtr->showValues & SHOW_X) {
        if (penPtr->showValues & SHOW_Y) {
            Blt_FmtString(string, TCL_DOUBLE_SPACE, fmt, x);
            strcat(string, ",");
            Blt_FmtString(string + strlen(string), TCL_DOUBLE_SPACE, fmt,y);
        } else {
            Blt_FmtString(string, TCL_DOUBLE_SPACE, fmt, x); 
        }
    } else if (penPtr->showValues & SHOW_Y) {
        Blt_FmtString(string, TCL_DOUBLE_SPACE, fmt, y); 
    }
    if (graphPtr->flags & INVERTED) {
        anchorPos.y = (segPtr->y1 + segPtr->y2) * 0.5;
        anchorPos.x = segPtr->x2;
        if (x < graphPtr->baseline) {
            anchorPos.x = segPtr->x1;
        } 
    } else {
        anchorPos.x = (segPtr->x1 + segPtr->x2) * 0.5;
        anchorPos.y = segPtr->y1;
        if (y < graphPtr->baseline) {                       
            anchorPos.y = segPtr->y2;
        }
    }
    Blt_DrawText(graphPtr->tkwin, drawable, string, &penPtr->valueStyle, 
                 (int)anchorPos.x, (int)anchorPos.y);
}

static void
DrawBarSegment(Graph *graphPtr, Drawable drawable, BarElement *elemPtr, 
               BarPen *penPtr, BarSegment *segPtr)
{
    DrawRectangle(graphPtr, drawable, elemPtr, penPtr, segPtr);
    if ((penPtr->showErrorBars & SHOW_X) && (elemPtr->flags & HAVE_XERROR)) {
        DrawXErrorBar(graphPtr, drawable, elemPtr, penPtr, segPtr);
    }
    if ((penPtr->showErrorBars & SHOW_Y) && (elemPtr->flags & HAVE_YERROR)) {
        DrawYErrorBar(graphPtr, drawable, elemPtr, penPtr, segPtr);
    }
    if (penPtr->showValues & (SHOW_X|SHOW_Y)) {
        DrawValue(graphPtr, drawable, elemPtr, penPtr, segPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * DrawNormalProc --
 *
 *      Draws the rectangle representing the bar element.  
 *
 *      Don't draw bars that aren't visible (i.e. within the limits of the
 *      axis).
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      X drawing commands are output.
 *
 *---------------------------------------------------------------------------
 */
static void
DrawNormalProc(Graph *graphPtr, Drawable drawable, Element *basePtr)
{
    BarElement *elemPtr = (BarElement *)basePtr;
    BarSegment *segPtr;

    for (segPtr = elemPtr->headPtr; segPtr != NULL; segPtr = segPtr->next) {
        if (!PLAYING(graphPtr, segPtr->index)) {
            continue;
        }
        DrawBarSegment(graphPtr, drawable, elemPtr, segPtr->penPtr, segPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * DrawActiveProc --
 *
 *      Draws bars representing the active segments of the bar element.  
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      X drawing commands are output.
 *
 *---------------------------------------------------------------------------
 */
static void
DrawActiveProc(Graph *graphPtr, Drawable drawable, Element *basePtr)
{
    BarElement *elemPtr = (BarElement *)basePtr;
    BarSegment *segPtr;
    TkRegion rgn;

    rgn = SetClipRegion(graphPtr, elemPtr);
    for (segPtr = elemPtr->headPtr; segPtr != NULL; segPtr = segPtr->next) {
        if (!PLAYING(graphPtr, segPtr->index)) {
            continue;
        }
        if (segPtr->flags & ACTIVE) {
            DrawBarSegment(graphPtr, drawable, elemPtr, elemPtr->activePenPtr, 
                           segPtr);
        }
    }
    UnsetClipRegion(graphPtr, elemPtr, rgn);
}

/*
 *---------------------------------------------------------------------------
 *
 * SymbolToPostScript --
 *
 *      Draw a symbol centered at the given x,y window coordinate based
 *      upon the element symbol type and size.
 *
 * Results:
 *      None.
 *
 * Problems:
 *      Most notable is the round-off errors generated when calculating the
 *      centered position of the symbol.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static void
SymbolToPostScriptProc(Graph *graphPtr, Blt_Ps ps, Element *basePtr,
                       double x, double y, int size)
{
    BarElement *elemPtr = (BarElement *)basePtr;
    BarPen *penPtr;

    penPtr = NORMALPEN(elemPtr);
    if ((penPtr->fillBg == NULL) && (penPtr->outlineColor == NULL)) {
        return;
    }
    /*
     * Build a PostScript procedure to draw the fill and outline of the
     * symbol after the path of the symbol shape has been formed.
     */
    Blt_Ps_Append(ps, "\n"
                  "/DrawSymbolProc {\n"
                  "gsave\n    ");
    if (penPtr->stipple != None) {
        if (penPtr->fillBg != NULL) {
            Blt_Ps_XSetBackground(ps, Blt_Bg_BorderColor(penPtr->fillBg));
            Blt_Ps_Append(ps, "    gsave fill grestore\n    ");
        }
        if (penPtr->outlineColor != NULL) {
            Blt_Ps_XSetForeground(ps, penPtr->outlineColor);
        } else {
            Blt_Ps_XSetForeground(ps, Blt_Bg_BorderColor(penPtr->fillBg));
        }
        Blt_Ps_XSetStipple(ps, graphPtr->display, penPtr->stipple);
    } else if (penPtr->outlineColor != NULL) {
        Blt_Ps_XSetForeground(ps, penPtr->outlineColor);
        Blt_Ps_Append(ps, "    fill\n");
    }
    Blt_Ps_Append(ps, "  grestore\n");
    Blt_Ps_Append(ps, "} def\n\n");
    Blt_Ps_Format(ps, "%g %g %d Sq\n", x, y, size);
}

static void
RectangleToPostScript(Graph *graphPtr, Blt_Ps ps, BarPen *penPtr, 
                             BarSegment *segPtr)
{
    if ((penPtr->fillBg == NULL) && (penPtr->outlineColor == NULL)) {
        return;
    }
    if ((segPtr->x1 >= segPtr->x2) || (segPtr->y1 >= segPtr->y2)) {
        return;
    }
    if (penPtr->stipple != None) {
        Blt_Ps_Rectangle(ps, segPtr->x1, segPtr->y1, segPtr->x2 - segPtr->x1, 
                         segPtr->y2 - segPtr->y1-1);
        if (penPtr->fillBg != NULL) {
            Blt_Ps_XSetBackground(ps, Blt_Bg_BorderColor(penPtr->fillBg));
            Blt_Ps_Append(ps, "gsave fill grestore\n");
        }
        if (penPtr->outlineColor != NULL) {
            Blt_Ps_XSetForeground(ps, penPtr->outlineColor);
        } else {
            Blt_Ps_XSetForeground(ps,Blt_Bg_BorderColor(penPtr->fillBg));
        }
        Blt_Ps_XSetStipple(ps, graphPtr->display, penPtr->stipple);
    } else if (penPtr->outlineColor != NULL) {
        Blt_Ps_XSetForeground(ps, penPtr->outlineColor);
        Blt_Ps_XFillRectangle(ps, segPtr->x1, segPtr->y1,
               segPtr->x2 - segPtr->x1, segPtr->y2 - segPtr->y1);
    }
    if ((penPtr->fillBg != NULL) && (penPtr->borderWidth > 0) && 
        (penPtr->relief != TK_RELIEF_FLAT)) {
        Blt_Ps_Draw3DRectangle(ps, Blt_Bg_Border(penPtr->fillBg), 
                segPtr->x1, segPtr->y1, 
                segPtr->x2 - segPtr->x1, segPtr->y2 - segPtr->y1,
                penPtr->borderWidth, penPtr->relief);
    }
}

static void
XErrorBarToPostScript(Graph *graphPtr, Blt_Ps ps, BarElement *elemPtr, 
                     BarPen *penPtr, BarSegment *segPtr)
{
    double high, low;
    double x, y;
    Region2d reg;

    GraphExtents(graphPtr, &reg);

    x = elemPtr->x.values[segPtr->index];
    y = elemPtr->y.values[segPtr->index];

    if ((!FINITE(x)) || (!FINITE(y))) {
        return;
    }
    if (elemPtr->xError.numValues > 0) {
        high = x + elemPtr->xError.values[segPtr->index];
        low  = x - elemPtr->xError.values[segPtr->index];
    } else {
        high = elemPtr->xHigh.values[segPtr->index];
        low  = elemPtr->xLow.values[segPtr->index];
    }

    Blt_Ps_XSetLineAttributes(ps, penPtr->errorBarColor,
        penPtr->errorBarLineWidth, NULL, CapButt, JoinMiter);

    Blt_Ps_Append(ps, "% X Error bar\n");
    Blt_Ps_Append(ps, "newpath\n");

    if ((FINITE(high)) && (FINITE(low)))  {
        Point2d p, q;
        Point2d minErr, maxErr;

        p = maxErr = Blt_Map2D(graphPtr, high, y, &elemPtr->axes);
        q = minErr = Blt_Map2D(graphPtr, low, y, &elemPtr->axes);
        if (Blt_LineRectClip(&reg, &p, &q)) {
            Blt_Ps_Format(ps, "  %g %g moveto %g %g lineto\n", 
                          p.x, p.y, q.x, q.y);
            Blt_Ps_Append(ps, "DashesProc stroke\n");
        }
        /* Left cap */
        p.x = q.x = minErr.x;
        p.y = maxErr.y - penPtr->errorBarCapWidth;
        q.y = minErr.y + penPtr->errorBarCapWidth;
        if (Blt_LineRectClip(&reg, &p, &q)) {
            Blt_Ps_Format(ps, "  %g %g moveto %g %g lineto\n", 
                          p.x, p.y, q.x, q.y);
            Blt_Ps_Append(ps, "DashesProc stroke\n");
        }
        /* Right cap */
        p.x = q.x = maxErr.x;
        p.y = maxErr.y - penPtr->errorBarCapWidth;
        q.y = minErr.y + penPtr->errorBarCapWidth;
        if (Blt_LineRectClip(&reg, &p, &q)) {
            Blt_Ps_Format(ps, "  %g %g moveto %g %g lineto\n", 
                          p.x, p.y, q.x, q.y);
            Blt_Ps_Append(ps, "DashesProc stroke\n");
        }
    }
    Blt_Ps_Append(ps, "% end segments\n");
}

static void
YErrorBarToPostScript(Graph *graphPtr, Blt_Ps ps, BarElement *elemPtr, 
                      BarPen *penPtr, BarSegment *segPtr)
{
    double high, low;
    double x, y;
    Region2d reg;

    GraphExtents(graphPtr, &reg);

    x = elemPtr->x.values[segPtr->index];
    y = elemPtr->y.values[segPtr->index];

    if ((!FINITE(x)) || (!FINITE(y))) {
        return;
    }
    if (elemPtr->yError.numValues > 0) {
        high = y + elemPtr->yError.values[segPtr->index];
        low  = y - elemPtr->yError.values[segPtr->index];
    } else {
        high = elemPtr->yHigh.values[segPtr->index];
        low  = elemPtr->yLow.values[segPtr->index];
    }

    Blt_Ps_XSetLineAttributes(ps, penPtr->errorBarColor,
        penPtr->errorBarLineWidth, NULL, CapButt, JoinMiter);
    Blt_Ps_Append(ps, "% Y Error bar\n");
    Blt_Ps_Append(ps, "newpath\n");

    if ((FINITE(high)) && (FINITE(low)))  {
        Point2d p, q;
        Point2d minErr, maxErr;

        p = maxErr = Blt_Map2D(graphPtr, x, high, &elemPtr->axes);
        q = minErr = Blt_Map2D(graphPtr, x, low, &elemPtr->axes);
        if (Blt_LineRectClip(&reg, &p, &q)) {
            Blt_Ps_Format(ps, "  %g %g moveto %g %g lineto\n", 
                          p.x, p.y, q.x, q.y);
            Blt_Ps_Append(ps, "DashesProc stroke\n");
        }
        /* Bottom cap */
        p.y = q.y = minErr.y;
        p.x = maxErr.x - penPtr->errorBarCapWidth;
        q.x = minErr.x + penPtr->errorBarCapWidth;
        if (Blt_LineRectClip(&reg, &p, &q)) {
            Blt_Ps_Format(ps, "  %g %g moveto %g %g lineto\n", 
                          p.x, p.y, q.x, q.y);
            Blt_Ps_Append(ps, "DashesProc stroke\n");
        }
        /* Top cap */
        p.y = q.y = maxErr.y;
        p.x = maxErr.x - penPtr->errorBarCapWidth;
        q.x = minErr.x + penPtr->errorBarCapWidth;
        if (Blt_LineRectClip(&reg, &p, &q)) {
            Blt_Ps_Format(ps, "  %g %g moveto %g %g lineto\n", 
                          p.x, p.y, q.x, q.y);
            Blt_Ps_Append(ps, "DashesProc stroke\n");
        }
    }
    Blt_Ps_Append(ps, "% end segments\n");
}

static void
ValueToPostScript(Graph *graphPtr, Blt_Ps ps, BarElement *elemPtr,
                   BarPen *penPtr, BarSegment *segPtr)
{
    const char *fmt;
    double x, y;
    Point2d anchorPos;
    char string[TCL_DOUBLE_SPACE * 2 + 2];
    
    fmt = penPtr->valueFormat;
    if (fmt == NULL) {
        fmt = "%g";
    }
    x = elemPtr->x.values[segPtr->index];
    y = elemPtr->y.values[segPtr->index];
    if (penPtr->showValues & SHOW_X) {
        if (penPtr->showValues & SHOW_Y) {
            Blt_FmtString(string, TCL_DOUBLE_SPACE, fmt, x);
            strcat(string, ",");
            Blt_FmtString(string + strlen(string), TCL_DOUBLE_SPACE, fmt,y);
        } else {
            Blt_FmtString(string, TCL_DOUBLE_SPACE, fmt, x); 
        }
    } else if (penPtr->showValues == SHOW_Y) {
        Blt_FmtString(string, TCL_DOUBLE_SPACE, fmt, y); 
    }
    if (graphPtr->flags & INVERTED) {
        anchorPos.y = (segPtr->y1 + segPtr->y2) * 0.5;
        anchorPos.x = segPtr->x1;
        if (x < graphPtr->baseline) {
            anchorPos.x -= segPtr->x2;
        } 
    } else {
        anchorPos.x = (segPtr->x1 + segPtr->x2) * 0.5;
        anchorPos.y = segPtr->y1;
        if (y < graphPtr->baseline) {                       
            anchorPos.y += segPtr->y2;
        }
    }
    Blt_Ps_DrawText(ps, string, &penPtr->valueStyle, anchorPos.x, anchorPos.y);
}

static void
SegmentToPostScript(Graph *graphPtr, Blt_Ps ps, BarElement *elemPtr, 
               BarPen *penPtr, BarSegment *segPtr)
{
    RectangleToPostScript(graphPtr, ps, penPtr, segPtr);
    if ((penPtr->showErrorBars & SHOW_X) && (elemPtr->flags & HAVE_XERROR)) {
        XErrorBarToPostScript(graphPtr, ps, elemPtr, penPtr, segPtr);
    }
    if ((penPtr->showErrorBars & SHOW_Y) && (elemPtr->flags & HAVE_YERROR)) {
        YErrorBarToPostScript(graphPtr, ps, elemPtr, penPtr, segPtr);
    }
    if (penPtr->showValues & (SHOW_X | SHOW_Y)) {
        ValueToPostScript(graphPtr, ps, elemPtr, penPtr, segPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ActiveToPostScript --
 *
 *      Similar to the NormalToPostScript procedure, generates PostScript
 *      commands to display the bars representing the active bar segments
 *      of the element.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      PostScript pen width, dashes, and color settings are changed.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static void
ActiveToPostScriptProc(Graph *graphPtr, Blt_Ps ps, Element *basePtr)
{
    BarElement *elemPtr = (BarElement *)basePtr;
    BarSegment *segPtr;

    for (segPtr = elemPtr->headPtr; segPtr != NULL; segPtr = segPtr->next) {
        if (!PLAYING(graphPtr, segPtr->index)) {
            continue;
        }
        if (segPtr->flags & ACTIVE) {
            SegmentToPostScript(graphPtr, ps, elemPtr, elemPtr->activePenPtr, 
                                segPtr);
        }
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * NormalToPostScriptProc --
 *
 *      Generates PostScript commands to form the bars representing the
 *      segments of the bar element.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      PostScript pen width, dashes, and color settings are changed.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static void
NormalToPostScriptProc(Graph *graphPtr, Blt_Ps ps, Element *basePtr)
{
    BarElement *elemPtr = (BarElement *)basePtr;
    BarSegment *segPtr;

    for (segPtr = elemPtr->headPtr; segPtr != NULL; segPtr = segPtr->next) {
        if (!PLAYING(graphPtr, segPtr->index)) {
            continue;
        }
        SegmentToPostScript(graphPtr, ps, elemPtr, segPtr->penPtr, segPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * DestroyProc --
 *
 *      Release memory and resources allocated for the bar element.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Everything associated with the bar element is freed up.
 *
 *---------------------------------------------------------------------------
 */

static void
DestroyProc(Graph *graphPtr, Element *basePtr)
{
    BarElement *elemPtr = (BarElement *)basePtr;

    DestroyPen(graphPtr, elemPtr->builtinPenPtr);
    if (elemPtr->activePenPtr != NULL) {
        Blt_FreePen((Pen *)elemPtr->activePenPtr);
    }
    ResetElement(elemPtr);
    if (elemPtr->styles != NULL) {
        Blt_FreeStyles(elemPtr->styles);
        Blt_Chain_Destroy(elemPtr->styles);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_BarElement --
 *
 *      Allocate memory and initialize methods for the new bar element.
 *
 * Results:
 *      The pointer to the newly allocated element structure is returned.
 *
 * Side effects:
 *      Memory is allocated for the bar element structure.
 *
 *---------------------------------------------------------------------------
 */
static ElementProcs barProcs = {
    NearestProc,
    ConfigureProc,
    DestroyProc,
    DrawActiveProc,
    DrawNormalProc,
    DrawSymbolProc,
    ExtentsProc,
    NULL,                               /* Find the points within the
                                         * search radius. */
    ActiveToPostScriptProc,
    NormalToPostScriptProc,
    SymbolToPostScriptProc,
    MapProc,
};


Element *
Blt_BarElement(Graph *graphPtr, Blt_HashEntry *hPtr)
{
    BarElement *elemPtr;

    elemPtr = Blt_AssertCalloc(1, sizeof(BarElement));
    elemPtr->procsPtr = &barProcs;
    elemPtr->configSpecs = barElemConfigSpecs;
    elemPtr->legendRelief = TK_RELIEF_FLAT;
    Blt_GraphSetObjectClass(&elemPtr->obj, CID_ELEM_BAR);
    elemPtr->obj.name = Blt_GetHashKey(&graphPtr->elements.nameTable, hPtr);
    elemPtr->obj.graphPtr = graphPtr;
    /* By default, an element's name and label are the same. */
    elemPtr->label = Blt_AssertStrdup(elemPtr->obj.name);
    elemPtr->builtinPenPtr = &elemPtr->builtinPen;
    InitPen(elemPtr->builtinPenPtr);
    elemPtr->styles = Blt_Chain_Create();
    bltBarStylesOption.clientData = (ClientData)sizeof(BarStyle);
    elemPtr->hashPtr = hPtr;
    elemPtr->painter = Blt_GetPainter(graphPtr->tkwin, 1.0);
    Blt_SetHashValue(hPtr, elemPtr);
    return (Element *)elemPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * Blt_InitBarGroups --
 *
 *      Generate a table of abscissa frequencies.  Duplicate x-coordinates
 *      (depending upon the bar drawing mode) indicate that something
 *      special should be done with each bar segment mapped to the same
 *      abscissa (i.e. it should be stacked, aligned, or overlay-ed with
 *      other segments)
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Memory is allocated for the bar element structure.
 *
 *---------------------------------------------------------------------------
 */
void
Blt_InitBarGroups(Graph *graphPtr)
{
    Blt_ChainLink link;
    int numGroups, numMembers;

    /*
     * Free resources associated with a previous group table. This includes
     * the array of group information and the table itself.
     */
    Blt_DestroyBarGroups(graphPtr);
    if (graphPtr->mode == BARS_INFRONT) {
        return;                         /* No set table is needed for
                                         * "infront" mode */
    }
    /*
     * Initialize a hash table and fill it with unique abscissas.  Keep
     * track of the frequency of each x-coordinate and how many abscissas
     * have duplicate mappings.
     */
    Blt_InitHashTable(&graphPtr->groupTable, sizeof(BarGroupKey) / sizeof(int));
    numGroups = numMembers = 0;
    for (link = Blt_Chain_FirstLink(graphPtr->elements.displayList);
        link != NULL; link = Blt_Chain_NextLink(link)) {
        BarElement *elemPtr;
        int i, numPoints;

        elemPtr = Blt_Chain_GetValue(link);
        if ((elemPtr->flags & HIDDEN) ||
            (elemPtr->obj.classId != CID_ELEM_BAR)) {
            continue;                   /* Hidden or not a bar element. */
        }
        numPoints = NUMBEROFPOINTS(elemPtr);
        for (i = 0; i < numPoints; i++) {
            Blt_HashEntry *hPtr;
            BarGroupKey key;
            int isNew;
            
            memset(&key, 0, sizeof(key));
            key.value = elemPtr->x.values[i];
            key.axes = elemPtr->axes;
            key.axes.y = NULL;

            hPtr = Blt_CreateHashEntry(&graphPtr->groupTable, (char *)&key,
                &isNew);
            if (isNew) {
                BarGroup *groupPtr;

                /* Create a group entry for each unique value/axis pairing. */
                groupPtr = Blt_AssertMalloc(sizeof(BarGroup));
                groupPtr->axes = elemPtr->axes;
                Blt_SetHashValue(hPtr, groupPtr);
                groupPtr->sum = fabs(elemPtr->y.values[i]);
                groupPtr->max = elemPtr->y.values[i];
                groupPtr->numMembers = 1;
                numGroups++;
                if (numMembers < groupPtr->numMembers) {
                    numMembers = groupPtr->numMembers;
                }
            } else {
                BarGroup *groupPtr;

                groupPtr = Blt_GetHashValue(hPtr);
                if (groupPtr->max < elemPtr->y.values[i]) {
                    groupPtr->max = elemPtr->y.values[i];
                }
                groupPtr->sum += fabs(elemPtr->y.values[i]); 
                groupPtr->numMembers++;
                if (numMembers < groupPtr->numMembers) {
                    numMembers = groupPtr->numMembers;
                }
            }
        }
    }
    graphPtr->maxBarGroupSize = numMembers;
    graphPtr->numBarGroups = numGroups;
}

void
Blt_ResetBarGroups(Graph *graphPtr)
{
    Blt_HashSearch iter;
    Blt_HashEntry *hPtr;
    
    for (hPtr = Blt_FirstHashEntry(&graphPtr->groupTable, &iter); 
         hPtr != NULL; hPtr = Blt_NextHashEntry(&iter)) {
        BarGroup *groupPtr;

        groupPtr = Blt_GetHashValue(hPtr);
        groupPtr->lastY = 0.0;
        groupPtr->count = 0;
    }
}

void
Blt_DestroyBarGroups(Graph *graphPtr)
{
    Blt_HashSearch iter;
    Blt_HashEntry *hPtr;

    graphPtr->numBarGroups = 0;
    graphPtr->maxBarGroupSize = 0;
    for (hPtr = Blt_FirstHashEntry(&graphPtr->groupTable, &iter); 
         hPtr != NULL; hPtr = Blt_NextHashEntry(&iter)) {
        BarGroup *groupPtr;
        
        groupPtr = Blt_GetHashValue(hPtr);
        Blt_Free(groupPtr);
    }
    Blt_DeleteHashTable(&graphPtr->groupTable);
    Blt_InitHashTable(&graphPtr->groupTable, sizeof(BarGroupKey) / sizeof(int));
}

