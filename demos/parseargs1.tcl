
package require BLT

set args [blt::parseargs create]
puts stderr [$args configure]
exit 0
$args add "debug" -short "-d" -long "-debug" -type int \
	-default 0 -min 0 -max 10 
$args add "verbose" -short "-v" -long "-verbose" -type boolean -default 0 \
    -action store_true -nargs 0

foreach name [$args names] { 
    puts stderr $name=[$args arg configure $name]
}

$args parse "-v -debug 10"
