
package require BLT

if {[info procs test] != "test"} {
    source defs
}

if [file exists ../library] {
    set blt_library ../library
}

set VERBOSE 0

test compare.1 {string no arg} {
    list [catch {blt::stringutils} msg] $msg
} {1 {wrong # args: should be one of...
  blt::stringutils begins str pattern ?switches?
  blt::stringutils contains str pattern ?switches?
  blt::stringutils dictcompare string1 string2
  blt::stringutils ends str pattern ?switches?
  blt::stringutils equals str pattern ?switches?
  blt::stringutils isbetween str firstStr lastStr ?switches?
  blt::stringutils ismember str list ?switches?}}

test compare.2 {number no arg} {
    list [catch {blt::numberutils} msg] $msg
} {1 {wrong # args: should be one of...
  blt::numberutils < value1 value2
  blt::numberutils <= value1 value2
  blt::numberutils == value1 value2
  blt::numberutils > value1 value2
  blt::numberutils >= value1 value2
  blt::numberutils eq value1 value2
  blt::numberutils equals value1 value2
  blt::numberutils ge value1 value2
  blt::numberutils greaterthan value1 value2
  blt::numberutils gt value1 value2
  blt::numberutils isbetween value firstNum lastNum
  blt::numberutils ismember value numberList ?switches?
  blt::numberutils le value1 value2
  blt::numberutils lessthan value1 value2
  blt::numberutils lt value1 value2}}

test compare.3 {number badArg} {
    list [catch {blt::numberutils badArg} msg] $msg
} {1 {bad operation "badArg": should be one of...
  blt::numberutils < value1 value2
  blt::numberutils <= value1 value2
  blt::numberutils == value1 value2
  blt::numberutils > value1 value2
  blt::numberutils >= value1 value2
  blt::numberutils eq value1 value2
  blt::numberutils equals value1 value2
  blt::numberutils ge value1 value2
  blt::numberutils greaterthan value1 value2
  blt::numberutils gt value1 value2
  blt::numberutils isbetween value firstNum lastNum
  blt::numberutils ismember value numberList ?switches?
  blt::numberutils le value1 value2
  blt::numberutils lessthan value1 value2
  blt::numberutils lt value1 value2}}

test compare.4 {number isbetween } {
    list [catch {blt::numberutils isbetween} msg] $msg
} {1 {wrong # args: should be "blt::numberutils isbetween value firstNum lastNum"}}

test compare.5 {number isbetween 1} {
    list [catch {blt::numberutils isbetween 1} msg] $msg
} {1 {wrong # args: should be "blt::numberutils isbetween value firstNum lastNum"}}

test compare.6 {number isbetween 1 2} {
    list [catch {blt::numberutils isbetween 1 2} msg] $msg
} {1 {wrong # args: should be "blt::numberutils isbetween value firstNum lastNum"}}


test compare.7 {number isbetween 1 2 3} {
    list [catch {blt::numberutils isbetween 1 2 3} msg] $msg
} {0 0}

test compare.8 {number isbetween 1 0 3} {
    list [catch {blt::numberutils isbetween 1 0 3} msg] $msg
} {0 1}

test compare.9 {number isbetween badArg 0 3} {
    list [catch {blt::numberutils isbetween badArg 0 3} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.10 {number isbetween 1 badArg 3} {
    list [catch {blt::numberutils isbetween 1 badArg 3} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.11 {number isbetween 1 0 badArg} {
    list [catch {blt::numberutils isbetween 1 0 badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.12 {number isbetween 1 0 3 extraArg} {
    list [catch {blt::numberutils isbetween 1 0 3 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils isbetween value firstNum lastNum"}}

test compare.13 {number isbetween 1 0 3} {
    list [catch {blt::numberutils isbetween 1 0 3} msg] $msg
} {0 1}

test compare.14 {number isbetween 1 3 0} {
    list [catch {blt::numberutils isbetween 1 3 0} msg] $msg
} {0 1}

test compare.15 {number isbetween 1.00000001 1 2} {
    list [catch {blt::numberutils isbetween 1.000000001 1 2} msg] $msg
} {0 1}

test compare.16 {number isbetween 1.00000001 1 0} {
    list [catch {blt::numberutils isbetween 1.000000001 1 0} msg] $msg
} {0 0}

test compare.17 {number eq 1.00000000000000000001 1 } {
    list [catch {blt::numberutils eq 1.00000000000000000001 1} msg] $msg
} {0 1}

test compare.18 {number eq} {
    list [catch {blt::numberutils eq} msg] $msg
} {1 {wrong # args: should be "blt::numberutils eq value1 value2"}}

test compare.19 {number eq badArg} {
    list [catch {blt::numberutils eq badArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils eq value1 value2"}}

test compare.20 {number eq badArg badArg} {
    list [catch {blt::numberutils eq badArg badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.21 {number eq 1 1 -badSwitch} {
    list [catch {blt::numberutils eq 1 1 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils eq value1 value2"}}

test compare.22 {number ismember} {
    list [catch {blt::numberutils ismember} msg] $msg
} {1 {wrong # args: should be "blt::numberutils ismember value numberList ?switches?"}}

test compare.23 {number ismember arg} {
    list [catch {blt::numberutils ismember arg } msg] $msg
} {1 {wrong # args: should be "blt::numberutils ismember value numberList ?switches?"}}

test compare.24 {number ismember 1.0 ""} {
    list [catch {blt::numberutils ismember 1.0 "" } msg] $msg
} {0 0}

test compare.25 {number ismember 1.0 "1 2 3"} {
    list [catch {blt::numberutils ismember 1.0 "1 2 3" } msg] $msg
} {0 1}

test compare.26 {number ismember 1.0 "1e0 2 3"} {
    list [catch {blt::numberutils ismember 1.0 "1e0 2 3" } msg] $msg
} {0 1}

test compare.27 {number ismember 1.0 "3 2 1"} {
    list [catch {blt::numberutils ismember 1.0 "3 2 1" } msg] $msg
} {0 1}

test compare.28 {number ismember 1.0 "3 1 2 1"} {
    list [catch {blt::numberutils ismember 1.0 "3 1 2 1" } msg] $msg
} {0 1}

test compare.29 {number ismember 100 "3 1 2 1e2"} {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" } msg] $msg
} {0 1}

test compare.30 {number ismember 1.0 "1 2 3" -sorted increasing} {
    list [catch {blt::numberutils ismember 1.0 "1 2 3" -sorted increasing} msg] $msg
} {0 1}

test compare.31 {number ismember 1.0 "1 2 3" -sorted decreasing} {
    list [catch {blt::numberutils ismember 1.0 "1 2 3" -sorted decreasing} msg] $msg
} {0 0}

test compare.32 {number ismember 1.0 "3 2 1" -sorted decreasing} {
    list [catch {blt::numberutils ismember 1.0 "3 2 1" -sorted decreasing} msg] $msg
} {0 1}

test compare.33 {number ismember 1.0 "1e0 2 3"} {
    list [catch {blt::numberutils ismember 1.0 "1e0 2 3" -sorted increasing} msg] $msg
} {0 1}

test compare.34 {number ismember 1.0 "3 2 1" -sorted increasing} {
    list [catch {blt::numberutils ismember 1.0 "3 2 1" -sorted increasing} msg] $msg
} {0 0}

test compare.35 {number ismember 1.0 "3 1 2 1" -sorted increasing} {
    list [catch {blt::numberutils ismember 1.0 "3 1 2 1" -sorted increasing} msg] $msg
} {0 1}

test compare.36 {number ismember 100 "3 1 2 1e2" -sorted increasing} {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" -sorted increasing} msg] $msg
} {0 1}

test compare.37 {number ismember 100 "3 1 2 1e2" -sorted badValue} {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" -sorted badValue} msg] $msg
} {1 {bad sorted value "badValue": should be decreasing, increasing, or none}}

test compare.38 {number ismember 100 "3 1 2 1e2" -badSwitch} {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -sorted decreasing|increasing}}

test compare.39 {string equals (no args)} {
    list [catch {blt::stringutils equals} msg] $msg
} {1 {wrong # args: should be "blt::stringutils equals str pattern ?switches?"}}

test compare.40 {string equals (one arg)} {
    list [catch {blt::stringutils equals a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils equals str pattern ?switches?"}}

test compare.41 {string equals (bad switch)} {
    list [catch {blt::stringutils equals a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trim left|right|both|none}} 

test compare.42 {string equals (bad switch)} {
    list [catch {blt::stringutils equals a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trim left|right|both|none}} 

test compare.43 {string equals -trim badValue} {
    list [catch {blt::stringutils equals a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.44 {string equals a b} {
    list [catch {blt::stringutils equals a b} msg] $msg
} {0 0}

test compare.45 {string equals a ab} {
    list [catch {blt::stringutils equals a ab} msg] $msg
} {0 0}

test compare.46 {string equals ab a} {
    list [catch {blt::stringutils equals ab a} msg] $msg
} {0 0}

test compare.47 {string equals a a} {
    list [catch {blt::stringutils equals a a} msg] $msg
} {0 1}

test compare.48 {string equals -trim left} {
    list [catch {blt::stringutils equals "  a" a -trim left} msg] $msg
} {0 1}

test compare.49 {string equals -trim right} {
    list [catch {blt::stringutils equals "a  " a -trim right} msg] $msg
} {0 1}

test compare.50 {string equals -trim both} {
    list [catch {blt::stringutils equals " a " a -trim both} msg] $msg
} {0 1}

test compare.51 {string equals -trim none} {
    list [catch {blt::stringutils equals "a" a -trim none} msg] $msg
} {0 1}

test compare.52 {string equals -trim left} {
    list [catch {blt::stringutils equals "  a" b -trim left} msg] $msg
} {0 0}

test compare.53 {string equals -trim right} {
    list [catch {blt::stringutils equals "a  " b -trim right} msg] $msg
} {0 0}

test compare.54 {string equals -trim both} {
    list [catch {blt::stringutils equals " a " b -trim both} msg] $msg
} {0 0}

test compare.55 {string equals -trim none} {
    list [catch {blt::stringutils equals "a" b -trim none} msg] $msg
} {0 0}


test compare.56 {string equals a A -nocase} {
    list [catch {blt::stringutils equals a A -nocase} msg] $msg
} {0 1}

test compare.57 {string equals A a} {
    list [catch {blt::stringutils equals A a -nocase} msg] $msg
} {0 1}

test compare.58 {string equals AB ab} {
    list [catch {blt::stringutils equals AB ab -nocase} msg] $msg
} {0 1}

test compare.59 {string equals AAAA aaa} {
    list [catch {blt::stringutils equals AAAA aaa} msg] $msg
} {0 0}

test compare.60 {string isbetween d a z} {
    list [catch {blt::stringutils isbetween d a z} msg] $msg
} {0 1}

test compare.61 {string begins (no args)} {
    list [catch {blt::stringutils begins} msg] $msg
} {1 {wrong # args: should be "blt::stringutils begins str pattern ?switches?"}}

test compare.62 {string begins (one arg)} {
    list [catch {blt::stringutils begins a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils begins str pattern ?switches?"}}

test compare.63 {string begins (bad switch)} {
    list [catch {blt::stringutils begins a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trim left|right|both|none}} 

test compare.64 {string begins (bad switch)} {
    list [catch {blt::stringutils begins a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trim left|right|both|none}} 

test compare.65 {string begins -trim badValue} {
    list [catch {blt::stringutils begins a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.66 {string begins a b} {
    list [catch {blt::stringutils begins a b} msg] $msg
} {0 0}

test compare.67 {string begins a ab} {
    list [catch {blt::stringutils begins a ab} msg] $msg
} {0 0}

test compare.68 {string begins ab a} {
    list [catch {blt::stringutils begins ab a} msg] $msg
} {0 1}

test compare.69 {string begins a a} {
    list [catch {blt::stringutils begins a a} msg] $msg
} {0 1}

test compare.70 {string begins -trim left} {
    list [catch {blt::stringutils begins "  a" a -trim left} msg] $msg
} {0 1}

test compare.71 {string begins -trim right} {
    list [catch {blt::stringutils begins "a  " a -trim right} msg] $msg
} {0 1}

test compare.72 {string begins -trim both} {
    list [catch {blt::stringutils begins " a " a -trim both} msg] $msg
} {0 1}

test compare.73 {string begins -trim none} {
    list [catch {blt::stringutils begins "a" a -trim none} msg] $msg
} {0 1}

test compare.74 {string begins -trim left} {
    list [catch {blt::stringutils begins "  a" b -trim left} msg] $msg
} {0 0}

test compare.75 {string begins -trim right} {
    list [catch {blt::stringutils begins "a  " b -trim right} msg] $msg
} {0 0}

test compare.76 {string begins -trim both} {
    list [catch {blt::stringutils begins " a " b -trim both} msg] $msg
} {0 0}

test compare.77 {string begins -trim none} {
    list [catch {blt::stringutils begins "a" b -trim none} msg] $msg
} {0 0}


test compare.78 {string begins a A -nocase} {
    list [catch {blt::stringutils begins a A -nocase} msg] $msg
} {0 1}

test compare.79 {string begins A a} {
    list [catch {blt::stringutils begins A a -nocase} msg] $msg
} {0 1}

test compare.80 {string begins AB ab} {
    list [catch {blt::stringutils begins AB ab -nocase} msg] $msg
} {0 1}

test compare.81 {string begins AAAA aaa} {
    list [catch {blt::stringutils begins AAAA aaa} msg] $msg
} {0 0}


test compare.82 {string ends (no args)} {
    list [catch {blt::stringutils ends} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ends str pattern ?switches?"}}

test compare.83 {string ends (one arg)} {
    list [catch {blt::stringutils ends a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ends str pattern ?switches?"}}

test compare.84 {string ends (bad switch)} {
    list [catch {blt::stringutils ends a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trim left|right|both|none}} 

test compare.85 {string ends (bad switch)} {
    list [catch {blt::stringutils ends a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trim left|right|both|none}} 

test compare.86 {string ends -trim badValue} {
    list [catch {blt::stringutils ends a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.87 {string ends a b} {
    list [catch {blt::stringutils ends a b} msg] $msg
} {0 0}

test compare.88 {string ends a ab} {
    list [catch {blt::stringutils ends a ab} msg] $msg
} {0 0}

test compare.89 {string ends ab a} {
    list [catch {blt::stringutils ends ab b} msg] $msg
} {0 1}

test compare.90 {string ends a a} {
    list [catch {blt::stringutils ends a a} msg] $msg
} {0 1}

test compare.91 {string ends -trim left} {
    list [catch {blt::stringutils ends "  a" a -trim left} msg] $msg
} {0 1}

test compare.92 {string ends -trim right} {
    list [catch {blt::stringutils ends "a  " a -trim right} msg] $msg
} {0 1}

test compare.93 {string ends -trim both} {
    list [catch {blt::stringutils ends " a " a -trim both} msg] $msg
} {0 1}

test compare.94 {string ends -trim none} {
    list [catch {blt::stringutils ends "a" a -trim none} msg] $msg
} {0 1}

test compare.95 {string ends -trim left} {
    list [catch {blt::stringutils ends "  a" b -trim left} msg] $msg
} {0 0}

test compare.96 {string ends -trim right} {
    list [catch {blt::stringutils ends "a  " b -trim right} msg] $msg
} {0 0}

test compare.97 {string ends -trim both} {
    list [catch {blt::stringutils ends " a " b -trim both} msg] $msg
} {0 0}

test compare.98 {string ends -trim none} {
    list [catch {blt::stringutils ends "a" b -trim none} msg] $msg
} {0 0}


test compare.99 {string ends a A -nocase} {
    list [catch {blt::stringutils ends a A -nocase} msg] $msg
} {0 1}

test compare.100 {string ends A a} {
    list [catch {blt::stringutils ends A a -nocase} msg] $msg
} {0 1}

test compare.101 {string ends AB ab} {
    list [catch {blt::stringutils ends AB ab -nocase} msg] $msg
} {0 1}

test compare.102 {string ends AAAA aaa} {
    list [catch {blt::stringutils ends AAAA aaa} msg] $msg
} {0 0}


test compare.103 {string contains (no args)} {
    list [catch {blt::stringutils contains} msg] $msg
} {1 {wrong # args: should be "blt::stringutils contains str pattern ?switches?"}}

test compare.104 {string contains (one arg)} {
    list [catch {blt::stringutils contains a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils contains str pattern ?switches?"}}

test compare.105 {string contains (bad switch)} {
    list [catch {blt::stringutils contains a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trim left|right|both|none}} 

test compare.106 {string contains (bad switch)} {
    list [catch {blt::stringutils contains a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trim left|right|both|none}} 

test compare.107 {string contains -trim badValue} {
    list [catch {blt::stringutils contains a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.108 {string contains a b} {
    list [catch {blt::stringutils contains a b} msg] $msg
} {0 0}

test compare.109 {string contains a ab} {
    list [catch {blt::stringutils contains a ab} msg] $msg
} {0 0}

test compare.110 {string contains ab a} {
    list [catch {blt::stringutils contains ab a} msg] $msg
} {0 1}

test compare.111 {string contains a a} {
    list [catch {blt::stringutils contains a a} msg] $msg
} {0 1}

test compare.112 {string contains -trim left} {
    list [catch {blt::stringutils contains "  a" a -trim left} msg] $msg
} {0 1}

test compare.113 {string contains -trim right} {
    list [catch {blt::stringutils contains "a  " a -trim right} msg] $msg
} {0 1}

test compare.114 {string contains -trim both} {
    list [catch {blt::stringutils contains " a " a -trim both} msg] $msg
} {0 1}

test compare.115 {string contains -trim none} {
    list [catch {blt::stringutils contains "a" a -trim none} msg] $msg
} {0 1}

test compare.116 {string contains -trim left} {
    list [catch {blt::stringutils contains "  a" b -trim left} msg] $msg
} {0 0}

test compare.117 {string contains -trim right} {
    list [catch {blt::stringutils contains "a  " b -trim right} msg] $msg
} {0 0}

test compare.118 {string contains -trim both} {
    list [catch {blt::stringutils contains " a " b -trim both} msg] $msg
} {0 0}

test compare.119 {string contains -trim none} {
    list [catch {blt::stringutils contains "a" b -trim none} msg] $msg
} {0 0}


test compare.120 {string contains a A -nocase} {
    list [catch {blt::stringutils contains a A -nocase} msg] $msg
} {0 1}

test compare.121 {string contains A a} {
    list [catch {blt::stringutils contains A a -nocase} msg] $msg
} {0 1}

test compare.122 {string contains AB ab} {
    list [catch {blt::stringutils contains AB ab -nocase} msg] $msg
} {0 1}

test compare.123 {string contains AAAA aaa} {
    list [catch {blt::stringutils contains AAAA aaa} msg] $msg
} {0 0}

test compare.124 {string contains abcdef d} {
    list [catch {blt::stringutils contains abcdef d} msg] $msg
} {0 1}

test compare.125 {string contains abcdef a} {
    list [catch {blt::stringutils contains abcdef a} msg] $msg
} {0 1}

test compare.126 {string contains abcdef f} {
    list [catch {blt::stringutils contains abcdef f} msg] $msg
} {0 1}

test compare.127 {string contains abcdef f} {
    list [catch {blt::stringutils contains abcdef cde} msg] $msg
} {0 1}


test compare.128 {string dictcompare (no args)} {
    list [catch {blt::stringutils dictcompare} msg] $msg
} {1 {wrong # args: should be "blt::stringutils dictcompare string1 string2"}}

test compare.129 {string dictcompare (one arg)} {
    list [catch {blt::stringutils dictcompare a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils dictcompare string1 string2"}}

test compare.130 {string dictcompare (bad switch)} {
    list [catch {blt::stringutils dictcompare a b -badSwitch} msg] $msg
} {1 {wrong # args: should be "blt::stringutils dictcompare string1 string2"}}

test compare.131 {string dictcompare a b} {
    list [catch {blt::stringutils dictcompare a b} msg] $msg
} {0 -1}

test compare.132 {string dictcompare A a} {
    list [catch {blt::stringutils dictcompare A a} msg] $msg
} {0 -1}

test compare.133 {string dictcompare Abc abc} {
    list [catch {blt::stringutils dictcompare Abc abc} msg] $msg
} {0 -1}

test compare.134 {string dictcompare abc Abc} {
    list [catch {blt::stringutils dictcompare abc Abc} msg] $msg
} {0 1}

test compare.135 {string dictcompare abc10 abc9} {
    list [catch {blt::stringutils dictcompare abc10 abc9} msg] $msg
} {0 1}

test compare.136 {string dictcompare abc2 abc9} {
    list [catch {blt::stringutils dictcompare abc2 abc9} msg] $msg
} {0 -7}

test compare.137 {string dictcompare Abc2 abc9} {
    list [catch {blt::stringutils dictcompare Abc2 abc9} msg] $msg
} {0 -7}

test compare.138 {string dictcompare abc2 Abc9} {
    list [catch {blt::stringutils dictcompare abc2 Abc9} msg] $msg
} {0 -7}


test compare.139 {string isbetween (no args)} {
    list [catch {blt::stringutils isbetween} msg] $msg
} {1 {wrong # args: should be "blt::stringutils isbetween str firstStr lastStr ?switches?"}}

test compare.140 {string isbetween (one arg)} {
    list [catch {blt::stringutils isbetween a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils isbetween str firstStr lastStr ?switches?"}}

test compare.141 {string isbetween (two args)} {
    list [catch {blt::stringutils isbetween a b} msg] $msg
}  {1 {wrong # args: should be "blt::stringutils isbetween str firstStr lastStr ?switches?"}}

test compare.142 {string isbetween (bad switch)} {
    list [catch {blt::stringutils isbetween a b c -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -dictionary 
   -ascii }}

test compare.143 {string isbetween -nocase} {
    list [catch {blt::stringutils isbetween b A C -nocase} msg] $msg
} {0 1}

test compare.144 {string isbetween -nocase} {
    list [catch {blt::stringutils isbetween a B C -nocase} msg] $msg
} {0 0}

test compare.145 {string isbetween -dictionary} {
    list [catch {blt::stringutils isbetween abc10 abc2 abc20 -dictionary} msg] $msg
} {0 1}

test compare.146 {string isbetween -dictionary} {
    list [catch {blt::stringutils isbetween c a z -dictionary} msg] $msg
} {0 1}

test compare.147 {string isbetween -dictionary} {
    list [catch {blt::stringutils isbetween c a z -ascii} msg] $msg
} {0 1}

test compare.148 {string isbetween -dictionary} {
    list [catch {blt::stringutils isbetween abc5 abc0 abc9 -dictionary} msg] $msg
} {0 1}

test compare.149 {string isbetween -dictionary} {
    list [catch {blt::stringutils isbetween abc5 abc0 abc9} msg] $msg
} {0 1}


test compare.139 {string ismember (no args)} {
    list [catch {blt::stringutils ismember} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ismember str list ?switches?"}}

test compare.140 {string ismember (one arg)} {
    list [catch {blt::stringutils ismember a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ismember str list ?switches?"}}

test compare.141 {string ismember (two args)} {
    list [catch {blt::stringutils ismember a b} msg] $msg
}  {0 0}

test compare.142 {string ismember (bad switch)} {
    list [catch {blt::stringutils ismember a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -sorted decreasing|increasing
   -dictionary 
   -ascii 
   -trim left|right|both|none}}

test compare.143 {string ismember -nocase} {
    list [catch {blt::stringutils ismember b {A B C} -nocase} msg] $msg
} {0 1}

test compare.144 {string ismember -nocase} {
    list [catch {blt::stringutils ismember a { B C } -nocase} msg] $msg
} {0 0}

test compare.145 {string ismember -dictionary} {
    list [catch {blt::stringutils ismember abc10 { abc2 Abc10 } -dictionary} msg] $msg
} {0 1}

test compare.146 {string ismember -dictionary} {
    list [catch {blt::stringutils ismember c a z -dictionary} msg] $msg
} {0 1}

test compare.147 {string ismember -dictionary} {
    list [catch {blt::stringutils ismember c a z -ascii} msg] $msg
} {0 1}
 
test compare.148 {string ismember -dictionary} {
    list [catch {blt::stringutils ismember abc5 abc0 abc9 -dictionary} msg] $msg
} {0 1}

test compare.149 {string ismember -dictionary} {
    list [catch {blt::stringutils ismember abc5 abc0 abc9} msg] $msg
} {0 1}



# contains test?
# begins test?
# distcompare test?
# ends test?
# equals test?
# isbetween test?
# ismember test?

