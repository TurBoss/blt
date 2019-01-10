
package require BLT

set bg [blt::background create linear  \
	    -jitter 3 \
	    -from w -to e \
	    -colorscale log \
	    -lowcolor "grey99"\
	    -highcolor "grey85"\
	    -repeat reversing \
	    -relativeto .controls]
blt::tk::frame .controls -bg $bg -borderwidth 4

option add *BltTkCheckbutton*background $bg
option add *BltTkLabel*background $bg

option add *HighlightThickness 0
set palette amwg_blueyellowred.rgb
set x [blt::vector create]
set y [blt::vector create]
$x linspace -2 2 50
$y linspace -2 2 50
set x2 [blt::vector create]
$x2 expr { $x* $x }
set y2 [blt::vector create]
$y2 expr { $y * $y}
set tmp [blt::vector create] 
set z [blt::vector create] 

foreach  i [$y2 values] {
  $tmp expr {($x * exp(-($i + $x2))) * 1000}
  $z append $tmp
  if 0 {
  foreach  j [$x2 values] k [$x values] {
	set value [expr ($k * exp(-($i + $j))) * 1000]

	lappend z $value
    }
  }
}

set mesh [blt::mesh create regular -y {0 100 50} -x {0 100 50}]

blt::contour .g -highlightthickness 0 -bg white
.g element create myContour -values $z -mesh $mesh 
.g isoline steps 10 -element myContour
.g legend configure -hide yes 
.g axis configure x -tickdirection in  -scale linear
.g axis configure y -tickdirection in  -scale linear
.g axis configure z \
    -palette $palette \
    -colorbarthickness 20 \
    -tickdirection in \
    -scale linear \
    -margin right 

proc UpdateColors {} {
     global usePaletteColors
     if { $usePaletteColors } {
        .g element configure myContour -color palette -fill palette
    } else {
        .g element configure myContour -color black -fill red
    }
}
proc FixPalette {} {
    global palette
    .g axis configure z -palette $palette
}

proc FixSymbols {} {
    global show
    if { $show(symbols) } {
	.g pen configure activeIsoline -symbol circle
    } else {
	.g pen configure activeIsoline -symbol none
    }
}

proc Decreasing {} {
    global decreasing
    .g axis configure z -decreasing $decreasing
}

proc Fix { what } {
    global show

    set bool $show($what)
    .g element configure myContour -show$what $bool
}

proc StartCutline { w x y } {
  variable _cutline 

  $w marker delete cutline_whole
  set coords [.g invtransform $x $y]
  $w marker create text  \
      -text A \
      -coords $coords \
      -anchor e \
      -tags "cutline_whole A"
  set _cutline(x1) $x
  set _cutline(y1) $y
}

proc ContinueCutline { w x y } {
  variable _cutline 

  if { [info exists _cutline(x2)] } {
    set coords [.g invtransform $x $y]
    $w marker configure B -coords $coords
    set coords [$w invtransform $_cutline(x1) $_cutline(y1) $x $y]
    $w marker configure Cutline -coords $coords
  } else {
    set coords [.g invtransform $x $y]
    $w marker create text \
	-text B \
	-coords $coords \
	-anchor w \
	-tags "cutline_whole B"
    set coords [$w invtransform $_cutline(x1) $_cutline(y1) $x $y]
    $w marker create line \
	-coords $coords \
	-linewidth 2 \
	-outline red3 \
	-tags "Cutline cutline_whole"
  }
  set _cutline(x2) $x
  set _cutline(y2) $y
}

proc FinishCutline { w x y } {
  variable _cutline 
  global palette z

  ContinueCutline $w $x $y
  update
  if { ![info exists _cutline(xv)] } {
    set _cutline(xv) [blt::vector create]
  }
  if { ![info exists _cutline(yv)] } {
    set _cutline(yv) [blt::vector create]
  }
  set coords [$w transform $_cutline(x1) $_cutline(y1) \
		  $_cutline(x2) $_cutline(y2)]
  $w element cutline myContour $_cutline(x1) $_cutline(y1) \
      $_cutline(x2) $_cutline(y2) $_cutline(xv) $_cutline(yv)
  $_cutline(xv) sort $_cutline(yv)
  if { ![winfo exists .cutline] } {
    set contour_min [$z min]
    set contour_max [$z max]
    set cutline_min [$_cutline(yv) min]
    set cutline_max [$_cutline(yv) min]
    puts stderr "contour=$contour_min $contour_max cutline=$cutline_min $cutline_max"
    blt::graph .cutline -height 1i
    .cutline legend configure -hide yes
    .cutline element create cutline \
	-symbol none \
	-linewidth 1 \
	-colormap y
    .cutline axis configure y \
	-palette $palette \
	-palettemin [$z min] -palettemax [$z max] \
	-colorbarthickness 20 \
	-loose no
    blt::table . \
	1,0 .cutline -fill x -cspan 1
    Blt_ZoomStack .cutline
  }
  .cutline element configure cutline -x $_cutline(xv) -y $_cutline(yv)
  array unset _cutline
}

array set show {
    boundary 0
    values 0
    symbols 0
    isolines 0
    colormap 0
    symbols 0
    wireframe 0
}

blt::tk::checkbutton .controls.boundary -text "Boundary" -variable show(boundary) \
    -command "Fix boundary"
blt::tk::checkbutton .controls.wireframe -text "Wireframe" -variable show(wireframe) \
    -command "Fix wireframe"
blt::tk::checkbutton .controls.colormap -text "Colormap"  \
    -variable show(colormap) -command "Fix colormap"
blt::tk::checkbutton .controls.isolines -text "Isolines" \
    -variable show(isolines) -command "Fix isolines"
blt::tk::checkbutton .controls.values -text "Values" \
    -variable show(values) -command "Fix values"
blt::tk::checkbutton .controls.symbols -text "Symbols" \
    -variable show(symbols) -command "FixSymbols"
blt::tk::checkbutton .controls.interp -text "Use palette colors" \
    -variable usePaletteColors -command "UpdateColors"
blt::tk::checkbutton .controls.decreasing -text "Decreasing" \
    -variable decreasing -command "Decreasing"
blt::combobutton .controls.palettes \
    -textvariable palette \
    -relief sunken \
    -background white \
    -arrowon yes \
    -menu .controls.palettes.menu 
blt::tk::label .controls.palettesl -text "Palettes" 
blt::combomenu .controls.palettes.menu \
    -background white \
    -textvariable palette \
    -height 200 \
    -yscrollbar .controls.palettes.menu.ybar \
    -xscrollbar .controls.palettes.menu.xbar

blt::tk::scrollbar .controls.palettes.menu.xbar 
blt::tk::scrollbar .controls.palettes.menu.ybar

foreach pal [blt::palette names] {
    set pal [string trim $pal ::]
    lappend palettes $pal
}

.controls.palettes.menu listadd [lsort -dictionary $palettes] -command FixPalette

blt::table .controls \
    0,0 .controls.boundary -anchor w -cspan 2 \
    1,0 .controls.colormap -anchor w -cspan 2\
    2,0 .controls.isolines -anchor w -cspan 2 \
    3,0 .controls.wireframe -anchor w -cspan 2 \
    4,0 .controls.symbols -anchor w -cspan 2 \
    5,0 .controls.values -anchor w -cspan 2 \
    6,0 .controls.interp -anchor w -cspan 2 \
    7,0 .controls.decreasing -anchor w -cspan 2 \
    8,0 .controls.palettesl -anchor w  \
    8,1 .controls.palettes -fill x

blt::table configure .controls r* c1 -resize none
blt::table configure .controls r9 -resize both

blt::table . \
    0,0 .g -fill both \
    0,1 .controls -fill both 


foreach key [array names show] {
    set show($key) [.g element cget myContour -show$key]
}

Blt_ZoomStack .g

.g isoline bind all <Enter> {
    %W isoline deactivate all
    %W isoline activate current
}

.g isoline bind all <Leave> {
    %W isoline deactivate all
}

bind .g <ButtonPress-3> [list StartCutline .g %x %y]
bind .g <B3-Motion> [list ContinueCutline .g %x %y]
bind .g <ButtonRelease-3> [list FinishCutline .g %x %y]

