
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

test compare.22 {number ==} {
    list [catch {blt::numberutils ==} msg] $msg
} {1 {wrong # args: should be "blt::numberutils == value1 value2"}}

test compare.23 {number == 1.00000000000000000001 1 } {
    list [catch {blt::numberutils == 1.00000000000000000001 1} msg] $msg
} {0 1}

test compare.24 {number == 1.0000001 1 } {
    list [catch {blt::numberutils == 1.0000001 1} msg] $msg
} {0 0}

test compare.25 {number == -0 +0 } {
    list [catch {blt::numberutils == -0 +0} msg] $msg
} {0 1}

test compare.26 {number == badArg} {
    list [catch {blt::numberutils == badArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils == value1 value2"}}

test compare.27 {number == badArg badArg} {
    list [catch {blt::numberutils == badArg badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.28 {number == 1 1 -badSwitch} {
    list [catch {blt::numberutils == 1 1 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils == value1 value2"}}

test compare.29 {number >=} {
    list [catch {blt::numberutils >=} msg] $msg
} {1 {wrong # args: should be "blt::numberutils >= value1 value2"}}

test compare.30 {number >= badArg} {
    list [catch {blt::numberutils >= badArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils >= value1 value2"}}

test compare.31 {number >= badArg badArg} {
    list [catch {blt::numberutils >= badArg badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.32 {number >= 1 1 -badSwitch} {
    list [catch {blt::numberutils >= 1 1 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils >= value1 value2"}}

test compare.33 {number >= 1.00000000000000000001 1 } {
    list [catch {blt::numberutils >= 1.00000000000000000001 1} msg] $msg
} {0 1}

test compare.34 {number >= 1.0000001 1 } {
    list [catch {blt::numberutils >= 1.0000001 1} msg] $msg
} {0 1}

test compare.35 {number >== -0 +0 } {
    list [catch {blt::numberutils >= -0 +0} msg] $msg
} {0 1}

test compare.36 {number >= 0.0000001 1 } {
    list [catch {blt::numberutils >= 0.0000001 1} msg] $msg
} {0 0}

test compare.29 {number ge} {
    list [catch {blt::numberutils ge} msg] $msg
} {1 {wrong # args: should be "blt::numberutils ge value1 value2"}}

test compare.30 {number ge badArg} {
    list [catch {blt::numberutils ge badArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils ge value1 value2"}}

test compare.31 {number ge badArg badArg} {
    list [catch {blt::numberutils ge badArg badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.32 {number ge 1 1 -badSwitch} {
    list [catch {blt::numberutils ge 1 1 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils ge value1 value2"}}

test compare.33 {number ge 1.00000000000000000001 1 } {
    list [catch {blt::numberutils ge 1.00000000000000000001 1} msg] $msg
} {0 1}

test compare.34 {number ge 1.0000001 1 } {
    list [catch {blt::numberutils ge 1.0000001 1} msg] $msg
} {0 1}

test compare.35 {number ge -0 +0 } {
    list [catch {blt::numberutils ge -0 +0} msg] $msg
} {0 1}

test compare.36 {number ge 0.0000001 1 } {
    list [catch {blt::numberutils ge 0.0000001 1} msg] $msg
} {0 0}


test compare.37 {number ismember} {
    list [catch {blt::numberutils ismember} msg] $msg
} {1 {wrong # args: should be "blt::numberutils ismember value numberList ?switches?"}}

test compare.38 {number ismember arg} {
    list [catch {blt::numberutils ismember arg } msg] $msg
} {1 {wrong # args: should be "blt::numberutils ismember value numberList ?switches?"}}

test compare.39 {number ismember 1.0 ""} {
    list [catch {blt::numberutils ismember 1.0 "" } msg] $msg
} {0 0}

test compare.40 {number ismember 1.0 "1 2 3"} {
    list [catch {blt::numberutils ismember 1.0 "1 2 3" } msg] $msg
} {0 1}

test compare.41 {number ismember 1.0 "1e0 2 3"} {
    list [catch {blt::numberutils ismember 1.0 "1e0 2 3" } msg] $msg
} {0 1}

test compare.42 {number ismember 1.0 "3 2 1"} {
    list [catch {blt::numberutils ismember 1.0 "3 2 1" } msg] $msg
} {0 1}

test compare.43 {number ismember 1.0 "3 1 2 1"} {
    list [catch {blt::numberutils ismember 1.0 "3 1 2 1" } msg] $msg
} {0 1}

test compare.44 {number ismember 100 "3 1 2 1e2"} {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" } msg] $msg
} {0 1}

test compare.45 {number ismember 1.0 "1 2 3" -sorted increasing} {
    list [catch {blt::numberutils ismember 1.0 "1 2 3" -sorted increasing} msg] $msg
} {0 1}

test compare.46 {number ismember 1.0 "1 2 3" -sorted decreasing} {
    list [catch {blt::numberutils ismember 1.0 "1 2 3" -sorted decreasing} msg] $msg
} {0 0}

test compare.47 {number ismember 1.0 "3 2 1" -sorted decreasing} {
    list [catch {blt::numberutils ismember 1.0 "3 2 1" -sorted decreasing} msg] $msg
} {0 1}

test compare.48 {number ismember 1.0 "1e0 2 3"} {
    list [catch {blt::numberutils ismember 1.0 "1e0 2 3" -sorted increasing} msg] $msg
} {0 1}

test compare.49 {number ismember 1.0 "3 2 1" -sorted increasing} {
    list [catch {blt::numberutils ismember 1.0 "3 2 1" -sorted increasing} msg] $msg
} {0 0}

test compare.50 {number ismember 1.0 "3 1 2 1" -sorted increasing} {
    list [catch {blt::numberutils ismember 1.0 "3 1 2 1" -sorted increasing} msg] $msg
} {0 1}

test compare.51 {number ismember 100 "3 1 2 1e2" -sorted increasing} {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" -sorted increasing} msg] $msg
} {0 1}

test compare.52 {number ismember 100 "3 1 2 1e2" -sorted badValue} {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" -sorted badValue} msg] $msg
} {1 {bad sorted value "badValue": should be decreasing, increasing, or none}}

test compare.53 {number ismember 100 "3 1 2 1e2" -badSwitch} {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -sorted sortDirection}}

test compare.54 {string equals (no args)} {
    list [catch {blt::stringutils equals} msg] $msg
} {1 {wrong # args: should be "blt::stringutils equals str pattern ?switches?"}}

test compare.55 {string equals (one arg)} {
    list [catch {blt::stringutils equals a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils equals str pattern ?switches?"}}

test compare.56 {string equals (bad switch)} {
    list [catch {blt::stringutils equals a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.57 {string equals (bad switch)} {
    list [catch {blt::stringutils equals a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.58 {string equals -trim badValue} {
    list [catch {blt::stringutils equals a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.59 {string equals a b} {
    list [catch {blt::stringutils equals a b} msg] $msg
} {0 0}

test compare.60 {string equals a ab} {
    list [catch {blt::stringutils equals a ab} msg] $msg
} {0 0}

test compare.61 {string equals ab a} {
    list [catch {blt::stringutils equals ab a} msg] $msg
} {0 0}

test compare.62 {string equals a a} {
    list [catch {blt::stringutils equals a a} msg] $msg
} {0 1}

test compare.63 {string equals -trim left} {
    list [catch {blt::stringutils equals "  a" a -trim left} msg] $msg
} {0 1}

test compare.64 {string equals -trim right} {
    list [catch {blt::stringutils equals "a  " a -trim right} msg] $msg
} {0 1}

test compare.65 {string equals -trim both} {
    list [catch {blt::stringutils equals " a " a -trim both} msg] $msg
} {0 1}

test compare.66 {string equals -trim none} {
    list [catch {blt::stringutils equals "a" a -trim none} msg] $msg
} {0 1}

test compare.67 {string equals -trim left} {
    list [catch {blt::stringutils equals "  a" b -trim left} msg] $msg
} {0 0}

test compare.68 {string equals -trim right} {
    list [catch {blt::stringutils equals "a  " b -trim right} msg] $msg
} {0 0}

test compare.69 {string equals -trim both} {
    list [catch {blt::stringutils equals " a " b -trim both} msg] $msg
} {0 0}

test compare.70 {string equals -trim none} {
    list [catch {blt::stringutils equals "a" b -trim none} msg] $msg
} {0 0}


test compare.71 {string equals a A -nocase} {
    list [catch {blt::stringutils equals a A -nocase} msg] $msg
} {0 1}

test compare.72 {string equals A a} {
    list [catch {blt::stringutils equals A a -nocase} msg] $msg
} {0 1}

test compare.73 {string equals AB ab} {
    list [catch {blt::stringutils equals AB ab -nocase} msg] $msg
} {0 1}

test compare.74 {string equals AAAA aaa} {
    list [catch {blt::stringutils equals AAAA aaa} msg] $msg
} {0 0}

test compare.75 {string isbetween d a z} {
    list [catch {blt::stringutils isbetween d a z} msg] $msg
} {0 1}

test compare.76 {string begins (no args)} {
    list [catch {blt::stringutils begins} msg] $msg
} {1 {wrong # args: should be "blt::stringutils begins str pattern ?switches?"}}

test compare.77 {string begins (one arg)} {
    list [catch {blt::stringutils begins a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils begins str pattern ?switches?"}}

test compare.78 {string begins (bad switch)} {
    list [catch {blt::stringutils begins a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.79 {string begins (bad switch)} {
    list [catch {blt::stringutils begins a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.80 {string begins -trim badValue} {
    list [catch {blt::stringutils begins a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.81 {string begins a b} {
    list [catch {blt::stringutils begins a b} msg] $msg
} {0 0}

test compare.82 {string begins a ab} {
    list [catch {blt::stringutils begins a ab} msg] $msg
} {0 0}

test compare.83 {string begins ab a} {
    list [catch {blt::stringutils begins ab a} msg] $msg
} {0 1}

test compare.84 {string begins a a} {
    list [catch {blt::stringutils begins a a} msg] $msg
} {0 1}

test compare.85 {string begins -trim left} {
    list [catch {blt::stringutils begins "  a" a -trim left} msg] $msg
} {0 1}

test compare.86 {string begins -trim right} {
    list [catch {blt::stringutils begins "a  " a -trim right} msg] $msg
} {0 1}

test compare.87 {string begins -trim both} {
    list [catch {blt::stringutils begins " a " a -trim both} msg] $msg
} {0 1}

test compare.88 {string begins -trim none} {
    list [catch {blt::stringutils begins "a" a -trim none} msg] $msg
} {0 1}

test compare.89 {string begins -trim left} {
    list [catch {blt::stringutils begins "  a" b -trim left} msg] $msg
} {0 0}

test compare.90 {string begins -trim right} {
    list [catch {blt::stringutils begins "a  " b -trim right} msg] $msg
} {0 0}

test compare.91 {string begins -trim both} {
    list [catch {blt::stringutils begins " a " b -trim both} msg] $msg
} {0 0}

test compare.92 {string begins -trim none} {
    list [catch {blt::stringutils begins "a" b -trim none} msg] $msg
} {0 0}


test compare.93 {string begins a A -nocase} {
    list [catch {blt::stringutils begins a A -nocase} msg] $msg
} {0 1}

test compare.94 {string begins A a} {
    list [catch {blt::stringutils begins A a -nocase} msg] $msg
} {0 1}

test compare.95 {string begins AB ab} {
    list [catch {blt::stringutils begins AB ab -nocase} msg] $msg
} {0 1}

test compare.96 {string begins AAAA aaa} {
    list [catch {blt::stringutils begins AAAA aaa} msg] $msg
} {0 0}


test compare.97 {string ends (no args)} {
    list [catch {blt::stringutils ends} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ends str pattern ?switches?"}}

test compare.98 {string ends (one arg)} {
    list [catch {blt::stringutils ends a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ends str pattern ?switches?"}}

test compare.99 {string ends (bad switch)} {
    list [catch {blt::stringutils ends a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.100 {string ends (bad switch)} {
    list [catch {blt::stringutils ends a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.101 {string ends -trim badValue} {
    list [catch {blt::stringutils ends a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.102 {string ends a b} {
    list [catch {blt::stringutils ends a b} msg] $msg
} {0 0}

test compare.103 {string ends a ab} {
    list [catch {blt::stringutils ends a ab} msg] $msg
} {0 0}

test compare.104 {string ends ab a} {
    list [catch {blt::stringutils ends ab b} msg] $msg
} {0 1}

test compare.105 {string ends a a} {
    list [catch {blt::stringutils ends a a} msg] $msg
} {0 1}

test compare.106 {string ends -trim left} {
    list [catch {blt::stringutils ends "  a" a -trim left} msg] $msg
} {0 1}

test compare.107 {string ends -trim right} {
    list [catch {blt::stringutils ends "a  " a -trim right} msg] $msg
} {0 1}

test compare.108 {string ends -trim both} {
    list [catch {blt::stringutils ends " a " a -trim both} msg] $msg
} {0 1}

test compare.109 {string ends -trim none} {
    list [catch {blt::stringutils ends "a" a -trim none} msg] $msg
} {0 1}

test compare.110 {string ends -trim left} {
    list [catch {blt::stringutils ends "  a" b -trim left} msg] $msg
} {0 0}

test compare.111 {string ends -trim right} {
    list [catch {blt::stringutils ends "a  " b -trim right} msg] $msg
} {0 0}

test compare.112 {string ends -trim both} {
    list [catch {blt::stringutils ends " a " b -trim both} msg] $msg
} {0 0}

test compare.113 {string ends -trim none} {
    list [catch {blt::stringutils ends "a" b -trim none} msg] $msg
} {0 0}


test compare.114 {string ends a A -nocase} {
    list [catch {blt::stringutils ends a A -nocase} msg] $msg
} {0 1}

test compare.115 {string ends A a} {
    list [catch {blt::stringutils ends A a -nocase} msg] $msg
} {0 1}

test compare.116 {string ends AB ab} {
    list [catch {blt::stringutils ends AB ab -nocase} msg] $msg
} {0 1}

test compare.117 {string ends AAAA aaa} {
    list [catch {blt::stringutils ends AAAA aaa} msg] $msg
} {0 0}


test compare.118 {string contains (no args)} {
    list [catch {blt::stringutils contains} msg] $msg
} {1 {wrong # args: should be "blt::stringutils contains str pattern ?switches?"}}

test compare.119 {string contains (one arg)} {
    list [catch {blt::stringutils contains a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils contains str pattern ?switches?"}}

test compare.120 {string contains (bad switch)} {
    list [catch {blt::stringutils contains a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.121 {string contains (bad switch)} {
    list [catch {blt::stringutils contains a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.122 {string contains -trim badValue} {
    list [catch {blt::stringutils contains a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.123 {string contains a b} {
    list [catch {blt::stringutils contains a b} msg] $msg
} {0 0}

test compare.124 {string contains a ab} {
    list [catch {blt::stringutils contains a ab} msg] $msg
} {0 0}

test compare.125 {string contains ab a} {
    list [catch {blt::stringutils contains ab a} msg] $msg
} {0 1}

test compare.126 {string contains a a} {
    list [catch {blt::stringutils contains a a} msg] $msg
} {0 1}

test compare.127 {string contains -trim left} {
    list [catch {blt::stringutils contains "  a" a -trim left} msg] $msg
} {0 1}

test compare.128 {string contains -trim right} {
    list [catch {blt::stringutils contains "a  " a -trim right} msg] $msg
} {0 1}

test compare.129 {string contains -trim both} {
    list [catch {blt::stringutils contains " a " a -trim both} msg] $msg
} {0 1}

test compare.130 {string contains -trim none} {
    list [catch {blt::stringutils contains "a" a -trim none} msg] $msg
} {0 1}

test compare.131 {string contains -trim left} {
    list [catch {blt::stringutils contains "  a" b -trim left} msg] $msg
} {0 0}

test compare.132 {string contains -trim right} {
    list [catch {blt::stringutils contains "a  " b -trim right} msg] $msg
} {0 0}

test compare.133 {string contains -trim both} {
    list [catch {blt::stringutils contains " a " b -trim both} msg] $msg
} {0 0}

test compare.134 {string contains -trim none} {
    list [catch {blt::stringutils contains "a" b -trim none} msg] $msg
} {0 0}


test compare.135 {string contains a A -nocase} {
    list [catch {blt::stringutils contains a A -nocase} msg] $msg
} {0 1}

test compare.136 {string contains A a} {
    list [catch {blt::stringutils contains A a -nocase} msg] $msg
} {0 1}

test compare.137 {string contains AB ab} {
    list [catch {blt::stringutils contains AB ab -nocase} msg] $msg
} {0 1}

test compare.138 {string contains AAAA aaa} {
    list [catch {blt::stringutils contains AAAA aaa} msg] $msg
} {0 0}

test compare.139 {string contains abcdef d} {
    list [catch {blt::stringutils contains abcdef d} msg] $msg
} {0 1}

test compare.140 {string contains abcdef a} {
    list [catch {blt::stringutils contains abcdef a} msg] $msg
} {0 1}

test compare.141 {string contains abcdef f} {
    list [catch {blt::stringutils contains abcdef f} msg] $msg
} {0 1}

test compare.142 {string contains abcdef f} {
    list [catch {blt::stringutils contains abcdef cde} msg] $msg
} {0 1}


test compare.143 {string dictcompare (no args)} {
    list [catch {blt::stringutils dictcompare} msg] $msg
} {1 {wrong # args: should be "blt::stringutils dictcompare string1 string2"}}

test compare.144 {string dictcompare (one arg)} {
    list [catch {blt::stringutils dictcompare a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils dictcompare string1 string2"}}

test compare.145 {string dictcompare (bad switch)} {
    list [catch {blt::stringutils dictcompare a b -badSwitch} msg] $msg
} {1 {wrong # args: should be "blt::stringutils dictcompare string1 string2"}}

test compare.146 {string dictcompare a b} {
    list [catch {blt::stringutils dictcompare a b} msg] $msg
} {0 -1}

test compare.147 {string dictcompare A a} {
    list [catch {blt::stringutils dictcompare A a} msg] $msg
} {0 -1}

test compare.148 {string dictcompare Abc abc} {
    list [catch {blt::stringutils dictcompare Abc abc} msg] $msg
} {0 -1}

test compare.149 {string dictcompare abc Abc} {
    list [catch {blt::stringutils dictcompare abc Abc} msg] $msg
} {0 1}

test compare.150 {string dictcompare abc10 abc9} {
    list [catch {blt::stringutils dictcompare abc10 abc9} msg] $msg
} {0 1}

test compare.151 {string dictcompare abc2 abc9} {
    list [catch {blt::stringutils dictcompare abc2 abc9} msg] $msg
} {0 -7}

test compare.152 {string dictcompare Abc2 abc9} {
    list [catch {blt::stringutils dictcompare Abc2 abc9} msg] $msg
} {0 -7}

test compare.153 {string dictcompare abc2 Abc9} {
    list [catch {blt::stringutils dictcompare abc2 Abc9} msg] $msg
} {0 -7}


test compare.154 {string isbetween (no args)} {
    list [catch {blt::stringutils isbetween} msg] $msg
} {1 {wrong # args: should be "blt::stringutils isbetween str firstStr lastStr ?switches?"}}

test compare.155 {string isbetween (one arg)} {
    list [catch {blt::stringutils isbetween a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils isbetween str firstStr lastStr ?switches?"}}

test compare.156 {string isbetween (two args)} {
    list [catch {blt::stringutils isbetween a b} msg] $msg
}  {1 {wrong # args: should be "blt::stringutils isbetween str firstStr lastStr ?switches?"}}

test compare.157 {string isbetween (bad switch)} {
    list [catch {blt::stringutils isbetween a b c -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase }}

test compare.158 {string isbetween -nocase} {
    list [catch {blt::stringutils isbetween b A C -nocase} msg] $msg
} {0 1}

test compare.159 {string isbetween -nocase} {
    list [catch {blt::stringutils isbetween a B C -nocase} msg] $msg
} {0 0}

test compare.160 {string ismember (no args)} {
    list [catch {blt::stringutils ismember} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ismember str list ?switches?"}}

test compare.161 {string ismember (one arg)} {
    list [catch {blt::stringutils ismember a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ismember str list ?switches?"}}

test compare.162 {string ismember emptystring} {
    list [catch {blt::stringutils ismember a {}} msg] $msg
}  {0 0}

test compare.163 {string ismember emptystrings} {
    list [catch {blt::stringutils ismember {} {}} msg] $msg
}  {0 0}

test compare.164 {string ismember first} {
    list [catch {blt::stringutils ismember a { a b c d e f }} msg] $msg
}  {0 1}

test compare.165 {string ismember last} {
    list [catch {blt::stringutils ismember f { a b c d e f }} msg] $msg
}  {0 1}

test compare.166 {string ismember middle} {
    list [catch {blt::stringutils ismember d { a b c d e f }} msg] $msg
}  {0 1}

test compare.167 {string ismember middle -sorted} {
    list [catch {blt::stringutils ismember d { a b c d e f } -sorted increasing} msg] $msg
}  {0 1}

test compare.168 {string ismember middle -sorted} {
    list [catch {blt::stringutils ismember d { a b c d e f } -sorted decreasing} msg] $msg
}  {0 0}

test compare.169 {string ismember middle -sorted} {
    list [catch {blt::stringutils ismember d { f e d c b a } -sorted decreasing} msg] $msg
}  {0 1}

test compare.170 {string ismember middle -sorted badValue} {
    list [catch {blt::stringutils ismember d { f e d c b a } -sorted badValue} msg] $msg
} {1 {bad sorted value "badValue": should be decreasing, increasing, or none}}

test compare.171 {string ismember (bad switch)} {
    list [catch {blt::stringutils ismember a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -sorted sortDirection
   -trimwhitespace trimName}}

test compare.172 {string ismember -nocase} {
    list [catch {blt::stringutils ismember b {A B C} -nocase} msg] $msg
} {0 1}

test compare.173 {string ismember -nocase} {
    list [catch {blt::stringutils ismember a { B C } -nocase} msg] $msg
} {0 0}

test compare.174 {string ismember -trim badValue} {
    list [catch {blt::stringutils ismember a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.175 {string ismember a b} {
    list [catch {blt::stringutils ismember a b} msg] $msg
} {0 0}

test compare.176 {string ismember -trim left} {
    list [catch {blt::stringutils ismember "  a" { c b a } -trim left} msg] $msg
} {0 1}

test compare.177 {string ismember -trim right} {
    list [catch {blt::stringutils ismember "a  " { c b a } -trim right} msg] $msg
} {0 1}

test compare.178 {string ismember -trim both} {
    list [catch {blt::stringutils ismember " a " { c b a } -trim both} msg] $msg
} {0 1}

test compare.179 {string ismember -trim none} {
    list [catch {blt::stringutils ismember "a" { c b a } -trim none} msg] $msg
} {0 1}

test compare.180 {string ismember -trim left} {
    list [catch {blt::stringutils ismember "  a" { c b a } -trim left} msg] $msg
} {0 1}

test compare.181 {string ismember -trim right} {
    list [catch {blt::stringutils ismember "a  " { c b a } -trim right} msg] $msg
} {0 1}

test compare.182 {string ismember -trim both} {
    list [catch {blt::stringutils ismember " a " b -trim both} msg] $msg
} {0 0}

test compare.183 {string ismember -trim none} {
    list [catch {blt::stringutils ismember "a" b -trim none} msg] $msg
} {0 0}


# contains test?
# begins test?
# distcompare test?
# ends test?
# equals test?
# isbetween test?
# ismember test?




