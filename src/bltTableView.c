/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * bltTableView.c --
 *
 * This module implements an table viewer widget for the BLT toolkit.
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

/* 
 * TODO:
 *      o -autocreate rows|columns|both|none for new table
 *      x Focus ring on cells.
 *      x Rules for row/columns (-rulewidth > 0)
 *      o Dashes for row/column rules.
 *      o -span for rows/columns titles 
 *      o Printing PS/PDF 
 *      x Underline text for active cells.
 *      x Imagebox cells
 *      x Combobox cells
 *      x Combobox column filters
 *      x Text editor widget for cells
 *      o XCopyArea for scrolling non-damaged areas.
 *      o Manage scrollbars geometry.
 *      o -padx -pady for styles 
 *      o -titlefont, -titlecolor, -activetitlecolor, -disabledtitlecolor 
 *         for columns?
 *      o -justify period (1.23), comma (1,23), space (1.23 ev)
 *      o color icons for resize cursors
 */
/*
 * Known Bugs:
 *      o Row and column titles sometimes get drawn in the wrong location.
 *      o Too slow loading.  
 *      o Should be told what rows and columns are being added instead
 *        of checking. Make that work with -whenidle.
 *      o Overallocate row and column arrays so that you don't have to
 *        keep allocating a new array everytime.
 */

#define BUILD_BLT_TK_PROCS 1
#include "bltInt.h"

#ifndef NO_TABLEVIEW

#ifdef HAVE_CTYPE_H
  #include <ctype.h>
#endif  /* HAVE_CTYPE_H */

#ifdef HAVE_STDLIB_H
  #include <stdlib.h> 
#endif /* HAVE_STDLIB_H */

#ifdef HAVE_STRING_H
  #include <string.h>
#endif /* HAVE_STRING_H */

#ifdef HAVE_LIMITS_H
  #include <limits.h>
#endif  /* HAVE_LIMITS_H */

#include <X11/Xutil.h>
#include <X11/Xatom.h>

#include "tclIntDecls.h"
#include "bltConfig.h"
#include "bltSwitch.h"
#include "bltNsUtil.h"
#include "bltVar.h"
#include "bltBg.h"
#include "bltFont.h"
#include "bltDataTable.h"
#include "bltBind.h"
#include "bltTableView.h"
#include "bltInitCmd.h"
#include "bltImage.h"
#include "bltText.h"
#include "bltOp.h"

#define TABLEVIEW_FIND_KEY "BLT TableView Find Command Interface"

#define TITLE_PADX              2
#define TITLE_PADY              1
#define FILTER_GAP              3

#define COLUMN_PAD              2
#define FOCUS_WIDTH             1
#define ICON_PADX               2
#define ICON_PADY               1
#define INSET_PAD               0
#define LABEL_PADX              0
#define LABEL_PADY              0

#include <X11/Xutil.h>
#include <X11/Xatom.h>

#define RESIZE_AREA             (8)
#define FCLAMP(x)       ((((x) < 0.0) ? 0.0 : ((x) > 1.0) ? 1.0 : (x)))
#define CHOOSE(default, override)       \
        (((override) == NULL) ? (default) : (override))

typedef ClientData (TagProc)(TableView *viewPtr, const char *string);

#define DEF_CELL_STATE                  "normal"
#define DEF_CELL_STYLE                  (char *)NULL

#define DEF_ACTIVE_TITLE_BG             STD_ACTIVE_BACKGROUND
#define DEF_ACTIVE_TITLE_FG             STD_ACTIVE_FOREGROUND
#define DEF_AUTO_CREATE                 "0"
#define DEF_COLUMN_FILTERS              "0"
#define DEF_BACKGROUND                  RGB_GREY97
#define DEF_BIND_TAGS                   "all"
#define DEF_BORDERWIDTH                 STD_BORDERWIDTH
#define DEF_COLUMN_ACTIVE_TITLE_RELIEF  "raised"
#define DEF_COLUMN_BIND_TAGS            "all"
#define DEF_COLUMN_COMMAND              (char *)NULL
#define DEF_COLUMN_EDIT                 "yes"
#define DEF_COLUMN_FORMAT_COMMAND       (char *)NULL
#define DEF_COLUMN_HIDE                 "no"
#define DEF_COLUMN_ICON                 (char *)NULL
#define DEF_COLUMN_MAX                  "0"             
#define DEF_COLUMN_MIN                  "0"
#define DEF_COLUMN_NORMAL_TITLE_BG      STD_NORMAL_BACKGROUND
#define DEF_COLUMN_NORMAL_TITLE_FG      STD_NORMAL_FOREGROUND
#define DEF_COLUMN_RESIZE_CURSOR        "sb_h_double_arrow"
#define DEF_COLUMN_PAD                  "2"
#define DEF_COLUMN_SHOW                 "yes"
#define DEF_COLUMN_STATE                "normal"
#define DEF_COLUMN_STYLE                (char *)NULL
#define DEF_COLUMN_TITLE_BORDERWIDTH    STD_BORDERWIDTH
#define DEF_COLUMN_TITLE_FONT           STD_FONT_NORMAL
#define DEF_COLUMN_TITLE_JUSTIFY        "center"
#define DEF_COLUMN_TITLE_RELIEF         "raised"
#define DEF_COLUMN_WEIGHT               "1.0"
#define DEF_COLUMN_WIDTH                "0"
#define DEF_DISABLED_TITLE_BG           STD_DISABLED_BACKGROUND
#define DEF_DISABLED_TITLE_FG           STD_DISABLED_FOREGROUND
#define DEF_EXPORT_SELECTION            "no"
#define DEF_FILTER_ACTIVE_BG            STD_ACTIVE_BACKGROUND
#define DEF_FILTER_ACTIVE_FG            RGB_BLACK
#define DEF_FILTER_ACTIVE_RELIEF        "raised"
#define DEF_FILTER_BORDERWIDTH          "1"
#define DEF_FILTER_HIGHLIGHT            "0"
#define DEF_FILTER_DISABLED_BG          STD_DISABLED_BACKGROUND
#define DEF_FILTER_DISABLED_FG          STD_DISABLED_FOREGROUND
#define DEF_FILTER_HIGHLIGHT_BG         RGB_GREY97 /*"#FFFFDD"*/
#define DEF_FILTER_HIGHLIGHT_FG         STD_NORMAL_FOREGROUND
#define DEF_FILTER_FONT                 STD_FONT_NORMAL
#define DEF_FILTER_ICON                 (char *)NULL
#define DEF_FILTER_MENU                 (char *)NULL
#define DEF_FILTER_NORMAL_BG            RGB_WHITE
#define DEF_FILTER_NORMAL_FG            RGB_BLACK
#define DEF_FILTER_SELECT_BG            STD_SELECT_BACKGROUND
#define DEF_FILTER_SELECT_FG            STD_SELECT_FOREGROUND
#define DEF_FILTER_SELECT_RELIEF        "sunken"
#define DEF_FILTER_RELIEF               "solid"
#define DEF_FILTER_SHOW                 "yes"
#define DEF_FILTER_STATE                "normal"
#define DEF_FILTER_TEXT                 ""
#define DEF_FOCUS_HIGHLIGHT_BG          STD_NORMAL_BACKGROUND
#define DEF_FOCUS_HIGHLIGHT_COLOR       RGB_BLACK
#define DEF_FOCUS_HIGHLIGHT_WIDTH       "2"
#define DEF_HEIGHT                      "400"
#define DEF_RELIEF                      "sunken"
#define DEF_ROW_ACTIVE_TITLE_RELIEF     "raised"
#define DEF_ROW_BIND_TAGS               "all"
#define DEF_ROW_COMMAND                 (char *)NULL
#define DEF_ROW_EDIT                    "yes"
#define DEF_ROW_HEIGHT                  "0"
#define DEF_ROW_HIDE                    "no"
#define DEF_ROW_ICON                    (char *)NULL
#define DEF_ROW_MAX                     "0"             
#define DEF_ROW_MIN                     "0"
#define DEF_ROW_NORMAL_TITLE_BG         STD_NORMAL_BACKGROUND
#define DEF_ROW_NORMAL_TITLE_FG         STD_NORMAL_FOREGROUND
#define DEF_ROW_RESIZE_CURSOR           "sb_v_double_arrow"
#define DEF_ROW_SHOW                    "yes"
#define DEF_ROW_STATE                   "normal"
#define DEF_ROW_STYLE                   (char *)NULL
#define DEF_ROW_TITLE_BORDERWIDTH       STD_BORDERWIDTH
#define DEF_ROW_TITLE_FONT              STD_FONT_NORMAL
#define DEF_ROW_TITLE_JUSTIFY           "center"
#define DEF_ROW_TITLE_RELIEF            "raised"
#define DEF_ROW_WEIGHT                  "1.0"
#define DEF_RULE_HEIGHT                 "0"
#define DEF_RULE_WIDTH                  "0"
#define DEF_SCROLL_INCREMENT            "20"
#define DEF_SCROLL_MODE                 "hierbox"
#define DEF_SELECT_MODE                 "singlerow"
#define DEF_SLIDE                       "columns"
#define DEF_SORT_COLUMN                 (char *)NULL
#define DEF_SORT_COLUMNS                (char *)NULL
#define DEF_SORT_COMMAND                (char *)NULL
#define DEF_SORT_DECREASING             "no"
#define DEF_SORT_DOWN_ICON              (char *)NULL
#define DEF_SORT_SELECTION              "no"
#define DEF_SORT_TYPE                   "auto"
#define DEF_SORT_UP_ICON                (char *)NULL
#define DEF_STYLE                       "default"
#define DEF_TABLE                       (char *)NULL
#define DEF_TAKE_FOCUS                  "1"
#define DEF_TITLES                      "columns"
#define DEF_WIDTH                       "200"
#define DEF_MAX_COLUMN_WIDTH            "0"
#define DEF_MAX_ROW_HEIGHT              "0"

static const char *sortTypeStrings[] = {
    "dictionary", "ascii", "integer", "real", "command", "none", "auto", NULL
};

typedef struct {
    TableView *viewPtr;
    Cell *cellPtr;
} FreeCell;

/*
 * ColumnIterator --
 *
 *      Tabs may be tagged with strings.  A tab may have many tags.  The
 *      same tag may be used for many tabs.
 *      
 */
typedef enum { 
    ITER_SINGLE, ITER_TAG, 
} IteratorType;

typedef struct _ColumnIterator {
    TableView *viewPtr;			
    IteratorType type;        
    Column *startPtr;        
    BLT_TABLE_ITERATOR cursor;
} ColumnIterator;

typedef struct _RowIterator {
    TableView *viewPtr;			
    IteratorType type;        
    Row *startPtr;        
    BLT_TABLE_ITERATOR cursor;
} RowIterator;


enum SortTypeValues { 
    SORT_DICTIONARY, SORT_ASCII, SORT_INTEGER, 
    SORT_REAL, SORT_COMMAND, SORT_NONE, SORT_AUTO
};

static Blt_OptionParseProc ObjToRowColumnFlag;
static Blt_OptionPrintProc RowColumnFlagToObj;
static Blt_CustomOption autoCreateOption = {
    ObjToRowColumnFlag, RowColumnFlagToObj, NULL, (ClientData)AUTO_MANAGE
};

static Blt_OptionParseProc ObjToSortColumn;
static Blt_OptionPrintProc SortColumnToObj;
static Blt_CustomOption sortColumnOption = {
    ObjToSortColumn, SortColumnToObj, NULL, (ClientData)0
};

static Blt_OptionParseProc ObjToSortOrder;
static Blt_OptionPrintProc SortOrderToObj;
static Blt_OptionFreeProc FreeSortOrderProc;
static Blt_CustomOption sortOrderOption = {
    ObjToSortOrder, SortOrderToObj, FreeSortOrderProc, (ClientData)0
};
static Blt_OptionParseProc ObjToEnum;
static Blt_OptionPrintProc EnumToObj;
static Blt_CustomOption typeOption = {
    ObjToEnum, EnumToObj, NULL, (ClientData)sortTypeStrings
};
static Blt_OptionParseProc ObjToIcon;
static Blt_OptionPrintProc IconToObj;
static Blt_OptionFreeProc FreeIconProc;
static Blt_CustomOption iconOption = {
    ObjToIcon, IconToObj, FreeIconProc, 
    (ClientData)0,                      /* Needs to point to the tableview
                                         * widget before calling option
                                         * routines. */
};
static Blt_OptionParseProc ObjToScrollMode;
static Blt_OptionPrintProc ScrollModeToObj;
static Blt_CustomOption scrollModeOption = {
    ObjToScrollMode, ScrollModeToObj, NULL, NULL,
};

static Blt_OptionParseProc ObjToSelectMode;
static Blt_OptionPrintProc SelectModeToObj;
static Blt_CustomOption selectModeOption = {
    ObjToSelectMode, SelectModeToObj, NULL, NULL,
};

static Blt_OptionParseProc ObjToState;
static Blt_OptionPrintProc StateToObj;
static Blt_CustomOption stateOption = {
    ObjToState, StateToObj, NULL, (ClientData)0
};

static Blt_OptionParseProc ObjToCellState;
static Blt_OptionPrintProc CellStateToObj;
static Blt_CustomOption cellStateOption = {
    ObjToCellState, CellStateToObj, NULL, (ClientData)0
};

static Blt_OptionParseProc ObjToLimits;
static Blt_OptionPrintProc LimitsToObj;
static Blt_CustomOption limitsOption =
{
    ObjToLimits, LimitsToObj, NULL, (ClientData)0
};

static Blt_OptionParseProc ObjToStyle;
static Blt_OptionPrintProc StyleToObj;
static Blt_OptionFreeProc FreeStyleProc;
static Blt_CustomOption styleOption = {
    ObjToStyle, StyleToObj, FreeStyleProc, 
    (ClientData)0,                      /* Needs to point to the tableview
                                         * widget before calling
                                         * routines. */
};

static Blt_CustomOption slideOption = {
    ObjToRowColumnFlag, RowColumnFlagToObj, NULL, (ClientData)SLIDE_ENABLED
};

static Blt_OptionParseProc ObjToTable;
static Blt_OptionPrintProc TableToObj;
static Blt_OptionFreeProc FreeTableProc;
static Blt_CustomOption tableOption = {
    ObjToTable, TableToObj, FreeTableProc, NULL,
};

static Blt_CustomOption titlesOption = {
    ObjToRowColumnFlag, RowColumnFlagToObj, NULL, (ClientData)SHOW_TITLES
};

static Blt_OptionParseProc ObjToCachedObj;
static Blt_OptionPrintProc CachedObjToObj;
static Blt_OptionFreeProc FreeCachedObjProc;
static Blt_CustomOption cachedObjOption = {
    ObjToCachedObj, CachedObjToObj, FreeCachedObjProc, NULL,
};

static Blt_OptionParseProc ObjToHide;
static Blt_OptionPrintProc HideToObj;
static Blt_CustomOption hideOption = {
    ObjToHide, HideToObj, NULL, (ClientData)0
};

static Blt_OptionParseProc ObjToShow;
static Blt_OptionPrintProc ShowToObj;
static Blt_CustomOption showOption = {
    ObjToShow, ShowToObj, NULL, (ClientData)0
};

static Blt_ConfigSpec tableSpecs[] =
{
    {BLT_CONFIG_BACKGROUND, "-activecolumntitlebackground", 
        "activeColumnTitleBackground", "ActiveTitleBackground", 
        DEF_ACTIVE_TITLE_BG, Blt_Offset(TableView, columns.activeTitleBg), 0},
    {BLT_CONFIG_COLOR, "-activecolumntitleforeground", 
        "activeColumnTitleForeground", "ActiveTitleForeground", 
        DEF_ACTIVE_TITLE_FG, Blt_Offset(TableView, columns.activeTitleFg), 0},
    {BLT_CONFIG_BACKGROUND, "-activerowtitlebackground", 
        "activeRowTitleBackground", "ActiveTitleBackground", 
        DEF_ACTIVE_TITLE_BG, Blt_Offset(TableView, rows.activeTitleBg), 0},
    {BLT_CONFIG_COLOR, "-activerowtitleforeground",
        "activeRowTitleForeground", "ActiveTitleForeground", 
        DEF_ACTIVE_TITLE_FG, Blt_Offset(TableView, rows.activeTitleFg), 0},
    {BLT_CONFIG_CUSTOM, "-autocreate", "autoCreate", "AutoCreate",
        DEF_AUTO_CREATE, Blt_Offset(TableView, flags), 
        BLT_CONFIG_DONT_SET_DEFAULT, &autoCreateOption},
    {BLT_CONFIG_BITMASK, "-columnfilters", "columnFilters", "ColumnFilters",
        DEF_COLUMN_FILTERS, Blt_Offset(TableView, flags), 
        BLT_CONFIG_DONT_SET_DEFAULT, (Blt_CustomOption *)COLUMN_FILTERS},
    {BLT_CONFIG_BACKGROUND, "-background", "background", "Background", 
        DEF_BACKGROUND, Blt_Offset(TableView, bg), 0},
    {BLT_CONFIG_SYNONYM, "-bd", "borderWidth"},
    {BLT_CONFIG_SYNONYM, "-bg", "background"},
    {BLT_CONFIG_PIXELS_NNEG, "-borderwidth", "borderWidth", "BorderWidth",
        DEF_BORDERWIDTH, Blt_Offset(TableView, borderWidth), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_OBJ, "-columncommand", "columnCommand", "ColumnCommand", 
        DEF_COLUMN_COMMAND, Blt_Offset(TableView, columns.cmdObjPtr),
        BLT_CONFIG_DONT_SET_DEFAULT | BLT_CONFIG_NULL_OK}, 
    {BLT_CONFIG_CURSOR, "-columnresizecursor", "columnResizeCursor", 
        "ResizeCursor", DEF_COLUMN_RESIZE_CURSOR, 
        Blt_Offset(TableView, columns.resizeCursor), 0},
    {BLT_CONFIG_BACKGROUND, "-columntitlebackground", "columnTitleBackground",
        "TitleBackground", DEF_COLUMN_NORMAL_TITLE_BG, 
        Blt_Offset(TableView, columns.normalTitleBg), 0},
    {BLT_CONFIG_PIXELS_NNEG, "-columntitleborderwidth", 
        "columnTitleBorderWidth", "TitleBorderWidth", 
        DEF_COLUMN_TITLE_BORDERWIDTH, 
     Blt_Offset(TableView, columns.titleBorderWidth),
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_FONT, "-columntitlefont", "columnTitleFont", "TitleFont", 
        DEF_COLUMN_TITLE_FONT, Blt_Offset(TableView, columns.titleFont), 0},
    {BLT_CONFIG_COLOR, "-columntitleforeground", "columnTitleForeground", 
        "TitleForeground", DEF_COLUMN_NORMAL_TITLE_FG, 
        Blt_Offset(TableView, columns.normalTitleFg), 0},
    {BLT_CONFIG_ACTIVE_CURSOR, "-cursor", "cursor", "Cursor", (char *)NULL, 
        Blt_Offset(TableView, cursor), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_CUSTOM, "-decreasingicon", "decreasingIcon","DecreasingIcon", 
        DEF_SORT_DOWN_ICON, Blt_Offset(TableView, sort.down), 
        BLT_CONFIG_NULL_OK | BLT_CONFIG_DONT_SET_DEFAULT, &iconOption},
    {BLT_CONFIG_BACKGROUND, "-disabledcolumntitlebackground", 
        "disabledColumnTitleBackground", "DisabledTitleBackground", 
        DEF_DISABLED_TITLE_BG, Blt_Offset(TableView, columns.disabledTitleBg)},
    {BLT_CONFIG_COLOR, "-disabledcolumntitleforeground", 
        "disabledColumnTitleForeground", "DisabledTitleForeground", 
        DEF_DISABLED_TITLE_FG, Blt_Offset(TableView, columns.disabledTitleFg)},
    {BLT_CONFIG_BACKGROUND, "-disabledrowtitlebackground", 
        "disabledRowTitleBackground", "DisabledTitleBackground", 
        DEF_DISABLED_TITLE_BG, Blt_Offset(TableView, rows.disabledTitleBg)},
    {BLT_CONFIG_COLOR, "-disabledrowtitleforeground", 
        "disabledRowTitleForeground", "DisabledTitleForeground", 
        DEF_DISABLED_TITLE_FG, Blt_Offset(TableView, rows.disabledTitleFg)},
    {BLT_CONFIG_BITMASK, "-exportselection", "exportSelection", 
        "ExportSelection", DEF_EXPORT_SELECTION, Blt_Offset(TableView, flags),
        BLT_CONFIG_DONT_SET_DEFAULT, (Blt_CustomOption *)SELECT_EXPORT},
    {BLT_CONFIG_PIXELS_NNEG, "-height", "height", "Height", DEF_HEIGHT, 
        Blt_Offset(TableView, reqHeight), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_COLOR, "-highlightbackground", "highlightBackground",
        "HighlightBackground", DEF_FOCUS_HIGHLIGHT_BG, 
        Blt_Offset(TableView, highlightBgColor), 0},
    {BLT_CONFIG_COLOR, "-highlightcolor", "highlightColor", "HighlightColor",
        DEF_FOCUS_HIGHLIGHT_COLOR, Blt_Offset(TableView, highlightColor), 0},
    {BLT_CONFIG_PIXELS_NNEG, "-highlightthickness", "highlightThickness",
        "HighlightThickness", DEF_FOCUS_HIGHLIGHT_WIDTH, 
        Blt_Offset(TableView, highlightWidth), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_CUSTOM, "-increasingicon", "increaingIcon", "IncreasingIcon", 
        DEF_SORT_UP_ICON, Blt_Offset(TableView, sort.up), 
        BLT_CONFIG_NULL_OK | BLT_CONFIG_DONT_SET_DEFAULT, &iconOption},
    {BLT_CONFIG_PIXELS_NNEG, "-maxcolumnwidth", "maxColumnWidth", 
        "MaxColumnWidth", DEF_MAX_COLUMN_WIDTH, 
        Blt_Offset(TableView, columns.maxWidth), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_PIXELS_NNEG, "-maxrowheight", "maxRowHeight", "MaxRowHeight", 
        DEF_MAX_ROW_HEIGHT, Blt_Offset(TableView, rows.maxHeight), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_RELIEF, "-relief", "relief", "Relief", DEF_RELIEF, 
        Blt_Offset(TableView, relief), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_OBJ, "-rowcommand", "rowCommand", "RowCommand", 
        DEF_ROW_COMMAND, Blt_Offset(TableView, rows.cmdObjPtr),
        BLT_CONFIG_DONT_SET_DEFAULT | BLT_CONFIG_NULL_OK}, 
    {BLT_CONFIG_CURSOR, "-rowresizecursor", "rows.resizeCursor","ResizeCursor", 
        DEF_ROW_RESIZE_CURSOR, Blt_Offset(TableView, rows.resizeCursor), 0},
    {BLT_CONFIG_BACKGROUND, "-rowtitlebackground", "rowTitleBackground", 
        "TitleBackground", DEF_ROW_NORMAL_TITLE_BG, 
        Blt_Offset(TableView, rows.normalTitleBg), 0},
    {BLT_CONFIG_PIXELS_NNEG, "-rowtitleborderwidth", "rows.titleBorderWidth", 
        "TitleBorderWidth", DEF_ROW_TITLE_BORDERWIDTH, 
        Blt_Offset(TableView, rows.titleBorderWidth), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_FONT, "-rowtitlefont", "rows.titleFont", "TitleFont", 
        DEF_ROW_TITLE_FONT, Blt_Offset(TableView, rows.titleFont), 0},
    {BLT_CONFIG_COLOR, "-rowtitleforeground", "rowTitleForeground", 
        "RowTitleForeground", DEF_ROW_NORMAL_TITLE_FG, 
        Blt_Offset(TableView, rows.normalTitleFg), 0},
    {BLT_CONFIG_CUSTOM, "-scrollmode", "scrollMode", "ScrollMode",
        DEF_SCROLL_MODE, Blt_Offset(TableView, scrollMode),
        BLT_CONFIG_DONT_SET_DEFAULT, &scrollModeOption},
    {BLT_CONFIG_OBJ, "-selectcommand", "selectCommand", "selectCommand",
        (char *)NULL, Blt_Offset(TableView, selectCmdObjPtr), 
        BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_CUSTOM, "-selectmode", "selectMode", "SelectMode",
        DEF_SELECT_MODE, Blt_Offset(TableView, selectMode), 
        BLT_CONFIG_DONT_SET_DEFAULT, &selectModeOption},
    {BLT_CONFIG_CUSTOM, "-slide", "slide", "Slide", DEF_SLIDE, 
        Blt_Offset(TableView, flags), BLT_CONFIG_DONT_SET_DEFAULT, 
        (Blt_CustomOption *)&slideOption},
    {BLT_CONFIG_BITMASK, "-sortselection", "sortSelection", "SortSelection",
        DEF_SORT_SELECTION, Blt_Offset(TableView, flags), 
        BLT_CONFIG_DONT_SET_DEFAULT, (Blt_CustomOption *)SELECT_SORTED},
    {BLT_CONFIG_CUSTOM, "-style", "style", "Style", DEF_STYLE, 
        Blt_Offset(TableView, stylePtr), BLT_CONFIG_DONT_SET_DEFAULT, 
        &styleOption},
    {BLT_CONFIG_CUSTOM, "-table", "table", "Table", DEF_TABLE,
        Blt_Offset(TableView, table), BLT_CONFIG_NULL_OK, &tableOption},
    {BLT_CONFIG_STRING, "-takefocus", "takeFocus", "TakeFocus",
        DEF_TAKE_FOCUS, Blt_Offset(TableView, takeFocus), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_CUSTOM, "-titles", "titles", "Titles", DEF_TITLES, 
        Blt_Offset(TableView, flags), BLT_CONFIG_DONT_SET_DEFAULT, 
        (Blt_CustomOption *)&titlesOption},
    {BLT_CONFIG_PIXELS_NNEG, "-width", "width", "Width", DEF_WIDTH, 
        Blt_Offset(TableView, reqWidth), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_OBJ, "-xscrollcommand", "xScrollCommand", "ScrollCommand",
        (char *)NULL, Blt_Offset(TableView, columns.scrollCmdObjPtr), 
        BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_PIXELS_NNEG, "-xscrollincrement", "xScrollIncrement", 
        "ScrollIncrement", DEF_SCROLL_INCREMENT, 
        Blt_Offset(TableView, columns.scrollUnits), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_OBJ, "-yscrollcommand", "yScrollCommand", "ScrollCommand",
        (char *)NULL, Blt_Offset(TableView, rows.scrollCmdObjPtr), 
        BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_PIXELS_NNEG, "-yscrollincrement", "yScrollIncrement", 
        "ScrollIncrement", DEF_SCROLL_INCREMENT, 
        Blt_Offset(TableView, rows.scrollUnits), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_END, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, 
        0, 0}
};

static Blt_ConfigSpec columnSpecs[] =
{
    {BLT_CONFIG_RELIEF, "-activetitlerelief", "activeTitleRelief", 
        "TitleRelief", DEF_COLUMN_ACTIVE_TITLE_RELIEF, 
        Blt_Offset(Column, activeTitleRelief), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_SYNONYM, "-bd", "borderWidth"},
    {BLT_CONFIG_OBJ, "-bindtags", "bindTags", "BindTags", DEF_COLUMN_BIND_TAGS, 
        Blt_Offset(Column, bindTagsObjPtr), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_OBJ, "-command", "command", "Command", DEF_COLUMN_COMMAND, 
        Blt_Offset(Column, cmdObjPtr),
        BLT_CONFIG_DONT_SET_DEFAULT | BLT_CONFIG_NULL_OK}, 
    {BLT_CONFIG_BITMASK_INVERT, "-edit", "edit", "Edit", DEF_COLUMN_EDIT, 
        Blt_Offset(Column, flags), BLT_CONFIG_DONT_SET_DEFAULT, 
        (Blt_CustomOption *)EDIT},
    {BLT_CONFIG_OBJ, "-filterdata", "filterData", "FilterData", (char *)NULL,
        Blt_Offset(Column, filterDataObjPtr), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_FONT, "-filterfont", "filterFont", "FilterFont", 
        DEF_FILTER_FONT, Blt_Offset(Column, filterFont), 0},
    {BLT_CONFIG_CUSTOM, "-filtericon", "filterIcon", "FilterIcon",
        DEF_FILTER_ICON, Blt_Offset(Column, filterIcon), 
        BLT_CONFIG_NULL_OK | BLT_CONFIG_DONT_SET_DEFAULT, &iconOption},
    {BLT_CONFIG_OBJ, "-filtermenu", "filterMenu", "FilterMenu", 
        DEF_FILTER_MENU, Blt_Offset(Column, filterMenuObjPtr), 
        BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_STRING, "-filtertext", "filterText", "FilterText",
        DEF_FILTER_TEXT, Blt_Offset(Column, filterText), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_OBJ, "-formatcommand", "formatCommand", "FormatCommand", 
        DEF_COLUMN_FORMAT_COMMAND, Blt_Offset(Column, fmtCmdObjPtr), 0},
    {BLT_CONFIG_CUSTOM, "-hide", "hide", "Hide", DEF_COLUMN_HIDE, 
        Blt_Offset(Column, flags), BLT_CONFIG_DONT_SET_DEFAULT, &hideOption},
    {BLT_CONFIG_BITMASK, "-filterhighlight", "filterhighliht", 
        "FilterHighlight", DEF_FILTER_HIGHLIGHT, 
        Blt_Offset(Column, flags), BLT_CONFIG_DONT_SET_DEFAULT, 
        (Blt_CustomOption *)FILTERHIGHLIGHT},
    {BLT_CONFIG_CUSTOM, "-icon", "icon", "icon", DEF_COLUMN_ICON, 
        Blt_Offset(Column, icon), 
        BLT_CONFIG_NULL_OK | BLT_CONFIG_DONT_SET_DEFAULT, &iconOption},
    {BLT_CONFIG_CUSTOM, "-show", "show", "Show", DEF_COLUMN_SHOW, 
        Blt_Offset(Column, flags), BLT_CONFIG_DONT_SET_DEFAULT, &showOption},
    {BLT_CONFIG_PIXELS_NNEG, "-rulewidth", "ruleWidth", "RuleWidth",
        DEF_RULE_WIDTH, Blt_Offset(Column, ruleWidth), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_OBJ, "-sortcommand", "sortCommand", "SortCommand",
        DEF_SORT_COMMAND, Blt_Offset(Column, sortCmdObjPtr),
        BLT_CONFIG_DONT_SET_DEFAULT | BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_CUSTOM, "-sortmode", "sortMode", "SortMode", DEF_SORT_TYPE, 
        Blt_Offset(Column, sortType), BLT_CONFIG_DONT_SET_DEFAULT, &typeOption},
    {BLT_CONFIG_CUSTOM, "-state", "state", "State", DEF_COLUMN_STATE, 
        Blt_Offset(Column, flags), BLT_CONFIG_DONT_SET_DEFAULT, 
        &stateOption},
    {BLT_CONFIG_CUSTOM, "-style", "style", "Style", DEF_COLUMN_STYLE, 
        Blt_Offset(Column, stylePtr), BLT_CONFIG_NULL_OK, &styleOption},
    {BLT_CONFIG_OBJ, "-title", "title", "Title", (char *)NULL, 
        Blt_Offset(Column, titleObjPtr), 
        BLT_CONFIG_NULL_OK | BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_JUSTIFY, "-titlejustify", "titleJustify", "TitleJustify", 
        DEF_COLUMN_TITLE_JUSTIFY, Blt_Offset(Column, titleJustify), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_RELIEF, "-titlerelief", "titleRelief", "TitleRelief", 
        DEF_COLUMN_TITLE_RELIEF, Blt_Offset(Column, titleRelief), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_DOUBLE, "-weight", "weight", "Weight", DEF_COLUMN_WEIGHT, 
        Blt_Offset(Column, weight), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_CUSTOM, "-width", "width", "Width", DEF_COLUMN_WIDTH, 
        Blt_Offset(Column, reqWidth), BLT_CONFIG_DONT_SET_DEFAULT, 
        &limitsOption},
    {BLT_CONFIG_END, (char *)NULL, (char *)NULL, (char *)NULL,
        (char *)NULL, 0, 0}
};

static Blt_ConfigSpec cellSpecs[] =
{
    {BLT_CONFIG_CUSTOM, "-state", "state", "State", DEF_CELL_STATE, 
        Blt_Offset(Cell, flags), BLT_CONFIG_DONT_SET_DEFAULT, 
        &cellStateOption},
    {BLT_CONFIG_CUSTOM, "-style", "style", "Style", DEF_CELL_STYLE, 
        Blt_Offset(Cell, stylePtr), BLT_CONFIG_NULL_OK, &styleOption},
    {BLT_CONFIG_END, NULL, NULL, NULL, NULL, 0, 0}
};

static Blt_ConfigSpec rowSpecs[] =
{
    {BLT_CONFIG_RELIEF, "-activetitlerelief", "activeTitleRelief", 
        "TitleRelief", DEF_ROW_ACTIVE_TITLE_RELIEF, 
        Blt_Offset(Row, activeTitleRelief), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_SYNONYM, "-bd", "borderWidth"},
    {BLT_CONFIG_OBJ, "-bindtags", "bindTags", "BindTags", DEF_ROW_BIND_TAGS, 
        Blt_Offset(Row, bindTagsObjPtr), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_STRING, "-command", "command", "Command", DEF_ROW_COMMAND, 
        Blt_Offset(Row, cmdObjPtr), 
        BLT_CONFIG_DONT_SET_DEFAULT | BLT_CONFIG_NULL_OK}, 
    {BLT_CONFIG_BITMASK_INVERT, "-edit", "edit", "Edit", DEF_ROW_EDIT, 
        Blt_Offset(Row, flags), BLT_CONFIG_DONT_SET_DEFAULT, 
        (Blt_CustomOption *)EDIT},
    {BLT_CONFIG_CUSTOM, "-height", "height", "Height", DEF_ROW_HEIGHT, 
        Blt_Offset(Row, reqHeight), BLT_CONFIG_DONT_SET_DEFAULT, 
        &limitsOption},
    {BLT_CONFIG_CUSTOM, "-hide", "hide", "Hide", DEF_ROW_HIDE, 
        Blt_Offset(Row, flags), BLT_CONFIG_DONT_SET_DEFAULT, &hideOption},
    {BLT_CONFIG_CUSTOM, "-icon", "icon", "icon", DEF_ROW_ICON, 
        Blt_Offset(Row, icon), 
        BLT_CONFIG_NULL_OK | BLT_CONFIG_DONT_SET_DEFAULT, &iconOption},
    {BLT_CONFIG_PIXELS_NNEG, "-ruleheight", "ruleHeight", "RuleHeight",
        DEF_RULE_HEIGHT, Blt_Offset(Row, ruleHeight), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_CUSTOM, "-show", "show", "Show", DEF_ROW_SHOW, 
        Blt_Offset(Row, flags), BLT_CONFIG_DONT_SET_DEFAULT, &showOption},
    {BLT_CONFIG_CUSTOM, "-state", "state", "State", DEF_ROW_STATE, 
        Blt_Offset(Row, flags), BLT_CONFIG_DONT_SET_DEFAULT, &stateOption},
    {BLT_CONFIG_CUSTOM, "-style", "style", "Style", DEF_ROW_STYLE, 
        Blt_Offset(Row, stylePtr), BLT_CONFIG_NULL_OK, &styleOption},
    {BLT_CONFIG_OBJ, "-title", "title", "Title", (char *)NULL, 
        Blt_Offset(Row, titleObjPtr), 
        BLT_CONFIG_NULL_OK | BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_JUSTIFY, "-titlejustify", "titleJustify", "TitleJustify", 
        DEF_ROW_TITLE_JUSTIFY, Blt_Offset(Row, titleJustify), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_RELIEF, "-titlerelief", "titleRelief", "TitleRelief",
        DEF_ROW_TITLE_RELIEF, Blt_Offset(Row, titleRelief), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_DOUBLE, "-weight", "weight", "Weight", DEF_ROW_WEIGHT, 
        Blt_Offset(Row, weight), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_END, (char *)NULL, (char *)NULL, (char *)NULL,
        (char *)NULL, 0, 0}
};

static Blt_ConfigSpec sortSpecs[] =
{
    {BLT_CONFIG_CUSTOM, "-columns", "columns", "columns",
        DEF_SORT_COLUMNS, Blt_Offset(TableView, sort.order),
        BLT_CONFIG_NULL_OK | BLT_CONFIG_DONT_SET_DEFAULT, &sortOrderOption},
    {BLT_CONFIG_BOOLEAN, "-decreasing", "decreasing", "Decreasing",
        DEF_SORT_DECREASING, Blt_Offset(TableView, sort.decreasing),
        BLT_CONFIG_DONT_SET_DEFAULT}, 
    {BLT_CONFIG_CUSTOM, "-mark", "mark", "mark",
        DEF_SORT_COLUMN, Blt_Offset(TableView, sort.firstPtr),
        BLT_CONFIG_NULL_OK | BLT_CONFIG_DONT_SET_DEFAULT, &sortColumnOption},
    {BLT_CONFIG_END, (char *)NULL, (char *)NULL, (char *)NULL,
        (char *)NULL, 0, 0}
};

/* 
 * Some of the configuration options here are holders for TCL code to use.
 */
static Blt_ConfigSpec filterSpecs[] =
{
    {BLT_CONFIG_BACKGROUND, "-activebackground", "activeBackground", 
        "ActiveBackground", DEF_FILTER_ACTIVE_BG, 
        Blt_Offset(TableView, filter.activeBg), 0},
    {BLT_CONFIG_COLOR, "-activeforeground", "activeForeground", 
        "ActiveForeground", DEF_FILTER_ACTIVE_FG, 
        Blt_Offset(TableView, filter.activeFg), 0},
    {BLT_CONFIG_RELIEF, "-activerelief", "activeRelief", "ActiveRelief", 
        DEF_FILTER_ACTIVE_RELIEF, Blt_Offset(TableView, filter.activeRelief), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_BACKGROUND, "-background", "background", "Background", 
        DEF_FILTER_NORMAL_BG, Blt_Offset(TableView, filter.normalBg), 0},
    {BLT_CONFIG_SYNONYM, "-bd", "borderWidth"},
    {BLT_CONFIG_SYNONYM, "-bg", "background"},
    {BLT_CONFIG_PIXELS_NNEG, "-borderwidth", "borderWidth", "BorderWidth", 
        DEF_FILTER_BORDERWIDTH, Blt_Offset(TableView, filter.borderWidth), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_PIXELS_NNEG, "-borderwidth", "borderWidth", "BorderWidth",
        DEF_BORDERWIDTH, Blt_Offset(TableView, filter.borderWidth), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_BACKGROUND, "-disabledbackground", "disabledBackground", 
        "DisabledBackground", DEF_FILTER_DISABLED_BG, 
        Blt_Offset(TableView, filter.disabledBg), 0},
    {BLT_CONFIG_COLOR, "-disabledforeground", "disabledForeground", 
        "DisabledForeground", DEF_FILTER_DISABLED_FG, 
        Blt_Offset(TableView, filter.disabledFg), 0},
    {BLT_CONFIG_SYNONYM, "-fg", "foreground"},
    {BLT_CONFIG_FONT, "-font", "font", "Font", DEF_FILTER_FONT, 
        Blt_Offset(TableView, filter.font), 0},
    {BLT_CONFIG_COLOR, "-foreground", "foreground", "Foreground", 
        DEF_FILTER_NORMAL_FG, Blt_Offset(TableView, filter.normalFg), 0},
    {BLT_CONFIG_BITMASK, "-hide", "hide", "Hide",
        DEF_COLUMN_FILTERS, Blt_Offset(TableView, flags), 
        BLT_CONFIG_DONT_SET_DEFAULT, (Blt_CustomOption *)COLUMN_FILTERS},
    {BLT_CONFIG_BACKGROUND, "-highlightbackground", "highlightBackground", 
        "HighlightBackground", DEF_FILTER_HIGHLIGHT_BG, 
        Blt_Offset(TableView, filter.highlightBg), 0},
    {BLT_CONFIG_COLOR, "-highlightforeground", "highlightForeground", 
        "HighlightForeground", DEF_FILTER_HIGHLIGHT_FG, 
        Blt_Offset(TableView, filter.highlightFg), 0},
    {BLT_CONFIG_OBJ, "-menu", "menu", "Menu", DEF_FILTER_MENU,
        Blt_Offset(TableView, filter.menuObjPtr), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_RELIEF, "-postedrelief", "postedRelief", "PostedRelief", 
        DEF_FILTER_SELECT_RELIEF, Blt_Offset(TableView, filter.selectRelief), 
        BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_RELIEF, "-relief", "relief", "Relief", DEF_FILTER_RELIEF, 
        Blt_Offset(TableView, filter.relief), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_BITMASK_INVERT, "-show", "show", "Show", DEF_FILTER_SHOW, 
        Blt_Offset(TableView, flags), BLT_CONFIG_DONT_SET_DEFAULT,
        (Blt_CustomOption *)COLUMN_FILTERS},
    {BLT_CONFIG_BACKGROUND, "-selectbackground", "selectBackground", 
        "SelectBackground", DEF_FILTER_SELECT_BG, 
        Blt_Offset(TableView, filter.selectBg), 0},
    {BLT_CONFIG_COLOR, "-selectforeground", "selectForeground", 
        "SelectForeground", DEF_FILTER_SELECT_FG, 
        Blt_Offset(TableView, filter.selectFg), 0},
    {BLT_CONFIG_END, (char *)NULL, (char *)NULL, (char *)NULL, (char *)NULL, 
        0, 0}
};

typedef struct {
    unsigned int flags;
} BBoxSwitches;

#define BBOX_ROOT     (1<<0)

static Blt_SwitchSpec bboxSwitches[] = 
{
    {BLT_SWITCH_BITS_NOARG, "-root", "", (char *)NULL,
        Blt_Offset(BBoxSwitches, flags), 0, BBOX_ROOT},
    {BLT_SWITCH_END}
};

typedef struct {
    unsigned int flags;
} IdentifySwitches;

#define IDENTIFY_ROOT     (1<<0)

static Blt_SwitchSpec identifySwitches[] = 
{
    {BLT_SWITCH_BITS_NOARG, "-root", "", (char *)NULL,
        Blt_Offset(IdentifySwitches, flags), 0, IDENTIFY_ROOT},
    {BLT_SWITCH_END}
};

typedef struct {
    unsigned int flags;
} NearestSwitches;

#define NEAREST_ROOT     (1<<0)

static Blt_SwitchSpec nearestSwitches[] = 
{
    {BLT_SWITCH_BITS_NOARG, "-root", "", (char *)NULL,
        Blt_Offset(NearestSwitches, flags), 0, NEAREST_ROOT},
    {BLT_SWITCH_END}
};

static Blt_HashTable findTable;
static int initialized = FALSE;

/* Forward Declarations */
static BLT_TABLE_NOTIFY_EVENT_PROC TableEventProc;
static Blt_BindPickProc TableViewPickProc;
static Blt_BindAppendTagsProc AppendTagsProc;
static Tcl_CmdDeleteProc TableViewInstCmdDeleteProc;
static Tcl_FreeProc TableViewFreeProc;
static Tcl_FreeProc RowFreeProc;
static Tcl_FreeProc ColumnFreeProc;
static Tcl_FreeProc FreeCellProc;
static Tcl_IdleProc DisplayProc;
static Tcl_IdleProc DisplayColumnTitlesProc;
static Tcl_ObjCmdProc TableViewCmdProc;
static Tcl_ObjCmdProc TableViewInstObjCmdProc;
static Tk_EventProc TableViewEventProc;
static Tk_ImageChangedProc IconChangedProc;
static Tk_SelectionProc SelectionProc;

/*
 *---------------------------------------------------------------------------
 *
 * EventuallyRedraw --
 *
 *      Queues a request to redraw the widget at the next idle point.  A
 *      new idle event procedure is queued only if the there's isn't one
 *      already queued and updates are turned on.
 *
 *      The DONT_UPDATE flag lets the user to turn off redrawing the
 *      tableview while changes are happening to the table itself.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Information gets redisplayed.  Right now we don't do selective
 *      redisplays:  the whole window will be redrawn.
 *
 *---------------------------------------------------------------------------
 */
static void
EventuallyRedraw(TableView *viewPtr)
{
    viewPtr->flags |= REDRAW;
    if ((viewPtr->tkwin != NULL) && 
        ((viewPtr->flags & (DONT_UPDATE|REDRAW_PENDING)) == 0)) {
        viewPtr->flags |= REDRAW_PENDING;
        Tcl_DoWhenIdle(DisplayProc, viewPtr);
    }
}

void
Blt_TableView_EventuallyRedraw(TableView *viewPtr)
{
    EventuallyRedraw(viewPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * PossiblyRedraw --
 *
 *      Queues a request to redraw the widget at the next idle point.  A
 *      new idle event procedure is queued only if the there's isn't one
 *      already queued and updates are turned on.
 *
 *      The DONT_UPDATE flag lets the user to turn off redrawing the
 *      tableview while changes are happening to the table itself.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Information gets redisplayed.  Right now we don't do selective
 *      redisplays:  the whole window will be redrawn.
 *
 *---------------------------------------------------------------------------
 */
static void
PossiblyRedraw(TableView *viewPtr)
{
    if ((viewPtr->tkwin != NULL) && 
        ((viewPtr->flags & (DONT_UPDATE|REDRAW_PENDING)) == 0)) {
        viewPtr->flags |= REDRAW_PENDING;
        Tcl_DoWhenIdle(DisplayProc, viewPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * EventuallyRedrawColumnTitles --
 *
 *      Queues a request to redraw the column titles at the next idle
 *      point.  A new idle event procedure is queued only if the there's
 *      isn't one already queued and updates are turned on.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Information gets redisplayed.  Only the column titles are
 *      redisplayed.
 *
 *---------------------------------------------------------------------------
 */
static void
EventuallyRedrawColumnTitles(TableView *viewPtr)
{
    viewPtr->flags |= REDRAW;
    if ((viewPtr->tkwin != NULL) && 
        ((viewPtr->columns.flags & REDRAW_PENDING) == 0)) {
        viewPtr->columns.flags |= REDRAW_PENDING;
        Tcl_DoWhenIdle(DisplayColumnTitlesProc, viewPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * GetRowIndexObj --
 *
 *      Returns a TCL obj (long) of the row's index.
 *
 *---------------------------------------------------------------------------
 */
static Tcl_Obj *
GetRowIndexObj(TableView *viewPtr, Row *rowPtr) 
{
    long index;

    index = blt_table_row_index(viewPtr->table, rowPtr->row);
    return Tcl_NewLongObj(index);
}

/*
 *---------------------------------------------------------------------------
 *
 * GetColumnIndexObj --
 *
 *      Returns a TCL obj (long) of the column's index.
 *
 *---------------------------------------------------------------------------
 */
static Tcl_Obj *
GetColumnIndexObj(TableView *viewPtr, Column *colPtr) 
{
    long index;

    index = blt_table_column_index(viewPtr->table, colPtr->column);
    return Tcl_NewLongObj(index);
}

/*
 *---------------------------------------------------------------------------
 *
 * GetColumnTitle --
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
INLINE static const char *
GetColumnTitle(Column *colPtr)
{
    return (colPtr->titleObjPtr == NULL) ?
        blt_table_column_label(colPtr->column) :
        Tcl_GetString(colPtr->titleObjPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * GetRowTitle --
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
INLINE static const char *
GetRowTitle(Row *rowPtr)
{
    return (rowPtr->titleObjPtr == NULL) ?
        blt_table_column_label(rowPtr->row) :
        Tcl_GetString(rowPtr->titleObjPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * RethreadRows --
 *
 *      Rethreads the list of rows according to the current row map.  This
 *      is done after the rows are sorted and the list needs to reflect the
 *      reordering in the map.
 *
 *---------------------------------------------------------------------------
 */
static void
RethreadRows(TableView *viewPtr) 
{
    Row *prevPtr, *rowPtr;
    size_t i;
    
    /* Relink the first N-1 rows. */
    prevPtr = NULL;
    for (i = 0; i < (viewPtr->rows.length - 1); i++) {
        Row *rowPtr;
        
        rowPtr = viewPtr->rows.map[i];
        rowPtr->index = i;
        rowPtr->prevPtr = prevPtr;
        rowPtr->nextPtr = viewPtr->rows.map[i+1];
        prevPtr = rowPtr;
    }
    /* Relink the last row. */
    rowPtr = viewPtr->rows.map[i];
    rowPtr->index = i;
    rowPtr->prevPtr = prevPtr;
    rowPtr->nextPtr = NULL;
    /* Reset the head and tail. */
    viewPtr->rows.lastPtr = rowPtr;
    viewPtr->rows.firstPtr = viewPtr->rows.map[0];
}

/*
 *---------------------------------------------------------------------------
 *
 * RethreadColumns --
 *
 *      Rethreads the list of columns according to the current row map.
 *      This is done after the columns are sorted and the list needs to
 *      reflect the reordering in the map.
 *
 *---------------------------------------------------------------------------
 */
static void
RethreadColumns(TableView *viewPtr) 
{
    Column *prevPtr, *colPtr;
    size_t i;
    
    /* Relink the first N-1 columns. */
    prevPtr = NULL;
    for (i = 0; i < (viewPtr->columns.length - 1); i++) {
        Column *colPtr;
        
        colPtr = viewPtr->columns.map[i];
        colPtr->index = i;
        colPtr->prevPtr = prevPtr;
        colPtr->nextPtr = viewPtr->columns.map[i+1];
        prevPtr = colPtr;
    }
    /* Relink the last column. */
    colPtr = viewPtr->columns.map[i];
    colPtr->index = i;
    colPtr->prevPtr = prevPtr;
    colPtr->nextPtr = NULL;
    /* Reset the head and tail. */
    viewPtr->columns.lastPtr = colPtr;
    viewPtr->columns.firstPtr = viewPtr->columns.map[0];
}


/*
 *---------------------------------------------------------------------------
 *
 * RenumberRows --
 *
 *      Reindexes the rows according to their current location in the row
 *      list.  This is reflected in both the row map and in the index in
 *      the individual row structure.
 *
 *---------------------------------------------------------------------------
 */
static void
RenumberRows(TableView *viewPtr) 
{
    Row *rowPtr;
    size_t i, j;

    /* If the sizes are different reallocate the row map. */
    if (viewPtr->rows.numAllocated != viewPtr->rows.length) {
        Row **map;

        if (viewPtr->rows.length == 0) {
            if (viewPtr->rows.map != NULL) {
                Blt_Free(viewPtr->rows.map);
            }
            map = NULL;
        } else {
            map = Blt_AssertRealloc(viewPtr->rows.map, 
                                    viewPtr->rows.length * sizeof(Row *));
        }
        viewPtr->rows.map = map;
        viewPtr->rows.numAllocated = viewPtr->rows.length;
    } 
    /* Reset the row map and reindex the rows. */
    for (i = 0, j = 0, rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL;
         rowPtr = rowPtr->nextPtr, i++) {
        rowPtr->index = i;
        if ((rowPtr->flags & HIDDEN) == 0) {
            viewPtr->rows.map[j] = rowPtr;
            rowPtr->visibleIndex = j;
            j++;
        }
    }
    viewPtr->rows.numMapped = j;
    assert(i == viewPtr->rows.length);
    viewPtr->rows.flags &= ~REINDEX;
}

/*
 *---------------------------------------------------------------------------
 *
 * RenumberColumns --
 *
 *      Reindexes the columns according to their current location in the
 *      column list.  This is reflected in both the column map and in the
 *      index in the individual column structure.
 *
 *---------------------------------------------------------------------------
 */
static void
RenumberColumns(TableView *viewPtr) 
{
    Column *colPtr;
    long i, j;

    /* If the sizes are different reallocate the column map. */
    if (viewPtr->columns.numAllocated != viewPtr->columns.length) {
        Column **map;

        if (viewPtr->columns.length == 0) {
            if (viewPtr->columns.map != NULL) {
                Blt_Free(viewPtr->columns.map);
            }
            map = NULL;
        } else {
            map = Blt_AssertRealloc(viewPtr->columns.map, 
                                    viewPtr->columns.length * sizeof(Column *));
        }
        viewPtr->columns.map = map;
        viewPtr->columns.numAllocated = viewPtr->columns.length;
    } 
    /* Reset the column map and reindex the columns. */
    for (i = 0, j = 0, colPtr = viewPtr->columns.firstPtr; colPtr != NULL;
         colPtr = colPtr->nextPtr, i++) {
        colPtr->index = i;
        if ((colPtr->flags & HIDDEN) == 0) {
            viewPtr->columns.map[j] = colPtr;
            colPtr->visibleIndex = j;
            j++;
        }
    }
    viewPtr->columns.numMapped = j;
    assert(i == viewPtr->columns.length);
    viewPtr->columns.flags &= ~REINDEX;
}

static Row *
GetRowContainer(TableView *viewPtr, BLT_TABLE_ROW row)
{
    Blt_HashEntry *hPtr;

    hPtr = Blt_FindHashEntry(&viewPtr->rows.table, (char *)row);
    if (hPtr == NULL) {
        return NULL;
    }
    return Blt_GetHashValue(hPtr);
}


static Column *
GetColumnContainer(TableView *viewPtr, BLT_TABLE_COLUMN col)
{
    Blt_HashEntry *hPtr;

    hPtr = Blt_FindHashEntry(&viewPtr->columns.table, (char *)col);
    if (hPtr == NULL) {
        return NULL;
    }
    return Blt_GetHashValue(hPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * MoveRows --
 *
 *      Moves one or more row.
 *
 *---------------------------------------------------------------------------
 */
static void
MoveRows(TableView *viewPtr, Row *destPtr, Row *firstPtr, Row *lastPtr, 
         int after) 
{
    assert (firstPtr->index <= lastPtr->index);
    /* Unlink the sub-list from the list of rows. */
    if (viewPtr->rows.firstPtr == firstPtr) {
        viewPtr->rows.firstPtr = lastPtr->nextPtr;
        lastPtr->nextPtr->prevPtr = NULL;
    } else {
        firstPtr->prevPtr->nextPtr = lastPtr->nextPtr;
    }
    if (viewPtr->rows.lastPtr == lastPtr) {
        viewPtr->rows.lastPtr = firstPtr->prevPtr;
        firstPtr->prevPtr->nextPtr = NULL;
    } else {
        lastPtr->nextPtr->prevPtr = firstPtr->prevPtr;
    }
    firstPtr->prevPtr = lastPtr->nextPtr = NULL;

    /* Now attach the detached list to the destination. */
    if (after) { 
        /* [a]->[dest]->[b] */
        /*            [first]->[last] */
        if (destPtr->nextPtr == NULL) {
            assert(destPtr == viewPtr->rows.lastPtr);
            viewPtr->rows.lastPtr = lastPtr; /* Append to the end. */
        } else {
            destPtr->nextPtr->prevPtr = lastPtr;
        }
        lastPtr->nextPtr = destPtr->nextPtr;
        destPtr->nextPtr = firstPtr;
        firstPtr->prevPtr = destPtr;
    } else {
        /*           [a]->[dest]->[b] */
        /* [first]->[last] */
        if (destPtr->prevPtr == NULL) {
            viewPtr->rows.firstPtr = firstPtr;
        } else {
            destPtr->prevPtr->nextPtr = firstPtr;
        }
        firstPtr->prevPtr = destPtr->prevPtr;
        destPtr->prevPtr = lastPtr;
        lastPtr->nextPtr = destPtr;
    }
    /* FIXME: You don't have to reset the entire map. */
    RenumberRows(viewPtr);
    /* FIXME: Layout changes with move but not geometry. */
    viewPtr->flags |= GEOMETRY;
    EventuallyRedraw(viewPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * MoveColumns --
 *
 *      Moves one or more columns.
 *
 *---------------------------------------------------------------------------
 */
static void
MoveColumns(TableView *viewPtr, Column *destPtr, Column *firstPtr, 
            Column *lastPtr, int after) 
{
    assert (firstPtr->index <= lastPtr->index);
    /* Unlink the sub-list from the list of columns. */
    if (viewPtr->columns.firstPtr == firstPtr) {
        viewPtr->columns.firstPtr = lastPtr->nextPtr;
        lastPtr->nextPtr->prevPtr = NULL;
    } else {
        firstPtr->prevPtr->nextPtr = lastPtr->nextPtr;
    }
    if (viewPtr->columns.lastPtr == lastPtr) {
        viewPtr->columns.lastPtr = firstPtr->prevPtr;
        firstPtr->prevPtr->nextPtr = NULL;
    } else {
        lastPtr->nextPtr->prevPtr = firstPtr->prevPtr;
    }
    firstPtr->prevPtr = lastPtr->nextPtr = NULL;

    /* Now attach the detached list to the destination. */
    if (after) { 
        /* [a]->[dest]->[b] */
        /*            [first]->[last] */
        if (destPtr->nextPtr == NULL) {
            assert(destPtr == viewPtr->columns.lastPtr);
            viewPtr->columns.lastPtr = lastPtr; /* Append to the end. */
        } else {
            destPtr->nextPtr->prevPtr = lastPtr;
        }
        lastPtr->nextPtr = destPtr->nextPtr;
        destPtr->nextPtr = firstPtr;
        firstPtr->prevPtr = destPtr;
    } else {
        /*           [a]->[dest]->[b] */
        /* [first]->[last] */
        if (destPtr->prevPtr == NULL) {
            viewPtr->columns.firstPtr = firstPtr;
        } else {
            destPtr->prevPtr->nextPtr = firstPtr;
        }
        firstPtr->prevPtr = destPtr->prevPtr;
        destPtr->prevPtr = lastPtr;
        lastPtr->nextPtr = destPtr;
    }
    /* FIXME: You don't have to reset the entire map. */
    RenumberColumns(viewPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * SelectCommandProc --
 *
 *      Invoked at the next idle point whenever the current selection
 *      changes.  Executes some application-specific code in the
 *      -selectcommand option.  This provides a way for applications to
 *      handle selection changes.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      TCL code gets executed for some application-specific task.
 *
 *---------------------------------------------------------------------------
 */
static void
SelectCommandProc(ClientData clientData) 
{
    TableView *viewPtr = clientData;
    Tcl_Obj *cmdObjPtr;

    viewPtr->flags &= ~SELECT_PENDING;
    cmdObjPtr = viewPtr->selectCmdObjPtr;
    if (cmdObjPtr != NULL) {
        Tcl_Preserve(viewPtr);
        if (Tcl_EvalObjEx(viewPtr->interp, cmdObjPtr, TCL_EVAL_GLOBAL) 
            != TCL_OK) {
            Tcl_BackgroundError(viewPtr->interp);
        }
        Tcl_Release(viewPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * EventuallyInvokeSelectCommand --
 *
 *      Queues a request to execute the -selectcommand code associated with
 *      the widget at the next idle point.  Invoked whenever the selection
 *      changes.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      TCL code gets executed for some application-specific task.
 *
 *---------------------------------------------------------------------------
 */
static void
EventuallyInvokeSelectCommand(TableView *viewPtr)
{
    if ((viewPtr->flags & SELECT_PENDING) == 0) {
        viewPtr->flags |= SELECT_PENDING;
        Tcl_DoWhenIdle(SelectCommandProc, viewPtr);
    }
}


/*
 *---------------------------------------------------------------------------
 *
 * SelectRow --
 *
 *      Adds the given row from the set of selected rows.  It's OK if the
 *      row has already been selected.
 *
 *---------------------------------------------------------------------------
 */
static void
SelectRow(TableView *viewPtr, Row *rowPtr)
{
    if ((rowPtr->flags & SELECTED) == 0) {
        RowSelection *selectPtr = &viewPtr->rows.selection;

        rowPtr->flags |= SELECTED;
        rowPtr->link = Blt_Chain_Append(selectPtr->list, rowPtr);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * DeselectRow --
 *
 *      Removes the given row from the set of selected rows. It's OK
 *      if the row isn't already selected.
 *
 *---------------------------------------------------------------------------
 */
static void
DeselectRow(TableView *viewPtr, Row *rowPtr)
{
    RowSelection *selectPtr = &viewPtr->rows.selection;

    rowPtr->flags &= ~SELECTED;
    Blt_Chain_DeleteLink(selectPtr->list, rowPtr->link);
}

static void
ClearSelections(TableView *viewPtr)
{
    switch (viewPtr->selectMode) {
    case SELECT_CELLS:
        if (viewPtr->selectCells.cellTable.numEntries > 0) {
            Blt_DeleteHashTable(&viewPtr->selectCells.cellTable);
            Blt_InitHashTable(&viewPtr->selectCells.cellTable,
                              sizeof(CellKey)/sizeof(int));
        }
        break;
    case SELECT_SINGLE_ROW:
    case SELECT_MULTIPLE_ROWS:
        {
            Blt_ChainLink link, next;

            for (link = Blt_Chain_FirstLink(viewPtr->rows.selection.list); 
                 link != NULL; link = next) {
                Row *rowPtr;
                
                next = Blt_Chain_NextLink(link);
                rowPtr = Blt_Chain_GetValue(link);
                DeselectRow(viewPtr, rowPtr);
            }
        }
        break;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * GetColumnXOffset --
 *
 *      Computes the closest X offset the the put the column in view.
 *      If the column is already in view the old X offset is returned.
 *
 * Results:
 *      Returns the X offset to view the column.
 *
 *---------------------------------------------------------------------------
 */
static long
GetColumnXOffset(TableView *viewPtr, Column *colPtr)
{
    long xOffset;

    xOffset = viewPtr->columns.scrollOffset;
    if (colPtr->worldX < viewPtr->columns.scrollOffset) {
        xOffset = colPtr->worldX;
    }
    if ((colPtr->worldX + colPtr->width) >= 
        (viewPtr->columns.scrollOffset + VPORTWIDTH(viewPtr))) {
        xOffset = (colPtr->worldX + colPtr->width) - VPORTWIDTH(viewPtr);
    } 
    if (xOffset < 0) {
        xOffset = 0;
    }
    return xOffset;
}

/*
 *---------------------------------------------------------------------------
 *
 * GetRowYOffset --
 *
 *      Computes the closest Y offset the the put the row in view.
 *      If the row is already in view the old Y offset is returned.
 *
 * Results:
 *      Returns the Y offset to view the row.
 *
 *---------------------------------------------------------------------------
 */
static long
GetRowYOffset(TableView *viewPtr, Row *rowPtr)
{
    long yOffset;

    yOffset = viewPtr->rows.scrollOffset;
    if (rowPtr->worldY < viewPtr->rows.scrollOffset) {
        yOffset = rowPtr->worldY;
    }
    if ((rowPtr->worldY + rowPtr->height) >= 
        (viewPtr->rows.scrollOffset + VPORTHEIGHT(viewPtr))) {
        yOffset = (rowPtr->worldY + rowPtr->height) - VPORTHEIGHT(viewPtr);
    } 
    if (yOffset < 0) {
        yOffset = 0;
    }
    return yOffset;
}

static TableView *tableViewInstance;

static int
CompareRowValues(Column *colPtr, const Row *r1Ptr, const Row *r2Ptr)
{
    TableView *viewPtr;
    const char *s1, *s2;
    long l1, l2;
    double d1, d2;
    int result;
    int sortType;
    BLT_TABLE_COLUMN col;
    BLT_TABLE_ROW r1, r2;

    viewPtr = colPtr->viewPtr;
    col = colPtr->column;
    r1 = r1Ptr->row;
    r2 = r2Ptr->row;

    /* Check for empty values. */
    if (!blt_table_value_exists(viewPtr->table, r1, col)) {
        if (!blt_table_value_exists(viewPtr->table, r2, col)) {
            return 0;
        }
        return 1;
    } else if (!blt_table_value_exists(viewPtr->table, r2, col)) {
        return -1;
    }

    result = 0;
    sortType = colPtr->sortType;
    if (sortType == SORT_AUTO) {
        switch (blt_table_column_type(col)) {
        case TABLE_COLUMN_TYPE_STRING:
            sortType = SORT_DICTIONARY; break;
        case TABLE_COLUMN_TYPE_LONG:
            sortType = SORT_INTEGER;    break;
        case TABLE_COLUMN_TYPE_TIME:
        case TABLE_COLUMN_TYPE_DOUBLE:
            sortType = SORT_REAL;       break;
        default:
            sortType = SORT_DICTIONARY; break;
        }
    }
    switch (sortType) {
    case SORT_ASCII:
        s1 = blt_table_get_string(viewPtr->table, r1, col);
        s2 = blt_table_get_string(viewPtr->table, r2, col);
        result = strcmp(s1, s2);
        break;
    case SORT_DICTIONARY:
        s1 = blt_table_get_string(viewPtr->table, r1, col);
        s2 = blt_table_get_string(viewPtr->table, r2, col);
        result = Blt_DictionaryCompare(s1, s2);
        break;
    case SORT_INTEGER:
        l1 = blt_table_get_long(NULL, viewPtr->table, r1, col, 0);
        l2 = blt_table_get_long(NULL, viewPtr->table, r2, col, 0);
        result = l1 - l2;
        break;
    case SORT_REAL:
        d1 = blt_table_get_double(NULL, viewPtr->table, r1, col);
        d2 = blt_table_get_double(NULL, viewPtr->table, r2, col);
        result = (d1 > d2) ? 1 : (d1 < d2) ? -1 : 0;
        break;
    }
    return result;
}


static int
CompareRowsWithCommand(TableView *viewPtr, Column *colPtr, Row *r1Ptr, 
                       Row *r2Ptr)
{
    Tcl_Interp *interp;
    Tcl_Obj *objPtr, *cmdObjPtr, *resultObjPtr;
    int result;

    interp = viewPtr->interp;
    cmdObjPtr = Tcl_DuplicateObj(colPtr->sortCmdObjPtr);
    /* Table name */
    objPtr = Tcl_NewStringObj(blt_table_name(viewPtr->table), -1);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    /* Row */
    objPtr = GetRowIndexObj(viewPtr, r1Ptr);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    /* Column */
    objPtr = GetColumnIndexObj(viewPtr, colPtr);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    /* Row */
    objPtr = GetRowIndexObj(viewPtr, r2Ptr);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    /* Column */
    objPtr = GetColumnIndexObj(viewPtr, colPtr);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    
    Tcl_IncrRefCount(cmdObjPtr);
    result = Tcl_EvalObjEx(interp, cmdObjPtr, TCL_EVAL_GLOBAL);
    Tcl_DecrRefCount(cmdObjPtr);
    if (result != TCL_OK) {
        Tcl_BackgroundError(interp);
    }
    resultObjPtr = Tcl_GetObjResult(interp);
    if (Tcl_GetIntFromObj(interp, resultObjPtr, &result) != TCL_OK) {
        Tcl_BackgroundError(interp);
    }
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * CompareRows --
 *
 *      Comparison routine (used by qsort) to sort a chain of subnodes.
 *
 * Results:
 *      1 is the first is greater, -1 is the second is greater, 0
 *      if equal.
 *
 *---------------------------------------------------------------------------
 */
static int
CompareRows(const void *a, const void *b)
{
    Row *r1Ptr = *(Row **)a;
    Row *r2Ptr = *(Row **)b;
    TableView *viewPtr;
    int result;
    SortInfo *sortPtr;
    Blt_ChainLink link;

    viewPtr = tableViewInstance;
    sortPtr = &viewPtr->sort;
    result = 0;                         /* Suppress compiler warning. */
    for (link = Blt_Chain_FirstLink(sortPtr->order); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Column *colPtr;

        colPtr = Blt_Chain_GetValue(link);
        /* Fetch the data for sorting. */
        if ((colPtr->sortType == SORT_COMMAND) && 
            (colPtr->sortCmdObjPtr != NULL)) {
            result = CompareRowsWithCommand(viewPtr, colPtr, r1Ptr, r2Ptr);
        } else {
            result = CompareRowValues(colPtr, r1Ptr, r2Ptr);
        }
        if (result != 0) {
            break;
        }
    }
    if (sortPtr->decreasing) {
        return -result;
    } 
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * SortTableView --
 *
 *      Sorts the flatten array of entries.
 *
 *---------------------------------------------------------------------------
 */
static void
SortTableView(TableView *viewPtr)
{
    SortInfo *sortPtr = &viewPtr->sort;

    tableViewInstance = viewPtr;
    viewPtr->sort.flags &= ~SORT_PENDING;
    if (viewPtr->rows.length < 2) {
        return;
    }
    if (sortPtr->flags & SORTED) {
        Row *rowPtr;
        size_t i, j;

        if (sortPtr->decreasing == sortPtr->viewIsDecreasing) {
            return;
        }
        /* 
         * The view is already sorted but in the wrong direction.  Reverse
         * the entries in the array.
         */
        for (i = 0, j = 0, rowPtr = viewPtr->rows.lastPtr; rowPtr != NULL; 
             rowPtr = rowPtr->prevPtr, i++) {
            if ((rowPtr->flags & HIDDEN) == 0) {
                viewPtr->rows.map[j] = rowPtr;
                j++;
            }
        }
        sortPtr->viewIsDecreasing = sortPtr->decreasing;
    } else {
        size_t i;
        Row *rowPtr;

        /* Use the current row map to sort the rows. First include all the
         * rows.  We want to sort the hidden rows as well. */
        for (i = 0, rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
             rowPtr = rowPtr->nextPtr, i++) {
            viewPtr->rows.map[i] = rowPtr;
        }
        sortPtr->viewIsDecreasing = sortPtr->decreasing;
        qsort((char *)viewPtr->rows.map, viewPtr->rows.length, sizeof(Row *),
              (QSortCompareProc *)CompareRows);
        RethreadRows(viewPtr);
    }
    RenumberRows(viewPtr);
    sortPtr->viewIsDecreasing = sortPtr->decreasing;
    sortPtr->flags |= SORTED;
    viewPtr->flags |= LAYOUT_PENDING;
}

static void
DestroyStyles(TableView *viewPtr)
{
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;

    for (hPtr = Blt_FirstHashEntry(&viewPtr->styleTable, &iter); hPtr != NULL;
         hPtr = Blt_NextHashEntry(&iter)) {
        CellStyle *stylePtr;

        stylePtr = Blt_GetHashValue(hPtr);
        stylePtr->hashPtr = NULL;
        (*stylePtr->classPtr->freeProc)(stylePtr);
    }
    Blt_DeleteHashTable(&viewPtr->styleTable);
}

static int
GetStyle(Tcl_Interp *interp, TableView *viewPtr, Tcl_Obj *objPtr, 
         CellStyle **stylePtrPtr)
{
    const char *string;
    Blt_HashEntry *hPtr;

    string = Tcl_GetString(objPtr);
    hPtr = Blt_FindHashEntry(&viewPtr->styleTable, string);
    if (hPtr == NULL) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "can't find style \"", 
                Tcl_GetString(objPtr),
                "\" in \"", Tk_PathName(viewPtr->tkwin), "\"", 
                (char *)NULL);
        }
        return TCL_ERROR;
    }
    *stylePtrPtr = Blt_GetHashValue(hPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * GetCachedObj --
 *
 *      Gets or creates a unique string identifier.  Strings are reference
 *      counted.  The string is placed into a hashed table local to the
 *      tableview.
 *
 * Results:
 *      Returns the pointer to the hashed string.
 *
 *---------------------------------------------------------------------------
 */
static Tcl_Obj *
GetCachedObj(TableView *viewPtr, Tcl_Obj *objPtr)
{
    Blt_HashEntry *hPtr;
    int isNew;
    const char *string;

    string = Tcl_GetString(objPtr);
    hPtr = Blt_CreateHashEntry(&viewPtr->cachedObjTable, string, &isNew);
    if (isNew) {
        Blt_SetHashValue(hPtr, objPtr);
    } else {
        objPtr = Blt_GetHashValue(hPtr);
    }
    Tcl_IncrRefCount(objPtr);
    return objPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * FreeCachedObj --
 *
 *      Releases the cached Tcl_Obj.  Cached Tcl_Objs are reference
 *      counted, so only when the reference count is zero (i.e. no one else
 *      is using the string) is the entry removed from the hash table.
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static void
FreeCachedObj(TableView *viewPtr, Tcl_Obj *objPtr)
{
    Blt_HashEntry *hPtr;
    const char *string;

    string = Tcl_GetString(objPtr);
    hPtr = Blt_FindHashEntry(&viewPtr->cachedObjTable, string);
    assert(hPtr != NULL);
    if (objPtr->refCount <= 1) {
        Blt_DeleteHashEntry(&viewPtr->cachedObjTable, hPtr);
    }
    Tcl_DecrRefCount(objPtr);
}

static Column *
GetFirstColumn(TableView *viewPtr)
{
    Column *colPtr;

    for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
         colPtr = colPtr->nextPtr) {
        if ((colPtr->flags & (HIDDEN|DISABLED|DELETED)) == 0) {
            return colPtr;
        }
    }
    return NULL;
}

static Column *
GetNextColumn(Column *colPtr)
{
    for (colPtr = colPtr->nextPtr; colPtr != NULL; colPtr = colPtr->nextPtr) {
        if ((colPtr->flags & (HIDDEN|DISABLED|DELETED)) == 0) {
            return colPtr;
        }
    }
    return NULL;
}

static Column *
GetPrevColumn(Column *colPtr)
{
    for (colPtr = colPtr->prevPtr; colPtr != NULL; colPtr = colPtr->prevPtr) {
        if ((colPtr->flags & (HIDDEN|DISABLED|DELETED)) == 0) {
            return colPtr;
        }
    }
    return NULL;
}

static Column *
GetLastColumn(TableView *viewPtr)
{
    Column *colPtr;

    for (colPtr = viewPtr->columns.lastPtr; colPtr != NULL; 
         colPtr = colPtr->prevPtr) {
        if ((colPtr->flags & (HIDDEN|DISABLED|DELETED)) == 0) {
            return colPtr;
        }
    }
    return NULL;
}


/*
 *---------------------------------------------------------------------------
 *
 * NearestColumn --
 *
 *      Finds the column at or closest to the given screen X-coordinate in
 *      the viewport.  Only visible columns are examined.
 *
 * Results:
 *      Returns the pointer to the closest column.  If no column is visible 
 *      (columns may be hidden), NULL is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Column *
NearestColumn(TableView *viewPtr, int x, int selectOne)
{
    long low, high;

    if (viewPtr->columns.lastIndex == -1) {
        return NULL;                    /* No visible columns. */
    }
    if (x < viewPtr->rows.titleWidth) {
        return (selectOne) ? viewPtr->columns.map[0] : NULL;
    }
    x = WORLDX(viewPtr, x);             /* The column positions are stored
                                         * in world coordinates, convert
                                         * the sample screen x-coordinate
                                         * to a world coordinate. */

    /* Use a binary search to find the column that contains the
     * x-coordinate from the array of visible columns.  */
    low = viewPtr->columns.firstIndex; high = viewPtr->columns.lastIndex;
    while (low <= high) {
        long mid;
        Column *colPtr;
        
        mid = (low + high) >> 1;
        colPtr = viewPtr->columns.map[mid];
        if (x < colPtr->worldX) {
            high = mid - 1;
        } else if (x >= (colPtr->worldX + colPtr->width)) {
            low = mid + 1;
        } else {
            return colPtr;              /* Found it. */
        }
    }
    if (selectOne) {
        Column *colPtr;

        colPtr = viewPtr->columns.map[viewPtr->columns.lastIndex];
        return colPtr;
    }
    return NULL;
}

static int
GetColumnByIndex(TableView *viewPtr, const char *string, Column **colPtrPtr)
{
    Column *focusPtr, *colPtr;
    char c;

    focusPtr = colPtr = NULL;
    if (viewPtr->focusPtr != NULL) {
        CellKey *keyPtr;
        
        keyPtr = Blt_GetHashKey(&viewPtr->cellTable,
                                viewPtr->focusPtr->hashPtr);
        focusPtr = keyPtr->colPtr;
    }
    c = string[0];
    if (c == '@') {
        int x;

        if (Tcl_GetInt(NULL, string + 1, &x) == TCL_OK) {
            colPtr = NearestColumn(viewPtr, x, FALSE);
        }
    } else if ((c == 'e') && (strcmp(string, "end") == 0)) {
        colPtr = GetLastColumn(viewPtr);
    } else if ((c == 'l') && (strcmp(string, "last") == 0)) {
        colPtr = GetLastColumn(viewPtr);
    } else if ((c == 'f') && (strcmp(string, "first") == 0)) {
        colPtr = GetFirstColumn(viewPtr);
    } else if ((c == 'f') && (strcmp(string, "focus") == 0)) {
        if (focusPtr != NULL) {
            colPtr = focusPtr;
        }
    } else if ((c == 'a') && (strcmp(string, "active") == 0)) {
        colPtr = viewPtr->columns.activeTitlePtr;
    } else if ((c == 'c') && (strcmp(string, "current") == 0)) {
        TableObj *objPtr;

        objPtr = Blt_GetCurrentItem(viewPtr->bindTable);
        if ((objPtr != NULL) && ((objPtr->flags & DELETED) == 0)) {
            ItemType type;

            type = (ItemType)Blt_GetCurrentHint(viewPtr->bindTable);
            switch (type) {
            case ITEM_COLUMN_TITLE:
            case ITEM_COLUMN_FILTER:
            case ITEM_COLUMN_RESIZE:
                colPtr = (Column *)objPtr;
                break;
            case ITEM_CELL:
                {
                    Cell *cellPtr;
                    CellKey *keyPtr;
                    
                    cellPtr = (Cell *)objPtr;
                    keyPtr = GetKey(viewPtr, cellPtr);
                    colPtr = keyPtr->colPtr;
                }
                break;
            default:
                break;
            }
        }
    } else if ((c == 'p') && (strcmp(string, "previous") == 0)) {
        if (focusPtr != NULL) {
            colPtr = GetPrevColumn(focusPtr);
        }
    } else if ((c == 'n') && (strcmp(string, "next") == 0)) {
        if (focusPtr != NULL) {
            colPtr = GetNextColumn(focusPtr);
        }
    } else if ((c == 'n') && (strcmp(string, "none") == 0)) {
        colPtr = NULL;
    } else if ((c == 'v') && (strcmp(string, "view.left") == 0)) {
        if (viewPtr->columns.lastIndex != -1) {
            colPtr = viewPtr->columns.map[viewPtr->columns.firstIndex];
        }
    } else if ((c == 'v') && (strcmp(string, "view.right") == 0)) {
        if (viewPtr->columns.lastIndex != -1) {
            colPtr = viewPtr->columns.map[viewPtr->columns.lastIndex];
        } 
    } else if ((c == 's') && (strcmp(string, "slide.active") == 0)) {
        colPtr = viewPtr->columns.slidePtr;
    } else {
        return TCL_ERROR;
    }
    *colPtrPtr = colPtr;
    return TCL_OK;
}

static int
GetColumnFromObj(Tcl_Interp *interp, TableView *viewPtr, Tcl_Obj *objPtr, 
          Column **colPtrPtr)
{
    BLT_TABLE_COLUMN col;
    Blt_HashEntry *hPtr;
    const char *string;

    *colPtrPtr = NULL;
    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    string = Tcl_GetString(objPtr);
    /* First check if it's a special column index name.  */
    if (GetColumnByIndex(viewPtr, string, colPtrPtr) == TCL_OK) {
        return TCL_OK;
    }
    /* Next see if it's a column in the table. */
    col = blt_table_get_column(interp, viewPtr->table, objPtr);
    if (col == NULL) {
        return TCL_ERROR;
    }
    hPtr = Blt_FindHashEntry(&viewPtr->columns.table, (char *)col);
    if (hPtr == NULL) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "can't find column \"", string, 
                "\" in \"", Tk_PathName(viewPtr->tkwin), "\"", (char *)NULL);
        }
        return TCL_ERROR;
    }
    *colPtrPtr = Blt_GetHashValue(hPtr);
    return TCL_OK;
}


static Row *
GetFirstRow(TableView *viewPtr)
{
    Row *rowPtr;

    for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
         rowPtr = rowPtr->nextPtr) {
        if ((rowPtr->flags & (HIDDEN|DISABLED|DELETED)) == 0) {
            return rowPtr;
        }
    }
    return NULL;
}

static Row *
GetNextRow(Row *rowPtr)
{
    for (rowPtr = rowPtr->nextPtr; rowPtr != NULL; rowPtr = rowPtr->nextPtr) {
        if ((rowPtr->flags & (HIDDEN|DISABLED|DELETED)) == 0) {
            return rowPtr;
        }
    }
    return NULL;
}

static Row *
GetPrevRow(Row *rowPtr)
{
    for (rowPtr = rowPtr->prevPtr; rowPtr != NULL; rowPtr = rowPtr->prevPtr) {
        if ((rowPtr->flags & (HIDDEN|DISABLED|DELETED)) == 0) {
            return rowPtr;
        }
    }
    return NULL;
}

static Row *
GetLastRow(TableView *viewPtr)
{
    Row *rowPtr;

    for (rowPtr = viewPtr->rows.lastPtr; rowPtr != NULL; 
         rowPtr = rowPtr->prevPtr) {
        if ((rowPtr->flags & (HIDDEN|DISABLED|DELETED)) == 0) {
            return rowPtr;
        }
    }
    return NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * NearestRow --
 *
 *      Finds the row closest to the given screen Y-coordinate in the
 *      viewport.
 *
 * Results:
 *      Returns the pointer to the closest row.  If no row is visible (rows
 *      may be hidden), NULL is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Row *
NearestRow(TableView *viewPtr, int y, int selectOne)
{
    long low, high;

    if (viewPtr->rows.lastIndex == -1) {
        return NULL;                    /* No visible rows. */
    }
    if (y < (viewPtr->columns.titleHeight + viewPtr->columns.filterHeight)) {
        return (selectOne) ? viewPtr->rows.map[viewPtr->rows.firstIndex] : NULL;
    }
    y = WORLDY(viewPtr, y);            /* The row positions are stored in
                                        * world coordinates, convert the
                                        * sample screen y-coordinate to a
                                        * world coordinate. */

    /* Use a binary search to find the row that contains the
     * y-coordinate from the array of visible rows.  */
    low = viewPtr->rows.firstIndex; high = viewPtr->rows.lastIndex;
    while (low <= high) {
        long mid;
        Row *rowPtr;
        
        mid = (low + high) >> 1;
        rowPtr = viewPtr->rows.map[mid];
        if (y < rowPtr->worldY) {
            high = mid - 1;
        } else if (y >= (rowPtr->worldY + rowPtr->height)) {
            low = mid + 1;
        } else {
            return rowPtr;              /* Found it. */
        }
    }
    if (selectOne) {
        Row *rowPtr;

        rowPtr = viewPtr->rows.map[viewPtr->rows.lastIndex];
        return rowPtr;
    }
    return NULL;
}

static int
GetRowByIndex(TableView *viewPtr, Tcl_Obj *objPtr, Row **rowPtrPtr)
{
    Row *focusPtr, *rowPtr;
    char c;
    const char *string;
    int length;

    focusPtr = rowPtr = NULL;
    if (viewPtr->focusPtr != NULL) {
        CellKey *keyPtr;
        
        keyPtr = Blt_GetHashKey(&viewPtr->cellTable,
                                viewPtr->focusPtr->hashPtr);
        focusPtr = keyPtr->rowPtr;
    }
    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if (c == '@') {
        int y;

        if (Tcl_GetInt(NULL, string + 1, &y) == TCL_OK) {
            rowPtr = NearestRow(viewPtr, y, FALSE);
        }
    } else if ((c == 'a') && (length > 1) && 
               (strncmp(string, "active", length) == 0)) {
        rowPtr = viewPtr->rows.activeTitlePtr;
    } else if ((c == 'e') && (length > 1) && 
               (strncmp(string, "end", length) == 0)) {
        rowPtr = GetLastRow(viewPtr);
    } else if ((c == 'c') && (strncmp(string, "current", length) == 0)) {
        TableObj *objPtr;

        objPtr = Blt_GetCurrentItem(viewPtr->bindTable);
        if ((objPtr != NULL) && ((objPtr->flags & DELETED) == 0)) {
            ItemType type;

            type = (ItemType)Blt_GetCurrentHint(viewPtr->bindTable);
            switch (type) {
            case ITEM_ROW_TITLE:
            case ITEM_ROW_FILTER:
            case ITEM_ROW_RESIZE:
                rowPtr = (Row *)objPtr;
                break;
            case ITEM_CELL:
                {
                    Cell *cellPtr;
                    CellKey *keyPtr;
                    
                    cellPtr = (Cell *)objPtr;
                    keyPtr = GetKey(viewPtr, cellPtr);
                    rowPtr = keyPtr->rowPtr;
                }
                break;
            default:
                break;
            }
        }
    } else if ((c == 'l') && (strncmp(string, "last", length) == 0)) {
        rowPtr = GetLastRow(viewPtr);
    } else if ((c == 'f') && (strncmp(string, "first", length) == 0)) {
        rowPtr = GetFirstRow(viewPtr);
    } else if ((c == 'f') && (strncmp(string, "focus", length) == 0)) {
        if (focusPtr != NULL) {
            rowPtr = focusPtr;
        }
    } else if ((c == 'p') && (strncmp(string, "previous", length) == 0)) {
        if (focusPtr != NULL) {
            rowPtr = GetPrevRow(focusPtr);
        }
    } else if ((c == 'n') && (strncmp(string, "next", length) == 0)) {
        if (focusPtr != NULL) {
            rowPtr = GetNextRow(focusPtr);
        }
    } else if ((c == 'n') && (strncmp(string, "none", length) == 0)) {
        rowPtr = NULL;
    } else if ((c == 'm') && (strncmp(string, "mark", length) == 0)) {
        rowPtr = viewPtr->rows.selection.markPtr;
    } else if ((c == 'a') && (length > 1) && 
               (strncmp(string, "anchor", length) == 0)) {
        rowPtr = viewPtr->rows.selection.anchorPtr;
    } else if ((c == 'v') && (length > 5) &&
               (strncmp(string, "view.top", length) == 0)) {
        if (viewPtr->rows.lastIndex != -1) {
            rowPtr = viewPtr->rows.map[viewPtr->rows.firstIndex];
        }
    } else if ((c == 'v') && (length > 5) &&
               (strncmp(string, "view.bottom", length) == 0)) {
        if (viewPtr->rows.lastIndex != -1) {
            rowPtr = viewPtr->rows.map[viewPtr->rows.lastIndex];
        } 
    } else {
        return TCL_ERROR;
    }
    *rowPtrPtr = rowPtr;
    return TCL_OK;
}

static int
GetRowFromObj(Tcl_Interp *interp, TableView *viewPtr, Tcl_Obj *objPtr, Row **rowPtrPtr)
{
    BLT_TABLE_ROW row;
    Blt_HashEntry *hPtr;

    *rowPtrPtr = NULL;
    /* Next see if it's a row in the table. */
    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    /* First check if it's a special column index.  */
    if (GetRowByIndex(viewPtr, objPtr, rowPtrPtr) == TCL_OK) {
        return TCL_OK;
    }
    row = blt_table_get_row(interp, viewPtr->table, objPtr);
    if (row == NULL) {
        return TCL_ERROR;
    }
    hPtr = Blt_FindHashEntry(&viewPtr->rows.table, (char *)row);
    if (hPtr == NULL) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "can't find row \"", Tcl_GetString(objPtr),
                "\" in \"", Tk_PathName(viewPtr->tkwin), "\"", 
                (char *)NULL);
        }
        return TCL_ERROR;
    }
    *rowPtrPtr = Blt_GetHashValue(hPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * IconChangedProc
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
/* ARGSUSED */
static void
IconChangedProc(ClientData clientData, int x, int y, int width, int height,
                int imageWidth, int imageHeight)        
{
    TableView *viewPtr = clientData;

    viewPtr->flags |= (GEOMETRY | LAYOUT_PENDING);
    EventuallyRedraw(viewPtr);
}

static Icon
GetIcon(TableView *viewPtr, const char *iconName)
{
    Blt_HashEntry *hPtr;
    int isNew;
    struct _Icon *iconPtr;

    hPtr = Blt_CreateHashEntry(&viewPtr->iconTable, iconName, &isNew);
    if (isNew) {
        Tk_Image tkImage;
        int width, height;

        tkImage = Tk_GetImage(viewPtr->interp, viewPtr->tkwin,(char *)iconName, 
                IconChangedProc, viewPtr);
        if (tkImage == NULL) {
            Blt_DeleteHashEntry(&viewPtr->iconTable, hPtr);
            return NULL;
        }
        Tk_SizeOfImage(tkImage, &width, &height);
        iconPtr = Blt_AssertMalloc(sizeof(struct _Icon));
        iconPtr->viewPtr = viewPtr;
        iconPtr->tkImage = tkImage;
        iconPtr->hashPtr = hPtr;
        iconPtr->refCount = 1;
        iconPtr->width = width;
        iconPtr->height = height;
        Blt_SetHashValue(hPtr, iconPtr);
    } else {
        iconPtr = Blt_GetHashValue(hPtr);
        iconPtr->refCount++;
    }
    return iconPtr;
}

static void
FreeIcon(Icon icon)
{
    struct _Icon *iconPtr = icon;

    iconPtr->refCount--;
    if (iconPtr->refCount == 0) {
        TableView *viewPtr;

        viewPtr = iconPtr->viewPtr;
        Blt_DeleteHashEntry(&viewPtr->iconTable, iconPtr->hashPtr);
        Tk_FreeImage(iconPtr->tkImage);
        Blt_Free(iconPtr);
    }
}

static void
DestroyIcons(TableView *viewPtr)
{
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;

    for (hPtr = Blt_FirstHashEntry(&viewPtr->iconTable, &iter);
         hPtr != NULL; hPtr = Blt_NextHashEntry(&iter)) {
        struct _Icon *iconPtr;

        iconPtr = Blt_GetHashValue(hPtr);
        Tk_FreeImage(iconPtr->tkImage);
        Blt_Free(iconPtr);
    }
    Blt_DeleteHashTable(&viewPtr->iconTable);
}


/*
 *---------------------------------------------------------------------------
 *
 * ObjToRowColumnFlag --
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToRowColumnFlag(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
                   Tcl_Obj *objPtr, char *widgRec, int offset, int flags)      
{
    int bit = (intptr_t)clientData;
    char c;
    const char *string;
    TableView *viewPtr = (TableView *)widgRec;
    int length;

    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if ((c == 'b') && (strncmp(string, "both", length) == 0)) {
        viewPtr->rows.flags |= bit;
        viewPtr->columns.flags |= bit;
    } else if ((c == 'c') && (strncmp(string, "columns", length) == 0)) {
        viewPtr->rows.flags &= ~bit;
        viewPtr->columns.flags |= bit;
    } else if ((c == 'r') && (strncmp(string, "rows", length) == 0)) {
        viewPtr->rows.flags |= bit;
        viewPtr->columns.flags &= ~bit;
    } else if ((c == 'n') && (strncmp(string, "none", length) == 0)) {
        viewPtr->rows.flags &= ~bit;
        viewPtr->columns.flags &= ~bit;
    } else {
        Tcl_AppendResult(interp, "unknown value \"", string, 
                "\": should be both, columns, rows, or none", (char *)NULL);
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowColumnFlagToObj --
 *
 *      Returns the row/column flag value as a string.
 *
 * Results:
 *      The TCL string object is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
RowColumnFlagToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
                   char *widgRec, int offset, int flags)      
{
    TableView *viewPtr = (TableView *)widgRec;
    int bit = (intptr_t)clientData;
    const char *string;

    string = "???";                      /* Suppress compiler warning. */
    if (viewPtr->rows.flags & bit) {
        if (viewPtr->columns.flags & bit) {
            string = "both";
        } else {
            string = "rows";
        }
    } else {
        if (viewPtr->columns.flags & bit) {
            string = "columns";
        } else {
            string = "none";
        }
    }
    return Tcl_NewStringObj(string, -1);
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToSortColumn --
 *
 *      Converts the string, reprsenting a column, to its numeric form.
 *
 * Results:
 *      If the string is successfully converted, TCL_OK is returned.
 *      Otherwise, TCL_ERROR is returned and an error message is left in
 *      interpreter's result field.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToSortColumn(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
                Tcl_Obj *objPtr, char *widgRec, int offset, int flags)      
{
    TableView *viewPtr = (TableView *)widgRec;
    Column **colPtrPtr = (Column **)(widgRec + offset);
    Column *colPtr;

    if (GetColumnFromObj(interp, viewPtr, objPtr, &colPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    *colPtrPtr = colPtr;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SortColumnToObj --
 *
 * Results:
 *      The string representation of the column is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
SortColumnToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
                    char *widgRec, int offset, int flags)       
{
    Column *colPtr = *(Column **)(widgRec + offset);

    if (colPtr == NULL) {
        return Tcl_NewStringObj("", -1);
    }
    return GetColumnIndexObj(colPtr->viewPtr, colPtr);
}


/*ARGSUSED*/
static void
FreeSortOrderProc(ClientData clientData, Display *display, char *widgRec, 
                  int offset)
{
    Blt_Chain *chainPtr = (Blt_Chain *)(widgRec + offset);

    if (*chainPtr != NULL) {
        Blt_Chain_Destroy(*chainPtr);
        *chainPtr = NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToSortOrder --
 *
 *      Converts the string reprsenting a column, to its numeric form.
 *
 * Results:
 *      If the string is successfully converted, TCL_OK is returned.
 *      Otherwise, TCL_ERROR is returned and an error message is left
 *      in interpreter's result field.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToSortOrder(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
               Tcl_Obj *objPtr, char *widgRec, int offset, int flags)       
{
    TableView *viewPtr = (TableView *)widgRec;
    Blt_Chain *chainPtr = (Blt_Chain *)(widgRec + offset);
    Blt_Chain chain;
    int i, objc;
    Tcl_Obj **objv;

    if (Tcl_ListObjGetElements(interp, objPtr, &objc, &objv) != TCL_OK) {
        return TCL_ERROR;
    }
    chain = Blt_Chain_Create();
    for (i = 0; i < objc; i++) {
        Column *colPtr;

        if (GetColumnFromObj(interp, viewPtr, objv[i], &colPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        if (colPtr == NULL) {
            fprintf(stderr, "ObjToColumns: Column %s is NULL\n", 
                    Tcl_GetString(objv[i])); 
            continue;
        }
        Blt_Chain_Append(chain, colPtr);
    }
    if (*chainPtr != NULL) {
        Blt_Chain_Destroy(*chainPtr);
    }
    *chainPtr = chain;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SortOrderToObj --
 *
 * Results:
 *      The string representation of the column is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
SortOrderToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
                   char *widgRec, int offset, int flags)        
{
    Blt_Chain chain = *(Blt_Chain *)(widgRec + offset);
    Blt_ChainLink link;
    Tcl_Obj *listObjPtr;

    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    for (link = Blt_Chain_FirstLink(chain); link != NULL; 
         link = Blt_Chain_NextLink(link)) {
        Column *colPtr;
        Tcl_Obj *objPtr;

        colPtr = Blt_Chain_GetValue(link);
        objPtr = GetColumnIndexObj(colPtr->viewPtr, colPtr);
        Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    }
    return listObjPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToEnum --
 *
 *      Converts the string into its enumerated type.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToEnum(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
          Tcl_Obj *objPtr, char *widgRec, int offset, int flags)
{
    int *enumPtr = (int *)(widgRec + offset);
    char c;
    char **p;
    int i;
    int count;
    char *string;

    string = Tcl_GetString(objPtr);
    c = string[0];
    count = 0;
    for (p = (char **)clientData; *p != NULL; p++) {
        if ((c == p[0][0]) && (strcmp(string, *p) == 0)) {
            *enumPtr = count;
            return TCL_OK;
        }
        count++;
    }
    *enumPtr = -1;

    Tcl_AppendResult(interp, "bad value \"", string, "\": should be ", 
        (char *)NULL);
    p = (char **)clientData; 
    if (count > 0) {
        Tcl_AppendResult(interp, p[0], (char *)NULL);
    }
    for (i = 1; i < (count - 1); i++) {
        Tcl_AppendResult(interp, " ", p[i], ", ", (char *)NULL);
    }
    if (count > 1) {
        Tcl_AppendResult(interp, " or ", p[count - 1], "", (char *)NULL);
    }
    return TCL_ERROR;
}

/*
 *---------------------------------------------------------------------------
 *
 * EnumToObj --
 *
 *      Returns the string associated with the enumerated type.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
EnumToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
              char *widgRec, int offset, int flags)     
{
    int value = *(int *)(widgRec + offset);
    char **strings = (char **)clientData;
    char **p;
    int count;

    count = 0;
    for (p = strings; *p != NULL; p++) {
        if (value == count) {
            return Tcl_NewStringObj(*p, -1);
        }
        count++;
    }
    return Tcl_NewStringObj("unknown value", -1);
}

/*ARGSUSED*/
static void
FreeIconProc(ClientData clientData, Display *display, char *widgRec, int offset)
{
    Icon *iconPtr = (Icon *)(widgRec + offset);

    if (*iconPtr != NULL) {
        FreeIcon(*iconPtr);
        *iconPtr = NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToIcon --
 *
 *      Convert the names of an icon into a Tk image.
 *
 * Results:
 *      If the string is successfully converted, TCL_OK is returned.
 *      Otherwise, TCL_ERROR is returned and an error message is left in
 *      interpreter's result field.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToIcon(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
          Tcl_Obj *objPtr, char *widgRec, int offset, int flags)    
{
    TableView *viewPtr = clientData;
    Icon *iconPtr = (Icon *)(widgRec + offset);
    Icon icon;
    int length;
    const char *string;

    string = Tcl_GetStringFromObj(objPtr, &length);
    icon = NULL;
    if (length > 0) {
        icon = GetIcon(viewPtr, string);
        if (icon == NULL) {
            return TCL_ERROR;
        }
    }
    if (*iconPtr != NULL) {
        FreeIcon(*iconPtr);
    }
    *iconPtr = icon;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * IconToObj --
 *
 *      Converts the icon into its string representation (its name).
 *
 * Results:
 *      The name of the icon is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
IconToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
          char *widgRec, int offset, int flags) 
{
    Icon icon = *(Icon *)(widgRec + offset);

    if (icon == NULL) {
        return Tcl_NewStringObj("", -1);
    } 
    return Tcl_NewStringObj(Blt_Image_Name((icon)->tkImage), -1);
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToScrollMode --
 *
 *      Convert the string reprsenting a scroll mode, to its numeric form.
 *
 * Results:
 *      If the string is successfully converted, TCL_OK is returned.
 *      Otherwise, TCL_ERROR is returned and an error message is left in
 *      interpreter's result field.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToScrollMode(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
                Tcl_Obj *objPtr, char *widgRec, int offset, int flags)      
{
    char c;
    const char *string;
    int *modePtr = (int *)(widgRec + offset);
    int length;

    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if ((c == 'l') && (strncmp(string, "listbox", length) == 0)) {
        *modePtr = BLT_SCROLL_MODE_LISTBOX;
    } else if ((c == 't') && (strncmp(string, "hierbox", length) == 0)) {
        *modePtr = BLT_SCROLL_MODE_HIERBOX;
    } else if ((c == 'c') && (strncmp(string, "canvas", length) == 0)) {
        *modePtr = BLT_SCROLL_MODE_CANVAS;
    } else {
        Tcl_AppendResult(interp, "bad scroll mode \"", string,
                "\": should be tableview, listbox, or canvas", (char *)NULL);
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ScrollModeToObj --
 *
 * Results:
 *      The string representation of the scroll mode is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
ScrollModeToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin, 
                    char *widgRec, int offset, int flags)       
{
    int mode = *(int *)(widgRec + offset);

    switch (mode) {
    case BLT_SCROLL_MODE_LISTBOX: 
        return Tcl_NewStringObj("listbox", 7);
    case BLT_SCROLL_MODE_HIERBOX: 
        return Tcl_NewStringObj("hierbox", 7);
    case BLT_SCROLL_MODE_CANVAS:  
        return Tcl_NewStringObj("canvas", 6);
    default:
        return Tcl_NewStringObj("???", 3);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToSelectMode --
 *
 *      Convert the string reprsenting a scroll mode, to its numeric form.
 *
 * Results:
 *      If the string is successfully converted, TCL_OK is returned.
 *      Otherwise, TCL_ERROR is returned and an error message is left
 *      in interpreter's result field.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToSelectMode(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
                Tcl_Obj *objPtr, char *widgRec, int offset, int flags)      
{
    const char *string;
    char c;
    int *modePtr = (int *)(widgRec + offset);
    int length;

    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if ((c == 's') && (strncmp(string, "singlerow", length) == 0)) {
        *modePtr = SELECT_SINGLE_ROW;
    } else if ((c == 'm') && (strncmp(string, "multiplerows", length) == 0)) {
        *modePtr = SELECT_MULTIPLE_ROWS;
    } else if ((c == 'c') && (strncmp(string, "cells", length) == 0)) {
        *modePtr = SELECT_CELLS;
    } else {
        Tcl_AppendResult(interp, "bad select mode \"", string,
            "\": should be singlerow, multiplerows, or cells",(char *)NULL);
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SelectModeToObj --
 *
 * Results:
 *      The string representation of the select mode is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
SelectModeToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
                    char *widgRec, int offset, int flags)       
{
    int mode = *(int *)(widgRec + offset);

    switch (mode) {
    case SELECT_SINGLE_ROW:
        return Tcl_NewStringObj("singlerow", 9);
    case SELECT_MULTIPLE_ROWS:
        return Tcl_NewStringObj("multiplerows", 12);
    case SELECT_CELLS:
        return Tcl_NewStringObj("cells", 5);
    default:
        return Tcl_NewStringObj("???", 3);
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToState --
 *
 *      Convert the name of a state into an integer.
 *
 * Results:
 *      If the string is successfully converted, TCL_OK is returned.
 *      Otherwise, TCL_ERROR is returned and an error message is left in
 *      interpreter's result field.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToState(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
           Tcl_Obj *objPtr, char *widgRec, int offset, int flags)   
{
    int *flagsPtr = (int *)(widgRec + offset);
    const char *string;
    char c;
    int length;

    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if ((c == 'n') && (strncmp(string, "normal", length) == 0)) {
        *flagsPtr &= ~(DISABLED|HIGHLIGHT);
    } else if ((c == 'd') && (strncmp(string, "disabled", length) == 0)) {
        *flagsPtr &= ~(DISABLED|HIGHLIGHT);
        *flagsPtr |= DISABLED;
    } else if ((c == 'h') && (strncmp(string, "highlighted", length) == 0)) {
        *flagsPtr &= ~(DISABLED|HIGHLIGHT);
        *flagsPtr |= HIGHLIGHT;
    } else {
        Tcl_AppendResult(interp, "invalid state \"", string, "\"",
                         (char *)NULL);
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StateToObj --
 *
 *      Converts the state into its string representation.
 *
 * Results:
 *      The name of the state is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
StateToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
               char *widgRec, int offset, int flags)    
{
    int state = *(int *)(widgRec + offset);

    if (state & DISABLED) {
        return Tcl_NewStringObj("disabled", 8);
    } 
    if (state & HIGHLIGHT) {
        return Tcl_NewStringObj("highlighted", 11);
    } 
    return Tcl_NewStringObj("normal", 6);
}


/*
 *---------------------------------------------------------------------------
 *
 * ObjToCellState --
 *
 *      Converts the string representing a cell state into a bitflag.
 *
 * Results:
 *      The return value is a standard TCL result.  The state flags are
 *      updated.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToCellState(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
               Tcl_Obj *objPtr, char *widgRec, int offset, int flags)   
{
    Cell *cellPtr = (Cell *)widgRec;
    unsigned int *flagsPtr = (unsigned int *)(widgRec + offset);
    const char *string;
    char c;
    int length, mask;
    TableView *viewPtr = clientData;

    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if ((c == 'n') && (strncmp(string, "normal", length) == 0)) {
        mask = 0;
        if (cellPtr == viewPtr->postPtr) {
            viewPtr->postPtr = NULL;
        }
    } else if ((c == 'p') && (strncmp(string, "disabled", length) == 0)) {
        mask = DISABLED;
    } else if ((c == 'h') && (strncmp(string, "highlighted", length) == 0)) {
        mask = HIGHLIGHT;
    } else if ((c == 'p') && (strncmp(string, "posted", length) == 0)) {
        mask = POSTED;
    } else {
        Tcl_AppendResult(interp, "unknown state \"", string, 
            "\": should be disabled, posted, or normal", (char *)NULL);
        return TCL_ERROR;
    }
    if (cellPtr == viewPtr->postPtr) {
        viewPtr->postPtr = NULL;
    }
    if (mask & POSTED) {
        viewPtr->postPtr = cellPtr;
    }        
    *flagsPtr &= ~CELL_FLAGS_MASK;
    *flagsPtr |= mask;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CellStateToObj --
 *
 *      Return the name of the style.
 *
 * Results:
 *      The name representing the style is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
CellStateToObj(
    ClientData clientData,              /* Not used. */
    Tcl_Interp *interp,
    Tk_Window tkwin,                    /* Not used. */
    char *widgRec,                      /* Widget information record */
    int offset,                         /* Offset to field in structure */
    int flags)  
{
    unsigned int state = *(unsigned int *)(widgRec + offset);
    const char *string;

    if (state & DISABLED) {
        string = "disabled";
    } else if (state & POSTED) {
        string = "posted";
    } else if (state & HIGHLIGHT) {
        string = "highlighted";
    } else {
        string = "normal";
    }
    return Tcl_NewStringObj(string, -1);
}

/*ARGSUSED*/
static void
FreeStyleProc(ClientData clientData, Display *display, char *widgRec, 
              int offset)
{
    CellStyle **stylePtrPtr = (CellStyle **)(widgRec + offset);
    CellStyle *stylePtr;

    stylePtr = *stylePtrPtr;
    if (stylePtr != NULL) {
        stylePtr->refCount--;
        if (stylePtr->refCount <= 0) {
            (*stylePtr->classPtr->freeProc)(stylePtr);
        }
        *stylePtrPtr = NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToStyle --
 *
 *      Convert the name of an icon into a tableview style.
 *
 * Results:
 *      If the string is successfully converted, TCL_OK is returned.
 *      Otherwise, TCL_ERROR is returned and an error message is left in
 *      interpreter's result field.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToStyle(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
           Tcl_Obj *objPtr, char *widgRec, int offset, int flags)   
{
    CellStyle **stylePtrPtr = (CellStyle **)(widgRec + offset);
    CellStyle *stylePtr;
    const char *string;
    TableView *viewPtr;

    viewPtr = clientData;
    stylePtr = NULL;
    string = Tcl_GetString(objPtr);
    if (string[0] != '\0') {
        if (GetStyle(interp, viewPtr, objPtr, &stylePtr) != TCL_OK) {
            return TCL_ERROR;
        }
        stylePtr->refCount++;           /* Increment the reference count to
                                         * indicate that we are using this
                                         * style. */
    }
    viewPtr->flags |= (LAYOUT_PENDING | GEOMETRY);
    if (*stylePtrPtr != NULL) {
        (*stylePtrPtr)->refCount--;
        if ((*stylePtrPtr)->refCount <= 0) {
            (*stylePtr->classPtr->freeProc)(*stylePtrPtr);
        }
    }
    *stylePtrPtr = stylePtr;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StyleToObj --
 *
 *      Converts the style into its string representation (its name).
 *
 * Results:
 *      The name of the style is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
StyleToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
               char *widgRec, int offset, int flags)    
{
    CellStyle *stylePtr = *(CellStyle **)(widgRec + offset);

    if (stylePtr != NULL) {
        if (stylePtr->name != NULL) {
            return Tcl_NewStringObj(stylePtr->name, -1);
        }
    } 
    return Tcl_NewStringObj("", -1);
}

/*ARGSUSED*/
static void
FreeTableProc(ClientData clientData, Display *display, char *widgRec, 
              int offset)
{
    BLT_TABLE *tablePtr = (BLT_TABLE *)(widgRec + offset);

    if (*tablePtr != NULL) {
        TableView *viewPtr = clientData;

        /* 
         * Release the current table, removing any rows/columns and entry
         * fields.
         */
        ClearSelections(viewPtr);
        blt_table_close(*tablePtr);
        *tablePtr = NULL;
    }
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToTable --
 *
 *      Convert the string representing the name of a table object into a
 *      table token.
 *
 * Results:
 *      If the string is successfully converted, TCL_OK is returned.
 *      Otherwise, TCL_ERROR is returned and an error message is left in
 *      interpreter's result field.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToTable(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
           Tcl_Obj *objPtr, char *widgRec, int offset, int flags)   
{
    TableView *viewPtr = (TableView *)widgRec;
    BLT_TABLE *tablePtr = (BLT_TABLE *)(widgRec + offset);
    BLT_TABLE table;
    const char *string;
    int length;

    string = Tcl_GetStringFromObj(objPtr, &length);
    if (length == 0) {
        table = NULL;
    } else {
        if (blt_table_open(interp, string, &table) != TCL_OK) {
            return TCL_ERROR;
        }
    }
    if (*tablePtr != NULL) {
        FreeTableProc(clientData, viewPtr->display, widgRec, offset);
        viewPtr->rows.notifier = NULL;
        viewPtr->columns.notifier = NULL;
    }
    *tablePtr = table;
    viewPtr->flags |= (GEOMETRY | LAYOUT_PENDING);
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}
/*
 *---------------------------------------------------------------------------
 *
 * TableToObj --
 *
 * Results:
 *      The string representation of the table is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
TableToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin, 
               char *widgRec, int offset, int flags)    
{
    BLT_TABLE table = *(BLT_TABLE *)(widgRec + offset);
    const char *name;

    if (table == NULL) {
        name = "";
    } else {
        name = blt_table_name(table);
    }
    return Tcl_NewStringObj(name, -1);
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToCachedObj --
 *
 *      Converts the string to a cached Tcl_Obj. Cacheded Tcl_Obj's are
 *      hashed, reference counted strings.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToCachedObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
               Tcl_Obj *objPtr, char *widgRec, int offset, int flags)     
{
    TableView *viewPtr = clientData;
    Tcl_Obj **objPtrPtr = (Tcl_Obj **)(widgRec + offset);

    *objPtrPtr = GetCachedObj(viewPtr, objPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CachedObjToObj --
 *
 *      Returns the cached Tcl_Obj.
 *
 * Results:
 *      The fill style string is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
CachedObjToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
               char *widgRec, int offset, int flags)      
{
    Tcl_Obj *objPtr = *(Tcl_Obj **)(widgRec + offset);

    if (objPtr == NULL) {
        return Tcl_NewStringObj("", -1);
    }
    return objPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * FreeCachedObjProc --
 *
 *      Free the cached obj from the widget record, setting it to NULL.
 *
 * Results:
 *      The CACHEDOBJ in the widget record is set to NULL.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static void
FreeCachedObjProc(ClientData clientData, Display *display, char *widgRec,
                  int offset)
{
    Tcl_Obj **objPtrPtr = (Tcl_Obj **)(widgRec + offset);

    if (*objPtrPtr != NULL) {
        TableView *viewPtr = clientData;

        FreeCachedObj(viewPtr, *objPtrPtr);
        *objPtrPtr = NULL;
    }
}


/*
 *---------------------------------------------------------------------------
 *
 * ObjToHide --
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToHide(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
          Tcl_Obj *objPtr, char *widgRec, int offset, int flags)      
{
    int *flagsPtr = (int*)(widgRec + offset);
    Row *rowPtr = (Row *)widgRec;
    int state;

    if (Tcl_GetBooleanFromObj(interp, objPtr, &state) != TCL_OK) {
        return TCL_ERROR;
    }
    if (state == ((*flagsPtr & HIDDEN) != 0)) {
        return TCL_OK;
    }
    if (rowPtr->flags & COLUMN) {
        rowPtr->viewPtr->columns.flags |= REINDEX;
    } else {
        rowPtr->viewPtr->rows.flags |= REINDEX;
    }
    if (state) {
        *flagsPtr |= HIDDEN;
    } else {
        *flagsPtr &= ~HIDDEN;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * HideToObj --
 *
 *      Returns the current -hide value as a string.
 *
 * Results:
 *      The TCL string object is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
HideToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
          char *widgRec, int offset, int flags)      
{
    int *flagsPtr = (int*)(widgRec + offset);
    int state;

    state = ((*flagsPtr & HIDDEN) != 0);
    return Tcl_NewBooleanObj(state);
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToShow --
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToShow(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
          Tcl_Obj *objPtr, char *widgRec, int offset, int flags)      
{
    int *flagsPtr = (int*)(widgRec + offset);
    Row *rowPtr = (Row *)widgRec;
    int state;

    if (Tcl_GetBooleanFromObj(interp, objPtr, &state) != TCL_OK) {
        return TCL_ERROR;
    }
    if (state == ((*flagsPtr & HIDDEN) == 0)) {
        return TCL_OK;
    }
    if (rowPtr->flags & COLUMN) {
        rowPtr->viewPtr->columns.flags |= REINDEX;
    } else {
        rowPtr->viewPtr->rows.flags |= REINDEX;
    }
    if (state) {
        *flagsPtr &= ~HIDDEN;
    } else {
        *flagsPtr |= HIDDEN;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ShowToObj --
 *
 *      Returns the current -show value as a string.
 *
 * Results:
 *      The TCL string object is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
ShowToObj(ClientData clientData, Tcl_Interp *interp, Tk_Window tkwin,
          char *widgRec, int offset, int flags)      
{
    int *flagsPtr = (int*)(widgRec + offset);
    int state;

    state = ((*flagsPtr & HIDDEN) == 0);
    return Tcl_NewBooleanObj(state);
}
/*
 *---------------------------------------------------------------------------
 *
 * GetBoundedWidth --
 *
 *      Bounds a given width value to the limits described in the limit
 *      structure.  The initial starting value may be overridden by the
 *      nominal value in the limits.
 *
 * Results:
 *      Returns the constrained value.
 *
 *---------------------------------------------------------------------------
 */
static int
GetBoundedWidth(int w, Limits *limitsPtr)               
{
    if (limitsPtr->flags & LIMITS_SET_NOM) {
        w = limitsPtr->nom;         /* Override initial value */
    }
    if (w < limitsPtr->min) {
        w = limitsPtr->min;         /* Bounded by minimum value */
    } else if (w > limitsPtr->max) {
        w = limitsPtr->max;         /* Bounded by maximum value */
    }
    return w;
}

/*
 *---------------------------------------------------------------------------
 *
 * GetBoundedHeight --
 *
 *      Bounds a given value to the limits described in the limit structure.
 *      The initial starting value may be overridden by the nominal value in
 *      the limits.
 *
 * Results:
 *      Returns the constrained value.
 *
 *---------------------------------------------------------------------------
 */
static int
GetBoundedHeight(int h, Limits *limitsPtr)
{
    if (limitsPtr->flags & LIMITS_SET_NOM) {
        h = limitsPtr->nom;             /* Override initial value */
    }
    if (h < limitsPtr->min) {
        h = limitsPtr->min;             /* Bounded by minimum value */
    } else if (h > limitsPtr->max) {
        h = limitsPtr->max;             /* Bounded by maximum value */
    }
    return h;
}

/*
 *---------------------------------------------------------------------------
 *
 * ObjToLimits --
 *
 *      Converts the list of elements into zero or more pixel values which
 *      determine the range of pixel values possible.  An element can be in
 *      any form accepted by Tk_GetPixels. The list has a different meaning
 *      based upon the number of elements.
 *
 *          # of elements:
 *
 *          0 - the limits are reset to the defaults.
 *          1 - the minimum and maximum values are set to this
 *              value, freezing the range at a single value.
 *          2 - first element is the minimum, the second is the
 *              maximum.
 *          3 - first element is the minimum, the second is the
 *              maximum, and the third is the nominal value.
 *
 *      Any element may be the empty string which indicates the default.
 *
 * Results:
 *      The return value is a standard TCL result.  The min and max fields
 *      of the range are set.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ObjToLimits(
    ClientData clientData,      /* Not used. */
    Tcl_Interp *interp,         /* Interpreter to send results back to */
    Tk_Window tkwin,            /* Widget of table */
    Tcl_Obj *objPtr,            /* New width list */
    char *widgRec,              /* Widget record */
    int offset,                 /* Offset to field in structure */
    int flags)  
{
    Limits *limitsPtr = (Limits *)(widgRec + offset);
    Tcl_Obj **elv;
    int elc;
    int limits[3];
    int limitsFlags;

    elv = NULL;
    elc = 0;

    /* Initialize limits to default values */
    limits[2] = LIMITS_NOM;
    limits[1] = LIMITS_MAX;
    limits[0] = LIMITS_MIN;
    limitsFlags = 0;

    if (objPtr != NULL) {
        int i;

        if (Tcl_ListObjGetElements(interp, objPtr, &elc, &elv) != TCL_OK) {
            return TCL_ERROR;
        }
        if (elc > 3) {
            Tcl_AppendResult(interp, "wrong # limits \"", 
                        Tcl_GetString(objPtr), "\"", (char *)NULL);
            return TCL_ERROR;
        }
        for (i = 0; i < elc; i++) {
            int length, size;

            Tcl_GetStringFromObj(elv[i], &length);
            if (length == 0) {
                continue;             /* Empty string: use default value */
            }
            limitsFlags |= (LIMITS_SET_BIT << i);
            if (Tk_GetPixelsFromObj(interp, tkwin, elv[i], &size) != TCL_OK) {
                return TCL_ERROR;
            }
            if ((size < LIMITS_MIN) || (size > LIMITS_MAX)) {
                Tcl_AppendResult(interp, "bad limits \"", 
                        Tcl_GetString(objPtr), "\"", (char *)NULL);
                return TCL_ERROR;
            }
            limits[i] = size;
        }
    }
    /*
    * Check the limits specified.  We can't check the requested
    * size of widgets.
    */
    switch (elc) {
    case 1:
        limitsFlags |= (LIMITS_SET_MIN | LIMITS_SET_MAX);
        limits[1] = limits[0];       /* Set minimum and maximum to value */
        break;

    case 2:
        if (limits[1] < limits[0]) {
            Tcl_AppendResult(interp, "bad range \"", Tcl_GetString(objPtr),
                "\": min > max", (char *)NULL);
            return TCL_ERROR;         /* Minimum is greater than maximum */
        }
        break;
    case 3:
        if (limits[1] < limits[0]) {
            Tcl_AppendResult(interp, "bad range \"", Tcl_GetString(objPtr),
                             "\": min > max", (char *)NULL);
            return TCL_ERROR;         /* Minimum is greater than maximum */
        }
        if ((limits[2] < limits[0]) || (limits[2] > limits[1])) {
            Tcl_AppendResult(interp, "nominal value \"", 
                             Tcl_GetString(objPtr),
                             "\" out of range", (char *)NULL);
            return TCL_ERROR;        /* Nominal is outside of range defined
                                      * by minimum and maximum */
        }
        break;
    }
    limitsPtr->min = limits[0];
    limitsPtr->max = limits[1];
    limitsPtr->nom = limits[2];
    limitsPtr->flags = limitsFlags;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ResetLimits --
 *
 *      Resets the limits to their default values.
 *
 * Results:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
INLINE static void
ResetLimits(Limits *limitsPtr)  /* Limits to be imposed on the value */
{
    limitsPtr->flags = 0;
    limitsPtr->min = LIMITS_MIN;
    limitsPtr->max = LIMITS_MAX;
    limitsPtr->nom = LIMITS_NOM;
}

/*
 *---------------------------------------------------------------------------
 *
 * NameOfLimits --
 *
 *      Convert the values into a list representing the limits.
 *
 * Results:
 *      The static string representation of the limits is returned.
 *
 *---------------------------------------------------------------------------
 */
static Tcl_Obj *
NameOfLimits(Tcl_Interp *interp, Limits *limitsPtr)
{
    Tcl_Obj *listObjPtr, *objPtr;

    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    objPtr = (limitsPtr->flags & LIMITS_SET_MIN) ? 
        Tcl_NewIntObj(limitsPtr->min) : Tcl_NewStringObj("", 0);
    Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    objPtr = (limitsPtr->flags & LIMITS_SET_MAX) ? 
        Tcl_NewIntObj(limitsPtr->max) : Tcl_NewStringObj("", 0);
    Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    objPtr = (limitsPtr->flags & LIMITS_SET_NOM) ? 
        Tcl_NewIntObj(limitsPtr->nom) : Tcl_NewStringObj("", 0);
    Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    return listObjPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * LimitsToObj --
 *
 *      Convert the limits of the pixel values allowed into a list.
 *
 * Results:
 *      The string representation of the limits is returned.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static Tcl_Obj *
LimitsToObj(
    ClientData clientData,              /* Not used. */
    Tcl_Interp *interp,                 /* Not used. */
    Tk_Window tkwin,                    /* Not used. */
    char *widgRec,                      /* Row/column structure record */
    int offset,                         /* Offset to field in structure */
    int flags)  
{
    Limits *limitsPtr = (Limits *)(widgRec + offset);

    return NameOfLimits(interp, limitsPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * RowTraceProc --
 *
 * Results:
 *      Returns TCL_OK.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowTraceProc(ClientData clientData, BLT_TABLE_TRACE_EVENT *eventPtr)
{
    TableView *viewPtr = clientData; 

    if (eventPtr->mask & (TABLE_TRACE_WRITES | TABLE_TRACE_UNSETS)) {
        Column *colPtr;
        Row *rowPtr;
        long rowIndex, colIndex;

        colPtr = GetColumnContainer(viewPtr, eventPtr->column);
        rowPtr = GetRowContainer(viewPtr, eventPtr->row);
        rowIndex = colIndex = -1;
        if (colPtr != NULL) {
            colIndex = colPtr->index;
        }
        if (rowPtr != NULL) {
            rowPtr->flags |= GEOMETRY;
            rowIndex = rowPtr->index;
        }
        viewPtr->flags |= GEOMETRY | LAYOUT_PENDING;
        /* Check if the event's row or column occur outside of the range of
         * visible cells. */
        if ((rowIndex > viewPtr->rows.lastIndex) || 
            (colIndex > viewPtr->columns.lastIndex)) {
            return TCL_OK;
        }
        PossiblyRedraw(viewPtr);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnTraceProc --
 *
 * Results:
 *      Returns TCL_OK.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnTraceProc(ClientData clientData, BLT_TABLE_TRACE_EVENT *eventPtr)
{
    TableView *viewPtr = clientData; 

    if (eventPtr->mask & (TABLE_TRACE_WRITES | TABLE_TRACE_UNSETS)) {
        Column *colPtr;
        Row *rowPtr;
        long rowIndex, colIndex;

        colPtr = GetColumnContainer(viewPtr, eventPtr->column);
        rowPtr = GetRowContainer(viewPtr, eventPtr->row);
        rowIndex = colIndex = -1;
        if (colPtr != NULL) {
            colPtr->flags |= GEOMETRY;
            colIndex = colPtr->index;
        }
        if (rowPtr != NULL) {
            rowIndex = rowPtr->index;
        }
        viewPtr->flags |= GEOMETRY | LAYOUT_PENDING;
        /* Check if the event's row or column occur outside of the range of
         * visible cells. */
        if ((rowIndex > viewPtr->rows.lastIndex) || 
            (colIndex > viewPtr->columns.lastIndex)) {
            return TCL_OK;
        }
        PossiblyRedraw(viewPtr);
    }
    return TCL_OK;
}

static void
FreeCellProc(DestroyData data)
{
    FreeCell *freePtr = (FreeCell *)data;
    
    Blt_Pool_FreeItem(freePtr->viewPtr->cellPool, freePtr->cellPtr);
    Blt_Free(freePtr);
}

static void
DestroyCell(TableView *viewPtr, Cell *cellPtr) 
{
    FreeCell *freePtr;

    if (cellPtr == viewPtr->activePtr) {
        viewPtr->activePtr = NULL;
    }
    Blt_DeleteBindings(viewPtr->bindTable, cellPtr);
    if (cellPtr == viewPtr->focusPtr) {
        viewPtr->focusPtr = NULL;
        Blt_SetFocusItem(viewPtr->bindTable, viewPtr->focusPtr, 
                         (ClientData)ITEM_CELL);
    }
    if (cellPtr->stylePtr != NULL) {
        CellKey *keyPtr;
        Blt_HashEntry *hPtr;

        keyPtr = GetKey(viewPtr, cellPtr);
        /* Remove the cell from the style's cell table. */
        hPtr = Blt_FindHashEntry(&cellPtr->stylePtr->table, (char *)keyPtr);
        if (hPtr != NULL) {
            Blt_DeleteHashEntry(&cellPtr->stylePtr->table, hPtr);
        }
        cellPtr->stylePtr->refCount--;
        if (cellPtr->stylePtr->refCount <= 0) {
            (*cellPtr->stylePtr->classPtr->freeProc)(cellPtr->stylePtr);
        }
    }
    ClearSelections(viewPtr);
    if (((cellPtr->flags & CELL_DONT_DELETE) == 0) && 
        (cellPtr->hashPtr != NULL)) {
        Blt_DeleteHashEntry(&viewPtr->cellTable, cellPtr->hashPtr);
    }
    if ((cellPtr->text != NULL) && (cellPtr->flags & TEXTALLOC)) {
        Blt_Free(cellPtr->text);
    }
    if (cellPtr->tkImage != NULL) {
        Tk_FreeImage(cellPtr->tkImage);
    }
    cellPtr->flags |= DELETED;
    freePtr = Blt_AssertMalloc(sizeof(FreeCell));
    freePtr->cellPtr = cellPtr;
    freePtr->viewPtr = viewPtr;
    Tcl_EventuallyFree(freePtr, FreeCellProc);
}

static void
RemoveRowCells(TableView *viewPtr, Row *rowPtr)
{
    CellKey key;
    Column *colPtr;

    /* For each column remove the row, column combination in the table. */
    key.rowPtr = rowPtr;
    for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
         colPtr = colPtr->nextPtr) {
        Blt_HashEntry *hPtr;

        key.colPtr = colPtr;
        hPtr = Blt_FindHashEntry(&viewPtr->cellTable, &key);
        if (hPtr != NULL) {
            Cell *cellPtr;

            cellPtr = Blt_GetHashValue(hPtr);
            DestroyCell(viewPtr, cellPtr);
        }
    }
}

static void
RemoveColumnCells(TableView *viewPtr, Column *colPtr)
{
    CellKey key;
    Row *rowPtr;

    /* For each row remove the row,column combination in the table. */
    key.colPtr = colPtr;
    for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
         rowPtr = rowPtr->nextPtr) {
        Blt_HashEntry *hPtr;

        key.rowPtr = rowPtr;
        hPtr = Blt_FindHashEntry(&viewPtr->cellTable, &key);
        if (hPtr != NULL) {
            Cell *cellPtr;

            cellPtr = Blt_GetHashValue(hPtr);
            DestroyCell(viewPtr, cellPtr);
        }
    }
}

static void
RowFreeProc(DestroyData data)
{
    Row *rowPtr = (Row *)data;
    TableView *viewPtr;

    viewPtr = rowPtr->viewPtr;
    Blt_Pool_FreeItem(viewPtr->rows.pool, rowPtr);
}

static void
DestroyRow(Row *rowPtr)
{
    TableView *viewPtr;

    viewPtr = rowPtr->viewPtr;
    cachedObjOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    iconOption.clientData = viewPtr;
    Blt_DeleteBindings(viewPtr->bindTable, rowPtr);
    Blt_FreeOptions(rowSpecs, (char *)rowPtr, viewPtr->display, 0);
    if (rowPtr->hashPtr != NULL) {
        Blt_DeleteHashEntry(&viewPtr->rows.table, rowPtr->hashPtr);
    }
    if ((rowPtr->row != NULL) && (viewPtr->table != NULL)) {
        blt_table_clear_row_traces(viewPtr->table, rowPtr->row);
    }
    if ((rowPtr->flags & DELETED) == 0) {
        RemoveRowCells(viewPtr, rowPtr);
    }
    if (viewPtr->rows.firstPtr == rowPtr) {
        viewPtr->rows.firstPtr = rowPtr->nextPtr;
    }
    if (viewPtr->rows.lastPtr == rowPtr) {
        viewPtr->rows.lastPtr = rowPtr->prevPtr;
    }
    if (rowPtr->nextPtr != NULL) {
        rowPtr->nextPtr->prevPtr = rowPtr->prevPtr;
    }
    if (rowPtr->prevPtr != NULL) {
        rowPtr->prevPtr->nextPtr = rowPtr->nextPtr;
    }
    rowPtr->prevPtr = rowPtr->nextPtr = NULL;
    viewPtr->rows.length--;
    rowPtr->flags |= DELETED;
    Tcl_EventuallyFree(rowPtr, RowFreeProc);
}

static Row *
NewRow(TableView *viewPtr, BLT_TABLE_ROW row, Blt_HashEntry *hPtr)
{
    Row *rowPtr;

    rowPtr = Blt_Pool_AllocItem(viewPtr->rows.pool, sizeof(Row));
    memset(rowPtr, 0, sizeof(Row));
    rowPtr->row = row;
    rowPtr->viewPtr = viewPtr;
    rowPtr->flags = GEOMETRY;
    rowPtr->weight = 1.0;
    rowPtr->max = SHRT_MAX;
    rowPtr->titleJustify = TK_JUSTIFY_RIGHT;
    rowPtr->titleRelief = rowPtr->activeTitleRelief = TK_RELIEF_RAISED;
    rowPtr->hashPtr = hPtr;
    rowPtr->index = viewPtr->rows.length;
    ResetLimits(&rowPtr->reqHeight);
    Blt_SetHashValue(hPtr, rowPtr);
    if (viewPtr->rows.firstPtr == NULL) {
        viewPtr->rows.lastPtr = viewPtr->rows.firstPtr = rowPtr;
    } else {
        rowPtr->prevPtr = viewPtr->rows.lastPtr;
        if (viewPtr->rows.lastPtr != NULL) {
            viewPtr->rows.lastPtr->nextPtr = rowPtr;
        }
        viewPtr->rows.lastPtr = rowPtr;
    }
    viewPtr->rows.length++;
    return rowPtr;
}

static Row *
CreateRow(TableView *viewPtr, BLT_TABLE_ROW row, Blt_HashEntry *hPtr)
{
    Row *rowPtr;
    
    rowPtr = NewRow(viewPtr, row, hPtr);
    iconOption.clientData = viewPtr;
    cachedObjOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    if (Blt_ConfigureComponentFromObj(viewPtr->interp, viewPtr->tkwin,
        blt_table_row_label(row), "Row", rowSpecs, 0, (Tcl_Obj **)NULL,
        (char *)rowPtr, 0) != TCL_OK) {
        DestroyRow(rowPtr);
        return NULL;
    }
    return rowPtr;
}

static void
ColumnFreeProc(DestroyData data)
{
    Column *colPtr = (Column *)data;
    TableView *viewPtr;

    viewPtr = colPtr->viewPtr;
    Blt_Pool_FreeItem(viewPtr->columns.pool, colPtr);
}

static void
DestroyColumn(Column *colPtr)
{
    TableView *viewPtr;

    viewPtr = colPtr->viewPtr;
    cachedObjOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    iconOption.clientData = viewPtr;
    Blt_DeleteBindings(viewPtr->bindTable, colPtr);
    Blt_FreeOptions(columnSpecs, (char *)colPtr, viewPtr->display, 0);
    if (colPtr->hashPtr != NULL) {
        Blt_DeleteHashEntry(&viewPtr->columns.table, colPtr->hashPtr);
    }
    if ((colPtr->column != NULL) && (viewPtr->table != NULL)) {
        blt_table_clear_column_traces(viewPtr->table, colPtr->column);
    }
    if ((colPtr->flags & DELETED) == 0) {
        RemoveColumnCells(viewPtr, colPtr);
    }
    if (viewPtr->columns.firstPtr == colPtr) {
        viewPtr->columns.firstPtr = colPtr->nextPtr;
    }
    if (viewPtr->columns.lastPtr == colPtr) {
        viewPtr->columns.lastPtr = colPtr->prevPtr;
    }
    if (colPtr->nextPtr != NULL) {
        colPtr->nextPtr->prevPtr = colPtr->prevPtr;
    }
    if (colPtr->prevPtr != NULL) {
        colPtr->prevPtr->nextPtr = colPtr->nextPtr;
    }
    colPtr->prevPtr = colPtr->nextPtr = NULL;
    viewPtr->columns.length--;
    colPtr->flags |= DELETED;
    Tcl_EventuallyFree(colPtr, ColumnFreeProc);
}

static Column *
NewColumn(TableView *viewPtr, BLT_TABLE_COLUMN col, Blt_HashEntry *hPtr)
{
    Column *colPtr;

    colPtr = Blt_Pool_AllocItem(viewPtr->columns.pool, sizeof(Column));
    memset(colPtr, 0, sizeof(Column));
    colPtr->column = col;
    colPtr->viewPtr = viewPtr;
    colPtr->flags = GEOMETRY | COLUMN;
    colPtr->weight = 1.0;
    colPtr->ruleWidth = 1;
    colPtr->pad.side1 = colPtr->pad.side2 = 0;
    colPtr->max = SHRT_MAX;
    colPtr->sortType = SORT_AUTO;
    colPtr->titleJustify = TK_JUSTIFY_CENTER;
    colPtr->titleRelief = colPtr->activeTitleRelief = TK_RELIEF_RAISED;
    colPtr->hashPtr = hPtr;
    colPtr->index = viewPtr->columns.length;
    Blt_SetHashValue(hPtr, colPtr);
    ResetLimits(&colPtr->reqWidth);

    if (viewPtr->columns.firstPtr == NULL) {
        viewPtr->columns.lastPtr = viewPtr->columns.firstPtr = colPtr;
    } else {
        colPtr->prevPtr = viewPtr->columns.lastPtr;
        if (viewPtr->columns.lastPtr != NULL) {
            viewPtr->columns.lastPtr->nextPtr = colPtr;
        }
        viewPtr->columns.lastPtr = colPtr;
    }
    viewPtr->columns.length++;
    return colPtr;
}

static void
ComputeColumnTitleGeometry(TableView *viewPtr, Column *colPtr)
{
    unsigned int aw, ah, iw, ih, tw, th;
    const char *title;
    
    colPtr->titleWidth  = 2 * (viewPtr->columns.titleBorderWidth + TITLE_PADX);
    colPtr->titleHeight = 2 * (viewPtr->columns.titleBorderWidth + TITLE_PADY);
    colPtr->textHeight = colPtr->textWidth = 0;
    aw = ah = tw = th = iw = ih = 0;
    if (colPtr->icon != NULL) {
        iw = IconWidth(colPtr->icon);
        ih = IconHeight(colPtr->icon);
        colPtr->titleWidth += iw;
    }
    title = GetColumnTitle(colPtr);
    if (title != NULL) {
        TextStyle ts;

        Blt_Ts_InitStyle(ts);
        Blt_Ts_SetFont(ts, viewPtr->columns.titleFont);
        Blt_Ts_GetExtents(&ts, title,  &tw, &th);
        colPtr->textWidth = tw;
        colPtr->textHeight = th;
        colPtr->titleWidth += tw;
        if (colPtr->icon != NULL) {
            colPtr->titleWidth += TITLE_PADX;
        }
    }
    if ((viewPtr->sort.up != NULL) && (viewPtr->sort.down != NULL)) {
        aw = MAX(IconWidth(viewPtr->sort.up), IconWidth(viewPtr->sort.down));
        ah = MAX(IconHeight(viewPtr->sort.up), IconHeight(viewPtr->sort.down));
    } else {
        Blt_FontMetrics fm;

        Blt_Font_GetMetrics(viewPtr->columns.titleFont, &fm);
        ah = fm.linespace;
        aw = colPtr->textHeight * 60 / 100;
    }
    colPtr->titleWidth  += aw + TITLE_PADX;
    colPtr->titleHeight += MAX3(ih, th, ah);
}

/*
 * InitColumnFilters -- 
 *
 *      Called by ConfigureTableView routine to initialize the column
 *      filters menu used by all columns.  This calls TCL code to 
 *      create the column filter menu and scrollbars.
 */
static int
InitColumnFilters(Tcl_Interp *interp, TableView *viewPtr)
{
    int result;
    Tcl_Obj *cmdObjPtr, *objPtr;

    if ((viewPtr->flags & COLUMN_FILTERS) == 0) {
        return TCL_OK;
    }
    if (!Blt_CommandExists(interp,"::blt::TableView::InitColumnFilters")) {
        return TCL_OK;
    }
    cmdObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    objPtr = Tcl_NewStringObj("::blt::TableView::InitColumnFilters", -1);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    objPtr = Tcl_NewStringObj(Tk_PathName(viewPtr->tkwin), -1);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    Tcl_IncrRefCount(cmdObjPtr);
    Tcl_Preserve(viewPtr);
    result = Tcl_EvalObjEx(interp, cmdObjPtr, TCL_EVAL_GLOBAL);
    Tcl_Release(viewPtr);
    Tcl_DecrRefCount(cmdObjPtr);
    return result;
}

/*
 * ComputeColumnFiltersGeometry -- 
 *
 *      +---------------------------+   
 *      |b|x|icon|x|text|x|arrow|x|b|   
 *      +---------------------------+
 *
 * b = filter borderwidth
 * x = padx 
 */
static void
ComputeColumnFiltersGeometry(TableView *viewPtr)
{
    unsigned int ah;
    FilterInfo *filterPtr;
    Column *colPtr;

    filterPtr = &viewPtr->filter;
    viewPtr->columns.filterHeight = 0;
    viewPtr->arrowWidth = ah = Blt_TextWidth(filterPtr->font, "0", 1) + 
        2 * (filterPtr->borderWidth + 1);
    for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
         colPtr = colPtr->nextPtr) {
        unsigned int tw, th, ih, iw;

        tw = th = ih = iw = 0;
        if (colPtr->filterIcon != NULL) {
            ih = IconHeight(colPtr->filterIcon);
            iw = IconWidth(colPtr->filterIcon);
        }
        if (colPtr->filterText != NULL) {
            TextStyle ts;
            Blt_Font font;

            Blt_Ts_InitStyle(ts);
            font = CHOOSE(filterPtr->font, colPtr->filterFont);
            Blt_Ts_SetFont(ts, font);
            Blt_Ts_GetExtents(&ts, colPtr->filterText, &tw, &th);
            colPtr->filterTextWidth = tw;
            colPtr->filterTextHeight = th;
        } else {
            Blt_FontMetrics fm;
            Blt_Font font;

            font = CHOOSE(filterPtr->font, colPtr->filterFont);
            Blt_Font_GetMetrics(font, &fm);
            th = fm.linespace;
            colPtr->filterTextWidth = 0, colPtr->filterTextHeight = th;
        }
        
        colPtr->filterHeight = MAX3(ah, th, ih);
        if (viewPtr->columns.filterHeight < colPtr->filterHeight) {
            viewPtr->columns.filterHeight = colPtr->filterHeight;
        }
    }
    viewPtr->columns.filterHeight += 
        2 * (filterPtr->borderWidth + TITLE_PADY + 1);
}

static int 
ConfigureColumn(TableView *viewPtr, Column *colPtr)
{
    if (Blt_ConfigModified(columnSpecs, "-font", "-title", "-hide", "-icon", 
        "-arrowwidth", "-borderwidth", (char *)NULL)) {
        if (viewPtr->columns.flags & SHOW_TITLES) {
            ComputeColumnTitleGeometry(viewPtr, colPtr);
        } 
    }
    if (Blt_ConfigModified(columnSpecs, "-filtertext", (char *)NULL)) {
        ComputeColumnFiltersGeometry(viewPtr);
    }
    if (Blt_ConfigModified(columnSpecs, "-style", (char *)NULL)) {
        /* If the style changed, recompute the geometry of the cells. */
        colPtr->flags |= GEOMETRY;
        viewPtr->flags |= GEOMETRY;
    }
    return TCL_OK;
}

static Column *
CreateColumn(TableView *viewPtr, BLT_TABLE_COLUMN col, Blt_HashEntry *hPtr)
{
    Column *colPtr;
    
    colPtr = NewColumn(viewPtr, col, hPtr);
    iconOption.clientData = viewPtr;
    cachedObjOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    if (Blt_ConfigureComponentFromObj(viewPtr->interp, viewPtr->tkwin,
        blt_table_column_label(colPtr->column), "Column", columnSpecs, 0,
        (Tcl_Obj **)NULL, (char *)colPtr, 0) != TCL_OK) {
        DestroyColumn(colPtr);
        return NULL;
    }
    ConfigureColumn(viewPtr, colPtr);
    return colPtr;
}

static int
GetRowIterator(Tcl_Interp *interp, TableView *viewPtr, Tcl_Obj *objPtr, 
               RowIterator *iterPtr)
{
    Row *rowPtr;

    iterPtr->startPtr = NULL;
    iterPtr->type = ITER_SINGLE;
    iterPtr->viewPtr = viewPtr;
    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    if (GetRowFromObj(NULL, viewPtr, objPtr, &rowPtr) == TCL_OK) {
        iterPtr->startPtr = rowPtr;
        return TCL_OK;
    }
    if (blt_table_iterate_rows(interp, viewPtr->table, objPtr, 
                               &iterPtr->cursor) != TCL_OK){
        return TCL_ERROR;
    }
    return TCL_OK;
}

static int
GetColumnIterator(Tcl_Interp *interp, TableView *viewPtr, Tcl_Obj *objPtr, 
                  ColumnIterator *iterPtr)
{
    Column *colPtr;

    iterPtr->startPtr = NULL;
    iterPtr->type = ITER_SINGLE;
    iterPtr->viewPtr = viewPtr;
    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    if (GetColumnFromObj(NULL, viewPtr, objPtr, &colPtr) == TCL_OK) {
        iterPtr->startPtr = colPtr;
        return TCL_OK;
    }
    if (blt_table_iterate_columns(interp, viewPtr->table, objPtr, 
                                  &iterPtr->cursor) != TCL_OK){
        return TCL_ERROR;
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * NextTaggedColumn --
 *
 *      Returns the next column derived from the given tag.
 *
 * Results:
 *      Returns the pointer to the next column in the iterator.  If no more
 *      columns are available, then NULL is returned.
 *
 *---------------------------------------------------------------------------
 */
static Column *
NextTaggedColumn(ColumnIterator *iterPtr)
{
    BLT_TABLE_COLUMN col;
            
    switch (iterPtr->type) {
    case ITER_TAG:
        col = blt_table_next_tagged_column(&iterPtr->cursor);
        if (col != NULL) {
            return GetColumnContainer(iterPtr->viewPtr, col);
        }
        break;

    case ITER_SINGLE:
    default:
        break;
    }   
    return NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * FirstTaggedColumn --
 *
 *      Returns the first column derived from the given tag.
 *
 * Results:
 *      Returns the first column in the sequence.  If no more columns are in
 *      the list, then NULL is returned.
 *
 *---------------------------------------------------------------------------
 */
static Column *
FirstTaggedColumn(ColumnIterator *iterPtr)
{
    BLT_TABLE_COLUMN col;

    switch (iterPtr->type) {
    case ITER_TAG:
        col = blt_table_first_tagged_column(&iterPtr->cursor);
        if (col != NULL) {
            return GetColumnContainer(iterPtr->viewPtr, col);
        }
        break;
    case ITER_SINGLE:
        return iterPtr->startPtr;
    default:
        break;
    } 
    return NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * NextTaggedRow --
 *
 *      Returns the next row derived from the given tag.
 *
 * Results:
 *      Returns the pointer to the next row in the iterator.  If no more
 *      rows are available, then NULL is returned.
 *
 *---------------------------------------------------------------------------
 */
static Row *
NextTaggedRow(RowIterator *iterPtr)
{
    BLT_TABLE_ROW row;

    switch (iterPtr->type) {
    case ITER_TAG:
        row = blt_table_next_tagged_row(&iterPtr->cursor);
        if (row != NULL) {
            return GetRowContainer(iterPtr->viewPtr, row);
        }
        break;
    case ITER_SINGLE:
    default:
        break;
    }   
    return NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * FirstTaggedRow --
 *
 *      Returns the first row derived from the given tag.
 *
 *---------------------------------------------------------------------------
 */
static Row *
FirstTaggedRow(RowIterator *iterPtr)
{
    BLT_TABLE_ROW row;

    switch (iterPtr->type) {
    case ITER_TAG:
        row = blt_table_first_tagged_row(&iterPtr->cursor);
        if (row != NULL) {
            return GetRowContainer(iterPtr->viewPtr, row);
        }
        break;
    case ITER_SINGLE:
        return iterPtr->startPtr;
    default:
        break;
    } 
    return NULL;
}

static Blt_Chain 
IterateRowsObjv(Tcl_Interp *interp, TableView *viewPtr, int objc, 
                Tcl_Obj *const *objv)
{
    Blt_HashTable rowTable;
    Blt_Chain chain;
    int i;

    chain = Blt_Chain_Create();
    if (viewPtr->table == NULL) {
        return chain;
    }
    Blt_InitHashTableWithPool(&rowTable, BLT_ONE_WORD_KEYS);
    for (i = 0; i < objc; i++) {
        BLT_TABLE_ITERATOR iter;
        BLT_TABLE_ROW row;
        Row *rowPtr;
        
        if (GetRowFromObj(NULL, viewPtr, objv[i], &rowPtr) == TCL_OK) {
            if (rowPtr != NULL) {
                int isNew;

                Blt_CreateHashEntry(&rowTable, (char *)rowPtr->row, &isNew);
                if (isNew) {
                    Blt_Chain_Append(chain, rowPtr);
                }
                continue;
            }
        }
        if (blt_table_iterate_rows(interp, viewPtr->table, objv[i], &iter)
            != TCL_OK){
            Blt_DeleteHashTable(&rowTable);
            Blt_Chain_Destroy(chain);
            return NULL;
        }
        /* Append the new rows onto the chain. */
        for (row = blt_table_first_tagged_row(&iter); row != NULL; 
             row = blt_table_next_tagged_row(&iter)) {
            int isNew;

            Blt_CreateHashEntry(&rowTable, (char *)row, &isNew);
            if (isNew) {
                Blt_Chain_Append(chain, GetRowContainer(viewPtr, row));
            }
        }
    }
    Blt_DeleteHashTable(&rowTable);
    return chain;
}

static Blt_Chain 
IterateColumnsObjv(Tcl_Interp *interp, TableView *viewPtr, int objc, 
                Tcl_Obj *const *objv)
{
    Blt_HashTable colTable;
    Blt_Chain chain;
    int i;

    chain = Blt_Chain_Create();
    if (viewPtr->table == NULL) {
        return chain;
    }
    Blt_InitHashTableWithPool(&colTable, BLT_ONE_WORD_KEYS);
    for (i = 0; i < objc; i++) {
        BLT_TABLE_ITERATOR iter;
        BLT_TABLE_COLUMN col;
        Column *colPtr;
        
        if (GetColumnFromObj(NULL, viewPtr, objv[i], &colPtr) == TCL_OK) {
            if (colPtr != NULL) {
                int isNew;

                Blt_CreateHashEntry(&colTable, (char *)colPtr->column, &isNew);
                if (isNew) {
                    Blt_Chain_Append(chain, colPtr);
                }
                continue;
            }
        }
        if (blt_table_iterate_columns(interp, viewPtr->table, objv[i], &iter)
            != TCL_OK){
            Blt_DeleteHashTable(&colTable);
            Blt_Chain_Destroy(chain);
            return NULL;
        }
        /* Append the new columns onto the chain. */
        for (col = blt_table_first_tagged_column(&iter); col != NULL; 
             col = blt_table_next_tagged_column(&iter)) {
            int isNew;

            Blt_CreateHashEntry(&colTable, (char *)col, &isNew);
            if (isNew) {
                Blt_Chain_Append(chain, GetColumnContainer(viewPtr, col));
            }
        }
    }
    Blt_DeleteHashTable(&colTable);
    return chain;
}

static Cell *
GetCell(TableView *viewPtr, Row *rowPtr, Column *colPtr)
{
    CellKey key;
    Blt_HashEntry *hPtr;

    key.rowPtr = rowPtr;
    key.colPtr = colPtr;
    hPtr = Blt_FindHashEntry(&viewPtr->cellTable, (char *)&key);
    if (hPtr == NULL) {
        return NULL;
    }
    return Blt_GetHashValue(hPtr);
}

static int
GetCellByIndex(Tcl_Interp *interp, TableView *viewPtr, Tcl_Obj *objPtr, 
               Cell **cellPtrPtr)
{
    char c;
    const char *string;
    int length;

    *cellPtrPtr = NULL;
    string = Tcl_GetStringFromObj(objPtr, &length);
    c = string[0];
    if (c == '@') {
        int x, y;

        if (Blt_GetXY(NULL, viewPtr->tkwin, string, &x, &y) == TCL_OK) {
            Column *colPtr;
            Row *rowPtr;

            colPtr = NearestColumn(viewPtr, x, FALSE);
            rowPtr = NearestRow(viewPtr, y, FALSE);
            if ((rowPtr != NULL) && (colPtr != NULL)) {
                *cellPtrPtr = GetCell(viewPtr, rowPtr, colPtr);
            }
        }
        return TCL_OK;
    } else if ((c == 'a') && (length > 1) && 
               (strncmp(string, "active", length) == 0)) {
        *cellPtrPtr = viewPtr->activePtr;
        return TCL_OK;
    } else if ((c == 'f') && (strncmp(string, "focus", length) == 0)) {
        *cellPtrPtr = viewPtr->focusPtr;
        return TCL_OK;
    } else if ((c == 'n') && (strncmp(string, "none", length) == 0)) {
        *cellPtrPtr = NULL;
        return TCL_OK;
    } else if ((c == 'c') && (strncmp(string, "current", length) == 0)) {
        TableObj *objPtr;

        objPtr = Blt_GetCurrentItem(viewPtr->bindTable);
        if ((objPtr != NULL) && ((objPtr->flags & DELETED) == 0)) {
            ItemType type;
            
            type = (ItemType)Blt_GetCurrentHint(viewPtr->bindTable);
            if (type == ITEM_CELL) {
                *cellPtrPtr = (Cell *)objPtr;
            }
        }
        return TCL_OK;
    } else if ((c == 'l') && (strncmp(string, "left", length) == 0)) {
        if (viewPtr->focusPtr != NULL) {
            Column *colPtr;
            CellKey *keyPtr;

            keyPtr = GetKey(viewPtr, viewPtr->focusPtr);
            colPtr = GetPrevColumn(keyPtr->colPtr);
            if (colPtr != NULL) {
                *cellPtrPtr = GetCell(viewPtr, keyPtr->rowPtr, colPtr);

            }
        }
        return TCL_OK;
    } else if ((c == 'r') && (strncmp(string, "right", length) == 0)) {
        if (viewPtr->focusPtr != NULL) {
            Column *colPtr;
            CellKey *keyPtr;
            
            keyPtr = GetKey(viewPtr, viewPtr->focusPtr);
            colPtr = GetNextColumn(keyPtr->colPtr);
            if (colPtr != NULL) {
                *cellPtrPtr = GetCell(viewPtr, keyPtr->rowPtr, colPtr);
            }
        }
        return TCL_OK;
    } else if ((c == 'u') && (strncmp(string, "up", length) == 0)) {
        if (viewPtr->focusPtr != NULL) {
            Row *rowPtr;
            CellKey *keyPtr;
            
            keyPtr = GetKey(viewPtr, viewPtr->focusPtr);
            rowPtr = GetPrevRow(keyPtr->rowPtr);
            if (rowPtr != NULL) {
                *cellPtrPtr = GetCell(viewPtr, rowPtr, keyPtr->colPtr);
            }
        }
        return TCL_OK;
    } else if ((c == 'd') && (strncmp(string, "down", length) == 0)) {
        if (viewPtr->focusPtr != NULL) {
            Row *rowPtr;
            CellKey *keyPtr;
            
            keyPtr = GetKey(viewPtr, viewPtr->focusPtr);
            rowPtr = GetNextRow(keyPtr->rowPtr);
            if (rowPtr != NULL) {
                *cellPtrPtr = GetCell(viewPtr, rowPtr, keyPtr->colPtr);
            }
        }
        return TCL_OK;
    } else if ((c == 'm') && (strncmp(string, "mark", length) == 0)) {
        CellSelection *selPtr = &viewPtr->selectCells;

        if (selPtr->markPtr != NULL) {
            *cellPtrPtr = GetCell(viewPtr, selPtr->markPtr->rowPtr,
                                  selPtr->markPtr->colPtr);
        }
        return TCL_OK;
    } else if ((c == 'a') && (length > 1) && 
               (strncmp(string, "anchor", length) == 0)) {
        CellSelection *selPtr = &viewPtr->selectCells;

        if (selPtr->anchorPtr != NULL) {
            *cellPtrPtr = GetCell(viewPtr, selPtr->anchorPtr->rowPtr,
                selPtr->anchorPtr->colPtr);
        }
        return TCL_OK;
    } 
    return TCL_CONTINUE;
}

/* 
 * GetCellFromObj --
 *
 *      @x,y
 *      active
 *      focus
 *      none
 *      current
 *      left
 *      right
 *      up
 *      down
 *      mark
 *      anchor
 *
 *      or list of {row column}, where row is a row index or tag and
 *      column is a column index or tag.
 */
static int
GetCellFromObj(Tcl_Interp *interp, TableView *viewPtr, Tcl_Obj *objPtr, 
               Cell **cellPtrPtr)
{
    int objc;
    Tcl_Obj **objv;
    Row *rowPtr;
    Column *colPtr;

    *cellPtrPtr = NULL;
    if (GetCellByIndex(interp, viewPtr, objPtr, cellPtrPtr) == TCL_OK) {
        return TCL_OK;
    }
    /*
     * Pick apart the cell descriptor to get the row and columns.
     */
    if (Tcl_ListObjGetElements(interp, objPtr, &objc, &objv) != TCL_OK) {
        return TCL_ERROR;
    }
    if (objc != 2) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "wrong # elements in cell index \"",
                             Tcl_GetString(objPtr), "\"", (char *)NULL);
        }
        return TCL_ERROR;
    }
    if ((GetRowFromObj(interp, viewPtr, objv[0], &rowPtr) != TCL_OK) ||
        (GetColumnFromObj(interp, viewPtr, objv[1], &colPtr) != TCL_OK)) {
        return TCL_ERROR;
    }
    if ((colPtr != NULL) && (rowPtr != NULL)) {
        *cellPtrPtr = GetCell(viewPtr, rowPtr, colPtr);
    }
    return TCL_OK;
}

static int
GetCellsFromObj(Tcl_Interp *interp, TableView *viewPtr, Tcl_Obj *objPtr, 
                Blt_Chain *cellsPtr)
{
    Blt_Chain cells;
    Cell *cellPtr;
    ColumnIterator ci;
    Row *rowPtr;
    RowIterator ri;
    Tcl_Obj **objv;
    int objc;

    cellPtr = NULL;
    if (GetCellByIndex(interp, viewPtr, objPtr, &cellPtr) == TCL_OK) {
        cells = Blt_Chain_Create();
        Blt_Chain_Append(cells, cellPtr);
        *cellsPtr = cells;
        return TCL_OK;
    }
    /*
     * Pick apart the cell descriptor to get the row and columns.
     */
    if (Tcl_ListObjGetElements(interp, objPtr, &objc, &objv) != TCL_OK) {
        return TCL_ERROR;
    }
    if (objc != 2) {
        if (interp != NULL) {
            Tcl_AppendResult(interp, "wrong # elements in cell index \"",
                             Tcl_GetString(objPtr), "\"", (char *)NULL);
        }
        return TCL_ERROR;
    }
    if ((GetRowIterator(interp, viewPtr, objv[0], &ri) != TCL_OK) ||
        (GetColumnIterator(interp, viewPtr, objv[1], &ci) != TCL_OK)) {
        return TCL_ERROR;
    }
    cells = Blt_Chain_Create();
    for (rowPtr = FirstTaggedRow(&ri); rowPtr != NULL; 
         rowPtr = NextTaggedRow(&ri)) {
        Column *colPtr;

        for (colPtr = FirstTaggedColumn(&ci); colPtr != NULL; 
             colPtr = NextTaggedColumn(&ci)) {
            cellPtr = GetCell(viewPtr, rowPtr, colPtr);
            Blt_Chain_Append(cells, cellPtr);
        }
    }
    *cellsPtr = cells;
    return TCL_OK;
}

static Blt_Chain 
IterateCellsObjv(Tcl_Interp *interp, TableView *viewPtr, int objc, 
                Tcl_Obj *const *objv)
{
    Blt_HashTable cellTable;
    Blt_Chain chain;
    int i;

    chain = Blt_Chain_Create();
    if (viewPtr->table == NULL) {
        return chain;
    }
    Blt_InitHashTableWithPool(&cellTable, sizeof(CellKey)/sizeof(int));
    for (i = 0; i < objc; i++) {
        Blt_Chain cells;
        Blt_ChainLink link;

        if (GetCellsFromObj(interp, viewPtr, objv[i], &cells) != TCL_OK) {
            Blt_Chain_Destroy(chain);
            return NULL;
        }
        for (link = Blt_Chain_FirstLink(cells); link != NULL;
             link = Blt_Chain_NextLink(link)) {
            Cell *cellPtr;

            cellPtr = Blt_Chain_GetValue(link);
            if (cellPtr != NULL) {
                int isNew;
                CellKey *keyPtr;

                keyPtr = GetKey(viewPtr, cellPtr);
                Blt_CreateHashEntry(&cellTable, (char *)keyPtr, &isNew);
                if (isNew) {
                    Blt_Chain_Append(chain, cellPtr);
                }
            }
        }
        Blt_Chain_Destroy(cells);
    }
    Blt_DeleteHashTable(&cellTable);
    return chain;
}

static CellStyle *
GetCurrentStyle(TableView *viewPtr, Row *rowPtr, Column *colPtr, Cell *cellPtr)
{
    if ((cellPtr != NULL) && (cellPtr->stylePtr != NULL)) {
        return cellPtr->stylePtr;
    }
    if ((rowPtr != NULL) && (rowPtr->stylePtr != NULL)) {
        return rowPtr->stylePtr;
    }
    if ((colPtr != NULL) && (colPtr->stylePtr != NULL)) {
        return colPtr->stylePtr;
    }
    return viewPtr->stylePtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * LostSelection --
 *
 *      This procedure is called back by Tk when the selection is grabbed
 *      away.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The existing selection is unhighlighted, and the window is marked
 *      as not containing a selection.
 *
 *---------------------------------------------------------------------------
 */
static void
LostSelection(ClientData clientData)
{
    TableView *viewPtr = clientData;

    if (viewPtr->flags & SELECT_EXPORT) {
        ClearSelections(viewPtr);
        EventuallyRedraw(viewPtr);
    }
}


/*
 *---------------------------------------------------------------------------
 *
 * SelectRows --
 *
 *      Sets the selection flag for a range of nodes.  The range is
 *      determined by two pointers which designate the first/last nodes of
 *      the range.
 *
 * Results:
 *      Always returns TCL_OK.
 *
 *---------------------------------------------------------------------------
 */
static int
SelectRows(TableView *viewPtr, Row *fromPtr, Row *toPtr)
{
    RenumberRows(viewPtr);
    if (fromPtr->index > toPtr->index) {
        Row *rowPtr;

        for (rowPtr = fromPtr; rowPtr != NULL; rowPtr = rowPtr->prevPtr) {
            if ((rowPtr->flags & HIDDEN) == 0) {
                switch (viewPtr->rows.selection.flags & SELECT_MASK) {
                case SELECT_CLEAR:
                    DeselectRow(viewPtr, rowPtr); break;
                case SELECT_SET:
                    SelectRow(viewPtr, rowPtr);   break;
                case SELECT_TOGGLE:
                    if (rowPtr->flags & SELECTED) {
                        DeselectRow(viewPtr, rowPtr);
                    } else {
                        SelectRow(viewPtr, rowPtr);
                    }
                    break;
                }
            }
            if (rowPtr == toPtr) {
                break;
            }
        }
    } else {
        Row *rowPtr;

        for (rowPtr = fromPtr; rowPtr != NULL; rowPtr = rowPtr->nextPtr) {
            if ((rowPtr->flags & HIDDEN) == 0) {
                switch (viewPtr->rows.selection.flags & SELECT_MASK) {
                case SELECT_CLEAR:
                    DeselectRow(viewPtr, rowPtr);   
                    break;
                case SELECT_SET:
                    SelectRow(viewPtr, rowPtr);     
                    break;
                case SELECT_TOGGLE:
                    if (rowPtr->flags & SELECTED) {
                        DeselectRow(viewPtr, rowPtr);
                    } else {
                        SelectRow(viewPtr, rowPtr);
                    }
                    break;
                }
            }
            if (rowPtr == toPtr) {
                break;
            }
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SelectRange --
 *
 *      Sets the selection flag for a range of nodes.  The range is
 *      determined by two pointers which designate the first/last nodes of
 *      the range.
 *
 * Results:
 *      Always returns TCL_OK.
 *
 *---------------------------------------------------------------------------
 */
static void
AddSelectionRange(TableView *viewPtr)
{
    CellKey key;
    CellSelection *selPtr;
    Column *colFirstPtr, *colLastPtr;
    Row *rowPtr, *rowFirstPtr, *rowLastPtr;

    selPtr = &viewPtr->selectCells;
    if (selPtr->anchorPtr == NULL) {
        return;
    }
    if (selPtr->anchorPtr->rowPtr->index > selPtr->markPtr->rowPtr->index) {
        rowLastPtr = selPtr->anchorPtr->rowPtr;
        rowFirstPtr = selPtr->markPtr->rowPtr;
    } else {
        rowFirstPtr = selPtr->anchorPtr->rowPtr;
        rowLastPtr = selPtr->markPtr->rowPtr;
    }        
    if (selPtr->anchorPtr->colPtr->index > selPtr->markPtr->colPtr->index) {
        colLastPtr = selPtr->anchorPtr->colPtr;
        colFirstPtr = selPtr->markPtr->colPtr;
    } else {
        colFirstPtr = selPtr->anchorPtr->colPtr;
        colLastPtr = selPtr->markPtr->colPtr;
    }        
    for (rowPtr = rowFirstPtr; rowPtr != NULL; rowPtr = rowPtr->nextPtr) {
        Column *colPtr;

        key.rowPtr = rowPtr;
        for (colPtr = colFirstPtr; colPtr != NULL; colPtr = colPtr->nextPtr) {
            int isNew;

            key.colPtr = colPtr;
            Blt_CreateHashEntry(&selPtr->cellTable, &key, &isNew);
            if (colPtr == colLastPtr) {
                break;
            }
        }
        if (rowPtr == rowLastPtr) {
            break;
        }
    }
    selPtr->markPtr = selPtr->anchorPtr = NULL;
}

/*
 *---------------------------------------------------------------------------
 *
 * GetSelectedCells --
 *
 *      Sets the selection flag for a range of nodes.  The range is
 *      determined by two pointers which designate the first/last nodes of
 *      the range.
 *
 * Results:
 *      Always returns TCL_OK.
 *
 *---------------------------------------------------------------------------
 */
static void
GetSelectedCells(TableView *viewPtr, CellKey *anchorPtr, CellKey *markPtr)
{
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;
    CellSelection *selPtr;
    Column *minColPtr, *maxColPtr;
    Row *minRowPtr, *maxRowPtr;

    selPtr = &viewPtr->selectCells;
    minRowPtr = maxRowPtr = NULL;
    minColPtr = maxColPtr = NULL;
    for (hPtr = Blt_FirstHashEntry(&selPtr->cellTable, &iter); 
         hPtr != NULL; hPtr = Blt_NextHashEntry(&iter)) {
        CellKey *keyPtr;

        keyPtr = (CellKey *)Blt_GetHashKey(&selPtr->cellTable, hPtr);
        if ((minRowPtr == NULL) || (minRowPtr->index > keyPtr->rowPtr->index)){
            minRowPtr = keyPtr->rowPtr;
        } 
        if ((maxRowPtr == NULL) || (maxRowPtr->index < keyPtr->rowPtr->index)){
            maxRowPtr = keyPtr->rowPtr;
        } 
        if ((minColPtr == NULL) || (minColPtr->index > keyPtr->colPtr->index)){
            minColPtr = keyPtr->colPtr;
        } 
        if ((maxColPtr == NULL) || (maxColPtr->index < keyPtr->colPtr->index)){
            maxColPtr = keyPtr->colPtr;
        } 
    }        
    anchorPtr->rowPtr = minRowPtr;
    anchorPtr->colPtr = minColPtr;
    markPtr->rowPtr = maxRowPtr;
    markPtr->colPtr = maxColPtr;
}

static void
GetSelectedRows(TableView *viewPtr, CellKey *anchorPtr, CellKey *markPtr)
{
    Row *rowPtr;
    Column *colPtr;
    CellSelection *selPtr;

    selPtr = &viewPtr->selectCells;
    for (rowPtr = anchorPtr->rowPtr; rowPtr != NULL; rowPtr = rowPtr->nextPtr) {
        int selected;

        selected = FALSE;
        rowPtr->flags &= ~HAS_SELECTION;
        for (colPtr = anchorPtr->colPtr; colPtr != NULL; 
             colPtr = colPtr->nextPtr) {
            CellKey key;

            key.colPtr = colPtr;
            key.rowPtr = rowPtr;
            if (Blt_FindHashEntry(&selPtr->cellTable, &key) != NULL) {
                selected = TRUE;
                break;
            }
            if (colPtr == markPtr->colPtr) {
                break;
            }
        }
        if (selected) {
            rowPtr->flags |= HAS_SELECTION;
        }
        if (rowPtr == markPtr->rowPtr) {
            break;
        }
    }
}

static void
GetSelectedColumns(TableView *viewPtr, CellKey *anchorPtr, CellKey *markPtr)
{
    Row *rowPtr;
    Column *colPtr;
    CellSelection *selPtr;

    selPtr = &viewPtr->selectCells;
    for (colPtr = anchorPtr->colPtr; colPtr != NULL; colPtr = colPtr->nextPtr) {
        int selected;

        selected = FALSE;
        colPtr->flags &= ~HAS_SELECTION;
        for (rowPtr = anchorPtr->rowPtr; rowPtr != NULL;
             rowPtr = rowPtr->nextPtr) {
            CellKey key;

            key.colPtr = colPtr;
            key.rowPtr = rowPtr;
            if (Blt_FindHashEntry(&selPtr->cellTable, &key) != NULL) {
                selected = TRUE;
                break;
            }
            if (rowPtr == markPtr->rowPtr) {
                break;
            }
        }
        if (selected) {
            colPtr->flags |= HAS_SELECTION;
        }
        if (colPtr == markPtr->colPtr) {
            break;
        }
    }
}

static void
ComputeRowTitleGeometry(TableView *viewPtr, Row *rowPtr)
{
    unsigned int iw, ih, tw, th;
    unsigned int gap;
    const char *title;
    
    rowPtr->titleWidth = 2 * (viewPtr->rows.titleBorderWidth + TITLE_PADX);
    rowPtr->titleHeight = 2 * (viewPtr->rows.titleBorderWidth + TITLE_PADY);
        
    gap = tw = th = iw = ih = 0;
    if (rowPtr->icon != NULL) {
        iw = IconWidth(rowPtr->icon);
        ih = IconHeight(rowPtr->icon);
        rowPtr->titleWidth += iw;
    }
    title = GetRowTitle(rowPtr);
    if (title != NULL) {
        TextStyle ts;

        Blt_Ts_InitStyle(ts);
        Blt_Ts_SetFont(ts, viewPtr->rows.titleFont);
        Blt_Ts_GetExtents(&ts, title, &tw, &th);
    }
    gap = ((iw > 0) && (tw > 0)) ? 2 : 0;
    rowPtr->titleHeight += MAX(ih, th);
    rowPtr->titleWidth += tw + gap + iw;
}


static int 
ConfigureRow(TableView *viewPtr, Row *rowPtr) 
{
    if (Blt_ConfigModified(rowSpecs, "-titlefont", "-title", "-hide", "-icon", 
        "-show", "-borderwidth", (char *)NULL)) {
        if (viewPtr->rows.flags & SHOW_TITLES) {
            ComputeRowTitleGeometry(viewPtr, rowPtr);
        } 
    }
    if (Blt_ConfigModified(rowSpecs, "-style", (char *)NULL)) {
        /* If the style changed, recompute the geometry of the cells. */
        rowPtr->flags |= GEOMETRY;
        viewPtr->flags |= GEOMETRY;
    }
    return TCL_OK;
}


#ifdef notdef
static void
PrintEventFlags(int type)
{
    Tcl_DString ds;

    Tcl_DStringInit(&ds);
    Tcl_DStringAppend(&ds "event flags are: ", -1);
    if (type & TABLE_NOTIFY_COLUMN_CHANGED) {
        Tcl_DStringAppend(&ds "-column ", -1);
    } 
    if (type & TABLE_NOTIFY_ROW_CHANGED) {
        Tcl_DStringAppend(&ds "-row ", -1);
    } 
    if (type & TABLE_NOTIFY_CREATE) {
        Tcl_DStringAppend(&ds "-create ", -1);
    } 
    if (type & TABLE_NOTIFY_DELETE) {
        Tcl_DStringAppend(&ds "-delete ", -1);
    }
    if (type & TABLE_NOTIFY_MOVE) {
        Tcl_DStringAppend(&ds "-move ", -1);
    }
    if (type & TABLE_NOTIFY_RELABEL) {
        Tcl_DStringAppend(&ds "-relabel ", -1);
    }
    fprintf(stderr, "%s\n", Tcl_DStringValue(&ds));
    Tcl_DStringFree(&ds);
}
#endif


static Cell *
NewCell(TableView *viewPtr, Blt_HashEntry *hashPtr)
{
    Cell *cellPtr;

    cellPtr = Blt_Pool_AllocItem(viewPtr->cellPool, sizeof(Cell));
    cellPtr->hashPtr = hashPtr;
    cellPtr->flags = GEOMETRY;
    cellPtr->text = NULL;
    cellPtr->tkImage = NULL;
    cellPtr->stylePtr = NULL;
    cellPtr->width = cellPtr->height = 0;
    return cellPtr;
}

static void
ReorderColumns(TableView *viewPtr)
{
    BLT_TABLE_COLUMN col;
    Column *lastPtr;
    size_t i;

    lastPtr = NULL;
    for (i = 0, col = blt_table_first_column(viewPtr->table); col != NULL; 
         col = blt_table_next_column(col), i++) {
        Column *colPtr;

        colPtr = GetColumnContainer(viewPtr, col);
        assert(colPtr != NULL);
        viewPtr->columns.map[i] = colPtr;
        if (lastPtr != NULL) {
            lastPtr->nextPtr = colPtr;
        }
        colPtr->prevPtr = lastPtr;
        lastPtr = colPtr;
    }
    viewPtr->columns.firstPtr = viewPtr->columns.map[0];
    viewPtr->columns.lastPtr = viewPtr->columns.map[i - 1];
    viewPtr->flags |= LAYOUT_PENDING;
    EventuallyRedraw(viewPtr);
}


static void
ReorderRows(TableView *viewPtr)
{
    BLT_TABLE_ROW row;
    Row *lastPtr;
    size_t i;

    lastPtr = NULL;
    for (i = 0, row = blt_table_first_row(viewPtr->table); row != NULL; 
         row = blt_table_next_row(row), i++) {
        Row *rowPtr;

        rowPtr = GetRowContainer(viewPtr, row);
        assert(rowPtr != NULL);
        viewPtr->rows.map[i] = rowPtr;
        if (lastPtr != NULL) {
            lastPtr->nextPtr = rowPtr;
        }
        rowPtr->prevPtr = lastPtr;
        lastPtr = rowPtr;
    }
    viewPtr->rows.firstPtr = viewPtr->rows.map[0];
    viewPtr->rows.lastPtr = viewPtr->rows.map[i - 1];
    viewPtr->flags |= LAYOUT_PENDING;
    EventuallyRedraw(viewPtr);
}

static void
ComputeCellGeometry(TableView *viewPtr, Cell *cellPtr)
{
    CellStyle *stylePtr;
    CellKey *keyPtr;

    keyPtr = GetKey(viewPtr, cellPtr);
    stylePtr = GetCurrentStyle(viewPtr, keyPtr->rowPtr, keyPtr->colPtr,
         cellPtr);
    (*stylePtr->classPtr->geomProc)(viewPtr, cellPtr, stylePtr);
}
    
static void
AddCellGeometry(TableView *viewPtr, Cell *cellPtr)
{
    CellKey *keyPtr;
    Column *colPtr;
    Row *rowPtr;

    keyPtr = GetKey(viewPtr, cellPtr);
    rowPtr = keyPtr->rowPtr;
    colPtr = keyPtr->colPtr;
    ComputeCellGeometry(viewPtr, cellPtr);
    /* Override the initial width of the cell if it exceeds the designated
     * maximum.  */
    if ((viewPtr->columns.maxWidth > 0) && 
        (cellPtr->width > viewPtr->columns.maxWidth)) {
        cellPtr->width = viewPtr->columns.maxWidth;
    }
    if (cellPtr->width > colPtr->nom) {
        colPtr->nom = cellPtr->width;
    }
    /* Override the initial height of the cell if it exceeds the designated
     * maximum.  */
    if ((viewPtr->rows.maxHeight > 0) && 
        (cellPtr->height > viewPtr->rows.maxHeight)) {
        cellPtr->height = viewPtr->rows.maxHeight;
    }
    if (cellPtr->height > rowPtr->nom) {
        rowPtr->nom = cellPtr->height;
    }
}

static void
AddColumnTitleGeometry(TableView *viewPtr, Column *colPtr)
{
    if (colPtr->flags & GEOMETRY) {
        if (viewPtr->columns.flags & SHOW_TITLES) {
            ComputeColumnTitleGeometry(viewPtr, colPtr);
        } else {
            colPtr->titleWidth = colPtr->titleHeight = 0;
        }
    }
    colPtr->nom = colPtr->titleWidth;
    if ((colPtr->flags & HIDDEN) == 0) {
        if (colPtr->titleHeight > viewPtr->columns.titleHeight) {
            viewPtr->columns.titleHeight = colPtr->titleHeight;
        }
    }
    if (viewPtr->flags & COLUMN_FILTERS) {
        ComputeColumnFiltersGeometry(viewPtr);
    }
}

static void
AddRowTitleGeometry(TableView *viewPtr, Row *rowPtr)
{
    if (rowPtr->flags & GEOMETRY) {
        if (viewPtr->rows.flags & SHOW_TITLES) {
            ComputeRowTitleGeometry(viewPtr, rowPtr);
        } else {
            rowPtr->titleHeight = rowPtr->titleWidth = 0;
        }
    }
    rowPtr->nom = rowPtr->titleHeight;
    if ((rowPtr->flags & HIDDEN) == 0) {
        if (rowPtr->titleWidth > viewPtr->rows.titleWidth) {
            viewPtr->rows.titleWidth = rowPtr->titleWidth;
        }
    }
    if (viewPtr->flags & COLUMN_FILTERS) {
        ComputeColumnFiltersGeometry(viewPtr);
    }
}

static void
AddRow(TableView *viewPtr, BLT_TABLE_ROW row)
{
    Blt_HashEntry *hPtr;
    CellKey key;
    Column *colPtr;
    Row *rowPtr;
    int isNew;

    hPtr = Blt_CreateHashEntry(&viewPtr->rows.table, (char *)row, &isNew);
    assert(isNew);
    rowPtr = CreateRow(viewPtr, row, hPtr);
    AddRowTitleGeometry(viewPtr, rowPtr);
    key.rowPtr = rowPtr;
    for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
         colPtr = colPtr->nextPtr) {
        Cell *cellPtr;
        Blt_HashEntry *h2Ptr;
        int isNew;
        
        key.colPtr = colPtr;
        h2Ptr = Blt_CreateHashEntry(&viewPtr->cellTable, (char *)&key, &isNew);
        assert(isNew);
        cellPtr = NewCell(viewPtr, h2Ptr);
        AddCellGeometry(viewPtr, cellPtr);
        Blt_SetHashValue(h2Ptr, cellPtr);
    }
    viewPtr->flags |= GEOMETRY | LAYOUT_PENDING;
    viewPtr->rows.flags |= REINDEX;
    PossiblyRedraw(viewPtr);
}

static void
AddColumn(TableView *viewPtr, BLT_TABLE_COLUMN col)
{
    Blt_HashEntry *hPtr;
    CellKey key;
    Column *colPtr;
    Row *rowPtr;
    int isNew;

    hPtr = Blt_CreateHashEntry(&viewPtr->columns.table, (char *)col, &isNew);
    assert(isNew);
    colPtr = CreateColumn(viewPtr, col, hPtr);
    AddColumnTitleGeometry(viewPtr, colPtr);
    key.colPtr = colPtr;
    for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
         rowPtr = rowPtr->nextPtr) {
        Cell *cellPtr;
        Blt_HashEntry *h2Ptr;
        int isNew;
        
        key.rowPtr = rowPtr;
        h2Ptr = Blt_CreateHashEntry(&viewPtr->cellTable, (char *)&key, &isNew);
        assert(isNew);
        cellPtr = NewCell(viewPtr, h2Ptr);
        AddCellGeometry(viewPtr, cellPtr);
        Blt_SetHashValue(h2Ptr, cellPtr);
    }
    viewPtr->flags |= GEOMETRY | LAYOUT_PENDING;
    viewPtr->columns.flags |= REINDEX;
    PossiblyRedraw(viewPtr);
}

static void
DeleteRow(TableView *viewPtr, BLT_TABLE_ROW row)
{
    Row *rowPtr;
    
    rowPtr = GetRowContainer(viewPtr, row);
    assert(rowPtr);
    RemoveRowCells(viewPtr, rowPtr);
    DestroyRow(rowPtr);
    viewPtr->flags |= LAYOUT_PENDING;
    viewPtr->rows.flags |= REINDEX;
    EventuallyRedraw(viewPtr);
}

static void
DeleteColumn(TableView *viewPtr, BLT_TABLE_COLUMN col)
{
    Column *colPtr;
    
    colPtr = GetColumnContainer(viewPtr, col);
    assert(colPtr);
    RemoveColumnCells(viewPtr, colPtr);
    DestroyColumn(colPtr);
    viewPtr->flags |= LAYOUT_PENDING;
    viewPtr->columns.flags |= REINDEX;
    EventuallyRedraw(viewPtr);
}

static int
TableEventProc(ClientData clientData, BLT_TABLE_NOTIFY_EVENT *eventPtr)
{
    TableView *viewPtr = clientData; 

   if (eventPtr->type & (TABLE_NOTIFY_DELETE|TABLE_NOTIFY_CREATE)) {
       if (eventPtr->type == TABLE_NOTIFY_ROWS_CREATED) {
           if (viewPtr->rows.flags & AUTO_MANAGE) {
               /* Add the row and eventually reindex */
               AddRow(viewPtr, eventPtr->row);
           }
       } else if (eventPtr->type == TABLE_NOTIFY_COLUMNS_CREATED) {
           if (viewPtr->columns.flags & AUTO_MANAGE) {
               /* Add the column and eventually reindex */
               AddColumn(viewPtr, eventPtr->column);
           }
       } else if (eventPtr->type == TABLE_NOTIFY_ROWS_DELETED) {
           if (viewPtr->rows.flags & AUTO_MANAGE) {
               /* Delete the row and eventually reindex */
               DeleteRow(viewPtr, eventPtr->row);
           }
       } else if (eventPtr->type == TABLE_NOTIFY_COLUMNS_DELETED) {
           if (viewPtr->columns.flags & AUTO_MANAGE) {
               /* Delete the column and eventually reindex */
               DeleteColumn(viewPtr, eventPtr->column);
           }
       }
       return TCL_OK;
    } 
    if (eventPtr->type & TABLE_NOTIFY_COLUMN_CHANGED) {
        if (eventPtr->type & TABLE_NOTIFY_RELABEL) {
            Column *colPtr;
            
            colPtr = GetColumnContainer(viewPtr, eventPtr->column);
            if ((colPtr != NULL) && (colPtr->titleObjPtr == NULL)) {
                ComputeColumnTitleGeometry(viewPtr, colPtr);
                viewPtr->flags |= LAYOUT_PENDING;
                EventuallyRedraw(viewPtr);
            }
        } else if (eventPtr->type & TABLE_NOTIFY_MOVE) {
            ReorderColumns(viewPtr);
            viewPtr->flags |= LAYOUT_PENDING;
            EventuallyRedraw(viewPtr);
        }       
    }
    if (eventPtr->type & TABLE_NOTIFY_ROW_CHANGED) {
        if (eventPtr->type & TABLE_NOTIFY_RELABEL) {
            Row *rowPtr;
            
            rowPtr = GetRowContainer(viewPtr, eventPtr->row);
            if ((rowPtr != NULL) && (rowPtr->titleObjPtr == NULL)) {
                ComputeRowTitleGeometry(viewPtr, rowPtr);
                viewPtr->flags |= LAYOUT_PENDING;
                EventuallyRedraw(viewPtr);
            }
        } else if (eventPtr->type & TABLE_NOTIFY_MOVE) {
            ReorderRows(viewPtr);
            viewPtr->flags |= LAYOUT_PENDING;
            EventuallyRedraw(viewPtr);
        }       
    }
    return TCL_OK;
}

static BindTag 
MakeBindTag(TableView *viewPtr, ClientData clientData, int type)
{
    Blt_HashEntry *hPtr;
    int isNew;                          /* Not used. */
    struct _BindTag tag;

    memset(&tag, 0, sizeof(tag));
    tag.type = type;
    tag.clientData = clientData;
    hPtr = Blt_CreateHashEntry(&viewPtr->bindTagTable, &tag, &isNew);
    return Blt_GetHashKey(&viewPtr->bindTagTable, hPtr);
}

static BindTag
MakeStringBindTag(TableView *viewPtr, const char *string, int type)
{
    Blt_HashEntry *hPtr;
    int isNew;                          /* Not used. */

    hPtr = Blt_CreateHashEntry(&viewPtr->uidTable, string, &isNew);
    return MakeBindTag(viewPtr, Blt_GetHashKey(&viewPtr->uidTable, hPtr), type);
}


static void
AddBindTags(TableView *viewPtr, Blt_Chain tags, Tcl_Obj *objPtr, int type)
{
    int objc;
    Tcl_Obj **objv;
    
    if (Tcl_ListObjGetElements(NULL, objPtr, &objc, &objv) == TCL_OK) {
        int i;

        for (i = 0; i < objc; i++) {
            const char *string;

            string = Tcl_GetString(objv[i]);
            Blt_Chain_Append(tags, MakeStringBindTag(viewPtr, string, type));
        }
    }
}

static void
AppendTagsProc(Blt_BindTable table, ClientData item, ClientData hint, 
               Blt_Chain tags)
{
    TableView *viewPtr;
    ItemType type = (ItemType)hint;
    TableObj *objPtr;
    
    objPtr = item;
    if (objPtr->flags & DELETED) {
        return;
    }
    viewPtr = Blt_GetBindingData(table);
    switch(type) {
    case ITEM_COLUMN_FILTER:
    case ITEM_COLUMN_TITLE:
    case ITEM_COLUMN_RESIZE:
        {
            Column *colPtr = item;
            
            Blt_Chain_Append(tags, MakeBindTag(viewPtr, item, type));
            if (colPtr->bindTagsObjPtr != NULL) {
                AddBindTags(viewPtr, tags, colPtr->bindTagsObjPtr, type);
            }
        }
        break;

    case ITEM_ROW_RESIZE:
    case ITEM_ROW_TITLE:
        {
            Row *rowPtr = item;
            
            Blt_Chain_Append(tags, MakeBindTag(viewPtr, item, type));
            if (rowPtr->bindTagsObjPtr != NULL) {
                AddBindTags(viewPtr, tags, rowPtr->bindTagsObjPtr, type);
            }
        }
        break;
        
    case ITEM_CELL:
        {
            Cell *cellPtr = item;
            CellStyle *stylePtr;
            CellKey *keyPtr;
            BindTag tag;
            
            keyPtr = GetKey(viewPtr, cellPtr);
            Blt_Chain_Append(tags, MakeBindTag(viewPtr, cellPtr, type));
            stylePtr = GetCurrentStyle(viewPtr, keyPtr->rowPtr, keyPtr->colPtr,
                                       cellPtr);
            Blt_Chain_Append(tags, MakeBindTag(viewPtr, keyPtr->rowPtr, type));
            Blt_Chain_Append(tags, MakeBindTag(viewPtr, keyPtr->colPtr, type));
            if (stylePtr->name != NULL) {
                /* Append cell style name. */
                tag = MakeStringBindTag(viewPtr, stylePtr->name, type);
                Blt_Chain_Append(tags, tag);
            }
            /* Append cell style class. */
            tag = MakeStringBindTag(viewPtr, stylePtr->classPtr->className,
                             type);
            Blt_Chain_Append(tags, tag);
            Blt_Chain_Append(tags, MakeStringBindTag(viewPtr, "all", type));
        }
        break;

    default:
        fprintf(stderr, "unknown item type (%d) %p\n", type, item);
        break;
    }
}

static void
ComputeGeometry(TableView *viewPtr)
{
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;
    Column *colPtr;
    Row *rowPtr;
    long i;

    viewPtr->flags &= ~GEOMETRY;        
    viewPtr->rows.titleWidth = viewPtr->columns.titleHeight = 0;

    /* Step 1. Set the initial size of the row or column by computing its
     *         title size. Get the geometry of hidden rows and columns so
     *         that it doesn't cost to show/hide them. */
    for (i = 0, colPtr = viewPtr->columns.firstPtr; colPtr != NULL;
         colPtr = colPtr->nextPtr, i++) {
        if (colPtr->flags & GEOMETRY) {
            if (viewPtr->columns.flags & SHOW_TITLES) {
                ComputeColumnTitleGeometry(viewPtr, colPtr);
            } else {
                colPtr->titleWidth = colPtr->titleHeight = 0;
            }
        }
        colPtr->index = i;
        colPtr->nom = colPtr->titleWidth;
        if ((colPtr->flags & HIDDEN) == 0) {
            if (colPtr->titleHeight > viewPtr->columns.titleHeight) {
                viewPtr->columns.titleHeight = colPtr->titleHeight;
            }
        }
    }
    for (i = 0, rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL;
         rowPtr = rowPtr->nextPtr, i++) {
        if (rowPtr->flags & GEOMETRY) {
            if (viewPtr->rows.flags & SHOW_TITLES) {
                ComputeRowTitleGeometry(viewPtr, rowPtr);
            } else {
                rowPtr->titleHeight = rowPtr->titleWidth = 0;
            }
        }
        rowPtr->index = i;
        rowPtr->nom = rowPtr->titleHeight;
        if ((rowPtr->flags & HIDDEN) == 0) {
            if (rowPtr->titleWidth > viewPtr->rows.titleWidth) {
                viewPtr->rows.titleWidth = rowPtr->titleWidth;
            }
        }
    }
    /* Step 2: Get the dimensions each cell (recomputing the geometry as
     *         needed) and compute the tallest/widest cell in each
     *         row/column. */
    for (hPtr = Blt_FirstHashEntry(&viewPtr->cellTable, &iter); hPtr != NULL;
         hPtr = Blt_NextHashEntry(&iter)) {
        CellKey *keyPtr;
        Row *rowPtr;
        Column *colPtr;
        Cell *cellPtr;
        
        keyPtr = (CellKey *)Blt_GetHashKey(&viewPtr->cellTable, hPtr);
        rowPtr = keyPtr->rowPtr;
        colPtr = keyPtr->colPtr;
        cellPtr = Blt_GetHashValue(hPtr);
        if ((rowPtr->flags|colPtr->flags|cellPtr->flags) & GEOMETRY) {
            ComputeCellGeometry(viewPtr, cellPtr);
        }
        /* Override the initial width of the cell if it exceeds the
         * designated maximum.  */
        if ((viewPtr->columns.maxWidth > 0) && 
            (cellPtr->width > viewPtr->columns.maxWidth)) {
            cellPtr->width = viewPtr->columns.maxWidth;
        }
        if (cellPtr->width > colPtr->nom) {
            colPtr->nom = cellPtr->width;
        }
        /* Override the initial height of the cell if it exceeds the
         * designated maximum.  */
        if ((viewPtr->rows.maxHeight > 0) && 
            (cellPtr->height > viewPtr->rows.maxHeight)) {
            cellPtr->height = viewPtr->rows.maxHeight;
        }
        if (cellPtr->height > rowPtr->nom) {
            rowPtr->nom = cellPtr->height;
        }
    }
    if (viewPtr->flags & COLUMN_FILTERS) {
        ComputeColumnFiltersGeometry(viewPtr);
    }
    viewPtr->flags |= LAYOUT_PENDING;
}

static void
AdjustColumns(TableView *viewPtr)
{
    Column *lastPtr;
    long x;
    Column *colPtr;
    double weight;
    int growth;
    long numOpen;

    growth = VPORTWIDTH(viewPtr) - viewPtr->worldWidth;
    assert(growth > 0);
    lastPtr = NULL;

    numOpen = 0;
    weight = 0.0;

    /* Find out how many columns still have space available */
    for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
         colPtr = colPtr->nextPtr) {
        if (colPtr->flags & HIDDEN) {
            continue;
        }
        lastPtr = colPtr;
        if ((colPtr->weight == 0.0) || (colPtr->width >= colPtr->max)) {
            continue;
        }
        numOpen++;
        weight += colPtr->weight;
    }
    while ((numOpen > 0) && (weight > 0.0) && (growth > 0)) {
        int ration;
        Column *colPtr;

        ration = (int)(growth / weight);
        if (ration == 0) {
            ration = 1;
        }
        for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
             colPtr = colPtr->nextPtr) {
            int size, avail;
            
            if ((numOpen <= 0) || (growth <= 0) || (weight <= 0.0)) {
                break;
            }
            if (colPtr->flags & HIDDEN) {
                continue;
            }
            lastPtr = colPtr;
            if ((colPtr->weight == 0.0) || (colPtr->width >= colPtr->max)) {
                continue;
            }
            size = (int)(ration * colPtr->weight);
            if (size > growth) {
                size = growth; 
            }
            avail = colPtr->max - colPtr->width;
            if (size > avail) {
                size = avail;
                numOpen--;
                weight -= colPtr->weight;
            }
            colPtr->width += size;
            growth -= size;
        }
    }
    if ((growth > 0) && (lastPtr != NULL)) {
        lastPtr->width += growth;
    }
    x = 0;
    for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
         colPtr = colPtr->nextPtr) {
        if (colPtr->flags & HIDDEN) {
            continue;                   /* Ignore hidden columns. */
        }
        colPtr->worldX = x;
        x += colPtr->width;
    }
}

static void
ComputeLayout(TableView *viewPtr)
{
    Column *colPtr;
    Row *rowPtr;
    long i;
    unsigned long x, y;

    viewPtr->flags &= ~LAYOUT_PENDING;
    x = y = 0;
    for (i = 0, rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
         rowPtr = rowPtr->nextPtr, i++) {
        rowPtr->flags &= ~GEOMETRY;     /* Always remove the geometry
                                         * flag. */
        rowPtr->index = i;              /* Reset the index. */
        rowPtr->height = GetBoundedHeight(rowPtr->nom, &rowPtr->reqHeight);
        if (rowPtr->reqHeight.flags & LIMITS_SET_NOM) {
            /*
             * This could be done more cleanly.  We want to ensure that the
             * requested nominal size is not overridden when determining
             * the normal sizes.  So temporarily fix min and max to the
             * nominal size and reset them back later.
             */
            rowPtr->min = rowPtr->max = rowPtr->height;
        } else {
            /* The range defaults to 0..MAXINT */
            rowPtr->min = rowPtr->reqHeight.min;
            rowPtr->max = rowPtr->reqHeight.max;
        }
        rowPtr->worldY = y;
        if ((rowPtr->flags & HIDDEN) == 0) {
            y += rowPtr->height;
        }
    }
    viewPtr->worldHeight = y;
#ifdef notdef
    if (viewPtr->worldHeight < VPORTHEIGHT(viewPtr)) {
        AdjustRows(viewPtr);
    }
#endif

    for (i = 0, colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
         colPtr = colPtr->nextPtr, i++) {

        colPtr->flags &= ~GEOMETRY;     /* Always remove the geometry
                                         * flag. */
        colPtr->index = i;              /* Reset the index. */
        colPtr->width = 0;
        colPtr->worldX = x;
        colPtr->width = GetBoundedWidth(colPtr->nom, &colPtr->reqWidth);
        if (colPtr->reqWidth.flags & LIMITS_SET_NOM) {
            /*
             * This could be done more cleanly.  We want to ensure that the
             * requested nominal size is not overridden when determining
             * the normal sizes.  So temporarily fix min and max to the
             * nominal size and reset them back later.
             */
            colPtr->min = colPtr->max = colPtr->width;
        } else {
            /* The range defaults to 0..MAXINT */
            colPtr->min = colPtr->reqWidth.min;
            colPtr->max = colPtr->reqWidth.max;
        }
        if ((colPtr->flags & HIDDEN) == 0) {
            x += colPtr->width;
        }
    }
    viewPtr->worldWidth = x;
    if (viewPtr->worldWidth < VPORTWIDTH(viewPtr)) {
        AdjustColumns(viewPtr);
    }
    viewPtr->height = viewPtr->worldHeight = y;
    viewPtr->width  = viewPtr->worldWidth  = x;
    viewPtr->width  += 2 * viewPtr->inset;
    viewPtr->height += 2 * viewPtr->inset;
    if (viewPtr->columns.flags & SHOW_TITLES) {
        viewPtr->height += viewPtr->columns.titleHeight;
    }
    if (viewPtr->flags & COLUMN_FILTERS) {
        viewPtr->height += viewPtr->columns.filterHeight;
    }
    if (viewPtr->rows.flags & SHOW_TITLES) {
        viewPtr->width += viewPtr->rows.titleWidth;
    }
    /* Flag to recompute visible rows and columns. */
    viewPtr->rows.flags |= SCROLL_PENDING; 
    viewPtr->columns.flags |= SCROLL_PENDING;
}

static void
ComputeVisibleEntries(TableView *viewPtr)
{
    int viewWidth, viewHeight;
    long xOffset, yOffset;
    long low, high;
    long first;

    if (viewPtr->rows.flags & REINDEX) {
        RenumberRows(viewPtr);
    }
    if (viewPtr->columns.flags & REINDEX) {
        RenumberColumns(viewPtr);
    }
    xOffset = Blt_AdjustViewport(viewPtr->columns.scrollOffset, 
        viewPtr->worldWidth, VPORTWIDTH(viewPtr), viewPtr->columns.scrollUnits,
        viewPtr->scrollMode);
    yOffset = Blt_AdjustViewport(viewPtr->rows.scrollOffset, 
        viewPtr->worldHeight, VPORTHEIGHT(viewPtr), viewPtr->rows.scrollUnits, 
        viewPtr->scrollMode);
    if ((viewPtr->rows.length == 0) || (viewPtr->columns.length == 0)) {
        /*return;*/
    }
    if (xOffset != viewPtr->columns.scrollOffset) {
        viewPtr->columns.scrollOffset = xOffset;
    }
    if (yOffset != viewPtr->rows.scrollOffset) {
        viewPtr->rows.scrollOffset = yOffset;
    }
    viewWidth = VPORTWIDTH(viewPtr);
    viewHeight = VPORTHEIGHT(viewPtr);

    viewPtr->rows.firstIndex = 0, viewPtr->rows.lastIndex = -1;

    /* Find the row that contains the start of the viewport.  */

    first = -1;
    low = 0; high = viewPtr->rows.numMapped - 1;
    while (low <= high) {
        long mid;
        Row *rowPtr;
        
        mid = (low + high) >> 1;
        rowPtr = viewPtr->rows.map[mid];
        if (yOffset > (rowPtr->worldY + rowPtr->height)) {
            low = mid + 1;
        } else if (yOffset < rowPtr->worldY) {
            high = mid - 1;
        } else {
            first = mid;
            break;
        }
    }
    if (first >= 0) {
        long last;

        /* Now look for the last row in the viewport. */

        last = viewPtr->rows.numMapped - 1;
        yOffset += viewHeight - 1;
        low = first; high = viewPtr->rows.numMapped - 1;
        while (low <= high) {
            Row *rowPtr;
            long mid;
            
            mid = (low + high) >> 1;
            rowPtr = viewPtr->rows.map[mid];
            if (yOffset > (rowPtr->worldY + rowPtr->height)) {
                low = mid + 1;
            } else if (yOffset < rowPtr->worldY) {
                high = mid - 1;
            } else {
                last = mid;
                break;
            }
        }
        viewPtr->rows.firstIndex = first;
        viewPtr->rows.lastIndex = last;
    }

    viewPtr->columns.firstIndex = 0, viewPtr->columns.lastIndex = -1;

    /* Find the column that contains the start of the viewport.  */

    first = -1;
    low = 0; high = viewPtr->columns.numMapped - 1;
    while (low <= high) {
        Column *colPtr;
        long mid;
        
        mid = (low + high) >> 1;
        colPtr = viewPtr->columns.map[mid];
        if (xOffset > (colPtr->worldX + colPtr->width + colPtr->ruleWidth)) {
            low = mid + 1;
        } else if (xOffset < colPtr->worldX) {
            high = mid - 1;
        } else {
            first = mid;
            break;
        }
    }
    if (first >= 0) {
        long last;

        /* Now look for the last column in the viewport. */

        last = viewPtr->columns.numMapped - 1;
        xOffset += viewWidth - 1;
        low = first; high = viewPtr->columns.numMapped - 1;
        while (low <= high) {
            Column *colPtr;
            long mid;
            
            mid = (low + high) >> 1;
            colPtr = viewPtr->columns.map[mid];
            if (xOffset > (colPtr->worldX+colPtr->width+colPtr->ruleWidth)) {
                low = mid + 1;
            } else if (xOffset < colPtr->worldX) {
                high = mid - 1;
            } else {
                last = mid;
                break;
            }
        }
        viewPtr->columns.firstIndex = first;
        viewPtr->columns.lastIndex = last;
    }
}

/*ARGSUSED*/
static ClientData
TableViewPickProc(
    ClientData clientData,
    int x, int y,                       /* Screen coordinates of the test
                                         * point. */
    ClientData *hintPtr)                /* (out) Context of item selected:
                                         * should be ITEM_CELL,
                                         * ITEM_ROW_TITLE, ITEM_ROW_RESIZE,
                                         * ITEM_COLUMN_TITLE, or
                                         * ITEM_COLUMN_RESIZE. */
{
    TableView *viewPtr = clientData;
    Row *rowPtr;
    Column *colPtr;
    int worldX, worldY;
    Cell *cellPtr;

    /* This simulates a cell grab. All events are assigned to the current
     * "grab" cell. */
    if (viewPtr->postPtr != NULL) {
        if (hintPtr != NULL) {
            *hintPtr = (ClientData)ITEM_CELL;
        }
        return viewPtr->postPtr;
    }
    if (viewPtr->filter.postPtr != NULL) {
        if (hintPtr != NULL) {
            *hintPtr = (ClientData)ITEM_COLUMN_FILTER;
        }
        return viewPtr->filter.postPtr;
    }
    if (hintPtr != NULL) {
        *hintPtr = NULL;
    }
    if (viewPtr->flags & GEOMETRY) {
        ComputeGeometry(viewPtr);
    }   
    if (viewPtr->flags & LAYOUT_PENDING) {
        /* Can't trust the selected items if rows/columns have been added
         * or deleted. So recompute the layout. */
        ComputeLayout(viewPtr);
    }
    if ((viewPtr->columns.flags | viewPtr->rows.flags) & SCROLL_PENDING) {
        ComputeVisibleEntries(viewPtr);
    }
    colPtr = NearestColumn(viewPtr, x, FALSE);
    rowPtr = NearestRow(viewPtr, y, FALSE);
    worldX = WORLDX(viewPtr, x);
    worldY = WORLDY(viewPtr, y);
    /* Determine if we're picking a column heading as opposed a cell.  */
    if ((colPtr != NULL) && ((colPtr->flags & (DISABLED|HIDDEN)) == 0) &&
        (viewPtr->columns.flags & SHOW_TITLES)) {

        if (y < (viewPtr->inset + viewPtr->columns.titleHeight)) {
            if (hintPtr != NULL) {
                ItemType type;
                
                if (worldX >= (colPtr->worldX + colPtr->width - RESIZE_AREA)) {
                    type = ITEM_COLUMN_RESIZE;
                } else {
                    type = ITEM_COLUMN_TITLE;
                }
                *hintPtr = (ClientData)type;
            }
            return colPtr;              /* We're picking the filter. */
        }
        if (y < (viewPtr->inset + viewPtr->columns.titleHeight + 
                 viewPtr->columns.filterHeight)) {
            if (hintPtr != NULL) {
                *hintPtr = (ClientData)ITEM_COLUMN_FILTER;
            }
            return colPtr;              /* We're picking the title/resize. */
        }
    }
    /* Determine if we're picking a row heading as opposed a cell.  */
    if ((rowPtr != NULL) && ((rowPtr->flags & (DISABLED|HIDDEN)) == 0) &&
        (viewPtr->rows.flags & SHOW_TITLES) && 
        (x < (viewPtr->inset + viewPtr->rows.titleWidth))) {
        if (hintPtr != NULL) {
            ItemType type;

            if (worldY >= (rowPtr->worldY + rowPtr->height - RESIZE_AREA)) {
                type = ITEM_ROW_RESIZE;
            } else {
                type = ITEM_ROW_TITLE;
            }
            *hintPtr = (ClientData)type;
        }
        return rowPtr;                  /* We're picking the title/resize. */
    }
    if ((colPtr == NULL) || (colPtr->flags & (DISABLED|HIDDEN))) {
        return NULL;                    /* Ignore disabled columns. */
    }
    if ((rowPtr == NULL) || (rowPtr->flags & (DISABLED|HIDDEN))) {
        return NULL;                    /* Ignore disabled rows. */
    }
    cellPtr = GetCell(viewPtr, rowPtr, colPtr);
    if (hintPtr != NULL) {
        *hintPtr = (ClientData)ITEM_CELL;
    }
    return cellPtr;
}

/*
 * TableView Procedures
 */
static void
ResetTableView(TableView *viewPtr)
{
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;

#ifdef notdef
    if (viewPtr->flags & REDRAW_PENDING) {
        Tcl_CancelIdleCall(DisplayProc, viewPtr);
    }
    if (viewPtr->columns.flags & REDRAW_PENDING) {
        Tcl_CancelIdleCall(DisplayColumnTitlesProc, viewPtr);
    }
    if (viewPtr->flags & SELECT_PENDING) {
        Tcl_CancelIdleCall(SelectCommandProc, viewPtr);
    }
#endif
    /* Free old row, columns, and cells. */
    for (hPtr = Blt_FirstHashEntry(&viewPtr->columns.table, &iter); 
         hPtr != NULL; hPtr = Blt_NextHashEntry(&iter)) {
        Column *colPtr;
        
        colPtr = Blt_GetHashValue(hPtr);
        colPtr->hashPtr = NULL;
        colPtr->flags |= DELETED;       /* Mark the column as deleted. This
                                         * prevents DestroyColumn from
                                         * pruning out cells that reside in
                                         * this column. We'll delete the
                                         * entire cell table. */
        DestroyColumn(colPtr);
    }
    for (hPtr = Blt_FirstHashEntry(&viewPtr->rows.table, &iter); hPtr != NULL;
         hPtr = Blt_NextHashEntry(&iter)) {
        Row *rowPtr;

        rowPtr = Blt_GetHashValue(hPtr);
        rowPtr->hashPtr = NULL;
        rowPtr->flags |= DELETED;       /* Mark the row as deleted. This
                                         * prevents DestroyRow from pruning
                                         * out cells that reside in this
                                         * row. We'll delete the entire
                                         * cell table. */
        DestroyRow(rowPtr);
    }
    for (hPtr = Blt_FirstHashEntry(&viewPtr->cellTable, &iter); hPtr != NULL;
         hPtr = Blt_NextHashEntry(&iter)) {
        Cell *cellPtr;

        cellPtr = Blt_GetHashValue(hPtr);
        cellPtr->flags |= CELL_DONT_DELETE;
        DestroyCell(viewPtr, cellPtr);
    }
    Blt_SetCurrentItem(viewPtr->bindTable, NULL, NULL);
    Blt_DeleteHashTable(&viewPtr->rows.table);
    Blt_DeleteHashTable(&viewPtr->columns.table);
    Blt_DeleteHashTable(&viewPtr->cellTable);
    Blt_InitHashTable(&viewPtr->cellTable, sizeof(CellKey)/sizeof(int));
    Blt_InitHashTable(&viewPtr->rows.table, BLT_ONE_WORD_KEYS);
    Blt_InitHashTable(&viewPtr->columns.table, BLT_ONE_WORD_KEYS);
    if (viewPtr->rows.map != NULL) {
        Blt_Free(viewPtr->rows.map);
        viewPtr->rows.map = NULL;
    }
    if (viewPtr->columns.map != NULL) {
        Blt_Free(viewPtr->columns.map);
        viewPtr->columns.map = NULL;
    }
    viewPtr->columns.firstIndex = viewPtr->columns.lastIndex = -1;
    viewPtr->rows.firstIndex = viewPtr->rows.lastIndex = -1;
    viewPtr->columns.firstPtr = viewPtr->columns.lastPtr = NULL;
    viewPtr->rows.firstPtr = viewPtr->rows.lastPtr = NULL;
    viewPtr->rows.length = viewPtr->columns.length = 0;
    viewPtr->rows.numAllocated = viewPtr->columns.numAllocated = 0;
    viewPtr->rows.numMapped = viewPtr->columns.numMapped = 0;
    viewPtr->focusPtr = viewPtr->activePtr = viewPtr->postPtr = NULL;
    viewPtr->rows.activeTitlePtr = viewPtr->rows.resizePtr = NULL;
    viewPtr->columns.activeTitlePtr = viewPtr->columns.resizePtr = NULL;
    ClearSelections(viewPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * TableViewFreeProc --
 *
 *      This procedure is invoked by Tcl_EventuallyFree or Tcl_Release to
 *      clean up the internal structure of a TableView at a safe time (when
 *      no-one is using it anymore).
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      Everything associated with the widget is freed up.
 *
 *---------------------------------------------------------------------------
 */
static void
TableViewFreeProc(DestroyData dataPtr) /* Pointer to the widget record. */
{
    TableView *viewPtr = (TableView *)dataPtr;

    ResetTableView(viewPtr);
    if (viewPtr->table != NULL) {
        blt_table_close(viewPtr->table);
        viewPtr->rows.notifier = NULL;
        viewPtr->columns.notifier = NULL;
        viewPtr->table = NULL;
    }
    if (viewPtr->sort.upArrow != NULL) {
        Blt_FreePicture(viewPtr->sort.upArrow);
    }
    if (viewPtr->sort.downArrow != NULL) {
        Blt_FreePicture(viewPtr->sort.downArrow);
    }
    if (viewPtr->painter != NULL) {
        Blt_FreePainter(viewPtr->painter);
    }
    iconOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    tableOption.clientData = viewPtr;
    Blt_FreeOptions(tableSpecs, (char *)viewPtr, viewPtr->display, 0);
    Blt_FreeOptions(filterSpecs, (char *)viewPtr, viewPtr->display, 0);
    if (viewPtr->tkwin != NULL) {
        Tk_DeleteSelHandler(viewPtr->tkwin, XA_PRIMARY, XA_STRING);
    }
    Blt_DestroyBindingTable(viewPtr->bindTable);
    /* Destroy remaing styles after freeing table option but before dumping
     * icons.  The styles may be using icons. */
    DestroyStyles(viewPtr);
    DestroyIcons(viewPtr);
    Blt_DeleteHashTable(&viewPtr->selectCells.cellTable);
    Blt_Chain_Destroy(viewPtr->rows.selection.list);
    Blt_DeleteHashTable(&viewPtr->cellTable);
    Blt_DeleteHashTable(&viewPtr->rows.table);
    Blt_DeleteHashTable(&viewPtr->columns.table);
    Blt_DeleteHashTable(&viewPtr->bindTagTable);
    Blt_DeleteHashTable(&viewPtr->uidTable);
    Blt_DeleteHashTable(&viewPtr->cachedObjTable);
    Blt_Pool_Destroy(viewPtr->rows.pool);
    Blt_Pool_Destroy(viewPtr->columns.pool);
    Blt_Pool_Destroy(viewPtr->cellPool);
    Blt_Free(viewPtr);
}

/*
 *---------------------------------------------------------------------------
 *
 * TableViewEventProc --
 *
 *      This procedure is invoked by the Tk dispatcher for various events
 *      on tableview widgets.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      When the window gets deleted, internal structures get cleaned up.
 *      When it gets exposed, it is redisplayed.
 *
 *---------------------------------------------------------------------------
 */
static void
TableViewEventProc(ClientData clientData, XEvent *eventPtr)
{
    TableView *viewPtr = clientData;

    if (eventPtr->type == Expose) {
        if (eventPtr->xexpose.count == 0) {
            viewPtr->columns.flags |= SCROLL_PENDING;
            viewPtr->rows.flags |= SCROLL_PENDING;
            EventuallyRedraw(viewPtr);
            Blt_PickCurrentItem(viewPtr->bindTable);
        }
    } else if (eventPtr->type == ConfigureNotify) {
        /* Size of the viewport has changed. Recompute visibilty. */
        viewPtr->flags |= LAYOUT_PENDING;
        viewPtr->columns.flags |= SCROLL_PENDING;
        viewPtr->rows.flags |= SCROLL_PENDING;
        EventuallyRedraw(viewPtr);
    } else if ((eventPtr->type == FocusIn) || (eventPtr->type == FocusOut)) {
        if (eventPtr->xfocus.detail != NotifyInferior) {
            if (eventPtr->type == FocusIn) {
                viewPtr->flags |= FOCUS;
            } else {
                viewPtr->flags &= ~FOCUS;
            }
            EventuallyRedraw(viewPtr);
        }
    } else if (eventPtr->type == DestroyNotify) {
        if (viewPtr->tkwin != NULL) {
            viewPtr->tkwin = NULL;
            Tcl_DeleteCommandFromToken(viewPtr->interp, viewPtr->cmdToken);
        }
        if (viewPtr->flags & REDRAW_PENDING) {
            Tcl_CancelIdleCall(DisplayProc, viewPtr);
        }
        if (viewPtr->columns.flags & REDRAW_PENDING) {
            Tcl_CancelIdleCall(DisplayColumnTitlesProc, viewPtr);
        }
        if (viewPtr->flags & SELECT_PENDING) {
            Tcl_CancelIdleCall(SelectCommandProc, viewPtr);
        }
        Tcl_EventuallyFree(viewPtr, TableViewFreeProc);
    }
}

typedef struct {
    int count;
    int length;
    Tcl_DString *dsPtr;
} CsvWriter;

/* Selection Procedures */
static void
CsvStartRecord(CsvWriter *writerPtr)
{
    writerPtr->count = 0;
}

static void
CsvEndRecord(CsvWriter *writerPtr)
{
    Tcl_DStringAppend(writerPtr->dsPtr, "\n", 1);
    writerPtr->length++;
}

static void
CsvAppendRecord(CsvWriter *writerPtr, const char *field, int length, 
                BLT_TABLE_COLUMN_TYPE type)
{
    const char *fp;
    char *p;
    int extra, doQuote;

    doQuote = (type == TABLE_COLUMN_TYPE_STRING);
    extra = 0;
    if (field == NULL) {
        length = 0;
    } else {
        for (fp = field; *fp != '\0'; fp++) {
            if ((*fp == '\n') || (*fp == ',') || (*fp == ' ') || 
                (*fp == '\t')) {
                doQuote = TRUE;
            } else if (*fp == '"') {
                doQuote = TRUE;
                extra++;
            }
        }
        if (doQuote) {
            extra += 2;
        }
        if (length < 0) {
            length = fp - field;
        }
    }
    if (writerPtr->count > 0) {
        Tcl_DStringAppend(writerPtr->dsPtr, ",", 1);
        writerPtr->length++;
    }
    length = length + extra + writerPtr->length;
    Tcl_DStringSetLength(writerPtr->dsPtr, length);
    p = Tcl_DStringValue(writerPtr->dsPtr) + writerPtr->length;
    writerPtr->length = length;
    if (field != NULL) {
        if (doQuote) {
            *p++ = '"';
        }
        for (fp = field; *fp != '\0'; fp++) {
            if (*fp == '"') {
                *p++ = '"';
            }
            *p++ = *fp;
        }
        if (doQuote) {
            *p++ = '"';
        }
    }
    writerPtr->count++;
}

static void
CsvAppendValue(CsvWriter *writerPtr, TableView *viewPtr, Row *rowPtr, 
            Column *colPtr)
{
    const char *string;
    int length;
    BLT_TABLE_COLUMN_TYPE type;
    
    string = blt_table_get_string(viewPtr->table, rowPtr->row, colPtr->column);
    length = (string == NULL) ? 0 : strlen(string);
    type = blt_table_column_type(colPtr->column);
    CsvAppendRecord(writerPtr, string, length, type);
}

static void
CsvAppendRow(CsvWriter *writerPtr, TableView *viewPtr, Row *rowPtr)
{
    Column *colPtr;

    CsvStartRecord(writerPtr);
    for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
         colPtr = colPtr->nextPtr) {
        if ((colPtr->flags & HIDDEN) == 0) {
            CsvAppendValue(writerPtr, viewPtr, rowPtr, colPtr);
        }
    }
    CsvEndRecord(writerPtr);
}


/*
 *---------------------------------------------------------------------------
 *
 * SelectionProc --
 *
 *      This procedure is called back by Tk when the selection is requested
 *      by someone.  It returns part or all of the selection in a buffer
 *      provided by the caller.
 *
 * Results:
 *      The return value is the number of non-NULL bytes stored at buffer.
 *      Buffer is filled (or partially filled) with a NUL-terminated string
 *      containing part or all of the selection, as given by offset and
 *      maxBytes.
 *
 * Side effects:
 *      None.
 *
 *---------------------------------------------------------------------------
 */
static int
SelectionProc(
    ClientData clientData,              /* Information about the widget. */
    int offset,                         /* Offset within selection of first
                                         * character to be returned. */
    char *buffer,                       /* Location in which to place
                                         * selection. */
    int maxBytes)                       /* Maximum number of bytes to place
                                         * at buffer, not including
                                         * terminating NULL character. */
{
    Tcl_DString ds;
    TableView *viewPtr = clientData;
    int size;
    CsvWriter writer;

    if ((viewPtr->flags & SELECT_EXPORT) == 0) {
        return -1;
    }
    /*
     * Retrieve the names of the selected entries.
     */
    Tcl_DStringInit(&ds);
    memset(&writer, 0, sizeof(CsvWriter));
    writer.dsPtr = &ds;
    writer.length = 0;
    writer.count = 0;
    switch (viewPtr->selectMode) {
    case SELECT_CELLS:
        {
            Row *rowPtr;
            CellKey anchor, mark;

            GetSelectedCells(viewPtr, &anchor, &mark);
            GetSelectedRows(viewPtr, &anchor, &mark);
            GetSelectedColumns(viewPtr, &anchor, &mark);
            for (rowPtr = anchor.rowPtr; 
                 (rowPtr != NULL) && (rowPtr->index <= mark.rowPtr->index); 
                 rowPtr = rowPtr->nextPtr) {
                Column *colPtr;

                if ((rowPtr->flags & HAS_SELECTION) == 0) {
                    continue;
                }
                CsvStartRecord(&writer);
                for (colPtr = anchor.colPtr; colPtr != NULL; 
                     colPtr = colPtr->nextPtr) {

                    if ((colPtr->flags & HAS_SELECTION) == 0) {
                        continue;
                    }
                    CsvAppendValue(&writer, viewPtr, rowPtr, colPtr);
                    if (colPtr == mark.colPtr) {
                        break;
                    }
                }
                CsvEndRecord(&writer);
                if (rowPtr == mark.rowPtr) {
                    break;
                }
            }
        }
        break;

    case SELECT_SINGLE_ROW:
    case SELECT_MULTIPLE_ROWS:
        if (viewPtr->flags & SELECT_SORTED) {
            Blt_ChainLink link;
            
            for (link = Blt_Chain_FirstLink(viewPtr->rows.selection.list); 
                 link != NULL; link = Blt_Chain_NextLink(link)) {
                Row *rowPtr;
                
                rowPtr = Blt_Chain_GetValue(link);
                CsvAppendRow(&writer, viewPtr, rowPtr);
            }
        } else {
            Row *rowPtr;
            
            for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
                 rowPtr = rowPtr->nextPtr) {
                if (rowPtr->flags & SELECTED) {
                    CsvAppendRow(&writer, viewPtr, rowPtr);
                }
            }
        }
    }
    size = Tcl_DStringLength(&ds) - offset;
    strncpy(buffer, Tcl_DStringValue(&ds) + offset, maxBytes);
    Tcl_DStringFree(&ds);
    buffer[maxBytes] = '\0';
    return (size > maxBytes) ? maxBytes : size;
}

static int
AttachTable(Tcl_Interp *interp, TableView *viewPtr)
{
    Row *rowPtr;
    unsigned int flags;

    if (viewPtr->flags & SELECT_PENDING) {
        Tcl_CancelIdleCall(SelectCommandProc, viewPtr);
    }

    ResetTableView(viewPtr);
    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    /* Try to match the current rows and columns in the view with the new
     * table names. This is so that we can keep various configuration
     * options that might have been set. */

    viewPtr->columns.notifier = blt_table_create_column_notifier(interp, 
        viewPtr->table, NULL, TABLE_NOTIFY_ALL_EVENTS, 
        TableEventProc, NULL, viewPtr);
    viewPtr->rows.notifier = blt_table_create_row_notifier(interp, 
        viewPtr->table, NULL, TABLE_NOTIFY_ALL_EVENTS, 
        TableEventProc, NULL, viewPtr);
    /* Rows. */
    if (viewPtr->rows.flags & AUTO_MANAGE) {
        BLT_TABLE_ROW row;
        Row **map;
        size_t i, numRows;

        numRows = blt_table_num_rows(viewPtr->table);
        if (numRows == 0) {
            if (viewPtr->rows.map != NULL) {
                Blt_Free(viewPtr->rows.map);
            }
            map = NULL;
        } else {
            map = Blt_AssertRealloc(viewPtr->rows.map, numRows * sizeof(Row *));
        }
        for (i = 0, row = blt_table_first_row(viewPtr->table); row != NULL;  
             row = blt_table_next_row(row), i++) {
            Blt_HashEntry *hPtr;
            Row *rowPtr;
            int isNew;
            
            hPtr = Blt_CreateHashEntry(&viewPtr->rows.table, (char *)row, 
                                       &isNew);
            assert(isNew);
            rowPtr = CreateRow(viewPtr, row, hPtr);
            map[i] = rowPtr;
        }
        viewPtr->rows.length = viewPtr->rows.numAllocated = numRows;
        viewPtr->rows.map = map;
        RenumberRows(viewPtr);
    }
    /* Columns. */
    if (viewPtr->columns.flags & AUTO_MANAGE) {
        BLT_TABLE_COLUMN col;
        Column **map;
        size_t i, numColumns;

        numColumns = blt_table_num_columns(viewPtr->table);
        if (numColumns == 0) {
            if (viewPtr->columns.map != NULL) {
                Blt_Free(viewPtr->columns.map);
            }
            map = NULL;
        } else {
            map = Blt_AssertRealloc(viewPtr->columns.map, 
                                numColumns *sizeof(Column *));
        }
        for (i = 0, col = blt_table_first_column(viewPtr->table); col != NULL;  
             col = blt_table_next_column(col), i++) {
            Blt_HashEntry *hPtr;
            Column *colPtr;
            int isNew;
            
            hPtr = Blt_CreateHashEntry(&viewPtr->columns.table, (char *)col,
                &isNew);
            assert(isNew);
            colPtr = CreateColumn(viewPtr, col, hPtr);
            Blt_SetHashValue(hPtr, colPtr);
            map[i] = colPtr;
        }
        viewPtr->columns.numAllocated = viewPtr->columns.length = numColumns;
        viewPtr->columns.map = map;
        RenumberColumns(viewPtr);
    }
    /* Create cells */
    for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL;
         rowPtr = rowPtr->nextPtr) {
        CellKey key;
        Column *colPtr;
        
        key.rowPtr = rowPtr;
        for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL;
             colPtr = colPtr->nextPtr) {
            Cell *cellPtr;
            Blt_HashEntry *hPtr;
            int isNew;
            
            key.colPtr = colPtr;
            hPtr = Blt_CreateHashEntry(&viewPtr->cellTable, (char *)&key, 
                &isNew);
            cellPtr = NewCell(viewPtr, hPtr);
            Blt_SetHashValue(hPtr, cellPtr);
        }
    }
    flags = TABLE_TRACE_FOREIGN_ONLY | TABLE_TRACE_WRITES | TABLE_TRACE_UNSETS;
    blt_table_trace_row(viewPtr->table, TABLE_TRACE_ALL_ROWS, flags, 
        RowTraceProc, NULL, viewPtr);
    blt_table_trace_column(viewPtr->table, TABLE_TRACE_ALL_COLUMNS, flags, 
        ColumnTraceProc, NULL, viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ConfigureTableView --
 *
 *      Updates the GCs and other information associated with the tableview
 *      widget.
 *
 * Results:
 *      The return value is a standard TCL result.  If TCL_ERROR is
 *      returned, then interp->result contains an error message.
 *
 * Side effects:
 *      Configuration information, such as text string, colors, font,
 *      etc. get set for viewPtr; old resources get freed, if there were
 *      any.  The widget is redisplayed.
 *
 *---------------------------------------------------------------------------
 */
static int
ConfigureTableView(Tcl_Interp *interp, TableView *viewPtr)      
{
    GC newGC;
    XGCValues gcValues;
    unsigned long gcMask;

    /* GC for normal row title. */
    gcMask = GCForeground | GCFont;
    gcValues.foreground = viewPtr->rows.normalTitleFg->pixel;
    gcValues.font = Blt_Font_Id(viewPtr->rows.titleFont);
    newGC = Tk_GetGC(viewPtr->tkwin, gcMask, &gcValues);
    if (viewPtr->rows.normalTitleGC != NULL) {
        Tk_FreeGC(viewPtr->display, viewPtr->rows.normalTitleGC);
    }
    viewPtr->rows.normalTitleGC = newGC;

    /* GC for active row title. */
    gcValues.foreground = viewPtr->rows.activeTitleFg->pixel;
    newGC = Tk_GetGC(viewPtr->tkwin, gcMask, &gcValues);
    if (viewPtr->rows.activeTitleGC != NULL) {
        Tk_FreeGC(viewPtr->display, viewPtr->rows.activeTitleGC);
    }
    viewPtr->rows.activeTitleGC = newGC;

    /* GC for disabled row title. */
    gcValues.foreground = viewPtr->rows.disabledTitleFg->pixel;
    newGC = Tk_GetGC(viewPtr->tkwin, gcMask, &gcValues);
    if (viewPtr->rows.disabledTitleGC != NULL) {
        Tk_FreeGC(viewPtr->display, viewPtr->rows.disabledTitleGC);
    }
    viewPtr->rows.disabledTitleGC = newGC;

    /* GC for normal column title. */
    gcMask = GCForeground | GCFont;
    gcValues.foreground = viewPtr->columns.normalTitleFg->pixel;
    gcValues.font = Blt_Font_Id(viewPtr->columns.titleFont);
    newGC = Tk_GetGC(viewPtr->tkwin, gcMask, &gcValues);
    if (viewPtr->columns.normalTitleGC != NULL) {
        Tk_FreeGC(viewPtr->display, viewPtr->columns.normalTitleGC);
    }
    viewPtr->columns.normalTitleGC = newGC;

    /* GC for active column title. */
    gcValues.foreground = viewPtr->columns.activeTitleFg->pixel;
    newGC = Tk_GetGC(viewPtr->tkwin, gcMask, &gcValues);
    if (viewPtr->columns.activeTitleGC != NULL) {
        Tk_FreeGC(viewPtr->display, viewPtr->columns.activeTitleGC);
    }
    viewPtr->columns.activeTitleGC = newGC;

    /* GC for disabled row title. */
    gcValues.foreground = viewPtr->columns.disabledTitleFg->pixel;
    newGC = Tk_GetGC(viewPtr->tkwin, gcMask, &gcValues);
    if (viewPtr->columns.disabledTitleGC != NULL) {
        Tk_FreeGC(viewPtr->display, viewPtr->columns.disabledTitleGC);
    }
    viewPtr->columns.disabledTitleGC = newGC;

    viewPtr->inset = viewPtr->highlightWidth + viewPtr->borderWidth + INSET_PAD;

    /*
     * If the table object was changed, we need to setup the new one.
     */
    if (Blt_ConfigModified(tableSpecs, "-table", (char *)NULL)) {
        if (AttachTable(interp, viewPtr) != TCL_OK) {
            return TCL_ERROR;
        }
    }

    /*
     * These options change the layout of the box.  Mark the widget for update.
     */
    if (Blt_ConfigModified(tableSpecs, "-width", "-height", "-hide", 
                           (char *)NULL)) {
        viewPtr->columns.flags |= SCROLL_PENDING;
        viewPtr->rows.flags |= SCROLL_PENDING;
    }
    if (Blt_ConfigModified(tableSpecs, "-font", "-linespacing", (char *)NULL)) {
        viewPtr->flags |= GEOMETRY;
    }
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ConfigureFilters --
 *
 *      Updates the GCs and other information associated with the tableview
 *      widget.
 *
 * Results:
 *      The return value is a standard TCL result.  If TCL_ERROR is
 *      returned, then interp->result contains an error message.
 *
 * Side effects:
 *      Configuration information, such as text string, colors, font,
 *      etc. get set for viewPtr; old resources get freed, if there were
 *      any.  The widget is redisplayed.
 *
 *---------------------------------------------------------------------------
 */
static int
ConfigureFilters(Tcl_Interp *interp, TableView *viewPtr)        
{
    GC newGC;
    XGCValues gcValues;
    unsigned long gcMask;
    FilterInfo *filterPtr;

    filterPtr = &viewPtr->filter;

    /* GC for normal filter. */
    gcMask = GCForeground | GCFont;
    gcValues.foreground = filterPtr->normalFg->pixel;
    gcValues.font = Blt_Font_Id(filterPtr->font);
    newGC = Tk_GetGC(viewPtr->tkwin, gcMask, &gcValues);
    if (filterPtr->normalGC != NULL) {
        Tk_FreeGC(viewPtr->display, filterPtr->normalGC);
    }
    filterPtr->normalGC = newGC;

    /* GC for active filter. */
    gcValues.foreground = filterPtr->activeFg->pixel;
    newGC = Tk_GetGC(viewPtr->tkwin, gcMask, &gcValues);
    if (filterPtr->activeGC != NULL) {
        Tk_FreeGC(viewPtr->display, filterPtr->activeGC);
    }
    filterPtr->activeGC = newGC;

    /* GC for disabled filter. */
    gcValues.foreground = filterPtr->disabledFg->pixel;
    newGC = Tk_GetGC(viewPtr->tkwin, gcMask, &gcValues);
    if (filterPtr->disabledGC != NULL) {
        Tk_FreeGC(viewPtr->display, filterPtr->disabledGC);
    }
    filterPtr->disabledGC = newGC;

    /* GC for selected (posted) filter. */
    gcValues.foreground = filterPtr->selectFg->pixel;
    newGC = Tk_GetGC(viewPtr->tkwin, gcMask, &gcValues);
    if (filterPtr->selectGC != NULL) {
        Tk_FreeGC(viewPtr->display, filterPtr->selectGC);
    }
    filterPtr->selectGC = newGC;

    /* GC for highlighted filter. */
    gcValues.foreground = filterPtr->highlightFg->pixel;
    newGC = Tk_GetGC(viewPtr->tkwin, gcMask, &gcValues);
    if (filterPtr->highlightGC != NULL) {
        Tk_FreeGC(viewPtr->display, filterPtr->highlightGC);
    }
    filterPtr->highlightGC = newGC;

    /*
     * These options change the layout of the box.  Mark the widget for update.
     */
    if (Blt_ConfigModified(filterSpecs, "-show", "-hide", (char *)NULL)) {
        viewPtr->columns.flags |= SCROLL_PENDING;
        viewPtr->rows.flags |= SCROLL_PENDING;
    }
    if (Blt_ConfigModified(tableSpecs, "-font", (char *)NULL)) {
        viewPtr->flags |= LAYOUT_PENDING;
    }
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

static Blt_Picture
GetFilterArrowPicture(FilterInfo *filterPtr, int w, int h, XColor *colorPtr)
{
    if ((filterPtr->downArrow == NULL) ||
        (Blt_Picture_Width(filterPtr->downArrow) != w) ||
        (Blt_Picture_Height(filterPtr->downArrow) != h)) {
        Blt_Picture picture;
        int ix, iy, iw, ih;
        
        if (filterPtr->downArrow != NULL) {
            Blt_FreePicture(filterPtr->downArrow);
        }
        iw = w * 75 / 100;
        ih = h * 40 / 100;
        
        picture = Blt_CreatePicture(w, h);
        Blt_BlankPicture(picture, 0x0);
        iy = (h - ih) / 2;
        ix = (w - iw) / 2;
        Blt_PaintArrowHead(picture, ix, iy, iw, ih,
                           Blt_XColorToPixel(colorPtr), ARROW_DOWN);
        filterPtr->downArrow = picture;
    }
    return filterPtr->downArrow;
}

/*
 *---------------------------------------------------------------------------
 *
 * DrawColumnFilter --
 *
 *      Draws the combo filter button given the screen coordinates and the
 *      value to be displayed.
 *
 * Results:
 *      None.
 *
 * Side Effects:
 *      The combo filter button value is drawn.
 *
 *      +---------------------------+   
 *      |b|x|icon|x|text|x|arrow|x|b|   
 *      +---------------------------+
 *
 * b = filter borderwidth
 * x = padx 
 *---------------------------------------------------------------------------
 */
static void
DrawColumnFilter(TableView *viewPtr, Column *colPtr, Drawable drawable, 
                 int x, int y)
{
    Blt_Bg bg, filterBg;
    GC gc;
    unsigned int colWidth, rowHeight, filterWidth, filterHeight;
    int relief;
    XColor *fg;
    FilterInfo *filterPtr;

    filterPtr = &viewPtr->filter;
    if (viewPtr->columns.titleHeight < 1) {
        return;
    }
    rowHeight = viewPtr->columns.filterHeight;
    colWidth  = colPtr->width;
    if ((rowHeight == 0) || (colWidth == 0)) {
        return;
    }
    relief = filterPtr->relief;
    if (colPtr->flags & DISABLED) {     /* Disabled  */
        fg = filterPtr->disabledFg;
        filterBg = bg = filterPtr->disabledBg;
        gc = filterPtr->disabledGC;
    } else if (colPtr == filterPtr->postPtr) {   /* Posted */
        bg = filterPtr->activeBg;
        fg = filterPtr->activeFg;
        filterBg = filterPtr->normalBg;
        gc = filterPtr->activeGC;
        relief = filterPtr->selectRelief;
    } else if (colPtr == filterPtr->activePtr) {  /* Active */
        bg = filterPtr->activeBg;
        fg = filterPtr->activeFg;
        filterBg = filterPtr->normalBg;
        gc = filterPtr->activeGC;
        relief = filterPtr->activeRelief;
    } else if (colPtr->flags & FILTERHIGHLIGHT) { /* Highlighted */
        fg = filterPtr->highlightFg;
        filterBg = bg = filterPtr->highlightBg;
        gc = filterPtr->highlightGC;
        relief = TK_RELIEF_FLAT;
    } else {                            /* Normal */
        fg = filterPtr->normalFg;
        filterBg = bg = filterPtr->normalBg;
        gc = filterPtr->normalGC;
        relief = TK_RELIEF_FLAT;
    }

    /* Draw background. */
    Blt_Bg_FillRectangle(viewPtr->tkwin, drawable, bg, x, y, colWidth,
        rowHeight, 0, TK_RELIEF_FLAT);
    x += filterPtr->outerBorderWidth;
    y += filterPtr->outerBorderWidth;
    colWidth -= 2 * filterPtr->outerBorderWidth;
    rowHeight -= 2 * filterPtr->outerBorderWidth;
    Blt_Bg_FillRectangle(viewPtr->tkwin, drawable, filterBg, x, y, colWidth,
        rowHeight, filterPtr->borderWidth, filterPtr->relief);
    rowHeight -= 2 * (filterPtr->borderWidth + TITLE_PADY);
    colWidth  -= 2 * (filterPtr->borderWidth + TITLE_PADX);
    x += filterPtr->borderWidth + TITLE_PADX;
    y += filterPtr->borderWidth + TITLE_PADY;
    filterHeight = rowHeight;
    filterWidth = colPtr->width - 
        2 * (filterPtr->borderWidth + TITLE_PADX);
    /* Justify (x) and center (y) the contents of the cell. */
    if (rowHeight > filterHeight) {
        y += (rowHeight - filterHeight) / 2;
    }
    if (colPtr->filterIcon != NULL) {
        int ix, iy, gap;

        ix = x;
        iy = y;
        if (filterHeight > IconHeight(colPtr->filterIcon)) {
            iy += (filterHeight - IconHeight(colPtr->filterIcon)) / 2;
        }
        Tk_RedrawImage(IconBits(colPtr->filterIcon), 0, 0, 
                IconWidth(colPtr->filterIcon), IconHeight(colPtr->filterIcon), 
                drawable, ix, iy);
        gap = 0;
        if (colPtr->filterText != NULL) {
            gap = TITLE_PADX;
        }
        x += IconWidth(colPtr->filterIcon) + gap;
        filterWidth -= IconWidth(colPtr->filterIcon) + gap;
    }
    if (colPtr->filterText != NULL) {
        int tx, ty;
        int maxLength;

        tx = x;
        ty = y;
        if (filterHeight > colPtr->filterTextHeight) {
            ty += (filterHeight - colPtr->filterTextHeight) / 2;
        }
        maxLength = filterWidth;
        if ((colPtr == filterPtr->activePtr)||(colPtr == filterPtr->postPtr)) {
            maxLength -= viewPtr->arrowWidth + TITLE_PADX;
        }
        if (maxLength > 0) {
            TextStyle ts;
            TextLayout *textPtr;
            Blt_Font font;

            font = CHOOSE(filterPtr->font, colPtr->filterFont);
            Blt_Ts_InitStyle(ts);
            Blt_Ts_SetFont(ts, font);
            Blt_Ts_SetGC(ts, gc);
            Blt_Ts_SetMaxLength(ts, maxLength);
            textPtr = Blt_Ts_CreateLayout(colPtr->filterText, -1, &ts);
            Blt_Ts_DrawLayout(viewPtr->tkwin, drawable, textPtr, &ts, tx, ty);
            Blt_Free(textPtr);
        }
    }
    if ((colPtr == filterPtr->activePtr) || (colPtr == filterPtr->postPtr)) {
        int ax, ay, aw, ah;

        aw = viewPtr->arrowWidth + (2 * filterPtr->borderWidth);
        ah = filterHeight;
        ax = x + filterWidth - aw;
        ay = y;
        Blt_Bg_FillRectangle(viewPtr->tkwin, drawable, bg, ax, ay,
                             aw, ah, filterPtr->borderWidth, relief);
        aw -= 2 * filterPtr->borderWidth;
        ax += filterPtr->borderWidth;
        if ((ax <= Tk_Width(viewPtr->tkwin)) &&
             (ay <= Tk_Height(viewPtr->tkwin)) && (aw > 0) && (ah > 0)) {
            Blt_Picture picture;

            picture = GetFilterArrowPicture(filterPtr, aw, ah, fg);
            if (viewPtr->painter == NULL) {
                viewPtr->painter = Blt_GetPainter(viewPtr->tkwin, 1.0);
            }
            Blt_PaintPicture(viewPtr->painter, drawable,
                             picture, 0, 0, aw, ah, ax, ay);
        }
    }
}

static void
DisplayCell(TableView *viewPtr, Cell *cellPtr, Drawable drawable, int buffer)
{
    CellKey *keyPtr;
    Row *rowPtr;
    Column *colPtr;
    int x, y;
    int x1, x2, y1, y2;
    int clipped;
    CellStyle *stylePtr;

    keyPtr = GetKey(viewPtr, cellPtr);
    rowPtr = keyPtr->rowPtr;
    colPtr = keyPtr->colPtr;
    stylePtr = GetCurrentStyle(viewPtr, rowPtr, colPtr, cellPtr);
    x1 = x = SCREENX(viewPtr, colPtr->worldX);
    y1 = y = SCREENY(viewPtr, rowPtr->worldY);
    x2 = x1 + colPtr->width;
    y2 = y1 + rowPtr->height;
    clipped = FALSE;
    if ((x1 >= (Tk_Width(viewPtr->tkwin) - viewPtr->inset)) ||
        (y1 >= (Tk_Height(viewPtr->tkwin) - viewPtr->inset)) ||
        (x2 <= (viewPtr->inset + viewPtr->rows.titleWidth)) ||
        (y2 <= (viewPtr->inset + + viewPtr->columns.filterHeight +
                viewPtr->columns.titleHeight))) {
        return;                         /* Cell isn't in viewport.  This
                                         * can happen when the active cell
                                         * is scrolled off screen. */
    }
    if (x1 < (viewPtr->inset + viewPtr->rows.titleWidth)) {
        x1 = viewPtr->inset + viewPtr->rows.titleWidth;
        clipped = TRUE;
    }
    if (x2 >= (Tk_Width(viewPtr->tkwin) - viewPtr->inset)) {
        x2 = Tk_Width(viewPtr->tkwin) - viewPtr->inset;
        clipped = TRUE;
    }
    if (y1 < (viewPtr->inset + viewPtr->columns.filterHeight + 
              viewPtr->columns.titleHeight)) {
        y1 = viewPtr->inset + viewPtr->columns.filterHeight + 
            viewPtr->columns.titleHeight;
        clipped = TRUE;
    }
    if (y2 >= (Tk_Height(viewPtr->tkwin) - viewPtr->inset)) {
        y2 = Tk_Height(viewPtr->tkwin) - viewPtr->inset;
        clipped = TRUE;
    }
    if ((buffer) || (clipped)) {
        long w, h, dx, dy;
        Pixmap pixmap;

        w = x2 - x1;
        h = y2 - y1;
        dx = x1 - x;
        dy = y1 - y;

        if ((h < 1) || (w < 1)) {
            return;
        }
        /* Draw into a pixmap and then copy it into the drawable.  */
        pixmap = Blt_GetPixmap(viewPtr->display, Tk_WindowId(viewPtr->tkwin), 
                w, h, Tk_Depth(viewPtr->tkwin));
        (*stylePtr->classPtr->drawProc)(viewPtr, cellPtr, pixmap, stylePtr, 
                                        -dx, -dy);
        XCopyArea(viewPtr->display, pixmap, drawable, 
                  viewPtr->rows.normalTitleGC,
                  0, 0, w, h, x + dx, y + dy);
        Tk_FreePixmap(viewPtr->display, pixmap);
    } else {
        (*stylePtr->classPtr->drawProc)(viewPtr, cellPtr, drawable, stylePtr, 
                                        x, y);
    }
}

static Blt_Picture
GetSortArrowPicture(TableView *viewPtr, int w, int h)
{
    if (viewPtr->sort.decreasing) {
        if (viewPtr->sort.upArrow == NULL) {
            Blt_Picture picture;
            int ix, iy, iw, ih;
            
            iw = w * 50 / 100;
            ih = h * 80 / 100;
            iy = (h - ih) / 2;
            ix = (w - iw) / 2;
            picture = Blt_CreatePicture(w, h);
            Blt_BlankPicture(picture, 0x0);
            Blt_PaintArrow(picture, ix, iy, iw, ih, 0xFFFF0000, ARROW_UP);
            viewPtr->sort.upArrow = picture;
        }
        return viewPtr->sort.upArrow;
    } else {
        if (viewPtr->sort.downArrow == NULL) {
            Blt_Picture picture;
            int ix, iy, iw, ih;

            iw = w * 50 / 100;
            ih = h * 80 / 100;
            iy = (h - ih) / 2;
            ix = (w - iw) / 2;
            picture = Blt_CreatePicture(w, h);
            Blt_BlankPicture(picture, 0x0);
            Blt_PaintArrow(picture, ix, iy, iw, ih, 0xFF0000FF, ARROW_DOWN);
            viewPtr->sort.downArrow = picture;
        }            
        return viewPtr->sort.downArrow;
    }
}

static void
DrawColumnTitle(TableView *viewPtr, Column *colPtr, Drawable drawable, int x, 
                int y)
{
    Blt_Bg bg;
    GC gc;
    SortInfo *sortPtr;
    int relief;
    int wanted, colWidth, colHeight;
    unsigned int aw, ah, iw, ih, tw, th;
    unsigned int igap, agap;
    const char *title;
    
    sortPtr = &viewPtr->sort;
    if (viewPtr->columns.titleHeight < 1) {
        return;
    }
    colWidth = colPtr->width;
    colHeight = viewPtr->columns.titleHeight;
    if ((colWidth == 0) || (colHeight == 0)) {
        return;
    }
    relief = colPtr->titleRelief;
    if (colPtr->flags & DISABLED) {
        /* Disabled  */
        bg = viewPtr->columns.disabledTitleBg;
        gc = viewPtr->columns.disabledTitleGC;
    } else if (colPtr == viewPtr->columns.activeTitlePtr) {  
        /* Active */
        bg = viewPtr->columns.activeTitleBg;
        gc = viewPtr->columns.activeTitleGC;
        relief = colPtr->activeTitleRelief;
    } else {                            
        /* Normal */
        bg = viewPtr->columns.normalTitleBg;
        gc = viewPtr->columns.normalTitleGC;
    }

    /* Clear the title area by drawing the background. */
    Blt_Bg_FillRectangle(viewPtr->tkwin, drawable, bg, x, y, colWidth, 
        viewPtr->columns.titleHeight,viewPtr->columns.titleBorderWidth, relief);

    colWidth -= 2 * (viewPtr->columns.titleBorderWidth + TITLE_PADX);
    colHeight -= 2 * (viewPtr->columns.titleBorderWidth + TITLE_PADY);

    x += viewPtr->columns.titleBorderWidth + TITLE_PADX;
    y += viewPtr->columns.titleBorderWidth + TITLE_PADY;

    tw = th = iw = ih = aw = ah = 0;
    agap = igap = 0;
    if (colPtr == sortPtr->firstPtr) {
        if ((sortPtr->up != NULL) && (sortPtr->down != NULL)) {
            aw = MAX(IconWidth(sortPtr->up), IconWidth(sortPtr->down));
            ah = MAX(IconHeight(sortPtr->up), IconHeight(sortPtr->down));
        } else {
            Blt_FontMetrics fm;

            Blt_Font_GetMetrics(viewPtr->columns.titleFont, &fm);
            ah = fm.linespace;
            aw = ah * 60 / 100;
        }
    }
    if (colPtr->icon != NULL) {
        iw = IconWidth(colPtr->icon);
        ih = IconHeight(colPtr->icon);
    }
    if ((iw > 0) && (colPtr->textWidth > 0)) {
        igap = TITLE_PADX;
    }
    if ((colPtr == viewPtr->sort.firstPtr) && (aw > 0)) {
        agap = TITLE_PADX;
    }
    tw = colPtr->textWidth;
    th = colPtr->textHeight;
    wanted = tw + aw + iw + agap + igap;
    if (colWidth > wanted) {
        switch (colPtr->titleJustify) {
        case TK_JUSTIFY_RIGHT:
            x += colWidth - wanted;
            break;
        case TK_JUSTIFY_CENTER:
            x += (colWidth - wanted) / 2;
            break;
        case TK_JUSTIFY_LEFT:
            break;
        }
    } else if (colWidth < wanted) {
        tw = colWidth - aw + iw + agap + igap;
    }
    if (colPtr->icon != NULL) {
        int iy;

        /* Center the icon vertically.  We already know the column title is
         * at least as tall as the icon. */
        iy = y;
        if (colHeight > ih) {
            iy += (colHeight - ih) / 2;
        }
        Tk_RedrawImage(IconBits(colPtr->icon), 0, 0, iw, ih, drawable, x, iy);
        x += iw + igap;
    }
    title = GetColumnTitle(colPtr);
    if (title != NULL) {
        TextStyle ts;
        int ty;

        ty = y;
        if (colHeight > colPtr->textHeight) {
            ty += (colHeight - colPtr->textHeight) / 2;
        }
        Blt_Ts_InitStyle(ts);
        Blt_Ts_SetFont(ts, viewPtr->columns.titleFont);
        Blt_Ts_SetGC(ts, gc);
        Blt_Ts_SetMaxLength(ts, tw);
        Blt_Ts_DrawText(viewPtr->tkwin, drawable, title, -1, &ts, x, ty);
        x += tw + agap;
    }
    if (colPtr == viewPtr->sort.firstPtr) {
        int ax, ay;
        ax = x;
        ay = y + (colHeight - ah) / 2;
        ay--;
        if ((viewPtr->sort.decreasing) && (viewPtr->sort.up != NULL)) {
            Tk_RedrawImage(IconBits(viewPtr->sort.up), 0, 0, aw, ah, drawable, 
                ax, ay);
        } else if (viewPtr->sort.down != NULL) {
            Tk_RedrawImage(IconBits(viewPtr->sort.down), 0, 0, aw, ah, drawable,
                ax, ay);
        } else if ((aw > 0) && (ah > 0)) {
            Blt_Picture picture;

            picture = GetSortArrowPicture(viewPtr, aw, ah);
            if (viewPtr->painter == NULL) {
                viewPtr->painter = Blt_GetPainter(viewPtr->tkwin, 1.0);
            }
            Blt_PaintPicture(viewPtr->painter, drawable, picture, 0, 0,
                             aw, ah, ax, ay);
        }
    }
}

static void
DrawRowTitle(TableView *viewPtr, Row *rowPtr, Drawable drawable, int x, int y)
{
    Blt_Bg bg;
    GC gc;
    int h, dy;
    int avail, need;
    int relief;
    const char *title;
    
    if (viewPtr->rows.titleWidth < 1) {
        return;
    }
    relief = rowPtr->titleRelief;
    if (rowPtr->flags & DISABLED) {     /* Disabled  */
        bg = viewPtr->rows.disabledTitleBg;
        gc = viewPtr->rows.disabledTitleGC;
    } else if (rowPtr == viewPtr->rows.activeTitlePtr) {  /* Active */
        bg = viewPtr->rows.activeTitleBg;
        gc = viewPtr->rows.activeTitleGC;
        relief = rowPtr->activeTitleRelief;
    } else {                            /* Normal */
        bg = viewPtr->rows.normalTitleBg;
        gc = viewPtr->rows.normalTitleGC;
    }
    dy = y;
    h = rowPtr->height;
    if (rowPtr->index == (viewPtr->rows.length - 1)) {
        /* If there's any room left over, let the last row take it. */
        h = Tk_Height(viewPtr->tkwin) - y;
    }
    if ((viewPtr->rows.titleWidth == 0) || (h == 0)) {
        return;
    }
    /* Clear the title area by drawing the background. */
    Blt_Bg_FillRectangle(viewPtr->tkwin, drawable, bg, x, dy, 
        viewPtr->rows.titleWidth, h, viewPtr->rows.titleBorderWidth, relief);

    avail = viewPtr->rows.titleWidth - 
        2 * (viewPtr->rows.titleBorderWidth + TITLE_PADX); 
    need  = rowPtr->titleWidth     - 
        2 * (viewPtr->rows.titleBorderWidth + TITLE_PADX);
    x += viewPtr->rows.titleBorderWidth + TITLE_PADX;
    y += viewPtr->rows.titleBorderWidth + TITLE_PADY;
    if (avail > need) {
        switch (rowPtr->titleJustify) {
        case TK_JUSTIFY_RIGHT:
            x += avail - need;
            break;
        case TK_JUSTIFY_CENTER:
            x += (avail - need) / 2;
            break;
        case TK_JUSTIFY_LEFT:
            break;
        }
    }
    if (rowPtr->icon != NULL) {
        int ix, iy, iw, ih;

        ih = IconHeight(rowPtr->icon);
        iw = IconWidth(rowPtr->icon);
        ix = x;
        /* Center the icon vertically.  We already know the column title is
         * at least as tall as the icon. */
        iy = y + (rowPtr->titleHeight - ih) / 2;
        Tk_RedrawImage(IconBits(rowPtr->icon), 0, 0, iw, ih, drawable, ix, iy);
        x += iw + 2;
        avail -= iw + 2;
    }
    title = GetRowTitle(rowPtr);
    if (title != NULL) {
        TextStyle ts;
        int ty;

        ty = y;
        if (rowPtr->height > rowPtr->titleHeight) {
            ty += (rowPtr->height - rowPtr->titleHeight) / 2;
        }
        Blt_Ts_InitStyle(ts);
        Blt_Ts_SetFont(ts, viewPtr->rows.titleFont);
        Blt_Ts_SetGC(ts, gc);
        Blt_Ts_SetMaxLength(ts, avail);
        Blt_Ts_DrawText(viewPtr->tkwin, drawable, title, -1, &ts, x, ty);
    }
}

static void
DisplayRowTitle(TableView *viewPtr, Row *rowPtr, Drawable drawable)
{
    int x, y, y1, y2;
    int isClipped;

    x = viewPtr->inset;
    y1 = y = SCREENY(viewPtr, rowPtr->worldY);
    y2 = y1 + rowPtr->height;
    if ((y1 >= (Tk_Height(viewPtr->tkwin) - viewPtr->inset)) ||
        (y2 <= (viewPtr->inset + viewPtr->columns.filterHeight +
                viewPtr->columns.titleHeight))) {
        return;                         /* Row starts after the window or
                                         * ends before the window. */
    }
    isClipped = FALSE;
    if (y1 < (viewPtr->inset + viewPtr->columns.filterHeight + 
              viewPtr->columns.titleHeight)) {
        y1 = viewPtr->inset + viewPtr->columns.filterHeight +
            viewPtr->columns.titleHeight;
        isClipped = TRUE;
    }
    if (y2 >= (Tk_Height(viewPtr->tkwin) - viewPtr->inset)) {
        y2 = Tk_Height(viewPtr->tkwin) - viewPtr->inset;
        isClipped = TRUE;
    }
    if (isClipped) {
        long h, dy;
        Pixmap pixmap;

        h = y2 - y1;
        dy = y1 - y;

        /* Draw into a pixmap and then copy it into the drawable.  */
        pixmap = Blt_GetPixmap(viewPtr->display, Tk_WindowId(viewPtr->tkwin), 
                viewPtr->rows.titleWidth, h, Tk_Depth(viewPtr->tkwin));
        DrawRowTitle(viewPtr, rowPtr, pixmap, 0, -dy);
        XCopyArea(viewPtr->display, pixmap, drawable, viewPtr->rows.normalTitleGC,
                  0, 0, viewPtr->rows.titleWidth, h, x, y + dy);
        Tk_FreePixmap(viewPtr->display, pixmap);
    } else {
        DrawRowTitle(viewPtr, rowPtr, drawable, x, y);
    }
}

static void
DisplayColumnTitle(TableView *viewPtr, Column *colPtr, Drawable drawable)
{
    int x, y, x1, x2;
    int clipped;

    y = viewPtr->inset;
    x1 = x = SCREENX(viewPtr, colPtr->worldX);
    x2 = x1 + colPtr->width;
    if ((x1 >= (Tk_Width(viewPtr->tkwin) - viewPtr->inset)) ||
        (x2 <= (viewPtr->inset + viewPtr->rows.titleWidth))) {
        return;                         /* Column starts after the end of
                                         * the viewport or ends before the
                                         * start of the viewport. */
    }
    clipped = FALSE;
    if (x1 < (viewPtr->inset + viewPtr->rows.titleWidth)) {
        x1 = viewPtr->inset + viewPtr->rows.titleWidth;
        clipped = TRUE;
    }
    if (x2 > (Tk_Width(viewPtr->tkwin) - viewPtr->inset)) {
        x2 = Tk_Width(viewPtr->tkwin) - viewPtr->inset;
        clipped = TRUE;
    }
    if (clipped) {
        long w, dx;
        Pixmap pixmap;

        w = x2 - x1;
        dx = x1 - x;
        /* Draw into a pixmap and then copy it into the drawable.  */
        pixmap = Blt_GetPixmap(viewPtr->display, Tk_WindowId(viewPtr->tkwin), 
                w, viewPtr->columns.titleHeight, Tk_Depth(viewPtr->tkwin));
        DrawColumnTitle(viewPtr, colPtr, pixmap, -dx, 0);
        XCopyArea(viewPtr->display, pixmap, drawable, 
                  viewPtr->columns.normalTitleGC,
                  0, 0, w, viewPtr->columns.titleHeight, x + dx, y);
        Tk_FreePixmap(viewPtr->display, pixmap);
    } else {
        DrawColumnTitle(viewPtr, colPtr, drawable, x, y);
    }
}

static void
DisplayColumnFilter(TableView *viewPtr, Column *colPtr, Drawable drawable)
{
    int x, y, x1, x2;
    int clipped;

    y = viewPtr->inset + viewPtr->columns.titleHeight;
    x1 = x = SCREENX(viewPtr, colPtr->worldX);
    x2 = x1 + colPtr->width;
    if ((x1 >= (Tk_Width(viewPtr->tkwin) - viewPtr->inset)) ||
        (x2 <= (viewPtr->inset + viewPtr->rows.titleWidth))) {
        return;                         /* Column starts after the end of
                                         * the viewport or ends before the
                                         * start of the viewport. */
    }
    clipped = FALSE;
    if (x1 < (viewPtr->inset + viewPtr->rows.titleWidth)) {
        x1 = viewPtr->inset + viewPtr->rows.titleWidth;
        clipped = TRUE;
    }
    if (x2 > (Tk_Width(viewPtr->tkwin) - viewPtr->inset)) {
        x2 = Tk_Width(viewPtr->tkwin) - viewPtr->inset;
        clipped = TRUE;
    }
    if (clipped) {
        long w, dx;
        Pixmap pixmap;

        w = x2 - x1;
        dx = x1 - x;

        /* Draw into a pixmap and then copy it into the drawable.  */
        pixmap = Blt_GetPixmap(viewPtr->display, Tk_WindowId(viewPtr->tkwin), 
                w, viewPtr->columns.filterHeight, Tk_Depth(viewPtr->tkwin));
        DrawColumnFilter(viewPtr, colPtr, pixmap, -dx, 0);
        XCopyArea(viewPtr->display, pixmap, drawable, viewPtr->columns.normalTitleGC,
                0, 0, w, viewPtr->columns.filterHeight, x + dx, y);
        Tk_FreePixmap(viewPtr->display, pixmap);
    } else {
        DrawColumnFilter(viewPtr, colPtr, drawable, x, y);
    }
}

static void
DisplayColumnTitles(TableView *viewPtr, Drawable drawable)
{
    long i;

    for (i = viewPtr->columns.firstIndex; i <= viewPtr->columns.lastIndex; i++) {
        Column *colPtr;

        colPtr = viewPtr->columns.map[i];
        assert((colPtr->flags & HIDDEN) == 0);
        DisplayColumnTitle(viewPtr, colPtr, drawable);
        if (viewPtr->flags & COLUMN_FILTERS) {
            DisplayColumnFilter(viewPtr, colPtr, drawable);
        }
    }
}

static void
DisplayColumnTitlesProc(ClientData clientData)
{
    long i;
    int x, y, w, h;
    Drawable drawable;
    TableView *viewPtr = clientData;
    
    viewPtr->columns.flags &= ~REDRAW_PENDING;
    w = Tk_Width(viewPtr->tkwin) - 2 * viewPtr->inset;
    h = viewPtr->columns.titleHeight;
        
    /* Create an area the size of just the title area and fill it with the
     * widget background.  */
    drawable = Blt_GetPixmap(viewPtr->display, Tk_WindowId(viewPtr->tkwin), 
        w, h, Tk_Depth(viewPtr->tkwin));
    Blt_Bg_FillRectangle(viewPtr->tkwin, drawable, viewPtr->bg, 0, 0, w, h,
        0, TK_RELIEF_FLAT);

    if (viewPtr->rows.flags & viewPtr->columns.flags & SHOW_TITLES) {
        /* When showing both row and column titles, the area above the row
         * titles needs to be filled: both for the height of the column
         * title and column filter (if there is one). */
        if ((viewPtr->rows.titleWidth > 0) && 
            (viewPtr->columns.titleHeight > 0)) {
            Blt_Bg_FillRectangle(viewPtr->tkwin, drawable, 
                viewPtr->columns.normalTitleBg, 0, 0, 
                viewPtr->rows.titleWidth, viewPtr->columns.titleHeight, 
                viewPtr->columns.titleBorderWidth, TK_RELIEF_RAISED);
        }
    }
    y = 0;
    /* Draw all the column titles except for the currently sliding column. */
    for (i = viewPtr->columns.firstIndex; i <= viewPtr->columns.lastIndex; 
         i++) {
        Column *colPtr;

        colPtr = viewPtr->columns.map[i];
        if (colPtr == viewPtr->columns.slidePtr) {
            continue;
        }
        assert((colPtr->flags & HIDDEN) == 0);
        x = SCREENX(viewPtr, colPtr->worldX) - viewPtr->inset;
        DrawColumnTitle(viewPtr, colPtr, drawable, x, y);
    }
    /* Draw all the currently sliding column last, because we want it to
     * appear above of existing columns. */
    if (viewPtr->columns.slidePtr != NULL) {
        x = SCREENX(viewPtr, viewPtr->columns.slidePtr->worldX) + 
            viewPtr->columns.slideOffset;
        DrawColumnTitle(viewPtr, viewPtr->columns.slidePtr, drawable, x, y);
    }
    XCopyArea(viewPtr->display, drawable, Tk_WindowId(viewPtr->tkwin), 
              viewPtr->columns.normalTitleGC, 0, 0, w, h,
              viewPtr->inset, viewPtr->inset);
    Tk_FreePixmap(viewPtr->display, drawable);
}

static void
DisplayRowTitles(TableView *viewPtr, Drawable drawable)
{
    long i;

    for (i = viewPtr->rows.firstIndex; i <= viewPtr->rows.lastIndex; i++) {
        Row *rowPtr;

        rowPtr = viewPtr->rows.map[i];
        assert((rowPtr->flags & HIDDEN) == 0);
        DisplayRowTitle(viewPtr, rowPtr, drawable);
    }
}


static void
DrawOuterBorders(TableView *viewPtr, Drawable drawable)
{
    /* Draw 3D border just inside of the focus highlight ring. */
    if (viewPtr->borderWidth > 0) {
        int w, h;

        w = Tk_Width(viewPtr->tkwin)  - 2 * viewPtr->highlightWidth;
        h = Tk_Height(viewPtr->tkwin) - 2 * viewPtr->highlightWidth;
        if ((w > 0) && (h > 0)) {
            Blt_Bg_DrawRectangle(viewPtr->tkwin, drawable, 
                viewPtr->rows.normalTitleBg, viewPtr->highlightWidth, 
                viewPtr->highlightWidth, w, h, 
                viewPtr->borderWidth, viewPtr->relief);
        }
    }
    /* Draw focus highlight ring. */
    if (viewPtr->highlightWidth > 0) {
        XColor *color;
        GC gc;

        color = (viewPtr->flags & FOCUS)
            ? viewPtr->highlightColor : viewPtr->highlightBgColor;
        gc = Tk_GCForColor(color, drawable);
        Tk_DrawFocusHighlight(viewPtr->tkwin, gc, viewPtr->highlightWidth,
            drawable);
    }
}

#ifdef notdef
static void
AdjustRows(TableView *viewPtr)
{
    Row *lastPtr;
    double weight;
    int growth;
    long numOpen;
    long i, y;
    Row *rowPtr;

    growth = VPORTHEIGHT(viewPtr) - viewPtr->worldHeight;
    assert(growth > 0);
    lastPtr = NULL;
    numOpen = 0;
    weight = 0.0;
    /* Find out how many columns still have space available */
    for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
         rowPtr = rowPtr-nextPtr) {
        if (rowPtr->flags & HIDDEN) {
            continue;
        }
        lastPtr = rowPtr;
        if ((rowPtr->weight == 0.0) || (rowPtr->height >= rowPtr->max) || 
            (rowPtr->reqHeight > 0)) {
            continue;
        }
        numOpen++;
        weight += rowPtr->weight;
    }

    while ((numOpen > 0) && (weight > 0.0) && (growth > 0)) {
        int ration;
        long i;
        Row *rowPtr;

        ration = (int)(growth / weight);
        if (ration == 0) {
            ration = 1;
        }
        for (rowPtr = rowPtr->rows.firstPtr; rowPtr != NULL;
             rowPtr = rowPtr->nextPtr) { 
            int size, avail;

            if (rowPtr->flags & HIDDEN) {
                continue;
            }
            lastPtr = rowPtr;
            if ((rowPtr->weight == 0.0) || (rowPtr->height >= rowPtr->max) || 
                (rowPtr->reqHeight > 0)) {
                continue;
            }
            size = (int)(ration * rowPtr->weight);
            if (size > growth) {
                size = growth; 
            }
            avail = rowPtr->max - rowPtr->height;
            if (size > avail) {
                size = avail;
                numOpen--;
                weight -= rowPtr->height;
            }
            rowPtr->height += size;
            growth -= size;
            if ((numOpen <= 0) || (growth <= 0) || (weight <= 0.0)) {
                break;
            }
        }
    }
    if ((growth > 0) && (lastPtr != NULL)) {
        lastPtr->height += growth;
    }
    y = 0;
    for (rowPtr = rowPtr->rows.firstPtr; rowPtr != NULL; 
         rowPtr = rowPtr->nextPtr) {
        if (rowPtr->flags & HIDDEN) {
            continue;                   /* Ignore hidden columns. */
        }
        rowPtr->worldY = y;
        y += rowPtr->height;
    }
}
#endif



/*
 *---------------------------------------------------------------------------
 *
 * TableView operations
 *
 *---------------------------------------------------------------------------
 */

/*
 *---------------------------------------------------------------------------
 *
 * ActivateOp --
 *
 *      Makes the cell appear active.
 *
 *      pathName activate cellName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ActivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Cell *cellPtr, *activePtr;

    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    activePtr = viewPtr->activePtr;
    viewPtr->activePtr = cellPtr;
    /* If we aren't already queued to redraw the widget, try to directly
     * draw into window. */
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        Drawable drawable;

        drawable = Tk_WindowId(viewPtr->tkwin);
        if (activePtr != NULL) {
            DisplayCell(viewPtr, activePtr, drawable, TRUE);
        }
        DisplayCell(viewPtr, cellPtr, drawable, TRUE);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * BboxOp --
 *
 *      pathName bbox cellName ?switches ...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
BboxOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    Column *colPtr;
    Row *rowPtr;
    TableView *viewPtr = clientData;
    Tcl_Obj *listObjPtr;
    int w, h;
    int x1, y1, x2, y2;
    BBoxSwitches switches;
    
    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    if (viewPtr->flags & (LAYOUT_PENDING|GEOMETRY)) {
        /*
         * The layout is dirty.  Recompute it now, before we use the world
         * dimensions.  But remember that the "bbox" operation isn't valid
         * for hidden entries (since they're not visible, they don't have
         * world coordinates).
         */
        ComputeGeometry(viewPtr);
    }

    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr)  != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, bboxSwitches, objc - 3, objv + 3, 
        &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    rowPtr = keyPtr->rowPtr;
    colPtr = keyPtr->colPtr;
    x1 = colPtr->worldX;
    x2 = colPtr->worldX + colPtr->width;
    y1 = rowPtr->worldY;
    y2 = rowPtr->worldY + rowPtr->height;

    w = VPORTWIDTH(viewPtr);
    h = VPORTHEIGHT(viewPtr);
    /*
     * Do a min-max text for the intersection of the viewport and the
     * computed bounding box.  If there is no intersection, return the
     * empty string.
     */
    if ((x2 < viewPtr->columns.scrollOffset) || 
        (y2 < viewPtr->rows.scrollOffset) ||
        (x1 >= (viewPtr->columns.scrollOffset + w)) || 
        (y1 >= (viewPtr->rows.scrollOffset + h))) {
        return TCL_OK;
    }
    x1 = SCREENX(viewPtr, x1);
    y1 = SCREENY(viewPtr, y1);
    x2 = SCREENX(viewPtr, x2);
    y2 = SCREENY(viewPtr, y2);
    if (switches.flags & BBOX_ROOT) {
        int rootX, rootY;
        
        Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
        x1 += rootX, y1 += rootY;
        x2 += rootX, y2 += rootY;
    }
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(x1));
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(y1));
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(x2));
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(y2));
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * BindOp --
 *
 *      pathName bind tag sequence command
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
BindOp(TableView *viewPtr, Tcl_Interp *interp, int objc, Tcl_Obj *const *objv)
{
    BindTag tag;
    Cell *cellPtr;

    /*
     * Cells are selected by id only.  All other strings are interpreted as
     * a binding tag.
     */
    if ((GetCellFromObj(NULL, viewPtr, objv[2], &cellPtr) == TCL_OK) &&
        (cellPtr != NULL)) {
        tag = MakeBindTag(viewPtr, cellPtr, ITEM_CELL);
    } else {
        /* Assume that this is a binding tag. */
        tag = MakeStringBindTag(viewPtr, Tcl_GetString(objv[2]), ITEM_CELL);
    } 
    return Blt_ConfigureBindingsFromObj(interp, viewPtr->bindTable, tag, 
         objc - 3, objv + 3);
}

/*
 *---------------------------------------------------------------------------
 *
 * CellActivateOp --
 *
 *      Turns on highlighting for a particular cell.  Only one cell can be
 *      active at a time.
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 *      pathName cell activate ?cellName?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CellActivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Cell *cellPtr, *activePtr;

    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    activePtr = viewPtr->activePtr;
    viewPtr->activePtr = cellPtr;
    /* If we aren't already queued to redraw the widget, try to directly
     * draw into window. */
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        Drawable drawable;

        drawable = Tk_WindowId(viewPtr->tkwin);
        if (activePtr != NULL) {
            DisplayCell(viewPtr, activePtr, drawable, TRUE);
        }
        DisplayCell(viewPtr, cellPtr, drawable, TRUE);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CellBboxOp --
 *
 *      pathName cell bbox cellName ?switches...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CellBboxOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    Column *colPtr;
    Row *rowPtr;
    TableView *viewPtr = clientData;
    Tcl_Obj *listObjPtr;
    int w, h;
    int x1, y1, x2, y2;
    BBoxSwitches switches;
    
    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    if (viewPtr->flags & (LAYOUT_PENDING|GEOMETRY)) {
        /*
         * The layout is dirty.  Recompute it now, before we use the world
         * dimensions.  But remember that the "bbox" operation isn't valid
         * for hidden entries (since they're not visible, they don't have
         * world coordinates).
         */
        ComputeGeometry(viewPtr);
    }

    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr)  != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, bboxSwitches, objc - 4, objv + 4, 
        &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    rowPtr = keyPtr->rowPtr;
    colPtr = keyPtr->colPtr;
    x1 = colPtr->worldX;
    x2 = colPtr->worldX + colPtr->width;
    y1 = rowPtr->worldY;
    y2 = rowPtr->worldY + rowPtr->height;

    w = VPORTWIDTH(viewPtr);
    h = VPORTHEIGHT(viewPtr);
    /*
     * Do a min-max text for the intersection of the viewport and the
     * computed bounding box.  If there is no intersection, return the
     * empty string.
     */
    if ((x2 < viewPtr->columns.scrollOffset) || 
        (y2 < viewPtr->rows.scrollOffset) ||
        (x1 >= (viewPtr->columns.scrollOffset + w)) || 
        (y1 >= (viewPtr->rows.scrollOffset + h))) {
        return TCL_OK;
    }
    x1 = SCREENX(viewPtr, x1);
    y1 = SCREENY(viewPtr, y1);
    x2 = SCREENX(viewPtr, x2);
    y2 = SCREENY(viewPtr, y2);
    if (switches.flags & BBOX_ROOT) {
        int rootX, rootY;
        
        Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
        x1 += rootX, y1 += rootY;
        x2 += rootX, y2 += rootY;
    }
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(x1));
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(y1));
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(x2));
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(y2));
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CellCgetOp --
 *
 *      pathName cell cget cellName option 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CellCgetOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Cell *cellPtr;

    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    cellStateOption.clientData = viewPtr;
    return Blt_ConfigureValueFromObj(interp, viewPtr->tkwin, cellSpecs,
        (char *)cellPtr, objv[4], 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * CellConfigureOp --
 *
 *      This procedure is called to process an objv/objc list, plus the Tk
 *      option database, in order to configure (or reconfigure) the widget.
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 * Side effects:
 *      Configuration information, such as text string, colors, font,
 *      etc. get set for viewPtr; old resources get freed, if there were
 *      any.  The widget is redisplayed.
 *
 *      pathName cell configure cellName ?option value ...?
 *
 *---------------------------------------------------------------------------
 */
static int
CellConfigureOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Cell *cellPtr;
    CellStyle *oldStylePtr;

    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    cellStateOption.clientData = viewPtr;
    if (objc == 4) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, 
                cellSpecs, (char *)cellPtr, (Tcl_Obj *)NULL, 0);
    } else if (objc == 5) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, 
                cellSpecs, (char *)cellPtr, objv[4], 0);
    } 
    iconOption.clientData = viewPtr;
    tableOption.clientData = viewPtr;
    oldStylePtr = cellPtr->stylePtr;
    if (Blt_ConfigureWidgetFromObj(interp, viewPtr->tkwin, cellSpecs, 
        objc - 4, objv + 4, (char *)cellPtr, BLT_CONFIG_OBJV_ONLY) != TCL_OK) {
        return TCL_ERROR;
    }
    if ((Blt_ConfigModified(cellSpecs, "-style", (char *)NULL)) &&
        (oldStylePtr != cellPtr->stylePtr)) {
        CellKey *keyPtr;

        keyPtr = GetKey(viewPtr, cellPtr);
        if (cellPtr->stylePtr != NULL) {
            int isNew;
            Blt_HashEntry *hPtr;

            cellPtr->stylePtr->refCount++;      
            hPtr = Blt_CreateHashEntry(&cellPtr->stylePtr->table, 
                                       (char *)keyPtr, &isNew);
            assert(isNew);
            Blt_SetHashValue(hPtr, cellPtr);
        }
        if (oldStylePtr != NULL) {
            Blt_HashEntry *hPtr;

            /* Remove the cell from old style's table of cells. */
            oldStylePtr->refCount--;
            hPtr = Blt_FindHashEntry(&oldStylePtr->table, (char *)keyPtr);
            if (hPtr != NULL) {
                Blt_DeleteHashEntry(&oldStylePtr->table, hPtr);
            }
            if (oldStylePtr->refCount <= 0) {
                (*oldStylePtr->classPtr->freeProc)(oldStylePtr);
            }
        }
        cellPtr->flags |= GEOMETRY;     /* Assume that the new style
                                         * changes the geometry of the
                                         * cell. */
    }
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CellDeactivateOp --
 *
 *      Deactivates all cells.
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 *        pathName deactivate 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CellDeactivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                 Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Cell *activePtr;

    activePtr = viewPtr->activePtr;
    viewPtr->activePtr = NULL;
    /* If we aren't already queued to redraw the widget, try to directly
     * draw into window. */
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        if (activePtr != NULL) {
            Drawable drawable;

            drawable = Tk_WindowId(viewPtr->tkwin);
            DisplayCell(viewPtr, activePtr, drawable, TRUE);
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CellFocusOp --
 *
 *      Gets or sets focus on a cell.
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 *        pathName cell focus ?cellName?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CellFocusOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Cell *cellPtr;

    if (objc == 3) {
        Tcl_Obj *listObjPtr;

        listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
        if (viewPtr->focusPtr != NULL) {
            CellKey *keyPtr;
            Column *colPtr;
            Row *rowPtr;
            Tcl_Obj *objPtr;

            keyPtr = GetKey(viewPtr, viewPtr->focusPtr);
            rowPtr = keyPtr->rowPtr;
            colPtr = keyPtr->colPtr;
            objPtr = GetRowIndexObj(viewPtr, rowPtr);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
            objPtr = GetColumnIndexObj(viewPtr, colPtr);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
        }
        Tcl_SetObjResult(interp, listObjPtr);
        return TCL_OK;
    }
    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr != NULL) {
        CellKey *keyPtr;
        Row *rowPtr;
        Column *colPtr;

        keyPtr = GetKey(viewPtr, cellPtr);
        rowPtr = keyPtr->rowPtr;
        colPtr = keyPtr->colPtr;
        if ((rowPtr->flags|colPtr->flags) & (HIDDEN|DISABLED)) {
            return TCL_OK;              /* Can't set focus to hidden or
                                         * disabled cell */
        }
        if (cellPtr != viewPtr->focusPtr) {
            viewPtr->focusPtr = cellPtr;
            EventuallyRedraw(viewPtr);
        }
        Blt_SetFocusItem(viewPtr->bindTable, viewPtr->focusPtr, ITEM_CELL);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CellIdentifyOp --
 *
 *      pathName cell identify cellName x y 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CellIdentifyOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    CellStyle *stylePtr;
    Column *colPtr;
    Row *rowPtr;
    TableView *viewPtr = clientData;
    const char *string;
    int x, y, rootX, rootY;

    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    if ((Tcl_GetIntFromObj(interp, objv[4], &x) != TCL_OK) ||
        (Tcl_GetIntFromObj(interp, objv[5], &y) != TCL_OK)) {
        return TCL_ERROR;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    colPtr = keyPtr->colPtr;
    rowPtr = keyPtr->rowPtr;
    /* Convert from root coordinates to window-local coordinates to cell-local
     * coordinates */
    Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
    x -= rootX + SCREENX(viewPtr, colPtr->worldX);
    y -= rootY + SCREENY(viewPtr, rowPtr->worldY);
    string = NULL;
    stylePtr = GetCurrentStyle(viewPtr, rowPtr, colPtr, cellPtr);
    if (stylePtr->classPtr->identProc != NULL) {
        string = (*stylePtr->classPtr->identProc)(viewPtr, cellPtr, stylePtr, 
                                                  x, y);
    }
    if (string != NULL) {
        Tcl_SetStringObj(Tcl_GetObjResult(interp), string, -1);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CellIndexOp --
 *
 *      Converts the string representing a cell index into their respective
 *      "node field" identifiers.
 *
 * Results: 
 *      A standard TCL result.  Interp->result will contain the identifier
 *      of each inode found. If an inode could not be found, then the
 *      serial identifier will be the empty string.
 *
 *      pathName cell index cellName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CellIndexOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    Column *colPtr;
    Row *rowPtr;
    TableView *viewPtr = clientData;
    Tcl_Obj *listObjPtr, *objPtr;

    if ((GetCellFromObj(NULL, viewPtr, objv[3], &cellPtr) != TCL_OK) ||
        (cellPtr == NULL)) {
        return TCL_OK;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    colPtr = keyPtr->colPtr;
    rowPtr = keyPtr->rowPtr;
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    objPtr = GetRowIndexObj(viewPtr, rowPtr);
    Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    objPtr = GetColumnIndexObj(viewPtr, colPtr);
    Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CellInvokeOp --
 *
 *      pathName cell invoke cellName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CellInvokeOp(ClientData clientData, Tcl_Interp *interp, int objc, 
             Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Row *rowPtr;
    Column *colPtr;
    Cell *cellPtr;
    CellStyle *stylePtr;
    CellKey *keyPtr;

    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    colPtr = keyPtr->colPtr;
    rowPtr = keyPtr->rowPtr;
    stylePtr = GetCurrentStyle(viewPtr, rowPtr, colPtr, cellPtr);
    if (stylePtr->cmdObjPtr != NULL) {
        int result;
        Tcl_Obj *cmdObjPtr, *objPtr;

        cmdObjPtr = Tcl_DuplicateObj(stylePtr->cmdObjPtr);
        objPtr = GetRowIndexObj(viewPtr, rowPtr);
        Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
        objPtr = GetColumnIndexObj(viewPtr, colPtr);
        Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
        Tcl_IncrRefCount(cmdObjPtr);
        Tcl_Preserve(cellPtr);
        result = Tcl_EvalObjEx(interp, cmdObjPtr, TCL_EVAL_GLOBAL);
        Tcl_Release(cellPtr);
        Tcl_DecrRefCount(cmdObjPtr);
        if (result != TCL_OK) {
            return TCL_ERROR;
        }
    } 
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CellSeeOp --
 *
 *      pathName cell see cellName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CellSeeOp(ClientData clientData, Tcl_Interp *interp, int objc, 
          Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    TableView *viewPtr = clientData;
    long xOffset, yOffset;

    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    yOffset = GetRowYOffset(viewPtr, keyPtr->rowPtr);
    xOffset = GetColumnXOffset(viewPtr, keyPtr->colPtr);
    if (xOffset != viewPtr->columns.scrollOffset) {
        viewPtr->columns.scrollOffset = xOffset;
        viewPtr->columns.flags |= SCROLL_PENDING;
    }
    if (yOffset != viewPtr->rows.scrollOffset) {
        viewPtr->rows.scrollOffset = yOffset;
        viewPtr->rows.flags |= SCROLL_PENDING;
    }
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CellStyleOp --
 *
 *      pathName cell style cellName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CellStyleOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    CellStyle *stylePtr;
    Cell *cellPtr;
    CellKey *keyPtr;

    cellPtr = NULL;                     /* Suppress compiler warning. */
    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    stylePtr = GetCurrentStyle(viewPtr, keyPtr->rowPtr, keyPtr->colPtr, 
                cellPtr);
    Tcl_SetStringObj(Tcl_GetObjResult(interp), stylePtr->name, -1);
    return TCL_OK;
}


/*
 *---------------------------------------------------------------------------
 *
 * CellWritableOp --
 *
 *        pathName cell writable cellName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CellWritableOp(ClientData clientData, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    TableView *viewPtr = clientData;
    int state;

    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }

    state = FALSE;
    if (cellPtr != NULL) {
        CellKey *keyPtr;
        CellStyle *stylePtr;
        Column *colPtr;
        Row *rowPtr;

        keyPtr = GetKey(viewPtr, cellPtr);
        colPtr = keyPtr->colPtr;
        rowPtr = keyPtr->rowPtr;
        stylePtr = GetCurrentStyle(viewPtr, rowPtr, colPtr, cellPtr);
        
        state = (stylePtr->flags & EDIT);
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CellOp --
 *
 *      This procedure handles cell operations.
 *
 * Results:
 *      A standard TCL result.
 *
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec cellOps[] =
{
    {"activate",   1, CellActivateOp,    3, 4, "?cellName?",},
    {"bbox",       1, CellBboxOp,        4, 0, "cellName ?switches ...?",},
    {"cget",       2, CellCgetOp,        5, 5, "cellName option",},
    {"configure",  2, CellConfigureOp,   4, 0, "cellName ?option value ...?",},
    {"deactivate", 1, CellDeactivateOp,  3, 3, "",},
    {"focus",      2, CellFocusOp,       3, 4, "?cellName?",},
    {"identify",   2, CellIdentifyOp,    6, 6, "cellName x y",},
    {"index",      3, CellIndexOp,       4, 4, "cellName",},
    {"invoke",     3, CellInvokeOp,      4, 4, "cellName",},
    {"see",        3, CellSeeOp,         4, 4, "cellName",},
    {"style",      3, CellStyleOp,       4, 4, "cellName",},
    {"writable",   3, CellWritableOp,    4, 4, "cellName",},
};
static int numCellOps = sizeof(cellOps) / sizeof(Blt_OpSpec);

static int
CellOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;
    int result;

    proc = Blt_GetOpFromObj(interp, numCellOps, cellOps, BLT_OP_ARG2, objc, 
        objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    result = (*proc) (clientData, interp, objc, objv);
    return result;
}


/*
 *---------------------------------------------------------------------------
 *
 * CgetOp --
 *
 *      pathName cget option
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CgetOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    iconOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    tableOption.clientData = viewPtr;
    return Blt_ConfigureValueFromObj(interp, viewPtr->tkwin, tableSpecs,
        (char *)viewPtr, objv[2], 0);
}
/*
 *---------------------------------------------------------------------------
 *
 * ColumnActivateOp --
 *
 *      Sets the button to appear active.
 *
 *      pathName column activate ?colName?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnActivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                 Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Column *colPtr, *activePtr;
    
    if (GetColumnFromObj(interp, viewPtr, objv[3], &colPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if ((viewPtr->columns.flags & SHOW_TITLES) == 0)  {
        return TCL_OK;                  /* Don't draw column titles. */
    }
    if (colPtr == NULL) {
        return TCL_OK;
    }
    if (colPtr->flags & (HIDDEN | DISABLED)) {
        return TCL_OK;                  /* Disabled or hidden row. */
    }

    activePtr = viewPtr->columns.activeTitlePtr;
    viewPtr->columns.activeTitlePtr = colPtr;

    /* If we aren't already queued to redraw the widget, try to directly
     * draw into window. */
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        Drawable drawable;

        drawable = Tk_WindowId(viewPtr->tkwin);
        if (activePtr != NULL) {
            DisplayColumnTitle(viewPtr, activePtr, drawable);
        }
        DisplayColumnTitle(viewPtr, colPtr, drawable);
        return TCL_OK;
    } 
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnBboxOp --
 *
 *      pathName column bbox colName ?switches...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnBboxOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    Column *colPtr;
    TableView *viewPtr = clientData;
    Tcl_Obj *listObjPtr;
    int x1, y1, x2, y2;
    BBoxSwitches switches;
    
    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    if (viewPtr->flags & (LAYOUT_PENDING|GEOMETRY)) {
        /*
         * The layout is dirty.  Recompute it now, before we use the world
         * dimensions.  But remember that the "bbox" operation isn't valid
         * for hidden entries (since they're not visible, they don't have
         * world coordinates).
         */
        ComputeGeometry(viewPtr);
    }
    if (GetColumnFromObj(interp, viewPtr, objv[3], &colPtr)  != TCL_OK) {
        return TCL_ERROR;
    }
    if (colPtr == NULL) {
        return TCL_OK;
    }
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, bboxSwitches, objc - 4, objv + 4, 
        &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    x1 = colPtr->worldX;
    x2 = colPtr->worldX + colPtr->width;
    y1 = viewPtr->inset;
    y2 = y1 + viewPtr->columns.titleHeight;

    x1 = SCREENX(viewPtr, x1);
    x2 = SCREENX(viewPtr, x2);
    if (switches.flags & BBOX_ROOT) {
        int rootX, rootY;
        
        Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
        x1 += rootX, y1 += rootY;
        x2 += rootX, y2 += rootY;
    }
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(x1));
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(y1));
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(x2));
    Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(y2));
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnBindOp --
 *
 *      Bind a callback to an event on a column title.
 *
 *        pathName column bind tag ?-type? sequence command
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnBindOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
             Tcl_Obj *const *objv)
{
    BindTag tag;
    Column *colPtr;
    const char *string;
    int length;
    char c;
    ItemType type;

    string = Tcl_GetStringFromObj(objv[4], &length);
    type = ITEM_CELL;
    c = string[0];
    if ((c == 'c') && (strncmp(string, "cell", length) == 0)) {
        type = ITEM_CELL;
    } else if ((c == 't') && (strncmp(string, "title", length) == 0)) {
        type = ITEM_COLUMN_TITLE;
    } else if ((c == 'r') && (strncmp(string, "resize", length) == 0)) {
        type = ITEM_COLUMN_RESIZE;
    } else if ((c == 'f') && (strncmp(string, "filter", length) == 0)) {
        type = ITEM_COLUMN_FILTER;
    } else {
        Tcl_AppendResult(interp, "Bad column bind tag type \"", string, "\"",
                         (char *)NULL);
        return TCL_ERROR;
    }
    if (GetColumnFromObj(NULL, viewPtr, objv[3], &colPtr) == TCL_OK) {
        if (colPtr == NULL) {
            return TCL_OK;
        }
        tag = MakeBindTag(viewPtr, colPtr, type);
    } else {
        tag = MakeStringBindTag(viewPtr, Tcl_GetString(objv[3]), type);
    }
    return Blt_ConfigureBindingsFromObj(interp, viewPtr->bindTable, tag,
        objc - 5, objv + 5);
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnCgetOp --
 *
 *      pathName column cget colName option
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnCgetOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
             Tcl_Obj *const *objv)
{
    Column *colPtr;

    if (GetColumnFromObj(interp, viewPtr, objv[3], &colPtr) != TCL_OK){
        return TCL_ERROR;
    }
    if (colPtr == NULL) {
        return TCL_OK;
    }
    return Blt_ConfigureValueFromObj(interp, viewPtr->tkwin, columnSpecs, 
        (char *)colPtr, objv[4], 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnConfigureOp --
 *
 *      This procedure is called to process a list of configuration options
 *      database, in order to reconfigure the one of more entries in the
 *      widget.
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 * Side effects:
 *      Configuration information, such as text string, colors, font,
 *      etc. get set for viewPtr; old resources get freed, if there were
 *      any.
 *
 *      pathName column configure colName ?option value?
 *
 *---------------------------------------------------------------------------
 */
static int
ColumnConfigureOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
                  Tcl_Obj *const *objv)
{
    Column *colPtr;
    ColumnIterator iter;

    cachedObjOption.clientData = viewPtr;
    iconOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    if ((objc == 4) || (objc == 5)) {
        /* Must refer to a single if reporting the configuration options. */
        if (GetColumnFromObj(interp, viewPtr, objv[3], &colPtr) != TCL_OK) {
            return TCL_ERROR;
        }
        if (colPtr == NULL) {
            return TCL_OK;
        }
        if (objc == 4) {
            return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, columnSpecs,
                (char *)colPtr, (Tcl_Obj *)NULL, 0);
        } else if (objc == 5) {
            return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, columnSpecs,
                (char *)colPtr, objv[4], 0);
        }
    }
    if (GetColumnIterator(interp, viewPtr, objv[3], &iter) != TCL_OK) {
        return TCL_ERROR;
    }
    for (colPtr = FirstTaggedColumn(&iter); colPtr != NULL; 
         colPtr = NextTaggedColumn(&iter)) {

        if (Blt_ConfigureWidgetFromObj(interp, viewPtr->tkwin, columnSpecs, 
                objc - 4, objv + 4, (char *)colPtr, BLT_CONFIG_OBJV_ONLY) 
                != TCL_OK) {
            return TCL_ERROR;
        }
        /*FIXME: Makes every change redo everything. */
        if (Blt_ConfigModified(columnSpecs, "-formatcommand", "-style", "-icon",
                               (char *)NULL)) {
            colPtr->flags |= GEOMETRY;
            viewPtr->flags |= GEOMETRY;
        }
        ConfigureColumn(viewPtr, colPtr);
    }
    viewPtr->flags |= LAYOUT_PENDING;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnDeactivateOp --
 *
 *      Sets the column to appear normally.
 *
 *      pathName column deactivate
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnDeactivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                   Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Column *activePtr;
    
    if ((viewPtr->columns.flags & SHOW_TITLES) == 0) {
        return TCL_OK;                  /* Disabled or hidden row. */
    }
    activePtr = viewPtr->columns.activeTitlePtr;
    viewPtr->columns.activeTitlePtr = NULL;
    /* If we aren't already queued to redraw the widget, try to directly
     * draw into window. */
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        if (activePtr != NULL) {
            Drawable drawable;

            drawable = Tk_WindowId(viewPtr->tkwin);
            DisplayColumnTitle(viewPtr, activePtr, drawable);
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnDeleteOp --
 *
 *      pathName column delete ?colName ...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnDeleteOp(ClientData clientData, Tcl_Interp *interp, int objc,
               Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Blt_Chain columns;
    Blt_ChainLink link;

    /* Mark all the named columns as deleted. */
    columns = IterateColumnsObjv(interp, viewPtr, objc - 3, objv + 3);
    if (columns == NULL) {
        return TCL_ERROR;
    }
    for (link = Blt_Chain_FirstLink(columns); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Column *colPtr;

        colPtr = Blt_Chain_GetValue(link);
        DestroyColumn(colPtr);
    }
    Blt_Chain_Destroy(columns);

    /* Requires a new layout. Sort order and individual geometries stay the
     * same. */
    viewPtr->flags |= LAYOUT_PENDING;
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnExistsOp --
 *
 *      pathName column exists colName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnExistsOp(ClientData clientData, Tcl_Interp *interp, int objc,
               Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    int exists;
    Column *colPtr;

    exists = FALSE;
    if (GetColumnFromObj(NULL, viewPtr, objv[3], &colPtr) == TCL_OK) {
        exists = (colPtr != NULL);
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), exists);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnExposeOp --
 *
 *      pathName column expose ?colName ...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnExposeOp(ClientData clientData, Tcl_Interp *interp, int objc,
               Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    if (objc == 3) {
        Column *colPtr;
        Tcl_Obj *listObjPtr;

        listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
        for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
             colPtr = colPtr->nextPtr) {
            if ((colPtr->flags & HIDDEN) == 0) {
                Tcl_Obj *objPtr;

                objPtr = GetColumnIndexObj(viewPtr, colPtr);
                Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
            }
        }
        Tcl_SetObjResult(interp, listObjPtr);
    } else {
        int redraw;
        Blt_Chain columns;
        Blt_ChainLink link;
        
        columns = IterateColumnsObjv(interp, viewPtr, objc - 3, objv + 3);
        if (columns == NULL) {
            return TCL_ERROR;
        }
        redraw = FALSE;
        for (link = Blt_Chain_FirstLink(columns); link != NULL; 
             link = Blt_Chain_NextLink(link)) {
            Column *colPtr;
            
            colPtr = Blt_Chain_GetValue(link);
            if (colPtr->flags & HIDDEN) {
                colPtr->flags &= ~HIDDEN;
                redraw = TRUE;
            }
        }
        Blt_Chain_Destroy(columns);
        if (redraw) {
            viewPtr->columns.flags |= (SCROLL_PENDING | REINDEX);
            EventuallyRedraw(viewPtr);
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnFindOp --
 *
 *      pathName column find x1 y1 x2 y2 ?switches ...?
 *
 *---------------------------------------------------------------------------
 */
static int
ColumnFindOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    int x1, y1, x2, y2;
    long i;
    int rootX, rootY;

    if ((Tk_GetPixelsFromObj(interp, viewPtr->tkwin, objv[3], &x1) != TCL_OK) ||
        (Tk_GetPixelsFromObj(interp, viewPtr->tkwin, objv[4], &y1) != TCL_OK) ||
        (Tk_GetPixelsFromObj(interp, viewPtr->tkwin, objv[5], &x2) != TCL_OK) ||
        (Tk_GetPixelsFromObj(interp, viewPtr->tkwin, objv[6], &y2) != TCL_OK)) {
        return TCL_ERROR;
    } 
    rootX = rootY = 0;
#ifdef notdef
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, colFindSwitches, objc - 7, objv + 7, 
        &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    if (switches.flags & FIND_ROOT) {
        Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
    }
#endif
    if (x1 > x2) {
        int tmp;

        tmp = x2; x2 = x1; x1 = tmp;
    }
    if (y1 > y2) {
        int tmp;

        tmp = y2; y2 = y1; y1 = tmp;
    }
    y1 = WORLDX(viewPtr, y1 - rootY);
    y2 = WORLDX(viewPtr, y2 - rootY);
    if ((y2 < viewPtr->inset) || 
        (y1 >= (viewPtr->inset + viewPtr->columns.titleHeight))) {
        Tcl_SetWideIntObj(Tcl_GetObjResult(interp), -1);
        return TCL_OK;
    }
    /*
     * Since the entry positions were previously computed in world
     * coordinates, convert x-coordinates from screen to world coordinates
     * too.
     */
    x1 = WORLDX(viewPtr, x1 - rootX);
    x2 = WORLDX(viewPtr, x2 - rootX);
    for (i = viewPtr->columns.firstIndex; i <= viewPtr->columns.lastIndex; i++) {
        Column *colPtr;

        colPtr = viewPtr->columns.map[i];
        if ((x1 < (colPtr->worldX + colPtr->width)) && 
            (x2 > colPtr->worldX)) {
            size_t index;

            index = blt_table_column_index(viewPtr->table, colPtr->column);
            Tcl_SetWideIntObj(Tcl_GetObjResult(interp), index);
            return TCL_OK;
        }
    }
    Tcl_SetWideIntObj(Tcl_GetObjResult(interp), -1);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnHideOp --
 *
 *      pathName column hide ?colName ...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnHideOp(ClientData clientData, Tcl_Interp *interp, int objc,
             Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    if (objc == 3) {
        Column *colPtr;
        Tcl_Obj *listObjPtr;

        listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
        for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
             colPtr = colPtr->nextPtr) {
            if (colPtr->flags & HIDDEN) {
                Tcl_Obj *objPtr;

                objPtr = GetColumnIndexObj(viewPtr, colPtr);
                Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
            }
        }
        Tcl_SetObjResult(interp, listObjPtr);
    } else {
        int redraw;
        Blt_Chain columns;
        Blt_ChainLink link;
        
        columns = IterateColumnsObjv(interp, viewPtr, objc - 3, objv + 3);
        if (columns == NULL) {
            return TCL_ERROR;
        }
        redraw = FALSE;
        for (link = Blt_Chain_FirstLink(columns); link != NULL; 
             link = Blt_Chain_NextLink(link)) {
            Column *colPtr;
            
            colPtr = Blt_Chain_GetValue(link);
            if ((colPtr->flags & HIDDEN) == 0) {
                colPtr->flags |= HIDDEN;
                redraw = TRUE;
            }
        }
        Blt_Chain_Destroy(columns);
        if (redraw) {
            viewPtr->columns.flags |= (REINDEX | SCROLL_PENDING);
            EventuallyRedraw(viewPtr);
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnIdentifyOp --
 *
 *      pathName column identify colName x y ?switches ...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnIdentifyOp(ClientData clientData, Tcl_Interp *interp, int objc, 
              Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Column *colPtr, *nearestPtr;
    int x, y;
    IdentifySwitches switches;
    
    if (GetColumnFromObj(interp, viewPtr, objv[3], &colPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (colPtr == NULL) {
        Tcl_AppendResult(interp, "can't find column \"", Tcl_GetString(objv[3]),
                "\" in \"", Tk_PathName(viewPtr->tkwin), "\"", (char *)NULL);
        return TCL_ERROR;
    }
    if ((Tk_GetPixelsFromObj(interp, viewPtr->tkwin, objv[4], &x) != TCL_OK) ||
        (Tk_GetPixelsFromObj(interp, viewPtr->tkwin, objv[5], &y) != TCL_OK)) {
        return TCL_ERROR;
    }
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, identifySwitches, objc - 6, objv + 6, 
        &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    if (switches.flags & IDENTIFY_ROOT) {
        int rootX, rootY;
        
        Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
        x -= rootX;
        y -= rootY;
    }        
    nearestPtr = NearestColumn(viewPtr, x, FALSE);
    if (nearestPtr != colPtr) {
        return TCL_OK;
    }
    /* Determine if we're picking a column heading as opposed a cell.  */
    if (((colPtr->flags & (DISABLED|HIDDEN)) == 0) &&
        (viewPtr->columns.flags & SHOW_TITLES)) {
        const char *string;
        
        string = NULL;
        if (y < (viewPtr->inset + viewPtr->columns.titleHeight)) {
            int worldX;

            worldX = WORLDX(viewPtr, x);
            if (worldX >= (colPtr->worldX + colPtr->width - RESIZE_AREA)) {
                string = "resize";
            } else {
                string = "title";
            }
        } else if (y < (viewPtr->inset + viewPtr->columns.titleHeight + 
                        viewPtr->columns.filterHeight)) {
            string = "filter";
        }
        if (string != NULL) {
            Tcl_SetStringObj(Tcl_GetObjResult(interp), string, -1);
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnIndexOp --
 *
 *      pathName column index colName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnIndexOp(ClientData clientData, Tcl_Interp *interp, int objc, 
              Tcl_Obj *const *objv)
{
    Column *colPtr;
    TableView *viewPtr = clientData;
    ssize_t index;

    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    if (GetColumnFromObj(interp, viewPtr, objv[3], &colPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    index = (colPtr != NULL) ? 
        blt_table_column_index(viewPtr->table, colPtr->column) : -1;
    Tcl_SetWideIntObj(Tcl_GetObjResult(interp), index);
    return TCL_OK;
}


/*
 *---------------------------------------------------------------------------
 *
 * ColumnInsertOp --
 *
 *      Add new columns to the table.
 *
 *      Can you create columns that have no datatable column yet?  For what
 *      purpose?  
 *              Preset order of columns.  
 *              Preset column attribute such as title, color, format, etc. 
 *              binding?  Hold onto requested position.
 *      Don't add into column list until attached.
 *      Mark column as inserted.
 *      Additional column structure to hold unattached columns.
 *      By definition, all inserted columns should be unattached.
 *      Unattaching the table: automatically removes uninserted rows.
 *      No data table attached?  Problem: column names are through the
 *      datatable.
 *
 *      pathName column insert colName position ?option values?
 *
 *      pathName column insert "myColumn" end 
 *
 *      colName tableview column: 
 *      index tableview column: @x,y active focus current left right up down 
 *                              mark anchor
 *      colName datatable column: c1, c2, c3
 *      index datatable column: 0..n end
 *      tag datatable column: @all @end
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnInsertOp(ClientData clientData, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    BLT_TABLE_COLUMN col;
    Blt_HashEntry *hPtr;
    CellKey key;
    Column *colPtr;
    Row *rowPtr;
    TableView *viewPtr = clientData;
    const char *title;
    int isNew;
    long insertPos;
    
    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    col = blt_table_get_column(interp, viewPtr->table, objv[3]);
    if (col == NULL) {
        return TCL_ERROR;
    }
    /* Check for a valid position before creating the column.  */
    if (Blt_GetPositionFromObj(viewPtr->interp, objv[4], &insertPos) != TCL_OK){
        return TCL_ERROR;
    }
    /* 
     * Create an entry for the column. The column has to already exist in
     * the data table. 
     */
    hPtr = Blt_CreateHashEntry(&viewPtr->columns.table, (char *)col, &isNew);
    if (!isNew) {
        Tcl_AppendResult(interp, "a column \"", Tcl_GetString(objv[3]),
                "\" already exists in \"", Tk_PathName(viewPtr->tkwin),
                "\"", (char *)NULL);
        return TCL_ERROR;
    }
    colPtr = NewColumn(viewPtr, col, hPtr);
    Blt_SetHashValue(hPtr, colPtr);
    colPtr->flags |= STICKY;            /* Don't allow column to be
                                         * reset. */
    iconOption.clientData = viewPtr;
    cachedObjOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    title = GetColumnTitle(colPtr);
    if (Blt_ConfigureComponentFromObj(viewPtr->interp, viewPtr->tkwin, 
        title, "Column", columnSpecs, objc - 5, objv + 5, 
        (char *)colPtr, 0) != TCL_OK) { 
        DestroyColumn(colPtr);
        return TCL_ERROR;
    }
    if ((insertPos != -1) && (insertPos < (viewPtr->columns.length - 1))) {
        Column *destPtr;

        destPtr = viewPtr->columns.map[insertPos];
        MoveColumns(viewPtr, destPtr, colPtr, colPtr, FALSE);
    }
    key.colPtr = colPtr;
    /* Automatically populate cells for each row in the new column. */
    for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
         rowPtr = rowPtr->nextPtr) {
        Blt_HashEntry *hPtr;
        int isNew;

        key.rowPtr = rowPtr;
        hPtr = Blt_CreateHashEntry(&viewPtr->cellTable, (char *)&key, &isNew);
        if (isNew) {
            Cell *cellPtr;

            cellPtr = NewCell(viewPtr, hPtr);
            Blt_SetHashValue(hPtr, cellPtr);
        }
    }
    viewPtr->flags |= GEOMETRY;
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnInvokeOp --
 *
 *      This procedure is called to invoke a column command.
 *
 *        pathName column invoke colName
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 *      pathName column invoke colName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnInvokeOp(ClientData clientData, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    Column *colPtr;
    TableView *viewPtr = clientData;
    Tcl_Obj *objPtr, *cmdObjPtr;
    int result;
    
    if (GetColumnFromObj(interp, viewPtr, objv[3], &colPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (colPtr == NULL) {
        return TCL_OK;
    }
    cmdObjPtr = (colPtr->cmdObjPtr == NULL) 
        ? viewPtr->columns.cmdObjPtr : colPtr->cmdObjPtr;
    if (((viewPtr->columns.flags & SHOW_TITLES) == 0) || 
        (colPtr->flags & (DISABLED|HIDDEN)) || (cmdObjPtr == NULL)) {
        return TCL_OK;
    }
    Tcl_Preserve(viewPtr);
    /* command pathName colIndex  */
    cmdObjPtr = Tcl_DuplicateObj(cmdObjPtr);  
    objPtr = Tcl_NewStringObj(Tk_PathName(viewPtr->tkwin), -1);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    objPtr = GetColumnIndexObj(viewPtr, colPtr);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    Tcl_IncrRefCount(cmdObjPtr);
    result = Tcl_EvalObjEx(interp, cmdObjPtr, TCL_EVAL_GLOBAL);
    Tcl_DecrRefCount(cmdObjPtr);
    Tcl_Release(viewPtr);
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnMoveOp --
 *
 *      Move one or more columns.
 *
 *      pathName column move destCol firstCol lastCol ?switches ...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnMoveOp(ClientData clientData, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    Column *destPtr, *firstPtr, *lastPtr;
    TableView *viewPtr = clientData;
    int after = TRUE;

    if ((GetColumnFromObj(interp, viewPtr, objv[3], &destPtr) != TCL_OK) ||
        (GetColumnFromObj(interp, viewPtr, objv[4], &firstPtr) != TCL_OK) ||
        (GetColumnFromObj(interp, viewPtr, objv[5], &lastPtr) != TCL_OK)) {
        return TCL_ERROR;
    }
    if (viewPtr->columns.flags & REINDEX) {
        RenumberColumns(viewPtr);
    }

    /* Check if range is valid. */
    if (firstPtr->index > lastPtr->index) {
        return TCL_OK;                  /* No range. */
    }

    /* Check that destination is outside the range of columns to be moved. */
    if ((destPtr->index >= firstPtr->index) &&
        (destPtr->index <= lastPtr->index)) {
        Tcl_AppendResult(interp, "destination column \"", 
                Tcl_GetString(objv[3]),
                 "\" can't be in the range of columns to be moved", 
                (char *)NULL);
        return TCL_ERROR;
    }
    MoveColumns(viewPtr, destPtr, firstPtr, lastPtr, after);
    /* FIXME: Layout changes with move but not geometry. */
    viewPtr->flags |= GEOMETRY;
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}


/*
 *---------------------------------------------------------------------------
 *
 * ColumnNamesOp --
 *
 *      pathName column names ?pattern?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnNamesOp(ClientData clientData, Tcl_Interp *interp, int objc, 
              Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Tcl_Obj *listObjPtr;
    Column *colPtr;

    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
         colPtr = colPtr->nextPtr) {
        const char *label;
        int found;
        int i;

        found = TRUE;
        label = blt_table_column_label(colPtr->column);
        for (i = 3; i < objc; i++) {
            const char *pattern;

            pattern = Tcl_GetString(objv[i]);
            found = Tcl_StringMatch(label, pattern);
            if (found) {
                break;
            }
        }
        if (found) {
            Tcl_ListObjAppendElement(interp, listObjPtr, 
                                     Tcl_NewStringObj(label, -1));
        }
    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

static int
ColumnNearestOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    int x;                         /* Screen coordinates of the test point. */
    Column *colPtr;
    ssize_t index;
    NearestSwitches switches;

    if (Tk_GetPixelsFromObj(interp, viewPtr->tkwin, objv[3], &x) != TCL_OK) {
        return TCL_ERROR;
    } 
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, nearestSwitches, objc - 4, objv + 4, 
        &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    if (switches.flags & NEAREST_ROOT) {
        int rootX, rootY;
        
        Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
        x += rootX;
    }
    colPtr = NearestColumn(viewPtr, x, TRUE);
    index = -1;
    if ((viewPtr->table != NULL) && (colPtr != NULL)) {
        index = blt_table_column_index(viewPtr->table, colPtr->column);
    }
    Tcl_SetWideIntObj(Tcl_GetObjResult(interp), index);
    return TCL_OK;
}

static void
UpdateColumnMark(TableView *viewPtr, int newMark)
{
    Column *colPtr;
    int dx;
    int width;

    colPtr = viewPtr->columns.resizePtr;
    if (colPtr == NULL) {
        return;
    }
    dx = newMark - viewPtr->columns.resizeAnchor; 
    width = colPtr->width;
    if ((colPtr->reqWidth.min > 0) && ((width + dx) < colPtr->reqWidth.min)) {
        dx = colPtr->reqWidth.min - width;
    }
    if ((colPtr->reqWidth.max > 0) && ((width + dx) > colPtr->reqWidth.max)) {
        dx = colPtr->reqWidth.max - width;
    }
    if ((width + dx) < 4) {
        dx = 4 - width;
    }
    viewPtr->columns.resizeMark = viewPtr->columns.resizeAnchor + dx;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnResizeActivateOp --
 *
 *      Turns on/off the resize cursor.
 *
 *      pathName column resize activate colName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnResizeActivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                       Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Column *colPtr;

    if (GetColumnFromObj(interp, viewPtr, objv[4], &colPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if ((colPtr == NULL) || (colPtr->flags & (HIDDEN|DISABLED))){
        return TCL_OK;
    }
    if (viewPtr->columns.resizeCursor != None) {
        Tk_DefineCursor(viewPtr->tkwin, viewPtr->columns.resizeCursor);
    } 
    viewPtr->columns.resizePtr = colPtr;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnResizeAnchorOp --
 *
 *      Set the anchor for the resize.
 *
 *      pathName column resize anchor ?x?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnResizeAnchorOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                  Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    if (objc == 5) { 
        int x;

        if (Tcl_GetIntFromObj(interp, objv[4], &x) != TCL_OK) {
            return TCL_ERROR;
        } 
        viewPtr->columns.resizeAnchor = x;
        UpdateColumnMark(viewPtr, x);
    }
    Tcl_SetIntObj(Tcl_GetObjResult(interp), viewPtr->columns.resizeAnchor);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnResizeDeactiveOp --
 *
 *      Turns off the resize cursor.
 *
 *      pathName column resize deactivate 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnResizeDeactivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                         Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    if (viewPtr->cursor != None) {
        Tk_DefineCursor(viewPtr->tkwin, viewPtr->cursor);
    } else {
        Tk_UndefineCursor(viewPtr->tkwin);
    }
    viewPtr->columns.resizePtr = NULL;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnResizeMarkOp --
 *
 *      Sets the resize mark.  The distance between the mark and the anchor
 *      is the delta to change the width of the active column.
 *
 *      pathName column resize mark ?x?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnResizeMarkOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                   Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    if (objc == 5) { 
        Column *colPtr;
        int x;

        if (Tcl_GetIntFromObj(interp, objv[4], &x) != TCL_OK) {
            return TCL_ERROR;
        } 
        UpdateColumnMark(viewPtr, x);
        colPtr = viewPtr->columns.resizePtr;
        if (colPtr != NULL) {
            int dx;
            
            dx = (viewPtr->columns.resizeMark - viewPtr->columns.resizeAnchor);
            colPtr->reqWidth.nom = colPtr->width + dx;
            colPtr->reqWidth.flags |= LIMITS_SET_NOM;
            viewPtr->columns.resizeAnchor = viewPtr->columns.resizeMark;
            viewPtr->flags |= LAYOUT_PENDING;
            EventuallyRedraw(viewPtr);
        }
    }
    Tcl_SetIntObj(Tcl_GetObjResult(interp), viewPtr->columns.resizeMark);
    return TCL_OK;
}

static Blt_OpSpec columnResizeOps[] =
{ 
    {"activate",   2, ColumnResizeActivateOp,   5, 5, "column"},
    {"anchor",     2, ColumnResizeAnchorOp,     4, 5, "?x?"},
    {"deactivate", 1, ColumnResizeDeactivateOp, 4, 4, ""},
    {"mark",       1, ColumnResizeMarkOp,       4, 5, "?x?"},
};

static int numColumnResizeOps = sizeof(columnResizeOps) / sizeof(Blt_OpSpec);

/*
 *---------------------------------------------------------------------------
 *
 * ColumnResizeOp --
 *
 *---------------------------------------------------------------------------
 */
static int
ColumnResizeOp(ClientData clientData, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numColumnResizeOps, columnResizeOps, 
        BLT_OP_ARG3, objc, objv,0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}


/*
 *---------------------------------------------------------------------------
 *
 * ColumnSeeOp --
 *
 *      pathName column see col
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnSeeOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    Column *colPtr;
    TableView *viewPtr = clientData;
    long xOffset;

    if (GetColumnFromObj(interp, viewPtr, objv[3], &colPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (colPtr == NULL) {
        return TCL_OK;
    }
    xOffset = GetColumnXOffset(viewPtr, colPtr);
    if (xOffset != viewPtr->columns.scrollOffset) {
        viewPtr->columns.scrollOffset = xOffset;
        viewPtr->columns.flags |= SCROLL_PENDING;
        EventuallyRedraw(viewPtr);
    }
    return TCL_OK;
}


/*
 *---------------------------------------------------------------------------
 *
 * ColumnSlideContinueOp --
 *
 *      This procedure is called to continue the slide operation.
 *
 *        pathName column slide continue x
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnSlideContinueOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                      Tcl_Obj *const *objv)
{
    Column *colPtr;
    TableView *viewPtr = clientData; 
    int x, dx;
    int offset;
    int fullRedraw;

    if (Blt_GetPixelsFromObj(interp, viewPtr->tkwin, objv[4], PIXELS_ANY, &x)
        != TCL_OK) {
        return TCL_ERROR;
    }
    if ((viewPtr->columns.flags & SLIDE_ENABLED) == 0)  {
        return TCL_OK;                  /* Sliding turned off. */
    }
    if (viewPtr->columns.slidePtr == NULL) {
        Tcl_AppendResult(interp, "No column designated for sliding.  "
            "Must call \"column slide anchor\" first", (char *)NULL); 
        return TCL_ERROR;
    }
    dx = x - viewPtr->columns.slideAnchor;
    if ((viewPtr->columns.flags & SLIDE_ACTIVE) == 0) { 
        if (ABS(dx) > 10) {
            viewPtr->columns.flags |= SLIDE_ACTIVE;
        }
    }        
    if ((viewPtr->columns.flags & SLIDE_ACTIVE) == 0)  {
        return TCL_OK;
    }
    viewPtr->columns.slideAnchor = x;
    offset = viewPtr->columns.slideOffset + dx;
    colPtr = viewPtr->columns.slidePtr;

    /* Auto-scroll if left or right of column titles. */
    if (x < 0) {
        Column *prevPtr;
        
        prevPtr = GetPrevColumn(colPtr);
        if (prevPtr == NULL) {
            return TCL_OK;              /* Don't move column, there's no
                                         * column before this one. */
        }
        viewPtr->columns.scrollOffset -= 10;
        offset -= 10;
#ifdef notdef
        viewPtr->columns.flags |= SCROLL_PENDING;
        EventuallyRedrawColumnTitles(viewPtr);
        return TCL_OK;
#endif
    } else if (x >= Tk_Width(viewPtr->tkwin)) {
        Column *nextPtr;
        
        nextPtr = GetNextColumn(colPtr);
        if (nextPtr == NULL) {
            return TCL_OK;              /* Don't move column, there's no column
                                         * after this one. */
        }
        viewPtr->columns.scrollOffset += 10; 
        offset += 10;
#ifdef notdef
        viewPtr->columns.flags |= SCROLL_PENDING;
        viewPtr->flags |= LAYOUT_PENDING;
        EventuallyRedrawColumnTitles(viewPtr);
        return TCL_OK;
#endif
    }

    fullRedraw = FALSE;
    if (offset < 0) {
        Column *prevPtr;
        int d;
        
        prevPtr = GetPrevColumn(colPtr);
        if (prevPtr == NULL) {
            return TCL_OK;              /* Can't swap columns. There's no
                                         * column before this one. */
        }
        d = -prevPtr->width;            
        if (offset < (d * 2/3)) {
            viewPtr->flags |= LAYOUT_PENDING;
            viewPtr->columns.flags |= SCROLL_PENDING;
            MoveColumns(viewPtr, prevPtr, colPtr, colPtr, FALSE);
            fullRedraw = TRUE;
            offset -= d;
        }
    } else {
        Column *nextPtr;
        int d;

        nextPtr = GetNextColumn(colPtr);
        if (nextPtr == NULL) {
            return TCL_OK;              /* Can't swap columns. There's no
                                         * column after this one. */
        }
        d = nextPtr->width;
        if (offset > (d * 2/3)) {
            /* Swap tab positions and reset slide offset. */
            viewPtr->flags |= LAYOUT_PENDING;
            viewPtr->columns.flags |= SCROLL_PENDING;
            MoveColumns(viewPtr, nextPtr, colPtr, colPtr, TRUE);
            fullRedraw = TRUE;
            offset -= d;
        }
    }
    viewPtr->columns.flags |= SCROLL_PENDING;
    if (fullRedraw) {
        EventuallyRedraw(viewPtr);
    } else {
        EventuallyRedrawColumnTitles(viewPtr);
    }
    viewPtr->columns.slideOffset = offset;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnSlideIsActiveOp --
 *
 *      Returns if column sliding is active.  Column sliding is activated
 *      when the pointer is moved greater than 10 pixels horizonatally (in
 *      either direction) from the anchor point.
 *
 *        pathName column slide isactive
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnSlideIsActiveOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                      Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData; 
    int state;
    
    state = ((viewPtr->columns.flags & SLIDE_ACTIVE) != 0);
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnSlideIsAutoOp --
 *
 *      Indicates if automatic scrolling is turned on for column sliding.
 *      If given x-coordinate is outside of the column titles (to the left
 *      or right), then the columns will be automatically scrolled.
 *
 *        pathName column slide isauto x
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnSlideIsAutoOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                    Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData; 
    int x, dx;
    int state;
    
    if (Blt_GetPixelsFromObj(interp, viewPtr->tkwin, objv[4], PIXELS_ANY, &x)
        != TCL_OK) {
        return TCL_ERROR;
    }
    if (viewPtr->columns.slidePtr == NULL) {
        Tcl_SetBooleanObj(Tcl_GetObjResult(interp), FALSE);
        return TCL_OK;                  /* No slide is designated. */
    }
    dx = x - viewPtr->columns.slideAnchor;
    if ((viewPtr->columns.flags & SLIDE_ACTIVE) == 0) {
        if (ABS(dx) > 10) {
            viewPtr->columns.flags |= SLIDE_ACTIVE;
        }
    }        
    if ((viewPtr->columns.flags & SLIDE_ACTIVE) == 0)  {
        Tcl_SetBooleanObj(Tcl_GetObjResult(interp), FALSE);
        return TCL_OK;
    }
    state = (x < 0) || (x >= Tk_Width(viewPtr->tkwin));
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnSlideStartOp --
 *
 *      This procedure is called to start a column slide operation.  The
 *      designated column in made the current slide column.  The slide
 *      offset is reset to 0 and the current x-coordinate screen coordinate
 *      is saved as the slide anchor.  
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 *      pathName column slide anchor colName x
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnSlideStartOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                   Tcl_Obj *const *objv)
{
    Column *colPtr;
    TableView *viewPtr = clientData; 
    int x;
    
    if ((viewPtr->columns.flags & SLIDE_ENABLED) == 0)  {
        return TCL_OK;                  /* Sliding turned off. */
    }
    if (GetColumnFromObj(interp, viewPtr, objv[4], &colPtr) != TCL_OK) {
        return TCL_ERROR;               /* Can't find column. */
    }
    if (colPtr == NULL) {
        return TCL_OK;
    }
    if (Blt_GetPixelsFromObj(interp, viewPtr->tkwin, objv[5], PIXELS_ANY, &x)
        != TCL_OK) {
        return TCL_ERROR;
    }
    viewPtr->columns.slidePtr = colPtr;
    viewPtr->columns.slideAnchor = x;
    viewPtr->columns.slideOffset = 0;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnSlideStopOp --
 *
 *      This procedure is called to end the slide operation.
 *
 *        pathName column slide stop
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ColumnSlideStopOp(ClientData clientData, Tcl_Interp *interp, int objc, 
             Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData; 
    
    viewPtr->columns.slideOffset = 0;
    viewPtr->columns.slidePtr = NULL;
    viewPtr->columns.flags &= ~SLIDE_ACTIVE;
    viewPtr->flags |= LAYOUT_PENDING;
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * ColumnSlideOp --
 *
 *      This procedure handles sliding column operations.
 *
 * Results:
 *      A standard TCL result.
 *
 *      pathName column slide start colName x
 *      pathName column slide continue x
 *      pathName column slide isactive
 *      pathName column slide isauto x
 *      pathName column slide stop
 *
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec columnSlideOps[] =
{
    {"continue", 1, ColumnSlideContinueOp,  5, 5, "x" }, 
    {"isactive", 4, ColumnSlideIsActiveOp,  4, 4, "" }, 
    {"isauto",   4, ColumnSlideIsAutoOp,    5, 5, "x" }, 
    {"start",    3, ColumnSlideStartOp,     6, 6, "colName x" }, 
    {"stop",     3, ColumnSlideStopOp,      4, 4, "" }, 
};

static int numColumnSlideOps = sizeof(columnSlideOps) / sizeof(Blt_OpSpec);

static int
ColumnSlideOp(ClientData clientData, Tcl_Interp *interp, int objc, 
              Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numColumnSlideOps, columnSlideOps, 
        BLT_OP_ARG3, objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc)(clientData, interp, objc, objv);
}

static Blt_OpSpec columnOps[] = {
    {"activate",   1, ColumnActivateOp,   4, 4, "colName"}, 
    {"bbox",       2, ColumnBboxOp,       4, 0, "colName ?switches ...?"},
    {"bind",       2, ColumnBindOp,       5, 7, "tagName type ?sequence command?"},
    {"cget",       2, ColumnCgetOp,       5, 5, "colName option"}, 
    {"configure",  2, ColumnConfigureOp,  4, 0, "colName ?option value ...?"}, 
    {"deactivate", 2, ColumnDeactivateOp, 3, 3, ""},
    {"delete",     2, ColumnDeleteOp,     3, 0, "colName..."}, 
    {"exists",     3, ColumnExistsOp,     4, 4, "colName"}, 
    {"expose",     3, ColumnExposeOp,     3, 0, "?colName ...?"},
    {"find",       1, ColumnFindOp,       7, 7, "x1 y1 x2 y2"},
    {"hide",       1, ColumnHideOp,       3, 0, "?colName ...?"},
    {"identify",   2, ColumnIdentifyOp,   6, 6, "colName x y"}, 
    {"index",      3, ColumnIndexOp,      4, 4, "colName"}, 
    {"insert",     3, ColumnInsertOp,     5, 0, "colName pos ?option value ...?"},  
    {"invoke",     3, ColumnInvokeOp,     4, 4, "colName"},  
    {"move",       1, ColumnMoveOp,       6, 0, "destCol firstCol lastCol ?switches?"},  
    {"names",      2, ColumnNamesOp,      3, 0, "?colName ...?"},
    {"nearest",    2, ColumnNearestOp,    4, 0, "x ?switches ...?"},
    {"resize",     1, ColumnResizeOp,     3, 0, "args"},
    {"see",        2, ColumnSeeOp,        4, 4, "colName"}, 
    {"show",       2, ColumnExposeOp,     3, 0, "?colName ...?"},
    {"slide",      2, ColumnSlideOp,      3, 0, "args"}, 
};
static int numColumnOps = sizeof(columnOps) / sizeof(Blt_OpSpec);

/*
 *---------------------------------------------------------------------------
 *
 * ColumnOp --
 *
 *---------------------------------------------------------------------------
 */
static int
ColumnOp(ClientData clientData, Tcl_Interp *interp, int objc, 
         Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numColumnOps, columnOps, BLT_OP_ARG2, 
        objc, objv,0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}

/*
 *---------------------------------------------------------------------------
 *
 * ConfigureOp --
 *
 *      This procedure is called to process an objv/objc list, plus the Tk
 *      option database, in order to configure (or reconfigure) the widget.
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then interp->result
 *      contains an error message.
 *
 * Side effects:
 *      Configuration information, such as text string, colors, font,
 *      etc. get set for viewPtr; old resources get freed, if there were
 *      any.  The widget is redisplayed. 
 *
 *---------------------------------------------------------------------------
 */
static int
ConfigureOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    iconOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    tableOption.clientData = viewPtr;
    if (objc == 2) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, tableSpecs, 
                (char *)viewPtr, (Tcl_Obj *)NULL, 0);
    } else if (objc == 3) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, tableSpecs,
                (char *)viewPtr, objv[2], 0);
    } 
    if (Blt_ConfigureWidgetFromObj(interp, viewPtr->tkwin, tableSpecs, 
        objc - 2, objv + 2, (char *)viewPtr, BLT_CONFIG_OBJV_ONLY) != TCL_OK) {
        return TCL_ERROR;
    }
    if (ConfigureTableView(interp, viewPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * CurselectionOp --
 *
 *      pathName curselection
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
CurselectionOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    Tcl_Obj *listObjPtr;

    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    switch (viewPtr->selectMode) {
    case SELECT_CELLS:
        {
            Blt_HashEntry *hPtr;
            Blt_HashSearch iter;
            CellSelection *selPtr = &viewPtr->selectCells;

            for (hPtr = Blt_FirstHashEntry(&selPtr->cellTable, &iter); 
                 hPtr != NULL; hPtr = Blt_NextHashEntry(&iter)) {
                CellKey *keyPtr;
                Tcl_Obj *objPtr, *subListObjPtr;
                
                keyPtr = Blt_GetHashValue(hPtr);
                subListObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
                objPtr = GetRowIndexObj(viewPtr, keyPtr->rowPtr);
                Tcl_ListObjAppendElement(interp, subListObjPtr, objPtr);
                objPtr = GetColumnIndexObj(viewPtr, keyPtr->colPtr);
                Tcl_ListObjAppendElement(interp, subListObjPtr, objPtr);
                Tcl_ListObjAppendElement(interp, listObjPtr, subListObjPtr);
            }
        }
        break;
    case SELECT_SINGLE_ROW:
    case SELECT_MULTIPLE_ROWS:
        if (viewPtr->flags & SELECT_SORTED) {
            Blt_ChainLink link;
            
            for (link = Blt_Chain_FirstLink(viewPtr->rows.selection.list); 
                 link != NULL; link = Blt_Chain_NextLink(link)) {
                Row *rowPtr;
                Tcl_Obj *objPtr;
                
                rowPtr = Blt_Chain_GetValue(link);
                objPtr = GetRowIndexObj(viewPtr, rowPtr);
                Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
            }
        } else {
            Row *rowPtr;
            
            for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL;
                 rowPtr = rowPtr->nextPtr) {
                if (rowPtr->flags & SELECTED) {
                    Tcl_Obj *objPtr;
                    
                    objPtr = GetRowIndexObj(viewPtr, rowPtr);
                    Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
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
 * DeactivateOp --
 *
 *      Makes the formerly active cell appear normal.
 *
 *      pathName deactivate 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
DeactivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
             Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Cell *activePtr;

    activePtr = viewPtr->activePtr;
    viewPtr->activePtr = NULL;
    /* If we aren't already queued to redraw the widget, try to directly draw
     * into window. */
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        if (activePtr != NULL) {
            Drawable drawable;

            drawable = Tk_WindowId(viewPtr->tkwin);
            DisplayCell(viewPtr, activePtr, drawable, TRUE);
        }
    }
    return TCL_OK;
}

typedef struct {
    BLT_TABLE table;                    /* Table to be evaluated */
    BLT_TABLE_ROW row;                  /* Current row. */
    Blt_HashTable varTable;             /* Variable cache. */
    TableView *viewPtr;

    /* Public values */
    Tcl_Obj *emptyValueObjPtr;
    const char *tag;
    unsigned int flags;
} FindSwitches;

#define FIND_INVERT     (1<<0)

static Blt_SwitchSpec findSwitches[] = 
{
    {BLT_SWITCH_OBJ, "-emptyvalue", "string",   (char *)NULL,
        Blt_Offset(FindSwitches, emptyValueObjPtr), 0},
    {BLT_SWITCH_STRING, "-addtag", "tagName", (char *)NULL,
        Blt_Offset(FindSwitches, tag), 0},
    {BLT_SWITCH_BITS_NOARG, "-invert", "", (char *)NULL,
        Blt_Offset(FindSwitches, flags), 0, FIND_INVERT},
    {BLT_SWITCH_END}
};

static int
ColumnVarResolverProc(
    Tcl_Interp *interp,                 /* Current interpreter. */
    const char *name,                   /* Variable name being resolved. */
    Tcl_Namespace *nsPtr,               /* Current namespace context. */
    int flags,                          /* TCL_LEAVE_ERR_MSG => leave error
                                         * message. */
    Tcl_Var *varPtr)                    /* (out) Resolved variable. */ 
{
    Blt_HashEntry *hPtr;
    BLT_TABLE_COLUMN col;
    FindSwitches *switchesPtr;
    Tcl_Obj *valueObjPtr;
    int64_t index;
    
    /* 
     * Global variables:  table, viewPtr, varTable, rowPtr.
     */
    hPtr = Blt_FindHashEntry(&findTable, nsPtr);
    if (hPtr == NULL) {
        /* This should never happen.  We can't find data associated with
         * the current namespace.  But this routine should never be called
         * unless we're in a namespace that with linked with this variable
         * resolver. */
        return TCL_CONTINUE;    
    }
    switchesPtr = Blt_GetHashValue(hPtr);

    /* Look up the column from the variable name given. */
    if ((isdigit(name[0])) &&
        (Blt_GetInt64((Tcl_Interp *)NULL, (char *)name, &index) == TCL_OK)) {
        col = blt_table_get_column_by_index(switchesPtr->table, index);
    } else {
        col = blt_table_get_column_by_label(switchesPtr->table, name);
    }
    if (col == NULL) {
        /* Variable name doesn't refer to any column. Pass it back to the
         * TCL interpreter and let it resolve it normally. */
        return TCL_CONTINUE;
    }
    valueObjPtr = blt_table_get_obj(switchesPtr->table, switchesPtr->row, col);
    if (valueObjPtr == NULL) {
        valueObjPtr = switchesPtr->emptyValueObjPtr;
        if (valueObjPtr == NULL) {
            return TCL_CONTINUE;
        }
    }
    *varPtr = Blt_GetCachedVar(&switchesPtr->varTable, name, valueObjPtr);
    return TCL_OK;
}

static int
EvaluateExpr(Tcl_Interp *interp, Tcl_Obj *exprObjPtr, int *boolPtr)
{
    Tcl_Obj *resultObjPtr;
    int bool;

    if (Tcl_ExprObj(interp, exprObjPtr, &resultObjPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (Tcl_GetBooleanFromObj(interp, resultObjPtr, &bool) != TCL_OK) {
        return TCL_ERROR;
    }
    Tcl_DecrRefCount(resultObjPtr);
    *boolPtr = bool;
    return TCL_OK;
}

static int
FindRows(Tcl_Interp *interp, TableView *viewPtr, Tcl_Obj *objPtr, 
         FindSwitches *switchesPtr)
{
    Blt_HashEntry *hPtr;
    Row *rowPtr;
    Tcl_Namespace *nsPtr;
    Tcl_Obj *listObjPtr;
    int isNew;
    int result = TCL_OK;

    Tcl_AddInterpResolvers(interp, TABLEVIEW_FIND_KEY, 
        (Tcl_ResolveCmdProc*)NULL, ColumnVarResolverProc, 
        (Tcl_ResolveCompiledVarProc*)NULL);

    Blt_InitHashTable(&switchesPtr->varTable, BLT_ONE_WORD_KEYS);

    if (!initialized) {
        Blt_InitHashTable(&findTable, BLT_ONE_WORD_KEYS);
        initialized = TRUE;
    }
    nsPtr = Tcl_GetCurrentNamespace(interp);
    hPtr = Blt_CreateHashEntry(&findTable, nsPtr, &isNew);
    assert(isNew);
    Blt_SetHashValue(hPtr, switchesPtr);

    /* Now process each row, evaluating the expression. */
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **) NULL);
    for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
         rowPtr = rowPtr->nextPtr) {
        int bool;

        if (rowPtr->flags & HIDDEN) {
            continue;                   /* Ignore hidden rows. */
        }
        switchesPtr->row = rowPtr->row;
        result = EvaluateExpr(interp, objPtr, &bool);
        if (result != TCL_OK) {
            break;
        }
        if (switchesPtr->flags & FIND_INVERT) {
            bool = !bool;
        }
        if (bool) {
            Tcl_Obj *objPtr;

            if (switchesPtr->tag != NULL) {
                result = blt_table_set_row_tag(interp, viewPtr->table, 
                        rowPtr->row, switchesPtr->tag);
                if (result != TCL_OK) {
                    break;
                }
            }
            objPtr = GetRowIndexObj(viewPtr, rowPtr);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
        }
    }
    if (result != TCL_OK) {
        Tcl_DecrRefCount(listObjPtr);
    } else {
        Tcl_SetObjResult(interp, listObjPtr);
    }
    /* Clean up. */
    Blt_DeleteHashEntry(&findTable, hPtr);
    Blt_FreeCachedVars(&switchesPtr->varTable);
    if (!Tcl_RemoveInterpResolvers(interp, TABLEVIEW_FIND_KEY)) {
        Tcl_AppendResult(interp, "can't delete resolver scheme", 
                (char *)NULL);
        return TCL_ERROR;
    }
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * FilterActivateOp --
 *
 *      Sets the filter to appear active.
 *
 *      pathName filter activate ?col?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
FilterActivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                 Tcl_Obj *const *objv)
{
    Column *colPtr, *activePtr;
    FilterInfo *filterPtr;
    TableView *viewPtr = clientData;

    if (GetColumnFromObj(interp, viewPtr, objv[3], &colPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (((viewPtr->columns.flags & SHOW_TITLES) == 0) || (colPtr == NULL) ||
        (colPtr->flags & (HIDDEN | DISABLED))) {
        return TCL_OK;                  /* Disabled or hidden row. */
    }
    filterPtr = &viewPtr->filter;
    activePtr = filterPtr->activePtr;
    filterPtr->activePtr = colPtr;

    /* If we aren't already queued to redraw the widget, try to directly
     * draw into window. */
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        Drawable drawable;

        drawable = Tk_WindowId(viewPtr->tkwin);
        if (activePtr != NULL) {
            DisplayColumnFilter(viewPtr, activePtr, drawable);
        }
        DisplayColumnFilter(viewPtr, colPtr, drawable);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * FilterCgetOp --
 *
 *      pathName filter cget -option
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
FilterCgetOp(ClientData clientData, Tcl_Interp *interp, int objc, 
             Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    return Blt_ConfigureValueFromObj(interp, viewPtr->tkwin, filterSpecs, 
        (char *)viewPtr, objv[3], 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * FilterConfigureOp --
 *
 *      This procedure is called to process a list of configuration
 *      options database, in order to reconfigure the one of more
 *      entries in the widget.
 *
 *        pathName filter configure option value
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 *      pathName filter configure 
 *---------------------------------------------------------------------------
 */
static int
FilterConfigureOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                  Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    if (objc == 3) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, filterSpecs, 
                (char *)viewPtr, (Tcl_Obj *)NULL, 0);
    } else if (objc == 4) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, filterSpecs, 
                (char *)viewPtr, objv[3], 0);
    }
    if (Blt_ConfigureWidgetFromObj(interp, viewPtr->tkwin, filterSpecs, 
        objc - 3, objv + 3, (char *)viewPtr, BLT_CONFIG_OBJV_ONLY) != TCL_OK) {
        return TCL_ERROR;
    }
    viewPtr->flags |= LAYOUT_PENDING;
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * FilterDeactivateOp --
 *
 *      Sets the filter to appear normally.
 *
 *      pathName filter deactivate
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
FilterDeactivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                   Tcl_Obj *const *objv)
{
    Column *activePtr;
    FilterInfo *filterPtr;
    TableView *viewPtr = clientData;
    
    if ((viewPtr->columns.flags & SHOW_TITLES) == 0) {
        return TCL_OK;                  /* Disabled or hidden row. */
    }
    filterPtr = &viewPtr->filter;
    activePtr = filterPtr->activePtr;
    filterPtr->activePtr = NULL;
    /* If we aren't already queued to redraw the widget, try to directly draw
     * into window. */
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        if (activePtr != NULL) {
            Drawable drawable;

            drawable = Tk_WindowId(viewPtr->tkwin);
            DisplayColumnFilter(viewPtr, activePtr, drawable);
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * FilterInsideOp --
 *
 *      pathName filter inside cell x y
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
FilterInsideOp(ClientData clientData, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    Column *colPtr;
    TableView *viewPtr = clientData;
    int state;
    int x, y, rootX, rootY;

    if (GetColumnFromObj(interp, viewPtr, objv[3], &colPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (colPtr == NULL) {
        fprintf(stderr, "FilterInside: Column %s is NULL\n", Tcl_GetString(objv[3])); 
    }
    if ((Tcl_GetIntFromObj(interp, objv[4], &x) != TCL_OK) ||
        (Tcl_GetIntFromObj(interp, objv[5], &y) != TCL_OK)) {
        return TCL_ERROR;
    }
    /* Convert from root coordinates to window-local coordinates */
    Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
    x -= rootX, y -= rootY;
    state = FALSE;
    if (colPtr != NULL) {
        x = WORLDX(viewPtr, x);
        if ((x >= colPtr->worldX) && (x < (colPtr->worldX + colPtr->width)) &&
            (y >= viewPtr->inset + viewPtr->columns.titleHeight) && 
            (y < (viewPtr->inset + viewPtr->columns.titleHeight + 
                  viewPtr->columns.filterHeight))) {
            state = TRUE;
        }
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * FilterPostOp --
 *
 *      Posts the filter menu associated with this widget.
 *
 * Results:
 *      Standard TCL result.
 *
 * Side effects:
 *      Commands may get excecuted; variables may get set; sub-menus may
 *      get posted.
 *
 *      pathName filter post col
 *
 *---------------------------------------------------------------------------
 */
static int
FilterPostOp(ClientData clientData, Tcl_Interp *interp, int objc, 
             Tcl_Obj *const *objv)
{
    Column *colPtr;
    FilterInfo *filterPtr;
    TableView *viewPtr = clientData;
    Tk_Window tkwin;
    const char *menuName;
    int result;
    int rootX, rootY;
    int x1, y1, x2, y2;

    filterPtr = &viewPtr->filter;
    if (objc == 3) {
        ssize_t index;

        /* Report the column that has the filter menu posted. */
        index = -1;
        if (filterPtr->postPtr != NULL) {
            index = blt_table_column_index(viewPtr->table, 
                                           filterPtr->postPtr->column);
        }
        Tcl_SetWideIntObj(Tcl_GetObjResult(interp), index);
        return TCL_OK;
    }
    if (GetColumnFromObj(interp, viewPtr, objv[3], &colPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (colPtr == NULL) {
        return TCL_OK;
    }
    if (filterPtr->postPtr != NULL) {
        return TCL_OK;                 /* Another filter's menu is currently
                                        * posted. */
    }
    if (colPtr->flags & (DISABLED|HIDDEN)) {
        return TCL_OK;                 /* Filter's menu is in a column that is
                                        * hidden or disabled. */
    }
    if (filterPtr->menuObjPtr == NULL) {
        return TCL_OK;                  /* No menu associated with filter. */
    }
    menuName = Tcl_GetString(filterPtr->menuObjPtr);
    tkwin = Tk_NameToWindow(interp, menuName, viewPtr->tkwin);
    if (tkwin == NULL) {
        return TCL_ERROR;
    }
    if (Tk_Parent(tkwin) != viewPtr->tkwin) {
        Tcl_AppendResult(interp, "can't post \"", Tk_PathName(tkwin), 
                "\": it isn't a descendant of ", Tk_PathName(viewPtr->tkwin),
                (char *)NULL);
        return TCL_ERROR;
    }

    Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
    x1 = SCREENX(viewPtr, colPtr->worldX) + rootX;
    x2 = x1 + colPtr->width;
    y1 = viewPtr->inset + viewPtr->columns.titleHeight + rootY;
    y2 = y1 + viewPtr->columns.filterHeight;
    
    result = TCL_ERROR;
    if (filterPtr->postCmdObjPtr != NULL) {
        Tcl_Obj *cmdObjPtr;

        /* Call the designated post command for the filter menu. Pass it the
         * bounding box of the filter button so it can arrange itself */
        cmdObjPtr = Tcl_DuplicateObj(filterPtr->postCmdObjPtr);
        Tcl_ListObjAppendElement(interp, cmdObjPtr, Tcl_NewIntObj(x2));
        Tcl_ListObjAppendElement(interp, cmdObjPtr, Tcl_NewIntObj(y2));
        Tcl_ListObjAppendElement(interp, cmdObjPtr, Tcl_NewIntObj(x1));
        Tcl_ListObjAppendElement(interp, cmdObjPtr, Tcl_NewIntObj(y1));
        Tcl_IncrRefCount(cmdObjPtr);
        Tcl_Preserve(viewPtr);
        result = Tcl_EvalObjEx(interp, cmdObjPtr, TCL_EVAL_GLOBAL);
        Tcl_Release(viewPtr);
        Tcl_DecrRefCount(cmdObjPtr);
        if (result == TCL_OK) {
            filterPtr->postPtr = colPtr;
        }
        if (result != TCL_OK) {
            return TCL_ERROR;
        }
    }
    if (strcmp(Tk_Class(tkwin), "BltComboMenu") == 0) {
        Tcl_Obj *cmdObjPtr, *objPtr, *listObjPtr;

        cmdObjPtr = Tcl_DuplicateObj(filterPtr->menuObjPtr);
        /* menu post -align right -box {x1 y1 x2 y2}  */
        objPtr = Tcl_NewStringObj("post", 4);
        Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
        objPtr = Tcl_NewStringObj("-align", 6);
        Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
        objPtr = Tcl_NewStringObj("right", 5);
        Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
        objPtr = Tcl_NewStringObj("-box", 4);
        Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
        listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
        Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(x2));
        Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(y2));
        Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(x1));
        Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewIntObj(y1));
        Tcl_ListObjAppendElement(interp, cmdObjPtr, listObjPtr);
        Tcl_IncrRefCount(cmdObjPtr);
        Tcl_Preserve(viewPtr);
        result = Tcl_EvalObjEx(interp, cmdObjPtr, TCL_EVAL_GLOBAL);
        Tcl_Release(viewPtr);
        Tcl_DecrRefCount(cmdObjPtr);
        if (result == TCL_OK) {
            filterPtr->postPtr = colPtr;
        }
    }
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        if (filterPtr->postPtr != NULL) {
            Drawable drawable;
            
            drawable = Tk_WindowId(viewPtr->tkwin);
            DisplayColumnFilter(viewPtr, filterPtr->postPtr, drawable);
        }
    }
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * FilterUnpostOp --
 *
 * Results:
 *      Standard TCL result.
 *
 * Side effects:
 *      Commands may get excecuted; variables may get set; sub-menus may
 *      get posted.
 *
 *  pathName filter unpost
 *
 *---------------------------------------------------------------------------
 */
static int
FilterUnpostOp(ClientData clientData, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    Column *colPtr;
    FilterInfo *filterPtr;
    TableView *viewPtr = clientData;
    Tk_Window tkwin;
    const char *menuName;

    filterPtr = &viewPtr->filter;
    if ((filterPtr->menuObjPtr == NULL) || (filterPtr->postPtr == NULL)) {
        return TCL_OK;
    }
    colPtr = filterPtr->postPtr;
    assert((colPtr->flags & (HIDDEN|DISABLED)) == 0);

    menuName = Tcl_GetString(filterPtr->menuObjPtr);
    tkwin = Tk_NameToWindow(interp, menuName, viewPtr->tkwin);
    if (tkwin == NULL) {
        return TCL_ERROR;
    }
    if (Tk_Parent(tkwin) != viewPtr->tkwin) {
        Tcl_AppendResult(interp, "can't unpost \"", Tk_PathName(tkwin), 
                "\": it isn't a descendant of ", 
                Tk_PathName(viewPtr->tkwin), (char *)NULL);
        return TCL_ERROR;
    }
    Blt_UnmapToplevelWindow(tkwin);
    if (Tk_IsMapped(tkwin)) {
        Tk_UnmapWindow(tkwin);
    }
    filterPtr->postPtr = NULL;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * FilterOp --
 *
 *      Comparison routine (used by qsort) to sort a chain of subnodes.
 *      A simple string comparison is performed on each node name.
 *
 *      pathName filter configure col
 *      pathName filter cget col -recurse root
 *
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec filterOps[] =
{
    {"activate",   1, FilterActivateOp,    4, 4, "col",},
    {"cget",       2, FilterCgetOp,        4, 4, "option",},
    {"configure",  2, FilterConfigureOp,   3, 0, "?option value ...?",},
    {"deactivate", 1, FilterDeactivateOp,  3, 3, "",},
    {"inside",     1, FilterInsideOp,      6, 6, "col x y",},
    {"post",       1, FilterPostOp,        3, 4, "?col?",},
    {"unpost",     1, FilterUnpostOp,      3, 3, "",},
};
static int numFilterOps = sizeof(filterOps) / sizeof(Blt_OpSpec);

/*ARGSUSED*/
static int
FilterOp(ClientData clientData, Tcl_Interp *interp, int objc, 
         Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numFilterOps, filterOps, BLT_OP_ARG2, objc, 
            objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}

/*
 *---------------------------------------------------------------------------
 *
 * FindOp --
 *
 *      Find rows based upon the expression provided.
 *
 * Results:
 *      A standard TCL result.  The interpreter result will contain a list of
 *      the node serial identifiers.
 *
 *      pathName find expr 
 *      
 *---------------------------------------------------------------------------
 */
static int
FindOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    FindSwitches switches;
    TableView *viewPtr = clientData;
    int result;

    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, findSwitches, objc - 3, objv + 3, 
        &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    switches.table = viewPtr->table;
    switches.viewPtr = viewPtr;
    result = FindRows(interp, viewPtr, objv[2], &switches);
    Blt_FreeSwitches(findSwitches, &switches, 0);
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * FocusOp --
 *
 *      pathName focus ?cell?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
FocusOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    TableView *viewPtr = clientData;

    if (objc == 2) {
        Tcl_Obj *listObjPtr;

        listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
        if (viewPtr->focusPtr != NULL) {
            CellKey *keyPtr;
            Column *colPtr;
            Row *rowPtr;
            Tcl_Obj *objPtr;

            keyPtr = GetKey(viewPtr, viewPtr->focusPtr);
            rowPtr = keyPtr->rowPtr;
            colPtr = keyPtr->colPtr;
            objPtr = GetRowIndexObj(viewPtr, rowPtr);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
            objPtr = GetColumnIndexObj(viewPtr, colPtr);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
        }
        Tcl_SetObjResult(interp, listObjPtr);
        return TCL_OK;
    }
    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr != NULL) {
        CellKey *keyPtr;
        Column *colPtr;
        Row *rowPtr;

        keyPtr = GetKey(viewPtr, cellPtr);
        rowPtr = keyPtr->rowPtr;
        colPtr = keyPtr->colPtr;
        if ((rowPtr->flags|colPtr->flags) & (HIDDEN|DISABLED)) {
            return TCL_OK;              /* Can't set focus to hidden or
                                         * disabled cell */
        }
        if (cellPtr != viewPtr->focusPtr) {
            viewPtr->focusPtr = cellPtr;
            EventuallyRedraw(viewPtr);
        }
        Blt_SetFocusItem(viewPtr->bindTable, viewPtr->focusPtr, ITEM_CELL);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * GrabOp --
 *
 *      pathName grab ?cell?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
GrabOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    TableView *viewPtr = clientData;

    if (objc == 2) {
        Tcl_Obj *listObjPtr;

        listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
        if (viewPtr->postPtr != NULL) {
            CellKey *keyPtr;
            Column *colPtr;
            Row *rowPtr;
            Tcl_Obj *objPtr;

            keyPtr = GetKey(viewPtr, viewPtr->postPtr);
            colPtr = keyPtr->colPtr;
            rowPtr = keyPtr->rowPtr;
            objPtr = GetRowIndexObj(viewPtr, rowPtr);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
            objPtr = GetColumnIndexObj(viewPtr, colPtr);
            Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
        }
        Tcl_SetObjResult(interp, listObjPtr);
        return TCL_OK;
    }
    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    Blt_SetCurrentItem(viewPtr->bindTable, viewPtr->postPtr, ITEM_CELL);
    viewPtr->postPtr = cellPtr;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * HighlightOp --
 *
 *      Makes the cell appear highlighted.  The cell is redrawn in its
 *      highlighted foreground and background colors.
 *
 *      pathName highlight cell
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
HighlightOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    TableView *viewPtr = clientData;
    const char *string;

    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    /* If we aren't already queued to redraw the widget, try to directly draw
     * into window. */
    string = Tcl_GetString(objv[1]);
    if (string[0] == 'h') {
        cellPtr->flags |= HIGHLIGHT;
    } else {
        cellPtr->flags &= ~HIGHLIGHT;
    }
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        Drawable drawable;

        drawable = Tk_WindowId(viewPtr->tkwin);
        DisplayCell(viewPtr, cellPtr, drawable, TRUE);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * IdentifyOp --
 *
 *      pathName identify cell x y 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
IdentifyOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    CellStyle *stylePtr;
    Column *colPtr;
    Row *rowPtr;
    TableView *viewPtr = clientData;
    const char *string;
    int x, y, rootX, rootY;

    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    if ((Tcl_GetIntFromObj(interp, objv[3], &x) != TCL_OK) ||
        (Tcl_GetIntFromObj(interp, objv[4], &y) != TCL_OK)) {
        return TCL_ERROR;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    colPtr = keyPtr->colPtr;
    rowPtr = keyPtr->rowPtr;
    /* Convert from root coordinates to window-local coordinates to cell-local
     * coordinates */
    Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
    x -= rootX + SCREENX(viewPtr, colPtr->worldX);
    y -= rootY + SCREENY(viewPtr, rowPtr->worldY);
    string = NULL;
    stylePtr = GetCurrentStyle(viewPtr, rowPtr, colPtr, cellPtr);
    if (stylePtr->classPtr->identProc != NULL) {
        string = (*stylePtr->classPtr->identProc)(viewPtr, cellPtr, stylePtr, 
                                                  x, y);
    }
    if (string != NULL) {
        Tcl_SetStringObj(Tcl_GetObjResult(interp), string, -1);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * IndexOp --
 *
 *      pathName index cell
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
IndexOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    Column *colPtr;
    Row *rowPtr;
    TableView *viewPtr = clientData;
    Tcl_Obj *listObjPtr, *objPtr;

    if ((GetCellFromObj(NULL, viewPtr, objv[2], &cellPtr) != TCL_OK) ||
        (cellPtr == NULL)) {
        return TCL_OK;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    colPtr = keyPtr->colPtr;
    rowPtr = keyPtr->rowPtr;
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    objPtr = GetRowIndexObj(viewPtr, rowPtr);
    Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    objPtr = GetColumnIndexObj(viewPtr, colPtr);
    Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * InsideOp --
 *
 *      pathName inside cell x y
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
InsideOp(ClientData clientData, Tcl_Interp *interp, int objc, 
         Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    TableView *viewPtr = clientData;
    int state;
    int x, y, rootX, rootY;

    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if ((Tcl_GetIntFromObj(interp, objv[3], &x) != TCL_OK) ||
        (Tcl_GetIntFromObj(interp, objv[4], &y) != TCL_OK)) {
        return TCL_ERROR;
    }
    /* Convert from root coordinates to window-local coordinates */
    Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
    x -= rootX, y -= rootY;
    state = FALSE;
    if (cellPtr != NULL) {
        CellKey *keyPtr;
        Column *colPtr;
        Row *rowPtr;

        keyPtr = GetKey(viewPtr, cellPtr);
        colPtr = keyPtr->colPtr;
        rowPtr = keyPtr->rowPtr;
        x = WORLDX(viewPtr, x);
        y = WORLDY(viewPtr, y);
        
        if ((x >= colPtr->worldX) && (x < (colPtr->worldX + colPtr->width)) &&
            (y >= rowPtr->worldY) && (y < (rowPtr->worldY + rowPtr->height))) {
            state = TRUE;
        }
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * InvokeOp --
 *
 *      pathName invoke cell
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
InvokeOp(ClientData clientData, Tcl_Interp *interp, int objc, 
         Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    CellStyle *stylePtr;
    Column *colPtr;
    Row *rowPtr;
    TableView *viewPtr = clientData;

    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    colPtr = keyPtr->colPtr;
    rowPtr = keyPtr->rowPtr;
    stylePtr = GetCurrentStyle(viewPtr, rowPtr, colPtr, cellPtr);
    if (stylePtr->cmdObjPtr != NULL) {
        int result;
        Tcl_Obj *cmdObjPtr, *objPtr;

        cmdObjPtr = Tcl_DuplicateObj(stylePtr->cmdObjPtr);
        objPtr = GetRowIndexObj(viewPtr, rowPtr);
        Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
        objPtr = GetColumnIndexObj(viewPtr, colPtr);
        Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
        Tcl_IncrRefCount(cmdObjPtr);
        Tcl_Preserve(cellPtr);
        result = Tcl_EvalObjEx(interp, cmdObjPtr, TCL_EVAL_GLOBAL);
        Tcl_Release(cellPtr);
        Tcl_DecrRefCount(cmdObjPtr);
        if (result != TCL_OK) {
            return TCL_ERROR;
        }
    } 
    return TCL_OK;
}


/*
 *---------------------------------------------------------------------------
 *
 * IsHiddenOp --
 *
 *      pathName ishidden cell
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
IsHiddenOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    TableView *viewPtr = clientData;
    int state;

    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    state = FALSE;
    if (cellPtr != NULL) {
        CellKey *keyPtr;
        Column *colPtr;
        Row *rowPtr;

        keyPtr = GetKey(viewPtr, cellPtr);
        colPtr = keyPtr->colPtr;
        rowPtr = keyPtr->rowPtr;
        state = ((rowPtr->flags|colPtr->flags) & HIDDEN);
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowActivateOp --
 *
 *      Sets the button to appear active.
 *
 *      pathName row activate row
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowActivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
              Tcl_Obj *const *objv)
{
    Drawable drawable;
    Row *rowPtr, *activePtr;
    TableView *viewPtr = clientData;
    
    if (GetRowFromObj(interp, viewPtr, objv[3], &rowPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (rowPtr == NULL) {
        return TCL_OK;
    }
    if (((viewPtr->rows.flags & SHOW_TITLES) == 0) || 
        (rowPtr->flags & (HIDDEN | DISABLED))) {
        return TCL_OK;                  /* Disabled or hidden row. */
    }
    activePtr = viewPtr->rows.activeTitlePtr;
    viewPtr->rows.activeTitlePtr = rowPtr;
    drawable = Tk_WindowId(viewPtr->tkwin);
    /* If we aren't already queued to redraw the widget, try to directly draw
     * into window. */
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        if (activePtr != NULL) {
            DisplayRowTitle(viewPtr, activePtr, drawable);
        }
        DisplayRowTitle(viewPtr, rowPtr, drawable);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowBindOp --
 *
 *        pathName row bind tag type sequence command
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowBindOp(ClientData clientData, Tcl_Interp *interp, int objc, 
          Tcl_Obj *const *objv)
{
    BindTag tag;
    ItemType type;
    Row *rowPtr;
    TableView *viewPtr = clientData;
    char c;
    const char *string;
    int length;

    string = Tcl_GetStringFromObj(objv[4], &length);
    c = string[0];
    if ((c == 'c') && (strncmp(string, "cell", length) == 0)) {
        type = ITEM_CELL;
    } else if ((c == 't') && (strncmp(string, "title", length) == 0)) {
        type = ITEM_ROW_TITLE;
    } else if ((c == 'r') && (strncmp(string, "resize", length) == 0)) {
        type = ITEM_ROW_RESIZE;
    } else {
        return TCL_ERROR;
    }
    if (GetRowFromObj(NULL, viewPtr, objv[3], &rowPtr) == TCL_OK) {
        if (rowPtr == NULL) {
            return TCL_OK;
        }
        tag = MakeBindTag(viewPtr, rowPtr, type);
    } else {
        tag = MakeStringBindTag(viewPtr, Tcl_GetString(objv[3]), type);
    }
    return Blt_ConfigureBindingsFromObj(interp, viewPtr->bindTable, tag,
        objc - 5, objv + 5);
}

/*
 *---------------------------------------------------------------------------
 *
 * RowCgetOp --
 *
 *---------------------------------------------------------------------------
 */
static int
RowCgetOp(ClientData clientData, Tcl_Interp *interp, int objc, 
          Tcl_Obj *const *objv)
{
    Row *rowPtr;
    TableView *viewPtr = clientData;

    if (GetRowFromObj(interp, viewPtr, objv[3], &rowPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (rowPtr == NULL) {
        return TCL_OK;
    }
    return Blt_ConfigureValueFromObj(interp, viewPtr->tkwin, rowSpecs, 
        (char *)rowPtr, objv[4], 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * RowConfigureOp --
 *
 *      This procedure is called to process a list of configuration
 *      options database, in order to reconfigure the one of more
 *      entries in the widget.
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 * Side effects:
 *      Configuration information, such as text string, colors, font,
 *      etc. get set for viewPtr; old resources get freed, if there
 *      were any.  The hypertext is redisplayed.
 *
 *      pathName row configure row ?option value?
 *
 *---------------------------------------------------------------------------
 */
static int
RowConfigureOp(ClientData clientData, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    Row *rowPtr;
    TableView *viewPtr = clientData;

    cachedObjOption.clientData = viewPtr;
    iconOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    if (GetRowFromObj(interp, viewPtr, objv[3], &rowPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (rowPtr == NULL) {
        return TCL_OK;
    }
    if (objc == 4) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, rowSpecs, 
                (char *)rowPtr, (Tcl_Obj *)NULL, 0);
    } else if (objc == 5) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, rowSpecs, 
                (char *)rowPtr, objv[4], 0);
    }
    if (Blt_ConfigureWidgetFromObj(interp, viewPtr->tkwin, rowSpecs, 
        objc - 4, objv + 4, (char *)rowPtr, BLT_CONFIG_OBJV_ONLY) != TCL_OK) {
        return TCL_ERROR;
    }
    ConfigureRow(viewPtr, rowPtr);

    viewPtr->flags |= LAYOUT_PENDING;
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowDeactivateOp --
 *
 *      Turn off active highlighting for all row titles.
 *
 *      pathName row deactivate
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowDeactivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                Tcl_Obj *const *objv)
{
    Drawable drawable;
    Row *activePtr;
    TableView *viewPtr = clientData;
    
    if ((viewPtr->rows.flags & SHOW_TITLES) == 0) {
        return TCL_OK;                  /* Not displaying row titles. */
    } /*  */
    activePtr = viewPtr->rows.activeTitlePtr;
    viewPtr->rows.activeTitlePtr = NULL;
    drawable = Tk_WindowId(viewPtr->tkwin);

    /* If we aren't already queued to redraw the widget, try to directly draw
     * into window. */
    if ((viewPtr->flags & REDRAW_PENDING) == 0) {
        if (activePtr != NULL) {
            DisplayRowTitle(viewPtr, activePtr, drawable);
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowDeleteOp --
 *
 *      pathName row delete row row row 
 *
 *---------------------------------------------------------------------------
 */
/* ARGSUSED */
static int
RowDeleteOp(ClientData clientData, Tcl_Interp *interp, int objc,
            Tcl_Obj *const *objv)
{
    Blt_Chain chain;
    Blt_ChainLink link;
    TableView *viewPtr = clientData;

    /* Mark all the named columns as deleted. */
    chain = IterateRowsObjv(interp, viewPtr, objc - 3, objv + 3);
    if (chain == NULL) {
        return TCL_ERROR;
    }
    for (link = Blt_Chain_FirstLink(chain); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Row *rowPtr;

        rowPtr = Blt_Chain_GetValue(link);
        DestroyRow(rowPtr);
    }
    Blt_Chain_Destroy(chain);

    /* Requires a new layout. Sort order and individual geometies stay the
     * same. */
    viewPtr->flags |= LAYOUT_PENDING;
    viewPtr->rows.flags |= REINDEX;
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowExistsOp --
 *
 *      pathName row exists row
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowExistsOp(ClientData clientData, Tcl_Interp *interp, int objc,
            Tcl_Obj *const *objv)
{
    Row *rowPtr;
    TableView *viewPtr = clientData;
    int exists;

    exists = FALSE;
    if (GetRowFromObj(NULL, viewPtr, objv[3], &rowPtr) == TCL_OK) {
        exists = (rowPtr != NULL);
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), exists);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowExposeOp --
 *
 *      pathName row expose ?rowName ...?
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowExposeOp(ClientData clientData, Tcl_Interp *interp, int objc,
            Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    if (objc == 3) {
        Row *rowPtr;
        Tcl_Obj *listObjPtr;

        listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
        for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
             rowPtr = rowPtr->nextPtr) {
            if ((rowPtr->flags & HIDDEN) == 0) {
                Tcl_Obj *objPtr;

                objPtr = GetRowIndexObj(viewPtr, rowPtr);
                Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
            }
        }
        Tcl_SetObjResult(interp, listObjPtr);
    } else {
        Blt_Chain chain;
        Blt_ChainLink link;
        int redraw;
        
        chain = IterateRowsObjv(interp, viewPtr, objc - 3, objv + 3);
        if (chain == NULL) {
            return TCL_ERROR;
        }
        redraw = FALSE;
        for (link = Blt_Chain_FirstLink(chain); link != NULL; 
             link = Blt_Chain_NextLink(link)) {
            Row *rowPtr;
            
            rowPtr = Blt_Chain_GetValue(link);
            if (rowPtr->flags & HIDDEN) {
                rowPtr->flags &= ~HIDDEN;
                redraw = TRUE;
            }
        }
        Blt_Chain_Destroy(chain);
        if (redraw) {
            viewPtr->rows.flags |= (SCROLL_PENDING | REINDEX);
            EventuallyRedraw(viewPtr);
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowHideOp --
 *
 *      pathName row hide ?rowName ...?
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowHideOp(ClientData clientData, Tcl_Interp *interp, int objc,
          Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    if (objc == 3) {
        Row *rowPtr;
        Tcl_Obj *listObjPtr;

        listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
        for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
             rowPtr = rowPtr->nextPtr) {
            if (rowPtr->flags & HIDDEN) {
                Tcl_Obj *objPtr;

                objPtr = GetRowIndexObj(viewPtr, rowPtr);
                Tcl_ListObjAppendElement(interp, listObjPtr, objPtr);
            }
        }
        Tcl_SetObjResult(interp, listObjPtr);
    } else {
        Blt_Chain chain;
        Blt_ChainLink link;
        int redraw;
        
        chain = IterateRowsObjv(interp, viewPtr, objc - 3, objv + 3);
        if (chain == NULL) {
            return TCL_ERROR;
        }
        redraw = FALSE;
        for (link = Blt_Chain_FirstLink(chain); link != NULL; 
             link = Blt_Chain_NextLink(link)) {
            Row *rowPtr;
            
            rowPtr = Blt_Chain_GetValue(link);
            if ((rowPtr->flags & HIDDEN) == 0) {
                rowPtr->flags |= HIDDEN;
                redraw = TRUE;
            }
        }
        Blt_Chain_Destroy(chain);
        if (redraw) {
            viewPtr->rows.flags |= (SCROLL_PENDING | REINDEX);
            EventuallyRedraw(viewPtr);
        }
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowIndexOp --
 *
 *      pathName row index row
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowIndexOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    Row *rowPtr;
    TableView *viewPtr = clientData;
    ssize_t index;

    if (GetRowFromObj(interp, viewPtr, objv[3], &rowPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    index = (rowPtr != NULL) ? 
        blt_table_row_index(viewPtr->table, rowPtr->row) : -1;
    Tcl_SetWideIntObj(Tcl_GetObjResult(interp), index);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowInsertOp --
 *
 *      Add new rows to the displayed in the tableview widget.  
 *
 *      pathName row insert row position ?option values?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowInsertOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    BLT_TABLE_ROW row;
    Blt_HashEntry *hPtr;
    CellKey key;
    Column *colPtr;
    Row *rowPtr;
    TableView *viewPtr = clientData;
    int isNew;
    long insertPos;

    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    row = blt_table_get_row(interp, viewPtr->table, objv[3]);
    if (row == NULL) {
        return TCL_ERROR;
    }
    hPtr = Blt_CreateHashEntry(&viewPtr->rows.table, (char *)row, &isNew);
    if (!isNew) {
        Tcl_AppendResult(interp, "a row \"", Tcl_GetString(objv[3]),
                "\" already exists in \"", Tk_PathName(viewPtr->tkwin),
                "\"", (char *)NULL);
        return TCL_ERROR;
    }
    if (Blt_GetPositionFromObj(viewPtr->interp, objv[4], &insertPos) != TCL_OK){
        return TCL_ERROR;
    }
    rowPtr = NewRow(viewPtr, row, hPtr);
    iconOption.clientData = viewPtr;
    cachedObjOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    if (Blt_ConfigureComponentFromObj(viewPtr->interp, viewPtr->tkwin,
        blt_table_row_label(rowPtr->row), "Row", rowSpecs, objc - 4, objv + 4,
        (char *)rowPtr, 0) != TCL_OK) {
        DestroyRow(rowPtr);
        return TCL_ERROR;
    }
    if ((insertPos != -1) && (insertPos < (viewPtr->rows.length - 1))) {
        Row *destPtr;

        destPtr = viewPtr->rows.map[insertPos];
        MoveRows(viewPtr, destPtr, rowPtr, rowPtr, FALSE);
    }
    /* Generate cells for the new row. */
    key.rowPtr = rowPtr;
    for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL;
         colPtr = colPtr->nextPtr) {
        Blt_HashEntry *hPtr;
        int isNew;

        key.colPtr = colPtr;
        hPtr = Blt_CreateHashEntry(&viewPtr->cellTable, (char *)&key, &isNew);
        if (isNew) {
            Cell *cellPtr;

            cellPtr = NewCell(viewPtr, hPtr);
            Blt_SetHashValue(hPtr, cellPtr);
        }
    }
    viewPtr->flags |= LAYOUT_PENDING;
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowInvokeOp --
 *
 *      This procedure is called to invoke a command associated with the
 *      row title.  The title must be not disabled or hidden for the 
 *      command to be executed.
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then interp->result
 *      contains an error message.
 *
 *      pathName row invoke rowName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowInvokeOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    Row *rowPtr;
    TableView *viewPtr = clientData;
    Tcl_Obj *objPtr, *cmdObjPtr;
    int result;

    if (GetRowFromObj(interp, viewPtr, objv[3], &rowPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (rowPtr == NULL) {
        return TCL_OK;
    }
    cmdObjPtr = (rowPtr->cmdObjPtr == NULL) 
        ? viewPtr->rows.cmdObjPtr : rowPtr->cmdObjPtr;
    if (((viewPtr->rows.flags & SHOW_TITLES) == 0) || 
        (rowPtr->flags & (DISABLED|HIDDEN)) || (cmdObjPtr == NULL)) {
        return TCL_OK;
    }
    Tcl_Preserve(viewPtr);
    cmdObjPtr = Tcl_DuplicateObj(cmdObjPtr);  
    objPtr = GetRowIndexObj(viewPtr, rowPtr);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    Tcl_IncrRefCount(cmdObjPtr);
    result = Tcl_EvalObjEx(interp, cmdObjPtr, TCL_EVAL_GLOBAL);
    Tcl_DecrRefCount(cmdObjPtr);
    Tcl_Release(viewPtr);
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowMoveOp --
 *
 *      Move one or more rows.
 *
 *      pathName row move dest first last ?switches?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowMoveOp(ClientData clientData, Tcl_Interp *interp, int objc, 
          Tcl_Obj *const *objv)
{
    Row *destPtr, *firstPtr, *lastPtr;
    TableView *viewPtr = clientData;
    int after = TRUE;

    if (viewPtr->table == NULL) {
        return TCL_OK;
    }
    if (GetRowFromObj(interp, viewPtr, objv[3], &destPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (GetRowFromObj(interp, viewPtr, objv[4], &firstPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (GetRowFromObj(interp, viewPtr, objv[5], &lastPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (viewPtr->rows.flags & REINDEX) {
        RenumberRows(viewPtr);
    }

    /* Check if range is valid. */
    if (firstPtr->index > lastPtr->index) {
        return TCL_OK;                  /* No range. */
    }

    /* Check that destination is outside the range of columns to be moved. */
    if ((destPtr->index >= firstPtr->index) &&
        (destPtr->index <= lastPtr->index)) {
        Tcl_AppendResult(interp, "destination row \"", 
                Tcl_GetString(objv[3]),
                 "\" can't be in the range of rows to be moved", 
                (char *)NULL);
        return TCL_ERROR;
    }
    MoveRows(viewPtr, destPtr, firstPtr, lastPtr, after);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowNamesOp --
 *
 *      pathName row names ?pattern ...?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowNamesOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    Row *rowPtr;
    TableView *viewPtr = clientData;
    Tcl_Obj *listObjPtr;

    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    for (rowPtr = viewPtr->rows.firstPtr; rowPtr != NULL; 
         rowPtr = rowPtr->nextPtr) {
        const char *label;
        int found;
        int i;

        found = TRUE;
        label = blt_table_row_label(rowPtr->row);
        for (i = 3; i < objc; i++) {
            const char *pattern;

            pattern = Tcl_GetString(objv[i]);
            found = Tcl_StringMatch(label, pattern);
            if (found) {
                break;
            }
        }
        if (found) {
            Tcl_ListObjAppendElement(interp, listObjPtr, 
                                     Tcl_NewStringObj(label, -1));
        }
    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowNearestOp --
 *
 *      pathName row nearest y ?-root?
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowNearestOp(ClientData clientData, Tcl_Interp *interp, int objc, 
             Tcl_Obj *const *objv)
{
    Row *rowPtr;
    TableView *viewPtr = clientData;
    int y;                              /* Screen y-coordinate of the test
                                         * point. */
    long index;
    NearestSwitches switches;

    if (Tk_GetPixelsFromObj(interp, viewPtr->tkwin, objv[3], &y) != TCL_OK) {
        return TCL_ERROR;
    } 
    memset(&switches, 0, sizeof(switches));
    if (Blt_ParseSwitches(interp, nearestSwitches, objc - 4, objv + 4, 
        &switches, BLT_SWITCH_DEFAULTS) < 0) {
        return TCL_ERROR;
    }
    if (switches.flags & NEAREST_ROOT) {
        int rootX, rootY;
        
        Tk_GetRootCoords(viewPtr->tkwin, &rootX, &rootY);
        y += rootY;
    }
    rowPtr = NearestRow(viewPtr, y, TRUE);
    index = -1;
    if ((viewPtr->table != NULL) && (rowPtr != NULL)) {
        index = blt_table_row_index(viewPtr->table, rowPtr->row);
    } 
    Tcl_SetWideIntObj(Tcl_GetObjResult(interp), index);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * UpdateRowMark --
 *
 *---------------------------------------------------------------------------
 */
static void
UpdateRowMark(TableView *viewPtr, int newMark)
{
    Row *rowPtr;
    int dy;
    int height;

    if (viewPtr->rows.resizePtr == NULL) {
        return;                         /* No row being resized. */
    }
    rowPtr = viewPtr->rows.resizePtr;
    dy = newMark - viewPtr->rows.resizeAnchor; 
    height = rowPtr->height;
    if ((rowPtr->reqHeight.min > 0) && ((height+dy) < rowPtr->reqHeight.min)) {
        dy = rowPtr->reqHeight.min - height;
    }
    if ((rowPtr->reqHeight.max > 0) && ((height+dy) > rowPtr->reqHeight.max)) {
        dy = rowPtr->reqHeight.max - height;
    }
    if ((height + dy) < 4) {
        dy = 4 - height;
    }
    viewPtr->rows.resizeMark = viewPtr->rows.resizeAnchor + dy;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowResizeActivateOp --
 *
 *      Turns on/off the resize cursor.
 *
 *      pathName row resize activate row 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowResizeActivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                    Tcl_Obj *const *objv)
{
    Row *rowPtr;
    TableView *viewPtr = clientData;

    if (GetRowFromObj(interp, viewPtr, objv[4], &rowPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if ((rowPtr == NULL) || (rowPtr->flags & (HIDDEN|DISABLED))){
        return TCL_OK;
    }
    if (viewPtr->rows.resizeCursor != None) {
        Tk_DefineCursor(viewPtr->tkwin, viewPtr->rows.resizeCursor);
    } 
    viewPtr->rows.resizePtr = rowPtr;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowResizeAnchorOp --
 *
 *      Set the anchor for the resize.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowResizeAnchorOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                  Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    if (objc == 5) { 
        int y;

        if (Tcl_GetIntFromObj(NULL, objv[4], &y) != TCL_OK) {
            return TCL_ERROR;
        } 
        viewPtr->rows.resizeAnchor = y;
        UpdateRowMark(viewPtr, y);
    }
    Tcl_SetIntObj(Tcl_GetObjResult(interp), viewPtr->rows.resizeAnchor);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowResizeDeactiveOp --
 *
 *      Turns off the resize cursor.
 *
 *      pathName row resize deactivate 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowResizeDeactivateOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                      Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    Tk_UndefineCursor(viewPtr->tkwin);
    viewPtr->rows.resizePtr = NULL;
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * RowResizeMarkOp --
 *
 *      Sets the resize mark.  The distance between the mark and the anchor
 *      is the delta to change the width of the active row.
 *
 *      pathName row resize mark x 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowResizeMarkOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    if (objc == 5) {
        Row *rowPtr;
        int y;

        if (Tcl_GetIntFromObj(NULL, objv[4], &y) != TCL_OK) {
            return TCL_ERROR;
        } 
        UpdateRowMark(viewPtr, y);
        rowPtr = viewPtr->rows.resizePtr;
        if (rowPtr != NULL) {
            int dy;
            
            dy = (viewPtr->rows.resizeMark - viewPtr->rows.resizeAnchor);
            rowPtr->reqHeight.nom = rowPtr->height + dy;
            rowPtr->reqHeight.flags |= LIMITS_SET_NOM;
            viewPtr->rows.resizeAnchor = viewPtr->rows.resizeMark;
            viewPtr->flags |= LAYOUT_PENDING;
            EventuallyRedraw(viewPtr);
        }
    }
    Tcl_SetIntObj(Tcl_GetObjResult(interp), viewPtr->rows.resizeMark);
    return TCL_OK;
}

static Blt_OpSpec rowResizeOps[] =
{ 
    {"activate",   2, RowResizeActivateOp,   5, 5, "row"},
    {"anchor",     2, RowResizeAnchorOp,     4, 5, "?y?"},
    {"deactivate", 1, RowResizeDeactivateOp, 4, 4, ""},
    {"mark",       1, RowResizeMarkOp,       4, 5, "?y?"},
};

static int numRowResizeOps = sizeof(rowResizeOps) / sizeof(Blt_OpSpec);

/*
 *---------------------------------------------------------------------------
 *
 * RowResizeOp --
 *
 *---------------------------------------------------------------------------
 */
static int
RowResizeOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numRowResizeOps, rowResizeOps, BLT_OP_ARG3, 
        objc, objv,0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}

/*
 *---------------------------------------------------------------------------
 *
 * RowSeeOp --
 *
 *      Implements the quick scan.
 *
 *      pathName row see row
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
RowSeeOp(ClientData clientData, Tcl_Interp *interp, int objc, 
         Tcl_Obj *const *objv)
{
    Row *rowPtr;
    TableView *viewPtr = clientData;
    long yOffset;

    if (GetRowFromObj(interp, viewPtr, objv[3], &rowPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (rowPtr == NULL) {
        return TCL_OK;
    }
    yOffset = GetRowYOffset(viewPtr, rowPtr);
    if (yOffset != viewPtr->rows.scrollOffset) {
        viewPtr->rows.scrollOffset = yOffset;
        viewPtr->rows.flags |= SCROLL_PENDING;
        EventuallyRedraw(viewPtr);
    }
    return TCL_OK;
}

static Blt_OpSpec rowOps[] =
{
    {"activate",   1, RowActivateOp,   4, 4, "rowName",},
    {"bind",       1, RowBindOp,       5, 7, "tagName type ?sequence command?",},
    {"cget",       2, RowCgetOp,       5, 5, "rowName option",},
    {"configure",  2, RowConfigureOp,  4, 0, "rowName ?option value ...?",},
    {"deactivate", 3, RowDeactivateOp, 3, 3, "",},
    {"delete",     3, RowDeleteOp,     4, 0, "rowName...",},
    {"exists",     3, RowExistsOp,     4, 4, "rowName",},
    {"expose",     3, RowExposeOp,     3, 0, "?rowName ...?",},
    {"hide",       1, RowHideOp,       3, 0, "?rowName ...?",},
    {"index",      3, RowIndexOp,      4, 4, "rowName",},
    {"insert",     3, RowInsertOp,     5, 0, "rowName position ?option value ...?",},
    {"invoke",     3, RowInvokeOp,     4, 4, "rowName",},
    {"move",       1, RowMoveOp,       6, 0, "destCol firstCol lastCol ?switches?",},  
    {"names",      2, RowNamesOp,      3, 0, "?pattern ...?",},
    {"nearest",    2, RowNearestOp,    4, 0, "y ?switches ...?",},
    {"resize",     1, RowResizeOp,     3, 0, "args",},
    {"see",        2, RowSeeOp,        4, 4, "rowName",},
    {"show",       2, RowExposeOp,     3, 0, "?rowName ...?",},
};
static int numRowOps = sizeof(rowOps) / sizeof(Blt_OpSpec);

/*
 *---------------------------------------------------------------------------
 *
 * RowOp --
 *
 *---------------------------------------------------------------------------
 */
static int
RowOp(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numRowOps, rowOps, BLT_OP_ARG2, 
        objc, objv,0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}

/*
 *---------------------------------------------------------------------------
 *
 * ScanOp --
 *
 *      Implements the quick scan.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
ScanOp(TableView *viewPtr, Tcl_Interp *interp, int objc, Tcl_Obj *const *objv)
{
    Tk_Window tkwin;
    char *string;
    char c;
    int length;
    int oper;
    int x, y;

#define SCAN_MARK       1
#define SCAN_DRAGTO     2
    string = Tcl_GetStringFromObj(objv[2], &length);
    c = string[0];
    tkwin = viewPtr->tkwin;
    if ((c == 'm') && (strncmp(string, "mark", length) == 0)) {
        oper = SCAN_MARK;
    } else if ((c == 'd') && (strncmp(string, "dragto", length) == 0)) {
        oper = SCAN_DRAGTO;
    } else {
        Tcl_AppendResult(interp, "bad scan operation \"", string,
            "\": should be either \"mark\" or \"dragto\"", (char *)NULL);
        return TCL_ERROR;
    }
    if ((Blt_GetPixelsFromObj(interp, tkwin, objv[3], PIXELS_ANY, &x) 
         != TCL_OK) ||
        (Blt_GetPixelsFromObj(interp, tkwin, objv[4], PIXELS_ANY, &y) 
         != TCL_OK)) {
        return TCL_ERROR;
    }
    if (oper == SCAN_MARK) {
        viewPtr->scanAnchorX = x;
        viewPtr->scanAnchorY = y;
        viewPtr->scanX = viewPtr->columns.scrollOffset;
        viewPtr->scanY = viewPtr->rows.scrollOffset;
    } else {
        int worldX, worldY;
        int dx, dy;

        dx = viewPtr->scanAnchorX - x;
        dy = viewPtr->scanAnchorY - y;
        worldX = viewPtr->scanX + (10 * dx);
        worldY = viewPtr->scanY + (10 * dy);

        if (worldX < 0) {
            worldX = 0;
        } else if (worldX >= viewPtr->worldWidth) {
            worldX = viewPtr->worldWidth - viewPtr->columns.scrollUnits;
        }
        if (worldY < 0) {
            worldY = 0;
        } else if (worldY >= viewPtr->worldHeight) {
            worldY = viewPtr->worldHeight - viewPtr->rows.scrollUnits;
        }
        viewPtr->columns.scrollOffset = worldX;
        viewPtr->rows.scrollOffset = worldY;
        viewPtr->rows.flags |= SCROLL_PENDING;
        EventuallyRedraw(viewPtr);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SeeOp --
 *
 *      Changes to view to encompass the specified cell.
 *
 *      pathName see cell
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
SeeOp(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    TableView *viewPtr = clientData;
    long xOffset, yOffset;

    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    yOffset = GetRowYOffset(viewPtr, keyPtr->rowPtr);
    xOffset = GetColumnXOffset(viewPtr, keyPtr->colPtr);
    if (xOffset != viewPtr->columns.scrollOffset) {
        viewPtr->columns.scrollOffset = xOffset;
        viewPtr->columns.flags |= SCROLL_PENDING;
    }
    if (yOffset != viewPtr->rows.scrollOffset) {
        viewPtr->rows.scrollOffset = yOffset;
        viewPtr->rows.flags |= SCROLL_PENDING;
    }
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SelectionAnchorOp --
 *
 *      Sets the selection anchor to the element given by a index.  The
 *      selection anchor is the end of the selection that is fixed while
 *      dragging out a selection with the mouse.  The index "anchor" may be
 *      used to refer to the anchor element.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The selection changes.
 *
 *      pathName selection anchor cell
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
SelectionAnchorOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                  Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    TableView *viewPtr = clientData;

    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    if (viewPtr->selectMode == SELECT_CELLS) {
        CellSelection *selPtr;

        selPtr = &viewPtr->selectCells;
        /* Set both the selection anchor and the mark. This indicates that a
         * single cell is selected. */
        selPtr->markPtr = selPtr->anchorPtr = keyPtr;
    } else {
        RowSelection *selectPtr;

        selectPtr = &viewPtr->rows.selection;
        /* Set both the anchor and the mark. Indicates that a single row is
         * selected. */
        selectPtr->anchorPtr = selectPtr->markPtr = keyPtr->rowPtr;
        SelectRow(viewPtr, keyPtr->rowPtr);
    }
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SelectionClearallOp
 *
 *      Clears the entire selection.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The selection changes.
 *
 *      pathName selection clearall
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
SelectionClearallOp(ClientData clientData, Tcl_Interp *interp, int objc,
                    Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    ClearSelections(viewPtr);
    EventuallyRedraw(viewPtr);
    if (viewPtr->selectCmdObjPtr != NULL) {
        EventuallyInvokeSelectCommand(viewPtr);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SelectionExportOp
 *
 *      Exports the current selection.  It is not an error if not selection
 *      is present.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The selection is exported.
 *
 *      pathName selection export
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
SelectionExportOp(ClientData clientData, Tcl_Interp *interp, int objc,
                  Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    int state;

    if (viewPtr->selectMode == SELECT_CELLS) {
        state = (viewPtr->selectCells.cellTable.numEntries > 0);
    } else {
        state = (Blt_Chain_GetLength(viewPtr->rows.selection.list) > 0);
    }
    if (state) {
        Tk_OwnSelection(viewPtr->tkwin, XA_PRIMARY, LostSelection, viewPtr);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SelectionIncludesOp
 *
 *      Returns 1 if the element indicated by index is currently
 *      selected, 0 if it isn't.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The selection changes.
 *
 *      pathName selection includes cell
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
SelectionIncludesOp(ClientData clientData, Tcl_Interp *interp, int objc,
                    Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    Column *colPtr;
    Row *rowPtr;
    TableView *viewPtr = clientData;
    int state;

    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        Tcl_SetBooleanObj(Tcl_GetObjResult(interp), FALSE);
        return TCL_OK;
    }
    state = FALSE;
    keyPtr = GetKey(viewPtr, cellPtr);
    colPtr = keyPtr->colPtr;
    rowPtr = keyPtr->rowPtr;
    if (viewPtr->selectMode == SELECT_CELLS) {
        if (((rowPtr->flags|colPtr->flags) & (HIDDEN | DISABLED)) == 0) {
            Blt_HashEntry *hPtr;

            hPtr = Blt_FindHashEntry(&viewPtr->selectCells.cellTable, keyPtr);
            if (hPtr != NULL) {
                state = TRUE;
            }
        }
    } else {
        if ((rowPtr->flags & (HIDDEN | DISABLED)) == 0) {
            state = rowPtr->flags & SELECTED;
        }
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SelectionMarkOp --
 *
 *      Sets the selection mark to the element given by a index.  The
 *      selection anchor is the end of the selection that is movable while
 *      dragging out a selection with the mouse.  The index "mark" may be used
 *      to refer to the anchor element.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The selection changes.
 *
 *      pathName selection mark cell
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
SelectionMarkOp(ClientData clientData, Tcl_Interp *interp, int objc,
                Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    TableView *viewPtr = clientData;

    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    if (viewPtr->selectMode == SELECT_CELLS) {
        CellSelection *selPtr = &viewPtr->selectCells;
        
        if (selPtr->anchorPtr == NULL) {
            fprintf(stderr, "Attempting to set mark before anchor. Cell selection anchor must be set first\n");
            return TCL_OK;
        }
        selPtr->markPtr = GetKey(viewPtr, cellPtr);
        selPtr->flags &= ~SELECT_MASK;
        selPtr->flags |= SELECT_SET;
    } else {
        CellKey *keyPtr;
        Row *rowPtr;
        RowSelection *selectPtr;
        
        selectPtr = &viewPtr->rows.selection;
        if (selectPtr->anchorPtr == NULL) {
            Tcl_AppendResult(interp, "row selection anchor must be set first", 
                             (char *)NULL);
            return TCL_ERROR;
        }
        keyPtr = GetKey(viewPtr, cellPtr);
        rowPtr = keyPtr->rowPtr;
        if (selectPtr->markPtr != rowPtr) {
            Blt_ChainLink link, next;

            /* Deselect rows from the list all the way back to the anchor. */
            for (link = Blt_Chain_LastLink(selectPtr->list); link != NULL; 
                link = next) {
                Row *selRowPtr;

                next = Blt_Chain_PrevLink(link);
                selRowPtr = Blt_Chain_GetValue(link);
                if (selRowPtr == selectPtr->anchorPtr) {
                    break;
                }
                DeselectRow(viewPtr, selRowPtr);
            }
            selectPtr->flags &= ~SELECT_MASK;
            selectPtr->flags |= SELECT_SET;
            SelectRows(viewPtr, selectPtr->anchorPtr, rowPtr);
            selectPtr->markPtr = rowPtr;
        }
    }
    EventuallyRedraw(viewPtr);
    if (viewPtr->selectCmdObjPtr != NULL) {
        EventuallyInvokeSelectCommand(viewPtr);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SelectionPresentOp
 *
 *      Returns 1 if there is a selection and 0 if it isn't.
 *
 * Results:
 *      A standard TCL result.  interp->result will contain a boolean string
 *      indicating if there is a selection.
 *
 *      pathName selection present 
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
SelectionPresentOp(ClientData clientData, Tcl_Interp *interp, int objc,
                   Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    int state;

    if (viewPtr->selectMode == SELECT_CELLS) {
        state = (viewPtr->selectCells.cellTable.numEntries > 0);
    } else {
        state = (Blt_Chain_GetLength(viewPtr->rows.selection.list) > 0);
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SelectionSetOp
 *
 *      Selects, deselects, or toggles all of the elements in the range
 *      between first and last, inclusive, without affecting the selection
 *      state of elements outside that range.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The selection changes.
 *
 *      pathName selection set cell cell
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
SelectionSetOp(ClientData clientData, Tcl_Interp *interp, int objc, 
               Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *anchorPtr, *markPtr;
    TableView *viewPtr = clientData;

    if (viewPtr->flags & (GEOMETRY | LAYOUT_PENDING)) {
        /*
         * The layout is dirty.  Recompute it now so that we can use
         * view.top and view.bottom for nodes.
         */
        ComputeGeometry(viewPtr);
    }
    if (GetCellFromObj(NULL, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        /* Silently ignore invalid cell selections. This is to prevent
         * errors when the table is empty. */
        return TCL_OK;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    anchorPtr = GetKey(viewPtr, cellPtr);
    if ((anchorPtr->rowPtr->flags|anchorPtr->colPtr->flags) & HIDDEN) {
        Tcl_AppendResult(interp, "can't select hidden anchor",
                         (char *)NULL);
        return TCL_ERROR;
    }
    if (GetCellFromObj(NULL, viewPtr, objv[4], &cellPtr) != TCL_OK) {
        /* Silently ignore invalid cell selections. This is to prevent
         * errors when the table is empty. */
        return TCL_OK;
    }
    markPtr = GetKey(viewPtr, cellPtr);
    if ((markPtr->rowPtr->flags|markPtr->colPtr->flags) & HIDDEN) {
        Tcl_AppendResult(interp, "can't select hidden mark", (char *)NULL);
        return TCL_ERROR;
    }
    if (viewPtr->selectMode == SELECT_CELLS) {
        CellSelection *selPtr = &viewPtr->selectCells;
        const char *string;

        selPtr->anchorPtr = anchorPtr;
        selPtr->markPtr = markPtr;
        selPtr->flags &= ~SELECT_MASK;
        string = Tcl_GetString(objv[2]);
        switch (string[0]) {
        case 's':
            selPtr->flags |= SELECT_SET;     break;
        case 'c':
            selPtr->flags |= SELECT_CLEAR;   break;
        case 't':
            selPtr->flags |= SELECT_TOGGLE;   break;
        }
        if ((anchorPtr != NULL) && (markPtr != NULL)) {
            AddSelectionRange(viewPtr);
        }
    } else {
        RowSelection *selectPtr = &viewPtr->rows.selection;
        const char *string;

        selectPtr->flags &= ~SELECT_MASK;
        string = Tcl_GetString(objv[2]);
        switch (string[0]) {
        case 's':
            selectPtr->flags |= SELECT_SET;     break;
        case 'c':
            selectPtr->flags |= SELECT_CLEAR;   break;
        case 't':
            selectPtr->flags |= SELECT_TOGGLE;   break;
        }
        SelectRows(viewPtr, anchorPtr->rowPtr, markPtr->rowPtr);
        selectPtr->flags &= ~SELECT_MASK;
        selectPtr->anchorPtr = anchorPtr->rowPtr;
        selectPtr->markPtr = markPtr->rowPtr;
    }
    if (viewPtr->flags & SELECT_EXPORT) {
        Tk_OwnSelection(viewPtr->tkwin, XA_PRIMARY, LostSelection, viewPtr);
    }
    EventuallyRedraw(viewPtr);
    if (viewPtr->selectCmdObjPtr != NULL) {
        EventuallyInvokeSelectCommand(viewPtr);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SelectionOp --
 *
 *      This procedure handles the individual options for text selections.
 *      The selected text is designated by start and end indices into the text
 *      pool.  The selected segment has both a anchored and unanchored ends.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The selection changes.
 *
 *      pathName selection op args
 *
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec selectionOps[] =
{
    {"anchor",   1, SelectionAnchorOp,   4, 4, "cellName",},
    {"clear",    5, SelectionSetOp,      5, 5, "anchorCell markCell",},
    {"clearall", 6, SelectionClearallOp, 3, 3, "",},
    {"export",   1, SelectionExportOp,   3, 3, "",},
    {"includes", 1, SelectionIncludesOp, 4, 4, "cellName",},
    {"mark",     1, SelectionMarkOp,     4, 4, "cellName",},
    {"present",  1, SelectionPresentOp,  3, 3, "",},
    {"set",      1, SelectionSetOp,      5, 5, "anchorCell markCell",},
    {"toggle",   1, SelectionSetOp,      5, 5, "anchorCell markCell",},
};
static int numSelectionOps = sizeof(selectionOps) / sizeof(Blt_OpSpec);

static int
SelectionOp(ClientData clientData, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numSelectionOps, selectionOps, BLT_OP_ARG2, 
        objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (viewPtr, interp, objc, objv);
}

static int
SortAutoOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    SortInfo *sortPtr;
    TableView *viewPtr = clientData;

    sortPtr = &viewPtr->sort;
    if (objc == 4) {
        int state;
        int isAuto;

        isAuto = ((sortPtr->flags & SORT_ALWAYS) != 0);
        if (Tcl_GetBooleanFromObj(interp, objv[3], &state) != TCL_OK) {
            return TCL_ERROR;
        }
        if (isAuto != state) {
            viewPtr->flags |= LAYOUT_PENDING;
            EventuallyRedraw(viewPtr);
        }
        if (state) {
            sortPtr->flags |= SORT_ALWAYS;
        } else {
            sortPtr->flags &= ~SORT_ALWAYS;
        }
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), sortPtr->flags & SORT_ALWAYS);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SortCgetOp --
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
SortCgetOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    return Blt_ConfigureValueFromObj(interp, viewPtr->tkwin, sortSpecs, 
        (char *)viewPtr, objv[3], 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * SortConfigureOp --
 *
 *      This procedure is called to process a list of configuration
 *      options database, in order to reconfigure the one of more
 *      entries in the widget.
 *
 *        pathName sort configure option value
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then
 *      interp->result contains an error message.
 *
 *---------------------------------------------------------------------------
 */
static int
SortConfigureOp(ClientData clientData, Tcl_Interp *interp, int objc, 
                Tcl_Obj *const *objv)
{
    SortInfo *sortPtr;
    TableView *viewPtr = clientData;

    if (objc == 3) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, sortSpecs, 
                (char *)viewPtr, (Tcl_Obj *)NULL, 0);
    } else if (objc == 4) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, sortSpecs, 
                (char *)viewPtr, objv[3], 0);
    }
    sortPtr = &viewPtr->sort;
    if (Blt_ConfigureWidgetFromObj(interp, viewPtr->tkwin, sortSpecs, 
        objc - 3, objv + 3, (char *)viewPtr, BLT_CONFIG_OBJV_ONLY) != TCL_OK) {
        return TCL_ERROR;
    }
    sortPtr->flags &= ~SORTED;
    viewPtr->flags |= LAYOUT_PENDING;
    if (sortPtr->flags & SORT_ALWAYS) {
        sortPtr->flags |= SORT_PENDING;
    }
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*ARGSUSED*/
static int
SortOnceOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;

    viewPtr->flags |= LAYOUT_PENDING;
    viewPtr->sort.flags &= ~SORTED;
    viewPtr->sort.flags |= SORT_PENDING;
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * SortOp --
 *
 *      Comparison routine (used by qsort) to sort a chain of subnodes.
 *      A simple string comparison is performed on each node name.
 *
 *      pathName sort auto
 *      pathName sort once -recurse root
 *
 * Results:
 *      1 is the first is greater, -1 is the second is greater, 0
 *      if equal.
 *
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec sortOps[] =
{
    {"auto",      1, SortAutoOp,      3, 4, "?boolean?",},
    {"cget",      2, SortCgetOp,      4, 4, "option",},
    {"configure", 2, SortConfigureOp, 3, 0, "?option value ...?",},
    {"once",      1, SortOnceOp,      3, 3, "",},
};
static int numSortOps = sizeof(sortOps) / sizeof(Blt_OpSpec);

/*ARGSUSED*/
static int
SortOp(ClientData clientData, Tcl_Interp *interp, int objc, 
       Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numSortOps, sortOps, BLT_OP_ARG2, objc, 
            objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}

/*
 *---------------------------------------------------------------------------
 *
 * StyleApplyOp --
 *
 *        pathName style apply styleName cellName ...
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StyleApplyOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
             Tcl_Obj *const *objv)
{
    Blt_Chain cells;
    Blt_ChainLink link;
    CellStyle *stylePtr;

    if (GetStyle(interp, viewPtr, objv[3], &stylePtr)  != TCL_OK) {
        return TCL_ERROR;
    }
    cells = IterateCellsObjv(interp, viewPtr, objc - 4, objv + 4);
    for (link = Blt_Chain_FirstLink(cells); link != NULL;
         link = Blt_Chain_NextLink(link)) {
        Cell *cellPtr;

        cellPtr = Blt_Chain_GetValue(link);
        if (cellPtr->stylePtr != stylePtr) {
            CellKey *keyPtr;
            int isNew;

            keyPtr = GetKey(viewPtr, cellPtr);
            if (cellPtr->stylePtr != NULL) {
                Blt_HashEntry *hPtr;

                /* Remove the cell from old style's table of cells. */
                hPtr = Blt_FindHashEntry(&stylePtr->table, (char *)keyPtr);
                if (hPtr != NULL) {
                    Blt_DeleteHashEntry(&stylePtr->table, hPtr);
                }
                cellPtr->stylePtr->refCount--;
                if (cellPtr->stylePtr->refCount <= 0) {
                    (*cellPtr->stylePtr->classPtr->freeProc)(cellPtr->stylePtr);
                }
            }
            stylePtr->refCount++;       
            cellPtr->stylePtr = stylePtr;
            Blt_CreateHashEntry(&stylePtr->table, (char *)keyPtr, &isNew);
            cellPtr->flags |= GEOMETRY; /* Assume that the new style
                                         * changes the geometry of the
                                         * cell. */
            viewPtr->flags |= (LAYOUT_PENDING | GEOMETRY);
            EventuallyRedraw(viewPtr);
        }
    }
    Blt_Chain_Destroy(cells);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StyleCgetOp --
 *
 *        pathName style cget "styleName" -background
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StyleCgetOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    CellStyle *stylePtr;

    if (GetStyle(interp, viewPtr, objv[3], &stylePtr)  != TCL_OK) {
        return TCL_ERROR;
    }
    iconOption.clientData = viewPtr;
    return Blt_ConfigureValueFromObj(interp, viewPtr->tkwin, 
        stylePtr->classPtr->specs, (char *)stylePtr, objv[4], 0);
}

/*
 *---------------------------------------------------------------------------
 *
 * StyleConfigureOp --
 *
 *      This procedure is called to process a list of configuration options
 *      database, in order to reconfigure a style.
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then interp->result
 *      contains an error message.
 *
 * Side effects:
 *      Configuration information, such as text string, colors, font, etc. get
 *      set for stylePtr; old resources get freed, if there were any.
 *
 *      pathName style configure styleName ?option value?..
 *
 *---------------------------------------------------------------------------
 */
static int
StyleConfigureOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
                 Tcl_Obj *const *objv)
{
    CellStyle *stylePtr;

    if (GetStyle(interp, viewPtr, objv[3], &stylePtr)  != TCL_OK) {
        return TCL_ERROR;
    }
    iconOption.clientData = viewPtr;
    if (objc == 4) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, 
            stylePtr->classPtr->specs, (char *)stylePtr, (Tcl_Obj *)NULL, 0);
    } else if (objc == 5) {
        return Blt_ConfigureInfoFromObj(interp, viewPtr->tkwin, 
                stylePtr->classPtr->specs, (char *)stylePtr, objv[5], 0);
    }
    if (Blt_ConfigureWidgetFromObj(interp, viewPtr->tkwin, 
        stylePtr->classPtr->specs, objc - 4, objv + 4, (char *)stylePtr, 
        BLT_CONFIG_OBJV_ONLY) != TCL_OK) {
        return TCL_ERROR;
    }
    (*stylePtr->classPtr->configProc)(viewPtr, stylePtr);
    viewPtr->flags |= (LAYOUT_PENDING | GEOMETRY);
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StyleCreateOp --
 *
 *        pathName style create combobox "styleName" -background blue
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StyleCreateOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
              Tcl_Obj *const *objv)
{
    CellStyle *stylePtr;
    char c;
    const char *string;
    int type, length;

    string = Tcl_GetStringFromObj(objv[3], &length);
    c = string[0];
    if ((c == 't') && (strncmp(string, "textbox", length) == 0)) {
        type = STYLE_TEXTBOX;
    } else if ((c == 'c') && (length > 2) && 
               (strncmp(string, "checkbox", length) == 0)) {
        type = STYLE_CHECKBOX;
    } else if ((c == 'c') && (length > 2) && 
               (strncmp(string, "combobox", length) == 0)) {
        type = STYLE_COMBOBOX;
    } else if ((c == 'i') && (strncmp(string, "imagebox", length) == 0)) {
        type = STYLE_IMAGEBOX;
    } else if ((c == 'p') && (strncmp(string, "pushbutton", length) == 0)) {
        type = STYLE_PUSHBUTTON;
    } else {
        Tcl_AppendResult(interp, "unknown style type \"", string, 
                "\": should be textbox, checkbox, combobox, or imagebox", 
                (char *)NULL);
        return TCL_ERROR;
    }
    string = Tcl_GetString(objv[4]);
    stylePtr = Blt_TableView_CreateCellStyle(interp, viewPtr, type, string);
    if (stylePtr == NULL) {
        return TCL_ERROR;
    }
    iconOption.clientData = viewPtr;
    if (Blt_ConfigureComponentFromObj(interp, viewPtr->tkwin, stylePtr->name, 
        stylePtr->classPtr->className, stylePtr->classPtr->specs, objc - 5, 
        objv + 5, (char *)stylePtr, 0) != TCL_OK) {
        (*stylePtr->classPtr->freeProc)(stylePtr);
        return TCL_ERROR;
    }
    (*stylePtr->classPtr->configProc)(viewPtr, stylePtr);
    Tcl_SetObjResult(interp, objv[4]);
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StyleDeleteOp --
 *
 *      Eliminates one or more style names.  A style still may be in use
 *      after its name has been officially removed.  Only its hash table
 *      entry is removed.  The style itself remains until its reference
 *      count returns to zero (i.e. no one else is using it).
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then interp->result
 *      contains an error message.
 *
 *      pathName style delete ?styleName ...?
 *
 *---------------------------------------------------------------------------
 */
static int
StyleDeleteOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
              Tcl_Obj *const *objv)
{
    int i;

    for (i = 3; i < objc; i++) {
        CellStyle *stylePtr;

        if (GetStyle(interp, viewPtr, objv[i], &stylePtr) != TCL_OK) {
            return TCL_ERROR;
        }
        if (stylePtr == viewPtr->stylePtr) {
            continue;                   /* Can't delete fallback style. */
        }
        /* 
         * Removing the style from the hash tables frees up the style name
         * again.  The style itself may not be removed until it's been
         * released by everything using it.
         */
        if (stylePtr->hashPtr != NULL) {
            Blt_DeleteHashEntry(&viewPtr->styleTable, stylePtr->hashPtr);
            stylePtr->hashPtr = NULL;
            stylePtr->name = NULL;      /* Name points to hash key. */
        } 
        stylePtr->refCount--;
        if (stylePtr->refCount <= 0) {
            (*stylePtr->classPtr->freeProc)(stylePtr);
        }
    }
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StyleExistsOp --
 *
 *      pathName style exists styleName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StyleExistsOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
             Tcl_Obj *const *objv)
{
    Blt_HashEntry *hPtr;
    int exists;
    const char *name;

    name = Tcl_GetString(objv[3]);
    hPtr = Blt_FindHashEntry(&viewPtr->styleTable, name);
    exists = (hPtr != NULL);
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), exists);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StyleGetOp --
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then interp->result
 *      contains an error message.
 *
 *      pathName style get cellName
 *
 *---------------------------------------------------------------------------
 */
static int
StyleGetOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    CellStyle *stylePtr;
    Column *colPtr;
    Row *rowPtr;

    if (GetCellFromObj(interp, viewPtr, objv[3], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    colPtr = keyPtr->colPtr;
    rowPtr = keyPtr->rowPtr;
    stylePtr = GetCurrentStyle(viewPtr, rowPtr, colPtr, cellPtr);
    if (stylePtr->name != NULL) {
        Tcl_SetStringObj(Tcl_GetObjResult(interp), stylePtr->name, -1);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StyleNamesOp --
 *
 *      Lists the names of all the current styles in the tableview widget.
 *
 *        pathName style names ?pattern ...?
 *
 * Results:
 *      Always TCL_OK.
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StyleNamesOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
             Tcl_Obj *const *objv)
{
    Blt_HashEntry *hPtr;
    Blt_HashSearch iter;
    Tcl_Obj *listObjPtr;
    
    listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    for (hPtr = Blt_FirstHashEntry(&viewPtr->styleTable, &iter); 
         hPtr != NULL; hPtr = Blt_NextHashEntry(&iter)) {
        CellStyle *stylePtr;
        int found;
        int i;

        found = TRUE;
        stylePtr = Blt_GetHashValue(hPtr);
        for (i = 3; i < objc; i++) {
            const char *pattern;

            pattern = Tcl_GetString(objv[i]);
            found = Tcl_StringMatch(stylePtr->name, pattern);
            if (found) {
                break;
            }
        }
        if (found) {
            Tcl_ListObjAppendElement(interp, listObjPtr,
                Tcl_NewStringObj(stylePtr->name, -1));
        }
    }
    Tcl_SetObjResult(interp, listObjPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StyleTypeOp --
 *
 *        pathName style type styleName
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
StyleTypeOp(TableView *viewPtr, Tcl_Interp *interp, int objc, 
            Tcl_Obj *const *objv)
{
    CellStyle *stylePtr;

    if (GetStyle(interp, viewPtr, objv[3], &stylePtr) != TCL_OK) {
        return TCL_ERROR;
    }
    Tcl_SetStringObj(Tcl_GetObjResult(interp), stylePtr->classPtr->type, -1);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * StyleOp --
 *
 *      pathName style apply 
 *      pathName style cget styleName -foreground
 *      pathName style configure styleName -fg blue -bg green
 *      pathName style create type styleName ?options?
 *      pathName style delete styleName
 *      pathName style get cellName
 *      pathName style names ?pattern ...?
 *      pathName style type styleName 
 *
 *---------------------------------------------------------------------------
 */
static Blt_OpSpec styleOps[] = {
    {"apply",     1, StyleApplyOp,     4, 0, "styleName cellName...",},
    {"cget",      2, StyleCgetOp,      5, 5, "styleName option",},
    {"configure", 2, StyleConfigureOp, 4, 0, "styleName options...",},
    {"create",    2, StyleCreateOp,    5, 0, "type styleName options...",},
    {"delete",    1, StyleDeleteOp,    3, 0, "?styleName ...?",},
    {"exists",    1, StyleExistsOp,    4, 4, "styleName",},
    {"get",       1, StyleGetOp,       4, 4, "cellName",},
    {"names",     1, StyleNamesOp,     3, 3, "",}, 
    {"type",      1, StyleTypeOp,      4, 4, "styleName",},
};

static int numStyleOps = sizeof(styleOps) / sizeof(Blt_OpSpec);

static int
StyleOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;
    int result;

    proc = Blt_GetOpFromObj(interp, numStyleOps, styleOps, BLT_OP_ARG2, objc, 
        objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    result = (*proc)(clientData, interp, objc, objv);
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * TypeOp --
 *
 * Results:
 *      A standard TCL result.  If TCL_ERROR is returned, then interp->result
 *      contains an error message.
 *
 *      pathName type cell
 *
 *---------------------------------------------------------------------------
 */
static int
TypeOp(TableView *viewPtr, Tcl_Interp *interp, int objc, Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    CellKey *keyPtr;
    CellStyle *stylePtr;
    Column *colPtr;
    Row *rowPtr;

    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    if (cellPtr == NULL) {
        return TCL_OK;
    }
    keyPtr = GetKey(viewPtr, cellPtr);
    colPtr = keyPtr->colPtr;
    rowPtr = keyPtr->rowPtr;
    stylePtr = GetCurrentStyle(viewPtr, rowPtr, colPtr, cellPtr);
    if (stylePtr->name != NULL) {
        Tcl_SetStringObj(Tcl_GetObjResult(interp), stylePtr->classPtr->type,-1);
    }
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * UpdatesOp --
 *
 *      pathName updates false
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
UpdatesOp(ClientData clientData, Tcl_Interp *interp, int objc, 
          Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    int state;

    if (objc == 3) {
        if (Tcl_GetBooleanFromObj(interp, objv[2], &state) != TCL_OK) {
            return TCL_ERROR;
        }
        if (state) {
            viewPtr->flags &= ~DONT_UPDATE;
            viewPtr->flags |= LAYOUT_PENDING | GEOMETRY;
            EventuallyRedraw(viewPtr);
        } else {
            viewPtr->flags |= DONT_UPDATE;
        }
    } else {
        state = (viewPtr->flags & DONT_UPDATE) ? FALSE : TRUE;
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * WritableOp --
 *
 *        pathName writable cell
 *
 *---------------------------------------------------------------------------
 */
/*ARGSUSED*/
static int
WritableOp(ClientData clientData, Tcl_Interp *interp, int objc, 
           Tcl_Obj *const *objv)
{
    Cell *cellPtr;
    TableView *viewPtr = clientData;
    int state;

    if (GetCellFromObj(interp, viewPtr, objv[2], &cellPtr) != TCL_OK) {
        return TCL_ERROR;
    }
    state = FALSE;
    if (cellPtr != NULL) {
        CellKey *keyPtr;
        CellStyle *stylePtr;
        Column *colPtr;
        Row *rowPtr;

        keyPtr = GetKey(viewPtr, cellPtr);
        colPtr = keyPtr->colPtr;
        rowPtr = keyPtr->rowPtr;
        stylePtr = GetCurrentStyle(viewPtr, rowPtr, colPtr, cellPtr);
        
        state = (stylePtr->flags & EDIT);
    }
    Tcl_SetBooleanObj(Tcl_GetObjResult(interp), state);
    return TCL_OK;
}

static int
XViewOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    int width, worldWidth;

    width = VPORTWIDTH(viewPtr);
    worldWidth = viewPtr->worldWidth;
    if (objc == 2) {
        double fract;
        Tcl_Obj *listObjPtr;

        /*
         * Note that we are bounding the fractions between 0.0 and 1.0
         * to support the "canvas"-style of scrolling.
         */
        listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
        fract = (double)viewPtr->columns.scrollOffset / worldWidth;
        fract = FCLAMP(fract);
        Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewDoubleObj(fract));
        fract = (double)(viewPtr->columns.scrollOffset + width) / worldWidth;
        fract = FCLAMP(fract);
        Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewDoubleObj(fract));
        Tcl_SetObjResult(interp, listObjPtr);
        return TCL_OK;
    }
    if (Blt_GetScrollInfoFromObj(interp, objc - 2, objv + 2, 
            &viewPtr->columns.scrollOffset, worldWidth, width, 
            viewPtr->columns.scrollUnits, viewPtr->scrollMode) != TCL_OK) {
        return TCL_ERROR;
    }
    viewPtr->columns.flags |= SCROLL_PENDING;
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

static int
YViewOp(ClientData clientData, Tcl_Interp *interp, int objc, 
        Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    int height, worldHeight;

    height = VPORTHEIGHT(viewPtr);
    worldHeight = viewPtr->worldHeight;
    if (objc == 2) {
        double fract;
        Tcl_Obj *listObjPtr;

        listObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
        /* Report first and last fractions */
        fract = (double)viewPtr->rows.scrollOffset / worldHeight;
        fract = FCLAMP(fract);
        Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewDoubleObj(fract));
        fract = (double)(viewPtr->rows.scrollOffset + height) / worldHeight;
        fract = FCLAMP(fract);
        Tcl_ListObjAppendElement(interp, listObjPtr, Tcl_NewDoubleObj(fract));
        Tcl_SetObjResult(interp, listObjPtr);
        return TCL_OK;
    }
    if (Blt_GetScrollInfoFromObj(interp, objc - 2, objv + 2, 
            &viewPtr->rows.scrollOffset, worldHeight, height, 
            viewPtr->rows.scrollUnits, viewPtr->scrollMode) != TCL_OK) {
        return TCL_ERROR;
    }
    viewPtr->rows.flags |= SCROLL_PENDING;
    EventuallyRedraw(viewPtr);
    return TCL_OK;
}

/* 
 * pathName op 
 */
static Blt_OpSpec viewOps[] =
{
    {"activate",     1, ActivateOp,      3, 3, "cellName"},
    {"bbox",         2, BboxOp,          3, 0, "cellName ?switches ...?"}, 
    {"bind",         2, BindOp,          3, 5, "cellName ?sequence command?"}, 
    {"cell",         2, CellOp,          2, 0, "args"}, 
    {"cget",         2, CgetOp,          3, 3, "option"}, 
    {"column",       3, ColumnOp,        2, 0, "oper args"}, 
    {"configure",    3, ConfigureOp,     2, 0, "?option value ...?"},
    {"curselection", 2, CurselectionOp,  2, 2, ""},
    {"deactivate",   1, DeactivateOp,    2, 2, ""},
    {"filter",       3, FilterOp,        2, 0, "args"},
    {"find",         3, FindOp,          2, 0, "expr"}, 
    {"focus",        2, FocusOp,         2, 3, "?cellName?"}, 
    {"grab",         1, GrabOp,          2, 3, "?cellName?"}, 
    {"highlight",    1, HighlightOp,     3, 3, "cellName"}, 
    {"identify",     2, IdentifyOp,      5, 5, "cellName x y"}, 
    {"index",        3, IndexOp,         3, 3, "cellName"}, 
    {"inside",       3, InsideOp,        5, 5, "cellName x y"}, 
    {"invoke",       3, InvokeOp,        3, 3, "cellName"}, 
    {"ishidden",     2, IsHiddenOp,      3, 3, "cellName"},
    {"row",          1, RowOp,           2, 0, "oper args"}, 
    {"scan",         2, ScanOp,          5, 5, "dragto|mark x y"},
    {"see",          3, SeeOp,           3, 3, "cellName"},
    {"selection",    3, SelectionOp,     2, 0, "oper args"},
    {"sort",         2, SortOp,          2, 0, "args"},
    {"style",        2, StyleOp,         2, 0, "args"},
    {"type",         1, TypeOp,          3, 3, "cellName"},
    {"unhighlight",  3, HighlightOp,     3, 3, "cellName"}, 
    {"updates",      2, UpdatesOp,       2, 3, "?bool?"},
    {"writable",     1, WritableOp,      3, 3, "cellName"},
    {"xview",        1, XViewOp,         2, 5, "?moveto fract? ?scroll number what?"},
    {"yview",        1, YViewOp,         2, 5, "?moveto fract? ?scroll number what?"},
};

static int numViewOps = sizeof(viewOps) / sizeof(Blt_OpSpec);

/*
 *---------------------------------------------------------------------------
 *
 * TableViewInstObjCmdProc --
 *
 *      This procedure is invoked to process commands on behalf of the
 *      tableview widget.
 *
 * Results:
 *      A standard TCL result.
 *
 * Side effects:
 *      See the user documentation.
 *
 *---------------------------------------------------------------------------
 */
static int
TableViewInstObjCmdProc(ClientData clientData, Tcl_Interp *interp, int objc,
                        Tcl_Obj *const *objv)
{
    TableView *viewPtr = clientData;
    Tcl_ObjCmdProc *proc;
    int result;

    proc = Blt_GetOpFromObj(interp, numViewOps, viewOps, BLT_OP_ARG1, 
        objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    Tcl_Preserve(viewPtr);
    result = (*proc) (clientData, interp, objc, objv);
    Tcl_Release(viewPtr);
    return result;
}

/*
 *---------------------------------------------------------------------------
 *
 * TableViewInstCmdDeleteProc --
 *
 *      This procedure is invoked when a widget command is deleted.  If the
 *      widget isn't already in the process of being destroyed, this
 *      command destroys it.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The widget is destroyed.
 *
 *---------------------------------------------------------------------------
 */
static void
TableViewInstCmdDeleteProc(ClientData clientData)
{
    TableView *viewPtr = clientData;

    /*
     * This procedure could be called either because the tableview window
     * was destroyed and the command was then deleted (in which case tkwin
     * is NULL) or because the command was deleted, and then this procedure
     * destroys the widget.
     */
    if (viewPtr->tkwin != NULL) {
        Tk_Window tkwin;

        tkwin = viewPtr->tkwin;
        viewPtr->tkwin = NULL;
        Tk_DestroyWindow(tkwin);
    }
}

static int
ReplaceTable(TableView *viewPtr, BLT_TABLE table)
{
    Column **colMap;
    Column *colPtr;
    Row **rowMap;
    long i;
    size_t oldSize, newSize, numColumns, numRows;
    unsigned int flags;

    /* Step 1: Cancel any pending idle callbacks for this table. */
    if (viewPtr->flags & SELECT_PENDING) {
        Tcl_CancelIdleCall(SelectCommandProc, viewPtr);
    }
    viewPtr->rows.lastIndex = viewPtr->rows.firstIndex = -1;
    viewPtr->columns.lastIndex = viewPtr->columns.firstIndex = -1;
    ClearSelections(viewPtr);

    /* 3. Allocate a map big enough for all columns.  Worst case is oldSize
     * + newSize. */
    oldSize = viewPtr->columns.length;
    newSize = blt_table_num_columns(table);
    numColumns = newSize;
    if (viewPtr->columns.flags & AUTO_MANAGE)  {
        numColumns += oldSize;
    }
    colMap = Blt_Calloc(numColumns, sizeof(Column *));
    if (colMap == NULL) {
        return TCL_ERROR;
    }
    /* Puts the sticky columns in the map first.  This will retain their
     * original locations. */
    for (colPtr = viewPtr->columns.firstPtr; colPtr != NULL; 
         colPtr = colPtr->nextPtr) {
        if (colPtr->flags & STICKY) {
            assert(colMap[colPtr->index] == NULL);
            colMap[colPtr->index] = colPtr;
            viewPtr->columns.map[colPtr->index] = NULL;
        }
    }
    /* Next add columns from the new table, that already have a column. */
    if (viewPtr->columns.flags & AUTO_MANAGE) {
        BLT_TABLE_COLUMN col;
        long i, j;

        for (col = blt_table_first_column(table); col != NULL; 
             col = blt_table_next_column(col)) {
            BLT_TABLE_COLUMN oldCol;
            const char *label;

            label = blt_table_column_label(col);
            /* Does this column label exist in the old table? */
            oldCol = blt_table_get_column_by_label(viewPtr->table, label);
            if (oldCol != NULL) {
                Blt_HashEntry *hPtr;
                int isNew;

                /* Get the column container and replace its column
                 * reference and hash with a new entry. */
                colPtr = GetColumnContainer(viewPtr, oldCol);

                /* Replace the previous hash entry with a new one. */
                hPtr = Blt_CreateHashEntry(&viewPtr->columns.table, 
                        (char *)oldCol, &isNew);
                assert(isNew);
                if (colPtr->hashPtr != NULL) {
                    Blt_DeleteHashEntry(&viewPtr->columns.table, colPtr->hashPtr);
                }
                colPtr->hashPtr = hPtr;
                colPtr->column = col;
                viewPtr->columns.map[colPtr->index] = NULL;
                colMap[colPtr->index] = colPtr;
            }
        }

        /* 5. New fill in the map with columns from the new table that
         * weren't in the old. */
        for (i = 0, col = blt_table_first_column(table); col != NULL;  
             col = blt_table_next_column(col)) {
            Blt_HashEntry *hPtr;
            Column *colPtr;
            int isNew;
            
            hPtr = Blt_CreateHashEntry(&viewPtr->columns.table, (char *)col, 
                                       &isNew);
            if (!isNew) {
                continue;               /* Handled in the previous
                                         * step.  */
            }
            colPtr = CreateColumn(viewPtr, col, hPtr);
            while (colMap[i] != NULL) { /* Find the next open slot. */
                i++;                        
            }
            colMap[i] = colPtr;
        }

        /* 6. Find any enpty slots and remove them. */
        for (i = j = 0; i < numColumns; i++) {
            if (colMap[i] == NULL) {
                continue;
            }
            j++;
            if (i < j) {
                colMap[j] = colMap[i];
            }
            colMap[j]->index = j;
        }
        numColumns = j;
        colMap = Blt_Realloc(colMap, numColumns * sizeof(Column *));
    }

    /* 7. Go through the old map and remove any left over columns that are
     * not in the new table. */
    for (i = 0; i < viewPtr->columns.length; i++) {
        Column *colPtr;

        colPtr = viewPtr->columns.map[i];
        if (colPtr != NULL) {
            DestroyColumn(colPtr);
        }
    }
    if (viewPtr->columns.map != NULL) {
        Blt_Free(viewPtr->columns.map);
    }

    RethreadColumns(viewPtr);

    /* 8. Allocate a new row array that can hold all the rows. */
    oldSize = viewPtr->rows.length;
    newSize = blt_table_num_rows(table);
    numRows = (viewPtr->rows.flags & AUTO_MANAGE) ? 
        MAX(oldSize, newSize) : newSize;
    rowMap = Blt_Calloc(numRows, sizeof(Row *));
    if (rowMap == NULL) {
        return TCL_ERROR;
    }

    if (viewPtr->rows.flags & AUTO_MANAGE) {
        BLT_TABLE_ROW row;
        long i, j;

        /* 9. Move rows that exist in both the old and new tables into the
         *    merge array. */
        for (i = 0; i < viewPtr->rows.length; i++) {
            BLT_TABLE_ROW newRow;
            Row *rowPtr;
            const char *label;

            rowPtr = viewPtr->rows.map[i];
            label = blt_table_row_label(rowPtr->row);
            newRow = blt_table_get_row_by_label(table, label);
            if (newRow != NULL) {
                Blt_HashEntry *hPtr;
                int isNew;

                hPtr = Blt_CreateHashEntry(&viewPtr->rows.table, (char *)newRow, 
                                           &isNew);
                assert(isNew);
                if (rowPtr->hashPtr != NULL) {
                    Blt_DeleteHashEntry(&viewPtr->rows.table, rowPtr->hashPtr);
                }
                rowPtr->hashPtr = hPtr;
                rowPtr->row = newRow;
                rowMap[i] = rowPtr;
                viewPtr->rows.map[i] = NULL;
            }
        }
        /* 10. Add rows from the the new table that don't already exist. */
        for (i = 0, row = blt_table_first_row(table); row != NULL;  
             row = blt_table_next_row(row)) {
            Blt_HashEntry *hPtr;
            Row *rowPtr;
            int isNew;

            hPtr = Blt_CreateHashEntry(&viewPtr->rows.table, (char *)row, &isNew);
            if (!isNew) {
                /* This works because we're matching against the row
                 * pointer not the row label.  */
                continue;
            }
            rowPtr = CreateRow(viewPtr, row, hPtr);
            while (rowMap[i] != NULL) {    /* Get the next open slot. */
                i++;                        
            }
            rowMap[i] = rowPtr;
        }
        /* 11. Compress empty slots in row array. Re-number the row
         *     indices. */
        for (i = j = 0; i < numRows; i++) {
            if (rowMap[i] == NULL) {
                continue;
            }
            j++;
            if (i < j) {
                rowMap[j] = rowMap[i];
            }
            rowMap[j]->index = j;
        }
        numRows = j;
        rowMap = Blt_Realloc(rowMap, numRows * sizeof(Row *));
    }

    /* 12. Remove all non-NULL rows. These are rows from the old table, not
     *     used in the new table. */
    for (i = 0; i < viewPtr->rows.length; i++) {
        Row *rowPtr;

        rowPtr = viewPtr->rows.map[i];
        if (rowPtr != NULL) {
            DestroyRow(rowPtr);
        }
    }
    if (viewPtr->rows.map != NULL) {
        Blt_Free(viewPtr->rows.map);
    }
    viewPtr->rows.map = rowMap;
    viewPtr->rows.length = numRows;

    RethreadRows(viewPtr);

    /* 13. Create cells */
    for (i = 0; i < viewPtr->rows.length; i++) {
        CellKey key;
        long j;
        
        key.rowPtr = viewPtr->rows.map[i];
        for (j = 0; j < viewPtr->columns.length; j++) {
            Blt_HashEntry *hPtr;
            int isNew;
            
            key.colPtr = viewPtr->columns.map[j];
            hPtr = Blt_CreateHashEntry(&viewPtr->cellTable, (char *)&key, 
                &isNew);
            if (isNew) {
                Cell *cellPtr;

                cellPtr = NewCell(viewPtr, hPtr);
                Blt_SetHashValue(hPtr, cellPtr);
            }
        }
    }

    viewPtr->table = table;
    flags = TABLE_TRACE_FOREIGN_ONLY | TABLE_TRACE_WRITES | TABLE_TRACE_UNSETS;
    blt_table_trace_row(table, TABLE_TRACE_ALL_ROWS, flags, 
        RowTraceProc, NULL, viewPtr);
    blt_table_trace_column(table, TABLE_TRACE_ALL_COLUMNS, flags, 
        ColumnTraceProc, NULL, viewPtr);
    return TCL_OK;
}

/*
 *---------------------------------------------------------------------------
 *
 * DisplayProc --
 *
 *      This procedure is invoked to display the widget.
 *
 *      Recompute the layout of the text if necessary. This is necessary if
 *      the world coordinate system has changed.  Specifically, the
 *      following may have occurred:
 *
 *        1.  a text attribute has changed (font, linespacing, etc.).
 *        2.  an entry's option changed, possibly resizing the entry.
 *
 *      This is deferred to the display routine since potentially many of
 *      these may occur.
 *
 *      Set the vertical and horizontal scrollbars.  This is done here
 *      since the window width and height are needed for the scrollbar
 *      calculations.
 *
 * Results:
 *      None.
 *
 * Side effects:
 *      The widget is redisplayed.
 *
 *---------------------------------------------------------------------------
 */
static void
DisplayProc(ClientData clientData)
{
    Pixmap drawable; 
    TableView *viewPtr = clientData;
    int reqWidth, reqHeight;
    long i;

    viewPtr->flags &= ~REDRAW_PENDING;
    if (viewPtr->tkwin == NULL) {
        return;                         /* Window has been destroyed. */
    }
#ifdef notdef
    fprintf(stderr, "DisplayProc %s\n", Tk_PathName(viewPtr->tkwin));
#endif
    if (viewPtr->rows.flags & REINDEX) {
        RenumberRows(viewPtr);
    }
    if (viewPtr->columns.flags & REINDEX) {
        RenumberColumns(viewPtr);
    }
    if (viewPtr->sort.flags & SORT_PENDING) {
        /* If the table needs sorting do it now before recalculating the
         * geometry. */
        SortTableView(viewPtr); 
    }
    if (viewPtr->flags & GEOMETRY) {
        ComputeGeometry(viewPtr);
    }   
    if (viewPtr->flags & LAYOUT_PENDING) {
        ComputeLayout(viewPtr);
    }
    if ((viewPtr->columns.flags | viewPtr->rows.flags) & SCROLL_PENDING) {
        int width, height;

        /* Scrolling means that the view port has changed or that the
         * visible entries need to be recomputed. */
        width = VPORTWIDTH(viewPtr);
        height = VPORTHEIGHT(viewPtr);
        if ((viewPtr->columns.flags & SCROLL_PENDING) && 
            (viewPtr->columns.scrollCmdObjPtr != NULL)) {
            /* Tell the x-scrollbar the new sizes. */
            Blt_UpdateScrollbar(viewPtr->interp, 
                viewPtr->columns.scrollCmdObjPtr, viewPtr->columns.scrollOffset,
                viewPtr->columns.scrollOffset + width, viewPtr->worldWidth);
            viewPtr->columns.flags &= ~SCROLL_PENDING;
        }
        if ((viewPtr->rows.flags & SCROLL_PENDING) && 
            (viewPtr->rows.scrollCmdObjPtr != NULL)) {
            /* Tell the y-scrollbar the new sizes. */
            Blt_UpdateScrollbar(viewPtr->interp, viewPtr->rows.scrollCmdObjPtr,
                viewPtr->rows.scrollOffset, viewPtr->rows.scrollOffset + height,
                viewPtr->worldHeight);
            viewPtr->rows.flags &= ~SCROLL_PENDING;
        }
        /* Determine the visible rows and columns. The can happen when the
         * -hide flags changes on a row or column. */
        ComputeVisibleEntries(viewPtr);
    }
    reqHeight = (viewPtr->reqHeight > 0) ? viewPtr->reqHeight : 
        viewPtr->worldHeight + viewPtr->columns.titleHeight + 
        viewPtr->columns.filterHeight + 2 * viewPtr->inset + 1;
    reqWidth = (viewPtr->reqWidth > 0) ? viewPtr->reqWidth : 
        viewPtr->worldWidth + viewPtr->rows.titleWidth + 2 * viewPtr->inset;

    if ((reqWidth != Tk_ReqWidth(viewPtr->tkwin)) || 
        (reqHeight != Tk_ReqHeight(viewPtr->tkwin))) {
        Tk_GeometryRequest(viewPtr->tkwin, reqWidth, reqHeight);
    }
    if (!Tk_IsMapped(viewPtr->tkwin)) {
        return;
    }
    if ((viewPtr->flags & REDRAW) == 0) {
        return;
    }
    viewPtr->flags &= ~REDRAW;
    Blt_PickCurrentItem(viewPtr->bindTable);
    if ((viewPtr->rows.firstIndex < 0) || (viewPtr->columns.firstIndex < 0)){
        /* Empty table, draw blank area. */
        Blt_Bg_FillRectangle(viewPtr->tkwin, Tk_WindowId(viewPtr->tkwin), 
                viewPtr->bg, 0, 0, Tk_Width(viewPtr->tkwin), 
                Tk_Height(viewPtr->tkwin), viewPtr->borderWidth, 
                viewPtr->relief);
        DrawOuterBorders(viewPtr, Tk_WindowId(viewPtr->tkwin));
    }

    drawable = Blt_GetPixmap(viewPtr->display, Tk_WindowId(viewPtr->tkwin), 
        Tk_Width(viewPtr->tkwin), Tk_Height(viewPtr->tkwin), 
        Tk_Depth(viewPtr->tkwin));
    Blt_Bg_FillRectangle(viewPtr->tkwin, drawable, 
        viewPtr->bg, 0, 0, Tk_Width(viewPtr->tkwin), 
        Tk_Height(viewPtr->tkwin), viewPtr->borderWidth, viewPtr->relief);

    if ((viewPtr->focusPtr == NULL) && 
        (viewPtr->rows.firstIndex >= 0) && (viewPtr->columns.firstIndex >= 0)) {
        /* Re-establish the focus entry at the top entry. */
        Row *rowPtr;
        Column *colPtr;

        colPtr = GetFirstColumn(viewPtr);
        rowPtr = GetFirstRow(viewPtr);
        viewPtr->focusPtr = GetCell(viewPtr, rowPtr, colPtr);
    }
    /* Draw the cells. */
    for (i = viewPtr->rows.firstIndex; i <= viewPtr->rows.lastIndex; i++) {
        long j;
        Row *rowPtr;

        rowPtr = viewPtr->rows.map[i];
        /* Draw each cell in the row. */
        for (j = viewPtr->columns.firstIndex; j <= viewPtr->columns.lastIndex; 
             j++) {
            Column *colPtr;
            Cell *cellPtr;

            colPtr = viewPtr->columns.map[j];
            cellPtr = GetCell(viewPtr, rowPtr, colPtr);
            assert(cellPtr != NULL);
            DisplayCell(viewPtr, cellPtr, drawable, FALSE);
        }
    }
    if (viewPtr->rows.flags & SHOW_TITLES) {
        DisplayRowTitles(viewPtr, drawable);
    }
    if (viewPtr->columns.flags & SHOW_TITLES) {
        DisplayColumnTitles(viewPtr, drawable);
    }
    if (viewPtr->rows.flags & viewPtr->columns.flags & SHOW_TITLES) {
        /* When showing both row and column titles, the area above the row
         * titles needs to be filled: both for the height of the column
         * title and column filter (if there is one). */
        if ((viewPtr->rows.titleWidth > 0) && 
            (viewPtr->columns.titleHeight > 0)) {
            Blt_Bg_FillRectangle(viewPtr->tkwin, drawable, 
                viewPtr->columns.normalTitleBg, viewPtr->inset, viewPtr->inset, 
                viewPtr->rows.titleWidth, viewPtr->columns.titleHeight, 
                viewPtr->columns.titleBorderWidth, TK_RELIEF_RAISED);
        }
        if ((viewPtr->rows.titleWidth > 0) && 
            (viewPtr->columns.filterHeight > 0)) {
            Blt_Bg_FillRectangle(viewPtr->tkwin, drawable, 
                viewPtr->columns.normalTitleBg, viewPtr->inset, 
                viewPtr->inset + viewPtr->columns.titleHeight, 
                viewPtr->rows.titleWidth, viewPtr->columns.filterHeight, 
                viewPtr->columns.titleBorderWidth, TK_RELIEF_RAISED);
        }
    }
    DrawOuterBorders(viewPtr, drawable);
    /* Now copy the new view to the window. */
    XCopyArea(viewPtr->display, drawable, Tk_WindowId(viewPtr->tkwin), 
        viewPtr->rows.normalTitleGC, 0, 0, Tk_Width(viewPtr->tkwin), 
        Tk_Height(viewPtr->tkwin), 0, 0);
    Tk_FreePixmap(viewPtr->display, drawable);
}

/*
 *---------------------------------------------------------------------------
 *
 * NewTableView --
 *
 *---------------------------------------------------------------------------
 */
static TableView *
NewTableView(Tcl_Interp *interp, Tk_Window tkwin)
{
    TableView *viewPtr;

    Tk_SetClass(tkwin, "BltTableView");
    viewPtr = Blt_AssertCalloc(1, sizeof(TableView));
    viewPtr->tkwin = tkwin;
    viewPtr->display = Tk_Display(tkwin);
    viewPtr->interp = interp;
    viewPtr->flags = GEOMETRY | LAYOUT_PENDING;
    viewPtr->columns.flags = 
        AUTO_MANAGE | SCROLL_PENDING | SLIDE_ENABLED | SHOW_TITLES;
    viewPtr->rows.flags = AUTO_MANAGE | SCROLL_PENDING;
    viewPtr->highlightWidth = 2;
    viewPtr->borderWidth = 2;
    viewPtr->relief = TK_RELIEF_SUNKEN;
    viewPtr->scrollMode = BLT_SCROLL_MODE_HIERBOX;
    viewPtr->columns.scrollUnits = viewPtr->rows.scrollUnits = 20;
    viewPtr->selectMode = SELECT_SINGLE_ROW;
    viewPtr->rows.selection.list = Blt_Chain_Create();
    viewPtr->reqWidth = viewPtr->reqHeight = 400;
    viewPtr->columns.titleBorderWidth = viewPtr->rows.titleBorderWidth = 2;
    viewPtr->filter.borderWidth = 1;
    viewPtr->filter.outerBorderWidth = 1;
    viewPtr->filter.relief = TK_RELIEF_SOLID;
    viewPtr->filter.selectRelief = TK_RELIEF_SUNKEN;
    viewPtr->filter.activeRelief = TK_RELIEF_RAISED;
    viewPtr->bindTable = Blt_CreateBindingTable(interp, tkwin, viewPtr, 
        TableViewPickProc, AppendTagsProc);
    Blt_InitHashTableWithPool(&viewPtr->cellTable, sizeof(CellKey)/sizeof(int));
    Blt_InitHashTableWithPool(&viewPtr->rows.table, BLT_ONE_WORD_KEYS);
    Blt_InitHashTableWithPool(&viewPtr->columns.table, BLT_ONE_WORD_KEYS);
    Blt_InitHashTable(&viewPtr->iconTable, BLT_STRING_KEYS);
    Blt_InitHashTable(&viewPtr->styleTable, BLT_STRING_KEYS);
    Blt_InitHashTable(&viewPtr->bindTagTable,
                      sizeof(struct _BindTag)/sizeof(int));
    Blt_InitHashTable(&viewPtr->uidTable, BLT_STRING_KEYS);
    Blt_InitHashTable(&viewPtr->cachedObjTable, BLT_STRING_KEYS);
    Blt_InitHashTableWithPool(&viewPtr->selectCells.cellTable, 
                              sizeof(CellKey)/sizeof(int));
    viewPtr->rows.pool    = Blt_Pool_Create(BLT_FIXED_SIZE_ITEMS);
    viewPtr->columns.pool = Blt_Pool_Create(BLT_FIXED_SIZE_ITEMS);
    viewPtr->cellPool   = Blt_Pool_Create(BLT_FIXED_SIZE_ITEMS);
    viewPtr->cmdToken = Tcl_CreateObjCommand(interp, Tk_PathName(tkwin), 
        TableViewInstObjCmdProc, viewPtr, TableViewInstCmdDeleteProc);

    Blt_SetWindowInstanceData(tkwin, viewPtr);
    Tk_CreateSelHandler(tkwin, XA_PRIMARY, XA_STRING, SelectionProc,
        viewPtr, XA_STRING);
    Tk_CreateEventHandler(tkwin, ExposureMask | StructureNotifyMask |
        FocusChangeMask, TableViewEventProc, viewPtr);

    return viewPtr;
}

/*
 *---------------------------------------------------------------------------
 *
 * TableViewCmdProc --
 *
 *      This procedure is invoked to process the TCL command that
 *      corresponds to a widget managed by this module. See the user
 *      documentation for details on what it does.
 *
 * Results:
 *      A standard TCL result.
 *
 * Side effects:
 *      See the user documentation.
 *
 *---------------------------------------------------------------------------
 */
/* ARGSUSED */
static int
TableViewCmdProc(
    ClientData clientData,              /* Main window associated with
                                         * interpreter. */
    Tcl_Interp *interp,                 /* Current interpreter. */
    int objc,                           /* Number of arguments. */
    Tcl_Obj *const *objv)               /* Argument strings. */
{
    CellStyle *stylePtr;
    TableView *viewPtr;
    Tcl_Obj *cmdObjPtr, *objPtr;
    Tk_Window mainWin = clientData;
    Tk_Window tkwin;
    const char *string;
    int result;

    tkwin = NULL;
    if (objc < 2) {
        Tcl_AppendResult(interp, "wrong # args: should be \"", 
                Tcl_GetString(objv[0]), " pathName ?option value ...?\"", 
                (char *)NULL);
        return TCL_ERROR;
    }
    /*
     * Invoke a procedure to initialize various bindings on tableview
     * entries.  If the procedure doesn't already exist, source it from
     * "$blt_library/TableView.tcl".  We deferred sourcing the file until
     * now so that the variable $blt_library could be set within a script.
     */
    if (!Blt_CommandExists(interp, "::blt::TableView::Initialize")) {
        if (Tcl_GlobalEval(interp, 
                "source [file join $blt_library bltTableView.tcl]") != TCL_OK) {
            char info[200];

            Blt_FmtString(info, 200, 
                             "\n\t(while loading bindings for %.50s)", 
                    Tcl_GetString(objv[0]));
            Tcl_AddErrorInfo(interp, info);
            return TCL_ERROR;
        }
    }
    string = Tcl_GetString(objv[1]);
    tkwin = Tk_CreateWindowFromPath(interp, mainWin, string, (char *)NULL);
    if (tkwin == NULL) {
        return TCL_ERROR;
    }
    viewPtr = NewTableView(interp, tkwin);
    /* 
     * Initialize the widget's configuration options here. The options need
     * to be set first, so that entry, column, and style components can use
     * them for their own GCs.
     */
    iconOption.clientData = viewPtr;
    styleOption.clientData = viewPtr;
    tableOption.clientData = viewPtr;
    if (Blt_ConfigureWidgetFromObj(interp, viewPtr->tkwin, tableSpecs, 
        objc - 2, objv + 2, (char *)viewPtr, 0) != TCL_OK) {
        goto error;
    }
    /* 
     * Rebuild the widget's GC and other resources that are predicated by
     * the widget's configuration options.  Do the same for the default
     * column.
     */
    if (ConfigureTableView(interp, viewPtr) != TCL_OK) {
        goto error;
    }

    stylePtr = Blt_TableView_CreateCellStyle(interp, viewPtr, STYLE_TEXTBOX, 
        "default");
    if (stylePtr == NULL) {
        goto error;
    }
    viewPtr->stylePtr = stylePtr;
    iconOption.clientData = viewPtr;
    if (Blt_ConfigureComponentFromObj(interp, tkwin, stylePtr->name,
         stylePtr->classPtr->className, stylePtr->classPtr->specs, 0, NULL, 
        (char *)stylePtr, 0) != TCL_OK) {
        (*stylePtr->classPtr->freeProc)(stylePtr);
        goto error;
    }
    (*stylePtr->classPtr->configProc)(viewPtr, stylePtr);

    if (Blt_ConfigureComponentFromObj(interp, tkwin, "filter", "Filter", 
        filterSpecs, 0, NULL, (char *)viewPtr, 0) != TCL_OK) {
        goto error;
    }
    if (Blt_ConfigModified(tableSpecs, "-columnfilters", (char *)NULL)) {
        ConfigureFilters(interp, viewPtr);
        if (InitColumnFilters(interp, viewPtr) != TCL_OK) {
            return TCL_ERROR;
        }
    }

    cmdObjPtr = Tcl_NewListObj(0, (Tcl_Obj **)NULL);
    objPtr = Tcl_NewStringObj("::blt::TableView::Initialize", -1);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objPtr);
    Tcl_ListObjAppendElement(interp, cmdObjPtr, objv[1]);
    Tcl_IncrRefCount(cmdObjPtr);
    result = Tcl_EvalObjEx(interp, cmdObjPtr, TCL_EVAL_GLOBAL);
    Tcl_DecrRefCount(cmdObjPtr);
    if (result != TCL_OK) {
        goto error;
    }
    Tcl_SetStringObj(Tcl_GetObjResult(interp), Tk_PathName(viewPtr->tkwin), -1);
    return TCL_OK;

  error:
    Tk_DestroyWindow(tkwin);
    return TCL_ERROR;
}

int
Blt_TableViewCmdInitProc(Tcl_Interp *interp)
{
    static Blt_CmdSpec cmdSpec = {"tableview", TableViewCmdProc};

    cmdSpec.clientData = Tk_MainWindow(interp);
    return Blt_InitCmd(interp, "::blt", &cmdSpec);
}
#endif /* NO_TABLEVIEW */
