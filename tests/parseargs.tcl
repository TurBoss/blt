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

test parseargs.14 {parseargs names} {
    list [catch {blt::parseargs names} msg] [lsort $msg]
} {0 {::fred ::parseargs0 ::parseargs1}}

test parseargs.15 {parseargs names pattern)} {
    list [catch {blt::parseargs names ::parseargs*} msg] [lsort $msg]
} {0 {::parseargs0 ::parseargs1}}

test parseargs.16 {parseargs names badPattern)} {
    list [catch {blt::parseargs names badPattern*} msg] $msg
} {0 {}}

test parseargs.17 {parseargs names pattern arg (wrong # args)} {
    list [catch {blt::parseargs names pattern arg} msg] $msg
} {1 {wrong # args: should be "blt::parseargs names ?pattern ...?"}}

test parseargs.18 {parseargs destroy (no args)} {
    list [catch {blt::parseargs destroy} msg] $msg
} {0 {}}

test parseargs.19 {parseargs destroy badParseargs} {
    list [catch {blt::parseargs destroy badParseargs} msg] $msg
} {1 {can't find a parser named "badParseargs"}}

test parseargs.20 {parseargs destroy fred} {
    list [catch {blt::parseargs destroy fred} msg] $msg
} {0 {}}

test parseargs.21 {parseargs destroy parseargs0 parseargs1} {
    list [catch {blt::parseargs destroy parseargs0 parseargs1} msg] $msg
} {0 {}}

test parseargs.22 {create} {
    list [catch {blt::parseargs create} msg] $msg
} {0 ::parseargs0}

test parseargs.23 {parseargs0} {
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

test parseargs.24 {parseargs0 badOp} {
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

test parseargs.25 {parseargs0 add (wrong # args)} {
    list [catch {parseargs0 add} msg] $msg
} {1 {wrong # args: should be "parseargs0 add argName ?switches ...?"}}

test parseargs.26 {parseargs0 add ""} {
    list [catch {parseargs0 add ""} msg] $msg
} {0 {}}

test parseargs.27 {parseargs0 delete ""} {
    list [catch {parseargs0 delete ""} msg] $msg
} {0 {}}

test parseargs.28 {parseargs0 add "newArg"} {
    list [catch {parseargs0 add "newArg"} msg] $msg
} {0 newArg}

test parseargs.29 {parseargs0 names} {
    list [catch {parseargs0 names} msg] [lsort $msg]
} {0 newArg}

test parseargs.30 {parseargs0 add "newArg"} {
    list [catch {parseargs0 add "newArg"} msg] $msg
} {1 {argument "newArg" already exists in the parser}}

test parseargs.31 {parseargs0 delete "newArg"} {
    list [catch {parseargs0 delete "newArg"} msg] $msg
} {0 {}}

test parseargs.32 {parseargs0 names} {
    list [catch {parseargs0 names} msg] [lsort $msg]
} {0 {}}

test parseargs.33 {parseargs0 add "newArg" -badSwitch } {
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
   -value value}}

test parseargs.34 {parseargs0 names} {
    list [catch {parseargs0 names} msg] [lsort $msg]
} {0 {}}

test parseargs.35 {parseargs0 add newArg} {
    list [catch {parseargs0 add newArg} msg] $msg
} {0 newArg}

test parseargs.36 {parseargs0 argument} {
    list [catch {parseargs0 argument} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument argName args..."}}

test parseargs.37 {parseargs0 argument badOp} {
    list [catch {parseargs0 argument badOp} msg] $msg
} {1 {bad operation "badOp": should be one of...
  parseargs0 argument cget argName option
  parseargs0 argument configure argName ?value ...?}}

test parseargs.38 {parseargs0 argument cget (missing arg)} {
    list [catch {parseargs0 argument cget} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.39 {parseargs0 argument cget badArg} {
    list [catch {parseargs0 argument cget badArg} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.40 {parseargs0 argument cget newArg} {
    list [catch {parseargs0 argument cget newArg} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.41 {parseargs0 argument cget badArg badOption} {
    list [catch {parseargs0 argument cget badArg badOption} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.42 {parseargs0 argument cget badArg badOption extraArg} {
    list [catch {parseargs0 argument cget badArg badOption extraArg} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.43 {parseargs0 argument cget newArg badOption} {
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
   -value value}}

test parseargs.44 {parseargs0 argument cget newArg -action} {
    list [catch {parseargs0 argument cget newArg -action} msg] $msg
} {0 store}

test parseargs.45 {parseargs0 argument cget newArg -command} {
    list [catch {parseargs0 argument cget newArg -command} msg] $msg
} {0 {}}

test parseargs.46 {parseargs0 argument cget newArg -choices} {
    list [catch {parseargs0 argument cget newArg -choices} msg] $msg
} {0 {}}

test parseargs.47 {parseargs0 argument cget newArg -default} {
    list [catch {parseargs0 argument cget newArg -default} msg] $msg
} {0 {}}

test parseargs.48 {parseargs0 argument cget newArg -description} {
    list [catch {parseargs0 argument cget newArg -description} msg] $msg
} {0 {}}

test parseargs.49 {parseargs0 argument cget newArg -exclude} {
    list [catch {parseargs0 argument cget newArg -exclude} msg] $msg
} {0 {}}

test parseargs.50 {parseargs0 argument cget newArg -help} {
    list [catch {parseargs0 argument cget newArg -help} msg] $msg
} {0 {}}

test parseargs.51 {parseargs0 argument cget newArg -long} {
    list [catch {parseargs0 argument cget newArg -long} msg] $msg
} {0 {}}

test parseargs.52 {parseargs0 argument cget newArg -metavar} {
    list [catch {parseargs0 argument cget newArg -metavar} msg] $msg
} {0 {}}

test parseargs.53 {parseargs0 argument cget newArg -max} {
    list [catch {parseargs0 argument cget newArg -max} msg] $msg
} {0 {}}

test parseargs.54 {parseargs0 argument cget newArg -min} {
    list [catch {parseargs0 argument cget newArg -min} msg] $msg
} {0 {}}

test parseargs.55 {parseargs0 argument cget newArg -nargs} {
    list [catch {parseargs0 argument cget newArg -nargs} msg] $msg
} {0 1}

test parseargs.56 {parseargs0 argument cget newArg -required} {
    list [catch {parseargs0 argument cget newArg -required} msg] $msg
} {0 0}

test parseargs.57 {parseargs0 argument cget newArg -short} {
    list [catch {parseargs0 argument cget newArg -short} msg] $msg
} {0 {}}

test parseargs.58 {parseargs0 argument cget newArg -type} {
    list [catch {parseargs0 argument cget newArg -type} msg] $msg
} {0 string}

test parseargs.59 {parseargs0 argument cget newArg -variable} {
    list [catch {parseargs0 argument cget newArg -variable} msg] $msg
} {0 {}}

test parseargs.60 {parseargs0 argument cget newArg -value} {
    list [catch {parseargs0 argument cget newArg -value} msg] $msg
} {0 {}}

test parseargs.61 {parseargs0 argument configure} {
    list [catch {parseargs0 argument configure} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument configure argName ?value ...?"}}

test parseargs.62 {parseargs0 argument configure badArg} {
    list [catch {parseargs0 argument configure badArg} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.63 {parseargs0 argument configure newArg} {
    list [catch {parseargs0 argument configure newArg} msg] $msg
} {0 {{-action store store} {-command {} {}} {-choices {} {}} {-current {} {}} {-default {} {}} {-description {} {}} {-exclude {} {}} {-help {} {}} {-long {} {}} {-metavar {} {}} {-max {} {}} {-min {} {}} {-nargs 1 1} {-required 0 0} {-short {} {}} {-type string string} {-variable {} {}} {-value {} {}}}}

test parseargs.64 {parseargs0 argument configure badArg badOption} {
    list [catch {parseargs0 argument configure badArg badOption} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.65 {parseargs0 argument configure badArg badOption extraArg} {
    list [catch {parseargs0 argument configure badArg badOption extraArg} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.66 {parseargs0 argument configure newArg badOption} {
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
   -value value}}

test parseargs.67 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store}}

test parseargs.68 {parseargs0 argument configure newArg -action badValue} {
    list [catch {
	parseargs0 argument configure newArg -action badValue
    } msg] $msg
} {1 {unknown action "badValue": should be int, store, append, store_false, or store_true}}

test parseargs.69 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store}}

test parseargs.70 {parseargs0 argument configure newArg -action store} {
    list [catch {parseargs0 argument configure newArg -action store} msg] $msg
} {0 {}}

test parseargs.71 {parseargs0 argument configure newArg -action append} {
    list [catch {parseargs0 argument configure newArg -action append} msg] $msg
} {0 {}}

test parseargs.72 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store append}}

test parseargs.73 {parseargs0 argument configure newArg -action store_false} {
    list [catch {
	parseargs0 argument configure newArg -action store_false
    } msg] $msg
} {0 {}}

test parseargs.74 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store_false}}

test parseargs.75 {parseargs0 argument configure newArg -action store_true} {
    list [catch {
	parseargs0 argument configure newArg -action store_true
    } msg] $msg
} {0 {}}

test parseargs.76 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store_true}}

test parseargs.77 {parseargs0 argument configure newArg -action store} {
    list [catch {
	parseargs0 argument configure newArg -action store
    } msg] $msg
} {0 {}}

test parseargs.78 {parseargs0 argument configure newArg -action store} {
    list [catch {
	parseargs0 argument configure newArg -action store
    } msg] $msg
} {0 {}}

test parseargs.79 {parseargs0 argument configure newArg -action stor} {
    list [catch {
	parseargs0 argument configure newArg -action stor
    } msg] $msg
} {1 {unknown action "stor": should be int, store, append, store_false, or store_true}}

test parseargs.80 {parseargs0 argument configure newArg -action store_} {
    list [catch {
	parseargs0 argument configure newArg -action store_
    } msg] $msg
} {1 {unknown action "store_": should be int, store, append, store_false, or store_true}}

test parseargs.81 {parseargs0 argument configure newArg -action store_t} {
    list [catch {
	parseargs0 argument configure newArg -action store_t
    } msg] $msg
} {0 {}}

test parseargs.82 {parseargs0 argument configure newArg -action a} {
    list [catch {
	parseargs0 argument configure newArg -action a
    } msg] $msg
} {0 {}}

test parseargs.83 {parseargs0 argument configure newArg -action store} {
    list [catch {
	parseargs0 argument configure newArg -action store
    } msg] $msg
} {0 {}}

test parseargs.84 {parseargs0 argument configure newArg -action} {
    list [catch {parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store}}


test parseargs.85 {parseargs0 argument configure newArg -command} {
    list [catch {parseargs0 argument configure newArg -command} msg] $msg
} {0 {-command {} {}}}

test parseargs.86 {parseargs0 argument configure newArg -command cmdString} {
    list [catch {
	parseargs0 argument configure newArg -command cmdString
    } msg] $msg
} {0 {}}

test parseargs.87 {parseargs0 argument configure newArg -command} {
    list [catch {parseargs0 argument configure newArg -command} msg] $msg
} {0 {-command {} cmdString}}

test parseargs.88 {parseargs0 argument configure newArg -command ""} {
    list [catch {
	parseargs0 argument configure newArg -command ""
    } msg] $msg
} {0 {}}

test parseargs.89 {parseargs0 argument configure newArg -command} {
    list [catch {parseargs0 argument configure newArg -command} msg] $msg
} {0 {-command {} {}}}


test parseargs.90 {parseargs0 argument configure newArg -choices} {
    list [catch {parseargs0 argument configure newArg -choices} msg] $msg
} {0 {-choices {} {}}}

test parseargs.91 {parseargs0 argument configure newArg -choices "a b c"} {
    list [catch {
	parseargs0 argument configure newArg -choices "a b c"
    } msg] $msg
} {0 {}}

test parseargs.92 {parseargs0 argument configure newArg -choices} {
    list [catch {parseargs0 argument configure newArg -choices} msg] $msg
} {0 {-choices {} {a b c}}}

test parseargs.93 {parseargs0 argument configure newArg -choices ""} {
    list [catch {
	parseargs0 argument configure newArg -choices ""
    } msg] $msg
} {0 {}}

test parseargs.94 {parseargs0 argument configure newArg -choices} {
    list [catch {parseargs0 argument configure newArg -choices} msg] $msg
} {0 {-choices {} {}}}

test parseargs.95 {parseargs0 argument configure newArg -default} {
    list [catch {parseargs0 argument configure newArg -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.96 {parseargs0 argument configure newArg -default defValue} {
    list [catch {
	parseargs0 argument configure newArg -default defValue
    } msg] $msg
} {0 {}}

test parseargs.97 {parseargs0 argument configure newArg -default} {
    list [catch {parseargs0 argument configure newArg -default} msg] $msg
} {0 {-default {} defValue}}

test parseargs.98 {parseargs0 argument configure newArg -default ""} {
    list [catch {
	parseargs0 argument configure newArg -default ""
    } msg] $msg
} {0 {}}

test parseargs.99 {parseargs0 argument configure newArg -default} {
    list [catch {parseargs0 argument configure newArg -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.100 {parseargs0 argument configure newArg -description} {
    list [catch {parseargs0 argument configure newArg -description} msg] $msg
} {0 {-description {} {}}}

test parseargs.101 {parseargs0 argument configure newArg -description desc} {
    list [catch {
	parseargs0 argument configure newArg -description desc
    } msg] $msg
} {0 {}}

test parseargs.102 {parseargs0 argument configure newArg -description} {
    list [catch {parseargs0 argument configure newArg -description} msg] $msg
} {0 {-description {} desc}}

test parseargs.103 {parseargs0 argument configure newArg -description ""} {
    list [catch {
	parseargs0 argument configure newArg -description ""
    } msg] $msg
} {0 {}}

test parseargs.104 {parseargs0 argument configure newArg -description} {
    list [catch {parseargs0 argument configure newArg -description} msg] $msg
} {0 {-description {} {}}}

test parseargs.105 {parseargs0 argument configure newArg -description} {
    list [catch {parseargs0 argument configure newArg -description} msg] $msg
} {0 {-description {} {}}}

test parseargs.106 {parseargs0 argument configure newArg -exclude} {
    list [catch {parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} {}}}

test parseargs.107 {parseargs0 argument configure newArg -exclude arg} {
    list [catch {
	parseargs0 argument configure newArg -exclude arg
    } msg] $msg
} {0 {}}

test parseargs.108 {parseargs0 argument configure newArg -exclude} {
    list [catch {parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} arg}}

test parseargs.109 {parseargs0 argument configure newArg -exclude ""} {
    list [catch {
	parseargs0 argument configure newArg -exclude ""
    } msg] $msg
} {0 {}}

test parseargs.110 {parseargs0 argument configure newArg -exclude} {
    list [catch {parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} {}}}

test parseargs.111 {parseargs0 argument configure newArg -exclude} {
    list [catch {parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} {}}}

test parseargs.112 {parseargs0 argument configure newArg -help} {
    list [catch {parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.113 {parseargs0 argument configure newArg -help help} {
    list [catch {
	parseargs0 argument configure newArg -help help
    } msg] $msg
} {0 {}}

test parseargs.114 {parseargs0 argument configure newArg -help} {
    list [catch {parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} help}}

test parseargs.115 {parseargs0 argument configure newArg -help ""} {
    list [catch {
	parseargs0 argument configure newArg -help ""
    } msg] $msg
} {0 {}}

test parseargs.116 {parseargs0 argument configure newArg -help} {
    list [catch {parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.117 {parseargs0 argument configure newArg -help} {
    list [catch {parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.118 {parseargs0 argument configure newArg -long} {
    list [catch {parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} {}}}

test parseargs.119 {parseargs0 argument configure newArg -long --long} {
    list [catch {
	parseargs0 argument configure newArg -long --long
    } msg] $msg
} {0 {}}

test parseargs.120 {parseargs0 argument configure newArg -long} {
    list [catch {parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} --long}}

test parseargs.121 {parseargs0 argument configure newArg -long ""} {
    list [catch {
	parseargs0 argument configure newArg -long ""
    } msg] $msg
} {0 {}}

test parseargs.122 {parseargs0 argument configure newArg -long} {
    list [catch {parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} {}}}

test parseargs.123 {parseargs0 argument configure newArg -long} {
    list [catch {parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} {}}}

test parseargs.124 {parseargs0 argument configure newArg -metavar} {
    list [catch {parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} {}}}

test parseargs.125 {parseargs0 argument configure newArg -metavar VAR} {
    list [catch {
	parseargs0 argument configure newArg -metavar VAR
    } msg] $msg
} {0 {}}

test parseargs.126 {parseargs0 argument configure newArg -metavar} {
    list [catch {parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} VAR}}

test parseargs.127 {parseargs0 argument configure newArg -metavar ""} {
    list [catch {
	parseargs0 argument configure newArg -metavar ""
    } msg] $msg
} {0 {}}

test parseargs.128 {parseargs0 argument configure newArg -metavar} {
    list [catch {parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} {}}}

test parseargs.129 {parseargs0 argument configure newArg -metavar} {
    list [catch {parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} {}}}

test parseargs.130 {parseargs0 argument configure newArg -max} {
    list [catch {parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} {}}}

test parseargs.131 {parseargs0 argument configure newArg -max VAR} {
    list [catch {
	parseargs0 argument configure newArg -max VAR
    } msg] $msg
} {0 {}}

test parseargs.132 {parseargs0 argument configure newArg -max} {
    list [catch {parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} VAR}}

test parseargs.133 {parseargs0 argument configure newArg -max ""} {
    list [catch {
	parseargs0 argument configure newArg -max ""
    } msg] $msg
} {0 {}}

test parseargs.134 {parseargs0 argument configure newArg -max} {
    list [catch {parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} {}}}

test parseargs.135 {parseargs0 argument configure newArg -max} {
    list [catch {parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} {}}}

test parseargs.136 {parseargs0 argument configure newArg -min} {
    list [catch {parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} {}}}

test parseargs.137 {parseargs0 argument configure newArg -min VAR} {
    list [catch {
	parseargs0 argument configure newArg -min VAR
    } msg] $msg
} {0 {}}

test parseargs.138 {parseargs0 argument configure newArg -min} {
    list [catch {parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} VAR}}

test parseargs.139 {parseargs0 argument configure newArg -min ""} {
    list [catch {
	parseargs0 argument configure newArg -min ""
    } msg] $msg
} {0 {}}

test parseargs.140 {parseargs0 argument configure newArg -min} {
    list [catch {parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} {}}}

test parseargs.141 {parseargs0 argument configure newArg -min} {
    list [catch {parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} {}}}


test parseargs.142 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 1}}

test parseargs.143 {parseargs0 argument configure newArg -nargs badNum} {
    list [catch {
	parseargs0 argument configure newArg -nargs badNum
    } msg] $msg
} {1 {invalid nargs "badNum": should be +, ?, *, or number}}

test parseargs.144 {parseargs0 argument configure newArg -nargs -1} {
    list [catch {parseargs0 argument configure newArg -nargs -1} msg] $msg
} {1 {invalid nargs "-1": should be +, ?, *, or number}}

test parseargs.145 {parseargs0 argument configure newArg -nargs ""} {
    list [catch {
	parseargs0 argument configure newArg -nargs ""
    } msg] $msg
} {1 {invalid nargs "": should be +, ?, *, or number}}

test parseargs.146 {parseargs0 argument configure newArg -nargs 0} {
    list [catch {parseargs0 argument configure newArg -nargs 0} msg] $msg
} {0 {}}

test parseargs.147 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 0}}

test parseargs.148 {parseargs0 argument configure newArg -nargs 100} {
    list [catch {parseargs0 argument configure newArg -nargs 100} msg] $msg
} {0 {}}

test parseargs.149 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 100}}

test parseargs.150 {parseargs0 argument configure newArg -nargs ?} {
    list [catch {parseargs0 argument configure newArg -nargs ?} msg] $msg
} {0 {}}

test parseargs.151 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 ?}}

test parseargs.152 {parseargs0 argument configure newArg -nargs +} {
    list [catch {parseargs0 argument configure newArg -nargs +} msg] $msg
} {0 {}}

test parseargs.153 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 +}}

test parseargs.154 {parseargs0 argument configure newArg -nargs *} {
    list [catch {parseargs0 argument configure newArg -nargs *} msg] $msg
} {0 {}}

test parseargs.155 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 *}}

test parseargs.156 {parseargs0 argument configure newArg -nargs 1} {
    list [catch {parseargs0 argument configure newArg -nargs 1} msg] $msg
} {0 {}}

test parseargs.157 {parseargs0 argument configure newArg -nargs} {
    list [catch {parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 1}}

test parseargs.158 {parseargs0 argument configure newArg -required badBool} {
    list [catch {
	parseargs0 argument configure newArg -required badBool
    } msg] $msg
} {1 {expected boolean value but got "badBool"}}

test parseargs.159 {parseargs0 argument configure newArg -required 1} {
    list [catch {parseargs0 argument configure newArg -required 1} msg] $msg
} {0 {}}

test parseargs.160 {parseargs0 argument configure newArg -required true} {
    list [catch {parseargs0 argument configure newArg -required true} msg] $msg
} {0 {}}

test parseargs.161 {parseargs0 argument configure newArg -required yes} {
    list [catch {parseargs0 argument configure newArg -required yes} msg] $msg
} {0 {}}

test parseargs.162 {parseargs0 argument configure newArg -required on} {
    list [catch {parseargs0 argument configure newArg -required on} msg] $msg
} {0 {}}

test parseargs.163 {parseargs0 argument configure newArg -required 0} {
    list [catch {parseargs0 argument configure newArg -required 0} msg] $msg
} {0 {}}

test parseargs.164 {parseargs0 argument configure newArg -required false} {
    list [catch {parseargs0 argument configure newArg -required false} msg] $msg
} {0 {}}

test parseargs.165 {parseargs0 argument configure newArg -required no} {
    list [catch {parseargs0 argument configure newArg -required no} msg] $msg
} {0 {}}

test parseargs.166 {parseargs0 argument configure newArg -required off} {
    list [catch {parseargs0 argument configure newArg -required off} msg] $msg
} {0 {}}

test parseargs.167 {parseargs0 argument configure newArg -short} {
    list [catch {parseargs0 argument configure newArg -short} msg] $msg
} {0 {-short {} {}}}

test parseargs.168 {parseargs0 argument configure newArg -short -s} {
    list [catch {
	parseargs0 argument configure newArg -short -s
    } msg] $msg
} {0 {}}

test parseargs.169 {parseargs0 argument configure newArg -short} {
    list [catch {parseargs0 argument configure newArg -short} msg] $msg
} {0 {-short {} -s}}

test parseargs.170 {parseargs0 argument configure newArg -short ""} {
    list [catch {
	parseargs0 argument configure newArg -short ""
    } msg] $msg
} {0 {}}

test parseargs.171 {parseargs0 argument configure newArg -short} {
    list [catch {parseargs0 argument configure newArg -short} msg] $msg
} {0 {-short {} {}}}

test parseargs.172 {parseargs0 argument configure newArg -type} {
    list [catch {parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string string}}

test parseargs.173 {parseargs0 argument configure newArg -type badType} {
    list [catch {parseargs0 argument configure newArg -type badType} msg] $msg
} {1 {unknown argument type "badType": should be int, double, string, or boolean}}

test parseargs.174 {parseargs0 argument configure newArg -type int} {
    list [catch {parseargs0 argument configure newArg -type int} msg] $msg
} {0 {}}

test parseargs.175 {parseargs0 argument configure newArg -type} {
    list [catch {parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string int}}

test parseargs.176 {parseargs0 argument configure newArg -type double} {
    list [catch {parseargs0 argument configure newArg -type double} msg] $msg
} {0 {}}

test parseargs.177 {parseargs0 argument configure newArg -type float} {
    list [catch {parseargs0 argument configure newArg -type float} msg] $msg
} {0 {}}

test parseargs.178 {parseargs0 argument configure newArg -type number} {
    list [catch {parseargs0 argument configure newArg -type number} msg] $msg
} {0 {}}

test parseargs.179 {parseargs0 argument configure newArg -type boolean} {
    list [catch {parseargs0 argument configure newArg -type boolean} msg] $msg
} {0 {}}

test parseargs.180 {parseargs0 argument configure newArg -type} {
    list [catch {parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string boolean}}

test parseargs.181 {parseargs0 argument configure newArg -type string} {
    list [catch {parseargs0 argument configure newArg -type string} msg] $msg
} {0 {}}

test parseargs.182 {parseargs0 argument configure newArg -type} {
    list [catch {parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string string}}

test parseargs.183 {parseargs0 argument configure newArg -variable} {
    list [catch {parseargs0 argument configure newArg -variable} msg] $msg
} {0 {-variable {} {}}}

test parseargs.184 {parseargs0 argument configure newArg -variable myVar} {
    list [catch {
	parseargs0 argument configure newArg -variable myVar
    } msg] $msg
} {0 {}}

test parseargs.185 {parseargs0 argument configure newArg -variable} {
    list [catch {parseargs0 argument configure newArg -variable} msg] $msg
} {0 {-variable {} myVar}}

test parseargs.186 {parseargs0 argument configure newArg -variable ""} {
    list [catch {
	parseargs0 argument configure newArg -variable ""
    } msg] $msg
} {0 {}}

test parseargs.187 {parseargs0 argument configure newArg -variable} {
    list [catch {parseargs0 argument configure newArg -variable} msg] $msg
} {0 {-variable {} {}}}

test parseargs.188 {parseargs0 argument configure newArg -value} {
    list [catch {parseargs0 argument configure newArg -value} msg] $msg
} {0 {-value {} {}}}

test parseargs.189 {parseargs0 argument configure newArg -value constValue} {
    list [catch {
	parseargs0 argument configure newArg -value constValue
    } msg] $msg
} {0 {}}

test parseargs.190 {parseargs0 argument configure newArg -value} {
    list [catch {parseargs0 argument configure newArg -value} msg] $msg
} {0 {-value {} constValue}}

test parseargs.191 {parseargs0 argument configure newArg -value ""} {
    list [catch {
	parseargs0 argument configure newArg -value ""
    } msg] $msg
} {0 {}}

test parseargs.192 {parseargs0 argument configure newArg -value} {
    list [catch {parseargs0 argument configure newArg -value} msg] $msg
} {0 {-value {} {}}}

test parseargs.193 {parseargs0 cget (missing arg)} {
    list [catch {parseargs0 cget} msg] $msg
} {1 {wrong # args: should be "parseargs0 cget option"}}

test parseargs.194 {parseargs0 cget badOption extraArg} {
    list [catch {parseargs0 cget badArg badOption extraArg} msg] $msg
} {1 {wrong # args: should be "parseargs0 cget option"}}

test parseargs.195 {parseargs0 cget badOption} {
    list [catch {parseargs0 cget badOption} msg] $msg
} {1 {unknown switch "badOption"
The following switches are available:
   -addhelp bool
   -abbreviations bool
   -default string
   -epilog string
   -error list
   -help string
   -prefixchars string
   -program programName
   -usage string}}

test parseargs.196 {parseargs0 cget -addhelp} {
    list [catch {parseargs0 cget -addhelp} msg] $msg
} {0 1}

test parseargs.197 {parseargs0 cget -abbreviations} {
    list [catch {parseargs0 cget -abbreviations} msg] $msg
} {0 0}

test parseargs.198 {parseargs0 cget -default} {
    list [catch {parseargs0 cget -default} msg] $msg
} {0 {}}

test parseargs.199 {parseargs0 cget -epilog} {
    list [catch {parseargs0 cget -epilog} msg] $msg
} {0 {}}

test parseargs.200 {parseargs0 cget -error} {
    list [catch {parseargs0 cget -error} msg] $msg
} {0 badoption}

test parseargs.201 {parseargs0 cget -help} {
    list [catch {parseargs0 cget -help} msg] $msg
} {0 {}}

test parseargs.202 {parseargs0 cget -prefixchars} {
    list [catch {parseargs0 cget -prefixchars} msg] $msg
} {0 -+}

test parseargs.203 {parseargs0 cget -program} {
    list [catch {parseargs0 cget -program} msg] $msg
} {0 {}}

test parseargs.204 {parseargs0 cget -usage} {
    list [catch {parseargs0 cget -usage} msg] $msg
} {0 {}}


test parseargs.205 {parseargs0 configure} {
    list [catch {parseargs0 configure} msg] $msg
} {0 {{-addhelp 1 1} {-abbreviations 0 0} {-default {} {}} {-epilog {} {}} {-error badoption badoption} {-help {} {}} {-prefixchars -+ -+} {-program {} {}} {-usage {} {}}}}


test parseargs.206 {parseargs0 configure badOption} {
    list [catch {parseargs0 configure badOption} msg] $msg
} {1 {unknown switch "badOption"
The following switches are available:
   -addhelp bool
   -abbreviations bool
   -default string
   -epilog string
   -error list
   -help string
   -prefixchars string
   -program programName
   -usage string}}

test parseargs.207 {parseargs0 configure -addhelp} {
    list [catch {parseargs0 configure -addhelp} msg] $msg
} {0 {-addhelp 1 1}}

test parseargs.208 {parseargs0 configure -abbreviations} {
    list [catch {parseargs0 configure -abbreviations} msg] $msg
} {0 {-abbreviations 0 0}}

test parseargs.209 {parseargs0 configure -default} {
    list [catch {parseargs0 configure -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.210 {parseargs0 configure -epilog} {
    list [catch {parseargs0 configure -epilog} msg] $msg
} {0 {-epilog {} {}}}

test parseargs.211 {parseargs0 configure -error} {
    list [catch {parseargs0 configure -error} msg] $msg
} {0 {-error badoption badoption}}

test parseargs.212 {parseargs0 configure -help} {
    list [catch {parseargs0 configure -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.213 {parseargs0 configure -prefixchars} {
    list [catch {parseargs0 configure -prefixchars} msg] $msg
} {0 {-prefixchars -+ -+}}

test parseargs.214 {parseargs0 configure -program} {
    list [catch {parseargs0 configure -program} msg] $msg
} {0 {-program {} {}}}

test parseargs.215 {parseargs0 configure -usage} {
    list [catch {parseargs0 configure -usage} msg] $msg
} {0 {-usage {} {}}}

test parseargs.216 {parseargs0 configure -addhelp badBool} {
    list [catch {parseargs0 configure -addhelp badBool} msg] $msg
} {1 {expected boolean value but got "badBool"}}

test parseargs.217 {parseargs0 configure -addhelp 1} {
    list [catch {parseargs0 configure -addhelp 1} msg] $msg
} {0 {}}

test parseargs.218 {parseargs0 configure -addhelp true} {
    list [catch {parseargs0 configure -addhelp true} msg] $msg
} {0 {}}

test parseargs.219 {parseargs0 configure -addhelp yes} {
    list [catch {parseargs0 configure -addhelp yes} msg] $msg
} {0 {}}

test parseargs.220 {parseargs0 configure -addhelp on} {
    list [catch {parseargs0 configure -addhelp on} msg] $msg
} {0 {}}

test parseargs.221 {parseargs0 configure -addhelp} {
    list [catch {parseargs0 configure -addhelp} msg] $msg
} {0 {-addhelp 1 1}}

test parseargs.222 {parseargs0 configure -addhelp 0} {
    list [catch {parseargs0 configure -addhelp 0} msg] $msg
} {0 {}}

test parseargs.223 {parseargs0 configure -addhelp false} {
    list [catch {parseargs0 configure -addhelp false} msg] $msg
} {0 {}}

test parseargs.224 {parseargs0 configure -addhelp no} {
    list [catch {parseargs0 configure -addhelp no} msg] $msg
} {0 {}}

test parseargs.225 {parseargs0 configure -addhelp off} {
    list [catch {parseargs0 configure -addhelp off} msg] $msg
} {0 {}}

test parseargs.226 {parseargs0 configure -addhelp} {
    list [catch {parseargs0 configure -addhelp} msg] $msg
} {0 {-addhelp 1 0}}

test parseargs.227 {parseargs0 configure -addhelp 1} {
    list [catch {parseargs0 configure -addhelp 1} msg] $msg
} {0 {}}

test parseargs.228 {parseargs0 configure -abbreviations badBool} {
    list [catch {parseargs0 configure -abbreviations badBool} msg] $msg
} {1 {expected boolean value but got "badBool"}}

test parseargs.229 {parseargs0 configure -abbreviations 1} {
    list [catch {parseargs0 configure -abbreviations 1} msg] $msg
} {0 {}}

test parseargs.230 {parseargs0 configure -abbreviations true} {
    list [catch {parseargs0 configure -abbreviations true} msg] $msg
} {0 {}}

test parseargs.231 {parseargs0 configure -abbreviations yes} {
    list [catch {parseargs0 configure -abbreviations yes} msg] $msg
} {0 {}}

test parseargs.232 {parseargs0 configure -abbreviations on} {
    list [catch {parseargs0 configure -abbreviations on} msg] $msg
} {0 {}}

test parseargs.233 {parseargs0 configure -abbreviations} {
    list [catch {parseargs0 configure -abbreviations} msg] $msg
} {0 {-abbreviations 0 1}}

test parseargs.234 {parseargs0 configure -abbreviations 0} {
    list [catch {parseargs0 configure -abbreviations 0} msg] $msg
} {0 {}}

test parseargs.235 {parseargs0 configure -abbreviations false} {
    list [catch {parseargs0 configure -abbreviations false} msg] $msg
} {0 {}}

test parseargs.236 {parseargs0 configure -abbreviations no} {
    list [catch {parseargs0 configure -abbreviations no} msg] $msg
} {0 {}}

test parseargs.237 {parseargs0 configure -abbreviations off} {
    list [catch {parseargs0 configure -abbreviations off} msg] $msg
} {0 {}}

test parseargs.238 {parseargs0 configure -abbreviations} {
    list [catch {parseargs0 configure -abbreviations} msg] $msg
} {0 {-abbreviations 0 0}}

test parseargs.239 {parseargs0 configure -default} {
    list [catch {parseargs0 configure -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.240 {parseargs0 configure -default defValue} {
    list [catch {
	parseargs0 configure -default defValue
    } msg] $msg
} {0 {}}

test parseargs.241 {parseargs0 configure -default} {
    list [catch {parseargs0 configure -default} msg] $msg
} {0 {-default {} defValue}}

test parseargs.242 {parseargs0 configure -default ""} {
    list [catch {
	parseargs0 configure -default ""
    } msg] $msg
} {0 {}}

test parseargs.243 {parseargs0 configure -default} {
    list [catch {parseargs0 configure -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.244 {parseargs0 configure -epilog} {
    list [catch {parseargs0 configure -epilog} msg] $msg
} {0 {-epilog {} {}}}

test parseargs.245 {parseargs0 configure -epilog defValue} {
    list [catch {
	parseargs0 configure -epilog defValue
    } msg] $msg
} {0 {}}

test parseargs.246 {parseargs0 configure -epilog} {
    list [catch {parseargs0 configure -epilog} msg] $msg
} {0 {-epilog {} defValue}}

test parseargs.247 {parseargs0 configure -epilog ""} {
    list [catch {
	parseargs0 configure -epilog ""
    } msg] $msg
} {0 {}}

test parseargs.248 {parseargs0 configure -epilog} {
    list [catch {parseargs0 configure -epilog} msg] $msg
} {0 {-epilog {} {}}}

test parseargs.249 {parseargs0 configure -help} {
    list [catch {parseargs0 configure -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.250 {parseargs0 configure -help defValue} {
    list [catch {
	parseargs0 configure -help defValue
    } msg] $msg
} {0 {}}

test parseargs.251 {parseargs0 configure -error badFlag} {
    list [catch {parseargs0 configure -error badFlag} msg] $msg
} {1 {unknown error flag "badFlag": should be badoption or extraargs}}

test parseargs.252 {parseargs0 configure -error badoption} {
    list [catch {parseargs0 configure -error badoption} msg] $msg
} {0 {}}

test parseargs.253 {parseargs0 configure -error} {
    list [catch {parseargs0 configure -error} msg] $msg
} {0 {-error badoption badoption}}

test parseargs.254 {parseargs0 configure -error extraargs} {
    list [catch {parseargs0 configure -error extraargs} msg] $msg
} {0 {}}

test parseargs.255 {parseargs0 configure -error} {
    list [catch {parseargs0 configure -error} msg] $msg
} {0 {-error badoption extraargs}}

test parseargs.256 {parseargs0 configure -error "extraargs badoption badFlag"} {
    list [catch {
	parseargs0 configure -error "extraargs badoption badFlag"
    } msg] $msg
} {1 {unknown error flag "badFlag": should be badoption or extraargs}}

test parseargs.257 {parseargs0 configure -error} {
    list [catch {parseargs0 configure -error} msg] $msg
} {0 {-error badoption extraargs}}

test parseargs.258 {parseargs0 configure -error "extraargs badoption"} {
    list [catch {
	parseargs0 configure -error "extraargs badoption"
    } msg] $msg
} {0 {}}


test parseargs.259 {parseargs0 configure -help} {
    list [catch {parseargs0 configure -help} msg] $msg
} {0 {-help {} defValue}}

test parseargs.260 {parseargs0 configure -help ""} {
    list [catch {
	parseargs0 configure -help ""
    } msg] $msg
} {0 {}}

test parseargs.261 {parseargs0 configure -help} {
    list [catch {parseargs0 configure -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.262 {parseargs0 configure -prefixchars} {
    list [catch {parseargs0 configure -prefixchars} msg] $msg
} {0 {-prefixchars -+ -+}}

test parseargs.263 {parseargs0 configure -prefixchars ?} {
    list [catch {
	parseargs0 configure -prefixchars ?
    } msg] $msg
} {0 {}}

test parseargs.264 {parseargs0 configure -prefixchars} {
    list [catch {parseargs0 configure -prefixchars} msg] $msg
} {0 {-prefixchars -+ ?}}

test parseargs.265 {parseargs0 configure -prefixchars ""} {
    list [catch {
	parseargs0 configure -prefixchars ""
    } msg] $msg
} {0 {}}

test parseargs.266 {parseargs0 configure -prefixchars} {
    list [catch {parseargs0 configure -prefixchars} msg] $msg
} {0 {-prefixchars -+ {}}}

test parseargs.267 {parseargs0 configure -prefixchars "+-"} {
    list [catch {
	parseargs0 configure -prefixchars "+-"
    } msg] $msg
} {0 {}}

test parseargs.268 {parseargs0 configure -prefixchars} {
    list [catch {parseargs0 configure -prefixchars} msg] $msg
} {0 {-prefixchars -+ +-}}

test parseargs.269 {parseargs0 configure -program} {
    list [catch {parseargs0 configure -program} msg] $msg
} {0 {-program {} {}}}

test parseargs.270 {parseargs0 configure -program defValue} {
    list [catch {
	parseargs0 configure -program defValue
    } msg] $msg
} {0 {}}

test parseargs.271 {parseargs0 configure -program} {
    list [catch {parseargs0 configure -program} msg] $msg
} {0 {-program {} defValue}}

test parseargs.272 {parseargs0 configure -program ""} {
    list [catch {
	parseargs0 configure -program ""
    } msg] $msg
} {0 {}}

test parseargs.273 {parseargs0 configure -program} {
    list [catch {parseargs0 configure -program} msg] $msg
} {0 {-program {} {}}}

test parseargs.274 {parseargs0 configure -usage} {
    list [catch {parseargs0 configure -usage} msg] $msg
} {0 {-usage {} {}}}

test parseargs.275 {parseargs0 configure -usage defValue} {
    list [catch {
	parseargs0 configure -usage defValue
    } msg] $msg
} {0 {}}

test parseargs.276 {parseargs0 configure -usage} {
    list [catch {parseargs0 configure -usage} msg] $msg
} {0 {-usage {} defValue}}

test parseargs.277 {parseargs0 configure -usage ""} {
    list [catch {
	parseargs0 configure -usage ""
    } msg] $msg
} {0 {}}

test parseargs.278 {parseargs0 configure -usage} {
    list [catch {parseargs0 configure -usage} msg] $msg
} {0 {-usage {} {}}}


test parseargs.279 {parseargs destroy parseargs0} {
    list [catch {blt::parseargs destroy parseargs0} msg] $msg
} {0 {}}

test parseargs.280 {parseargs0} {
    list [catch {parseargs0} msg] $msg
} {1 {invalid command name "parseargs0"}}

test parseargs.281 {blt::parseargs create -badOption 0} {
    list [catch {blt::parseargs create -badOption 0} msg] $msg
} {1 {unknown switch "-badOption"
The following switches are available:
   -addhelp bool
   -abbreviations bool
   -default string
   -epilog string
   -error list
   -help string
   -prefixchars string
   -program programName
   -usage string}}


test parseargs.282 {blt::parseargs create -default (missing arg)} {
    list [catch {blt::parseargs create -default} msg] $msg
} {1 {value for "-default" missing}}

test parseargs.283 {blt::parseargs create -addhelp (missing arg)} {
    list [catch {blt::parseargs create -addhelp} msg] $msg
} {1 {value for "-addhelp" missing}}

test parseargs.284 {blt::parseargs names} {
    list [catch {blt::parseargs names} msg] $msg
} {0 {}}

test parseargs.285 {blt::parseargs create myParser} {
    list [catch {blt::parseargs create myParser} msg] $msg
} {0 ::myParser}

test parseargs.286 {myParser add debug} {
    list [catch {
	myParser add debug -short -d -long --debug -type boolean -nargs ?
    } msg] $msg
} {0 debug}

test parseargs.287 {myParser names} {
    list [catch { myParser names } msg] $msg
} {0 debug}

test parseargs.288 {myParser add verbose} {
    list [catch {
	myParser add verbose -short -v -long --verbose -type int \
	    -nargs ? 
    } msg] $msg
} {0 verbose}

test parseargs.289 {myParser names} {
    list [catch { myParser names } msg] $msg
} {0 {debug verbose}}

test parseargs.290 {myParser parse "-v"} {
    list [catch { myParser parse "-v" } msg] $msg
} {0 {}}

test parseargs.291 {myParser parse "-v 0"} {
    list [catch { myParser parse "-v 0" } msg] $msg
} {0 {}}

test parseargs.292 {myParser parse "-v 1"} {
    list [catch { myParser parse "-v 1" } msg] $msg
} {0 {}}

test parseargs.293 {myParser parse "-v 10"} {
    list [catch { myParser parse "-v 10" } msg] $msg
} {0 {}}

test parseargs.294 {myParser parse "-v 11"} {
    list [catch { myParser parse "-v 11" } msg] $msg
} {0 {}}

test parseargs.295 {myParser parse "-v -11"} {
    list [catch { myParser parse "-v -11" } msg] $msg
} {0 {}}

test parseargs.296 {myParser parse "-v false"} {
    list [catch { myParser parse "-v false" } msg] $msg
} {1 {expected integer but got "false": bad value for "--verbose"}}

test parseargs.297 {myParser parse "-v 1 2 3"} {
    list [catch { myParser parse "-v 1 2 3" } msg] $msg
} {0 {2 3}}

test parseargs.298 {myParser parse "-v badInt"} {
    list [catch { myParser parse "-v badInt" } msg] $msg
} {1 {expected integer but got "badInt": bad value for "--verbose"}}

test parseargs.299 {myParser parse "-verbose 1"} {
    list [catch { myParser parse "-verbose 1" } msg] $msg
} {1 {unknown option "-verbose"}}

test parseargs.300 {myParser parse "--verbos 1"} {
    list [catch { myParser parse "--verbos 1" } msg] $msg
} {1 {unknown option "--verbos"}}

test parseargs.301 {myParser configure -abbreviations 1} {
    list [catch { myParser configure -abbreviations 1 } msg] $msg
} {0 {}}

test parseargs.302 {myParser parse "--verbos 1"} {
    list [catch { myParser parse "--verbos 1" } msg] $msg
} {0 {}}

test parseargs.303 {myParser configure -abbreviations 0} {
    list [catch { myParser configure -abbreviations 0 } msg] $msg
} {0 {}}

test parseargs.304 {myParser argument configure "verbose" -min 0 -max 10} {
    list [catch { 
	myParser argument configure "verbose" -min 0 -max 10 
    } msg] $msg
} {0 {}}

test parseargs.305 {myParser parse "--verbose 0"} {
    list [catch { myParser parse "--verbose 0" } msg] $msg
} {0 {}}

test parseargs.306 {myParser parse "--verbose 10"} {
    list [catch { myParser parse "--verbose 10" } msg] $msg
} {0 {}}

test parseargs.307 {myParser parse "--verbose 11"} {
    list [catch { myParser parse "--verbose 11" } msg] $msg
} {1 {value "11" is greater than maximium "10": bad value for "--verbose"}}

test parseargs.308 {myParser parse "--verbose -10"} {
    list [catch { myParser parse "--verbose -10" } msg] $msg
} {1 {value "-10" is less than minimum "0": bad value for "--verbose"}}

test parseargs.309 {myParser parse "--debug 0"} {
    list [catch { myParser parse "--debug 0" } msg] $msg
} {0 {}}

test parseargs.310 {myParser parse "--debug true"} {
    list [catch { myParser parse "--debug true" } msg] $msg
} {0 {}}

test parseargs.311 {myParser parse "--debug yes"} {
    list [catch { myParser parse "--debug yes" } msg] $msg
} {0 {}}

test parseargs.312 {myParser parse "--debug on"} {
    list [catch { myParser parse "--debug on" } msg] $msg
} {0 {}}

test parseargs.313 {myParser parse "--debug false"} {
    list [catch { myParser parse "--debug true" } msg] $msg
} {0 {}}

test parseargs.314 {myParser parse "--debug no"} {
    list [catch { myParser parse "--debug no" } msg] $msg
} {0 {}}

test parseargs.315 {myParser parse "--debug off"} {
    list [catch { myParser parse "--debug off" } msg] $msg
} {0 {}}

test parseargs.316 {myParser parse "--debug 0"} {
    list [catch { myParser parse "--debug 0" } msg] $msg
} {0 {}}

test parseargs.317 {myParser parse "--debug 1000"} {
    list [catch { myParser parse "--debug 1000" } msg] $msg
} {0 {}}

test parseargs.318 {myParser parse "--debug -1000"} {
    list [catch { myParser parse "--debug -1000" } msg] $msg
} {0 {}}

test parseargs.319 {myParser parse "--debug -next"} {
    list [catch { myParser parse "--debug -next" } msg] $msg
} {1 {unknown option "-next"}}

test parseargs.320 {myParser argument configure "verbose" -min 100 -max 10} {
    list [catch { 
	myParser argument configure "verbose" -min 100 -max 10 
    } msg] $msg
} {0 {}}

test parseargs.321 {myParser argument configure "verbose" -min 1 -max 10} {
    list [catch { 
	myParser argument configure "verbose" -min 1 -max 10 
    } msg] $msg
} {0 {}}

test parseargs.322 {myParser argument configure "verbose" -min abc -max def} {
    list [catch { 
	myParser argument configure "verbose" -min abc -max def 
    } msg] $msg
} {1 {expected integer but got "abc": bad minimum value for "--verbose"}}

test parseargs.323 {myParser argument configure "verbose" -min 0 -max 10} {
    list [catch { 
	myParser argument configure "verbose" -min 0 -max 10 
    } msg] $msg
} {0 {}}

test parseargs.324 {myParser configure -error ""} {
    list [catch { myParser configure -error "" } msg] $msg
} {0 {}}

test parseargs.325 {myParser parse "--debug -next"} {
    list [catch { myParser parse "--debug -next" } msg] $msg
} {0 -next}

test parseargs.326 {myParser add type} {
   list [catch { myParser add type -short -t -long -type \
		     -choices "fish fowl" -default fish 
    } msg] $msg
} {0 type}

test parseargs.327 {myParser parse "-type fish"} {
    list [catch { myParser parse "-type fish" } msg] $msg
} {0 {}}

test parseargs.328 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 fish}

test parseargs.329 {myParser parse "-type fowl"} {
    list [catch { myParser parse "-type fowl" } msg] $msg
} {0 {}}

test parseargs.330 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 fowl}

test parseargs.331 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 fowl}

test parseargs.332 {myParser parse "-type badValue"} {
    list [catch { myParser parse "-type badValue" } msg] $msg
} {1 {bad value "badValue", must be one of "fish fowl": bad value for "-type"}}

test parseargs.333 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 fowl}

test parseargs.334 {myParser argument configure "type"} {
    list [catch { 
	myParser argument configure "type" -type int -choices "1 5 10"
    } msg] $msg
} {0 {}}

test parseargs.335 {myParser parse "-type 5"} {
    list [catch { myParser parse "-type 5" } msg] $msg
} {0 {}}

test parseargs.336 {myParser parse "-type 1"} {
    list [catch { myParser parse "-type 1" } msg] $msg
} {0 {}}

test parseargs.337 {myParser parse "-type 10"} {
    list [catch { myParser parse "-type 10" } msg] $msg
} {0 {}}

test parseargs.338 {myParser parse "-type 7"} {
    list [catch { myParser parse "-type 7" } msg] $msg
} {1 {bad value "7", must be one of "1 5 10": bad value for "-type"}}

test parseargs.339 {myParser argument configure "type"} {
    list [catch { 
	myParser argument configure "type" -type float -choices "1 5 10"
    } msg] $msg
} {0 {}}

test parseargs.340 {myParser parse "-type 1e1"} {
    list [catch { myParser parse "-type 1e1" } msg] $msg
} {0 {}}

test parseargs.341 {myParser parse "-type 1 -type 10"} {
    list [catch { myParser parse "-type 1 -type 10" } msg] $msg
} {0 {}}

test parseargs.342 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 10}

test parseargs.343 {myParser parse "-type 1 -type 5 -type 10"} {
    list [catch { myParser parse "-type 1 -type 5 -type 10" } msg] $msg
} {0 {}}

test parseargs.344 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 10}


test parseargs.345 {myParser argument configure "type" -action append} {
    list [catch { myParser argument configure "type" -action append } msg] $msg
} {0 {}}

test parseargs.346 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 10}

test parseargs.347 {myParser parse "-type 1 -type 5 -type 10"} {
    list [catch { myParser parse "-type 1 -type 5 -type 10" } msg] $msg
} {0 {}}

test parseargs.348 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 {10 1 5 10}}

test parseargs.349 {myParser argument configure "type" -action store} {
    list [catch { myParser argument configure "type" -action store } msg] $msg
} {0 {}}

test parseargs.350 {myParser parse "-type 1e1"} {
    list [catch { myParser parse "-type 1e1" } msg] $msg
} {0 {}}

test parseargs.351 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 1e1}

test parseargs.352 {myParser argument configure "type" -action store_true} {
    list [catch {
	myParser argument configure "type" -action store_true
    } msg] $msg
} {0 {}}

test parseargs.353 {myParser parse "-type 5"} {
    list [catch { myParser parse "-type 5" } msg] $msg
} {0 {}}

test parseargs.354 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 1}

test parseargs.355 {myParser argument configure "type" -action store_false} {
    list [catch {
	myParser argument configure "type" -action store_false
    } msg] $msg
} {0 {}}

test parseargs.356 {myParser parse "-type 5"} {
    list [catch { myParser parse "-type 5" } msg] $msg
} {0 {}}

test parseargs.357 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 0}

test parseargs.358 {myParser argument configure "type" -action store} {
    list [catch {
	myParser argument configure "type" -action store -variable myVar
    } msg] $msg
} {0 {}}

test parseargs.359 {myParser parse "-type 5"} {
    list [catch { myParser parse "-type 5" } msg] $myVar
} {0 5}

set arrayvar(0) 1

test parseargs.360 {myParser argument configure "type" -variable badVar} {
    list [catch {
	myParser argument configure "type" -variable ::arrayvar
    } msg] $msg
} {0 {}}

test parseargs.361 {myParser parse "-type 5"} {
    list [catch { myParser parse "-type 5" } msg] $msg
} {1 {can't set "::arrayvar": variable is array}}

test parseargs.362 {myParser argument configure "type" -variable ""} {
    list [catch {
	myParser argument configure "type" -variable ""
    } msg] $msg
} {0 {}}


proc ProcessDate { value } {
    puts stderr "I'm in ProcessDate"
    return [clock scan $value -gmt yes]
}

test parseargs.363 {myParser add date} {
    list [catch {
	myParser add date -short -d -long -date -command ProcessDate 
    } msg] $msg
} {0 date}

test parseargs.364 {myParser parse "-date yesterday"} {
    list [catch { myParser parse "-date yesterday" } msg] $msg
} {0 {}}

test parseargs.365 {myParser parse "-date 1/1/1970"} {
    list [catch { myParser parse "-date 1/1/1970" } msg] $msg
} {0 {}}

test parseargs.366 {myParser argument configure "type" -required yes} {
    list [catch {
	myParser argument configure "type" -required yes
    } msg] $msg
} {0 {}}

test parseargs.367 {myParser reset} {
    list [catch { myParser reset } msg] $msg
} {0 {}}

test parseargs.368 {myParser parse "-date 1/1/1970"} {
    list [catch { myParser parse "-date 1/1/1970" } msg] $msg
} {1 {missing required argument "-type"}}

test parseargs.369 {myParser get "date"} {
    list [catch { myParser get "date" } msg] $msg
} {0 0}

test parseargs.370 {myParser exists "date"} {
    list [catch { myParser exists "date" } msg] $msg
} {0 1}

test parseargs.371 {myParser exists "type"} {
    list [catch { myParser exists "type" } msg] $msg
} {0 1}

test parseargs.372 {myParser delete (no arg)} {
    list [catch { myParser delete } msg] $msg
} {0 {}}

test parseargs.372 {myParser delete badArg} {
    list [catch { myParser delete badArg } msg] $msg
} {1 {can't find argument "badArg" in parser "::myParser"}}

test parseargs.372 {myParser delete "type" "verbose" } {
    list [catch { myParser delete "type" "verbose" } msg] $msg
} {0 {}}

test parseargs.373 {myParser exists "type"} {
    list [catch { myParser exists "type" } msg] $msg
} {0 0}

test parseargs.372 {myParser get type defValue} {
    list [catch { myParser get type defValue } msg] $msg
} {0 defValue}

exit 0


test parseargs.374 {parseargs0 insert 0} {
    list [catch {parseargs0 insert 0} msg] $msg
} {0 2}

test parseargs.375 {parseargs0 insert root} {
    list [catch {parseargs0 insert root} msg] $msg
} {0 3}

test parseargs.376 {parseargs0 insert all} {
    list [catch {parseargs0 insert all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.377 {parseargs0 insert 0 -at badPosition} {
    list [catch {parseargs0 insert 0 -at badPosition} msg] $msg
} {1 {expected integer but got "badPosition"}}

test parseargs.378 {parseargs0 insert 0 -at -1} {
    list [catch {parseargs0 insert 0 -at -1} msg] $msg
} {1 {bad value "-1": can't be negative}}

test parseargs.379 {parseargs0 insert 0 -at 1000} {
    list [catch {parseargs0 insert 0 -at 1000} msg] $msg
} {0 4}

test parseargs.380 {parseargs0 insert 0 -at (no arg)} {
    list [catch {parseargs0 insert 0 -at} msg] $msg
} {1 {value for "-at" missing}}

test parseargs.381 {parseargs0 insert 0 -tags myTag} {
    list [catch {parseargs0 insert 0 -tags myTag} msg] $msg
} {0 5}

test parseargs.382 {parseargs0 insert 0 -tags {myTag1 myTag2} } {
    list [catch {parseargs0 insert 0 -tags {myTag1 myTag2}} msg] $msg
} {0 6}

test parseargs.383 {parseargs0 insert 0 -tags root} {
    list [catch {parseargs0 insert 0 -tags root} msg] $msg
} {1 {can't add reserved tag "root"}}

test parseargs.384 {parseargs0 insert 0 -tags (missing arg)} {
    list [catch {parseargs0 insert 0 -tags} msg] $msg
} {1 {value for "-tags" missing}}

test parseargs.385 {parseargs0 insert 0 -label myLabel -tags thisTag} {
    list [catch {parseargs0 insert 0 -label myLabel -tags thisTag} msg] $msg
} {0 8}

test parseargs.386 {parseargs0 insert 0 -label (missing arg)} {
    list [catch {parseargs0 insert 0 -label} msg] $msg
} {1 {value for "-label" missing}}

test parseargs.387 {parseargs0 insert 1 -tags thisTag} {
    list [catch {parseargs0 insert 1 -tags thisTag} msg] $msg
} {0 9}

test parseargs.388 {parseargs0 insert 1 -data key (missing value)} {
    list [catch {parseargs0 insert 1 -data key} msg] $msg
} {1 {missing value for "key"}}

test parseargs.389 {parseargs0 insert 1 -data {key value}} {
    list [catch {parseargs0 insert 1 -data {key value}} msg] $msg
} {0 11}

test parseargs.390 {parseargs0 insert 1 -data {key1 value1 key2 value2}} {
    list [catch {parseargs0 insert 1 -data {key1 value1 key2 value2}} msg] $msg
} {0 12}

test parseargs.391 {get} {
    list [catch {
	parseargs0 get 12
    } msg] $msg
} {0 {key1 value1 key2 value2}}

test parseargs.392 {parseargs0 children} {
    list [catch {parseargs0 children} msg] $msg
} {1 {wrong # args: should be "parseargs0 children nodeName ?switches ...?"}}

test parseargs.393 {parseargs0 children 0} {
    list [catch {parseargs0 children 0} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test parseargs.394 {parseargs0 children root} {
    list [catch {parseargs0 children root} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test parseargs.395 {parseargs0 children 1} {
    list [catch {parseargs0 children 1} msg] $msg
} {0 {9 11 12}}

test parseargs.396 {parseargs0 insert myTag} {
    list [catch {parseargs0 insert myTag} msg] $msg
} {0 13}

test parseargs.397 {parseargs0 index myTag} {
    list [catch {parseargs0 index myTag} msg] $msg
} {0 5}

test parseargs.398 {parseargs0 children 5} {
    list [catch {parseargs0 children 5} msg] $msg
} {0 13}

test parseargs.399 {parseargs0 children -1 -nocomplain} {
    list [catch {parseargs0 children -1 -nocomplain} msg] $msg
} {0 {}}

test parseargs.400 {parseargs0 children badNode -nocomplain} {
    list [catch {parseargs0 children badNode -nocomplain} msg] $msg
} {0 {}}

test parseargs.401 {parseargs0 children myTag} {
    list [catch {parseargs0 children myTag} msg] $msg
} {0 13}

test parseargs.402 {parseargs0 children root -from 0 -to end} {
    list [catch {parseargs0 children root -from 0 -to end} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test parseargs.403 {parseargs0 children root -from 2} {
    list [catch {parseargs0 children root -from 2} msg] $msg
} {0 {3 4 5 6 8}}

test parseargs.404 {parseargs0 children root -from 2 -to end} {
    list [catch {parseargs0 children root -from 2 -to end} msg] $msg
} {0 {3 4 5 6 8}}

test parseargs.405 {parseargs0 children root -from end -to end} {
    list [catch {parseargs0 children root -from end -to end} msg] $msg
} {0 8}

test parseargs.406 {parseargs0 children root -from end} {
    list [catch {parseargs0 children root -from end} msg] $msg
} {0 8}

test parseargs.407 {parseargs0 children root -from 0 -to 2} {
    list [catch {parseargs0 children root -from 0 -to 2} msg] $msg
} {0 {1 2 3}}

test parseargs.408 {parseargs0 children root -from -1 -to -20} {
    list [catch {parseargs0 children root -from -1 -to -20} msg] $msg
} {1 {bad position "-1": can't be negative}}

test parseargs.409 {parseargs0 firstchild (missing arg)} {
    list [catch {parseargs0 firstchild} msg] $msg
} {1 {wrong # args: should be "parseargs0 firstchild nodeName"}}

test parseargs.410 {parseargs0 firstchild root} {
    list [catch {parseargs0 firstchild root} msg] $msg
} {0 1}

test parseargs.411 {parseargs0 lastchild (missing arg)} {
    list [catch {parseargs0 lastchild} msg] $msg
} {1 {wrong # args: should be "parseargs0 lastchild nodeName"}}

test parseargs.412 {parseargs0 lastchild root} {
    list [catch {parseargs0 lastchild root} msg] $msg
} {0 8}

test parseargs.413 {parseargs0 nextsibling (missing arg)} {
    list [catch {parseargs0 nextsibling} msg] $msg
} {1 {wrong # args: should be "parseargs0 nextsibling nodeName"}}

test parseargs.414 {parseargs0 nextsibling 1)} {
    list [catch {parseargs0 nextsibling 1} msg] $msg
} {0 2}

test parseargs.415 {parseargs0 nextsibling 2)} {
    list [catch {parseargs0 nextsibling 2} msg] $msg
} {0 3}

test parseargs.416 {parseargs0 nextsibling 3)} {
    list [catch {parseargs0 nextsibling 3} msg] $msg
} {0 4}

test parseargs.417 {parseargs0 nextsibling 4)} {
    list [catch {parseargs0 nextsibling 4} msg] $msg
} {0 5}

test parseargs.418 {parseargs0 nextsibling 5)} {
    list [catch {parseargs0 nextsibling 5} msg] $msg
} {0 6}

test parseargs.419 {parseargs0 nextsibling 6)} {
    list [catch {parseargs0 nextsibling 6} msg] $msg
} {0 8}

test parseargs.420 {parseargs0 nextsibling 8)} {
    list [catch {parseargs0 nextsibling 8} msg] $msg
} {0 -1}

test parseargs.421 {parseargs0 nextsibling all)} {
    list [catch {parseargs0 nextsibling all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.422 {parseargs0 nextsibling badTag)} {
    list [catch {parseargs0 nextsibling badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::parseargs0}}

test parseargs.423 {parseargs0 nextsibling -1)} {
    list [catch {parseargs0 nextsibling -1} msg] $msg
} {1 {can't find tag or id "-1" in ::parseargs0}}

test parseargs.424 {parseargs0 prevsibling 2)} {
    list [catch {parseargs0 prevsibling 2} msg] $msg
} {0 1}

test parseargs.425 {parseargs0 prevsibling 1)} {
    list [catch {parseargs0 prevsibling 1} msg] $msg
} {0 -1}

test parseargs.426 {parseargs0 prevsibling -1)} {
    list [catch {parseargs0 prevsibling -1} msg] $msg
} {1 {can't find tag or id "-1" in ::parseargs0}}

test parseargs.427 {parseargs0 root)} {
    list [catch {parseargs0 root} msg] $msg
} {0 0}

test parseargs.428 {parseargs0 root badArg)} {
    list [catch {parseargs0 root badArgs} msg] $msg
} {1 {wrong # args: should be "parseargs0 root "}}

test parseargs.429 {parseargs0 parent (missing arg))} {
    list [catch {parseargs0 parent} msg] $msg
} {1 {wrong # args: should be "parseargs0 parent nodeName"}}

test parseargs.430 {parseargs0 parent root)} {
    list [catch {parseargs0 parent root} msg] $msg
} {0 -1}

test parseargs.431 {parseargs0 parent 1)} {
    list [catch {parseargs0 parent 1} msg] $msg
} {0 0}

test parseargs.432 {parseargs0 parent myTag)} {
    list [catch {parseargs0 parent myTag} msg] $msg
} {0 0}

test parseargs.433 {parseargs0 next (missing arg))} {
    list [catch {parseargs0 next} msg] $msg
} {1 {wrong # args: should be "parseargs0 next nodeName"}}


test parseargs.434 {parseargs0 next (extra arg))} {
    list [catch {parseargs0 next root root} msg] $msg
} {1 {wrong # args: should be "parseargs0 next nodeName"}}

test parseargs.435 {parseargs0 next root} {
    list [catch {parseargs0 next root} msg] $msg
} {0 1}

test parseargs.436 {parseargs0 next 1)} {
    list [catch {parseargs0 next 1} msg] $msg
} {0 9}

test parseargs.437 {parseargs0 next 2)} {
    list [catch {parseargs0 next 2} msg] $msg
} {0 3}

test parseargs.438 {parseargs0 next 3)} {
    list [catch {parseargs0 next 3} msg] $msg
} {0 4}

test parseargs.439 {parseargs0 next 4)} {
    list [catch {parseargs0 next 4} msg] $msg
} {0 5}

test parseargs.440 {parseargs0 next 5)} {
    list [catch {parseargs0 next 5} msg] $msg
} {0 13}

test parseargs.441 {parseargs0 next 6)} {
    list [catch {parseargs0 next 6} msg] $msg
} {0 8}

test parseargs.442 {parseargs0 next 8)} {
    list [catch {parseargs0 next 8} msg] $msg
} {0 -1}

test parseargs.443 {parseargs0 previous 1)} {
    list [catch {parseargs0 previous 1} msg] $msg
} {0 0}

test parseargs.444 {parseargs0 previous 0)} {
    list [catch {parseargs0 previous 0} msg] $msg
} {0 -1}

test parseargs.445 {parseargs0 previous 8)} {
    list [catch {parseargs0 previous 8} msg] $msg
} {0 6}

test parseargs.446 {parseargs0 depth (no arg))} {
    list [catch {parseargs0 depth} msg] $msg
} {0 2}

test parseargs.447 {parseargs0 depth root))} {
    list [catch {parseargs0 depth root} msg] $msg
} {0 0}

test parseargs.448 {parseargs0 depth myTag))} {
    list [catch {parseargs0 depth myTag} msg] $msg
} {0 1}

test parseargs.449 {parseargs0 depth myTag))} {
    list [catch {parseargs0 depth myTag} msg] $msg
} {0 1}

test parseargs.450 {parseargs0 dump (missing arg)))} {
    list [catch {parseargs0 dump} msg] $msg
} {1 {wrong # args: should be "parseargs0 dump nodeName ?switches ...?"}}

test parseargs.451 {parseargs0 dump root -version 2.0} {
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

test parseargs.452 {parseargs0 dump 1 -version 2.0} {
    list [catch {parseargs0 dump 1 -version 2.0} msg] $msg
} {0 {# V2.0
-1 1 {node1} {} {}
1 9 {node1 node9} {} {thisTag}
1 11 {node1 node11} {key value} {}
1 12 {node1 node12} {key1 value1 key2 value2} {}
}}

test parseargs.453 {parseargs0 dump this -version 2.0} {
    list [catch {parseargs0 dump myTag -version 2.0} msg] $msg
} {0 {# V2.0
-1 5 {node5} {} {myTag}
5 13 {node5 node13} {} {}
}}

test parseargs.454 {parseargs0 dump 1 badSwitch} {
    list [catch {parseargs0 dump 1 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -data data
   -file fileName
   -version versionNum
   -notags }}

test parseargs.455 {parseargs0 dump 11 -version 2.0} {
    list [catch {parseargs0 dump 11 -version 2.0} msg] $msg
} {0 {# V2.0
-1 11 {node11} {key value} {}
}}

test parseargs.456 {parseargs0 dump all} {
    list [catch {parseargs0 dump all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.457 {parseargs0 dump all} {
    list [catch {parseargs0 dump all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.458 {parseargs0 dump 0 -file test.dump} {
    list [catch {parseargs0 dump 0 -file test.dump} msg] $msg
} {0 {}}

test parseargs.459 {parseargs0 get 9} {
    list [catch {parseargs0 get 9} msg] $msg
} {0 {}}

test parseargs.460 {parseargs0 get all} {
    list [catch {parseargs0 get all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.461 {parseargs0 get root} {
    list [catch {parseargs0 get root} msg] $msg
} {0 {}}

test parseargs.462 {parseargs0 get 9 key} {
    list [catch {parseargs0 get root} msg] $msg
} {0 {}}

test parseargs.463 {parseargs0 get 12} {
    list [catch {parseargs0 get 12} msg] $msg
} {0 {key1 value1 key2 value2}}

test parseargs.464 {parseargs0 get 12 key1} {
    list [catch {parseargs0 get 12 key1} msg] $msg
} {0 value1}

test parseargs.465 {parseargs0 get 12 key2} {
    list [catch {parseargs0 get 12 key2} msg] $msg
} {0 value2}

test parseargs.466 {parseargs0 get 12 key1 defValue } {
    list [catch {parseargs0 get 12 key1 defValue} msg] $msg
} {0 value1}

test parseargs.467 {parseargs0 get 12 key100 defValue } {
    list [catch {parseargs0 get 12 key100 defValue} msg] $msg
} {0 defValue}

test parseargs.468 {parseargs0 index (missing arg) } {
    list [catch {parseargs0 index} msg] $msg
} {1 {wrong # args: should be "parseargs0 index label|list"}}

test parseargs.469 {parseargs0 index 0 10 (extra arg) } {
    list [catch {parseargs0 index 0 10} msg] $msg
} {1 {wrong # args: should be "parseargs0 index label|list"}}

test parseargs.470 {parseargs0 index 0} {
    list [catch {parseargs0 index 0} msg] $msg
} {0 0}

test parseargs.471 {parseargs0 index root} {
    list [catch {parseargs0 index root} msg] $msg
} {0 0}

test parseargs.472 {parseargs0 index all} {
    list [catch {parseargs0 index all} msg] $msg
} {0 -1}

test parseargs.473 {parseargs0 index myTag} {
    list [catch {parseargs0 index myTag} msg] $msg
} {0 5}

test parseargs.474 {parseargs0 index thisTag} {
    list [catch {parseargs0 index thisTag} msg] $msg
} {0 -1}

test parseargs.475 {parseargs0 is (no args)} {
    list [catch {parseargs0 is} msg] $msg
} {1 {ambiguous operation "is" matches:  isancestor isbefore isleaf isroot}}

test parseargs.476 {parseargs0 isbefore} {
    list [catch {parseargs0 isbefore} msg] $msg
} {1 {wrong # args: should be "parseargs0 isbefore node1 node2"}}

test parseargs.477 {parseargs0 isbefore 0 10 20} {
    list [catch {parseargs0 isbefore 0 10 20} msg] $msg
} {1 {wrong # args: should be "parseargs0 isbefore node1 node2"}}

test parseargs.478 {parseargs0 isbefore 0 12} {
    list [catch {parseargs0 isbefore 0 12} msg] $msg
} {0 1}

test parseargs.479 {parseargs0 isbefore 12 0} {
    list [catch {parseargs0 isbefore 12 0} msg] $msg
} {0 0}

test parseargs.480 {parseargs0 isbefore 0 0} {
    list [catch {parseargs0 isbefore 0 0} msg] $msg
} {0 0}

test parseargs.481 {parseargs0 isbefore root 0} {
    list [catch {parseargs0 isbefore root 0} msg] $msg
} {0 0}

test parseargs.482 {parseargs0 isbefore 0 all} {
    list [catch {parseargs0 isbefore 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.483 {parseargs0 isancestor} {
    list [catch {parseargs0 isancestor} msg] $msg
} {1 {wrong # args: should be "parseargs0 isancestor node1 node2"}}

test parseargs.484 {parseargs0 isancestor 0 12 20} {
    list [catch {parseargs0 isancestor 0 12 20} msg] $msg
} {1 {wrong # args: should be "parseargs0 isancestor node1 node2"}}

test parseargs.485 {parseargs0 isancestor 0 12} {
    list [catch {parseargs0 isancestor 0 12} msg] $msg
} {0 1}

test parseargs.486 {parseargs0 isancestor 12 0} {
    list [catch {parseargs0 isancestor 12 0} msg] $msg
} {0 0}

test parseargs.487 {parseargs0 isancestor 1 2} {
    list [catch {parseargs0 isancestor 1 2} msg] $msg
} {0 0}

test parseargs.488 {parseargs0 isancestor root 0} {
    list [catch {parseargs0 isancestor root 0} msg] $msg
} {0 0}

test parseargs.489 {parseargs0 isancestor 0 all} {
    list [catch {parseargs0 isancestor 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.490 {parseargs0 isroot (missing arg)} {
    list [catch {parseargs0 isroot} msg] $msg
} {1 {wrong # args: should be "parseargs0 isroot nodeName"}}

test parseargs.491 {parseargs0 isroot 0 20 (extra arg)} {
    list [catch {parseargs0 isroot 0 20} msg] $msg
} {1 {wrong # args: should be "parseargs0 isroot nodeName"}}

test parseargs.492 {parseargs0 isroot 0} {
    list [catch {parseargs0 isroot 0} msg] $msg
} {0 1}

test parseargs.493 {parseargs0 isroot 12} {
    list [catch {parseargs0 isroot 12} msg] $msg
} {0 0}

test parseargs.494 {parseargs0 isroot 1} {
    list [catch {parseargs0 isroot 1} msg] $msg
} {0 0}

test parseargs.495 {parseargs0 isroot root} {
    list [catch {parseargs0 isroot root} msg] $msg
} {0 1}

test parseargs.496 {parseargs0 isroot all} {
    list [catch {parseargs0 isroot all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.497 {parseargs0 isleaf (missing arg)} {
    list [catch {parseargs0 isleaf} msg] $msg
} {1 {wrong # args: should be "parseargs0 isleaf nodeName"}}

test parseargs.498 {parseargs0 isleaf 0 20 (extra arg)} {
    list [catch {parseargs0 isleaf 0 20} msg] $msg
} {1 {wrong # args: should be "parseargs0 isleaf nodeName"}}

test parseargs.499 {parseargs0 isleaf 0} {
    list [catch {parseargs0 isleaf 0} msg] $msg
} {0 0}

test parseargs.500 {parseargs0 isleaf 12} {
    list [catch {parseargs0 isleaf 12} msg] $msg
} {0 1}

test parseargs.501 {parseargs0 isleaf 1} {
    list [catch {parseargs0 isleaf 1} msg] $msg
} {0 0}

test parseargs.502 {parseargs0 isleaf root} {
    list [catch {parseargs0 isleaf root} msg] $msg
} {0 0}

test parseargs.503 {parseargs0 isleaf all} {
    list [catch {parseargs0 isleaf all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.504 {parseargs0 isleaf 1000} {
    list [catch {parseargs0 isleaf 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::parseargs0}}

test parseargs.505 {parseargs0 isleaf badTag} {
    list [catch {parseargs0 isleaf badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::parseargs0}}

test parseargs.506 {parseargs0 set (missing arg)} {
    list [catch {parseargs0 set} msg] $msg
} {1 {wrong # args: should be "parseargs0 set nodeName ?fieldName value ...?"}}

test parseargs.507 {parseargs0 set 0 (missing arg)} {
    list [catch {parseargs0 set 0} msg] $msg
} {0 {}}

test parseargs.508 {parseargs0 set 0 key (missing arg)} {
    list [catch {parseargs0 set 0 key} msg] $msg
} {1 {missing value for field "key"}}

test parseargs.509 {parseargs0 set 0 key value} {
    list [catch {parseargs0 set 0 key value} msg] $msg
} {0 {}}

test parseargs.510 {parseargs0 set 0 key1 value1 key2 value2 key3 value3} {
    list [catch {parseargs0 set 0 key1 value1 key2 value2 key3 value3} msg] $msg
} {0 {}}

test parseargs.511 {parseargs0 set 0 key1 value1 key2 (missing arg)} {
    list [catch {parseargs0 set 0 key1 value1 key2} msg] $msg
} {1 {missing value for field "key2"}}

test parseargs.512 {parseargs0 set 0 key value} {
    list [catch {parseargs0 set 0 key value} msg] $msg
} {0 {}}

test parseargs.513 {parseargs0 set 0 key1 value1 key2 (missing arg)} {
    list [catch {parseargs0 set 0 key1 value1 key2} msg] $msg
} {1 {missing value for field "key2"}}

test parseargs.514 {parseargs0 set all} {
    list [catch {parseargs0 set all} msg] $msg
} {0 {}}

test parseargs.515 {parseargs0 set all abc 123} {
    list [catch {parseargs0 set all abc 123} msg] $msg
} {0 {}}

test parseargs.516 {parseargs0 set root} {
    list [catch {parseargs0 set root} msg] $msg
} {0 {}}

test parseargs.517 {parseargs0 restore stuff} {
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

test parseargs.518 {parseargs0 restore 0 -file test.dump} {
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


test parseargs.519 {parseargs0 unset 0 key1} {
    list [catch {parseargs0 unset 0 key1} msg] $msg
} {0 {}}

test parseargs.520 {parseargs0 get 0} {
    list [catch {parseargs0 get 0} msg] $msg
} {0 {key value key2 value2 key3 value3 abc 123}}

test parseargs.521 {parseargs0 unset 0 key2 key3} {
    list [catch {parseargs0 unset 0 key2 key3} msg] $msg
} {0 {}}

test parseargs.522 {parseargs0 get 0} {
    list [catch {parseargs0 get 0} msg] $msg
} {0 {key value abc 123}}

test parseargs.523 {parseargs0 unset 0} {
    list [catch {parseargs0 unset 0} msg] $msg
} {0 {}}

test parseargs.524 {parseargs0 get 0} {
    list [catch {parseargs0 get 0} msg] $msg
} {0 {}}

test parseargs.525 {parseargs0 unset all abc} {
    list [catch {parseargs0 unset all abc} msg] $msg
} {0 {}}

test parseargs.526 {parseargs0 restore stuff} {
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

test parseargs.527 {parseargs0 restore (missing arg)} {
    list [catch {parseargs0 restore} msg] $msg
} {1 {wrong # args: should be "parseargs0 restore nodeName ?switches ...?"}}

test parseargs.528 {parseargs0 restore 0 badSwitch} {
    list [catch {parseargs0 restore 0 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -data data
   -file fileName
   -notags 
   -overwrite }}


test parseargs.529 {parseargs0 restore 0 {} arg (extra arg)} {
    list [catch {parseargs0 restore 0 {} arg} msg] $msg
} {1 {unknown switch ""
The following switches are available:
   -data data
   -file fileName
   -notags 
   -overwrite }}


test parseargs.530 {parseargs0 size (missing arg)} {
    list [catch {parseargs0 size} msg] $msg
} {1 {wrong # args: should be "parseargs0 size nodeName"}}

test parseargs.531 {parseargs0 size 0} {
    list [catch {parseargs0 size 0} msg] $msg
} {0 12}

test parseargs.532 {parseargs0 size all} {
    list [catch {parseargs0 size all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.533 {parseargs0 size 0 10 (extra arg)} {
    list [catch {parseargs0 size 0 10} msg] $msg
} {1 {wrong # args: should be "parseargs0 size nodeName"}}

test parseargs.534 {parseargs0 delete (no args)} {
    list [catch {parseargs0 delete} msg] $msg
} {0 {}}

test parseargs.535 {parseargs0 delete 11} {
    list [catch {parseargs0 delete 11} msg] $msg
} {0 {}}

test parseargs.536 {parseargs0 delete 11} {
    list [catch {parseargs0 delete 11} msg] $msg
} {1 {can't find tag or id "11" in ::parseargs0}}

test parseargs.537 {parseargs0 delete 9 12} {
    list [catch {parseargs0 delete 9 12} msg] $msg
} {0 {}}

test parseargs.538 {parseargs0 dump 0 -version 2.0} {
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

test parseargs.539 {delete all} {
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

test parseargs.540 {delete all all} {
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

test parseargs.541 {parseargs0 apply (missing arg)} {
    list [catch {parseargs0 apply} msg] $msg
} {1 {wrong # args: should be "parseargs0 apply nodeName ?switches ...?"}}

test parseargs.542 {parseargs0 apply 0} {
    list [catch {parseargs0 apply 0} msg] $msg
} {0 {}}

test parseargs.543 {parseargs0 apply 0 -badSwitch} {
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

test parseargs.544 {parseargs0 apply badTag} {
    list [catch {parseargs0 apply badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::parseargs0}}

test parseargs.545 {parseargs0 apply all} {
    list [catch {parseargs0 apply all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.546 {parseargs0 apply myTag -precommand lappend} {
    list [catch {
	set mylist {}
	parseargs0 apply myTag -precommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {5 13}}

test parseargs.547 {parseargs0 apply root -precommand lappend} {
    list [catch {
	set mylist {}
	parseargs0 apply root -precommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {0 1 2 3 4 5 13 6 8}}

test parseargs.548 {parseargs0 apply -postcommand} {
    list [catch {
	set mylist {}
	parseargs0 apply root -postcommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.549 {parseargs0 apply -precommand -postcommand} {
    list [catch {
	set mylist {}
	parseargs0 apply root -precommand {lappend mylist} \
		-postcommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {0 1 1 2 2 3 3 4 4 5 13 13 5 6 6 8 8 0}}

test parseargs.550 {parseargs0 apply root -precommand lappend -depth 1} {
    list [catch {
	set mylist {}
	parseargs0 apply root -precommand {lappend mylist} -depth 1
	set mylist
    } msg] $msg
} {0 {0 1 2 3 4 5 6 8}}


test parseargs.551 {parseargs0 apply root -precommand -depth 0} {
    list [catch {
	set mylist {}
	parseargs0 apply root -precommand {lappend mylist} -depth 0
	set mylist
    } msg] $msg
} {0 0}

test parseargs.552 {parseargs0 apply root -precommand -tag myTag} {
    list [catch {
	set mylist {}
	parseargs0 apply root -precommand {lappend mylist} -tag myTag
	set mylist
    } msg] $msg
} {0 5}


test parseargs.553 {parseargs0 apply root -precommand -key key1} {
    list [catch {
	set mylist {}
	parseargs0 set myTag key1 0.0
	parseargs0 apply root -precommand {lappend mylist} -key key1
	parseargs0 unset myTag key1
	set mylist
    } msg] $msg
} {0 5}

test parseargs.554 {parseargs0 apply root -postcommand -regexp node.*} {
    list [catch {
	set mylist {}
	parseargs0 set myTag key1 0.0
	parseargs0 apply root -precommand {lappend mylist} -regexp {node5} 
	parseargs0 unset myTag key1
	set mylist
    } msg] $msg
} {0 5}

test parseargs.555 {parseargs0 find (missing arg)} {
    list [catch {parseargs0 find} msg] $msg
} {1 {wrong # args: should be "parseargs0 find nodeName ?switches ...?"}}

test parseargs.556 {parseargs0 find 0} {
    list [catch {parseargs0 find 0} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.557 {parseargs0 find root} {
    list [catch {parseargs0 find root} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.558 {parseargs0 find 0 -glob node*} {
    list [catch {parseargs0 find root -glob node*} msg] $msg
} {0 {1 2 3 4 13 5 6}}

test parseargs.559 {parseargs0 find 0 -glob nobody} {
    list [catch {parseargs0 find root -glob nobody} msg] $msg
} {0 {}}

test parseargs.560 {parseargs0 find 0 -regexp {node[0-3]}} {
    list [catch {parseargs0 find root -regexp {node[0-3]}} msg] $msg
} {0 {1 2 3 13}}

test parseargs.561 {parseargs0 find 0 -regexp {.*[A-Z].*}} {
    list [catch {parseargs0 find root -regexp {.*[A-Z].*}} msg] $msg
} {0 8}

test parseargs.562 {parseargs0 find 0 -exact myLabel} {
    list [catch {parseargs0 find root -exact myLabel} msg] $msg
} {0 8}

test parseargs.563 {parseargs0 find 0 -exact myLabel -invert} {
    list [catch {parseargs0 find root -exact myLabel -invert} msg] $msg
} {0 {1 2 3 4 13 5 6 0}}


test parseargs.564 {parseargs0 find 3 -exact node3} {
    list [catch {parseargs0 find 3 -exact node3} msg] $msg
} {0 3}

test parseargs.565 {parseargs0 find 0 -nocase -exact mylabel} {
    list [catch {parseargs0 find 0 -nocase -exact mylabel} msg] $msg
} {0 8}

test parseargs.566 {parseargs0 find 0 -nocase} {
    list [catch {parseargs0 find 0 -nocase} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.567 {parseargs0 find 0 -path -nocase -glob *node1* } {
    list [catch {parseargs0 find 0 -path -nocase -glob *node1*} msg] $msg
} {0 {1 13}}

test parseargs.568 {parseargs0 find 0 -count 5 } {
    list [catch {parseargs0 find 0 -count 5} msg] $msg
} {0 {1 2 3 4 13}}

test parseargs.569 {parseargs0 find 0 -count -5 } {
    list [catch {parseargs0 find 0 -count -5} msg] $msg
} {1 {bad value "-5": can't be negative}}

test parseargs.570 {parseargs0 find 0 -count badValue } {
    list [catch {parseargs0 find 0 -count badValue} msg] $msg
} {1 {expected integer but got "badValue"}}

test parseargs.571 {parseargs0 find 0 -count badValue } {
    list [catch {parseargs0 find 0 -count badValue} msg] $msg
} {1 {expected integer but got "badValue"}}

test parseargs.572 {parseargs0 find 0 -leafonly} {
    list [catch {parseargs0 find 0 -leafonly} msg] $msg
} {0 {1 2 3 4 13 6 8}}

test parseargs.573 {parseargs0 find 0 -leafonly -glob {node[18]}} {
    list [catch {parseargs0 find 0 -glob {node[18]} -leafonly} msg] $msg
} {0 1}

test parseargs.574 {parseargs0 find 0 -depth 0} {
    list [catch {parseargs0 find 0 -depth 0} msg] $msg
} {0 0}

test parseargs.575 {parseargs0 find 0 -depth 1} {
    list [catch {parseargs0 find 0 -depth 1} msg] $msg
} {0 {1 2 3 4 5 6 8 0}}

test parseargs.576 {parseargs0 find 0 -depth 2} {
    list [catch {parseargs0 find 0 -depth 2} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.577 {parseargs0 find 0 -depth 20} {
    list [catch {parseargs0 find 0 -depth 20} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.578 {parseargs0 find 1 -depth 0} {
    list [catch {parseargs0 find 1 -depth 0} msg] $msg
} {0 1}

test parseargs.579 {parseargs0 find 1 -depth 1} {
    list [catch {parseargs0 find 1 -depth 1} msg] $msg
} {0 1}

test parseargs.580 {parseargs0 find 1 -depth 2} {
    list [catch {parseargs0 find 1 -depth 2} msg] $msg
} {0 1}

test parseargs.581 {parseargs0 find all} {
    list [catch {parseargs0 find all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.582 {parseargs0 find badTag} {
    list [catch {parseargs0 find badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::parseargs0}}

test parseargs.583 {parseargs0 find 0 -addtag hi} {
    list [catch {parseargs0 find 0 -addtag hi} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.584 {parseargs0 find 0 -addtag all} {
    list [catch {parseargs0 find 0 -addtag all} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test parseargs.585 {parseargs0 find 0 -addtag root} {
    list [catch {parseargs0 find 0 -addtag root} msg] $msg
} {1 {can't add reserved tag "root"}}

test parseargs.586 {parseargs0 find 0 -exec {lappend list} -leafonly} {
    list [catch {
	set list {}
	parseargs0 find 0 -exec {lappend list} -leafonly
	set list
	} msg] $msg
} {0 {1 2 3 4 13 6 8}}

test parseargs.587 {parseargs0 find 0 -tag root} {
    list [catch {parseargs0 find 0 -tag root} msg] $msg
} {0 0}

test parseargs.588 {parseargs0 find 0 -tag myTag} {
    list [catch {parseargs0 find 0 -tag myTag} msg] $msg
} {0 5}

test parseargs.589 {parseargs0 find 0 -tag badTag} {
    list [catch {parseargs0 find 0 -tag badTag} msg] $msg
} {0 {}}

test parseargs.590 {parseargs0 tag (missing args)} {
    list [catch {parseargs0 tag} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag args ..."}}

test parseargs.591 {parseargs0 tag badOp} {
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
  parseargs0 tag unset nodeName tag..}}

test parseargs.592 {parseargs0 tag add} {
    list [catch {parseargs0 tag add} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag add tag ?nodeName...?"}}

test parseargs.593 {parseargs0 tag add newTag} {
    list [catch {parseargs0 tag add newTag} msg] $msg
} {0 {}}

test parseargs.594 {parseargs0 tag add tag badNode} {
    list [catch {parseargs0 tag add tag badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::parseargs0}}

test parseargs.595 {parseargs0 tag add newTag root} {
    list [catch {parseargs0 tag add newTag root} msg] $msg
} {0 {}}

test parseargs.596 {parseargs0 tag add newTag all} {
    list [catch {parseargs0 tag add newTag all} msg] $msg
} {0 {}}

test parseargs.597 {parseargs0 tag add tag2 0 1 2 3 4} {
    list [catch {parseargs0 tag add tag2 0 1 2 3 4} msg] $msg
} {0 {}}

test parseargs.598 {parseargs0 tag exists tag2} {
    list [catch {parseargs0 tag exists tag2} msg] $msg
} {0 1}

test parseargs.599 {parseargs0 tag exists tag2 0} {
    list [catch {parseargs0 tag exists tag2 0} msg] $msg
} {0 1}

test parseargs.600 {parseargs0 tag exists tag2 5} {
    list [catch {parseargs0 tag exists tag2 5} msg] $msg
} {0 0}

test parseargs.601 {parseargs0 tag exists badTag} {
    list [catch {parseargs0 tag exists badTag} msg] $msg
} {0 0}

test parseargs.602 {parseargs0 tag exists badTag 1000} {
    list [catch {parseargs0 tag exists badTag 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::parseargs0}}

test parseargs.603 {parseargs0 tag add tag2 0 1 2 3 4 1000} {
    list [catch {parseargs0 tag add tag2 0 1 2 3 4 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::parseargs0}}

test parseargs.604 {parseargs0 tag names} {
    list [catch {parseargs0 tag names} msg] [lsort $msg]
} {0 {all hi myTag myTag1 myTag2 newTag root tag2 thisTag}}

test parseargs.605 {parseargs0 tag names badNode} {
    list [catch {parseargs0 tag names badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::parseargs0}}

test parseargs.606 {parseargs0 tag names all} {
    list [catch {parseargs0 tag names all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.607 {parseargs0 tag names root} {
    list [catch {parseargs0 tag names root} msg] [lsort $msg]
} {0 {all hi newTag root tag2}}

test parseargs.608 {parseargs0 tag names 0 1} {
    list [catch {parseargs0 tag names 0 1} msg] [lsort $msg]
} {0 {all hi newTag root tag2}}

test parseargs.609 {parseargs0 tag nodes (missing arg)} {
    list [catch {parseargs0 tag nodes} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag nodes tag ?tag...?"}}

test parseargs.610 {parseargs0 tag nodes root badTag} {
    # It's not an error to use bad tag.
    list [catch {parseargs0 tag nodes root badTag} msg] $msg
} {0 {}}

test parseargs.611 {parseargs0 tag nodes root tag2} {
    list [catch {parseargs0 tag nodes root tag2} msg] [lsort $msg]
} {0 {0 1 2 3 4}}

test parseargs.612 {parseargs0 ancestor (missing arg)} {
    list [catch {parseargs0 ancestor} msg] $msg
} {1 {wrong # args: should be "parseargs0 ancestor node1 node2"}}

test parseargs.613 {parseargs0 ancestor 0 (missing arg)} {
    list [catch {parseargs0 ancestor 0} msg] $msg
} {1 {wrong # args: should be "parseargs0 ancestor node1 node2"}}

test parseargs.614 {parseargs0 ancestor 0 10} {
    list [catch {parseargs0 ancestor 0 10} msg] $msg
} {1 {can't find tag or id "10" in ::parseargs0}}

test parseargs.615 {parseargs0 ancestor 0 4} {
    list [catch {parseargs0 ancestor 0 4} msg] $msg
} {0 0}

test parseargs.616 {parseargs0 ancestor 1 8} {
    list [catch {parseargs0 ancestor 1 8} msg] $msg
} {0 0}

test parseargs.617 {parseargs0 ancestor root 0} {
    list [catch {parseargs0 ancestor root 0} msg] $msg
} {0 0}

test parseargs.618 {parseargs0 ancestor 8 8} {
    list [catch {parseargs0 ancestor 8 8} msg] $msg
} {0 8}

test parseargs.619 {parseargs0 ancestor 0 all} {
    list [catch {parseargs0 ancestor 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.620 {parseargs0 ancestor 7 9} {
    list [catch {
	set n1 1; set n2 1;
	for { set i 0 } { $i < 4 } { incr i } {
	    set n1 [parseargs0 insert $n1]
	    set n2 [parseargs0 insert $n2]
	}
	parseargs0 ancestor $n1 $n2
	} msg] $msg
} {0 1}

test parseargs.621 {parseargs0 path (missing arg)} {
    list [catch {parseargs0 path} msg] $msg
} {1 {wrong # args: should be "parseargs0 path ?args ...?"}}

test parseargs.622 {parseargs0 path badArg} {
    list [catch {parseargs0 path badArg} msg] $msg
} {1 {bad operation "badArg": should be one of...
  parseargs0 path create pathName ?switches ...?
  parseargs0 path parse pathName ?switches ...?
  parseargs0 path print nodeName ?switches ...?
  parseargs0 path separator ?sepString?}}

test parseargs.623 {parseargs0 path print root} {
    list [catch {parseargs0 path print root} msg] $msg
} {0 {}}

test parseargs.624 {parseargs0 path print 0} {
    list [catch {parseargs0 path print 0} msg] $msg
} {0 {}}

test parseargs.625 {parseargs0 path print 15} {
    list [catch {parseargs0 path print 15} msg] $msg
} {0 {node1 node15}}

test parseargs.626 {parseargs0 path print 15} {
    list [catch {parseargs0 path print 15 -separator /} msg] $msg
} {0 /node1/node15}

test parseargs.627 {parseargs0 path print 16} {
    list [catch {parseargs0 path print 16 -separator /} msg] $msg
} {0 /node1/node14/node16}

test parseargs.628 {parseargs0 path parse /} {
    list [catch {parseargs0 path parse / -separator /} msg] $msg
} {0 0}

test parseargs.629 {parseargs0 path parse /node1} {
    list [catch {parseargs0 path parse /node1 -separator /} msg] $msg
} {0 1}

test parseargs.630 {parseargs0 path parse /node1/node14} {
    list [catch {parseargs0 path parse /node1/node14 -separator /} msg] $msg
} {0 14}

test parseargs.631 {parseargs0 path parse } {
    list [catch {parseargs0 path parse /node1/node14/node16 -separator /} msg] $msg
} {0 16}

test parseargs.632 {parseargs0 path parse } {
    list [catch {parseargs0 path parse /node1/node14/node16/ -separator /} msg] $msg
} {0 16}

test parseargs.633 {parseargs0 path parse } {
    list [catch {parseargs0 path parse //node1//node14//node16// -separator /} msg] $msg
} {0 16}

test parseargs.634 {parseargs0 path parse } {
    list [catch {parseargs0 path parse ::node1::node14::node16 -separator ::} msg] $msg
} {0 16}

test parseargs.635 {parseargs0 path parse } {
    list [catch {parseargs0 path parse /node1/node14/node16 -separator /} msg] $msg
} {0 16}

test parseargs.636 {parseargs0 path parse } {
    list [catch {parseargs0 path parse /node1/node14/node16/ -separator /} msg] $msg
} {0 16}

test parseargs.637 {parseargs0 path parse } {
    list [catch {parseargs0 path parse //node1//node14//node16// -separator /} msg] $msg
} {0 16}

test parseargs.638 {parseargs0 path parse } {
    list [catch {parseargs0 path parse ::node1::node14::node16 -separator ::} msg] $msg
} {0 16}

test parseargs.639 {parseargs0 path parse ""} {
    list [catch {parseargs0 path parse {}} msg] $msg
} {0 0}

test parseargs.640 {parseargs0 path parse node1 } {
    list [catch {parseargs0 path parse node1} msg] $msg
} {0 1}

test parseargs.641 {parseargs0 path parse {node1 node14}} {
    list [catch {parseargs0 path parse {node1 node14}} msg] $msg
} {0 14}

test parseargs.642 {parseargs0 path parse } {
    list [catch {parseargs0 path parse {node1 node14 node16}} msg] $msg
} {0 16}


test parseargs.643 {parseargs0 path print all} {
    list [catch {parseargs0 path print all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.644 {parseargs0 path print 0 badSwitch} {
    list [catch {parseargs0 path print 0 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -from node
   -separator char
   -noleadingseparator }}


test parseargs.645 {parseargs0 tag forget} {
    list [catch {parseargs0 tag forget} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag forget tag..."}}

test parseargs.646 {parseargs0 tag forget badTag} {
    list [catch {
	parseargs0 tag forget badTag
	lsort [parseargs0 tag names]
    } msg] $msg
} {0 {all hi myTag myTag1 myTag2 newTag root tag2 thisTag}}

test parseargs.647 {parseargs0 tag forget hi} {
    list [catch {
	parseargs0 tag forget hi
	lsort [parseargs0 tag names]
    } msg] $msg
} {0 {all myTag myTag1 myTag2 newTag root tag2 thisTag}}

test parseargs.648 {parseargs0 tag forget tag1 tag2} {
    list [catch {
	parseargs0 tag forget myTag1 myTag2
	lsort [parseargs0 tag names]
    } msg] $msg
} {0 {all myTag newTag root tag2 thisTag}}

test parseargs.649 {parseargs0 tag forget all} {
    list [catch {
	parseargs0 tag forget all
	lsort [parseargs0 tag names]
    } msg] $msg
} {0 {all myTag newTag root tag2 thisTag}}

test parseargs.650 {parseargs0 tag forget root} {
    list [catch {
	parseargs0 tag forget root
	lsort [parseargs0 tag names]
    } msg] $msg
} {0 {all myTag newTag root tag2 thisTag}}

test parseargs.651 {parseargs0 tag delete} {
    list [catch {parseargs0 tag delete} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag delete tag nodeName..."}}

test parseargs.652 {parseargs0 tag delete tag} {
    list [catch {parseargs0 tag delete tag} msg] $msg
} {1 {wrong # args: should be "parseargs0 tag delete tag nodeName..."}}

test parseargs.653 {parseargs0 tag delete tag 0} {
    list [catch {parseargs0 tag delete tag 0} msg] $msg
} {0 {}}

test parseargs.654 {parseargs0 tag delete root 0} {
    list [catch {parseargs0 tag delete root 0} msg] $msg
} {1 {can't delete reserved tag "root"}}

test parseargs.655 {parseargs0 move} {
    list [catch {parseargs0 move} msg] $msg
} {1 {wrong # args: should be "parseargs0 move nodeName destNode ?switches ...?"}}

test parseargs.656 {parseargs0 move 0} {
    list [catch {parseargs0 move 0} msg] $msg
} {1 {wrong # args: should be "parseargs0 move nodeName destNode ?switches ...?"}}

test parseargs.657 {parseargs0 move 0 0} {
    list [catch {parseargs0 move 0 0} msg] $msg
} {1 {can't move root node}}

test parseargs.658 {parseargs0 move 0 badNode} {
    list [catch {parseargs0 move 0 badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::parseargs0}}

test parseargs.659 {parseargs0 move 0 all} {
    list [catch {parseargs0 move 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.660 {parseargs0 move 1 0 -before 2} {
    list [catch {
	parseargs0 move 1 0 -before 2
	parseargs0 children 0
    } msg] $msg
} {0 {1 2 3 4 5 6 8}}

test parseargs.661 {parseargs0 move 1 0 -after 2} {
    list [catch {
	parseargs0 move 1 0 -after 2
	parseargs0 children 0
    } msg] $msg
} {0 {2 1 3 4 5 6 8}}

test parseargs.662 {parseargs0 move 1 2} {
    list [catch {
	parseargs0 move 1 2
	parseargs0 children 0
    } msg] $msg
} {0 {2 3 4 5 6 8}}

test parseargs.663 {parseargs0 move 0 2} {
    list [catch {parseargs0 move 0 2} msg] $msg
} {1 {can't move root node}}

test parseargs.664 {parseargs0 move 1 17} {
    list [catch {parseargs0 move 1 17} msg] $msg
} {1 {can't move node: "1" is an ancestor of "17"}}

test parseargs.665 {parseargs0 attach} {
    list [catch {parseargs0 attach} msg] $msg
} {1 {wrong # args: should be "parseargs0 attach parserName ?switches ...?"}}

test parseargs.666 {parseargs0 attach parseargs2 badArg} {
    list [catch {parseargs0 attach parseargs2 badArg} msg] $msg
} {1 {unknown switch "badArg"
The following switches are available:
   -newtags }}


test parseargs.667 {parseargs1 attach parseargs0 -newtags} {
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

test parseargs.668 {parseargs1 attach parseargs0} {
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

test parseargs.669 {parseargs1 attach ""} {
    list [catch {parseargs1 attach ""} msg] $msg
} {0 {}}


test parseargs.670 {blt::parseargs destroy parseargs1} {
    list [catch {blt::parseargs destroy parseargs1} msg] $msg
} {0 {}}

test parseargs.671 {parseargs0 find root -badSwitch} {
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

test parseargs.672 {parseargs0 find root -order} {
    list [catch {parseargs0 find root -order} msg] $msg
} {1 {value for "-order" missing}}

test parseargs.673 {parseargs0 find root ..} {
    list [catch {parseargs0 find root -order preorder -order postorder -order inorder} msg] $msg
} {0 {20 18 16 14 1 21 19 17 15 2 0 3 4 13 5 6 8}}

test parseargs.674 {parseargs0 find root -order preorder} {
    list [catch {parseargs0 find root -order preorder} msg] $msg
} {0 {0 2 1 14 16 18 20 15 17 19 21 3 4 5 13 6 8}}

test parseargs.675 {parseargs0 find root -order postorder} {
    list [catch {parseargs0 find root -order postorder} msg] $msg
} {0 {20 18 16 14 21 19 17 15 1 2 3 4 13 5 6 8 0}}

test parseargs.676 {parseargs0 find root -order inorder} {
    list [catch {parseargs0 find root -order inorder} msg] $msg
} {0 {20 18 16 14 1 21 19 17 15 2 0 3 4 13 5 6 8}}

test parseargs.677 {parseargs0 find root -order breadthfirst} {
    list [catch {parseargs0 find root -order breadthfirst} msg] $msg
} {0 {0 2 3 4 5 6 8 1 13 14 15 16 17 18 19 20 21}}

test parseargs.678 {parseargs0 set all key1 myValue} {
    list [catch {parseargs0 set all key1 myValue} msg] $msg
} {0 {}}

test parseargs.679 {parseargs0 set 15 key1 123} {
    list [catch {parseargs0 set 15 key1 123} msg] $msg
} {0 {}}

test parseargs.680 {parseargs0 set 16 key1 1234 key2 abc} {
    list [catch {parseargs0 set 16 key1 123 key2 abc} msg] $msg
} {0 {}}

test parseargs.681 {parseargs0 find root -key } {
    list [catch {parseargs0 find root -key} msg] $msg
} {1 {value for "-key" missing}}

test parseargs.682 {parseargs0 find root -key noKey} {
    list [catch {parseargs0 find root -key noKey} msg] $msg
} {0 {}}

test parseargs.683 {parseargs0 find root -key key1} {
    list [catch {parseargs0 find root -key key1} msg] $msg
} {0 {20 18 16 14 21 19 17 15 1 2 3 4 13 5 6 8 0}}

test parseargs.684 {parseargs0 find root -key key2} {
    list [catch {parseargs0 find root -key key2} msg] $msg
} {0 16}

test parseargs.685 {parseargs0 find root -key key2 -exact notThere } {
    list [catch {parseargs0 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test parseargs.686 {parseargs0 find root -key key1 -glob notThere } {
    list [catch {parseargs0 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test parseargs.687 {parseargs0 find root -key badKey -regexp notThere } {
    list [catch {parseargs0 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test parseargs.688 {parseargs0 find root -key key1 -glob 12*} {
    list [catch {parseargs0 find root -key key1 -glob 12*} msg] $msg
} {0 {16 15}}

test parseargs.689 {parseargs0 sort} {
    list [catch {parseargs0 sort} msg] $msg
} {1 {wrong # args: should be "parseargs0 sort nodeName ?switches ...?"}}

test parseargs.690 {parseargs0 sort all} {
    list [catch {parseargs0 sort all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test parseargs.691 {parseargs0 sort -recurse} {
    list [catch {parseargs0 sort -recurse} msg] $msg
} {1 {can't find tag or id "-recurse" in ::parseargs0}}

test parseargs.692 {parseargs0 sort 0} {
    list [catch {parseargs0 sort 0} msg] $msg
} {0 {8 2 3 4 5 6}}

test parseargs.693 {parseargs0 sort 0 -recurse} {
    list [catch {parseargs0 sort 0 -recurse} msg] $msg
} {0 {0 8 1 2 3 4 5 6 13 14 15 16 17 18 19 20 21}}

test parseargs.694 {parseargs0 sort 0 -decreasing -key} {
    list [catch {parseargs0 sort 0 -decreasing -key} msg] $msg
} {1 {value for "-key" missing}}

test parseargs.695 {parseargs0 sort 0 -re} {
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


test parseargs.696 {parseargs0 sort 0 -decreasing} {
    list [catch {parseargs0 sort 0 -decreasing} msg] $msg
} {0 {6 5 4 3 2 8}}

test parseargs.697 {parseargs0 sort 0} {
    list [catch {
	set list {}
	foreach n [parseargs0 sort 0] {
	    lappend list [parseargs0 label $n]
	}	
	set list
    } msg] $msg
} {0 {myLabel node2 node3 node4 node5 node6}}

test parseargs.698 {parseargs0 sort 0 -decreasing} {
    list [catch {parseargs0 sort 0 -decreasing} msg] $msg
} {0 {6 5 4 3 2 8}}


test parseargs.699 {parseargs0 sort 0 -decreasing -key} {
    list [catch {parseargs0 sort 0 -decreasing -key} msg] $msg
} {1 {value for "-key" missing}}

test parseargs.700 {parseargs0 sort 0 -decreasing -key key1} {
    list [catch {parseargs0 sort 0 -decreasing -key key1} msg] $msg
} {0 {8 6 5 4 3 2}}

test parseargs.701 {parseargs0 sort 0 -decreasing -recurse -key key1} {
    list [catch {parseargs0 sort 0 -decreasing -recurse -key key1} msg] $msg
} {0 {15 16 0 1 2 3 4 5 6 8 13 14 17 18 19 20 21}}

test parseargs.702 {parseargs0 sort 0 -decreasing -key key1} {
    list [catch {
	set list {}
	foreach n [parseargs0 sort 0 -decreasing -key key1] {
	    lappend list [parseargs0 get $n key1]
	}
	set list
    } msg] $msg
} {0 {myValue myValue myValue myValue myValue myValue}}


test parseargs.703 {parseargs0 index 1->firstchild} {
    list [catch {parseargs0 index 1->firstchild} msg] $msg
} {0 14}

test parseargs.704 {parseargs0 index root->firstchild} {
    list [catch {parseargs0 index root->firstchild} msg] $msg
} {0 2}

test parseargs.705 {parseargs0 label root->parent} {
    list [catch {parseargs0 label root->parent} msg] $msg
} {1 {can't find tag or id "root->parent" in ::parseargs0}}

test parseargs.706 {parseargs0 index root->parent} {
    list [catch {parseargs0 index root->parent} msg] $msg
} {0 -1}

test parseargs.707 {parseargs0 index root->lastchild} {
    list [catch {parseargs0 index root->lastchild} msg] $msg
} {0 8}

test parseargs.708 {parseargs0 index root->next} {
    list [catch {parseargs0 index root->next} msg] $msg
} {0 2}

test parseargs.709 {parseargs0 index root->previous} {
    list [catch {parseargs0 index root->previous} msg] $msg
} {0 -1}

test parseargs.710 {parseargs0 label root->previous} {
    list [catch {parseargs0 label root->previous} msg] $msg
} {1 {can't find tag or id "root->previous" in ::parseargs0}}

test parseargs.711 {parseargs0 index 1->previous} {
    list [catch {parseargs0 index 1->previous} msg] $msg
} {0 2}

test parseargs.712 {parseargs0 label root->badModifier} {
    list [catch {parseargs0 label root->badModifier} msg] $msg
} {1 {can't find tag or id "root->badModifier" in ::parseargs0}}

test parseargs.713 {parseargs0 index root->badModifier} {
    list [catch {parseargs0 index root->badModifier} msg] $msg
} {0 -1}

test parseargs.714 {parseargs0 index root->firstchild->parent} {
    list [catch {parseargs0 index root->firstchild->parent} msg] $msg
} {0 0}

test parseargs.715 {parseargs0 trace} {
    list [catch {parseargs0 trace} msg] $msg
} {1 {wrong # args: should be one of...
  parseargs0 trace create nodeName key how command ?-whenidle?
  parseargs0 trace delete traceName ...
  parseargs0 trace info traceName
  parseargs0 trace names ?pattern ...?}}

test parseargs.716 {parseargs0 trace create} {
    list [catch {parseargs0 trace create} msg] $msg
} {1 {wrong # args: should be "parseargs0 trace create nodeName key how command ?-whenidle?"}}

test parseargs.717 {parseargs0 trace create root} {
    list [catch {parseargs0 trace create root} msg] $msg
} {1 {wrong # args: should be "parseargs0 trace create nodeName key how command ?-whenidle?"}}

test parseargs.718 {parseargs0 trace create root * } {
    list [catch {parseargs0 trace create root * } msg] $msg
} {1 {wrong # args: should be "parseargs0 trace create nodeName key how command ?-whenidle?"}}

test parseargs.719 {parseargs0 trace create root * rwuc} {
    list [catch {parseargs0 trace create root * rwuc} msg] $msg
} {1 {wrong # args: should be "parseargs0 trace create nodeName key how command ?-whenidle?"}}

proc Doit args { global mylist; lappend mylist $args }

test parseargs.720 {parseargs0 trace create all newKey rwuc Doit} {
    list [catch {parseargs0 trace create all newKey rwuc Doit} msg] $msg
} {0 trace0}

test parseargs.721 {parseargs0 trace info trace0} {
    list [catch {parseargs0 trace info trace0} msg] $msg
} {0 {all newKey rwuc Doit}}

test parseargs.722 {test create trace} {
    list [catch {
	set mylist {}
	parseargs0 set all newKey 20
	set mylist
	} msg] $msg
} {0 {{::parseargs0 0 newKey wc} {::parseargs0 2 newKey wc} {::parseargs0 1 newKey wc} {::parseargs0 14 newKey wc} {::parseargs0 16 newKey wc} {::parseargs0 18 newKey wc} {::parseargs0 20 newKey wc} {::parseargs0 15 newKey wc} {::parseargs0 17 newKey wc} {::parseargs0 19 newKey wc} {::parseargs0 21 newKey wc} {::parseargs0 3 newKey wc} {::parseargs0 4 newKey wc} {::parseargs0 5 newKey wc} {::parseargs0 13 newKey wc} {::parseargs0 6 newKey wc} {::parseargs0 8 newKey wc}}}

test parseargs.723 {test read trace} {
    list [catch {
	set mylist {}
	parseargs0 get root newKey
	set mylist
	} msg] $msg
} {0 {{::parseargs0 0 newKey r}}}

test parseargs.724 {test write trace} {
    list [catch {
	set mylist {}
	parseargs0 set all newKey 21
	set mylist
	} msg] $msg
} {0 {{::parseargs0 0 newKey w} {::parseargs0 2 newKey w} {::parseargs0 1 newKey w} {::parseargs0 14 newKey w} {::parseargs0 16 newKey w} {::parseargs0 18 newKey w} {::parseargs0 20 newKey w} {::parseargs0 15 newKey w} {::parseargs0 17 newKey w} {::parseargs0 19 newKey w} {::parseargs0 21 newKey w} {::parseargs0 3 newKey w} {::parseargs0 4 newKey w} {::parseargs0 5 newKey w} {::parseargs0 13 newKey w} {::parseargs0 6 newKey w} {::parseargs0 8 newKey w}}}

test parseargs.725 {test unset trace} {
    list [catch {
	set mylist {}
	parseargs0 set all newKey 21
	set mylist
	} msg] $msg
} {0 {{::parseargs0 0 newKey w} {::parseargs0 2 newKey w} {::parseargs0 1 newKey w} {::parseargs0 14 newKey w} {::parseargs0 16 newKey w} {::parseargs0 18 newKey w} {::parseargs0 20 newKey w} {::parseargs0 15 newKey w} {::parseargs0 17 newKey w} {::parseargs0 19 newKey w} {::parseargs0 21 newKey w} {::parseargs0 3 newKey w} {::parseargs0 4 newKey w} {::parseargs0 5 newKey w} {::parseargs0 13 newKey w} {::parseargs0 6 newKey w} {::parseargs0 8 newKey w}}}

test parseargs.726 {parseargs0 trace delete} {
    list [catch {parseargs0 trace delete} msg] $msg
} {0 {}}

test parseargs.727 {parseargs0 trace delete badId} {
    list [catch {parseargs0 trace delete badId} msg] $msg
} {1 {unknown trace "badId"}}

test parseargs.728 {parseargs0 trace delete trace0} {
    list [catch {parseargs0 trace delete trace0} msg] $msg
} {0 {}}

test parseargs.729 {test create trace} {
    list [catch {
	set mylist {}
	parseargs0 set all newKey 20
	set mylist
	} msg] $msg
} {0 {}}

test parseargs.730 {test unset trace} {
    list [catch {
	set mylist {}
	parseargs0 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}


test parseargs.731 {parseargs0 notify} {
    list [catch {parseargs0 notify} msg] $msg
} {1 {wrong # args: should be one of...
  parseargs0 notify create ?switches ...? command
  parseargs0 notify delete ?notifyName ...?
  parseargs0 notify info notifyName
  parseargs0 notify names ?pattern ...?}}

test parseargs.732 {parseargs0 notify create} {
    list [catch {parseargs0 notify create} msg] $msg
} {1 {wrong # args: should be "parseargs0 notify create ?switches ...? command"}}

test parseargs.733 {parseargs0 notify create -allevents} {
    list [catch {parseargs0 notify create -allevents Doit} msg] $msg
} {0 notify0}

test parseargs.734 {parseargs0 notify info notify0} {
    list [catch {parseargs0 notify info notify0} msg] $msg
} {0 {notify0 {-create -delete -move -sort -relabel} {Doit}}}

test parseargs.735 {parseargs0 notify info badId} {
    list [catch {parseargs0 notify info badId} msg] $msg
} {1 {unknown notify name "badId"}}

test parseargs.736 {parseargs0 notify info} {
    list [catch {parseargs0 notify info} msg] $msg
} {1 {wrong # args: should be "parseargs0 notify info notifyName"}}

test parseargs.737 {parseargs0 notify names} {
    list [catch {parseargs0 notify names} msg] $msg
} {0 notify0}


test parseargs.738 {test create notify} {
    list [catch {
	set mylist {}
	parseargs0 insert 1 -tags test
	set mylist
	} msg] $msg
} {0 {{-create 22}}}

test parseargs.739 {test move notify} {
    list [catch {
	set mylist {}
	parseargs0 move 8 test
	set mylist
	} msg] $msg
} {0 {{-move 8}}}

test parseargs.740 {test sort notify} {
    list [catch {
	set mylist {}
	parseargs0 sort 0 -reorder 
	set mylist
	} msg] $msg
} {0 {{-sort 0}}}

test parseargs.741 {test relabel notify} {
    list [catch {
	set mylist {}
	parseargs0 label test "newLabel"
	set mylist
	} msg] $msg
} {0 {{-relabel 22}}}

test parseargs.742 {test delete notify} {
    list [catch {
	set mylist {}
	parseargs0 delete test
	set mylist
	} msg] $msg
} {0 {{-delete 8} {-delete 22}}}


test parseargs.743 {parseargs0 notify delete badId} {
    list [catch {parseargs0 notify delete badId} msg] $msg
} {1 {unknown notify name "badId"}}


test parseargs.744 {test create notify} {
    list [catch {
	set mylist {}
	parseargs0 set all newKey 20
	set mylist
	} msg] $msg
} {0 {}}

test parseargs.745 {test delete notify} {
    list [catch {
	set mylist {}
	parseargs0 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}

test parseargs.746 {test delete notify} {
    list [catch {
	set mylist {}
	parseargs0 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}

test parseargs.747 {parseargs0 copy} {
    list [catch {parseargs0 copy} msg] $msg
} {1 {wrong # args: should be "parseargs0 copy parentNode ?parserName? nodeName ?switches ...?"}}

test parseargs.748 {parseargs0 copy root} {
    list [catch {parseargs0 copy root} msg] $msg
} {1 {wrong # args: should be "parseargs0 copy parentNode ?parserName? nodeName ?switches ...?"}}

test parseargs.749 {parseargs0 copy root 14} {
    list [catch {parseargs0 copy root 14} msg] $msg
} {0 23}

test parseargs.750 {parseargs0 copy 14 root} {
    list [catch {parseargs0 copy 14 root} msg] $msg
} {0 24}

test parseargs.751 {parseargs0 copy 14 root -recurse} {
    list [catch {parseargs0 copy 14 root -recurse} msg] $msg
} {1 {can't make cyclic copy: source node is an ancestor of the destination}}

test parseargs.752 {parseargs0 copy 3 2 -recurse -tags} {
    list [catch {parseargs0 copy 3 2 -recurse -tags} msg] $msg
} {0 25}

test parseargs.753 {copy parseargs to parseargs -recurse} {
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

test parseargs.754 {parseargs dir (no recurse flag)} {
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


test parseargs.755 {parseargs dir -recurse} {
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



test parseargs.756 {parseargs dir -recurse} {
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

test parseargs.757 {parseargs dir (default settings, no -recurse)} {
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

test parseargs.758 {parseargs dir -type "file pipe"} {
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

test parseargs.759 {parseargs dir -type link -recurse} {
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

