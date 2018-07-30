#!../src/bltwish

package require BLT

# Example of a pareto chart.
#
# The pareto chart mixes line and bar elements in the same graph.
# Each processing operating is represented by a bar element.  The
# total accumulated defects is displayed with a single line element.

blt::barchart .b \
    -title "Defects Found During Inspection" \
    -font {{Sans Serif} 12 Bold} \
    -width 6i \
    -height 3.5i \
    -bg white \
    -plotborderwidth 10 \
    -plotrelief raised \
    -plotpady 0 \
    -invertxy no

blt::table . .b -fill both

#0000cd
#cd0000
#00cd00
#3a5fcd
#cdcd00
#cd1076
#009acd
#00c5cd
#a2b5cd
#7ac5cd
#66cdaa
#a2cd5a
#cd9b9b
#cdba96
#cd3333
#cd6600
#cd8c95
#cd00cd
#9a32cd
#6ca6cd
#9ac0cd
#9bcd9b
#00cd66
#cdc673
#cdad00
#cd5555
#cd853f
#cd7054
#cd5b45
#cd6889
#cd69c9
#551a8b

set data {
    "Spot Weld"		82	#00D6F2 #00ABBE
    "Lathe"		49	#F26D74 #AA4C53
    "Gear Cut"		38	#02F252 #01AA39
    "Drill"		24	#3a5fcd #1E326A
    "Grind"		17	#DEDE00 #7D7D00
    "Lapping"		12	#cd1076 #5E0737
    "Press"		8	#7ac5cd #385C5F
    "De-burr"		4	#66cdaa #3E7D67
    "Packaging"		3	#a2cd5a #6A863B
    "Other"		12	#cd9b9b #906D6D
}

# Create an X-Y graph line element to trace the accumulated defects.
.b line create accum -label "" -symbol none -color red

# Define a bitmap to be used to stipple the background of each bar.
blt::bitmap define pattern1 { {4 4} {01 02 04 08} }

# For each process, create a bar element to display the magnitude.
set count 0
set sum 0
set ydata 0
set xdata 0
set b [blt::paintbrush create color -color orange1 -opacity 70]
set areab [blt::paintbrush create color -color lightblue -opacity 20]
foreach { label value fill outline } $data {
    incr count
    .b element create $label \
	-xdata $count \
	-ydata $value \
	-fill $fill \
	-borderwidth 0 \
	-outline $outline 

    set labels($count) $label
    # Get the total number of defects.
    set sum [expr $value + $sum]
    lappend ydata $sum
    lappend xdata $count
}

# Configure the coordinates of the accumulated defects, 
# now that we know what they are.
.b line configure accum -xdata $xdata -ydata $ydata \
	-areabackground $areab 
.b element lower accum
# Add text markers to label the percentage of total at each point.
foreach x $xdata y $ydata {
    set percent [expr ($y * 100.0) / $sum]
    if { $x == 0 } {
	set text "0%"
	set xoff  16
	set yoff 8
    } else {
	set text [format %.1f $percent] 
	set xoff -16
	set yoff 10
    }
    .b marker create text \
	-coords "$x $y" \
	-text $text \
	-font {Math 9} \
	-fg red4 \
	-anchor c \
	-xoffset $xoff -yoffset $yoff
}

# Display an auxillary y-axis for percentages.
.b axis configure y2 \
    -hide no \
    -min 0.0 \
    -max 100.0 \
    -tickdirection in \
    -title "Percentage" -grid no

# Title the y-axis
.b axis configure y -title "Defects" -grid no \
    -tickdirection in 

# Configure the x-axis to display the process names, instead of numbers.
.b axis configure x \
    -title "Process" \
    -command FormatLabels \
    -showticks no \
    -ticklength 5 \
    -linewidth 1 \
    -tickdirection in \
    -decreasing no \
    -subdivisions 0

proc FormatLabels { widget value } {
    global labels
    set value [expr round($value)]
    if {[info exists labels($value)] } {
	return $labels($value)
    }
    return ""
}

# No legend needed.
.b legend configure -hide no -bd 0

# Configure the grid lines.
.b axis configure x -gridcolor lightblue -grid yes 

Blt_ZoomStack .b
