===================
blt::utils::compare
===================

------------------------------------------------
Utility commands to compare strings and numbers.
------------------------------------------------

.. include:: man.rst
.. include:: toc.rst

SYNOPSIS
--------

**blt::numberutils** *oper* ?\ *arg* ... ?

**blt::stringutils** *oper* ?\ *arg* ... ?

DESCRIPTION
-----------

These utility commands are used by the **blt::tableview** widget to filter
rows.  

The **numberutils** operations work like the TCL **expr** command, but also
handle cases where the floating point numbers are almost equal. They also
work with NaN, -Inf, and +Inf values.

The **stringutils** operations provide switches to compactly handle
different cases.

SYNTAX
------

**blt::numberutils** *operation*  ?\ *arg* ... ?
  Compares floating point numbers. Both *operation* and its arguments
  determine the exact behavior of the command.  The operations available
  for datatables are listed below in `NUMBER COMPARISONS`_.

**blt::stringutils** *operation*  ?\ *arg* ... ?
  Compares strings. Both *operation* and its arguments
  determine the exact behavior of the command.  The operations available
  for datatables are listed below in `STRING COMPARISONS`_.

NUMBER COMPARISONS
------------------

**blt::numberutils eq** *x* *y* 
  Indicates if *x* is equal to *y*.  *X* and *y* are floating point
  numbers.  If *x* is equal to *y* "1" is returned, otherwise "0".

**blt::numberutils ge** *x* *y* 
  Indicates if *x* is greater than or equal to *y*.  *X* and *y* are
  floating point numbers.  If *x* is greater than or equal to *y* "1" is
  returned, otherwise "0".

**blt::numberutils gt** *x* *y* 
  Indicates if *x* is greater than *y*.  *X* and *y* are floating point
  numbers.  If *x* is greater than *y* "1" is returned, otherwise "0".

**blt::numberutils isbetween** *x* *first* *last*
  Indicates if *x* is between *first* and *last*.  *X*, *first*, and *last*
  are floating point numbers.  If *x* is greater than or equal to *first*
  and *x* is less than of equal to *last* "1" is returned, otherwise "0".

**blt::numberutils ismember** *x* *numList* ?\ *switches ...* ?
  Indicates if *x* is a member or *numList*.  *X* is a floating
  point number. *NumList* is a list of floating point numbers.  If *x*
  is in the list "1" is returned, otherwise "0".  *Switches* are described
  below.

  **-sorted** *sortDirection*
    Specifies that the list is sorted and how it is sorted. Searching long
    lists is sped up by first sorting *numList*.  If *sortDirection* is
    "increasing*, the list was sorted in increasing order (lowest to
    highest). If *sortDirection* is "decreasing" the list was sorted in
    decreasing order (highest to lowest).

**blt::numberutils le** *x* *y* 
  Indicates if *x* is less than or equal to *y*.  *X* and *y* are floating
  point numbers.  If *x* is less than or equal to *y* "1" is returned,
  otherwise "0".

**blt::numberutils lt** *x* *y* 
  Indicates if *x* is less than *y*.  *X* and *y* are floating point
  numbers.  If *x* is less than *y* "1" is returned, otherwise "0".

STRING COMPARISONS
------------------

**blt::stringutils begins** *string* *pattern* ?\ *switches* ... ?
  Indicates if *string* begins with the string *pattern*.  *String* and
  *pattern* are ordinary TCL strings.  If *string* is begins with *pattern*
  "1" is returned, otherwise "0". *Switches* can be any of the following.

  **-nocase** 
    Specifies to ignore case when determining if *string* starts with
    *pattern*.

  **-trimwhitespace** 
    Trims leading whitespace from *string* before determining if *string*
    starts with *pattern*.

**blt::stringutils contains** *string* *pattern* ?\ *switches* ... ?
  Indicates if *string* is contains *pattern*.  *String* and *pattern* are
  ordinary TCL strings.  If *string* is contains *pattern* "1" is returned,
  otherwise "0". *Switches* can be any of the following.

  **-nocase** 
    Specifies to ignore case when determining if *string* starts with
    *pattern*.

  **-trimwhitespace** 
    Trims leading whitespace for *string* before determining if *string*
    starts with *pattern*.

**blt::stringutils::dictcompare** *string1*  *string2* 
  Compares strings *string1* and *string2* using a dictionary comparison
  (like **lsort**\'s **-dictionary** option). Both *string1* and *string2*
  are ordinary TCL strings. If *string1* is greater than *string2*, -1 is 
  returned. If *string1 is less than *string2*, 1 is returned.  If *string1*
  equals *string2*, 0 is returned.

**blt::stringutils ends** *string* *pattern* ?\ *switches* ... ?
  Indicates if *string* ends with the string *pattern*.  *String* and
  *pattern* are ordinary TCL strings.  If *string* is ends with *pattern*
  "1" is returned, otherwise "0". *Switches* can be any of the following.

  **-nocase** 
    Indicates to ignore case when comparing *string* with *pattern*.

  **-trimwhitespace** 
    Trims trailing whitespace from *string* before determining if *string*
    ends with *pattern*.

**blt::stringutils equals** *string* *pattern* ?\ *switches* ... ?
  Indicates if *string* equals *pattern*.  *String* and *pattern* are
  ordinary TCL strings.  If *string* equals *pattern* "1" is returned,
  otherwise "0". *Switches* can be any of the following.

  **-nocase** 
    Indicates to ignore case when comparing *string* to *pattern*.

  **-trimwhitespace** 
    Trims leading and trailing whitespace from *string*
    before determining if *string* equals *pattern*.

**blt::stringutils isbetween** *string* *first* *last** ?\ *switches* ... ?
  Indicates if *string* is between *first* and *last*.  *String*,
  *first* and *last* are ordinary TCL strings.  If *string* is
  greater than or equal to *first* and less than or equal to *last*
  "1" is returned, otherwise "0". *Switches* can be any of the following.

  **-nocase** 
    Specifies to ignore case when comparing strings.

**blt::stringutils ismember** *string* *strList* ?\ *switches* ... ?
  Indicates if *string* is member of *strList*.  *String* is an ordinary
  TCL string. *StrList* is a list of TCL strings.  If *string* is in the
  list "1" is returned, otherwise "0".  *Switches* are described below.

  **-sorted** *sortDirection*
    Specifies that the list is sorted and how it is sorted. Searching long
    lists is sped up by previously sorting *strList*.  If *sortDirection*
    is "increasing*, it is assumed that the list has been sorted in
    increasing order (lowest to highest). If *sortDirection* is
    "decreasing" the list is sorted in decreasing order (highest to
    lowest).  If *sortDirection* is none, a linear search is performed.
    The default is "none".

EXAMPLE
-------

KEYWORDS
--------

datatable, tableview

COPYRIGHT
---------

2015 George A. Howlett. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

 1) Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2) Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the distribution.
 3) Neither the name of the authors nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.
 4) Products derived from this software may not be called "BLT" nor may
    "BLT" appear in their names without specific prior written permission
    from the author.

THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
