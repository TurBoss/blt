package require BLT

if {[info procs test] != "test"} {
    source defs
}

if [file exists ../library] {
    set blt_library ../library
}

#set VERBOSE 1

test parseargs.1 {blt::parseargs no args} {
    list [catch {blt::parseargs} msg] $msg
} {1 {wrong # args: should be one of...
  blt::parseargs create ?parserName?
  blt::parseargs destroy ?parserName ...?
  blt::parseargs exists parserName
  blt::parseargs names ?pattern ...?}}

test parseargs.2 {blt::parseargs create #auto} {
    list [catch {blt::parseargs create #auto} msg] $msg
} {0 ::parseargs0}

test parseargs.3 {blt::parseargs create #auto.suffix} {
    list [catch {blt::parseargs create #auto.suffix} msg] $msg
} {0 ::parseargs0.suffix}

test parseargs.4 {blt::parseargs create prefix.#auto} {
    list [catch {blt::parseargs create prefix.#auto} msg] $msg
} {0 ::prefix.parseargs0}

test parseargs.5 {blt::parseargs create prefix.#auto.suffix} {
    list [catch {blt::parseargs create prefix.#auto.suffix} msg] $msg
} {0 ::prefix.parseargs0.suffix}

test parseargs.6 {blt::parseargs create prefix.#auto.suffix.#auto} {
    list [catch {blt::parseargs create prefix.#auto.suffix.#auto} msg] $msg
} {0 ::prefix.parseargs0.suffix.#auto}

test parseargs.7 {blt::parseargs destroy [parseargs names *parseargs0*]} {
    list [catch {
      eval blt::parseargs destroy [blt::parseargs names *parseargs0*]
    } msg] $msg
} {0 {}}

test parseargs.8 {blt::parseargs create} {
    list [catch {blt::parseargs create} msg] $msg
} {0 ::parseargs0}

test parseargs.9 {blt::parseargs create} {
    list [catch {blt::parseargs create} msg] $msg
} {0 ::parseargs1}

test parseargs.10 {blt::parseargs create fred} {
    list [catch {blt::parseargs create fred} msg] $msg
} {0 ::fred}

test parseargs.11 {blt::parseargs create fred} {
    list [catch {blt::parseargs create fred} msg] $msg
} {1 {a command "::fred" already exists}}

test parseargs.12 {blt::parseargs create if} {
    list [catch {blt::parseargs create if} msg] $msg
} {1 {a command "::if" already exists}}

test parseargs.13 {blt::parseargs create (bad namespace)} {
    list [catch {blt::parseargs create badName::fred} msg] $msg
} {1 {unknown namespace "badName"}}

test parseargs.14 {blt::parseargs names} {
    list [catch {blt::parseargs names} msg] [lsort $msg]
} {0 {::fred ::parseargs0 ::parseargs1}}

test parseargs.15 {blt::parseargs names pattern)} {
    list [catch {blt::parseargs names ::parseargs*} msg] [lsort $msg]
} {0 {::parseargs0 ::parseargs1}}

test parseargs.16 {blt::parseargs names badPattern)} {
    list [catch {blt::parseargs names badPattern*} msg] $msg
} {0 {}}

test parseargs.17 {blt::parseargs names pattern arg (wrong # args)} {
    list [catch {blt::parseargs names pattern arg} msg] $msg
} {1 {wrong # args: should be "blt::parseargs names ?pattern ...?"}}

test parseargs.18 {blt::parseargs destroy (no args)} {
    list [catch {blt::parseargs destroy} msg] $msg
} {0 {}}

test parseargs.19 {blt::parseargs destroy badParseargs} {
    list [catch {blt::parseargs destroy badParseargs} msg] $msg
} {1 {can't find a parser named "badParseargs"}}

test parseargs.20 {blt::parseargs destroy fred} {
    list [catch {blt::parseargs destroy fred} msg] $msg
} {0 {}}

test parseargs.21 {blt::parseargs destroy parseargs0 parseargs1} {
    list [catch {blt::parseargs destroy parseargs0 parseargs1} msg] $msg
} {0 {}}

test parseargs.22 {blt::parseargs create} {
    list [catch {blt::parseargs create} msg] $msg
} {0 ::parseargs0}

test parseargs.23 {parseargs0} {
    list [catch { parseargs0} msg] $msg
} {1 {wrong # args: should be one of...
  parseargs0 add argName ?switches ...?
  parseargs0 argument argName args...
  parseargs0 cget option
  parseargs0 configure ?value ...?
  parseargs0 currentdb argName
  parseargs0 delete ?argName ...?
  parseargs0 exists argName
  parseargs0 get ?argName? ?defValue?
  parseargs0 help 
  parseargs0 ischanged argName
  parseargs0 names ?pattern ...?
  parseargs0 parse argList ?varName?
  parseargs0 reset ?argName ...?
  parseargs0 restore list
  parseargs0 save 
  parseargs0 set ?argName value ...?}}

test parseargs.24 { badOp} {
    list [catch { parseargs0 badOp} msg] $msg
} {1 {bad operation "badOp": should be one of...
  parseargs0 add argName ?switches ...?
  parseargs0 argument argName args...
  parseargs0 cget option
  parseargs0 configure ?value ...?
  parseargs0 currentdb argName
  parseargs0 delete ?argName ...?
  parseargs0 exists argName
  parseargs0 get ?argName? ?defValue?
  parseargs0 help 
  parseargs0 ischanged argName
  parseargs0 names ?pattern ...?
  parseargs0 parse argList ?varName?
  parseargs0 reset ?argName ...?
  parseargs0 restore list
  parseargs0 save 
  parseargs0 set ?argName value ...?}}

test parseargs.25 { add (wrong # args)} {
    list [catch { parseargs0 add} msg] $msg
} {1 {wrong # args: should be "parseargs0 add argName ?switches ...?"}}

test parseargs.26 { add ""} {
    list [catch { parseargs0 add ""} msg] $msg
} {0 {}}

test parseargs.27 { delete ""} {
    list [catch { parseargs0 delete ""} msg] $msg
} {0 {}}

test parseargs.28 { add "newArg"} {
    list [catch { parseargs0 add "newArg"} msg] $msg
} {0 newArg}

test parseargs.29 { names} {
    list [catch { parseargs0 names} msg] [lsort $msg]
} {0 newArg}

test parseargs.30 { add "newArg"} {
    list [catch { parseargs0 add "newArg"} msg] $msg
} {1 {argument "newArg" already exists in "::parseargs0"}}

test parseargs.31 { delete "newArg"} {
    list [catch { parseargs0 delete "newArg"} msg] $msg
} {0 {}}

test parseargs.32 { names} {
    list [catch { parseargs0 names} msg] [lsort $msg]
} {0 {}}

test parseargs.33 { add "newArg" -badSwitch } {
    list [catch { parseargs0 add "newArg" -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -action actionName
   -allowprefixchars bool
   -command cmdPrefix
   -choices choiceList
   -current value
   -default string
   -destination argName
   -exclude excludeList
   -help string
   -long longName
   -metavar string
   -max maxValue
   -min minValue
   -nargs number
   -required bool
   -short shortName
   -type typeName
   -value value
   -variable varName}}

test parseargs.34 { names} {
    list [catch { parseargs0 names} msg] [lsort $msg]
} {0 {}}

test parseargs.35 { add newArg} {
    list [catch { parseargs0 add newArg} msg] $msg
} {0 newArg}

test parseargs.36 {argument} {
    list [catch { parseargs0 argument} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument argName args..."}}

test parseargs.37 {argument badOp} {
    list [catch { parseargs0 argument badOp} msg] $msg
} {1 {bad operation "badOp": should be one of...
  parseargs0 argument cget argName option
  parseargs0 argument configure argName ?value ...?}}

test parseargs.38 {argument cget (missing arg)} {
    list [catch { parseargs0 argument cget} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.39 {argument cget badArg} {
    list [catch { parseargs0 argument cget badArg} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.40 {argument cget newArg} {
    list [catch { parseargs0 argument cget newArg} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.41 {argument cget badArg badOption} {
    list [catch { parseargs0 argument cget badArg badOption} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.42 {argument cget badArg badOption extraArg} {
    list [catch { 
      parseargs0 argument cget badArg badOption extraArg
    } msg] $msg
} {1 {wrong # args: should be "parseargs0 argument cget argName option"}}

test parseargs.43 {argument cget newArg badOption} {
    list [catch { parseargs0 argument cget newArg badOption} msg] $msg
} {1 {unknown switch "badOption"
The following switches are available:
   -action actionName
   -allowprefixchars bool
   -command cmdPrefix
   -choices choiceList
   -current value
   -default string
   -destination argName
   -exclude excludeList
   -help string
   -long longName
   -metavar string
   -max maxValue
   -min minValue
   -nargs number
   -required bool
   -short shortName
   -type typeName
   -value value
   -variable varName}}

test parseargs.44 {argument cget newArg -action} {
    list [catch { parseargs0 argument cget newArg -action} msg] $msg
} {0 store}

test parseargs.45 {argument cget newArg -command} {
    list [catch { parseargs0 argument cget newArg -command} msg] $msg
} {0 {}}

test parseargs.46 {argument cget newArg -choices} {
    list [catch { parseargs0 argument cget newArg -choices} msg] $msg
} {0 {}}

test parseargs.47 {argument cget newArg -default} {
    list [catch { parseargs0 argument cget newArg -default} msg] $msg
} {0 {}}

test parseargs.48 {argument cget newArg -exclude} {
    list [catch { parseargs0 argument cget newArg -exclude} msg] $msg
} {0 {}}

test parseargs.49 {argument cget newArg -help} {
    list [catch { parseargs0 argument cget newArg -help} msg] $msg
} {0 {}}

test parseargs.50 {argument cget newArg -long} {
    list [catch { parseargs0 argument cget newArg -long} msg] $msg
} {0 {}}

test parseargs.51 {argument cget newArg -metavar} {
    list [catch { parseargs0 argument cget newArg -metavar} msg] $msg
} {0 {}}

test parseargs.52 {argument cget newArg -max} {
    list [catch { parseargs0 argument cget newArg -max} msg] $msg
} {0 {}}

test parseargs.53 {argument cget newArg -min} {
    list [catch { parseargs0 argument cget newArg -min} msg] $msg
} {0 {}}

test parseargs.54 {argument cget newArg -nargs} {
    list [catch { parseargs0 argument cget newArg -nargs} msg] $msg
} {0 1}

test parseargs.55 {argument cget newArg -required} {
    list [catch { parseargs0 argument cget newArg -required} msg] $msg
} {0 0}

test parseargs.56 {argument cget newArg -short} {
    list [catch { parseargs0 argument cget newArg -short} msg] $msg
} {0 {}}

test parseargs.57 {argument cget newArg -type} {
    list [catch { parseargs0 argument cget newArg -type} msg] $msg
} {0 string}

test parseargs.58 {argument cget newArg -variable} {
    list [catch { parseargs0 argument cget newArg -variable} msg] $msg
} {0 {}}

test parseargs.59 {argument cget newArg -value} {
    list [catch { parseargs0 argument cget newArg -value} msg] $msg
} {0 {}}

test parseargs.60 {argument configure} {
    list [catch { parseargs0 argument configure} msg] $msg
} {1 {wrong # args: should be "parseargs0 argument configure argName ?value ...?"}}

test parseargs.61 {argument configure badArg} {
    list [catch { parseargs0 argument configure badArg} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.62 {argument configure newArg} {
    list [catch { parseargs0 argument configure newArg} msg] $msg
} {0 {{-action store store} {-allowprefixchars 0 0} {-command {} {}} {-choices {} {}} {-current {} {}} {-default {} {}} {-destination {} newArg} {-exclude {} {}} {-help {} {}} {-long {} {}} {-metavar {} {}} {-max {} {}} {-min {} {}} {-nargs 1 1} {-required 0 0} {-short {} {}} {-type string string} {-value {} {}} {-variable {} {}}}}

test parseargs.63 {argument configure badArg badOption} {
    list [catch { parseargs0 argument configure badArg badOption} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.64 {argument configure badArg badOption extraArg} {
    list [catch { parseargs0 argument configure badArg badOption extraArg} msg] $msg
} {1 {can't find argument "badArg" in parser "::parseargs0"}}

test parseargs.65 {argument configure newArg badOption} {
    list [catch { parseargs0 argument configure newArg badOption} msg] $msg
} {1 {unknown switch "badOption"
The following switches are available:
   -action actionName
   -allowprefixchars bool
   -command cmdPrefix
   -choices choiceList
   -current value
   -default string
   -destination argName
   -exclude excludeList
   -help string
   -long longName
   -metavar string
   -max maxValue
   -min minValue
   -nargs number
   -required bool
   -short shortName
   -type typeName
   -value value
   -variable varName}}

test parseargs.66 {argument configure newArg -action} {
    list [catch { parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store}}

test parseargs.67 {argument configure newArg -action badValue} {
    list [catch {
	parseargs0 argument configure newArg -action badValue
    } msg] $msg
} {1 {unknown action "badValue": should be store, append, store_false, store_true, or help}}

test parseargs.68 {argument configure newArg -action} {
    list [catch { parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store}}

test parseargs.69 {argument configure newArg -action store} {
    list [catch { 
      parseargs0 argument configure newArg -action store
    } msg] $msg
} {0 {}}

test parseargs.70 {argument configure newArg -action append} {
    list [catch { 
      parseargs0 argument configure newArg -action append
    } msg] $msg
} {0 {}}

test parseargs.71 {argument configure newArg -action} {
    list [catch { parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store append}}

test parseargs.72 {argument configure newArg -action store_false} {
    list [catch {
	parseargs0 argument configure newArg -action store_false
    } msg] $msg
} {0 {}}

test parseargs.73 {argument configure newArg -action} {
    list [catch { parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store_false}}

test parseargs.74 {argument configure newArg -action store_true} {
    list [catch {
	parseargs0 argument configure newArg -action store_true
    } msg] $msg
} {0 {}}

test parseargs.75 {argument configure newArg -action} {
    list [catch { parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store_true}}

test parseargs.76 {argument configure newArg -action store} {
    list [catch {
	parseargs0 argument configure newArg -action store
    } msg] $msg
} {0 {}}

test parseargs.77 {argument configure newArg -action store} {
    list [catch {
	parseargs0 argument configure newArg -action store
    } msg] $msg
} {0 {}}

test parseargs.78 {argument configure newArg -action stor} {
    list [catch {
	parseargs0 argument configure newArg -action stor
    } msg] $msg
} {1 {unknown action "stor": should be store, append, store_false, store_true, or help}}

test parseargs.79 {argument configure newArg -action store_} {
    list [catch {
	parseargs0 argument configure newArg -action store_
    } msg] $msg
} {1 {unknown action "store_": should be store, append, store_false, store_true, or help}}

test parseargs.80 {argument configure newArg -action store_t} {
    list [catch {
	parseargs0 argument configure newArg -action store_t
    } msg] $msg
} {0 {}}

test parseargs.81 {argument configure newArg -action a} {
    list [catch {
	parseargs0 argument configure newArg -action a
    } msg] $msg
} {0 {}}

test parseargs.82 {argument configure newArg -action store} {
    list [catch {
	parseargs0 argument configure newArg -action store
    } msg] $msg
} {0 {}}

test parseargs.83 {argument configure newArg -action} {
    list [catch { parseargs0 argument configure newArg -action} msg] $msg
} {0 {-action store store}}


test parseargs.84 {argument configure newArg -allowprefixchars badBool} {
    list [catch {
	parseargs0 argument configure newArg -allowprefixchars badBool
    } msg] $msg
} {1 {expected boolean value but got "badBool"}}

test parseargs.85 {argument configure newArg -allowprefixchars 1} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars 1
    } msg] $msg
} {0 {}}

test parseargs.86 {argument configure newArg -allowprefixchars true} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars true
    } msg] $msg
} {0 {}}

test parseargs.87 {argument configure newArg -allowprefixchars yes} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars yes
    } msg] $msg
} {0 {}}

test parseargs.88 {argument configure newArg -allowprefixchars on} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars on
    } msg] $msg
} {0 {}}

test parseargs.89 {argument configure newArg -allowprefixchars 0} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars 0
    } msg] $msg
} {0 {}}

test parseargs.90 {argument configure newArg -allowprefixchars false} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars false
    } msg] $msg
} {0 {}}

test parseargs.91 {argument configure newArg -allowprefixchars no} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars no
    } msg] $msg
} {0 {}}

test parseargs.92 {argument configure newArg -allowprefixchars off} { 
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars off} msg] $msg 
} {0 {}}

test parseargs.93 {argument configure newArg -command} {
    list [catch { parseargs0 argument configure newArg -command} msg] $msg
} {0 {-command {} {}}}

test parseargs.94 {argument configure newArg -command cmdString} {
    list [catch {
	parseargs0 argument configure newArg -command cmdString
    } msg] $msg
} {0 {}}

test parseargs.95 {argument configure newArg -command} {
    list [catch {parseargs0 argument configure newArg -command} msg] $msg
} {0 {-command {} cmdString}}

test parseargs.96 {argument configure newArg -command ""} {
    list [catch {
	parseargs0 argument configure newArg -command ""
    } msg] $msg
} {0 {}}

test parseargs.97 {argument configure newArg -command} {
    list [catch { parseargs0 argument configure newArg -command} msg] $msg
} {0 {-command {} {}}}


test parseargs.98 {argument configure newArg -choices} {
    list [catch { parseargs0 argument configure newArg -choices} msg] $msg
} {0 {-choices {} {}}}

test parseargs.99 {argument configure newArg -choices "a b c"} {
    list [catch {
	parseargs0 argument configure newArg -choices "a b c"
    } msg] $msg
} {0 {}}

test parseargs.100 {argument configure newArg -choices} {
    list [catch { parseargs0 argument configure newArg -choices} msg] $msg
} {0 {-choices {} {a b c}}}

test parseargs.101 {argument configure newArg -choices ""} {
    list [catch {
	parseargs0 argument configure newArg -choices ""
    } msg] $msg
} {0 {}}

test parseargs.102 {argument configure newArg -choices} {
    list [catch { parseargs0 argument configure newArg -choices} msg] $msg
} {0 {-choices {} {}}}

test parseargs.103 {argument configure newArg -default} {
    list [catch { parseargs0 argument configure newArg -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.104 {argument configure newArg -default defValue} {
    list [catch {
	parseargs0 argument configure newArg -default defValue
    } msg] $msg
} {0 {}}

test parseargs.105 {argument configure newArg -default} {
    list [catch { parseargs0 argument configure newArg -default} msg] $msg
} {0 {-default {} defValue}}

test parseargs.106 {argument configure newArg -default ""} {
    list [catch {
	parseargs0 argument configure newArg -default ""
    } msg] $msg
} {0 {}}

test parseargs.107 {argument configure newArg -default} {
    list [catch { parseargs0 argument configure newArg -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.108 {argument configure newArg -exclude} {
    list [catch { parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} {}}}

test parseargs.109 {argument configure newArg -exclude arg} {
    list [catch {
	parseargs0 argument configure newArg -exclude arg
    } msg] $msg
} {0 {}}

test parseargs.110 {argument configure newArg -exclude} {
    list [catch { parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} arg}}

test parseargs.111 {argument configure newArg -exclude ""} {
    list [catch {
	parseargs0 argument configure newArg -exclude ""
    } msg] $msg
} {0 {}}

test parseargs.112 {argument configure newArg -exclude} {
    list [catch { parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} {}}}

test parseargs.113 {argument configure newArg -exclude} {
    list [catch { parseargs0 argument configure newArg -exclude} msg] $msg
} {0 {-exclude {} {}}}

test parseargs.114 {argument configure newArg -help} {
    list [catch { parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.115 {argument configure newArg -help help} {
    list [catch {
	parseargs0 argument configure newArg -help help
    } msg] $msg
} {0 {}}

test parseargs.116 {argument configure newArg -help} {
    list [catch { parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} help}}

test parseargs.117 {argument configure newArg -help ""} {
    list [catch {
	parseargs0 argument configure newArg -help ""
    } msg] $msg
} {0 {}}

test parseargs.118 {argument configure newArg -help} {
    list [catch { parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.119 {argument configure newArg -help} {
    list [catch { parseargs0 argument configure newArg -help} msg] $msg
} {0 {-help {} {}}}

test parseargs.120 {argument configure newArg -long} {
    list [catch { parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} {}}}

test parseargs.121 {argument configure newArg -long --long} {
    list [catch {
	parseargs0 argument configure newArg -long --long
    } msg] $msg
} {0 {}}

test parseargs.122 {argument configure newArg -long} {
    list [catch { parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} --long}}

test parseargs.123 {argument configure newArg -long ""} {
    list [catch {
	parseargs0 argument configure newArg -long ""
    } msg] $msg
} {0 {}}

test parseargs.124 {argument configure newArg -long} {
    list [catch { parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} {}}}

test parseargs.125 {argument configure newArg -long} {
    list [catch { parseargs0 argument configure newArg -long} msg] $msg
} {0 {-long {} {}}}

test parseargs.126 {argument configure newArg -metavar} {
    list [catch { parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} {}}}

test parseargs.127 {argument configure newArg -metavar VAR} {
    list [catch {
	parseargs0 argument configure newArg -metavar VAR
    } msg] $msg
} {0 {}}

test parseargs.128 {argument configure newArg -metavar} {
    list [catch { parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} VAR}}

test parseargs.129 {argument configure newArg -metavar ""} {
    list [catch {
	parseargs0 argument configure newArg -metavar ""
    } msg] $msg
} {0 {}}

test parseargs.130 {argument configure newArg -metavar} {
    list [catch { parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} {}}}

test parseargs.131 {argument configure newArg -metavar} {
    list [catch { parseargs0 argument configure newArg -metavar} msg] $msg
} {0 {-metavar {} {}}}

test parseargs.132 {argument configure newArg -max} {
    list [catch { parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} {}}}

test parseargs.133 {argument configure newArg -max abc} {
    list [catch {
	parseargs0 argument configure newArg -max abc
    } msg] $msg
} {1 {expected floating-point number but got "abc"}}

test parseargs.134 {argument configure newArg -max} {
    list [catch { parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} {}}}

test parseargs.135 {argument configure newArg -max ""} {
    list [catch {
	parseargs0 argument configure newArg -max ""
    } msg] $msg
} {0 {}}

test parseargs.136 {argument configure newArg -max} {
    list [catch { parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} {}}}

test parseargs.137 {argument configure newArg -max} {
    list [catch { parseargs0 argument configure newArg -max} msg] $msg
} {0 {-max {} {}}}

test parseargs.138 {argument configure newArg -min} {
    list [catch { parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} {}}}

test parseargs.139 {argument configure newArg -min abc} {
    list [catch {
	parseargs0 argument configure newArg -min abc
    } msg] $msg
} {1 {expected floating-point number but got "abc"}}

test parseargs.140 {argument configure newArg -min} {
    list [catch { parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} {}}}

test parseargs.141 {argument configure newArg -min ""} {
    list [catch {
	parseargs0 argument configure newArg -min ""
    } msg] $msg
} {0 {}}

test parseargs.142 {argument configure newArg -min} {
    list [catch { parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} {}}}

test parseargs.143 {argument configure newArg -min} {
    list [catch { parseargs0 argument configure newArg -min} msg] $msg
} {0 {-min {} {}}}


test parseargs.144 {argument configure newArg -nargs} {
    list [catch { parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 1}}

test parseargs.145 {argument configure newArg -nargs badNum} {
    list [catch {
	parseargs0 argument configure newArg -nargs badNum
    } msg] $msg
} {1 {invalid nargs "badNum": should be +, ?, *, "last" or number}}

test parseargs.146 {argument configure newArg -nargs -1} {
    list [catch { parseargs0 argument configure newArg -nargs -1} msg] $msg
} {1 {invalid nargs "-1": should be +, ?, *, "last" or number}}

test parseargs.147 {argument configure newArg -nargs ""} {
    list [catch {
	parseargs0 argument configure newArg -nargs ""
    } msg] $msg
} {1 {invalid nargs "": should be +, ?, *, "last" or number}}

test parseargs.148 {argument configure newArg -nargs 0} {
    list [catch { parseargs0 argument configure newArg -nargs 0} msg] $msg
} {0 {}}

test parseargs.149 {argument configure newArg -nargs} {
    list [catch { parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 0}}

test parseargs.150 {argument configure newArg -nargs 100} {
    list [catch { parseargs0 argument configure newArg -nargs 100} msg] $msg
} {0 {}}

test parseargs.151 {argument configure newArg -nargs} {
    list [catch { parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 100}}

test parseargs.152 {argument configure newArg -nargs ?} {
    list [catch { parseargs0 argument configure newArg -nargs ?} msg] $msg
} {0 {}}

test parseargs.153 {argument configure newArg -nargs} {
    list [catch { parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 ?}}

test parseargs.154 {argument configure newArg -nargs +} {
    list [catch { parseargs0 argument configure newArg -nargs +} msg] $msg
} {0 {}}

test parseargs.155 {argument configure newArg -nargs} {
    list [catch { parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 +}}

test parseargs.156 {argument configure newArg -nargs *} {
    list [catch { parseargs0 argument configure newArg -nargs *} msg] $msg
} {0 {}}

test parseargs.157 {argument configure newArg -nargs} {
    list [catch { parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 *}}

test parseargs.158 {argument configure newArg -nargs 1} {
    list [catch { parseargs0 argument configure newArg -nargs 1} msg] $msg
} {0 {}}

test parseargs.159 {argument configure newArg -nargs} {
    list [catch { parseargs0 argument configure newArg -nargs} msg] $msg
} {0 {-nargs 1 1}}

test parseargs.160 {argument configure newArg -allowprefixchars badBool} {
    list [catch {
	parseargs0 argument configure newArg -allowprefixchars badBool
    } msg] $msg
} {1 {expected boolean value but got "badBool"}}

test parseargs.161 {argument configure newArg -allowprefixchars 1} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars 1
    } msg] $msg
} {0 {}}

test parseargs.162 {argument configure newArg -allowprefixchars true} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars true
    } msg] $msg
} {0 {}}

test parseargs.163 {argument configure newArg -allowprefixchars yes} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars yes
    } msg] $msg
} {0 {}}

test parseargs.164 {argument configure newArg -allowprefixchars on} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars on
    } msg] $msg
} {0 {}}

test parseargs.165 {argument configure newArg -allowprefixchars 0} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars 0
    } msg] $msg
} {0 {}}

test parseargs.166 {argument configure newArg -allowprefixchars false} {
    list [catch { 
      parseargs0 argument configure newArg -allowprefixchars false
    } msg] $msg
} {0 {}}

test parseargs.167 {argument configure newArg -allowprefixchars no} {
    list [catch { 
      parseargs0 argument configure newArg -allowprefixchars no
    } msg] $msg
} {0 {}}

test parseargs.168 {argument configure newArg -allowprefixchars off} {
    list [catch {
      parseargs0 argument configure newArg -allowprefixchars off
    } msg] $msg
} {0 {}}

test parseargs.169 {argument configure newArg -short} {
    list [catch { parseargs0 argument configure newArg -short} msg] $msg
} {0 {-short {} {}}}

test parseargs.170 {argument configure newArg -short -s} {
    list [catch {
	parseargs0 argument configure newArg -short -s
    } msg] $msg
} {0 {}}

test parseargs.171 {argument configure newArg -short} {
    list [catch { parseargs0 argument configure newArg -short} msg] $msg
} {0 {-short {} -s}}

test parseargs.172 {argument configure newArg -short ""} {
    list [catch {
	parseargs0 argument configure newArg -short ""
    } msg] $msg
} {0 {}}

test parseargs.173 {argument configure newArg -short} {
    list [catch { parseargs0 argument configure newArg -short} msg] $msg
} {0 {-short {} {}}}

test parseargs.174 {argument configure newArg -type} {
    list [catch { parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string string}}

test parseargs.175 {argument configure newArg -type badType} {
    list [catch { parseargs0 argument configure newArg -type badType} msg] $msg
} {1 {unknown argument type "badType": should be integer, double, string, or boolean}}

test parseargs.176 {argument configure newArg -type integer} {
    list [catch { parseargs0 argument configure newArg -type integer} msg] $msg
} {0 {}}

test parseargs.177 {argument configure newArg -type} {
    list [catch { parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string integer}}

test parseargs.178 {argument configure newArg -type double} {
    list [catch { parseargs0 argument configure newArg -type double} msg] $msg
} {0 {}}

test parseargs.179 {argument configure newArg -type float} {
    list [catch { parseargs0 argument configure newArg -type float} msg] $msg
} {0 {}}

test parseargs.180 {argument configure newArg -type number} {
    list [catch { parseargs0 argument configure newArg -type number} msg] $msg
} {0 {}}

test parseargs.181 {argument configure newArg -type boolean} {
    list [catch { parseargs0 argument configure newArg -type boolean} msg] $msg
} {0 {}}

test parseargs.182 {argument configure newArg -type} {
    list [catch { parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string boolean}}

test parseargs.183 {argument configure newArg -type string} {
    list [catch { parseargs0 argument configure newArg -type string} msg] $msg
} {0 {}}

test parseargs.184 {argument configure newArg -type} {
    list [catch { parseargs0 argument configure newArg -type} msg] $msg
} {0 {-type string string}}

test parseargs.185 {argument configure newArg -variable} {
    list [catch { parseargs0 argument configure newArg -variable} msg] $msg
} {0 {-variable {} {}}}

test parseargs.186 {argument configure newArg -variable myVar} {
    list [catch {
	parseargs0 argument configure newArg -variable myVar
    } msg] $msg
} {0 {}}

test parseargs.187 {argument configure newArg -variable} {
    list [catch { parseargs0 argument configure newArg -variable} msg] $msg
} {0 {-variable {} myVar}}

test parseargs.188 {argument configure newArg -variable ""} {
    list [catch {
	parseargs0 argument configure newArg -variable ""
    } msg] $msg
} {0 {}}

test parseargs.189 {argument configure newArg -variable} {
    list [catch { parseargs0 argument configure newArg -variable} msg] $msg
} {0 {-variable {} {}}}

test parseargs.190 {argument configure newArg -value} {
    list [catch { parseargs0 argument configure newArg -value} msg] $msg
} {0 {-value {} {}}}

test parseargs.191 {argument configure newArg -value constValue} {
    list [catch {
	parseargs0 argument configure newArg -value constValue
    } msg] $msg
} {0 {}}

test parseargs.192 {argument configure newArg -value} {
    list [catch { parseargs0 argument configure newArg -value} msg] $msg
} {0 {-value {} constValue}}

test parseargs.193 {argument configure newArg -value ""} {
    list [catch {
	parseargs0 argument configure newArg -value ""
    } msg] $msg
} {0 {}}

test parseargs.194 {argument configure newArg -value} {
    list [catch { parseargs0 argument configure newArg -value} msg] $msg
} {0 {-value {} {}}}

test parseargs.195 { cget (missing arg)} {
    list [catch { parseargs0 cget} msg] $msg
} {1 {wrong # args: should be "parseargs0 cget option"}}

test parseargs.196 { cget badOption extraArg} {
    list [catch { parseargs0 cget badArg badOption extraArg} msg] $msg
} {1 {wrong # args: should be "parseargs0 cget option"}}

test parseargs.197 { cget badOption} {
    list [catch { parseargs0 cget badOption} msg] $msg
} {1 {unknown switch "badOption"
The following switches are available:
   -abbreviations bool
   -default string
   -description string
   -epilog string
   -error errorList
   -prefixchars string
   -program programName
   -usage string}}

test parseargs.198 { cget -abbreviations} {
    list [catch { parseargs0 cget -abbreviations} msg] $msg
} {0 0}

test parseargs.199 { cget -default} {
    list [catch { parseargs0 cget -default} msg] $msg
} {0 {}}

test parseargs.200 { cget -epilog} {
    list [catch { parseargs0 cget -epilog} msg] $msg
} {0 {}}

test parseargs.201 { cget -error} {
    list [catch { parseargs0 cget -error} msg] $msg
} {0 badoption}

test parseargs.202 { cget -prefixchars} {
    list [catch { parseargs0 cget -prefixchars} msg] $msg
} {0 -+}

test parseargs.203 { cget -program} {
    list [catch { parseargs0 cget -program} msg] $msg
} {0 {}}

test parseargs.204 { cget -usage} {
    list [catch { parseargs0 cget -usage} msg] $msg
} {0 {}}


test parseargs.205 { configure} {
    list [catch { parseargs0 configure} msg] $msg
} {0 {{-abbreviations 0 0} {-default {} {}} {-description {} {}} {-epilog {} {}} {-error badoption badoption} {-prefixchars -+ -+} {-program {} {}} {-usage {} {}}}}


test parseargs.206 { configure badOption} {
    list [catch { parseargs0 configure badOption} msg] $msg
} {1 {unknown switch "badOption"
The following switches are available:
   -abbreviations bool
   -default string
   -description string
   -epilog string
   -error errorList
   -prefixchars string
   -program programName
   -usage string}}

test parseargs.207 { configure -abbreviations} {
    list [catch { parseargs0 configure -abbreviations} msg] $msg
} {0 {-abbreviations 0 0}}

test parseargs.208 { configure -default} {
    list [catch { parseargs0 configure -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.209 { configure -epilog} {
    list [catch { parseargs0 configure -epilog} msg] $msg
} {0 {-epilog {} {}}}

test parseargs.210 { configure -error} {
    list [catch { parseargs0 configure -error} msg] $msg
} {0 {-error badoption badoption}}

test parseargs.211 { configure -prefixchars} {
    list [catch { parseargs0 configure -prefixchars} msg] $msg
} {0 {-prefixchars -+ -+}}

test parseargs.212 { configure -program} {
    list [catch { parseargs0 configure -program} msg] $msg
} {0 {-program {} {}}}

test parseargs.213 { configure -usage} {
    list [catch { parseargs0 configure -usage} msg] $msg
} {0 {-usage {} {}}}

test parseargs.214 { configure -abbreviations badBool} {
    list [catch { parseargs0 configure -abbreviations badBool} msg] $msg
} {1 {expected boolean value but got "badBool"}}

test parseargs.215 { configure -abbreviations 1} {
    list [catch { parseargs0 configure -abbreviations 1} msg] $msg
} {0 {}}

test parseargs.216 { configure -abbreviations true} {
    list [catch { parseargs0 configure -abbreviations true} msg] $msg
} {0 {}}

test parseargs.217 { configure -abbreviations yes} {
    list [catch { parseargs0 configure -abbreviations yes} msg] $msg
} {0 {}}

test parseargs.218 { configure -abbreviations on} {
    list [catch { parseargs0 configure -abbreviations on} msg] $msg
} {0 {}}

test parseargs.219 { configure -abbreviations} {
    list [catch { parseargs0 configure -abbreviations} msg] $msg
} {0 {-abbreviations 0 1}}

test parseargs.220 { configure -abbreviations 0} {
    list [catch { parseargs0 configure -abbreviations 0} msg] $msg
} {0 {}}

test parseargs.221 { configure -abbreviations false} {
    list [catch { parseargs0 configure -abbreviations false} msg] $msg
} {0 {}}

test parseargs.222 { configure -abbreviations no} {
    list [catch { parseargs0 configure -abbreviations no} msg] $msg
} {0 {}}

test parseargs.223 { configure -abbreviations off} {
    list [catch { parseargs0 configure -abbreviations off} msg] $msg
} {0 {}}

test parseargs.224 { configure -abbreviations} {
    list [catch { parseargs0 configure -abbreviations} msg] $msg
} {0 {-abbreviations 0 0}}

test parseargs.225 { configure -default} {
    list [catch { parseargs0 configure -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.226 { configure -default defValue} {
    list [catch {
	parseargs0 configure -default defValue
    } msg] $msg
} {0 {}}

test parseargs.227 { configure -default} {
    list [catch { parseargs0 configure -default} msg] $msg
} {0 {-default {} defValue}}

test parseargs.228 { configure -default ""} {
    list [catch {
	parseargs0 configure -default ""
    } msg] $msg
} {0 {}}

test parseargs.229 { configure -default} {
    list [catch { parseargs0 configure -default} msg] $msg
} {0 {-default {} {}}}

test parseargs.230 { configure -epilog} {
    list [catch { parseargs0 configure -epilog} msg] $msg
} {0 {-epilog {} {}}}

test parseargs.231 { configure -epilog defValue} {
    list [catch {
	parseargs0 configure -epilog defValue
    } msg] $msg
} {0 {}}

test parseargs.232 { configure -epilog} {
    list [catch { parseargs0 configure -epilog} msg] $msg
} {0 {-epilog {} defValue}}

test parseargs.233 { configure -epilog ""} {
    list [catch {
	parseargs0 configure -epilog ""
    } msg] $msg
} {0 {}}

test parseargs.234 { configure -epilog} {
    list [catch { parseargs0 configure -epilog} msg] $msg
} {0 {-epilog {} {}}}

test parseargs.235 { configure -error badFlag} {
    list [catch { parseargs0 configure -error badFlag} msg] $msg
} {1 {unknown error flag "badFlag": should be badoption or extraargs}}

test parseargs.236 { configure -error badoption} {
    list [catch { parseargs0 configure -error badoption} msg] $msg
} {0 {}}

test parseargs.237 { configure -error} {
    list [catch { parseargs0 configure -error} msg] $msg
} {0 {-error badoption badoption}}

test parseargs.238 { configure -error extraargs} {
    list [catch { parseargs0 configure -error extraargs} msg] $msg
} {0 {}}

test parseargs.239 { configure -error} {
    list [catch { parseargs0 configure -error} msg] $msg
} {0 {-error badoption extraargs}}

test parseargs.240 { configure -error "extraargs badoption badFlag"} {
    list [catch {
	parseargs0 configure -error "extraargs badoption badFlag"
    } msg] $msg
} {1 {unknown error flag "badFlag": should be badoption or extraargs}}

test parseargs.241 { configure -error} {
    list [catch { parseargs0 configure -error} msg] $msg
} {0 {-error badoption extraargs}}

test parseargs.242 { configure -error "extraargs badoption"} {
    list [catch {
	parseargs0 configure -error "extraargs badoption"
    } msg] $msg
} {0 {}}

test parseargs.243 { configure -prefixchars} {
    list [catch { parseargs0 configure -prefixchars} msg] $msg
} {0 {-prefixchars -+ -+}}

test parseargs.244 { configure -prefixchars ?} {
    list [catch {
	parseargs0 configure -prefixchars ?
    } msg] $msg
} {0 {}}

test parseargs.245 { configure -prefixchars} {
    list [catch { parseargs0 configure -prefixchars} msg] $msg
} {0 {-prefixchars -+ ?}}

test parseargs.246 { configure -prefixchars ""} {
    list [catch {
	parseargs0 configure -prefixchars ""
    } msg] $msg
} {0 {}}

test parseargs.247 { configure -prefixchars} {
    list [catch { parseargs0 configure -prefixchars} msg] $msg
} {0 {-prefixchars -+ {}}}

test parseargs.248 { configure -prefixchars "+-"} {
    list [catch {
	parseargs0 configure -prefixchars "+-"
    } msg] $msg
} {0 {}}

test parseargs.249 { configure -prefixchars} {
    list [catch { parseargs0 configure -prefixchars} msg] $msg
} {0 {-prefixchars -+ +-}}

test parseargs.250 { configure -program} {
    list [catch { parseargs0 configure -program} msg] $msg
} {0 {-program {} {}}}

test parseargs.251 { configure -program defValue} {
    list [catch {
	parseargs0 configure -program defValue
    } msg] $msg
} {0 {}}

test parseargs.252 { configure -program} {
    list [catch { parseargs0 configure -program} msg] $msg
} {0 {-program {} defValue}}

test parseargs.253 { configure -program ""} {
    list [catch {
	parseargs0 configure -program ""
    } msg] $msg
} {0 {}}

test parseargs.254 { configure -program} {
    list [catch { parseargs0 configure -program} msg] $msg
} {0 {-program {} {}}}

test parseargs.255 { configure -usage} {
    list [catch { parseargs0 configure -usage} msg] $msg
} {0 {-usage {} {}}}

test parseargs.256 { configure -usage defValue} {
    list [catch {
	parseargs0 configure -usage defValue
    } msg] $msg
} {0 {}}

test parseargs.257 { configure -usage} {
    list [catch { parseargs0 configure -usage} msg] $msg
} {0 {-usage {} defValue}}

test parseargs.258 { configure -usage ""} {
    list [catch {
	parseargs0 configure -usage ""
    } msg] $msg
} {0 {}}

test parseargs.259 { configure -usage} {
    list [catch { parseargs0 configure -usage} msg] $msg
} {0 {-usage {} {}}}


test parseargs.260 {blt::parseargs destroy parseargs0} {
    list [catch {blt::parseargs destroy parseargs0} msg] $msg
} {0 {}}

test parseargs.261 {parseargs0} {
    list [catch { parseargs0} msg] $msg
} {1 {invalid command name "parseargs0"}}

test parseargs.262 {blt::parseargs create -badOption 0} {
    list [catch {blt::parseargs create -badOption 0} msg] $msg
} {1 {unknown switch "-badOption"
The following switches are available:
   -abbreviations bool
   -default string
   -description string
   -epilog string
   -error errorList
   -prefixchars string
   -program programName
   -usage string}}


test parseargs.263 {blt::parseargs create -default (missing arg)} {
    list [catch {blt::parseargs create -default} msg] $msg
} {1 {value for "-default" missing}}

test parseargs.264 {blt::parseargs create -abbreviations (missing arg)} {
    list [catch {blt::parseargs create -abbreviations} msg] $msg
} {1 {value for "-abbreviations" missing}}

test parseargs.265 {blt::parseargs names} {
    list [catch {blt::parseargs names} msg] $msg
} {0 {}}

test parseargs.266 {blt::parseargs create myParser} {
    list [catch {blt::parseargs create myParser -program myProgram} msg] $msg
} {0 ::myParser}

test parseargs.267 {myParser add debug} {
    list [catch {
	myParser add debug -short -d -long --debug -type boolean -nargs ? \
	    -default 0 -help "Set the debugging level."
    } msg] $msg
} {0 debug}

test parseargs.268 {myParser add test} {
    list [catch { myParser add test -long test -short t } msg] $msg
} {1 {long name "test" must start with one the following prefix characters "-+"}}

test parseargs.269 {myParser add test} {
    list [catch { myParser add test -short t -long test } msg] $msg
} {1 {short name "t" must start with one the following prefix characters "-+"}}

test parseargs.270 {myParser names} {
    list [catch { myParser names } msg] $msg
} {0 debug}

test parseargs.271 {myParser add verbose} {
    list [catch {
	myParser add verbose -short -v -long --verbose -type int \
	    -nargs ? -default 0
    } msg] $msg
} {0 verbose}

test parseargs.272 {myParser names} {
    list [catch { myParser names } msg] $msg
} {0 {debug verbose}}

test parseargs.273 {myParser parse "-v"} {
    list [catch { myParser parse "-v" } msg] $msg
} {0 {}}

test parseargs.274 {myParser parse "-v 0"} {
    list [catch { myParser parse "-v 0" } msg] $msg
} {0 {}}

test parseargs.275 {myParser parse "-v 1"} {
    list [catch { myParser parse "-v 1" } msg] $msg
} {0 {}}

test parseargs.276 {myParser parse "-v 10"} {
    list [catch { myParser parse "-v 10" } msg] $msg
} {0 {}}

test parseargs.277 {myParser parse "-v 11"} {
    list [catch { myParser parse "-v 11" } msg] $msg
} {0 {}}

test parseargs.278 {myParser parse "-v -11"} {
    list [catch { myParser parse "-v -11" } msg] $msg
} {0 {}}

test parseargs.279 {myParser parse "-v false"} {
    list [catch { myParser parse "-v false" } msg] $msg
} {1 {expected integer but got "false": bad value for "--verbose"}}

test parseargs.280 {myParser parse "-v 1 2 3"} {
    list [catch { myParser parse "-v 1 2 3" } msg] $msg
} {0 {2 3}}

test parseargs.281 {myParser parse "-v badInt"} {
    list [catch { myParser parse "-v badInt" } msg] $msg
} {1 {expected integer but got "badInt": bad value for "--verbose"}}

test parseargs.282 {myParser parse "-verbose 1"} {
    list [catch { myParser parse "-verbose 1" } msg] $msg
} {1 {unknown option "-verbose"}}

test parseargs.283 {myParser parse "--verbos 1"} {
    list [catch { myParser parse "--verbos 1" } msg] $msg
} {1 {unknown option "--verbos"}}

test parseargs.284 {myParser configure -abbreviations 1} {
    list [catch { myParser configure -abbreviations 1 } msg] $msg
} {0 {}}

test parseargs.285 {myParser parse "--verbos 1"} {
    list [catch { myParser parse "--verbos 1" } msg] $msg
} {0 {}}

test parseargs.286 {myParser configure -abbreviations 0} {
    list [catch { myParser configure -abbreviations 0 } msg] $msg
} {0 {}}

test parseargs.287 {myParser argument configure "verbose" -min 0 -max 10} {
    list [catch { 
	myParser argument configure "verbose" -min 0 -max 10 
    } msg] $msg
} {0 {}}

test parseargs.288 {myParser parse "--verbose 0"} {
    list [catch { myParser parse "--verbose 0" } msg] $msg
} {0 {}}

test parseargs.289 {myParser parse "--verbose 10"} {
    list [catch { myParser parse "--verbose 10" } msg] $msg
} {0 {}}

test parseargs.290 {myParser parse "--verbose 11"} {
    list [catch { myParser parse "--verbose 11" } msg] $msg
} {1 {value "11" is greater than maximium "10": bad value for "--verbose"}}

test parseargs.291 {myParser parse "--verbose -10"} {
    list [catch { myParser parse "--verbose -10" } msg] $msg
} {1 {value "-10" is less than minimum "0": bad value for "--verbose"}}

test parseargs.292 {myParser parse "--debug 0"} {
    list [catch { myParser parse "--debug 0" } msg] $msg
} {0 {}}

test parseargs.293 {myParser parse "--debug true"} {
    list [catch { myParser parse "--debug true" } msg] $msg
} {0 {}}

test parseargs.294 {myParser parse "--debug yes"} {
    list [catch { myParser parse "--debug yes" } msg] $msg
} {0 {}}

test parseargs.295 {myParser parse "--debug on"} {
    list [catch { myParser parse "--debug on" } msg] $msg
} {0 {}}

test parseargs.296 {myParser parse "--debug false"} {
    list [catch { myParser parse "--debug true" } msg] $msg
} {0 {}}

test parseargs.297 {myParser parse "--debug no"} {
    list [catch { myParser parse "--debug no" } msg] $msg
} {0 {}}

test parseargs.298 {myParser parse "--debug off"} {
    list [catch { myParser parse "--debug off" } msg] $msg
} {0 {}}

test parseargs.299 {myParser parse "--debug 0"} {
    list [catch { myParser parse "--debug 0" } msg] $msg
} {0 {}}

test parseargs.300 {myParser parse "--debug 1000"} {
    list [catch { myParser parse "--debug 1000" } msg] $msg
} {0 {}}

test parseargs.301 {myParser parse "--debug -1000"} {
    list [catch { myParser parse "--debug -1000" } msg] $msg
} {0 {}}

test parseargs.302 {myParser parse "--debug -next"} {
    list [catch { myParser parse "--debug -next" } msg] $msg
} {1 {unknown option "-next"}}

test parseargs.303 {myParser parse "-d -1,0,1,2"} {
    list [catch { myParser parse "-d -1,0,1,2" } msg] $msg
} {1 {expected boolean value but got "-1,0,1,2": bad value for "--debug"}}

test parseargs.304 {myParser argument configure "verbose" -min 100 -max 10} {
    list [catch { 
	myParser argument configure "verbose" -min 100 -max 10 
    } msg] $msg
} {0 {}}

test parseargs.305 {myParser argument configure "verbose" -min 1 -max 10} {
    list [catch { 
	myParser argument configure "verbose" -min 1 -max 10 
    } msg] $msg
} {0 {}}

test parseargs.306 {myParser argument configure "verbose" -min abc -max def} {
    list [catch { 
	myParser argument configure "verbose" -min abc -max def 
    } msg] $msg
} {1 {expected floating-point number but got "abc"}}

test parseargs.307 {myParser argument configure "verbose" -min 0 -max 10} {
    list [catch { 
	myParser argument configure "verbose" -min 0 -max 10 
    } msg] $msg
} {0 {}}

test parseargs.308 {myParser configure -error ""} {
    list [catch { myParser configure -error "" } msg] $msg
} {0 {}}

test parseargs.309 {myParser parse "--debug -next"} {
    list [catch { myParser parse "--debug -next" } msg] $msg
} {0 -next}

test parseargs.310 {myParser add type} {
   list [catch { myParser add type -short -t -long -type \
		     -choices "fish fowl" -default fish 
    } msg] $msg
} {0 type}

test parseargs.311 {myParser parse "-type fish"} {
    list [catch { myParser parse "-type fish" } msg] $msg
} {0 {}}

test parseargs.312 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 fish}

test parseargs.313 {myParser parse "-type fowl"} {
    list [catch { myParser parse "-type fowl" } msg] $msg
} {0 {}}

test parseargs.314 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 fowl}

test parseargs.315 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 fowl}

test parseargs.316 {myParser parse "-type badValue"} {
    list [catch { myParser parse "-type badValue" } msg] $msg
} {1 {bad value "badValue", must be one of "fish fowl": bad value for "-type"}}

test parseargs.317 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 fowl}

test parseargs.318 {myParser argument configure "type"} {
    list [catch { 
	myParser argument configure "type" -type int -choices "1 5 10"
    } msg] $msg
} {0 {}}

test parseargs.319 {myParser parse "-type 5"} {
    list [catch { myParser parse "-type 5" } msg] $msg
} {0 {}}

test parseargs.320 {myParser parse "-type 1"} {
    list [catch { myParser parse "-type 1" } msg] $msg
} {0 {}}

test parseargs.321 {myParser parse "-type 10"} {
    list [catch { myParser parse "-type 10" } msg] $msg
} {0 {}}

test parseargs.322 {myParser parse "-type 7"} {
    list [catch { myParser parse "-type 7" } msg] $msg
} {1 {bad value "7", must be one of "1 5 10": bad value for "-type"}}

test parseargs.323 {myParser argument configure "type"} {
    list [catch { 
	myParser argument configure "type" -type float -choices "1 5 10"
    } msg] $msg
} {0 {}}

test parseargs.324 {myParser parse "-type 1e1"} {
    list [catch { myParser parse "-type 1e1" } msg] $msg
} {0 {}}

test parseargs.325 {myParser parse "-type 1 -type 10"} {
    list [catch { myParser parse "-type 1 -type 10" } msg] $msg
} {0 {}}

test parseargs.326 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 10}

test parseargs.327 {myParser parse "-type 1 -type 5 -type 10"} {
    list [catch { myParser parse "-type 1 -type 5 -type 10" } msg] $msg
} {0 {}}

test parseargs.328 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 10}


test parseargs.329 {myParser argument configure "type" -action append} {
    list [catch { myParser argument configure "type" -action append } msg] $msg
} {0 {}}

test parseargs.330 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 10}

test parseargs.331 {myParser parse "-type 1 -type 5 -type 10"} {
    list [catch { myParser parse "-type 1 -type 5 -type 10" } msg] $msg
} {0 {}}

test parseargs.332 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 {10 1 5 10}}

test parseargs.333 {myParser argument configure "type" -action store} {
    list [catch { myParser argument configure "type" -action store } msg] $msg
} {0 {}}

test parseargs.334 {myParser parse "-type 1e1"} {
    list [catch { myParser parse "-type 1e1" } msg] $msg
} {0 {}}

test parseargs.335 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 1e1}

test parseargs.336 {myParser argument configure "type" -action store_true} {
    list [catch {
	myParser argument configure "type" -action store_true
    } msg] $msg
} {0 {}}

test parseargs.337 {myParser parse "-type 5"} {
    list [catch { myParser parse "-type 5" } msg] $msg
} {0 {}}

test parseargs.338 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 1}

test parseargs.339 {myParser argument configure "type" -action store_false} {
    list [catch {
	myParser argument configure "type" -action store_false
    } msg] $msg
} {0 {}}

test parseargs.340 {myParser parse "-type 5"} {
    list [catch { myParser parse "-type 5" } msg] $msg
} {0 {}}

test parseargs.341 {myParser get "type"} {
    list [catch { myParser get "type" } msg] $msg
} {0 0}

test parseargs.342 {myParser argument configure "type" -action store} {
    list [catch {
	myParser argument configure "type" -action store -variable myVar
    } msg] $msg
} {0 {}}

test parseargs.343 {myParser parse "-type 5"} {
    list [catch { myParser parse "-type 5" } msg] $myVar
} {0 5}

set arrayvar(0) 1

test parseargs.344 {myParser argument configure "type" -variable badVar} {
    list [catch {
	myParser argument configure "type" -variable ::arrayvar
    } msg] $msg
} {0 {}}

test parseargs.345 {myParser parse "-type 5"} {
    list [catch { myParser parse "-type 5" } msg] $msg
} {1 {can't set "::arrayvar": variable is array}}

test parseargs.346 {myParser argument configure "type" -variable ""} {
    list [catch {
	myParser argument configure "type" -variable ""
    } msg] $msg
} {0 {}}


proc ProcessDate { value } {
    return [clock scan $value -gmt yes]
}

test parseargs.347 {myParser add date} {
    list [catch { 
	myParser add date -long -date -command ProcessDate \
	    -type int -metavar date -help "Set the date."
    } msg] $msg
} {0 date}

test parseargs.348 {myParser parse "-date 1/1/1970"} {
    list [catch { myParser parse "-date 1/1/1970" } msg] $msg
} {0 {}}


test parseargs.349 {myParser get "date"} {
    list [catch { myParser get "date" } msg] $msg
} {0 0}

test parseargs.350 {myParser argument configure "type" -required yes} {
    list [catch {
	myParser argument configure "type" -required yes 
    } msg] $msg
} {0 {}}

test parseargs.351 {myParser reset} {
    list [catch { myParser reset } msg] $msg
} {0 {}}

test parseargs.352 {myParser parse "-date 1/1/1970"} {
    list [catch { myParser parse "-date 1/1/1970" } msg] $msg
} {1 {missing required argument "-type"}}

test parseargs.353 {myParser get "date"} {
    list [catch { myParser get "date" } msg] $msg
} {0 0}

test parseargs.354 {myParser exists "date"} {
    list [catch { myParser exists "date" } msg] $msg
} {0 1}

test parseargs.355 {myParser exists "type"} {
    list [catch { myParser exists "type" } msg] $msg
} {0 1}

test parseargs.356 {myParser delete (no arg)} {
    list [catch { myParser delete } msg] $msg
} {0 {}}

test parseargs.357 {myParser delete badArg} {
    list [catch { myParser delete badArg } msg] $msg
} {1 {can't find argument "badArg" in parser "::myParser"}}

test parseargs.358 {myParser delete "type" "verbose" } {
    list [catch { myParser delete "type" "verbose" } msg] $msg
} {0 {}}

test parseargs.359 {myParser exists "type"} {
    list [catch { myParser exists "type" } msg] $msg
} {0 0}

test parseargs.360 {myParser get} { 
    list [catch { myParser get } msg] $msg 
} {0 {debug 0 date 0}}

test parseargs.361 {myParser parse "-date 1/2/1970 e f g"} {
    list [catch { 
	myParser parse "-date 1/2/1970 e f g" 
    } msg] $msg
} {0 {e f g}}

test parseargs.362 {myParser parse "a b c -date 1/2/1970 e f g"} {
    list [catch { 
	myParser parse "a b c -date 1/2/1970 e f g" 
    } msg] $msg
} {0 {a b c e f g}}

test parseargs.363 {myParser parse "--debug a b c -date 1/2/1970 e f g"} {
    list [catch { 
	myParser parse "a b --debug 0 c d -date 1/2/1970 e f g" 
    } msg] $msg
} {0 {a b c d e f g}}

test parseargs.364 {myParser add files} {
    list [catch { 
	myParser add files -short -f -long -files -nargs * -metavar file
    } msg] $msg
} {0 files}

test parseargs.365 {myParser parse "-f"} {
    list [catch { myParser parse "-f" } msg] $msg
} {0 {}}

test parseargs.366 {myParser get files} { 
    list [catch { myParser get files } msg] $msg 
} {0 {}}

test parseargs.367 {myParser add -nodefault} {
    list [catch { 
	myParser add collection -long -collection -nargs * -nodefault 1
    } msg] $msg
} {0 collection}

test parseargs.367 {myParser names} {
    list [catch { myParser names } msg] $msg
} {0 {debug date files collection}}

test parseargs.367 {myParser exists collection} {
    list [catch { myParser exists collection } msg] $msg
} {0 1}

test parseargs.367 {myParser argument configure collection} {
    list [catch { myParser argument configure collection } msg] $msg
} {0 {{-action store store} {-allowprefixchars 0 0} {-command {} {}} {-choices {} {}} {-current {} {}} {-default {} {}} {-destination {} collection} {-exclude {} {}} {-help {} {}} {-long {} -collection} {-metavar {} {}} {-max {} {}} {-min {} {}} {-nargs 1 *} {-nodefault 0 1} {-required 0 0} {-short {} {}} {-type string string} {-value {} {}} {-variable {} {}}}}


test parseargs.368 {myParser parse ""} {
    list [catch { myParser parse "" } msg] $msg
} {0 {}}

test parseargs.369 {myParser parse ""} {
    list [catch { myParser parse "" } msg] $msg
} {0 {}}

test parseargs.370 {myParser get collection} { 
    list [catch { myParser get collection } msg] $msg 
} {1 {no value was specified for argument "collection"}}

test parseargs.371 {myParser get} { 
    list [catch { myParser get } msg] $msg 
} {0 {debug 0 date 86400 files {}}}

test parseargs.372 {myParser parse "--debug -f a b c"} {
    list [catch { myParser parse "--debug -f a b c" } msg] $msg
} {0 {}}

test parseargs.373 {myParser parse "-f a b c -date 1/2/1970 e f g"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c -date 1/2/1970 e f g" 
    } msg] $msg
} {0 {e f g}}

test parseargs.374 {myParser parse "0 1 2 3 --debug -f a b c -date 1/1/1970 e f g"} {
    list [catch { 
	myParser reset
	myParser parse "0 1 2 3 --debug -f a b c -date 1/1/1970 e f g" 
    } msg] $msg
} {0 {0 1 2 3 e f g}}

test parseargs.375 {myParser parse "0 1 2 3 --debug -f -date 1/1/1970 e f g"} {
    list [catch { 
	myParser reset
	myParser parse "0 1 2 3 --debug -f -date 1/1/1970 e f g" 
    } msg] $msg
} {0 {0 1 2 3 e f g}}

test parseargs.376 {myParser parse "0 1 2 3 --debug -f c -date 1/1/1970 e f g"} {
    list [catch { 
	myParser reset
	myParser parse "0 1 2 3 --debug -f c -date 1/1/1970 e f g" 
    } msg] $msg
} {0 {0 1 2 3 e f g}}

test parseargs.377 {myParser parse "0 1 2 3 --debug -f c d -date 1/1/1970 e f g"} {
    list [catch { 
	myParser reset
	myParser parse "0 1 2 3 --debug -f c d -date 1/1/1970 e f g" 
    } msg] $msg
} {0 {0 1 2 3 e f g}}

test parseargs.378 {myParser get} {
    list [catch { myParser get } msg] $msg
} {0 {debug 0 date 0 files {c d}}}

test parseargs.379 {myParser argument configure files -nargs 3} {
    list [catch { myParser argument configure files -nargs 3 } msg] $msg
} {0 {}}

test parseargs.380 {myParser parse "-f a b c"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c" 
    } msg] $msg
} {0 {}}

test parseargs.381 {myParser parse "-f a b c -d"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c -d" 
    } msg] $msg
} {0 {}}

test parseargs.382 {myParser parse "-f a b c d"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c d" 
    } msg] $msg
} {0 d}

test parseargs.383 {myParser parse "-f"} {
    list [catch { 
	myParser reset
	myParser parse "-f" 
    } msg] $msg
} {1 {argument "-files" requires 3 value(s), found 0}}

test parseargs.384 {myParser parse "-f a"} {
    list [catch { 
	myParser reset
	myParser parse "-f a" 
    } msg] $msg
} {1 {argument "-files" requires 3 value(s), found 1}}

test parseargs.385 {myParser parse "-f a b"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b" 
    } msg] $msg
} {1 {argument "-files" requires 3 value(s), found 2}}

test parseargs.386 {myParser parse "-f a b c"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c" 
    } msg] $msg
} {0 {}}

test parseargs.387 {myParser parse "-d 0"} {
    list [catch { 
	myParser reset
	myParser parse "-d 0" 
    } msg] $msg
} {0 {}}


test parseargs.388 {myParser parse "0 1 2 3 --debug -f c d -date 1/1/1970 e f g"} {
    list [catch { 
	myParser reset
	myParser parse "0 1 2 3 --debug -f c d -date 1/1/1970 e f g" 
    } msg] $msg
} {1 {argument "-files" requires 3 value(s), found 2}}


test parseargs.389 {myParser argument configure files -nargs +} {
    list [catch { myParser argument configure files -nargs + } msg] $msg
} {0 {}}

test parseargs.390 {myParser parse "-f a b c d"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c d" 
    } msg] $msg
} {0 {}}

test parseargs.391 {myParser parse "-f"} {
    list [catch { 
	myParser reset
	myParser parse "-f" 
    } msg] $msg
} {1 {no values provided for "files": requires 1 or more values}}

test parseargs.392 {myParser parse "-f a"} {
    list [catch { 
	myParser reset
	myParser parse "-f a" 
    } msg] $msg
} {0 {}}

test parseargs.393 {myParser parse "-f a b"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b" 
    } msg] $msg
} {0 {}}

test parseargs.394 {myParser parse "-f a b c"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c" 
    } msg] $msg
} {0 {}}

test parseargs.395 {myParser parse "-f a b c -d"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c -d" 
    } msg] $msg
} {0 {}}

test parseargs.396 {myParser get files} { 
    list [catch { myParser get files } msg] $msg 
} {0 {a b c}}

test parseargs.397 {myParser argument configure files -nargs 0} {
    list [catch { myParser argument configure files -nargs 0 } msg] $msg
} {0 {}}

test parseargs.398 {myParser parse "-f a b c d"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c d" 
    } msg] $msg
} {0 {a b c d}}

test parseargs.399 {myParser parse "-f"} {
    list [catch { 
	myParser reset
	myParser parse "-f" 
    } msg] $msg
} {0 {}}

test parseargs.400 {myParser parse "-f a"} {
    list [catch { 
	myParser reset
	myParser parse "-f a" 
    } msg] $msg
} {0 a}

test parseargs.401 {myParser parse "-f a b"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b" 
    } msg] $msg
} {0 {a b}}

test parseargs.402 {myParser parse "-f a b c"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c" 
    } msg] $msg
} {0 {a b c}}

test parseargs.403 {myParser parse "-f a b c -d"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c -d" 
    } msg] $msg
} {0 {a b c}}

test parseargs.404 {myParser get files} { 
    list [catch { myParser get files } msg] $msg 
} {0 {}}

test parseargs.405 {myParser add things} {
    list [catch { 
	myParser add things -nargs *
    } msg] $msg
} {0 things}

test parseargs.406 {myParser parse "a b c -d"} {
    list [catch { 
	myParser reset
	myParser parse "a b c -d" 
    } msg] $msg
} {0 {}}

test parseargs.407 {myParser get } { 
    list [catch { myParser get } msg] $msg 
} {0 {debug 0 date {} files {} things {a b c}}}

test parseargs.408 {myParser parse "-f a b c -d"} {
    list [catch { 
	myParser reset
	myParser parse "-f a b c -d" 
    } msg] $msg
} {0 {}}

test parseargs.409 {myParser get } { 
    list [catch { myParser get } msg] $msg 
} {0 {debug 0 date {} files {} things {a b c}}}

test parseargs.410 {myParser parse ""} {
    list [catch { 
	myParser reset
	myParser parse "" 
    } msg] $msg
} {0 {}}

test parseargs.411 {myParser get } { 
    list [catch { myParser get } msg] $msg 
} {0 {debug 0 date {} files {} things {}}}

test parseargs.412 {myParser parse "a"} {
    list [catch { 
	myParser reset
	myParser parse "a" 
    } msg] $msg
} {0 {}}

test parseargs.413 {myParser get } { 
    list [catch { myParser get } msg] $msg 
} {0 {debug 0 date {} files {} things a}}

test parseargs.414 {myParser parse "a b -d"} {
    list [catch { 
	myParser reset
	myParser parse "a b -d" 
    } msg] $msg
} {0 {}}

test parseargs.415 {myParser get } { 
    list [catch { myParser get } msg] $msg 
} {0 {debug 0 date {} files {} things {a b}}}

test parseargs.416 {myParser delete things } {
    list [catch { myParser delete things } msg] $msg
} {0 {}}

test parseargs.417 {myParser add first } {
    list [catch { myParser add first } msg] $msg
} {0 first}

test parseargs.418 {myParser add second } {
    list [catch { myParser add second } msg] $msg
} {0 second}

test parseargs.419 {myParser add third } {
    list [catch { myParser add third } msg] $msg
} {0 third}

test parseargs.420 {myParser parse ""} {
    list [catch { 
	myParser reset
	myParser parse "" 
    } msg] $msg
} {0 {}}

test parseargs.421 {myParser get} {
    list [catch { myParser get } msg] $msg
} {0 {debug 0 date {} files {} first {} second {} third {}}}

test parseargs.422 {myParser parse "a"} {
    list [catch { 
	myParser reset
	myParser parse "a" 
    } msg] $msg
} {0 {}}

test parseargs.423 {myParser get} {
    list [catch { myParser get } msg] $msg
} {0 {debug 0 date {} files {} first a second {} third {}}}

test parseargs.424 {myParser parse "a b"} {
    list [catch { 
	myParser reset
	myParser parse "a b" 
    } msg] $msg
} {0 {}}

test parseargs.425 {myParser get} {
    list [catch { myParser get } msg] $msg
} {0 {debug 0 date {} files {} first a second b third {}}}

test parseargs.426 {myParser parse "a b c"} {
    list [catch { 
	myParser reset
	myParser parse "a b c" 
    } msg] $msg
} {0 {}}

test parseargs.427 {myParser get} {
    list [catch { myParser get } msg] $msg
} {0 {debug 0 date {} files {} first a second b third c}}

test parseargs.428 {myParser parse "a b c d"} {
    list [catch { 
	myParser reset
	myParser parse "a b c d" 
    } msg] $msg
} {0 d}

test parseargs.429 {myParser get} {
    list [catch { myParser get } msg] $msg
} {0 {debug 0 date {} files {} first a second b third c}}

test parseargs.430 {myParser argument configure files -nargs 0} {
    list [catch { myParser argument configure files -nargs + } msg] $msg
} {0 {}}

test parseargs.431 {myParser add fahrenheit} {
    list [catch {
	myParser add fahrenheit -long --fahrenheit -type boolean -nargs 0 \
	    -action store_true -default 0 -exclude centigrade
    } msg] $msg
} {0 fahrenheit}

test parseargs.432 {myParser add centigrade} {
    list [catch {
	myParser add centigrade -long --centigrade -type boolean -nargs 0 \
	    -action store_true -default 0 -exclude fahrenheit
    } msg] $msg
} {0 centigrade}

test parseargs.433 {myParser parse "--fahrenheit"} {
    list [catch { 
	myParser reset
	myParser parse "--fahrenheit" 
    } msg] $msg
} {0 {}}

test parseargs.434 {myParser parse "--centigrade"} {
    list [catch { 
	myParser reset
	myParser parse "--centigrade" 
    } msg] $msg
} {0 {}}

test parseargs.435 {myParser parse "--fahrenheit --centigrade"} {
    list [catch { 
	myParser reset
	myParser parse "--fahrenheit --centigrade" 
    } msg] $msg
} {1 {can't set both "--fahrenheit" and "--centigrade"}}

test parseargs.436 {myParser parse "--centigrade --fahrenheit"} {
    list [catch { 
	myParser reset
	myParser parse "--centigrade --fahrenheit" 
    } msg] $msg
} {1 {can't set both "--fahrenheit" and "--centigrade"}}

test parseargs.437 {myParser delete "fahrenheit" "centigrade" } {
    list [catch { myParser delete "fahrenheit" "centigrade" } msg] $msg
} {0 {}}

test parseargs.438 {myParser add fahrenheit} {
    list [catch {
	myParser add fahrenheit -long --fahrenheit -type boolean -nargs 0 \
	    -action store_true -default 0 -exclude badArg
    } msg] $msg
} {0 fahrenheit}

test parseargs.439 {myParser add centigrade} {
    list [catch {
	myParser add centigrade -long --centigrade -type boolean -nargs 0 \
	    -action store_true -default 0 -exclude badArg
    } msg] $msg
} {0 centigrade}

test parseargs.440 {myParser parse "--fahrenheit"} {
    list [catch { 
	myParser reset
	myParser parse "--fahrenheit" 
    } msg] $msg
} {0 {}}

test parseargs.441 {myParser parse "--centigrade"} {
    list [catch { 
	myParser reset
	myParser parse "--centigrade" 
    } msg] $msg
} {0 {}}

test parseargs.442 {myParser parse "--fahrenheit --centigrade"} {
    list [catch { 
	myParser reset
	myParser parse "--fahrenheit --centigrade" 
    } msg] $msg
} {0 {}}

test parseargs.443 {myParser parse "--centigrade --fahrenheit"} {
    list [catch { 
	myParser reset
	myParser parse "--centigrade --fahrenheit" 
    } msg] $msg
} {0 {}}

test parseargs.444 {myParser delete "fahrenheit" "centigrade" } {
    list [catch { myParser delete "fahrenheit" "centigrade" } msg] $msg
} {0 {}}

test parseargs.445 {myParser add test} {
    list [catch {
	myParser add test -long -test -short -t -default "NA" -value 10 \
	    -nargs 0 -type int -metavar arg -help {
		Specifies the command for testing. Takes one or more arguments. 
		The first argument is the command.
	    }
    } msg] $msg
} {0 test}

test parseargs.446 {myParser parse ""} {
    list [catch { 
	myParser reset
	myParser parse "" 
    } msg] $msg
} {0 {}}

test parseargs.447 {myParser get test} {
    list [catch { myParser get test } msg] $msg
} {0 NA}

test parseargs.448 {myParser parse "-t"} {
    list [catch { 
	myParser reset
	myParser parse "-t" 
    } msg] $msg
} {0 {}}

test parseargs.449 {myParser get test} {
    list [catch { myParser get test } msg] $msg
} {0 10}

test parseargs.450 {myParser configure test -value NA} {
    list [catch {
	myParser argument configure test -value NA 
    } msg] $msg
} {0 {}}

test parseargs.451 {myParser parse "-t"} {
    list [catch { 
	myParser reset
	myParser parse "-t" 
    } msg] $msg
} {1 {expected integer but got "NA": bad value for "-test"}}

test parseargs.452 {myParser get test} {
    list [catch { myParser get test } msg] $msg
} {0 NA}

test parseargs.453 {myParser configure test -value 0} {
    list [catch {
	myParser argument configure test -value 0
    } msg] $msg
} {0 {}}

test parseargs.454 {myParser delete first second third} {
    list [catch { myParser delete first second third } msg] $msg
} {0 {}}

test parseargs.455 {myParser add --} {
    list [catch { myParser add -- -short -- -nargs last } msg] $msg
} {0 --}

test parseargs.456 {myParser parse "-t -- -t --debug --files" } {
    list [catch { 
	myParser reset
	myParser parse "-t -- -t --debug --files" 
    } msg] $msg
} {0 {-t --debug --files}}

test parseargs.457 {myParser parse "rm -- -i -r" } {
    list [catch { 
	myParser reset
	myParser parse "-- -i -r" 
    } msg] $msg
} {0 {-i -r}}

test parseargs.458 {myParser get} {
    list [catch { myParser get } msg] $msg
} {0 {debug 0 date {} files {} test NA -- {}}}

test parseargs.459 {myParser parse "rm -- -i -r" } {
    list [catch { 
	myParser reset
	myParser parse "-- -i -r" 
    } msg] $msg
} {0 {-i -r}}

test parseargs.460 {myParser argument configure test} {
    list [catch { 
	myParser argument configure test -allowprefixchars 1 -nargs * \
	    -type string
    } msg] $msg
} {0 {}}

test parseargs.461 {myParser parse "-t -d --debug -f -files" } {
    list [catch { 
	myParser reset
	myParser parse "-t -d --debug -f -files"
    } msg] $msg
} {0 {}}

test parseargs.462 {myParser get test} {
    list [catch { myParser get test } msg] $msg
} {0 {-d --debug -f -files}}

test parseargs.463 {myParser add first } {
    list [catch {
	myParser add first -required 1 -help "First argument."
    } msg] $msg
} {0 first}

test parseargs.464 {myParser add second } {
    list [catch {
	myParser add second -required 1 -help "Second argument."
    } msg] $msg
} {0 second}

test parseargs.465 {myParser add third } {
    list [catch {
	myParser add third -required 1 -help "Third argument."
    } msg] $msg
} {0 third}

test parseargs.466 {myParser add long } {
    list [catch {
	myParser add long -long --areallylongargumentname -short -a  -help "A really long argument name." -metavar value 
    } msg] $msg
} {0 long}

test parseargs.467 {myParser configure -description } {
    list [catch {
	myParser configure -description \
	    "Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum." \
	    -epilog \
	    "At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et expedita distinctio. Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellendus. Temporibus autem quibusdam et aut officiis debitis aut rerum necessitatibus saepe eveniet ut et voluptates repudiandae sint et molestiae non recusandae. Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis voluptatibus maiores alias consequatur aut perferendis doloribus asperiores repellat."
    } msg] $msg
} {0 {}}


test parseargs.468 {myParser help} {
    list [catch { myParser help } msg] $msg
} {0 {
usage: myProgram [-a value] [-d [boolean]] [-date date] [-f file ...]
                 [-t [arg ...]] [--] first second third

 Lorem ipsum dolor sit amet, consectetur adipiscing elit, sed do eiusmod 
 tempor incididunt ut labore et dolore magna aliqua. Ut enim ad minim 
 veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea 
 commodo consequat. Duis aute irure dolor in reprehenderit in voluptate 
 velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat 
 cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id 
 est laborum. 

required arguments:
 first                        First argument. 
 second                       Second argument. 
 third                        Third argument. 

optional arguments:
 -a, --areallylongargumentname value
                              A really long argument name. 
 -d, --debug [boolean]        Set the debugging level. 
     -date date               Set the date. 
 -f, -files file ...
 -t, -test [arg ...]          Specifies the command for testing. Takes one 
                              or more arguments. The first argument is the 
                              command. 
 --

At vero eos et accusamus et iusto odio dignissimos ducimus qui blanditiis praesentium voluptatum deleniti atque corrupti quos dolores et quas molestias excepturi sint occaecati cupiditate non provident, similique sunt in culpa qui officia deserunt mollitia animi, id est laborum et dolorum fuga. Et harum quidem rerum facilis est et expedita distinctio. Nam libero tempore, cum soluta nobis est eligendi optio cumque nihil impedit quo minus id quod maxime placeat facere possimus, omnis voluptas assumenda est, omnis dolor repellendus. Temporibus autem quibusdam et aut officiis debitis aut rerum necessitatibus saepe eveniet ut et voluptates repudiandae sint et molestiae non recusandae. Itaque earum rerum hic tenetur a sapiente delectus, ut aut reiciendis voluptatibus maiores alias consequatur aut perferendis doloribus asperiores repellat.
}}


test parseargs.469 { blt::parseargs create testParser } {
    list [catch { blt::parseargs create testParser } msg] $msg
} {0 ::testParser}

test parseargs.470 { testParser help } {
    list [catch { testParser help } msg] $msg
} {0 {
usage: parseargs
}}

test parseargs.471 { testParser add arg1 } {
    list [catch { testParser add arg1 } msg] $msg
} {0 arg1}

test parseargs.472 { testParser help } {
    list [catch { testParser help } msg] $msg
} {0 {
usage: parseargs [arg1]

optional arguments:
 arg1
}}

test parseargs.473 { testParser add arg2 } {
    list [catch { testParser add arg2 } msg] $msg
} {0 arg2}

test parseargs.474 { testParser argument configure arg2 } {
    list [catch { testParser argument configure arg2 -required 1 } msg] $msg
} {0 {}}

test parseargs.475 { testParser help } {
    list [catch { testParser help } msg] $msg
} {0 {
usage: parseargs arg2 [arg1]

required arguments:
 arg2

optional arguments:
 arg1
}}

test parseargs.476 { testParser argument configure arg1 } {
    list [catch { testParser argument configure arg1 -required 1 } msg] $msg
} {0 {}}

test parseargs.477 { testParser help } {
    list [catch { testParser help } msg] $msg
} {0 {
usage: parseargs arg1 arg2

required arguments:
 arg1
 arg2
}}

test parseargs.478 { testParser argument configure arg1 } {
    list [catch { 
	testParser argument configure arg1 -required 0  -long -arg1  -short -a
    } msg] $msg
} {0 {}}

test parseargs.479 { testParser help } {
    list [catch { testParser help } msg] $msg
} {0 {
usage: parseargs [-a string] arg2

required arguments:
 arg2

optional arguments:
 -a, -arg1 string
}}

test parseargs.480 { testParser add help } {
    list [catch { 
	testParser add help -action help -short -h -long -help -nargs 0
    } msg] $msg
} {0 help}

test parseargs.481 { testParser help } {
    list [catch { testParser help } msg] $msg
} {0 {
usage: parseargs [-a string] [-h] arg2

required arguments:
 arg2

optional arguments:
 -a, -arg1 string
 -h, -help
}}

test parseargs.482 { testParser parse -h } {
    list [catch { testParser parse -h } msg] $msg
} {3 {
usage: parseargs [-a string] [-h] arg2

required arguments:
 arg2

optional arguments:
 -a, -arg1 string
 -h, -help
}}

test parseargs.483 { testParser parse "a b c -h" } {
    list [catch { testParser parse "a b c -h" } msg] $msg
} {3 {
usage: parseargs [-a string] [-h] arg2

required arguments:
 arg2

optional arguments:
 -a, -arg1 string
 -h, -help
}}

test parseargs.484 { testParser parse "-h a b c d" } {
    list [catch { testParser parse "-h a b c d" } msg] $msg
} {3 {
usage: parseargs [-a string] [-h] arg2

required arguments:
 arg2

optional arguments:
 -a, -arg1 string
 -h, -help
}}

test parseargs.485 {blt::parseargs destroy testParser} {
    list [catch { blt::parseargs destroy testParser } msg] $msg
} {0 {}}

test parseargs.486 {blt::parseargs create testParser} {
    list [catch {
	blt::parseargs create testParser -program myProgram -abbreviations 1
    } msg] $msg
} {0 ::testParser}

test parseargs.487 {testParser add debug} {
    list [catch {
	testParser add debug -short -d -long -debug -type boolean -nargs 0 \
	    -default 0 -help "Set the debugging level."
    } msg] $msg
} {0 debug}

test parseargs.488 {testParser add delete} {
    list [catch {
	testParser add delete -short -x -long -delete -type string  \
	    -help "Delete an item."
    } msg] $msg
} {0 delete}

test parseargs.489 {testParser add dog} {
    list [catch {
	testParser add dog -short -y -long -dog -type string  \
	    -help "Run like a dog."
    } msg] $msg
} {0 dog}

test parseargs.490 {testParser parse "-d"} {
    list [catch { testParser parse "-d" } msg] $msg
} {0 {}}

test parseargs.491 {blt::parseargs destroy testParser myParser} {
    list [catch { blt::parseargs destroy testParser myParser } msg] $msg
} {0 {}}


test parseargs.492 {blt::parseargs create myParser} {
    list [catch {blt::parseargs create myParser} msg] $msg
} {0 ::myParser}

test parseargs.493 {myParser add temp} {
    list [catch {
	myParser add temp -short -t -long -temp -type float  \
	    -help "Set temperature."
    } msg] $msg
} {0 temp}

test parseargs.494 {myParser parse "-t -10"} {
    list [catch { myParser parse "-t -10" } msg] $msg
} {0 {}}

test parseargs.495 {myParser argument configure temp} {
    list [catch { myParser argument configure temp -min 0 } msg] $msg
} {0 {}}

test parseargs.496 {myParser get temp} {
    list [catch { myParser get temp} msg] $msg
} {0 -10}

test parseargs.497 {myParser parse "-t -10"} {
    list [catch { myParser parse "-t -10" } msg] $msg
} {1 {value "-10" is less than minimum "0": bad value for "-temp"}}

test parseargs.498 {myParser argument configure temp} {
    list [catch { myParser argument configure temp -min "" -max 100 } msg] $msg
} {0 {}}

test parseargs.499 {myParser parse "-t 100"} {
    list [catch { myParser parse "-t 100" } msg] $msg
} {0 {}}

test parseargs.500 {myParser parse "-t 0100"} {
    list [catch { myParser parse "-t 0100" } msg] $msg
} {0 {}}

test parseargs.501 {myParser get temp} {
    list [catch { myParser get temp} msg] $msg
} {0 0100}

test parseargs.502 {myParser argument configure temp} {
    list [catch { myParser argument configure temp -max badNum } msg] $msg
} {1 {expected floating-point number but got "badNum"}}


test parseargs.503 {myParser parse "-t 100"} {
    list [catch { myParser parse "-t 100" } msg] $msg
} {0 {}}


test parseargs.504 {myParser argument configure temp} {
    list [catch { myParser argument configure temp -default 120} msg] $msg
} {0 {}}

test parseargs.505 {myParser argument configure temp} {
    list [catch { myParser argument configure temp -min 120} msg] $msg
} {0 {}}

test parseargs.506 {myParser argument configure temp} {
    list [catch { myParser argument configure temp -min 120} msg] $msg
} {0 {}}

test parseargs.507 {myParser parse "-t 100"} {
    list [catch { myParser parse "-t 100" } msg] $msg
} {1 {value "100" is less than minimum "120": bad value for "-temp"}}

test parseargs.508 {myParser get temp} {
    list [catch { myParser get temp} msg] $msg
} {0 100}

test parseargs.509 {myParser argument configure temp} {
    list [catch { 
	myParser argument configure temp -min 0 -max 99.9999999 
    } msg] $msg
} {0 {}}

test parseargs.510 {myParser argument configure temp} {
    list [catch { 
	myParser argument configure temp -type integer
    } msg] $msg
} {1 {expected integer but got "99.9999999": bad maximum value for "-temp"}}

test parseargs.511 {myParser parse "-t 100"} {
    list [catch { myParser parse "-t 100" } msg] $msg
} {0 {}}

test parseargs.512 {myParser argument configure temp} {
    list [catch { 
	myParser argument configure temp -type boolean -min 0 -max 200
    } msg] $msg
} {0 {}}

test parseargs.513 {myParser parse "-t 100"} {
    list [catch { myParser parse "-t 100" } msg] $msg
} {0 {}}

test parseargs.514 {myParser get temp} {
    list [catch { myParser get temp} msg] $msg
} {0 100}


test parseargs.515 {myParser parse "-t 200"} {
    list [catch { myParser parse "-t 200" } msg] $msg
} {0 {}}


test parseargs.516 {myParser get temp} {
    list [catch { myParser get temp} msg] $msg
} {0 200}


test parseargs.517 {myParser parse "-t no"} {
    list [catch { myParser parse "-t no" } msg] $msg
} {0 {}}

test parseargs.518 {myParser parse "-t 0"} {
    list [catch { myParser parse "-t 0" } msg] $msg
} {0 {}}

test parseargs.519 {myParser argument configure temp} {
    list [catch { 
	myParser argument configure temp -type double -action append 
    } msg] $msg
} {0 {}}

test parseargs.520 {myParser parse "-t 10 -t 20 -t 30"} {
    list [catch { myParser parse "-t 10 -t 20 -t 30" } msg] $msg
} {0 {}}

test parseargs.521 {myParser get temp} {
    list [catch { myParser get temp} msg] $msg
} {0 {0 10 20 30}}


test parseargs.522 {myParser reset } {
    list [catch { myParser reset } msg] $msg
} {0 {}}

test parseargs.523 {myParser get temp} {
    list [catch { myParser get temp } msg] $msg
} {0 120}

test parseargs.524 {myParser argument configure temp} {
    list [catch { myParser argument configure temp} msg] $msg
} {0 {{-action store append} {-allowprefixchars 0 0} {-command {} {}} {-choices {} {}} {-current {} {}} {-default {} 120} {-destination {} temp} {-exclude {} {}} {-help {} {Set temperature.}} {-long {} -temp} {-metavar {} {}} {-max {} 200} {-min {} 0} {-nargs 1 1} {-required 0 0} {-short {} -t} {-type string double} {-value {} {}} {-variable {} {}}}}

test parseargs.525 {myParser parse "-t 1.0 -t 2.0 -t 3.0"} {
    list [catch { myParser parse "-t 1.0 -t 2.0 -t 3.0" } msg] $msg
} {0 {}}

test parseargs.526 {myParser get temp} {
    list [catch { myParser get temp} msg] $msg
} {0 {1.0 2.0 3.0}}

test parseargs.527 {myParser delete temp} {
    list [catch { myParser delete temp } msg] $msg
} {0 {}}

test parseargs.528 {myParser add temp} {
    list [catch { 
	myParser add temp -short -t -long -temp \
	    -type int -min 0 -max 200 -action append
    } msg] $msg
} {0 temp}

test parseargs.529 {myParser parse "-t 10 -t 20 -t 30"} {
    list [catch { myParser parse "-t 10 -t 20 -t 30" } msg] $msg
} {0 {}}

test parseargs.530 {myParser get temp} {
    list [catch { myParser get temp} msg] $msg
} {0 {10 20 30}}

test parseargs.531 {myParser configure temp} {
    list [catch { 
	myParser argument configure temp -choices "-10 50 100 -100"
    } msg] $msg
} {0 {}}

test parseargs.532 {myParser parse "-t 50"} {
    list [catch { myParser parse "-t 50" } msg] $msg
} {0 {}}

test parseargs.533 {myParser parse "-t 49"} {
    list [catch { myParser parse "-t 49" } msg] $msg
} {1 {bad value "49", must be one of "-10 50 100 -100": bad value for "-temp"}}

test parseargs.534 {myParser configure temp} {
    list [catch { myParser argument configure temp -min 50 -max 200 } msg] $msg
} {0 {}}

test parseargs.535 {myParser parse "-t -10"} {
    list [catch { myParser parse "-t -10" } msg] $msg
} {1 {value "-10" is less than minimum "50": bad value for "-temp"}}

test parseargs.536 {myParser parse "-t 75"} {
    list [catch { myParser parse "-t 75" } msg] $msg
} {1 {bad value "75", must be one of "-10 50 100 -100": bad value for "-temp"}}

test parseargs.537 {myParser delete temp} {
    list [catch { myParser delete temp } msg] $msg
} {0 {}}

test parseargs.538 {myParser add temp} {
    list [catch { 
	myParser add string -short -s -long -string -type string -action append 
	myParser add int -short -i -long -int -type int -action append \
	    -destination string
	myParser add float -short -f -long -float -type float -action append \
	    -destination string
	myParser add boolean -short -b -long -boolean -type boolean \
	    -action append -destination string
	myParser names
    } msg] $msg
} {0 {string int float boolean}}

test parseargs.539 {myParser parse "-s abc -i 10 -f 3.1415 -b yes"} {
    list [catch { myParser parse "-s abc -i 10 -f 3.1415 -b yes"} msg] $msg
} {0 {}}

test parseargs.540 {myParser get string} {
    list [catch { myParser get string } msg] $msg
} {0 {abc 10 3.1415 yes}}

test parseargs.541 {myParser get boolean} {
    list [catch { myParser get boolean } msg] $msg
} {0 {abc 10 3.1415 yes}}

test parseargs.542 {myParser get int} {
    list [catch { myParser get int } msg] $msg
} {0 {abc 10 3.1415 yes}}

test parseargs.543 {myParser get float} {
    list [catch { myParser get float } msg] $msg
} {0 {abc 10 3.1415 yes}}

test parseargs.544 {myParser delete string} {
    list [catch { myParser delete string } msg] $msg
} {0 {}}

test parseargs.545 {myParser parse "-i 10000"} {
    list [catch { myParser parse "-i 10000"} msg] $msg
} {0 {}}

test parseargs.546 {myParser get int} {
    list [catch { myParser get int } msg] $msg
} {0 10000}

test parseargs.547 {myParser get float} {
    list [catch { myParser get float } msg] $msg
} {0 {}}

test parseargs.548 {myParser parse "-f 0.01"} {
    list [catch { myParser parse "-f 0.01"} msg] $msg
} {0 {}}

test parseargs.549 {myParser get float} {
    list [catch { myParser get float } msg] $msg
} {0 0.01}

test parseargs.550 {myParser configure -prefixchar /} {
    list [catch { myParser configure -prefixchar /} msg] $msg
} {0 {}}

test parseargs.551 {myParser parse "-s abc -i 10 -f 3.1415 -b yes"} {
    list [catch { myParser parse "-s abc -i 10 -f 3.1415 -b yes"} msg] $msg
} {0 {-s abc -i 10 -f 3.1415 -b yes}}

test parseargs.552 {myParser configure -error "badoption extraargs" } {
    list [catch { myParser configure -error "badoption extraargs" } msg] $msg
} {0 {}}

test parseargs.553 {myParser parse "-s abc -i 10 -f 3.1415 -b yes"} {
    list [catch { myParser parse "-s abc -i 10 -f 3.1415 -b yes"} msg] $msg
} {1 {unknown arguments found}}

test parseargs.554 {myParser parse "/i 10 /f 3.1415 /b yes"} {
    list [catch { myParser parse "/i 10 /f 3.1415 /b yes"} msg] $msg
} {1 {unknown option "/i"}}

test parseargs.555 {myParser configure int -short /i -long /int} {
    list [catch { myParser argument configure int -short /i -long /int } msg] $msg
} {0 {}}

test parseargs.556 {myParser parse "/i 10"} {
    list [catch { myParser parse "/i 10"} msg] $msg
} {0 {}}

test parseargs.557 {myParser get int} {
    list [catch { myParser get int } msg] $msg
} {0 {10000 10}}

test parseargs.558 {myParser get string defValue} {
    list [catch { myParser get string defValue } msg] $msg
} {0 defValue}

blt::parseargs create myParser2
myParser2 add "input" -required yes  -help "Input file"
myParser2 add "output" -required yes  -help "Output file"
myParser2 add "keepList" -default "NA" \
    -help "List of things to keep."

test parseargs.559 {myParser2 parse no args} {
    list [catch { myParser2 parse "" optArr} msg] $msg
} {1 {missing required argument "input"}}

test parseargs.560 {myParser2 parse one arg} {
    list [catch { myParser2 parse "myInput" optArr} msg] $msg
} {1 {missing required argument "output"}}

test parseargs.561 {myParser2 parse two args} {
    list [catch { myParser2 parse "myInput myOutput" optArr} msg] $msg
} {0 {}}

test parseargs.562 {myParser2 get keepList} {
    list [catch { myParser2 get keepList } msg] $msg
} {0 NA}

test parseargs.563 {myParser2 get keepList} {
    list [catch { set optArr(keepList) } msg] $msg
} {0 NA}

test parseargs.564 {myParser2 parse two args} {
  list [catch { myParser2 parse "myInput myOutput thing" optArr} msg] $msg
} {0 {}}

test parseargs.565 {myParser2 get keepList} {
    list [catch { set optArr(keepList) } msg] $msg
} {0 thing}

test parseargs.566 {myParser2 parse two args} {
  list [catch { myParser2 parse {myInput myOutput "a b c"} optArr} msg] $msg
} {0 {}}

test parseargs.567 {myParser2 get keepList} {
    list [catch { set optArr(keepList) } msg] $msg
} {0 {a b c}}

blt::parseargs destroy myParser2

exit 0

# Missing tests.
#  1. Test abbreviations with --
#  3. Multiple nargs=* positional arguments,
#  4. Help 
# 10. -destination w/ append, store, store_true, store_false
# 12. +args.
# 14. 0 or 1 args w/ -allowprefixchars

