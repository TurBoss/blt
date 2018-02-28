
package require BLT

if {[info procs test] != "test"} {
    source defs
}

if [file exists ../library] {
    set blt_library ../library
}

#set VERBOSE 1

test parseargs.1 {parseargs no args} {
    list [catch {blt::parseargs} msg] $msg
} {1 {wrong # args: should be one of...
  blt::parseargs create ?parserName?
  blt::parseargs destroy ?parserName ...?
  blt::parseargs exists parserName
  blt::parseargs names ?pattern ...?}}

test parseargs.2 {parseargs create #auto} {
    list [catch {blt::parseargs create #auto} msg] $msg
} {0 ::parseargs0}

test parseargs.3 {parseargs create #auto.suffix} {
    list [catch {blt::parseargs create #auto.suffix} msg] $msg
} {0 ::parseargs0.suffix}

test parseargs.4 {parseargs create prefix.#auto} {
    list [catch {blt::parseargs create prefix.#auto} msg] $msg
} {0 ::prefix.parseargs0}

test parseargs.5 {parseargs create prefix.#auto.suffix} {
    list [catch {blt::parseargs create prefix.#auto.suffix} msg] $msg
} {0 ::prefix.parseargs0.suffix}

test parseargs.6 {parseargs create prefix.#auto.suffix.#auto} {
    list [catch {blt::parseargs create prefix.#auto.suffix.#auto} msg] $msg
} {0 ::prefix.parseargs0.suffix.#auto}

test parseargs.7 {parseargs destroy [parseargs names *parseargs0*]} {
    list [catch {eval blt::parseargs destroy [blt::parseargs names *parseargs0*]} msg] $msg
} {0 {}}

test parseargs.8 {create} {
    list [catch {blt::parseargs create} msg] $msg
} {0 ::parseargs0}

test parseargs.9 {create} {
    list [catch {blt::parseargs create} msg] $msg
} {0 ::parseargs1}

test parseargs.10 {create fred} {
    list [catch {blt::parseargs create fred} msg] $msg
} {0 ::fred}

test parseargs.11 {create fred} {
    list [catch {blt::parseargs create fred} msg] $msg
} {1 {a command "::fred" already exists}}

test parseargs.12 {create if} {
    list [catch {blt::parseargs create if} msg] $msg
} {1 {a command "::if" already exists}}

test parseargs.13 {parseargs create (bad namespace)} {
    list [catch {blt::parseargs create badName::fred} msg] $msg
} {1 {unknown namespace "badName"}}

test parseargs.14 {parseargs create (wrong # args)} {
    list [catch {blt::parseargs create a b} msg] $msg
} {1 {wrong # args: should be "blt::parseargs create ?parserName?"}}

test parseargs.15 {parseargs names} {
    list [catch {blt::parseargs names} msg] [lsort $msg]
} {0 {::fred ::parseargs0 ::parseargs1}}

test parseargs.16 {parseargs names pattern)} {
    list [catch {blt::parseargs names ::parseargs*} msg] [lsort $msg]
} {0 {::parseargs0 ::parseargs1}}

test parseargs.17 {parseargs names badPattern)} {
    list [catch {blt::parseargs names badPattern*} msg] $msg
} {0 {}}

test parseargs.18 {parseargs names pattern arg (wrong # args)} {
    list [catch {blt::parseargs names pattern arg} msg] $msg
} {1 {wrong # args: should be "blt::parseargs names ?pattern ...?"}}

test parseargs.19 {parseargs destroy (no args)} {
    list [catch {blt::parseargs destroy} msg] $msg
} {0 {}}

test parseargs.20 {parseargs destroy badParseargs} {
    list [catch {blt::parseargs destroy badParseargs} msg] $msg
} {1 {can't find a parser named "badParseargs"}}

test parseargs.21 {parseargs destroy fred} {
    list [catch {blt::parseargs destroy fred} msg] $msg
} {0 {}}

test parseargs.22 {parseargs destroy parseargs0 parseargs1} {
    list [catch {blt::parseargs destroy parseargs0 parseargs1} msg] $msg
} {0 {}}

test parseargs.23 {create} {
    list [catch {blt::parseargs create} msg] $msg
} {0 ::parseargs0}

test parseargs.24 {parseargs0} {
    list [catch {parseargs0} msg] $msg
} {1 {wrong # args: should be one of...
  parseargs0 add argName ?switches ...?
  parseargs0 argument argName args...
  parseargs0 cget option
  parseargs0 configure ?value ...?
  parseargs0 currentdb argName
  parseargs0 delete ?argName ...?
  parseargs0 exists argName
  parseargs0 get argName ?defValue?
  parseargs0 help 
  parseargs0 ischanged argName
  parseargs0 names ?pattern ...?
  parseargs0 parse argList ?varName?
  parseargs0 reset 
  parseargs0 restore token
  parseargs0 save token
  parseargs0 set argName value}}

test parseargs.25 {parseargs0 badOp} {
    list [catch {parseargs0 badOp} msg] $msg
} {1 {bad operation "badOp": should be one of...
  parseargs0 add argName ?switches ...?
  parseargs0 argument argName args...
  parseargs0 cget option
  parseargs0 configure ?value ...?
  parseargs0 currentdb argName
  parseargs0 delete ?argName ...?
  parseargs0 exists argName
  parseargs0 get argName ?defValue?
  parseargs0 help 
  parseargs0 ischanged argName
  parseargs0 names ?pattern ...?
  parseargs0 parse argList ?varName?
  parseargs0 reset 
  parseargs0 restore token
  parseargs0 save token
  parseargs0 set argName value}}

test parseargs.26 {parseargs0 add (wrong # args)} {
    list [catch {parseargs0 add} msg] $msg
} {1 {wrong # args: should be "parseargs0 add argName ?switches ...?"}}

test parseargs.27 {parseargs0 add ""} {
    list [catch {parseargs0 add ""} msg] $msg
} {0 {}}

test parseargs.28 {parseargs0 delete ""} {
    list [catch {parseargs0 delete ""} msg] $msg
} {0 {}}

test parseargs.29 {parseargs0 add "newArg"} {
    list [catch {parseargs0 add "newArg"} msg] $msg
} {0 newArg}

test parseargs.30 {parseargs0 names} {
    list [catch {parseargs0 names} msg] [lsort $msg]
} {0 newArg}

test parseargs.31 {parseargs0 add "newArg"} {
    list [catch {parseargs0 add "newArg"} msg] $msg
} {1 {argument "newArg" already exists in the parser.}}

test parseargs.32 {parseargs0 delete "newArg"} {
    list [catch {parseargs0 delete "newArg"} msg] $msg
} {0 {}}

test parseargs.33 {parseargs0 names} {
    list [catch {parseargs0 names} msg] [lsort $msg]
} {0 {}}

test parseargs.34 {parseargs0 add "newArg" -badSwitch } {
    list [catch {parseargs0 add "newArg" -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -action actionName
   -command cmdPrefix
   -choices list
   -current value
   -default defValue
   -description string
   -exclude list
   -help string
   -long longName
   -metavar string
   -max maxValue
   -min minValue
   -nargs number
   -required bool
   -short shortName
   -type typeName
   -variable varName
   -value varName}}

test parseargs.35 {parseargs0 names} {
    list [catch {parseargs0 names} msg] [lsort $msg]
} {0 {}}

test parseargs.36 {parseargs0 add newArg} {
    list [catch {parseargs0 add newArg} msg] $msg
} {0 newArg}

test parseargs.37 {parseargs0 argument} {
    list [catch {parseargs0 argument} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument argName args..."}}

test parseargs.38 {parseargs0 argument badOp} {
    list [catch {parseargs0 argument badOp} msg] $msg
} {1 {bad operation "badOp": should be one of...
  parseargs0 argument cget argName option
  parseargs0 argument configure argName ?value ...?}}

test parseargs.39 {parseargs0 argument cget (missing arg)} {
    list [catch {parseargs0 argument cget} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.40 {parseargs0 argument cget badArg} {
    list [catch {parseargs0 argument cget badArg} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.41 {parseargs0 argument cget newArg} {
    list [catch {parseargs0 argument cget newArg} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.42 {parseargs0 argument cget badArg badOption} {
    list [catch {parseargs0 argument cget badArg badOption} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.43 {parseargs0 argument cget badArg badOption extraArg} {
    list [catch {parseargs0 argument cget badArg badOption extraArg} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.44 {parseargs0 argument cget newArg badOption} {
    list [catch {parseargs0 argument cget newArg badOption} msg] $msg
} {1 {unknown switch "badOption"
The following switches are available:
   -action actionName
   -command cmdPrefix
   -choices list
   -current value
   -default defValue
   -description string
   -exclude list
   -help string
   -long longName
   -metavar string
   -max maxValue
   -min minValue
   -nargs number
   -required bool
   -short shortName
   -type typeName
   -variable varName
   -value varName}}

test parseargs.45 {parseargs0 argument cget newArg -action} {
    list [catch {parseargs0 argument cget newArg -action} msg] $msg
} {0 store}

test parseargs.46 {parseargs0 argument cget newArg -command} {
    list [catch {parseargs0 argument cget newArg -command} msg] $msg
} {0 {}}

test parseargs.47 {parseargs0 argument cget newArg -choices} {
    list [catch {parseargs0 argument cget newArg -choices} msg] $msg
} {0 {}}

test parseargs.48 {parseargs0 argument cget newArg -default} {
    list [catch {parseargs0 argument cget newArg -default} msg] $msg
} {0 {}}

test parseargs.49 {parseargs0 argument cget newArg -description} {
    list [catch {parseargs0 argument cget newArg -description} msg] $msg
} {0 {}}

test parseargs.50 {parseargs0 argument cget newArg -exclude} {
    list [catch {parseargs0 argument cget newArg -exclude} msg] $msg
} {0 {}}

test parseargs.51 {parseargs0 argument cget newArg -help} {
    list [catch {parseargs0 argument cget newArg -help} msg] $msg
} {0 {}}

test parseargs.52 {parseargs0 argument cget newArg -long} {
    list [catch {parseargs0 argument cget newArg -long} msg] $msg
} {0 {}}

test parseargs.53 {parseargs0 argument cget newArg -metavar} {
    list [catch {parseargs0 argument cget newArg -metavar} msg] $msg
} {0 {}}

test parseargs.54 {parseargs0 argument cget newArg -max} {
    list [catch {parseargs0 argument cget newArg -max} msg] $msg
} {0 {}}

test parseargs.55 {parseargs0 argument cget newArg -min} {
    list [catch {parseargs0 argument cget newArg -min} msg] $msg
} {0 {}}

test parseargs.56 {parseargs0 argument cget newArg -nargs} {
    list [catch {parseargs0 argument cget newArg -nargs} msg] $msg
} {0 1}

test parseargs.57 {parseargs0 argument cget newArg -required} {
    list [catch {parseargs0 argument cget newArg -required} msg] $msg
} {0 0}

test parseargs.58 {parseargs0 argument cget newArg -short} {
    list [catch {parseargs0 argument cget newArg -short} msg] $msg
} {0 {}}

test parseargs.59 {parseargs0 argument cget newArg -type} {
    list [catch {parseargs0 argument cget newArg -type} msg] $msg
} {0 string}

test parseargs.60 {parseargs0 argument cget newArg -variable} {
    list [catch {parseargs0 argument cget newArg -variable} msg] $msg
} {0 {}}

test parseargs.61 {parseargs0 argument cget newArg -value} {
    list [catch {parseargs0 argument cget newArg -value} msg] $msg
} {0 {}}

test parseargs.62 {parseargs0 argument configure} {
    list [catch {parseargs0 argument configure} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument configure argName ?value ...?"}}

test parseargs.63 {parseargs0 argument configure badArg} {
    list [catch {parseargs0 argument configure badArg} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.64 {parseargs0 argument configure newArg} {
    list [catch {parseargs0 argument configure newArg} msg] $msg
} {0 {{-action store store} {-command {} {}} {-choices {} {}} {-current {} {}} {-default {} {}} {-description {} {}} {-exclude {} {}} {-help {} {}} {-long {} {}} {-metavar {} {}} {-max {} {}} {-min {} {}} {-nargs 1 1} {-required 0 0} {-short {} {}} {-type string string} {-variable {} {}} {-value {} {}}}}

test parseargs.65 {parseargs0 argument configure badArg badOption} {
    list [catch {parseargs0 argument configure badArg badOption} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.66 {parseargs0 argument configure badArg badOption extraArg} {
    list [catch {parseargs0 argument configure badArg badOption extraArg} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.67 {parseargs0 argument configure newArg badOption} {
    list [catch {parseargs0 argument configure newArg badOption} msg] $msg
} {1 {unknown switch "badOption"
The following switches are available:
   -action actionName
   -command cmdPrefix
   -choices list
   -current value
   -default defValue
   -description string
   -exclude list
   -help string
   -long longName
   -metavar string
   -max maxValue
   -min minValue
   -nargs number
   -required bool
   -short shortName
   -type typeName
   -variable varName
   -value varName}}

test parseargs.68 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store}}

test parseargs.69 {parseargs0 argument configure newArg -action badValue} {
    list [catch {
	parseargs0 argument configure newArg -action badValue
    } msg] $msg
} {1 {unknown action "badValue": should be int, store, append, store_false, or store_true}}

test parseargs.70 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store}}

test parseargs.71 {parseargs0 argument configure newArg -action store} {
    list [catch {parseargs0 argument configure newArg -action store} msg] $msg
} {0 {}}

test parseargs.72 {parseargs0 argument configure newArg -action append} {
    list [catch {parseargs0 argument configure newArg -action append} msg] $msg
} {0 {}}

test parseargs.73 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store append}}

test parseargs.74 {parseargs0 argument configure newArg -action store_false} {
    list [catch {
	parseargs0 argument configure newArg -action store_false
    } msg] $msg
} {0 {}}

test parseargs.75 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store_false}}

test parseargs.76 {parseargs0 argument configure newArg -action store_true} {
    list [catch {
	parseargs0 argument configure newArg -action store_true
    } msg] $msg
} {0 {}}

test parseargs.77 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store_true}}

test parseargs.78 {parseargs0 argument configure newArg -action store} {
    list [catch {
	parseargs0 argument configure newArg -action store
    } msg] $msg
} {0 {}}

test parseargs.79 {parseargs0 argument configure newArg -action store} {
    list [catch {
	parseargs0 argument configure newArg -action store
    } msg] $msg
} {0 {}}

test parseargs.80 {parseargs0 argument configure newArg -action stor} {
    list [catch {
	parseargs0 argument configure newArg -action stor
    } msg] $msg
} {1 {unknown action "stor": should be int, store, append, store_false, or store_true}}

test parseargs.81 {parseargs0 argument configure newArg -action store_} {
    list [catch {
	parseargs0 argument configure newArg -action store_
    } msg] $msg
} {1 {unknown action "store_": should be int, store, append, store_false, or store_true}}

test parseargs.82 {parseargs0 argument configure newArg -action store_t} {
    list [catch {
	parseargs0 argument configure newArg -action store_t
    } msg] $msg
} {0 {}}

test parseargs.83 {parseargs0 argument configure newArg -action a} {
    list [catch {
	parseargs0 argument configure newArg -action a
    } msg] $msg
} {0 {}}

test parseargs.84 {parseargs0 argument configure newArg -action store} {
    list [catch {
	parseargs0 argument configure newArg -action store
    } msg] $msg
} {0 {}}

test parseargs.85 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store}}


test parseargs.86 {parseargs0 argument configure newArg -command} {
    list [catch {parseargs0 argument configure newArg -command} msg] $msg
} {0 {-command {} {}}}

test parseargs.87 {parseargs0 argument configure newArg -command cmdString} {
    list [catch {
	parseargs0 argument configure newArg -command cmdString
    } msg] $msg
} {0 {}}

test parseargs.88 {parseargs0 argument configure newArg -command} {
    list [catch {parseargs0 argument configure newArg -command} msg] $msg
} {0 {-command {} cmdString}}

test parseargs.89 {parseargs0 argument configure newArg -command ""} {
    list [catch {
	parseargs0 argument configure newArg -command ""
    } msg] $msg
} {0 {}}

test parseargs.90 {parseargs0 argument configure newArg -command} {
    list [catch {parseargs0 argument configure newArg -command} msg] $msg
} {0 {-command {} {}}}


test parseargs.91 {parseargs0 argument configure newArg -choices} {
    list [catch {parseargs0 argument configure newArg -choices} msg] $msg
} {0 {-choices {} {}}}

test parseargs.92 {parseargs0 argument configure newArg -choices "a b c"} {
    list [catch {
	parseargs0 argument configure newArg -choices "a b c"
    } msg] $msg
} {0 {}}

test parseargs.93 {parseargs0 argument configure newArg -choices} {
    list [catch {parseargs0 argument configure newArg -choices} msg] $msg
} {0 {-choices {} {a b c}}}

test parseargs.94 {parseargs0 argument configure newArg -choices ""} {
    list [catch {
	parseargs0 argument configure newArg -choices ""
    } msg] $msg
} {0 {}}

test parseargs.95 {parseargs0 argument configure newArg -choices} {
    list [catch {parseargs0 argument configure newArg -choices} msg] $msg
} {0 {-choices {} {}}}

test parseargs.96 {parseargs0 argument configure newArg -default} {
    list [catch {parseargs0 argument configure newArg -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.97 {parseargs0 argument configure newArg -default defValue} {
    list [catch {
	parseargs0 argument configure newArg -default defValue
    } msg] $msg
} {0 {}}

test parseargs.98 {parseargs0 argument configure newArg -default} {
    list [catch {parseargs0 argument configure newArg -default} msg] $msg
} {0 {-default {} defValue}}

test parseargs.99 {parseargs0 argument configure newArg -default ""} {
    list [catch {
	parseargs0 argument configure newArg -default ""
    } msg] $msg
} {0 {}}

test parseargs.100 {parseargs0 argument configure newArg -default} {
    list [catch {parseargs0 argument configure newArg -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.101 {parseargs0 argument configure newArg -description} {
    list [catch {parseargs0 argument configure newArg -description} msg] $msg
} {0 {-description {} {}}}

test parseargs.102 {parseargs0 argument configure newArg -description desc} {
    list [catch {
	parseargs0 argument configure newArg -description desc
    } msg] $msg
} {0 {}}

test parseargs.103 {parseargs0 argument configure newArg -description} {
    list [catch {parseargs0 argument configure newArg -description} msg] $msg
} {0 {-description {} desc}}

test parseargs.104 {parseargs0 argument configure newArg -description ""} {
    list [catch {
	parseargs0 argument configure newArg -description ""
    } msg] $msg
} {0 {}}

test parseargs.105 {parseargs0 argument configure newArg -description} {
    list [catch {parseargs0 argument configure newArg -description} msg] $msg
} {0 {-description {} {}}}

test parseargs.106 {parseargs0 argument configure newArg -description} {
    list [catch {parseargs0 argument configure newArg -description} msg] $msg
} {0 {-description {} {}}}

test parseargs.107 {parseargs0 argument configure newArg -exclude} {
    list [catch {parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} {}}}

test parseargs.108 {parseargs0 argument configure newArg -exclude arg} {
    list [catch {
	parseargs0 argument configure newArg -exclude arg
    } msg] $msg
} {0 {}}

test parseargs.109 {parseargs0 argument configure newArg -exclude} {
    list [catch {parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} arg}}

test parseargs.110 {parseargs0 argument configure newArg -exclude ""} {
    list [catch {
	parseargs0 argument configure newArg -exclude ""
    } msg] $msg
} {0 {}}

test parseargs.111 {parseargs0 argument configure newArg -exclude} {
    list [catch {parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} {}}}

test parseargs.112 {parseargs0 argument configure newArg -exclude} {
    list [catch {parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} {}}}

test parseargs.113 {parseargs0 argument configure newArg -help} {
    list [catch {parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.114 {parseargs0 argument configure newArg -help help} {
    list [catch {
	parseargs0 argument configure newArg -help help
    } msg] $msg
} {0 {}}

test parseargs.115 {parseargs0 argument configure newArg -help} {
    list [catch {parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} help}}

test parseargs.116 {parseargs0 argument configure newArg -help ""} {
    list [catch {
	parseargs0 argument configure newArg -help ""
    } msg] $msg
} {0 {}}

test parseargs.117 {parseargs0 argument configure newArg -help} {
    list [catch {parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.118 {parseargs0 argument configure newArg -help} {
    list [catch {parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.119 {parseargs0 argument configure newArg -long} {
    list [catch {parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} {}}}

test parseargs.120 {parseargs0 argument configure newArg -long --long} {
    list [catch {
	parseargs0 argument configure newArg -long --long
    } msg] $msg
} {0 {}}

test parseargs.121 {parseargs0 argument configure newArg -long} {
    list [catch {parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} --long}}

test parseargs.122 {parseargs0 argument configure newArg -long ""} {
    list [catch {
	parseargs0 argument configure newArg -long ""
    } msg] $msg
} {0 {}}

test parseargs.123 {parseargs0 argument configure newArg -long} {
    list [catch {parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} {}}}

test parseargs.124 {parseargs0 argument configure newArg -long} {
    list [catch {parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} {}}}

test parseargs.125 {parseargs0 argument configure newArg -metavar} {
    list [catch {parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} {}}}

test parseargs.126 {parseargs0 argument configure newArg -metavar VAR} {
    list [catch {
	parseargs0 argument configure newArg -metavar VAR
    } msg] $msg
} {0 {}}

test parseargs.127 {parseargs0 argument configure newArg -metavar} {
    list [catch {parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} VAR}}

test parseargs.128 {parseargs0 argument configure newArg -metavar ""} {
    list [catch {
	parseargs0 argument configure newArg -metavar ""
    } msg] $msg
} {0 {}}

test parseargs.129 {parseargs0 argument configure newArg -metavar} {
    list [catch {parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} {}}}

test parseargs.130 {parseargs0 argument configure newArg -metavar} {
    list [catch {parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} {}}}

test parseargs.131 {parseargs0 argument configure newArg -max} {
    list [catch {parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} {}}}

test parseargs.132 {parseargs0 argument configure newArg -max VAR} {
    list [catch {
	parseargs0 argument configure newArg -max VAR
    } msg] $msg
} {0 {}}

test parseargs.133 {parseargs0 argument configure newArg -max} {
    list [catch {parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} VAR}}

test parseargs.134 {parseargs0 argument configure newArg -max ""} {
    list [catch {
	parseargs0 argument configure newArg -max ""
    } msg] $msg
} {0 {}}

test parseargs.135 {parseargs0 argument configure newArg -max} {
    list [catch {parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} {}}}

test parseargs.136 {parseargs0 argument configure newArg -max} {
    list [catch {parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} {}}}

test parseargs.137 {parseargs0 argument configure newArg -min} {
    list [catch {parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} {}}}

test parseargs.138 {parseargs0 argument configure newArg -min VAR} {
    list [catch {
	parseargs0 argument configure newArg -min VAR
    } msg] $msg
} {0 {}}

test parseargs.139 {parseargs0 argument configure newArg -min} {
    list [catch {parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} VAR}}

test parseargs.140 {parseargs0 argument configure newArg -min ""} {
    list [catch {
	parseargs0 argument configure newArg -min ""
    } msg] $msg
} {0 {}}

test parseargs.141 {parseargs0 argument configure newArg -min} {
    list [catch {parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} {}}}

test parseargs.142 {parseargs0 argument configure newArg -min} {
    list [catch {parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} {}}}


test parseargs.143 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 1}}

test parseargs.144 {parseargs0 argument configure newArg -nargs badNum} {
    list [catch {
	parseargs0 argument configure newArg -nargs badNum
    } msg] $msg
} {1 {invalid nargs "badNum": should be +, ?, *, or number}}

test parseargs.145 {parseargs0 argument configure newArg -nargs -1} {
    list [catch {parseargs0 argument configure newArg -nargs -1} msg] $msg
} {1 {invalid nargs "-1": should be +, ?, *, or number}}

test parseargs.146 {parseargs0 argument configure newArg -nargs ""} {
    list [catch {
	parseargs0 argument configure newArg -nargs ""
    } msg] $msg
} {1 {invalid nargs "": should be +, ?, *, or number}}

test parseargs.147 {parseargs0 argument configure newArg -nargs 0} {
    list [catch {parseargs0 argument configure newArg -nargs 0} msg] $msg
} {0 {}}

test parseargs.148 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 0}}

test parseargs.149 {parseargs0 argument configure newArg -nargs 100} {
    list [catch {parseargs0 argument configure newArg -nargs 100} msg] $msg
} {0 {}}

test parseargs.150 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 100}}

test parseargs.151 {parseargs0 argument configure newArg -nargs ?} {
    list [catch {parseargs0 argument configure newArg -nargs ?} msg] $msg
} {0 {}}

test parseargs.152 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 ?}}

test parseargs.153 {parseargs0 argument configure newArg -nargs +} {
    list [catch {parseargs0 argument configure newArg -nargs +} msg] $msg
} {0 {}}

test parseargs.154 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 +}}

test parseargs.155 {parseargs0 argument configure newArg -nargs *} {
    list [catch {parseargs0 argument configure newArg -nargs *} msg] $msg
} {0 {}}

test parseargs.156 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 *}}

test parseargs.157 {parseargs0 argument configure newArg -nargs 1} {
    list [catch {parseargs0 argument configure newArg -nargs 1} msg] $msg
} {0 {}}

test parseargs.158 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 1}}

test parseargs.159 {parseargs0 argument configure newArg -required badBool} {
    list [catch {
	parseargs0 argument configure newArg -required badBool
    } msg] $msg
} {1 {expected boolean value but got "badBool"}}

test parseargs.160 {parseargs0 argument configure newArg -required 1} {
    list [catch {parseargs0 argument configure newArg -required 1} msg] $msg
} {0 {}}

test parseargs.161 {parseargs0 argument configure newArg -required true} {
    list [catch {parseargs0 argument configure newArg -required true} msg] $msg
} {0 {}}

test parseargs.162 {parseargs0 argument configure newArg -required yes} {
    list [catch {parseargs0 argument configure newArg -required yes} msg] $msg
} {0 {}}

test parseargs.163 {parseargs0 argument configure newArg -required on} {
    list [catch {parseargs0 argument configure newArg -required on} msg] $msg
} {0 {}}

test parseargs.164 {parseargs0 argument configure newArg -required 0} {
    list [catch {parseargs0 argument configure newArg -required 0} msg] $msg
} {0 {}}

test parseargs.165 {parseargs0 argument configure newArg -required false} {
    list [catch {parseargs0 argument configure newArg -required false} msg] $msg
} {0 {}}

test parseargs.166 {parseargs0 argument configure newArg -required no} {
    list [catch {parseargs0 argument configure newArg -required no} msg] $msg
} {0 {}}

test parseargs.167 {parseargs0 argument configure newArg -required off} {
    list [catch {parseargs0 argument configure newArg -required off} msg] $msg
} {0 {}}

test parseargs.168 {parseargs0 argument configure newArg -short} {
    list [catch {parseargs0 argument configure newArg -short} msg] $msg
} {0 {-short {} {}}}

test parseargs.169 {parseargs0 argument configure newArg -short -s} {
    list [catch {
	parseargs0 argument configure newArg -short -s
    } msg] $msg
} {0 {}}

test parseargs.170 {parseargs0 argument configure newArg -short} {
    list [catch {parseargs0 argument configure newArg -short} msg] $msg
} {0 {-short {} -s}}

test parseargs.171 {parseargs0 argument configure newArg -short ""} {
    list [catch {
	parseargs0 argument configure newArg -short ""
    } msg] $msg
} {0 {}}

test parseargs.172 {parseargs0 argument configure newArg -short} {
    list [catch {parseargs0 argument configure newArg -short} msg] $msg
} {0 {-short {} {}}}

test parseargs.173 {parseargs0 argument configure newArg -type} {
    list [catch {parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string string}}

test parseargs.174 {parseargs0 argument configure newArg -type badType} {
    list [catch {parseargs0 argument configure newArg -type badType} msg] $msg
} {1 {unknown argument type "badType": should be int, double, string, or boolean}}

test parseargs.175 {parseargs0 argument configure newArg -type int} {
    list [catch {parseargs0 argument configure newArg -type int} msg] $msg
} {0 {}}

test parseargs.176 {parseargs0 argument configure newArg -type} {
    list [catch {parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string int}}

test parseargs.177 {parseargs0 argument configure newArg -type double} {
    list [catch {parseargs0 argument configure newArg -type double} msg] $msg
} {0 {}}

test parseargs.178 {parseargs0 argument configure newArg -type float} {
    list [catch {parseargs0 argument configure newArg -type float} msg] $msg
} {0 {}}

test parseargs.179 {parseargs0 argument configure newArg -type number} {
    list [catch {parseargs0 argument configure newArg -type number} msg] $msg
} {0 {}}

test parseargs.180 {parseargs0 argument configure newArg -type boolean} {
    list [catch {parseargs0 argument configure newArg -type boolean} msg] $msg
} {0 {}}

test parseargs.181 {parseargs0 argument configure newArg -type} {
    list [catch {parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string boolean}}

test parseargs.182 {parseargs0 argument configure newArg -type string} {
    list [catch {parseargs0 argument configure newArg -type string} msg] $msg
} {0 {}}

test parseargs.183 {parseargs0 argument configure newArg -type} {
    list [catch {parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string string}}

test parseargs.184 {parseargs0 argument configure newArg -variable} {
    list [catch {parseargs0 argument configure newArg -variable} msg] $msg
} {0 {-variable {} {}}}

test parseargs.185 {parseargs0 argument configure newArg -value} {
    list [catch {parseargs0 argument configure newArg -value} msg] $msg
} {0 {-value {} {}}}

exit 0

test parseargs.186 {parseargs0 insert 0} {
    list [catch {parseargs0 insert 0} msg] $msg
} {0 1}

test parseargs.187 {parseargs0 insert 0} {
    list [catch {parseargs0 insert 0} msg] $msg
} {0 2}

test parseargs.188 {parseargs0 insert root} {
    list [catch {parseargs0 insert root} msg] $msg
} {0 3}

test parseargs.189 {parseargs0 insert all} {
    list [catch {parseargs0 insert all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.190 {parseargs0 insert 0 -at badPosition} {
    list [catch {parseargs0 insert 0 -at badPosition} msg] $msg
} {1 {expected integer but got "badPosition"}}

test parseargs.191 {parseargs0 insert 0 -at -1} {
    list [catch {parseargs0 insert 0 -at -1} msg] $msg
} {1 {bad value "-1": can't be negative}}

test parseargs.192 {parseargs0 insert 0 -at 1000} {
    list [catch {parseargs0 insert 0 -at 1000} msg] $msg
} {0 4}

test parseargs.193 {parseargs0 insert 0 -at (no arg)} {
    list [catch {parseargs0 insert 0 -at} msg] $msg
} {1 {value for "-at" missing}}

test parseargs.194 {parseargs0 insert 0 -tags myTag} {
    list [catch {parseargs0 insert 0 -tags myTag} msg] $msg
} {0 5}

test parseargs.195 {parseargs0 insert 0 -tags {myTag1 myTag2} } {
    list [catch {parseargs0 insert 0 -tags {myTag1 myTag2}} msg] $msg
} {0 6}

test parseargs.196 {parseargs0 insert 0 -tags root} {
    list [catch {parseargs0 insert 0 -tags root} msg] $msg
} {1 {can't add reserved tag "root"}}

test parseargs.197 {parseargs0 insert 0 -tags (missing arg)} {
    list [catch {parseargs0 insert 0 -tags} msg] $msg
} {1 {value for "-tags" missing}}

test parseargs.198 {parseargs0 insert 0 -label myLabel -tags thisTag} {
    list [catch {parseargs0 insert 0 -label myLabel -tags thisTag} msg] $msg
} {0 8}

test parseargs.199 {parseargs0 insert 0 -label (missing arg)} {
    list [catch {parseargs0 insert 0 -label} msg] $msg
} {1 {value for "-label" missing}}

test parseargs.200 {parseargs0 insert 1 -tags thisTag} {
    list [catch {parseargs0 insert 1 -tags thisTag} msg] $msg
} {0 9}

test parseargs.201 {parseargs0 insert 1 -data key (missing value)} {
    list [catch {parseargs0 insert 1 -data key} msg] $msg
} {1 {missing value for "key"}}

test parseargs.202 {parseargs0 insert 1 -data {key value}} {
    list [catch {parseargs0 insert 1 -data {key value}} msg] $msg
} {0 11}

test parseargs.203 {parseargs0 insert 1 -data {key1 value1 key2 value2}} {
    list [catch {parseargs0 insert 1 -data {key1 value1 key2 value2}} msg] $msg
} {0 12}

test parseargs.204 {get} {
    list [catch {
	parseargs0 get 12
    } msg] $msg
} {0 {key1 value1 key2 value2}}

test parseargs.205 {parseargs0 children} {
    list [catch {parseargs0 children} msg] $msg
} {1 {wrong # args: should be "parseargs0 children nodeName ?switches ...?"}}

test parseargs.206 {parseargs0 children 0} {
    list [catch {parseargs0 children 0} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test parseargs.207 {parseargs0 children root} {
    list [catch {parseargs0 children root} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test parseargs.208 {parseargs0 children 1} {
    list [catch {parseargs0 children 1} msg] $msg
} {0 {9 11 12}}

test parseargs.209 {parseargs0 insert myTag} {
    list [catch {parseargs0 insert myTag} msg] $msg
} {0 13}

test parseargs.210 {parseargs0 index myTag} {
    list [catch {parseargs0 index myTag} msg] $msg
} {0 5}

test parseargs.211 {parseargs0 children 5} {
    list [catch {parseargs0 children 5} msg] $msg
} {0 13}

test parseargs.212 {parseargs0 children -1 -nocomplain} {
    list [catch {parseargs0 children -1 -nocomplain} msg] $msg
} {0 {}}

test parseargs.213 {parseargs0 children badNode -nocomplain} {
    list [catch {parseargs0 children badNode -nocomplain} msg] $msg
} {0 {}}

test parseargs.214 {parseargs0 children myTag} {
    list [catch {parseargs0 children myTag} msg] $msg
} {0 13}

test parseargs.215 {parseargs0 children root -from 0 -to end} {
    list [catch {parseargs0 children root -from 0 -to end} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test parseargs.216 {parseargs0 children root -from 2} {
    list [catch {parseargs0 children root -from 2} msg] $msg
} {0 {3 4 5 6 8}}

test parseargs.217 {parseargs0 children root -from 2 -to end} {
    list [catch {parseargs0 children root -from 2 -to end} msg] $msg
} {0 {3 4 5 6 8}}

test parseargs.218 {parseargs0 children root -from end -to end} {
    list [catch {parseargs0 children root -from end -to end} msg] $msg
} {0 8}

test parseargs.219 {parseargs0 children root -from end} {
    list [catch {parseargs0 children root -from end} msg] $msg
} {0 8}

test parseargs.220 {parseargs0 children root -from 0 -to 2} {
    list [catch {parseargs0 children root -from 0 -to 2} msg] $msg
} {0 {1 2 3}}

test parseargs.221 {parseargs0 children root -from -1 -to -20} {
    list [catch {parseargs0 children root -from -1 -to -20} msg] $msg
} {1 {bad position "-1": can't be negative.}}

test parseargs.222 {parseargs0 firstchild (missing arg)} {
    list [catch {parseargs0 firstchild} msg] $msg
} {1 {wrong # args: should be "parseargs0 firstchild nodeName"}}

test parseargs.223 {parseargs0 firstchild root} {
    list [catch {parseargs0 firstchild root} msg] $msg
} {0 1}

test parseargs.224 {parseargs0 lastchild (missing arg)} {
    list [catch {parseargs0 lastchild} msg] $msg
} {1 {wrong # args: should be "parseargs0 lastchild nodeName"}}

test parseargs.225 {parseargs0 lastchild root} {
    list [catch {parseargs0 lastchild root} msg] $msg
} {0 8}

test parseargs.226 {parseargs0 nextsibling (missing arg)} {
    list [catch {parseargs0 nextsibling} msg] $msg
} {1 {wrong # args: should be "parseargs0 nextsibling nodeName"}}

test parseargs.227 {parseargs0 nextsibling 1)} {
    list [catch {parseargs0 nextsibling 1} msg] $msg
} {0 2}

test parseargs.228 {parseargs0 nextsibling 2)} {
    list [catch {parseargs0 nextsibling 2} msg] $msg
} {0 3}

test parseargs.229 {parseargs0 nextsibling 3)} {
    list [catch {parseargs0 nextsibling 3} msg] $msg
} {0 4}

test parseargs.230 {parseargs0 nextsibling 4)} {
    list [catch {parseargs0 nextsibling 4} msg] $msg
} {0 5}

test parseargs.231 {parseargs0 nextsibling 5)} {
    list [catch {parseargs0 nextsibling 5} msg] $msg
} {0 6}

test parseargs.232 {parseargs0 nextsibling 6)} {
    list [catch {parseargs0 nextsibling 6} msg] $msg
} {0 8}

test parseargs.233 {parseargs0 nextsibling 8)} {
    list [catch {parseargs0 nextsibling 8} msg] $msg
} {0 -1}

test parseargs.234 {parseargs0 nextsibling all)} {
    list [catch {parseargs0 nextsibling all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.235 {parseargs0 nextsibling badTag)} {
    list [catch {parseargs0 nextsibling badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::parseargs0}}

test parseargs.236 {parseargs0 nextsibling -1)} {
    list [catch {parseargs0 nextsibling -1} msg] $msg
} {1 {can't find tag or id "-1" in ::parseargs0}}

test parseargs.237 {parseargs0 prevsibling 2)} {
    list [catch {parseargs0 prevsibling 2} msg] $msg
} {0 1}

test parseargs.238 {parseargs0 prevsibling 1)} {
    list [catch {parseargs0 prevsibling 1} msg] $msg
} {0 -1}

test parseargs.239 {parseargs0 prevsibling -1)} {
    list [catch {parseargs0 prevsibling -1} msg] $msg
} {1 {can't find tag or id "-1" in ::parseargs0}}

test parseargs.240 {parseargs0 root)} {
    list [catch {parseargs0 root} msg] $msg
} {0 0}

test parseargs.241 {parseargs0 root badArg)} {
    list [catch {parseargs0 root badArgs} msg] $msg
} {1 {wrong # args: should be "parseargs0 root "}}

test parseargs.242 {parseargs0 parent (missing arg))} {
    list [catch {parseargs0 parent} msg] $msg
} {1 {wrong # args: should be "parseargs0 parent nodeName"}}

test parseargs.243 {parseargs0 parent root)} {
    list [catch {parseargs0 parent root} msg] $msg
} {0 -1}

test parseargs.244 {parseargs0 parent 1)} {
    list [catch {parseargs0 parent 1} msg] $msg
} {0 0}

test parseargs.245 {parseargs0 parent myTag)} {
    list [catch {parseargs0 parent myTag} msg] $msg
} {0 0}

test parseargs.246 {parseargs0 next (missing arg))} {
    list [catch {parseargs0 next} msg] $msg
} {1 {wrong # args: should be "parseargs0 next nodeName"}}


test parseargs.247 {parseargs0 next (extra arg))} {
    list [catch {parseargs0 next root root} msg] $msg
} {1 {wrong # args: should be "parseargs0 next nodeName"}}

test parseargs.248 {parseargs0 next root} {
    list [catch {parseargs0 next root} msg] $msg
} {0 1}

test parseargs.249 {parseargs0 next 1)} {
    list [catch {parseargs0 next 1} msg] $msg
} {0 9}

test parseargs.250 {parseargs0 next 2)} {
    list [catch {parseargs0 next 2} msg] $msg
} {0 3}

test parseargs.251 {parseargs0 next 3)} {
    list [catch {parseargs0 next 3} msg] $msg
} {0 4}

test parseargs.252 {parseargs0 next 4)} {
    list [catch {parseargs0 next 4} msg] $msg
} {0 5}

test parseargs.253 {parseargs0 next 5)} {
    list [catch {parseargs0 next 5} msg] $msg
} {0 13}

test parseargs.254 {parseargs0 next 6)} {
    list [catch {parseargs0 next 6} msg] $msg
} {0 8}

test parseargs.255 {parseargs0 next 8)} {
    list [catch {parseargs0 next 8} msg] $msg
} {0 -1}

test parseargs.256 {parseargs0 previous 1)} {
    list [catch {parseargs0 previous 1} msg] $msg
} {0 0}

test parseargs.257 {parseargs0 previous 0)} {
    list [catch {parseargs0 previous 0} msg] $msg
} {0 -1}

test parseargs.258 {parseargs0 previous 8)} {
    list [catch {parseargs0 previous 8} msg] $msg
} {0 6}

test parseargs.259 {parseargs0 depth (no arg))} {
    list [catch {parseargs0 depth} msg] $msg
} {0 2}

test parseargs.260 {parseargs0 depth root))} {
    list [catch {parseargs0 depth root} msg] $msg
} {0 0}

test parseargs.261 {parseargs0 depth myTag))} {
    list [catch {parseargs0 depth myTag} msg] $msg
} {0 1}

test parseargs.262 {parseargs0 depth myTag))} {
    list [catch {parseargs0 depth myTag} msg] $msg
} {0 1}

test parseargs.263 {parseargs0 dump (missing arg)))} {
    list [catch {parseargs0 dump} msg] $msg
} {1 {wrong # args: should be "parseargs0 dump nodeName ?switches ...?"}}

test parseargs.264 {parseargs0 dump root -version 2.0} {
    list [catch {parseargs0 dump root -version 2.0} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} node1} {} {}
1 9 {{} node1 node9} {} {thisTag}
1 11 {{} node1 node11} {key value} {}
1 12 {{} node1 node12} {key1 value1 key2 value2} {}
0 2 {{} node2} {} {}
0 3 {{} node3} {} {}
0 4 {{} node4} {} {}
0 5 {{} node5} {} {myTag}
5 13 {{} node5 node13} {} {}
0 6 {{} node6} {} {myTag2 myTag1}
0 8 {{} myLabel} {} {thisTag}
}}

test parseargs.265 {parseargs0 dump 1 -version 2.0} {
    list [catch {parseargs0 dump 1 -version 2.0} msg] $msg
} {0 {# V2.0
-1 1 {node1} {} {}
1 9 {node1 node9} {} {thisTag}
1 11 {node1 node11} {key value} {}
1 12 {node1 node12} {key1 value1 key2 value2} {}
}}

test parseargs.266 {parseargs0 dump this -version 2.0} {
    list [catch {parseargs0 dump myTag -version 2.0} msg] $msg
} {0 {# V2.0
-1 5 {node5} {} {myTag}
5 13 {node5 node13} {} {}
}}

test parseargs.267 {parseargs0 dump 1 badSwitch} {
    list [catch {parseargs0 dump 1 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -data data
   -file fileName
   -version versionNum
   -notags }}

test parseargs.268 {parseargs0 dump 11 -version 2.0} {
    list [catch {parseargs0 dump 11 -version 2.0} msg] $msg
} {0 {# V2.0
-1 11 {node11} {key value} {}
}}

test parseargs.269 {parseargs0 dump all} {
    list [catch {parseargs0 dump all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.270 {parseargs0 dump all} {
    list [catch {parseargs0 dump all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.271 {parseargs0 dump 0 -file test.dump} {
    list [catch {parseargs0 dump 0 -file test.dump} msg] $msg
} {0 {}}

test parseargs.272 {parseargs0 get 9} {
    list [catch {parseargs0 get 9} msg] $msg
} {0 {}}

test parseargs.273 {parseargs0 get all} {
    list [catch {parseargs0 get all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.274 {parseargs0 get root} {
    list [catch {parseargs0 get root} msg] $msg
} {0 {}}

test parseargs.275 {parseargs0 get 9 key} {
    list [catch {parseargs0 get root} msg] $msg
} {0 {}}

test parseargs.276 {parseargs0 get 12} {
    list [catch {parseargs0 get 12} msg] $msg
} {0 {key1 value1 key2 value2}}

test parseargs.277 {parseargs0 get 12 key1} {
    list [catch {parseargs0 get 12 key1} msg] $msg
} {0 value1}

test parseargs.278 {parseargs0 get 12 key2} {
    list [catch {parseargs0 get 12 key2} msg] $msg
} {0 value2}

test parseargs.279 {parseargs0 get 12 key1 defValue } {
    list [catch {parseargs0 get 12 key1 defValue} msg] $msg
} {0 value1}

test parseargs.280 {parseargs0 get 12 key100 defValue } {
    list [catch {parseargs0 get 12 key100 defValue} msg] $msg
} {0 defValue}

test parseargs.281 {parseargs0 index (missing arg) } {
    list [catch {parseargs0 index} msg] $msg
} {1 {wrong # args: should be "parseargs0 index label|list"}}

test parseargs.282 {parseargs0 index 0 10 (extra arg) } {
    list [catch {parseargs0 index 0 10} msg] $msg
} {1 {wrong # args: should be "parseargs0 index label|list"}}

test parseargs.283 {parseargs0 index 0} {
    list [catch {parseargs0 index 0} msg] $msg
} {0 0}

test parseargs.284 {parseargs0 index root} {
    list [catch {parseargs0 index root} msg] $msg
} {0 0}

test parseargs.285 {parseargs0 index all} {
    list [catch {parseargs0 index all} msg] $msg
} {0 -1}

test parseargs.286 {parseargs0 index myTag} {
    list [catch {parseargs0 index myTag} msg] $msg
} {0 5}

test parseargs.287 {parseargs0 index thisTag} {
    list [catch {parseargs0 index thisTag} msg] $msg
} {0 -1}

test parseargs.288 {parseargs0 is (no args)} {
    list [catch {parseargs0 is} msg] $msg
} {1 {ambiguous operation "is" matches:  isancestor isbefore isleaf isroot}}

test parseargs.289 {parseargs0 isbefore} {
    list [catch {parseargs0 isbefore} msg] $msg
} {1 {wrong # args: should be "parseargs0 isbefore node1 node2"}}

test parseargs.290 {parseargs0 isbefore 0 10 20} {
    list [catch {parseargs0 isbefore 0 10 20} msg] $msg
} {1 {wrong # args: should be "parseargs0 isbefore node1 node2"}}

test parseargs.291 {parseargs0 isbefore 0 12} {
    list [catch {parseargs0 isbefore 0 12} msg] $msg
} {0 1}

test parseargs.292 {parseargs0 isbefore 12 0} {
    list [catch {parseargs0 isbefore 12 0} msg] $msg
} {0 0}

test parseargs.293 {parseargs0 isbefore 0 0} {
    list [catch {parseargs0 isbefore 0 0} msg] $msg
} {0 0}

test parseargs.294 {parseargs0 isbefore root 0} {
    list [catch {parseargs0 isbefore root 0} msg] $msg
} {0 0}

test parseargs.295 {parseargs0 isbefore 0 all} {
    list [catch {parseargs0 isbefore 0 all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.296 {parseargs0 isancestor} {
    list [catch {parseargs0 isancestor} msg] $msg
} {1 {wrong # args: should be "parseargs0 isancestor node1 node2"}}

test parseargs.297 {parseargs0 isancestor 0 12 20} {
    list [catch {parseargs0 isancestor 0 12 20} msg] $msg
} {1 {wrong # args: should be "parseargs0 isancestor node1 node2"}}

test parseargs.298 {parseargs0 isancestor 0 12} {
    list [catch {parseargs0 isancestor 0 12} msg] $msg
} {0 1}

test parseargs.299 {parseargs0 isancestor 12 0} {
    list [catch {parseargs0 isancestor 12 0} msg] $msg
} {0 0}

test parseargs.300 {parseargs0 isancestor 1 2} {
    list [catch {parseargs0 isancestor 1 2} msg] $msg
} {0 0}

test parseargs.301 {parseargs0 isancestor root 0} {
    list [catch {parseargs0 isancestor root 0} msg] $msg
} {0 0}

test parseargs.302 {parseargs0 isancestor 0 all} {
    list [catch {parseargs0 isancestor 0 all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.303 {parseargs0 isroot (missing arg)} {
    list [catch {parseargs0 isroot} msg] $msg
} {1 {wrong # args: should be "parseargs0 isroot nodeName"}}

test parseargs.304 {parseargs0 isroot 0 20 (extra arg)} {
    list [catch {parseargs0 isroot 0 20} msg] $msg
} {1 {wrong # args: should be "parseargs0 isroot nodeName"}}

test parseargs.305 {parseargs0 isroot 0} {
    list [catch {parseargs0 isroot 0} msg] $msg
} {0 1}

test parseargs.306 {parseargs0 isroot 12} {
    list [catch {parseargs0 isroot 12} msg] $msg
} {0 0}

test parseargs.307 {parseargs0 isroot 1} {
    list [catch {parseargs0 isroot 1} msg] $msg
} {0 0}

test parseargs.308 {parseargs0 isroot root} {
    list [catch {parseargs0 isroot root} msg] $msg
} {0 1}

test parseargs.309 {parseargs0 isroot all} {
    list [catch {parseargs0 isroot all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.310 {parseargs0 isleaf (missing arg)} {
    list [catch {parseargs0 isleaf} msg] $msg
} {1 {wrong # args: should be "parseargs0 isleaf nodeName"}}

test parseargs.311 {parseargs0 isleaf 0 20 (extra arg)} {
    list [catch {parseargs0 isleaf 0 20} msg] $msg
} {1 {wrong # args: should be "parseargs0 isleaf nodeName"}}

test parseargs.312 {parseargs0 isleaf 0} {
    list [catch {parseargs0 isleaf 0} msg] $msg
} {0 0}

test parseargs.313 {parseargs0 isleaf 12} {
    list [catch {parseargs0 isleaf 12} msg] $msg
} {0 1}

test parseargs.314 {parseargs0 isleaf 1} {
    list [catch {parseargs0 isleaf 1} msg] $msg
} {0 0}

test parseargs.315 {parseargs0 isleaf root} {
    list [catch {parseargs0 isleaf root} msg] $msg
} {0 0}

test parseargs.316 {parseargs0 isleaf all} {
    list [catch {parseargs0 isleaf all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.317 {parseargs0 isleaf 1000} {
    list [catch {parseargs0 isleaf 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::parseargs0}}

test parseargs.318 {parseargs0 isleaf badTag} {
    list [catch {parseargs0 isleaf badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::parseargs0}}

test parseargs.319 {parseargs0 set (missing arg)} {
    list [catch {parseargs0 set} msg] $msg
} {1 {wrong # args: should be "parseargs0 set nodeName ?fieldName value ...?"}}

test parseargs.320 {parseargs0 set 0 (missing arg)} {
    list [catch {parseargs0 set 0} msg] $msg
} {0 {}}

test parseargs.321 {parseargs0 set 0 key (missing arg)} {
    list [catch {parseargs0 set 0 key} msg] $msg
} {1 {missing value for field "key"}}

test parseargs.322 {parseargs0 set 0 key value} {
    list [catch {parseargs0 set 0 key value} msg] $msg
} {0 {}}

test parseargs.323 {parseargs0 set 0 key1 value1 key2 value2 key3 value3} {
    list [catch {parseargs0 set 0 key1 value1 key2 value2 key3 value3} msg] $msg
} {0 {}}

test parseargs.324 {parseargs0 set 0 key1 value1 key2 (missing arg)} {
    list [catch {parseargs0 set 0 key1 value1 key2} msg] $msg
} {1 {missing value for field "key2"}}

test parseargs.325 {parseargs0 set 0 key value} {
    list [catch {parseargs0 set 0 key value} msg] $msg
} {0 {}}

test parseargs.326 {parseargs0 set 0 key1 value1 key2 (missing arg)} {
    list [catch {parseargs0 set 0 key1 value1 key2} msg] $msg
} {1 {missing value for field "key2"}}

test parseargs.327 {parseargs0 set all} {
    list [catch {parseargs0 set all} msg] $msg
} {0 {}}

test parseargs.328 {parseargs0 set all abc 123} {
    list [catch {parseargs0 set all abc 123} msg] $msg
} {0 {}}

test parseargs.329 {parseargs0 set root} {
    list [catch {parseargs0 set root} msg] $msg
} {0 {}}

test parseargs.330 {parseargs0 restore stuff} {
    list [catch {
	set data [parseargs0 dump root -version 2.0]
	blt::parseargs create
	parseargs1 restore root -data $data
	set data [parseargs1 dump root -version 2.0]
	blt::parseargs destroy parseargs1
	set data
	} msg] $msg
} {0 {# V2.0
-1 0 {{}} {key value key1 value1 key2 value2 key3 value3 abc 123} {}
0 1 {{} node1} {abc 123} {}
1 9 {{} node1 node9} {abc 123} {thisTag}
1 11 {{} node1 node11} {key value abc 123} {}
1 12 {{} node1 node12} {key1 value1 key2 value2 abc 123} {}
0 2 {{} node2} {abc 123} {}
0 3 {{} node3} {abc 123} {}
0 4 {{} node4} {abc 123} {}
0 5 {{} node5} {abc 123} {myTag}
5 13 {{} node5 node13} {abc 123} {}
0 6 {{} node6} {abc 123} {myTag2 myTag1}
0 8 {{} myLabel} {abc 123} {thisTag}
}}

test parseargs.331 {parseargs0 restore 0 -file test.dump} {
    list [catch {
	blt::parseargs create
	parseargs1 restore root -file test.dump
	set data [parseargs1 dump root -version 2.0]
	blt::parseargs destroy parseargs1
	file delete test.dump
	set data
	} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} node1} {} {}
1 9 {{} node1 node9} {} {thisTag}
1 11 {{} node1 node11} {key value} {}
1 12 {{} node1 node12} {key1 value1 key2 value2} {}
0 2 {{} node2} {} {}
0 3 {{} node3} {} {}
0 4 {{} node4} {} {}
0 5 {{} node5} {} {myTag}
5 13 {{} node5 node13} {} {}
0 6 {{} node6} {} {myTag2 myTag1}
0 8 {{} myLabel} {} {thisTag}
}}


test parseargs.332 {parseargs0 unset 0 key1} {
    list [catch {parseargs0 unset 0 key1} msg] $msg
} {0 {}}

test parseargs.333 {parseargs0 get 0} {
    list [catch {parseargs0 get 0} msg] $msg
} {0 {key value key2 value2 key3 value3 abc 123}}

test parseargs.334 {parseargs0 unset 0 key2 key3} {
    list [catch {parseargs0 unset 0 key2 key3} msg] $msg
} {0 {}}

test parseargs.335 {parseargs0 get 0} {
    list [catch {parseargs0 get 0} msg] $msg
} {0 {key value abc 123}}

test parseargs.336 {parseargs0 unset 0} {
    list [catch {parseargs0 unset 0} msg] $msg
} {0 {}}

test parseargs.337 {parseargs0 get 0} {
    list [catch {parseargs0 get 0} msg] $msg
} {0 {}}

test parseargs.338 {parseargs0 unset all abc} {
    list [catch {parseargs0 unset all abc} msg] $msg
} {0 {}}

test parseargs.339 {parseargs0 restore stuff} {
    list [catch {
	set data [parseargs0 dump root -version 2.0]
	blt::parseargs create parseargs1
	parseargs1 restore root -data $data
	set data [parseargs1 dump root -version 2.0]
	blt::parseargs destroy parseargs1
	set data
	} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} node1} {} {}
1 9 {{} node1 node9} {} {thisTag}
1 11 {{} node1 node11} {key value} {}
1 12 {{} node1 node12} {key1 value1 key2 value2} {}
0 2 {{} node2} {} {}
0 3 {{} node3} {} {}
0 4 {{} node4} {} {}
0 5 {{} node5} {} {myTag}
5 13 {{} node5 node13} {} {}
0 6 {{} node6} {} {myTag2 myTag1}
0 8 {{} myLabel} {} {thisTag}
}}

test parseargs.340 {parseargs0 restore (missing arg)} {
    list [catch {parseargs0 restore} msg] $msg
} {1 {wrong # args: should be "parseargs0 restore nodeName ?switches ...?"}}

test parseargs.341 {parseargs0 restore 0 badSwitch} {
    list [catch {parseargs0 restore 0 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -data data
   -file fileName
   -notags 
   -overwrite }}


test parseargs.342 {parseargs0 restore 0 {} arg (extra arg)} {
    list [catch {parseargs0 restore 0 {} arg} msg] $msg
} {1 {unknown switch ""
The following switches are available:
   -data data
   -file fileName
   -notags 
   -overwrite }}


test parseargs.343 {parseargs0 size (missing arg)} {
    list [catch {parseargs0 size} msg] $msg
} {1 {wrong # args: should be "parseargs0 size nodeName"}}

test parseargs.344 {parseargs0 size 0} {
    list [catch {parseargs0 size 0} msg] $msg
} {0 12}

test parseargs.345 {parseargs0 size all} {
    list [catch {parseargs0 size all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.346 {parseargs0 size 0 10 (extra arg)} {
    list [catch {parseargs0 size 0 10} msg] $msg
} {1 {wrong # args: should be "parseargs0 size nodeName"}}

test parseargs.347 {parseargs0 delete (no args)} {
    list [catch {parseargs0 delete} msg] $msg
} {0 {}}

test parseargs.348 {parseargs0 delete 11} {
    list [catch {parseargs0 delete 11} msg] $msg
} {0 {}}

test parseargs.349 {parseargs0 delete 11} {
    list [catch {parseargs0 delete 11} msg] $msg
} {1 {can't find tag or id "11" in ::parseargs0}}

test parseargs.350 {parseargs0 delete 9 12} {
    list [catch {parseargs0 delete 9 12} msg] $msg
} {0 {}}

test parseargs.351 {parseargs0 dump 0 -version 2.0} {
    list [catch {parseargs0 dump 0 -version 2.0} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} node1} {} {}
0 2 {{} node2} {} {}
0 3 {{} node3} {} {}
0 4 {{} node4} {} {}
0 5 {{} node5} {} {myTag}
5 13 {{} node5 node13} {} {}
0 6 {{} node6} {} {myTag2 myTag1}
0 8 {{} myLabel} {} {thisTag}
}}

test parseargs.352 {delete all} {
    list [catch {
	set data [parseargs0 dump root -version 2.0]
	blt::parseargs create
	parseargs1 restore root -data $data
	parseargs1 delete all
	set data [parseargs1 dump root -version 2.0]
	blt::parseargs destroy parseargs1
	set data
	} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
}}

test parseargs.353 {delete all all} {
    list [catch {
	set data [parseargs0 dump root -version 2.0]
	blt::parseargs create
	parseargs1 restore root -data $data
	parseargs1 delete all all
	set data [parseargs1 dump root -version 2.0]
	blt::parseargs destroy parseargs1
	set data
	} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
}}

test parseargs.354 {parseargs0 apply (missing arg)} {
    list [catch {parseargs0 apply} msg] $msg
} {1 {wrong # args: should be "parseargs0 apply nodeName ?switches ...?"}}

test parseargs.355 {parseargs0 apply 0} {
    list [catch {parseargs0 apply 0} msg] $msg
} {0 {}}

test parseargs.356 {parseargs0 apply 0 -badSwitch} {
    list [catch {parseargs0 apply 0 -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -precommand command
   -postcommand command
   -depth number
   -exact string
   -glob pattern
   -invert 
   -key pattern
   -keyexact string
   -keyglob pattern
   -keyregexp pattern
   -leafonly 
   -nocase 
   -path 
   -regexp pattern
   -tag tagList}}

test parseargs.357 {parseargs0 apply badTag} {
    list [catch {parseargs0 apply badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::parseargs0}}

test parseargs.358 {parseargs0 apply all} {
    list [catch {parseargs0 apply all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.359 {parseargs0 apply myTag -precommand lappend} {
    list [catch {
	set mylist {}
	parseargs0 apply myTag -precommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {5 13}}

test parseargs.360 {parseargs0 apply root -precommand lappend} {
    list [catch {
	set mylist {}
	parseargs0 apply root -precommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {0 1 2 3 4 5 13 6 8}}

test parseargs.361 {parseargs0 apply -postcommand} {
    list [catch {
	set mylist {}
	parseargs0 apply root -postcommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.362 {parseargs0 apply -precommand -postcommand} {
    list [catch {
	set mylist {}
	parseargs0 apply root -precommand {lappend mylist} \
		-postcommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {0 1 1 2 2 3 3 4 4 5 13 13 5 6 6 8 8 0}}

test parseargs.363 {parseargs0 apply root -precommand lappend -depth 1} {
    list [catch {
	set mylist {}
	parseargs0 apply root -precommand {lappend mylist} -depth 1
	set mylist
    } msg] $msg
} {0 {0 1 2 3 4 5 6 8}}


test parseargs.364 {parseargs0 apply root -precommand -depth 0} {
    list [catch {
	set mylist {}
	parseargs0 apply root -precommand {lappend mylist} -depth 0
	set mylist
    } msg] $msg
} {0 0}

test parseargs.365 {parseargs0 apply root -precommand -tag myTag} {
    list [catch {
	set mylist {}
	parseargs0 apply root -precommand {lappend mylist} -tag myTag
	set mylist
    } msg] $msg
} {0 5}


test parseargs.366 {parseargs0 apply root -precommand -key key1} {
    list [catch {
	set mylist {}
	parseargs0 set myTag key1 0.0
	parseargs0 apply root -precommand {lappend mylist} -key key1
	parseargs0 unset myTag key1
	set mylist
    } msg] $msg
} {0 5}

test parseargs.367 {parseargs0 apply root -postcommand -regexp node.*} {
    list [catch {
	set mylist {}
	parseargs0 set myTag key1 0.0
	parseargs0 apply root -precommand {lappend mylist} -regexp {node5} 
	parseargs0 unset myTag key1
	set mylist
    } msg] $msg
} {0 5}

test parseargs.368 {parseargs0 find (missing arg)} {
    list [catch {parseargs0 find} msg] $msg
} {1 {wrong # args: should be "parseargs0 find nodeName ?switches ...?"}}

test parseargs.369 {parseargs0 find 0} {
    list [catch {parseargs0 find 0} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.370 {parseargs0 find root} {
    list [catch {parseargs0 find root} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.371 {parseargs0 find 0 -glob node*} {
    list [catch {parseargs0 find root -glob node*} msg] $msg
} {0 {1 2 3 4 13 5 6}}

test parseargs.372 {parseargs0 find 0 -glob nobody} {
    list [catch {parseargs0 find root -glob nobody} msg] $msg
} {0 {}}

test parseargs.373 {parseargs0 find 0 -regexp {node[0-3]}} {
    list [catch {parseargs0 find root -regexp {node[0-3]}} msg] $msg
} {0 {1 2 3 13}}

test parseargs.374 {parseargs0 find 0 -regexp {.*[A-Z].*}} {
    list [catch {parseargs0 find root -regexp {.*[A-Z].*}} msg] $msg
} {0 8}

test parseargs.375 {parseargs0 find 0 -exact myLabel} {
    list [catch {parseargs0 find root -exact myLabel} msg] $msg
} {0 8}

test parseargs.376 {parseargs0 find 0 -exact myLabel -invert} {
    list [catch {parseargs0 find root -exact myLabel -invert} msg] $msg
} {0 {1 2 3 4 13 5 6 0}}


test parseargs.377 {parseargs0 find 3 -exact node3} {
    list [catch {parseargs0 find 3 -exact node3} msg] $msg
} {0 3}

test parseargs.378 {parseargs0 find 0 -nocase -exact mylabel} {
    list [catch {parseargs0 find 0 -nocase -exact mylabel} msg] $msg
} {0 8}

test parseargs.379 {parseargs0 find 0 -nocase} {
    list [catch {parseargs0 find 0 -nocase} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.380 {parseargs0 find 0 -path -nocase -glob *node1* } {
    list [catch {parseargs0 find 0 -path -nocase -glob *node1*} msg] $msg
} {0 {1 13}}

test parseargs.381 {parseargs0 find 0 -count 5 } {
    list [catch {parseargs0 find 0 -count 5} msg] $msg
} {0 {1 2 3 4 13}}

test parseargs.382 {parseargs0 find 0 -count -5 } {
    list [catch {parseargs0 find 0 -count -5} msg] $msg
} {1 {bad value "-5": can't be negative}}

test parseargs.383 {parseargs0 find 0 -count badValue } {
    list [catch {parseargs0 find 0 -count badValue} msg] $msg
} {1 {expected integer but got "badValue"}}

test parseargs.384 {parseargs0 find 0 -count badValue } {
    list [catch {parseargs0 find 0 -count badValue} msg] $msg
} {1 {expected integer but got "badValue"}}

test parseargs.385 {parseargs0 find 0 -leafonly} {
    list [catch {parseargs0 find 0 -leafonly} msg] $msg
} {0 {1 2 3 4 13 6 8}}

test parseargs.386 {parseargs0 find 0 -leafonly -glob {node[18]}} {
    list [catch {parseargs0 find 0 -glob {node[18]} -leafonly} msg] $msg
} {0 1}

test parseargs.387 {parseargs0 find 0 -depth 0} {
    list [catch {parseargs0 find 0 -depth 0} msg] $msg
} {0 0}

test parseargs.388 {parseargs0 find 0 -depth 1} {
    list [catch {parseargs0 find 0 -depth 1} msg] $msg
} {0 {1 2 3 4 5 6 8 0}}

test parseargs.389 {parseargs0 find 0 -depth 2} {
    list [catch {parseargs0 find 0 -depth 2} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.390 {parseargs0 find 0 -depth 20} {
    list [catch {parseargs0 find 0 -depth 20} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.391 {parseargs0 find 1 -depth 0} {
    list [catch {parseargs0 find 1 -depth 0} msg] $msg
} {0 1}

test parseargs.392 {parseargs0 find 1 -depth 1} {
    list [catch {parseargs0 find 1 -depth 1} msg] $msg
} {0 1}

test parseargs.393 {parseargs0 find 1 -depth 2} {
    list [catch {parseargs0 find 1 -depth 2} msg] $msg
} {0 1}

test parseargs.394 {parseargs0 find all} {
    list [catch {parseargs0 find all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.395 {parseargs0 find badTag} {
    list [catch {parseargs0 find badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::parseargs0}}

test parseargs.396 {parseargs0 find 0 -addtag hi} {
    list [catch {parseargs0 find 0 -addtag hi} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.397 {parseargs0 find 0 -addtag all} {
    list [catch {parseargs0 find 0 -addtag all} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.398 {parseargs0 find 0 -addtag root} {
    list [catch {parseargs0 find 0 -addtag root} msg] $msg
} {1 {can't add reserved tag "root"}}

test parseargs.399 {parseargs0 find 0 -exec {lappend list} -leafonly} {
    list [catch {
	set list {}
	parseargs0 find 0 -exec {lappend list} -leafonly
	set list
	} msg] $msg
} {0 {1 2 3 4 13 6 8}}

test parseargs.400 {parseargs0 find 0 -tag root} {
    list [catch {parseargs0 find 0 -tag root} msg] $msg
} {0 0}

test parseargs.401 {parseargs0 find 0 -tag myTag} {
    list [catch {parseargs0 find 0 -tag myTag} msg] $msg
} {0 5}

test parseargs.402 {parseargs0 find 0 -tag badTag} {
    list [catch {parseargs0 find 0 -tag badTag} msg] $msg
} {0 {}}

test parseargs.403 {parseargs0 tag (missing args)} {
    list [catch {parseargs0 tag} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag args ..."}}

test parseargs.404 {parseargs0 tag badOp} {
    list [catch {parseargs0 tag badOp} msg] $msg
} {1 {bad operation "badOp": should be one of...
  parseargs0 tag add tag ?nodeName...?
  parseargs0 tag delete tag nodeName...
  parseargs0 tag exists tag ?nodeName?
  parseargs0 tag forget tag...
  parseargs0 tag get nodeName ?pattern...?
  parseargs0 tag names ?nodeName...?
  parseargs0 tag nodes tag ?tag...?
  parseargs0 tag set nodeName tag...
  parseargs0 tag unset nodeName tag...}}

test parseargs.405 {parseargs0 tag add} {
    list [catch {parseargs0 tag add} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag add tag ?nodeName...?"}}

test parseargs.406 {parseargs0 tag add newTag} {
    list [catch {parseargs0 tag add newTag} msg] $msg
} {0 {}}

test parseargs.407 {parseargs0 tag add tag badNode} {
    list [catch {parseargs0 tag add tag badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::parseargs0}}

test parseargs.408 {parseargs0 tag add newTag root} {
    list [catch {parseargs0 tag add newTag root} msg] $msg
} {0 {}}

test parseargs.409 {parseargs0 tag add newTag all} {
    list [catch {parseargs0 tag add newTag all} msg] $msg
} {0 {}}

test parseargs.410 {parseargs0 tag add tag2 0 1 2 3 4} {
    list [catch {parseargs0 tag add tag2 0 1 2 3 4} msg] $msg
} {0 {}}

test parseargs.411 {parseargs0 tag exists tag2} {
    list [catch {parseargs0 tag exists tag2} msg] $msg
} {0 1}

test parseargs.412 {parseargs0 tag exists tag2 0} {
    list [catch {parseargs0 tag exists tag2 0} msg] $msg
} {0 1}

test parseargs.413 {parseargs0 tag exists tag2 5} {
    list [catch {parseargs0 tag exists tag2 5} msg] $msg
} {0 0}

test parseargs.414 {parseargs0 tag exists badTag} {
    list [catch {parseargs0 tag exists badTag} msg] $msg
} {0 0}

test parseargs.415 {parseargs0 tag exists badTag 1000} {
    list [catch {parseargs0 tag exists badTag 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::parseargs0}}

test parseargs.416 {parseargs0 tag add tag2 0 1 2 3 4 1000} {
    list [catch {parseargs0 tag add tag2 0 1 2 3 4 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::parseargs0}}

test parseargs.417 {parseargs0 tag names} {
    list [catch {parseargs0 tag names} msg] [lsort $msg]
} {0 {all hi myTag myTag1 myTag2 newTag root tag2 thisTag}}

test parseargs.418 {parseargs0 tag names badNode} {
    list [catch {parseargs0 tag names badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::parseargs0}}

test parseargs.419 {parseargs0 tag names all} {
    list [catch {parseargs0 tag names all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.420 {parseargs0 tag names root} {
    list [catch {parseargs0 tag names root} msg] [lsort $msg]
} {0 {all hi newTag root tag2}}

test parseargs.421 {parseargs0 tag names 0 1} {
    list [catch {parseargs0 tag names 0 1} msg] [lsort $msg]
} {0 {all hi newTag root tag2}}

test parseargs.422 {parseargs0 tag nodes (missing arg)} {
    list [catch {parseargs0 tag nodes} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag nodes tag ?tag...?"}}

test parseargs.423 {parseargs0 tag nodes root badTag} {
    # It's not an error to use bad tag.
    list [catch {parseargs0 tag nodes root badTag} msg] $msg
} {0 {}}

test parseargs.424 {parseargs0 tag nodes root tag2} {
    list [catch {parseargs0 tag nodes root tag2} msg] [lsort $msg]
} {0 {0 1 2 3 4}}

test parseargs.425 {parseargs0 ancestor (missing arg)} {
    list [catch {parseargs0 ancestor} msg] $msg
} {1 {wrong # args: should be "parseargs0 ancestor node1 node2"}}

test parseargs.426 {parseargs0 ancestor 0 (missing arg)} {
    list [catch {parseargs0 ancestor 0} msg] $msg
} {1 {wrong # args: should be "parseargs0 ancestor node1 node2"}}

test parseargs.427 {parseargs0 ancestor 0 10} {
    list [catch {parseargs0 ancestor 0 10} msg] $msg
} {1 {can't find tag or id "10" in ::parseargs0}}

test parseargs.428 {parseargs0 ancestor 0 4} {
    list [catch {parseargs0 ancestor 0 4} msg] $msg
} {0 0}

test parseargs.429 {parseargs0 ancestor 1 8} {
    list [catch {parseargs0 ancestor 1 8} msg] $msg
} {0 0}

test parseargs.430 {parseargs0 ancestor root 0} {
    list [catch {parseargs0 ancestor root 0} msg] $msg
} {0 0}

test parseargs.431 {parseargs0 ancestor 8 8} {
    list [catch {parseargs0 ancestor 8 8} msg] $msg
} {0 8}

test parseargs.432 {parseargs0 ancestor 0 all} {
    list [catch {parseargs0 ancestor 0 all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.433 {parseargs0 ancestor 7 9} {
    list [catch {
	set n1 1; set n2 1;
	for { set i 0 } { $i < 4 } { incr i } {
	    set n1 [parseargs0 insert $n1]
	    set n2 [parseargs0 insert $n2]
	}
	parseargs0 ancestor $n1 $n2
	} msg] $msg
} {0 1}

test parseargs.434 {parseargs0 path (missing arg)} {
    list [catch {parseargs0 path} msg] $msg
} {1 {wrong # args: should be "parseargs0 path ?args ...?"}}

test parseargs.435 {parseargs0 path badArg} {
    list [catch {parseargs0 path badArg} msg] $msg
} {1 {bad operation "badArg": should be one of...
  parseargs0 path create pathName ?switches ...?
  parseargs0 path parse pathName ?switches ...?
  parseargs0 path print nodeName ?switches ...?
  parseargs0 path separator ?sepString?}}

test parseargs.436 {parseargs0 path print root} {
    list [catch {parseargs0 path print root} msg] $msg
} {0 {}}

test parseargs.437 {parseargs0 path print 0} {
    list [catch {parseargs0 path print 0} msg] $msg
} {0 {}}

test parseargs.438 {parseargs0 path print 15} {
    list [catch {parseargs0 path print 15} msg] $msg
} {0 {node1 node15}}

test parseargs.439 {parseargs0 path print 15} {
    list [catch {parseargs0 path print 15 -separator /} msg] $msg
} {0 /node1/node15}

test parseargs.440 {parseargs0 path print 16} {
    list [catch {parseargs0 path print 16 -separator /} msg] $msg
} {0 /node1/node14/node16}

test parseargs.441 {parseargs0 path parse /} {
    list [catch {parseargs0 path parse / -separator /} msg] $msg
} {0 0}

test parseargs.442 {parseargs0 path parse /node1} {
    list [catch {parseargs0 path parse /node1 -separator /} msg] $msg
} {0 1}

test parseargs.443 {parseargs0 path parse /node1/node14} {
    list [catch {parseargs0 path parse /node1/node14 -separator /} msg] $msg
} {0 14}

test parseargs.444 {parseargs0 path parse } {
    list [catch {parseargs0 path parse /node1/node14/node16 -separator /} msg] $msg
} {0 16}

test parseargs.445 {parseargs0 path parse } {
    list [catch {parseargs0 path parse /node1/node14/node16/ -separator /} msg] $msg
} {0 16}

test parseargs.446 {parseargs0 path parse } {
    list [catch {parseargs0 path parse //node1//node14//node16// -separator /} msg] $msg
} {0 16}

test parseargs.447 {parseargs0 path parse } {
    list [catch {parseargs0 path parse ::node1::node14::node16 -separator ::} msg] $msg
} {0 16}

test parseargs.448 {parseargs0 path parse } {
    list [catch {parseargs0 path parse /node1/node14/node16 -separator /} msg] $msg
} {0 16}

test parseargs.449 {parseargs0 path parse } {
    list [catch {parseargs0 path parse /node1/node14/node16/ -separator /} msg] $msg
} {0 16}

test parseargs.450 {parseargs0 path parse } {
    list [catch {parseargs0 path parse //node1//node14//node16// -separator /} msg] $msg
} {0 16}

test parseargs.451 {parseargs0 path parse } {
    list [catch {parseargs0 path parse ::node1::node14::node16 -separator ::} msg] $msg
} {0 16}

test parseargs.452 {parseargs0 path parse ""} {
    list [catch {parseargs0 path parse {}} msg] $msg
} {0 0}

test parseargs.453 {parseargs0 path parse node1 } {
    list [catch {parseargs0 path parse node1} msg] $msg
} {0 1}

test parseargs.454 {parseargs0 path parse {node1 node14}} {
    list [catch {parseargs0 path parse {node1 node14}} msg] $msg
} {0 14}

test parseargs.455 {parseargs0 path parse } {
    list [catch {parseargs0 path parse {node1 node14 node16}} msg] $msg
} {0 16}


test parseargs.456 {parseargs0 path print all} {
    list [catch {parseargs0 path print all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.457 {parseargs0 path print 0 badSwitch} {
    list [catch {parseargs0 path print 0 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -from node
   -separator char
   -noleadingseparator }}


test parseargs.458 {parseargs0 tag forget} {
    list [catch {parseargs0 tag forget} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag forget tag..."}}

test parseargs.459 {parseargs0 tag forget badTag} {
    list [catch {
	parseargs0 tag forget badTag
	lsort [parseargs0 tag names]
    } msg] $msg
} {0 {all hi myTag myTag1 myTag2 newTag root tag2 thisTag}}

test parseargs.460 {parseargs0 tag forget hi} {
    list [catch {
	parseargs0 tag forget hi
	lsort [parseargs0 tag names]
    } msg] $msg
} {0 {all myTag myTag1 myTag2 newTag root tag2 thisTag}}

test parseargs.461 {parseargs0 tag forget tag1 tag2} {
    list [catch {
	parseargs0 tag forget myTag1 myTag2
	lsort [parseargs0 tag names]
    } msg] $msg
} {0 {all myTag newTag root tag2 thisTag}}

test parseargs.462 {parseargs0 tag forget all} {
    list [catch {
	parseargs0 tag forget all
	lsort [parseargs0 tag names]
    } msg] $msg
} {0 {all myTag newTag root tag2 thisTag}}

test parseargs.463 {parseargs0 tag forget root} {
    list [catch {
	parseargs0 tag forget root
	lsort [parseargs0 tag names]
    } msg] $msg
} {0 {all myTag newTag root tag2 thisTag}}

test parseargs.464 {parseargs0 tag delete} {
    list [catch {parseargs0 tag delete} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag delete tag nodeName..."}}

test parseargs.465 {parseargs0 tag delete tag} {
    list [catch {parseargs0 tag delete tag} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag delete tag nodeName..."}}

test parseargs.466 {parseargs0 tag delete tag 0} {
    list [catch {parseargs0 tag delete tag 0} msg] $msg
} {0 {}}

test parseargs.467 {parseargs0 tag delete root 0} {
    list [catch {parseargs0 tag delete root 0} msg] $msg
} {1 {can't delete reserved tag "root"}}

test parseargs.468 {parseargs0 move} {
    list [catch {parseargs0 move} msg] $msg
} {1 {wrong # args: should be "parseargs0 move nodeName destNode ?switches ...?"}}

test parseargs.469 {parseargs0 move 0} {
    list [catch {parseargs0 move 0} msg] $msg
} {1 {wrong # args: should be "parseargs0 move nodeName destNode ?switches ...?"}}

test parseargs.470 {parseargs0 move 0 0} {
    list [catch {parseargs0 move 0 0} msg] $msg
} {1 {can't move root node}}

test parseargs.471 {parseargs0 move 0 badNode} {
    list [catch {parseargs0 move 0 badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::parseargs0}}

test parseargs.472 {parseargs0 move 0 all} {
    list [catch {parseargs0 move 0 all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.473 {parseargs0 move 1 0 -before 2} {
    list [catch {
	parseargs0 move 1 0 -before 2
	parseargs0 children 0
    } msg] $msg
} {0 {1 2 3 4 5 6 8}}

test parseargs.474 {parseargs0 move 1 0 -after 2} {
    list [catch {
	parseargs0 move 1 0 -after 2
	parseargs0 children 0
    } msg] $msg
} {0 {2 1 3 4 5 6 8}}

test parseargs.475 {parseargs0 move 1 2} {
    list [catch {
	parseargs0 move 1 2
	parseargs0 children 0
    } msg] $msg
} {0 {2 3 4 5 6 8}}

test parseargs.476 {parseargs0 move 0 2} {
    list [catch {parseargs0 move 0 2} msg] $msg
} {1 {can't move root node}}

test parseargs.477 {parseargs0 move 1 17} {
    list [catch {parseargs0 move 1 17} msg] $msg
} {1 {can't move node: "1" is an ancestor of "17"}}

test parseargs.478 {parseargs0 attach} {
    list [catch {parseargs0 attach} msg] $msg
} {1 {wrong # args: should be "parseargs0 attach parserName ?switches ...?"}}

test parseargs.479 {parseargs0 attach parseargs2 badArg} {
    list [catch {parseargs0 attach parseargs2 badArg} msg] $msg
} {1 {unknown switch "badArg"
The following switches are available:
   -newtags }}


test parseargs.480 {parseargs1 attach parseargs0 -newtags} {
    list [catch {
	blt::parseargs create
	parseargs1 attach parseargs0 -newtags
	parseargs1 dump 0 -version 2.0
	} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 2 {{} node2} {} {}
2 1 {{} node2 node1} {} {}
1 14 {{} node2 node1 node14} {} {}
14 16 {{} node2 node1 node14 node16} {} {}
16 18 {{} node2 node1 node14 node16 node18} {} {}
18 20 {{} node2 node1 node14 node16 node18 node20} {} {}
1 15 {{} node2 node1 node15} {} {}
15 17 {{} node2 node1 node15 node17} {} {}
17 19 {{} node2 node1 node15 node17 node19} {} {}
19 21 {{} node2 node1 node15 node17 node19 node21} {} {}
0 3 {{} node3} {} {}
0 4 {{} node4} {} {}
0 5 {{} node5} {} {}
5 13 {{} node5 node13} {} {}
0 6 {{} node6} {} {}
0 8 {{} myLabel} {} {}
}}

test parseargs.481 {parseargs1 attach parseargs0} {
    list [catch {
	blt::parseargs create
	parseargs1 attach parseargs0
	parseargs1 dump 0 -version 2.0
	} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {tag2 newTag}
0 2 {{} node2} {} {tag2 newTag}
2 1 {{} node2 node1} {} {tag2 newTag}
1 14 {{} node2 node1 node14} {} {}
14 16 {{} node2 node1 node14 node16} {} {}
16 18 {{} node2 node1 node14 node16 node18} {} {}
18 20 {{} node2 node1 node14 node16 node18 node20} {} {}
1 15 {{} node2 node1 node15} {} {}
15 17 {{} node2 node1 node15 node17} {} {}
17 19 {{} node2 node1 node15 node17 node19} {} {}
19 21 {{} node2 node1 node15 node17 node19 node21} {} {}
0 3 {{} node3} {} {tag2 newTag}
0 4 {{} node4} {} {tag2 newTag}
0 5 {{} node5} {} {newTag myTag}
5 13 {{} node5 node13} {} {newTag}
0 6 {{} node6} {} {newTag}
0 8 {{} myLabel} {} {thisTag newTag}
}}

test parseargs.482 {parseargs1 attach ""} {
    list [catch {parseargs1 attach ""} msg] $msg
} {0 {}}


test parseargs.483 {blt::parseargs destroy parseargs1} {
    list [catch {blt::parseargs destroy parseargs1} msg] $msg
} {0 {}}

test parseargs.484 {parseargs0 find root -badSwitch} {
    list [catch {parseargs0 find root -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -addtag tagName
   -count number
   -depth number
   -exact string
   -excludes nodeList
   -exec command
   -expr exprString
   -glob pattern
   -includes nodeList
   -invert 
   -key string
   -keyexact string
   -keyglob pattern
   -keyregexp pattern
   -leafonly 
   -mindepth number
   -nocase 
   -order orderName
   -path 
   -regexp pattern
   -tag tagList}}

test parseargs.485 {parseargs0 find root -order} {
    list [catch {parseargs0 find root -order} msg] $msg
} {1 {value for "-order" missing}}

test parseargs.486 {parseargs0 find root ...} {
    list [catch {parseargs0 find root -order preorder -order postorder -order inorder} msg] $msg
} {0 {20 18 16 14 1 21 19 17 15 2 0 3 4 13 5 6 8}}

test parseargs.487 {parseargs0 find root -order preorder} {
    list [catch {parseargs0 find root -order preorder} msg] $msg
} {0 {0 2 1 14 16 18 20 15 17 19 21 3 4 5 13 6 8}}

test parseargs.488 {parseargs0 find root -order postorder} {
    list [catch {parseargs0 find root -order postorder} msg] $msg
} {0 {20 18 16 14 21 19 17 15 1 2 3 4 13 5 6 8 0}}

test parseargs.489 {parseargs0 find root -order inorder} {
    list [catch {parseargs0 find root -order inorder} msg] $msg
} {0 {20 18 16 14 1 21 19 17 15 2 0 3 4 13 5 6 8}}

test parseargs.490 {parseargs0 find root -order breadthfirst} {
    list [catch {parseargs0 find root -order breadthfirst} msg] $msg
} {0 {0 2 3 4 5 6 8 1 13 14 15 16 17 18 19 20 21}}

test parseargs.491 {parseargs0 set all key1 myValue} {
    list [catch {parseargs0 set all key1 myValue} msg] $msg
} {0 {}}

test parseargs.492 {parseargs0 set 15 key1 123} {
    list [catch {parseargs0 set 15 key1 123} msg] $msg
} {0 {}}

test parseargs.493 {parseargs0 set 16 key1 1234 key2 abc} {
    list [catch {parseargs0 set 16 key1 123 key2 abc} msg] $msg
} {0 {}}

test parseargs.494 {parseargs0 find root -key } {
    list [catch {parseargs0 find root -key} msg] $msg
} {1 {value for "-key" missing}}

test parseargs.495 {parseargs0 find root -key noKey} {
    list [catch {parseargs0 find root -key noKey} msg] $msg
} {0 {}}

test parseargs.496 {parseargs0 find root -key key1} {
    list [catch {parseargs0 find root -key key1} msg] $msg
} {0 {20 18 16 14 21 19 17 15 1 2 3 4 13 5 6 8 0}}

test parseargs.497 {parseargs0 find root -key key2} {
    list [catch {parseargs0 find root -key key2} msg] $msg
} {0 16}

test parseargs.498 {parseargs0 find root -key key2 -exact notThere } {
    list [catch {parseargs0 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test parseargs.499 {parseargs0 find root -key key1 -glob notThere } {
    list [catch {parseargs0 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test parseargs.500 {parseargs0 find root -key badKey -regexp notThere } {
    list [catch {parseargs0 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test parseargs.501 {parseargs0 find root -key key1 -glob 12*} {
    list [catch {parseargs0 find root -key key1 -glob 12*} msg] $msg
} {0 {16 15}}

test parseargs.502 {parseargs0 sort} {
    list [catch {parseargs0 sort} msg] $msg
} {1 {wrong # args: should be "parseargs0 sort nodeName ?switches ...?"}}

test parseargs.503 {parseargs0 sort all} {
    list [catch {parseargs0 sort all} msg] $msg
} {1 {tag "all" refers to more than one node.}}

test parseargs.504 {parseargs0 sort -recurse} {
    list [catch {parseargs0 sort -recurse} msg] $msg
} {1 {can't find tag or id "-recurse" in ::parseargs0}}

test parseargs.505 {parseargs0 sort 0} {
    list [catch {parseargs0 sort 0} msg] $msg
} {0 {8 2 3 4 5 6}}

test parseargs.506 {parseargs0 sort 0 -recurse} {
    list [catch {parseargs0 sort 0 -recurse} msg] $msg
} {0 {0 8 1 2 3 4 5 6 13 14 15 16 17 18 19 20 21}}

test parseargs.507 {parseargs0 sort 0 -decreasing -key} {
    list [catch {parseargs0 sort 0 -decreasing -key} msg] $msg
} {1 {value for "-key" missing}}

test parseargs.508 {parseargs0 sort 0 -re} {
    list [catch {parseargs0 sort 0 -re} msg] $msg
} {1 {ambiguous switch "-re"
The following switches are available:
   -ascii 
   -command command
   -decreasing 
   -dictionary 
   -integer 
   -key string
   -path 
   -real 
   -recurse 
   -reorder }}


test parseargs.509 {parseargs0 sort 0 -decreasing} {
    list [catch {parseargs0 sort 0 -decreasing} msg] $msg
} {0 {6 5 4 3 2 8}}

test parseargs.510 {parseargs0 sort 0} {
    list [catch {
	set list {}
	foreach n [parseargs0 sort 0] {
	    lappend list [parseargs0 label $n]
	}	
	set list
    } msg] $msg
} {0 {myLabel node2 node3 node4 node5 node6}}

test parseargs.511 {parseargs0 sort 0 -decreasing} {
    list [catch {parseargs0 sort 0 -decreasing} msg] $msg
} {0 {6 5 4 3 2 8}}


test parseargs.512 {parseargs0 sort 0 -decreasing -key} {
    list [catch {parseargs0 sort 0 -decreasing -key} msg] $msg
} {1 {value for "-key" missing}}

test parseargs.513 {parseargs0 sort 0 -decreasing -key key1} {
    list [catch {parseargs0 sort 0 -decreasing -key key1} msg] $msg
} {0 {8 6 5 4 3 2}}

test parseargs.514 {parseargs0 sort 0 -decreasing -recurse -key key1} {
    list [catch {parseargs0 sort 0 -decreasing -recurse -key key1} msg] $msg
} {0 {15 16 0 1 2 3 4 5 6 8 13 14 17 18 19 20 21}}

test parseargs.515 {parseargs0 sort 0 -decreasing -key key1} {
    list [catch {
	set list {}
	foreach n [parseargs0 sort 0 -decreasing -key key1] {
	    lappend list [parseargs0 get $n key1]
	}
	set list
    } msg] $msg
} {0 {myValue myValue myValue myValue myValue myValue}}


test parseargs.516 {parseargs0 index 1->firstchild} {
    list [catch {parseargs0 index 1->firstchild} msg] $msg
} {0 14}

test parseargs.517 {parseargs0 index root->firstchild} {
    list [catch {parseargs0 index root->firstchild} msg] $msg
} {0 2}

test parseargs.518 {parseargs0 label root->parent} {
    list [catch {parseargs0 label root->parent} msg] $msg
} {1 {can't find tag or id "root->parent" in ::parseargs0}}

test parseargs.519 {parseargs0 index root->parent} {
    list [catch {parseargs0 index root->parent} msg] $msg
} {0 -1}

test parseargs.520 {parseargs0 index root->lastchild} {
    list [catch {parseargs0 index root->lastchild} msg] $msg
} {0 8}

test parseargs.521 {parseargs0 index root->next} {
    list [catch {parseargs0 index root->next} msg] $msg
} {0 2}

test parseargs.522 {parseargs0 index root->previous} {
    list [catch {parseargs0 index root->previous} msg] $msg
} {0 -1}

test parseargs.523 {parseargs0 label root->previous} {
    list [catch {parseargs0 label root->previous} msg] $msg
} {1 {can't find tag or id "root->previous" in ::parseargs0}}

test parseargs.524 {parseargs0 index 1->previous} {
    list [catch {parseargs0 index 1->previous} msg] $msg
} {0 2}

test parseargs.525 {parseargs0 label root->badModifier} {
    list [catch {parseargs0 label root->badModifier} msg] $msg
} {1 {can't find tag or id "root->badModifier" in ::parseargs0}}

test parseargs.526 {parseargs0 index root->badModifier} {
    list [catch {parseargs0 index root->badModifier} msg] $msg
} {0 -1}

test parseargs.527 {parseargs0 index root->firstchild->parent} {
    list [catch {parseargs0 index root->firstchild->parent} msg] $msg
} {0 0}

test parseargs.528 {parseargs0 trace} {
    list [catch {parseargs0 trace} msg] $msg
} {1 {wrong # args: should be one of...
  parseargs0 trace create nodeName key how command ?-whenidle?
  parseargs0 trace delete traceName ...
  parseargs0 trace info traceName
  parseargs0 trace names ?pattern ...?}}

test parseargs.529 {parseargs0 trace create} {
    list [catch {parseargs0 trace create} msg] $msg
} {1 {wrong # args: should be "parseargs0 trace create nodeName key how command ?-whenidle?"}}

test parseargs.530 {parseargs0 trace create root} {
    list [catch {parseargs0 trace create root} msg] $msg
} {1 {wrong # args: should be "parseargs0 trace create nodeName key how command ?-whenidle?"}}

test parseargs.531 {parseargs0 trace create root * } {
    list [catch {parseargs0 trace create root * } msg] $msg
} {1 {wrong # args: should be "parseargs0 trace create nodeName key how command ?-whenidle?"}}

test parseargs.532 {parseargs0 trace create root * rwuc} {
    list [catch {parseargs0 trace create root * rwuc} msg] $msg
} {1 {wrong # args: should be "parseargs0 trace create nodeName key how command ?-whenidle?"}}

proc Doit args { global mylist; lappend mylist $args }

test parseargs.533 {parseargs0 trace create all newKey rwuc Doit} {
    list [catch {parseargs0 trace create all newKey rwuc Doit} msg] $msg
} {0 trace0}

test parseargs.534 {parseargs0 trace info trace0} {
    list [catch {parseargs0 trace info trace0} msg] $msg
} {0 {all newKey rwuc Doit}}

test parseargs.535 {test create trace} {
    list [catch {
	set mylist {}
	parseargs0 set all newKey 20
	set mylist
	} msg] $msg
} {0 {{::parseargs0 0 newKey wc} {::parseargs0 2 newKey wc} {::parseargs0 1 newKey wc} {::parseargs0 14 newKey wc} {::parseargs0 16 newKey wc} {::parseargs0 18 newKey wc} {::parseargs0 20 newKey wc} {::parseargs0 15 newKey wc} {::parseargs0 17 newKey wc} {::parseargs0 19 newKey wc} {::parseargs0 21 newKey wc} {::parseargs0 3 newKey wc} {::parseargs0 4 newKey wc} {::parseargs0 5 newKey wc} {::parseargs0 13 newKey wc} {::parseargs0 6 newKey wc} {::parseargs0 8 newKey wc}}}

test parseargs.536 {test read trace} {
    list [catch {
	set mylist {}
	parseargs0 get root newKey
	set mylist
	} msg] $msg
} {0 {{::parseargs0 0 newKey r}}}

test parseargs.537 {test write trace} {
    list [catch {
	set mylist {}
	parseargs0 set all newKey 21
	set mylist
	} msg] $msg
} {0 {{::parseargs0 0 newKey w} {::parseargs0 2 newKey w} {::parseargs0 1 newKey w} {::parseargs0 14 newKey w} {::parseargs0 16 newKey w} {::parseargs0 18 newKey w} {::parseargs0 20 newKey w} {::parseargs0 15 newKey w} {::parseargs0 17 newKey w} {::parseargs0 19 newKey w} {::parseargs0 21 newKey w} {::parseargs0 3 newKey w} {::parseargs0 4 newKey w} {::parseargs0 5 newKey w} {::parseargs0 13 newKey w} {::parseargs0 6 newKey w} {::parseargs0 8 newKey w}}}

test parseargs.538 {test unset trace} {
    list [catch {
	set mylist {}
	parseargs0 set all newKey 21
	set mylist
	} msg] $msg
} {0 {{::parseargs0 0 newKey w} {::parseargs0 2 newKey w} {::parseargs0 1 newKey w} {::parseargs0 14 newKey w} {::parseargs0 16 newKey w} {::parseargs0 18 newKey w} {::parseargs0 20 newKey w} {::parseargs0 15 newKey w} {::parseargs0 17 newKey w} {::parseargs0 19 newKey w} {::parseargs0 21 newKey w} {::parseargs0 3 newKey w} {::parseargs0 4 newKey w} {::parseargs0 5 newKey w} {::parseargs0 13 newKey w} {::parseargs0 6 newKey w} {::parseargs0 8 newKey w}}}

test parseargs.539 {parseargs0 trace delete} {
    list [catch {parseargs0 trace delete} msg] $msg
} {0 {}}

test parseargs.540 {parseargs0 trace delete badId} {
    list [catch {parseargs0 trace delete badId} msg] $msg
} {1 {unknown trace "badId"}}

test parseargs.541 {parseargs0 trace delete trace0} {
    list [catch {parseargs0 trace delete trace0} msg] $msg
} {0 {}}

test parseargs.542 {test create trace} {
    list [catch {
	set mylist {}
	parseargs0 set all newKey 20
	set mylist
	} msg] $msg
} {0 {}}

test parseargs.543 {test unset trace} {
    list [catch {
	set mylist {}
	parseargs0 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}


test parseargs.544 {parseargs0 notify} {
    list [catch {parseargs0 notify} msg] $msg
} {1 {wrong # args: should be one of...
  parseargs0 notify create ?switches ...? command
  parseargs0 notify delete ?notifyName ...?
  parseargs0 notify info notifyName
  parseargs0 notify names ?pattern ...?}}

test parseargs.545 {parseargs0 notify create} {
    list [catch {parseargs0 notify create} msg] $msg
} {1 {wrong # args: should be "parseargs0 notify create ?switches ...? command"}}

test parseargs.546 {parseargs0 notify create -allevents} {
    list [catch {parseargs0 notify create -allevents Doit} msg] $msg
} {0 notify0}

test parseargs.547 {parseargs0 notify info notify0} {
    list [catch {parseargs0 notify info notify0} msg] $msg
} {0 {notify0 {-create -delete -move -sort -relabel} {Doit}}}

test parseargs.548 {parseargs0 notify info badId} {
    list [catch {parseargs0 notify info badId} msg] $msg
} {1 {unknown notify name "badId"}}

test parseargs.549 {parseargs0 notify info} {
    list [catch {parseargs0 notify info} msg] $msg
} {1 {wrong # args: should be "parseargs0 notify info notifyName"}}

test parseargs.550 {parseargs0 notify names} {
    list [catch {parseargs0 notify names} msg] $msg
} {0 notify0}


test parseargs.551 {test create notify} {
    list [catch {
	set mylist {}
	parseargs0 insert 1 -tags test
	set mylist
	} msg] $msg
} {0 {{-create 22}}}

test parseargs.552 {test move notify} {
    list [catch {
	set mylist {}
	parseargs0 move 8 test
	set mylist
	} msg] $msg
} {0 {{-move 8}}}

test parseargs.553 {test sort notify} {
    list [catch {
	set mylist {}
	parseargs0 sort 0 -reorder 
	set mylist
	} msg] $msg
} {0 {{-sort 0}}}

test parseargs.554 {test relabel notify} {
    list [catch {
	set mylist {}
	parseargs0 label test "newLabel"
	set mylist
	} msg] $msg
} {0 {{-relabel 22}}}

test parseargs.555 {test delete notify} {
    list [catch {
	set mylist {}
	parseargs0 delete test
	set mylist
	} msg] $msg
} {0 {{-delete 8} {-delete 22}}}


test parseargs.556 {parseargs0 notify delete badId} {
    list [catch {parseargs0 notify delete badId} msg] $msg
} {1 {unknown notify name "badId"}}


test parseargs.557 {test create notify} {
    list [catch {
	set mylist {}
	parseargs0 set all newKey 20
	set mylist
	} msg] $msg
} {0 {}}

test parseargs.558 {test delete notify} {
    list [catch {
	set mylist {}
	parseargs0 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}

test parseargs.559 {test delete notify} {
    list [catch {
	set mylist {}
	parseargs0 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}

test parseargs.560 {parseargs0 copy} {
    list [catch {parseargs0 copy} msg] $msg
} {1 {wrong # args: should be "parseargs0 copy parentNode ?parserName? nodeName ?switches ...?"}}

test parseargs.561 {parseargs0 copy root} {
    list [catch {parseargs0 copy root} msg] $msg
} {1 {wrong # args: should be "parseargs0 copy parentNode ?parserName? nodeName ?switches ...?"}}

test parseargs.562 {parseargs0 copy root 14} {
    list [catch {parseargs0 copy root 14} msg] $msg
} {0 23}

test parseargs.563 {parseargs0 copy 14 root} {
    list [catch {parseargs0 copy 14 root} msg] $msg
} {0 24}

test parseargs.564 {parseargs0 copy 14 root -recurse} {
    list [catch {parseargs0 copy 14 root -recurse} msg] $msg
} {1 {can't make cyclic copy: source node is an ancestor of the destination}}

test parseargs.565 {parseargs0 copy 3 2 -recurse -tags} {
    list [catch {parseargs0 copy 3 2 -recurse -tags} msg] $msg
} {0 25}

test parseargs.566 {copy parseargs to parseargs -recurse} {
    list [catch {
	blt::parseargs create parseargs1
	foreach node [parseargs0 children root] {
	    parseargs1 copy root parseargs0 $node -recurse 
	}
	foreach node [parseargs0 children root] {
	    parseargs1 copy root parseargs0 $node -recurse 
	}
	parseargs1 dump root -version 2.0
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} node2} {key1 myValue} {}
1 2 {{} node2 node1} {key1 myValue} {}
2 3 {{} node2 node1 node14} {key1 myValue} {}
3 4 {{} node2 node1 node14 node16} {key1 123 key2 abc} {}
4 5 {{} node2 node1 node14 node16 node18} {key1 myValue} {}
5 6 {{} node2 node1 node14 node16 node18 node20} {key1 myValue} {}
3 7 {{} node2 node1 node14 {}} {key1 myValue} {}
2 8 {{} node2 node1 node15} {key1 123} {}
8 9 {{} node2 node1 node15 node17} {key1 myValue} {}
9 10 {{} node2 node1 node15 node17 node19} {key1 myValue} {}
10 11 {{} node2 node1 node15 node17 node19 node21} {key1 myValue} {}
0 12 {{} node3} {key1 myValue} {}
12 13 {{} node3 node2} {key1 myValue} {}
13 14 {{} node3 node2 node1} {key1 myValue} {}
14 15 {{} node3 node2 node1 node14} {key1 myValue} {}
15 16 {{} node3 node2 node1 node14 node16} {key1 123 key2 abc} {}
16 17 {{} node3 node2 node1 node14 node16 node18} {key1 myValue} {}
17 18 {{} node3 node2 node1 node14 node16 node18 node20} {key1 myValue} {}
15 19 {{} node3 node2 node1 node14 {}} {key1 myValue} {}
14 20 {{} node3 node2 node1 node15} {key1 123} {}
20 21 {{} node3 node2 node1 node15 node17} {key1 myValue} {}
21 22 {{} node3 node2 node1 node15 node17 node19} {key1 myValue} {}
22 23 {{} node3 node2 node1 node15 node17 node19 node21} {key1 myValue} {}
0 24 {{} node4} {key1 myValue} {}
0 25 {{} node5} {key1 myValue} {}
25 26 {{} node5 node13} {key1 myValue} {}
0 27 {{} node6} {key1 myValue} {}
0 28 {{} node14} {key1 myValue} {}
0 29 {{} node2} {key1 myValue} {}
29 30 {{} node2 node1} {key1 myValue} {}
30 31 {{} node2 node1 node14} {key1 myValue} {}
31 32 {{} node2 node1 node14 node16} {key1 123 key2 abc} {}
32 33 {{} node2 node1 node14 node16 node18} {key1 myValue} {}
33 34 {{} node2 node1 node14 node16 node18 node20} {key1 myValue} {}
31 35 {{} node2 node1 node14 {}} {key1 myValue} {}
30 36 {{} node2 node1 node15} {key1 123} {}
36 37 {{} node2 node1 node15 node17} {key1 myValue} {}
37 38 {{} node2 node1 node15 node17 node19} {key1 myValue} {}
38 39 {{} node2 node1 node15 node17 node19 node21} {key1 myValue} {}
0 40 {{} node3} {key1 myValue} {}
40 41 {{} node3 node2} {key1 myValue} {}
41 42 {{} node3 node2 node1} {key1 myValue} {}
42 43 {{} node3 node2 node1 node14} {key1 myValue} {}
43 44 {{} node3 node2 node1 node14 node16} {key1 123 key2 abc} {}
44 45 {{} node3 node2 node1 node14 node16 node18} {key1 myValue} {}
45 46 {{} node3 node2 node1 node14 node16 node18 node20} {key1 myValue} {}
43 47 {{} node3 node2 node1 node14 {}} {key1 myValue} {}
42 48 {{} node3 node2 node1 node15} {key1 123} {}
48 49 {{} node3 node2 node1 node15 node17} {key1 myValue} {}
49 50 {{} node3 node2 node1 node15 node17 node19} {key1 myValue} {}
50 51 {{} node3 node2 node1 node15 node17 node19 node21} {key1 myValue} {}
0 52 {{} node4} {key1 myValue} {}
0 53 {{} node5} {key1 myValue} {}
53 54 {{} node5 node13} {key1 myValue} {}
0 55 {{} node6} {key1 myValue} {}
0 56 {{} node14} {key1 myValue} {}
}}

test parseargs.567 {parseargs dir (no recurse flag)} {
    list [catch {
	file delete -force ./testdir
	file mkdir ./testdir/dir1
	file mkdir ./testdir/dir2
	file mkdir ./testdir/dir3
	file copy defs ./testdir/dir1
	set parseargs [blt::parseargs create]
	$parseargs dir 0 ./testdir \
	    -fields { perms type } \
	    -pattern defs \
	    -recurse
	set contents [$parseargs dump 0 -version 2.0]
	blt::parseargs destroy $parseargs
	file delete -force ./testdir
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 2 {{} dir1} {perms 493 type directory} {}
2 3 {{} dir1 defs} {perms 420 type file} {}
}}


test parseargs.568 {parseargs dir -recurse} {
    list [catch {
	file delete -force ./testdir
	file mkdir ./testdir/.dir0
	file mkdir ./testdir/dir1/dir2
	file copy defs ./testdir/dir1/dir2
	set parseargs [blt::parseargs create]
	$parseargs dir 0 ./testdir  -recurse -fields { perms type } -type f
	set contents [$parseargs dump 0 -version 2.0]
	blt::parseargs destroy $parseargs
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} dir1} {perms 493 type directory} {}
1 2 {{} dir1 dir2} {perms 493 type directory} {}
2 3 {{} dir1 dir2 defs} {perms 420 type file} {}
}}



test parseargs.569 {parseargs dir -recurse} {
    list [catch {
	file delete -force ./testdir
	file mkdir ./testdir
	file copy defs ./testdir
	set parseargs [blt::parseargs create]
	$parseargs dir 0 ./testdir -recurse -fields { size perms type }
	set contents [$parseargs dump 0 -version 2.0]
	blt::parseargs destroy $parseargs
	file delete -force ./testdir
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} defs} {size 2894 perms 420 type file} {}
}}

test parseargs.570 {parseargs dir (default settings, no -recurse)} {
    list [catch {
	file delete -force ./testdir
	file mkdir ./testdir/dir1
	file copy defs ./testdir/dir1
	set parseargs [blt::parseargs create]
	$parseargs dir 0 ./testdir -fields { perms type }
	set contents [$parseargs dump 0 -version 2.0]
	blt::parseargs destroy $parseargs
	file delete -force ./testdir
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} dir1} {perms 493 type directory} {}
}}

test parseargs.571 {parseargs dir -type "file pipe"} {
    list [catch {
	set parseargs [blt::parseargs create]
	$parseargs dir 0 /dev -fields { size perms type } -type "file pipe"
	set contents [$parseargs dump 0 -version 2.0]
	blt::parseargs destroy $parseargs
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} core} {size 140737477881856 perms 256 type file} {}
0 2 {{} stderr} {size 0 perms 384 type fifo} {}
0 3 {{} stdout} {size 0 perms 384 type fifo} {}
0 4 {{} initctl} {size 0 perms 384 type fifo} {}
}}

test parseargs.572 {parseargs dir -type link -recurse} {
    list [catch {
	file delete -force ./testdir
	file mkdir ./testdir/dir1
	file copy defs ./testdir/dir1
	file link -symbolic [pwd]/testdir/mylink [pwd]/testdir/dir1/defs 
	set parseargs [blt::parseargs create]
	$parseargs dir 0 ./testdir -fields { size perms type } -type "link" -recurse
	set contents [$parseargs dump 0 -version 2.0]
	blt::parseargs destroy $parseargs
	file delete -force ./testdir
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 2 {{} mylink} {size 2894 perms 420 type file} {}
}}

exit 0



















