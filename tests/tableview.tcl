
package require BLT

if {[info procs test] != "test"} {
    source defs
}
if [file exists ../library] {
    set blt_library ../library
}

#set VERBOSE 1
test tableview.1 {tableview no args} {
    list [catch {blt::tableview} msg] $msg
} {1 {wrong # args: should be "blt::tableview pathName ?option value ...?"}}

test tableview.2 {tableview .t} {
    list [catch {blt::tableview .t} msg] $msg
} {0 .t}

test tableview.3 {.t no args} {
    list [catch {.t} msg] $msg
} {1 {wrong # args: should be one of...
  .t activate cellName
  .t bbox cellName ?switches ...?
  .t bind cellName ?sequence command?
  .t cell args
  .t cget option
  .t column oper args
  .t configure ?option value ...?
  .t curselection 
  .t deactivate 
  .t filter args
  .t find expr
  .t focus ?cellName?
  .t grab ?cellName?
  .t highlight cellName
  .t identify cellName x y
  .t index cellName
  .t inside cellName x y
  .t invoke cellName
  .t ishidden cellName
  .t row oper args
  .t scan dragto|mark x y
  .t see cellName
  .t selection oper args
  .t sort args
  .t style args
  .t type cellName
  .t unhighlight cellName
  .t updates ?bool?
  .t writable cellName
  .t xview ?moveto fract? ?scroll number what?
  .t yview ?moveto fract? ?scroll number what?}}

test tableview.4 {.t activate (missing arg)} {
    list [catch {.t activate} msg] $msg
} {1 {wrong # args: should be ".t activate cellName"}}

test tableview.5 {.t activate badCellName)} {
    list [catch {.t activate badCellName} msg] $msg
} {0 {}}

test tableview.6 {.t bbox (missing arg)} {
    list [catch {.t bbox} msg] $msg
} {1 {wrong # args: should be ".t bbox cellName ?switches ...?"}}

test tableview.7 {.t bbox badCellName)} {
    list [catch {.t bbox badCellName} msg] $msg
} {0 {}}

test tableview.8 {.t bind (missing arg)} {
    list [catch {.t bind} msg] $msg
} {1 {wrong # args: should be ".t bind cellName ?sequence command?"}}

test tableview.9 {.t bind badCellName)} {
    list [catch {.t bind badCellName} msg] $msg
} {0 {}}

test tableview.10 {.t cell)} {
    list [catch {.t cell} msg] $msg
} {1 {wrong # args: should be one of...
  .t cell activate ?cellName?
  .t cell bbox cellName ?switches ...?
  .t cell cget cellName option
  .t cell configure cellName ?option value ...?
  .t cell deactivate 
  .t cell focus ?cellName?
  .t cell identify cellName x y
  .t cell index cellName
  .t cell invoke cellName
  .t cell see cellName
  .t cell style cellName
  .t cell writable cellName}}

test tableview.11 {.t cell activate (missing arg)} {
    list [catch {.t cell activate} msg] $msg
} {0 {}}

test tableview.12 {.t cell activate badCellName)} {
    list [catch {.t cell activate badCellName} msg] $msg
} {0 {}}

test tableview.13 {.t cell bbox (missing arg)} {
    list [catch {.t cell activate} msg] $msg
} {0 {}}

test tableview.14 {.t cell bbox badCellName)} {
    list [catch {.t cell activate badCellName} msg] $msg
} {0 {}}

test tableview.15 {.t cell cget)} {
    list [catch {.t cell cget} msg] $msg
} {1 {wrong # args: should be ".t cell cget cellName option"}}

test tableview.16 {.t cell cget badCellName} {
    list [catch {.t cell cget badCellName} msg] $msg
} {1 {wrong # args: should be ".t cell cget cellName option"}}

test tableview.17 {.t cell cget {0 0} -badOption} {
    list [catch {.t cell cget {0 0} -badOption} msg] $msg
} {0 {}}

test tableview.18 {.t cell cget {0 0} -state} {
    list [catch {.t cell cget {0 0} -state} msg] $msg
} {0 {}}

test tableview.19 {.t cell cget)} {
    list [catch {.t cell cget} msg] $msg
} {1 {wrong # args: should be ".t cell cget cellName option"}}

test tableview.20 {.t cell configure} {
    list [catch {.t cell configure} msg] $msg
} {1 {wrong # args: should be ".t cell configure cellName ?option value ...?"}}

test tableview.21 {.t cell configure {0 0} -badOption} {
    list [catch {.t cell configure {0 0} -badOption} msg] $msg
} {0 {}}

test tableview.22 {.t cell configure {0 0} -state} {
    list [catch {.t cell configure {0 0} -state} msg] $msg
} {0 {}}

test tableview.23 {.t cell configure {0 0} -state normal} {
    list [catch {.t cell configure {0 0} -state normal} msg] $msg
} {0 {}}

test tableview.24 {.t cell deactivate} {
    list [catch {.t cell deactivate} msg] $msg
} {0 {}}

test tableview.25 {.t cell deactivate extraArg} {
    list [catch {.t cell deactivate extraArg} msg] $msg
} {1 {wrong # args: should be ".t cell deactivate "}}

test tableview.26 {.t cell focus} {
    list [catch {.t cell focus } msg] $msg
} {0 {}}

test tableview.27 {.t cell focus badCellName} {
    list [catch {.t cell focus badCellName} msg] $msg
} {1 {wrong # elements in cell index "badCellName"}}

test tableview.28 {.t cell focus {badRow badCol}} {
    list [catch {.t cell focus {badRow badCol}} msg] $msg
} {0 {}}

test tableview.29 {.t cell focus {0 0}} {
    list [catch {.t cell focus {0 0}} msg] $msg
} {0 {}}

test tableview.30 {.t cell identify} {
    list [catch {.t cell identify} msg] $msg
} {1 {wrong # args: should be ".t cell identify cellName x y"}}

test tableview.31 {.t cell identify badCellName} {
    list [catch {.t cell identify badCellName} msg] $msg
} {1 {wrong # args: should be ".t cell identify cellName x y"}}

test tableview.32 {.t cell identify badCellName 0 0 } {
    list [catch {.t cell identify badCellName 0 0 } msg] $msg
} {1 {wrong # elements in cell index "badCellName"}}

test tableview.33 {.t cell identify {badRow badCol} 0 0} {
    list [catch {.t cell identify {badRow badCol} 0 0} msg] $msg
} {0 {}}

test tableview.34 {.t cell identify {0 0} 0 0} {
    list [catch {.t cell identify {0 0} 0 0} msg] $msg
} {0 {}}

test tableview.35 {.t cell index} {
    list [catch {.t cell index} msg] $msg
} {1 {wrong # args: should be ".t cell index cellName"}}

test tableview.36 {.t cell index badCellName} {
    list [catch {.t cell index badCellName} msg] $msg
} {0 {}}

test tableview.37 {.t cell index badCellName 0 0 } {
    list [catch {.t cell index badCellName 0 0 } msg] $msg
} {1 {wrong # args: should be ".t cell index cellName"}}

test tableview.38 {.t cell index {badRow badCol}} {
    list [catch {.t cell index {badRow badCol}} msg] $msg
} {0 {}}

test tableview.39 {.t cell index {0 0}} {
    list [catch {.t cell index {0 0}} msg] $msg
} {0 {}}

test tableview.40 {.t cell invoke} {
    list [catch {.t cell invoke} msg] $msg
} {1 {wrong # args: should be ".t cell invoke cellName"}}

test tableview.41 {.t cell invoke badCellName} {
    list [catch {.t cell invoke badCellName} msg] $msg
} {1 {wrong # elements in cell index "badCellName"}}

test tableview.42 {.t cell invoke badCellName 0 0 } {
    list [catch {.t cell invoke badCellName 0 0 } msg] $msg
} {1 {wrong # args: should be ".t cell invoke cellName"}}

test tableview.43 {.t cell invoke {badRow badCol}} {
    list [catch {.t cell invoke {badRow badCol}} msg] $msg
} {0 {}}

test tableview.44 {.t cell invoke {0 0}} {
    list [catch {.t cell invoke {0 0}} msg] $msg
} {0 {}}

test tableview.45 {.t cell see} {
    list [catch {.t cell see} msg] $msg
} {1 {wrong # args: should be ".t cell see cellName"}}

test tableview.46 {.t cell see badCellName} {
    list [catch {.t cell see badCellName} msg] $msg
} {1 {wrong # elements in cell index "badCellName"}}

test tableview.47 {.t cell see badCellName 0 0 } {
    list [catch {.t cell see badCellName 0 0 } msg] $msg
} {1 {wrong # args: should be ".t cell see cellName"}}

test tableview.48 {.t cell see {badRow badCol}} {
    list [catch {.t cell see {badRow badCol}} msg] $msg
} {0 {}}

test tableview.49 {.t cell see {0 0}} {
    list [catch {.t cell see {0 0}} msg] $msg
} {0 {}}

test tableview.50 {.t cell style} {
    list [catch {.t cell style} msg] $msg
} {1 {wrong # args: should be ".t cell style cellName"}}

test tableview.51 {.t cell style badCellName} {
    list [catch {.t cell style badCellName} msg] $msg
} {1 {wrong # elements in cell index "badCellName"}}

test tableview.52 {.t cell style badCellName 0 0 } {
    list [catch {.t cell style badCellName 0 0 } msg] $msg
} {1 {wrong # args: should be ".t cell style cellName"}}

test tableview.53 {.t cell style {badRow badCol}} {
    list [catch {.t cell style {badRow badCol}} msg] $msg
} {0 {}}

test tableview.54 {.t cell style {0 0}} {
    list [catch {.t cell style {0 0}} msg] $msg
} {0 {}}

test tableview.55 {.t cell writable} {
    list [catch {.t cell writable} msg] $msg
} {1 {wrong # args: should be ".t cell writable cellName"}}

test tableview.56 {.t cell writable badCellName} {
    list [catch {.t cell writable badCellName} msg] $msg
} {1 {wrong # elements in cell index "badCellName"}}

test tableview.57 {.t cell writable badCellName 0 0 } {
    list [catch {.t cell writable badCellName 0 0 } msg] $msg
} {1 {wrong # args: should be ".t cell writable cellName"}}

test tableview.58 {.t cell writable {badRow badCol}} {
    list [catch {.t cell writable {badRow badCol}} msg] $msg
} {0 {}}

test tableview.59 {.t cell writable {0 0}} {
    list [catch {.t cell writable {0 0}} msg] $msg
} {0 {}}

test tableview.60 {.t cget} {
    list [catch {.t cget} msg] $msg
} {1 {wrong # args: should be ".t cget option"}}

test tableview.61 {.t cget badOption} {
    list [catch {.t cget badOption} msg] $msg
} {1 {unknown option "badOption"}}

test tableview.62 {.t cget -table extraArg } {
    list [catch {.t cget -table extraArg } msg] $msg
} {1 {wrong # args: should be ".t cget option"}}

test tableview.63 {.t cget -table} {
    list [catch {.t cget -table} msg] $msg
} {0 {}}

test tableview.64 {.t column} {
    list [catch {.t column} msg] $msg
} {1 {wrong # args: should be one of...
  .t column activate colName
  .t column bbox colName ?switches ...?
  .t column bind tagName type ?sequence command?
  .t column cget colName option
  .t column configure colName ?option value ...?
  .t column deactivate 
  .t column delete colName...
  .t column exists colName
  .t column expose ?colName ...?
  .t column find x1 y1 x2 y2
  .t column hide ?colName ...?
  .t column identify colName x y
  .t column index colName
  .t column insert colName pos ?option value ...?
  .t column invoke colName
  .t column move destCol firstCol lastCol ?switches?
  .t column names 
  .t column nearest x
  .t column resize args
  .t column see colName
  .t column show ?colName ...?
  .t column slide args}}

test tableview.65 {.t column badOp} {
    list [catch {.t column badOp} msg] $msg
} {1 {bad operation "badOp": should be one of...
  .t column activate colName
  .t column bbox colName ?switches ...?
  .t column bind tagName type ?sequence command?
  .t column cget colName option
  .t column configure colName ?option value ...?
  .t column deactivate 
  .t column delete colName...
  .t column exists colName
  .t column expose ?colName ...?
  .t column find x1 y1 x2 y2
  .t column hide ?colName ...?
  .t column identify colName x y
  .t column index colName
  .t column insert colName pos ?option value ...?
  .t column invoke colName
  .t column move destCol firstCol lastCol ?switches?
  .t column names 
  .t column nearest x
  .t column resize args
  .t column see colName
  .t column show ?colName ...?
  .t column slide args}}

test tableview.66 {.t column activate } {
    list [catch {.t column activate } msg] $msg
} {1 {wrong # args: should be ".t column activate colName"}}

test tableview.67 {.t column activate badColumn } {
    list [catch {.t column activate badColumn } msg] $msg
} {0 {}}

test tableview.68 {.t column activate 0 } {
    list [catch {.t column activate 0 } msg] $msg
} {0 {}}

test tableview.69 {.t column bbox } {
    list [catch {.t column bbox } msg] $msg
} {1 {wrong # args: should be ".t column bbox colName ?switches ...?"}}

test tableview.70 {.t column bbox badColumn } {
    list [catch {.t column bbox badColumn } msg] $msg
} {0 {}}

test tableview.71 {.t column bbox 0 } {
    list [catch {.t column bbox 0 } msg] $msg
} {0 {}}

test tableview.72 {.t column bbox 0 -root } {
    list [catch {.t column bbox 0 -root } msg] $msg
} {0 {}}

test tableview.73 {.t column bind } {
    list [catch {.t column bind } msg] $msg
} {1 {wrong # args: should be ".t column bind tagName type ?sequence command?"}}

test tableview.74 {.t column bind badColumn } {
    list [catch {.t column bind badColumn } msg] $msg
} {1 {wrong # args: should be ".t column bind tagName type ?sequence command?"}}

test tableview.75 {.t column bind badColumn badType } {
    list [catch {.t column bind badColumn badType } msg] $msg
} {1 {Bad column bind tag type "badType"}}

test tableview.76 {.t column bind badColumn title } {
    list [catch {.t column bind badColumn title } msg] $msg
} {0 {}}

test tableview.77 {.t column bind 0 title } {
    list [catch {.t column bind 0 title } msg] $msg
} {0 {}}

test tableview.78 {.t column cget } {
    list [catch {.t column cget } msg] $msg
} {1 {wrong # args: should be ".t column cget colName option"}}

test tableview.79 {.t column cget badColumn } {
    list [catch {.t column cget badColumn } msg] $msg
} {1 {wrong # args: should be ".t column cget colName option"}}

test tableview.80 {.t column cget badColumn badOption } {
    list [catch {.t column cget badColumn badOption } msg] $msg
} {0 {}}

test tableview.81 {.t column cget 0 -title } {
    list [catch {.t column cget 0 -title } msg] $msg
} {0 {}}

test tableview.82 {.t column configure } {
    list [catch {.t column configure } msg] $msg
} {1 {wrong # args: should be ".t column configure colName ?option value ...?"}}

test tableview.83 {.t column configure badColumn } {
    list [catch {.t column configure badColumn } msg] $msg
} {0 {}}

test tableview.84 {.t column configure badColumn badOption } {
    list [catch {.t column configure badColumn badOption } msg] $msg
} {0 {}}

test tableview.85 {.t column configure 0 -title } {
    list [catch {.t column configure 0 -title } msg] $msg
} {0 {}}

test tableview.86 {.t column deactivate } {
    list [catch {.t column deactivate } msg] $msg
} {0 {}}

test tableview.87 {.t column deactivate extraArg } {
    list [catch {.t column deactivate extraArg } msg] $msg
} {1 {wrong # args: should be ".t column deactivate "}}

test tableview.88 {.t column delete } {
    list [catch {.t column delete } msg] $msg
} {0 {}}

test tableview.89 {.t column delete badColumn } {
    list [catch {.t column delete badColumn } msg] $msg
} {0 {}}

test tableview.90 {.t column delete badColumn badOption } {
    list [catch {.t column delete badColumn badOption } msg] $msg
} {0 {}}

test tableview.91 {.t column exists } {
    list [catch {.t column exists } msg] $msg
} {1 {wrong # args: should be ".t column exists colName"}}

test tableview.92 {.t column exists badColumn } {
    list [catch {.t column exists badColumn } msg] $msg
} {0 0}

test tableview.93 {.t column exists 0 } {
    list [catch {.t column exists 0 } msg] $msg
} {0 0}

test tableview.94 {.t column expose } {
    list [catch {.t column expose } msg] $msg
} {0 {}}

test tableview.95 {.t column expose badColumn } {
    list [catch {.t column expose badColumn } msg] $msg
} {0 {}}

test tableview.96 {.t column expose 0 } {
    list [catch {.t column expose 0 } msg] $msg
} {0 {}}

test tableview.97 {.t column expose } {
    list [catch {.t column expose } msg] $msg
} {0 {}}

test tableview.98 {.t column find } {
    list [catch {.t column find } msg] $msg
} {1 {wrong # args: should be ".t column find x1 y1 x2 y2"}}

test tableview.99 {.t column find 0 0 0 0 } {
    list [catch {.t column find 0 0 0 0 } msg] $msg
} {0 -1}

test tableview.100 {.t column find 0 10000 0 10000 } {
    list [catch {.t column find 0 10000 0 10000 } msg] $msg
} {0 -1}

test tableview.101 {.t column hide } {
    list [catch {.t column hide } msg] $msg
} {0 {}}

test tableview.102 {.t column hide badColumn } {
    list [catch {.t column hide badColumn } msg] $msg
} {0 {}}

test tableview.103 {.t column hide 0 } {
    list [catch {.t column hide 0 } msg] $msg
} {0 {}}

test tableview.104 {.t column hide } {
    list [catch {.t column hide } msg] $msg
} {0 {}}




exit 0




