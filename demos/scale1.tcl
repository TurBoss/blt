package require BLT

set normalBg [blt::background create linear \
		  -highcolor grey75 -lowcolor grey98 -jitter 3]
blt::palette create myPalette -colorformat name -cdata {
    white lightblue2 green3 yellow red
}
blt::scale .s -orient horizontal \
    -loose no -min -23.3 -max 43.2 \
    -hide grip -show value \
    -palette myPalette \
    -title Temperature \
    -width 5i \
    -bg $normalBg \
    -resolution 0.1 \
    -units "\u00B0C" 

blt::comboeditor .s.editor -exportselection yes
.s configure -edit yes -editor .s.editor

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
after 5000 {
    .s set 23.0
}
