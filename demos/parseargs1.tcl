
package require BLT

set args [blt::parseargs create]
puts stderr [$args configure]
$args add "debug" -short "-d" -long "-debug" -type int \
	-default 0 -min 0 -max 10 
$args add "verbose" -short "-v" -long "-verbose" -type boolean -default 0 \
    -action store_true -nargs 0
$args set "verbose" "true"
$args set "debug" "9"
puts stderr extra=[$args parse "fred barney -v -debug 10"]
foreach name [$args names] { 
    puts stderr $name=[$args arg configure $name]
}

