package require BLT

set normalBg [blt::background create linear \
		  -highcolor grey75 -lowcolor grey98 -jitter 3]
blt::palette create myPalette -colorformat rgb -cdata {
  255 105 180
  160 35  35
  200 0   0
  255 0   0
  255 110 0
  255 170 0
  255 225 50
  255 255 200
  210 245 255
  160 210 255
  0   191 255
  30  144 255
  65  105 225
  0   0   205
  0   0   150
  130 32  240
}
blt::scale .s -orient horizontal \
    -loose no -min 1 -max 1e10 \
    -show value \
    -palette myPalette \
    -title pH \
    -scale log \
    -width 5i \
    -bg $normalBg 

blt::comboeditor .s.editor -exportselection yes
.s configure -editor .s.editor

blt::table . \
    0,0 .s -fill both

focus .s

if 1 {
bind .s <Enter> {
    %W configure -show "grip value"
}
bind .s <Leave> {
    %W configure -hide "grip value"
}
}
