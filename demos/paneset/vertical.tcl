package require BLT

# Create a vertical paneset.
blt::paneset .ps \
    -height 500 \
    -orient vertical \
    -sashthickness 3 \
    -background red \
    -sashborderwidth 1 \
    -sashrelief sunken \
    -sashpad 1 


# Create windows for paneset. They must be children of the paneset.
option add *Divisions 4
blt::graph .ps.g 
blt::barchart .ps.b 
blt::barchart .ps.b2 

# Add 3 vertical panes
.ps add -window .ps.g -fill both 
.ps add -window .ps.b -fill both 
.ps add -window .ps.b2 -fill both 

focus .ps

blt::table . \
    0,0 .ps -fill both 

