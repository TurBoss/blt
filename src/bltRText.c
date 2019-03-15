/* -*- mode: c; c-basic-offset: 4; indent-tabs-mode: nil -*- */
/*
 * bltRText.c --
 *
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

textName insert 0,0 text ?tag? ?text tag?
textName configure ?option value ...?
textName cget option
textName get firstIndex lastIndex

textName image create textIndex -image imageName
textName image configure textIndex -image imageName
textName image cget textIndex -image imageName

textName delete 0,0 end 
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
textName configure tagName ?option value ...?
	 -font
	 -text
	 -foreground
	 -wrap none|auto|ellipsis
	 -wraplength 
	 	 
textName cget tagName option

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

*/

static Blt_ConfigSpec textSpecs[] = {
    {BLT_CONFIG_ANCHOR, "-anchor", "anchor", "Anchor", DEF_ANCHOR,
        Blt_Offset(RText, anchor), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_END}
};

static Blt_ConfigSpec tagSpecs[] = {
    {BLT_CONFIG_BACKGROUND, "-background", "background", "Background", 
	DEF_BACKGROUND, Blt_Offset(RTextTag, bg), 0},
    {BLT_CONFIG_FONT, "-font", "font", "Font", DEF_FONT, 
        Blt_Offset(RTextTag, font), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_COLOR, "-foreground", "foreground", "Foreground", 
        DEF_FOREGROUND, Blt_Offset(RTextTag, fgColor), BLT_CONFIG_NULL_OK},
    {BLT_CONFIG_SYNONYM, "-bg", "background"},
    {BLT_CONFIG_SYNONYM, "-fg", "foreground"},
    {BLT_CONFIG_JUSTIFY, "-justify", "justify", "Justify", DEF_JUSTIFY, 
	Blt_Offset(RTextTag, justify), BLT_CONFIG_DONT_SET_DEFAULT},
    {BLT_CONFIG_BITMASK, "-overstrike", "overstrike", "Overstrike", 
        DEF_OVERSTRIKE, Blt_Offset(RTextTag, flags), 
        BLT_CONFIG_DONT_SET_DEFAULT, (Blt_CustomOption *)OVERSTRIKE},
    {BLT_CONFIG_BITMASK, "-underline", "underline", "Underline", 
        DEF_UNDERLINE, Blt_Offset(RTextTag, flags), 
        BLT_CONFIG_DONT_SET_DEFAULT, (Blt_CustomOption *)UNDERLINE},
    {BLT_CONFIG_END}
};



/*
 *---------------------------------------------------------------------------
 *
 * TabOp --
 *
 *      This procedure handles tab operations.
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
};

static int numTabOps = sizeof(tabOps) / sizeof(Blt_OpSpec);

static int
TagOp(ClientData clientData, Tcl_Interp *interp, int objc, Tcl_Obj *const *objv)
{
    Tcl_ObjCmdProc *proc;

    proc = Blt_GetOpFromObj(interp, numTabOps, tabOps, BLT_OP_ARG2, 
           objc, objv, 0);
    if (proc == NULL) {
        return TCL_ERROR;
    }
    return (*proc) (clientData, interp, objc, objv);
}

