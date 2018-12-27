package require BLT

set normalBg [blt::background create linear \
		  -highcolor grey75 -lowcolor grey98 -jitter 3]
blt::palette create myPalette -colorformat name -cdata {
    white lightblue2 green3 yellow red
}
blt::scale .s -orient horizontal \
    -loose no -min 1 -max 1e10 \
    -show value \
    -palette myPalette \
    -title Temperature \
    -scale log \
    -width 5i \
    -bg $normalBg \
    -units "\u00B0C" 

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
