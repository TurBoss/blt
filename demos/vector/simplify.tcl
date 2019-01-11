

package require BLT

set afterId -1

proc UpdateGraph {} {
  global afterId
  after cancel $afterId
  set afterId [after idle Simplify]
}
proc Simplify {} {
  global afterId
  blt::simplify x0 y0 xs ys [.s get mark]
  set afterId -1
}

blt::graph .g \
    -width 700
blt::scale .s \
    -min 0.0 \
    -max 0.5 \
    -command UpdateGraph \
    -hide minarrow \
    -hide maxarrow

blt::table . \
    0,0 .g -fill both \
    1,0 .s -fill x
blt::table configure . c* r* -resize none
blt::table configure . c0 r0* -resize both
blt::vector x0
blt::vector y0
blt::vector xs
blt::vector ys

.g element create simplified \
    -x xs \
    -y ys \
    -pixels 3 \
    -label simplified \
    -color red4 -fill red1 \

.g element create original \
    -x x0 \
    -y y0 \
    -pixels 3 \
    -label original \
    -color green4 -fill green2 \
    -dashes 1

.g axis configure x  -title "radians"
.g axis configure y  -title "sine"
x0 seq -180 360 5
set pi 3.14159265358979323846
set pi1_2 [expr $pi/180.0]
x0 expr {x0 * $pi1_2}
y0 expr {(random(x0)-0.5) * 0.5 + sin(x0) }
.s set 0.25
