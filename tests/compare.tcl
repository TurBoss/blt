
package require BLT

if {[info procs test] != "test"} {
    source defs
}

if [file exists ../library] {
    set blt_library ../library
}

set VERBOSE 0

test compare.1 {stringutils no arg} {
    list [catch {blt::stringutils} msg] $msg
} {1 {wrong # args: should be one of...
  blt::stringutils begins str pattern ?switches?
  blt::stringutils contains str pattern ?switches?
  blt::stringutils dictcompare string1 string2
  blt::stringutils ends str pattern ?switches?
  blt::stringutils equals str pattern ?switches?
  blt::stringutils isbetween str first last ?switches?
  blt::stringutils ismember str list ?switches?}}

test compare.2 {numberutils no arg} {
    list [catch {blt::numberutils} msg] $msg
} {1 {wrong # args: should be one of...
  blt::numberutils < x y
  blt::numberutils <= x y
  blt::numberutils == x y
  blt::numberutils > x y
  blt::numberutils >= x y
  blt::numberutils eq x y
  blt::numberutils ge x y
  blt::numberutils gt x y
  blt::numberutils isbetween x first last
  blt::numberutils ismember x numberList ?switches?
  blt::numberutils le x y
  blt::numberutils lt x y}}

test compare.3 {numberutils badArg} {
    list [catch {blt::numberutils badArg} msg] $msg
} {1 {bad operation "badArg": should be one of...
  blt::numberutils < x y
  blt::numberutils <= x y
  blt::numberutils == x y
  blt::numberutils > x y
  blt::numberutils >= x y
  blt::numberutils eq x y
  blt::numberutils ge x y
  blt::numberutils gt x y
  blt::numberutils isbetween x first last
  blt::numberutils ismember x numberList ?switches?
  blt::numberutils le x y
  blt::numberutils lt x y}}

test compare.4 { isbetween } {
    list [catch {blt::numberutils isbetween} msg] $msg
} {1 {wrong # args: should be "blt::numberutils isbetween x first last"}}

test compare.5 { isbetween 1 } {
    list [catch {blt::numberutils isbetween 1} msg] $msg
} {1 {wrong # args: should be "blt::numberutils isbetween x first last"}}

test compare.6 { isbetween 1 2 } {
    list [catch {blt::numberutils isbetween 1 2} msg] $msg
} {1 {wrong # args: should be "blt::numberutils isbetween x first last"}}

test compare.7 { isbetween 1 2 3 } {
    list [catch {blt::numberutils isbetween 1 2 3} msg] $msg
} {0 0}

test compare.8 { isbetween 1 0 3 } {
    list [catch {blt::numberutils isbetween 1 0 3} msg] $msg
} {0 1}

test compare.9 { isbetween badArg 0 3 } {
    list [catch {blt::numberutils isbetween badArg 0 3} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.10 { isbetween 1 badArg 3 } {
    list [catch {blt::numberutils isbetween 1 badArg 3} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.11 { isbetween 1 0 badArg } {
    list [catch {blt::numberutils isbetween 1 0 badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.12 { isbetween 1 0 3 extraArg } {
    list [catch {blt::numberutils isbetween 1 0 3 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils isbetween x first last"}}

test compare.13 { isbetween 1 0 3 } {
    list [catch {blt::numberutils isbetween 1 0 3} msg] $msg
} {0 1}

# No range: last < first
test compare.14 { isbetween 1 3 0 } {
    list [catch {blt::numberutils isbetween 1 3 0} msg] $msg
} {0 0}

test compare.15 { isbetween 0.99999999999999 1 2 } {
    list [catch {blt::numberutils isbetween 0.99999999999999 1 2} msg] $msg
} {0 0}

test compare.16 { isbetween 1.00000001 1 0 } {
    list [catch {blt::numberutils isbetween 1.000000001 1 0} msg] $msg
} {0 0}

test compare.17 { eq 1.00000000000000000001 1 } {
    list [catch {blt::numberutils eq 1.00000000000000000001 1} msg] $msg
} {0 1}

test compare.18 { isbetween 3 -Inf Inf } {
    list [catch {blt::numberutils isbetween 3 -Inf Inf} msg] $msg
} {0 1}

test compare.19 { isbetween 3 Inf -Inf } {
    list [catch {blt::numberutils isbetween 3 Inf -Inf} msg] $msg
} {0 0}

test compare.20 { isbetween NaN NaN NaN } {
    list [catch {blt::numberutils isbetween NaN NaN NaN} msg] $msg
} {0 1}

test compare.21 { eq } {
    list [catch {blt::numberutils eq} msg] $msg
} {1 {wrong # args: should be "blt::numberutils eq x y"}}

test compare.22 { eq badArg } {
    list [catch {blt::numberutils eq badArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils eq x y"}}

test compare.23 { eq badArg badArg } {
    list [catch {blt::numberutils eq badArg badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.24 { eq 1 1 -badSwitch } {
    list [catch {blt::numberutils eq 1 1 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils eq x y"}}

test compare.25 { eq 1 1 } {
    list [catch {blt::numberutils eq 1 1} msg] $msg
} {0 1}

test compare.26 { eq 1.0 1 } {
    list [catch {blt::numberutils eq 1.0 1} msg] $msg
} {0 1}

test compare.27 { eq 1e0 1 } {
    list [catch {blt::numberutils eq 1e0 1} msg] $msg
} {0 1}

test compare.28 { eq 1e0 1.000000000000000000001 } {
    list [catch {blt::numberutils eq 1e0 1.000000000000000000001 } msg] $msg
} {0 1}

test compare.29 { eq NaN 1 } {
    list [catch {blt::numberutils eq NaN 1 } msg] $msg
} {0 0}

test compare.30 { eq NaN NaN } {
    list [catch {blt::numberutils eq NaN NaN } msg] $msg
} {0 1}

test compare.31 { eq -Inf -Inf } {
    list [catch {blt::numberutils eq -Inf -Inf } msg] $msg
} {0 1}

test compare.32 { eq -Inf NaN } {
    list [catch {blt::numberutils eq -Inf NaN } msg] $msg
} {0 0}

test compare.33 { eq Inf NaN } {
    list [catch {blt::numberutils eq Inf NaN } msg] $msg
} {0 0}

test compare.34 { eq Inf +Inf } {
    list [catch {blt::numberutils eq Inf +Inf } msg] $msg
} {0 1}

test compare.35 { == } {
    list [catch {blt::numberutils ==} msg] $msg
} {1 {wrong # args: should be "blt::numberutils == x y"}}

test compare.36 { == 1.00000000000000000001 1 } {
    list [catch {blt::numberutils == 1.00000000000000000001 1} msg] $msg
} {0 1}

test compare.37 { == 1.0000001 1 } {
    list [catch {blt::numberutils == 1.0000001 1} msg] $msg
} {0 0}

test compare.38 { == -0 +0 } {
    list [catch {blt::numberutils == -0 +0} msg] $msg
} {0 1}

test compare.39 { == badArg } {
    list [catch {blt::numberutils == badArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils == x y"}}

test compare.40 { == badArg badArg } {
    list [catch {blt::numberutils == badArg badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.41 { == 1 1 -badSwitch } {
    list [catch {blt::numberutils == 1 1 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils == x y"}}

test compare.42 { >= } {
    list [catch {blt::numberutils >=} msg] $msg
} {1 {wrong # args: should be "blt::numberutils >= x y"}}

test compare.43 { >= badArg } {
    list [catch {blt::numberutils >= badArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils >= x y"}}

test compare.44 { >= badArg badArg } {
    list [catch {blt::numberutils >= badArg badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.45 { >= 1 1 -badSwitch } {
    list [catch {blt::numberutils >= 1 1 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils >= x y"}}

test compare.46 { >= 1.00000000000000000001 1 } {
    list [catch {blt::numberutils >= 1.00000000000000000001 1} msg] $msg
} {0 1}

test compare.47 { >= 1.0000001 1 } {
    list [catch {blt::numberutils >= 1.0000001 1} msg] $msg
} {0 1}

test compare.48 { >== -0 +0 } {
    list [catch {blt::numberutils >= -0 +0} msg] $msg
} {0 1}

test compare.49 { >= 0.0000001 1 } {
    list [catch {blt::numberutils >= 0.0000001 1} msg] $msg
} {0 0}

test compare.50 { ge } {
    list [catch {blt::numberutils ge} msg] $msg
} {1 {wrong # args: should be "blt::numberutils ge x y"}}

test compare.51 { ge badArg } {
    list [catch {blt::numberutils ge badArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils ge x y"}}

test compare.52 { ge badArg badArg } {
    list [catch {blt::numberutils ge badArg badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.53 { ge 1 1 -badSwitch } {
    list [catch {blt::numberutils ge 1 1 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils ge x y"}}

test compare.54 { ge 1.00000000000000000001 1 } {
    list [catch {blt::numberutils ge 1.00000000000000000001 1} msg] $msg
} {0 1}

test compare.55 { ge 1.0000001 1 } {
    list [catch {blt::numberutils ge 1.0000001 1} msg] $msg
} {0 1}

test compare.56 { ge -0 +0 } {
    list [catch {blt::numberutils ge -0 +0} msg] $msg
} {0 1}

test compare.57 { ge 0.0000001 1 } {
    list [catch {blt::numberutils ge 0.0000001 1} msg] $msg
} {0 0}

test compare.42 { <= } {
    list [catch {blt::numberutils <=} msg] $msg
} {1 {wrong # args: should be "blt::numberutils <= x y"}}

test compare.43 { <= badArg } {
    list [catch {blt::numberutils <= badArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils <= x y"}}

test compare.44 { <= badArg badArg } {
    list [catch {blt::numberutils <= badArg badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.45 { <= 1 1 -badSwitch } {
    list [catch {blt::numberutils <= 1 1 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils <= x y"}}

test compare.46 { <= 1 1.00000000000000000001 } {
    list [catch {blt::numberutils <= 1 1.00000000000000000001} msg] $msg
} {0 1}

test compare.47 { <= 1 1.0000001 } {
    list [catch {blt::numberutils <= 1 1.0000001} msg] $msg
} {0 1}

test compare.48 { <= -0 +0 } {
    list [catch {blt::numberutils <= -0 +0} msg] $msg
} {0 1}

test compare.49 { <= 1 0.0000001 } {
    list [catch {blt::numberutils <= 1 0.0000001} msg] $msg
} {0 0}

test compare.50 { le } {
    list [catch {blt::numberutils le} msg] $msg
} {1 {wrong # args: should be "blt::numberutils le x y"}}

test compare.51 { le badArg } {
    list [catch {blt::numberutils le badArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils le x y"}}

test compare.52 { le badArg badArg } {
    list [catch {blt::numberutils le badArg badArg} msg] $msg
} {1 {expected floating-point number but got "badArg"}}

test compare.53 { le 1 1 -badSwitch } {
    list [catch {blt::numberutils le 1 1 extraArg} msg] $msg
} {1 {wrong # args: should be "blt::numberutils le x y"}}

test compare.54 { le 1 1.00000000000000000001 } {
    list [catch {blt::numberutils le 1 1.00000000000000000001 } msg] $msg
} {0 1}

test compare.55 { le 1 1.0000001 } {
    list [catch {blt::numberutils le 1 1.0000001} msg] $msg
} {0 1}

test compare.56 { le -0 +0 } {
    list [catch {blt::numberutils le -0 +0} msg] $msg
} {0 1}

test compare.57 { le 0.0000001 1 } {
    list [catch {blt::numberutils le 1 0.0000001} msg] $msg
} {0 0}

test compare.58 { ismember } {
    list [catch {blt::numberutils ismember} msg] $msg
} {1 {wrong # args: should be "blt::numberutils ismember x numberList ?switches?"}}

test compare.59 { ismember arg } {
    list [catch {blt::numberutils ismember arg } msg] $msg
} {1 {wrong # args: should be "blt::numberutils ismember x numberList ?switches?"}}

test compare.60 { ismember 1.0 "" } {
    list [catch {blt::numberutils ismember 1.0 "" } msg] $msg
} {0 0}

test compare.61 { ismember 1.0 "1 2 3" } {
    list [catch {blt::numberutils ismember 1.0 "1 2 3" } msg] $msg
} {0 1}

test compare.62 { ismember 1.0 "1e0 2 3" } {
    list [catch {blt::numberutils ismember 1.0 "1e0 2 3" } msg] $msg
} {0 1}

test compare.63 { ismember 1.0 "3 2 1" } {
    list [catch {blt::numberutils ismember 1.0 "3 2 1" } msg] $msg
} {0 1}

test compare.64 { ismember 1.0 "3 1 2 1" } {
    list [catch {blt::numberutils ismember 1.0 "3 1 2 1" } msg] $msg
} {0 1}

test compare.65 { ismember 100 "3 1 2 1e2" } {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" } msg] $msg
} {0 1}

test compare.66 { ismember 1.0 "1 2 3" -sorted increasing } {
    list [catch {blt::numberutils ismember 1.0 "1 2 3" -sorted increasing} msg] $msg
} {0 1}

test compare.67 { ismember 1.0 "1 2 3" -sorted decreasing } {
    list [catch {blt::numberutils ismember 1.0 "1 2 3" -sorted decreasing} msg] $msg
} {0 0}

test compare.68 { ismember 1.0 "3 2 1" -sorted decreasing } {
    list [catch {blt::numberutils ismember 1.0 "3 2 1" -sorted decreasing} msg] $msg
} {0 1}

test compare.69 { ismember 1.0 "1e0 2 3" } {
    list [catch {blt::numberutils ismember 1.0 "1e0 2 3" -sorted increasing} msg] $msg
} {0 1}

test compare.70 { ismember 1.0 "3 2 1" -sorted increasing } {
    list [catch {blt::numberutils ismember 1.0 "3 2 1" -sorted increasing} msg] $msg
} {0 0}

test compare.71 { ismember 1.0 "3 1 2 1" -sorted increasing } {
    list [catch {blt::numberutils ismember 1.0 "3 1 2 1" -sorted increasing} msg] $msg
} {0 1}

test compare.72 { ismember 100 "3 1 2 1e2" -sorted increasing } {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" -sorted increasing} msg] $msg
} {0 1}

test compare.73 { ismember 100 "3 1 2 1e2" -sorted badValue } {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" -sorted badValue} msg] $msg
} {1 {bad sorted value "badValue": should be decreasing, increasing, or none}}

test compare.74 { ismember 100 "3 1 2 1e2" -badSwitch } {
    list [catch {blt::numberutils ismember 100 "3 2 1e2" -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -sorted sortDirection}}

test compare.75 { equals (no args) } {
    list [catch {blt::stringutils equals} msg] $msg
} {1 {wrong # args: should be "blt::stringutils equals str pattern ?switches?"}}

test compare.76 { equals (one arg) } {
    list [catch {blt::stringutils equals a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils equals str pattern ?switches?"}}

test compare.77 { equals (bad switch) } {
    list [catch {blt::stringutils equals a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.78 { equals (bad switch) } {
    list [catch {blt::stringutils equals a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.79 { equals -trim badValue } {
    list [catch {blt::stringutils equals a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.80 { equals a b } {
    list [catch {blt::stringutils equals a b} msg] $msg
} {0 0}

test compare.81 { equals a ab } {
    list [catch {blt::stringutils equals a ab} msg] $msg
} {0 0}

test compare.82 { equals ab a } {
    list [catch {blt::stringutils equals ab a} msg] $msg
} {0 0}

test compare.83 { equals a a } {
    list [catch {blt::stringutils equals a a} msg] $msg
} {0 1}

test compare.84 { equals -trim left } {
    list [catch {blt::stringutils equals "  a" a -trim left} msg] $msg
} {0 1}

test compare.85 { equals -trim right } {
    list [catch {blt::stringutils equals "a  " a -trim right} msg] $msg
} {0 1}

test compare.86 { equals -trim both } {
    list [catch {blt::stringutils equals " a " a -trim both} msg] $msg
} {0 1}

test compare.87 { equals -trim none } {
    list [catch {blt::stringutils equals "a" a -trim none} msg] $msg
} {0 1}

test compare.88 { equals -trim left } {
    list [catch {blt::stringutils equals "  a" b -trim left} msg] $msg
} {0 0}

test compare.89 { equals -trim right } {
    list [catch {blt::stringutils equals "a  " b -trim right} msg] $msg
} {0 0}

test compare.90 { equals -trim both } {
    list [catch {blt::stringutils equals " a " b -trim both} msg] $msg
} {0 0}

test compare.91 { equals -trim none } {
    list [catch {blt::stringutils equals "a" b -trim none} msg] $msg
} {0 0}


test compare.92 { equals a A -nocase } {
    list [catch {blt::stringutils equals a A -nocase} msg] $msg
} {0 1}

test compare.93 { equals A a } {
    list [catch {blt::stringutils equals A a -nocase} msg] $msg
} {0 1}

test compare.94 { equals AB ab } {
    list [catch {blt::stringutils equals AB ab -nocase} msg] $msg
} {0 1}

test compare.95 { equals AAAA aaa } {
    list [catch {blt::stringutils equals AAAA aaa} msg] $msg
} {0 0}

test compare.96 { begins (no args) } {
    list [catch {blt::stringutils begins} msg] $msg
} {1 {wrong # args: should be "blt::stringutils begins str pattern ?switches?"}}

test compare.97 { begins (one arg) } {
    list [catch {blt::stringutils begins a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils begins str pattern ?switches?"}}

test compare.98 { begins (bad switch) } {
    list [catch {blt::stringutils begins a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.99 { begins (bad switch) } {
    list [catch {blt::stringutils begins a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.100 { begins -trim badValue } {
    list [catch {blt::stringutils begins a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.101 { begins a b } {
    list [catch {blt::stringutils begins a b} msg] $msg
} {0 0}

test compare.102 { begins a ab } {
    list [catch {blt::stringutils begins a ab} msg] $msg
} {0 0}

test compare.103 { begins ab a } {
    list [catch {blt::stringutils begins ab a} msg] $msg
} {0 1}

test compare.104 { begins a a } {
    list [catch {blt::stringutils begins a a} msg] $msg
} {0 1}

test compare.105 { begins -trim left } {
    list [catch {blt::stringutils begins "  a" a -trim left} msg] $msg
} {0 1}

test compare.106 { begins -trim right } {
    list [catch {blt::stringutils begins "a  " a -trim right} msg] $msg
} {0 1}

test compare.107 { begins -trim both } {
    list [catch {blt::stringutils begins " a " a -trim both} msg] $msg
} {0 1}

test compare.108 { begins -trim none } {
    list [catch {blt::stringutils begins "a" a -trim none} msg] $msg
} {0 1}

test compare.109 { begins -trim left } {
    list [catch {blt::stringutils begins "  a" b -trim left} msg] $msg
} {0 0}

test compare.110 { begins -trim right } {
    list [catch {blt::stringutils begins "a  " b -trim right} msg] $msg
} {0 0}

test compare.111 { begins -trim both } {
    list [catch {blt::stringutils begins " a " b -trim both} msg] $msg
} {0 0}

test compare.112 { begins -trim none } {
    list [catch {blt::stringutils begins "a" b -trim none} msg] $msg
} {0 0}


test compare.113 { begins a A -nocase } {
    list [catch {blt::stringutils begins a A -nocase} msg] $msg
} {0 1}

test compare.114 { begins A a } {
    list [catch {blt::stringutils begins A a -nocase} msg] $msg
} {0 1}

test compare.115 { begins AB ab } {
    list [catch {blt::stringutils begins AB ab -nocase} msg] $msg
} {0 1}

test compare.116 { begins AAAA aaa } {
    list [catch {blt::stringutils begins AAAA aaa} msg] $msg
} {0 0}


test compare.117 { ends (no args) } {
    list [catch {blt::stringutils ends} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ends str pattern ?switches?"}}

test compare.118 { ends (one arg) } {
    list [catch {blt::stringutils ends a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ends str pattern ?switches?"}}

test compare.119 { ends (bad switch) } {
    list [catch {blt::stringutils ends a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.120 { ends (bad switch) } {
    list [catch {blt::stringutils ends a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.121 { ends -trim badValue } {
    list [catch {blt::stringutils ends a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.122 { ends a b } {
    list [catch {blt::stringutils ends a b} msg] $msg
} {0 0}

test compare.123 { ends a ab } {
    list [catch {blt::stringutils ends a ab} msg] $msg
} {0 0}

test compare.124 { ends ab a } {
    list [catch {blt::stringutils ends ab b} msg] $msg
} {0 1}

test compare.125 { ends a a } {
    list [catch {blt::stringutils ends a a} msg] $msg
} {0 1}

test compare.126 { ends -trim left } {
    list [catch {blt::stringutils ends "  a" a -trim left} msg] $msg
} {0 1}

test compare.127 { ends -trim right } {
    list [catch {blt::stringutils ends "a  " a -trim right} msg] $msg
} {0 1}

test compare.128 { ends -trim both } {
    list [catch {blt::stringutils ends " a " a -trim both} msg] $msg
} {0 1}

test compare.129 { ends -trim none } {
    list [catch {blt::stringutils ends "a" a -trim none} msg] $msg
} {0 1}

test compare.130 { ends -trim left } {
    list [catch {blt::stringutils ends "  a" b -trim left} msg] $msg
} {0 0}

test compare.131 { ends -trim right } {
    list [catch {blt::stringutils ends "a  " b -trim right} msg] $msg
} {0 0}

test compare.132 { ends -trim both } {
    list [catch {blt::stringutils ends " a " b -trim both} msg] $msg
} {0 0}

test compare.133 { ends -trim none } {
    list [catch {blt::stringutils ends "a" b -trim none} msg] $msg
} {0 0}


test compare.134 { ends a A -nocase } {
    list [catch {blt::stringutils ends a A -nocase} msg] $msg
} {0 1}

test compare.135 { ends A a } {
    list [catch {blt::stringutils ends A a -nocase} msg] $msg
} {0 1}

test compare.136 { ends AB ab } {
    list [catch {blt::stringutils ends AB ab -nocase} msg] $msg
} {0 1}

test compare.137 { ends AAAA aaa } {
    list [catch {blt::stringutils ends AAAA aaa} msg] $msg
} {0 0}


test compare.138 { contains (no args) } {
    list [catch {blt::stringutils contains} msg] $msg
} {1 {wrong # args: should be "blt::stringutils contains str pattern ?switches?"}}

test compare.139 { contains (one arg) } {
    list [catch {blt::stringutils contains a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils contains str pattern ?switches?"}}

test compare.140 { contains (bad switch) } {
    list [catch {blt::stringutils contains a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.141 { contains (bad switch) } {
    list [catch {blt::stringutils contains a b -nocase -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -trimwhitespace trimName}}

test compare.142 { contains -trim badValue } {
    list [catch {blt::stringutils contains a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.143 { contains a b } {
    list [catch {blt::stringutils contains a b} msg] $msg
} {0 0}

test compare.144 { contains a ab } {
    list [catch {blt::stringutils contains a ab} msg] $msg
} {0 0}

test compare.145 { contains ab a } {
    list [catch {blt::stringutils contains ab a} msg] $msg
} {0 1}

test compare.146 { contains a a } {
    list [catch {blt::stringutils contains a a} msg] $msg
} {0 1}

test compare.147 { contains -trim left } {
    list [catch {blt::stringutils contains "  a" a -trim left} msg] $msg
} {0 1}

test compare.148 { contains -trim right } {
    list [catch {blt::stringutils contains "a  " a -trim right} msg] $msg
} {0 1}

test compare.149 { contains -trim both } {
    list [catch {blt::stringutils contains " a " a -trim both} msg] $msg
} {0 1}

test compare.150 { contains -trim none } {
    list [catch {blt::stringutils contains "a" a -trim none} msg] $msg
} {0 1}

test compare.151 { contains -trim left } {
    list [catch {blt::stringutils contains "  a" b -trim left} msg] $msg
} {0 0}

test compare.152 { contains -trim right } {
    list [catch {blt::stringutils contains "a  " b -trim right} msg] $msg
} {0 0}

test compare.153 { contains -trim both } {
    list [catch {blt::stringutils contains " a " b -trim both} msg] $msg
} {0 0}

test compare.154 { contains -trim none } {
    list [catch {blt::stringutils contains "a" b -trim none} msg] $msg
} {0 0}


test compare.155 { contains a A -nocase } {
    list [catch {blt::stringutils contains a A -nocase} msg] $msg
} {0 1}

test compare.156 { contains A a } {
    list [catch {blt::stringutils contains A a -nocase} msg] $msg
} {0 1}

test compare.157 { contains AB ab } {
    list [catch {blt::stringutils contains AB ab -nocase} msg] $msg
} {0 1}

test compare.158 { contains AAAA aaa } {
    list [catch {blt::stringutils contains AAAA aaa} msg] $msg
} {0 0}

test compare.159 { contains abcdef d } {
    list [catch {blt::stringutils contains abcdef d} msg] $msg
} {0 1}

test compare.160 { contains abcdef a } {
    list [catch {blt::stringutils contains abcdef a} msg] $msg
} {0 1}

test compare.161 { contains abcdef f } {
    list [catch {blt::stringutils contains abcdef f} msg] $msg
} {0 1}

test compare.162 { contains abcdef f } {
    list [catch {blt::stringutils contains abcdef cde} msg] $msg
} {0 1}


test compare.163 { dictcompare (no args) } {
    list [catch {blt::stringutils dictcompare} msg] $msg
} {1 {wrong # args: should be "blt::stringutils dictcompare string1 string2"}}

test compare.164 { dictcompare (one arg) } {
    list [catch {blt::stringutils dictcompare a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils dictcompare string1 string2"}}

test compare.165 { dictcompare (bad switch) } {
    list [catch {blt::stringutils dictcompare a b -badSwitch} msg] $msg
} {1 {wrong # args: should be "blt::stringutils dictcompare string1 string2"}}

test compare.166 { dictcompare a b } {
    list [catch {blt::stringutils dictcompare a b} msg] $msg
} {0 -1}

test compare.167 { dictcompare A a } {
    list [catch {blt::stringutils dictcompare A a} msg] $msg
} {0 -1}

test compare.168 { dictcompare Abc abc } {
    list [catch {blt::stringutils dictcompare Abc abc} msg] $msg
} {0 -1}

test compare.169 { dictcompare abc Abc } {
    list [catch {blt::stringutils dictcompare abc Abc} msg] $msg
} {0 1}

test compare.170 { dictcompare abc10 abc9 } {
    list [catch {blt::stringutils dictcompare abc10 abc9} msg] $msg
} {0 1}

test compare.171 { dictcompare abc2 abc9 } {
    list [catch {blt::stringutils dictcompare abc2 abc9} msg] $msg
} {0 -7}

test compare.172 { dictcompare Abc2 abc9 } {
    list [catch {blt::stringutils dictcompare Abc2 abc9} msg] $msg
} {0 -7}

test compare.173 { dictcompare abc2 Abc9 } {
    list [catch {blt::stringutils dictcompare abc2 Abc9} msg] $msg
} {0 -7}

test compare.174 { isbetween (no args) } {
    list [catch {blt::stringutils isbetween} msg] $msg
} {1 {wrong # args: should be "blt::stringutils isbetween str first last ?switches?"}}

test compare.175 { isbetween (one arg) } {
    list [catch {blt::stringutils isbetween a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils isbetween str first last ?switches?"}}

test compare.176 { isbetween (two args) } {
    list [catch {blt::stringutils isbetween a b} msg] $msg
} {1 {wrong # args: should be "blt::stringutils isbetween str first last ?switches?"}}

test compare.177 { isbetween (bad switch) } {
    list [catch {blt::stringutils isbetween a b c -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase }}

test compare.178 { isbetween -nocase } {
    list [catch {blt::stringutils isbetween b A C -nocase} msg] $msg
} {0 1}

test compare.179 { isbetween -nocase } {
    list [catch {blt::stringutils isbetween a B C -nocase} msg] $msg
} {0 0}

test compare.180 { ismember (no args) } {
    list [catch {blt::stringutils ismember} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ismember str list ?switches?"}}

test compare.181 { ismember (one arg) } {
    list [catch {blt::stringutils ismember a} msg] $msg
} {1 {wrong # args: should be "blt::stringutils ismember str list ?switches?"}}

test compare.182 { ismember emptystring } {
    list [catch {blt::stringutils ismember a {}} msg] $msg
}  {0 0}

test compare.183 { ismember emptystrings } {
    list [catch {blt::stringutils ismember {} {}} msg] $msg
}  {0 0}

test compare.184 { ismember first } {
    list [catch {blt::stringutils ismember a { a b c d e f }} msg] $msg
}  {0 1}

test compare.185 { ismember last } {
    list [catch {blt::stringutils ismember f { a b c d e f }} msg] $msg
}  {0 1}

test compare.186 { ismember middle } {
    list [catch {blt::stringutils ismember d { a b c d e f }} msg] $msg
}  {0 1}

test compare.187 { ismember middle -sorted } {
    list [catch {
	blt::stringutils ismember d { a b c d e f } -sorted increasing
    } msg] $msg
}  {0 1}

test compare.188 { ismember middle -sorted } {
    list [catch {
	blt::stringutils ismember d { a b c d e f } -sorted decreasing
    } msg] $msg
}  {0 0}

test compare.189 { ismember middle -sorted } {
    list [catch {
	blt::stringutils ismember d { f e d c b a } -sorted decreasing
    } msg] $msg
}  {0 1}

test compare.190 { ismember middle -sorted badValue } {
    list [catch {
	blt::stringutils ismember d { f e d c b a } -sorted badValue
    } msg] $msg
} {1 {bad sorted value "badValue": should be decreasing, increasing, or none}}

test compare.191 { ismember (bad switch) } {
    list [catch {blt::stringutils ismember a b -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -nocase 
   -sorted sortDirection
   -trimwhitespace trimName}}

test compare.192 { ismember -nocase } {
    list [catch {blt::stringutils ismember b {A B C} -nocase} msg] $msg
} {0 1}

test compare.193 { ismember -nocase } {
    list [catch {blt::stringutils ismember a { B C } -nocase} msg] $msg
} {0 0}

test compare.194 { ismember -trim badValue } {
    list [catch {blt::stringutils ismember a b -trim badValue} msg] $msg
} {1 {bad trim value "badValue": should be left, right, both, or none}}

test compare.195 { ismember a b } {
    list [catch {blt::stringutils ismember a b} msg] $msg
} {0 0}

test compare.196 { ismember -trim left } {
    list [catch {
	blt::stringutils ismember "  a" { c b a } -trim left
    } msg] $msg
} {0 1}

test compare.197 { ismember -trim right } {
    list [catch {
	blt::stringutils ismember "a  " { c b a } -trim right
    } msg] $msg
} {0 1}

test compare.198 { ismember -trim both } {
    list [catch {blt::stringutils ismember " a " { c b a } -trim both} msg] $msg
} {0 1}

test compare.199 { ismember -trim none } {
    list [catch {blt::stringutils ismember "a" { c b a } -trim none} msg] $msg
} {0 1}

test compare.200 { ismember -trim left } {
    list [catch {blt::stringutils ismember "  a" { c b a } -trim left} msg] $msg
} {0 1}

test compare.201 { ismember -trim right } {
    list [catch {
	blt::stringutils ismember "a  " { c b a } -trim right
    } msg] $msg
} {0 1}

test compare.202 { ismember -trim both } {
    list [catch {blt::stringutils ismember " a " b -trim both} msg] $msg
} {0 0}

test compare.203 { ismember -trim none } {
    list [catch {blt::stringutils ismember "a" b -trim none} msg] $msg
} {0 0}


# contains test?
# begins test?
# distcompare test?
# ends test?
# equals test?
# isbetween test?
# ismember test?





