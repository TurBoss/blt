
package require BLT

if {[info procs test] != "test"} {
    source defs
}

if [file exists ../library] {
    set blt_library ../library
}

#set VERBOSE 1
#set DIFF 1

proc ReadAndDeleteFile { fileName } {
    set f [open $fileName "r"]
    set contents [read $f]
    close $f
    file delete -force $fileName
    return $contents
}

file delete -force testDir

proc MakeTestFile { fileName perms contents } {
    if { [file exists testDir] } {
	if { ![file isdirectory testDir] } {
	    error "testDir is not directory"
	}
    } else {
	file mkdir testDir
    }
    set f [open testDir/$fileName "w" $perms]
    puts -nonewline $f $contents
    close $f
}

test tree.1 {tree no args} {
    list [catch {blt::tree} msg] $msg
} {1 {wrong # args: should be one of...
  blt::tree create ?treeName?
  blt::tree destroy ?treeName ...?
  blt::tree exists treeName
  blt::tree load treeName libpath
  blt::tree names ?pattern ...?}}

test tree.2 {tree create #auto} {
    list [catch {blt::tree create #auto} msg] $msg
} {0 ::tree0}

test tree.3 {tree create #auto.suffix} {
    list [catch {blt::tree create #auto.suffix} msg] $msg
} {0 ::tree0.suffix}

test tree.4 {tree create prefix.#auto} {
    list [catch {blt::tree create prefix.#auto} msg] $msg
} {0 ::prefix.tree0}

test tree.5 {tree create prefix.#auto.suffix} {
    list [catch {blt::tree create prefix.#auto.suffix} msg] $msg
} {0 ::prefix.tree0.suffix}

test tree.6 {tree create prefix.#auto.suffix.#auto} {
    list [catch {blt::tree create prefix.#auto.suffix.#auto} msg] $msg
} {0 ::prefix.tree0.suffix.#auto}

test tree.7 {tree create badNs::#auto} {
    list [catch {blt::tree create badNs::#auto} msg] $msg
} {1 {unknown namespace "badNs"}}

test tree.8 {tree create ::::xyz} {
    list [catch {blt::tree create ::::xyz} msg] $msg
} {0 ::xyz}

test tree.9 {tree destroy} {
    list [catch {blt::tree destroy} msg] $msg
} {0 {}}

test tree.10 {tree destroy badTree} {
    list [catch {blt::tree destroy badTree} msg] $msg
} {1 {can't find a tree named "badTree"}}

test tree.11 {tree destroy badNs::badTree} {
    list [catch {blt::tree destroy badNs::badTree} msg] $msg
} {1 {can't find a tree named "badNs::badTree"}}

test tree.12 {tree destroy ::badNs::xyz} {
    list [catch {blt::tree destroy ::badNs::xyz} msg] $msg
} {1 {can't find a tree named "::badNs::xyz"}}

test tree.13 {tree destroy ::::::xyz} {
    list [catch {blt::tree destroy ::::::xyz} msg] $msg
} {0 {}}

test tree.14 {tree destroy [tree names *tree0*]} {
    list [catch {eval blt::tree destroy [blt::tree names *tree0*]} msg] $msg
} {0 {}}

test tree.15 {create} {
    list [catch {blt::tree create} msg] $msg
} {0 ::tree0}

test tree.16 {create} {
    list [catch {blt::tree create} msg] $msg
} {0 ::tree1}

test tree.17 {create fred} {
    list [catch {blt::tree create fred} msg] $msg
} {0 ::fred}

test tree.18 {create fred} {
    list [catch {blt::tree create fred} msg] $msg
} {1 {a tree "::fred" already exists}}

test tree.19 {exists fred} {
    list [catch {blt::tree exists fred} msg] $msg
} {0 1}

test tree.20 {exists if} {
    list [catch {blt::tree exists if} msg] $msg
} {0 0}

test tree.21 {create if} {
    list [catch {blt::tree create if} msg] $msg
} {1 {a command "::if" already exists}}

proc tree2 {} {}
proc tree3 {} {}

test tree.22 {create} {
    list [catch {blt::tree create} msg] $msg
} {0 ::tree4}

rename tree2 ""
rename tree3 ""

test tree.23 {delete tree4} {
    list [catch {blt::tree destroy tree4} msg] $msg
} {0 {}}

test tree.24 {tree create (bad namespace)} {
    list [catch {blt::tree create badName::fred} msg] $msg
} {1 {unknown namespace "badName"}}

test tree.25 {tree create (wrong # args)} {
    list [catch {blt::tree create a b} msg] $msg
} {1 {wrong # args: should be "blt::tree create ?treeName?"}}

test tree.26 {tree names} {
    list [catch {blt::tree names} msg] [lsort $msg]
} {0 {::fred ::tree0 ::tree1}}

test tree.27 {tree names pattern)} {
    list [catch {blt::tree names ::tree*} msg] [lsort $msg]
} {0 {::tree0 ::tree1}}

test tree.28 {tree names badPattern)} {
    list [catch {blt::tree names badPattern*} msg] $msg
} {0 {}}

test tree.29 {tree names pattern arg (wrong # args)} {
    list [catch {blt::tree names pattern arg} msg] $msg
} {1 {wrong # args: should be "blt::tree names ?pattern ...?"}}

test tree.30 {tree destroy (no args)} {
    list [catch {blt::tree destroy} msg] $msg
} {0 {}}

test tree.31 {tree destroy badTree} {
    list [catch {blt::tree destroy badTree} msg] $msg
} {1 {can't find a tree named "badTree"}}

test tree.32 {exists fred} {
    list [catch {blt::tree exists fred} msg] $msg
} {0 1}

test tree.33 {tree destroy fred} {
    list [catch {blt::tree destroy fred} msg] $msg
} {0 {}}

test tree.34 {exists fred} {
    list [catch {blt::tree exists fred} msg] $msg
} {0 0}

test tree.35 {tree destroy tree0 tree1} {
    list [catch {blt::tree destroy tree0 tree1} msg] $msg
} {0 {}}

test tree.36 {exists tree0} {
    list [catch {blt::tree exists tree0} msg] $msg
} {0 0}

test tree.37 {exists tree1} {
    list [catch {blt::tree exists tree1} msg] $msg
} {0 0}


test tree.38 {create} {
    list [catch {blt::tree create} msg] $msg
} {0 ::tree0}

test tree.39 {tree0} {
    list [catch {tree0} msg] $msg
} {1 {wrong # args: should be one of...
  tree0 ancestor node1 node2
  tree0 append nodeName valueName ?value ...?
  tree0 apply nodeName ?switches ...?
  tree0 attach treeName ?switches ...?
  tree0 children nodeName ?switches ...?
  tree0 copy parentNode ?treeName? nodeName ?switches ...?
  tree0 degree nodeName
  tree0 delete ?nodeName ...?
  tree0 depth ?nodeName?
  tree0 dir nodeName path ?switches ...?
  tree0 dump nodeName ?switches ...?
  tree0 dup nodeName
  tree0 exists nodeName ?valueName?
  tree0 export formatName ?switches ...?
  tree0 find nodeName ?switches ...?
  tree0 findchild nodeName label
  tree0 firstchild nodeName
  tree0 get nodeName ?valueName? ?defValue?
  tree0 import formatName ?switches ...?
  tree0 index label|list
  tree0 insert parentNode ?switches ...?
  tree0 isancestor node1 node2
  tree0 isbefore node1 node2
  tree0 isleaf nodeName
  tree0 isroot nodeName
  tree0 keys nodeName ?nodeName...?
  tree0 label nodeName ?newLabel?
  tree0 lappend nodeName valueName ?value ...?
  tree0 lastchild nodeName
  tree0 lindex nodeName valueName index
  tree0 linsert nodeName valueName index ?value...?
  tree0 llength nodeName valueName
  tree0 lrange nodeName valueName first last
  tree0 lreplace nodeName valueName first last ?value...?
  tree0 move nodeName destNode ?switches ...?
  tree0 names nodeName ?valueName?
  tree0 next nodeName
  tree0 nextsibling nodeName
  tree0 notify args ...
  tree0 parent nodeName
  tree0 path ?args ...?
  tree0 position ?switches ...? nodeName...
  tree0 previous nodeName
  tree0 prevsibling nodeName
  tree0 replace nodeName destNode
  tree0 restore nodeName ?switches ...?
  tree0 root 
  tree0 set nodeName ?valueName value ...?
  tree0 size nodeName
  tree0 sort nodeName ?switches ...?
  tree0 tag args ...
  tree0 trace args ...
  tree0 type nodeName valueName
  tree0 unset nodeName ?valueName ...?}}

test tree.40 {tree0 badOp} {
    list [catch {tree0 badOp} msg] $msg
} {1 {bad operation "badOp": should be one of...
  tree0 ancestor node1 node2
  tree0 append nodeName valueName ?value ...?
  tree0 apply nodeName ?switches ...?
  tree0 attach treeName ?switches ...?
  tree0 children nodeName ?switches ...?
  tree0 copy parentNode ?treeName? nodeName ?switches ...?
  tree0 degree nodeName
  tree0 delete ?nodeName ...?
  tree0 depth ?nodeName?
  tree0 dir nodeName path ?switches ...?
  tree0 dump nodeName ?switches ...?
  tree0 dup nodeName
  tree0 exists nodeName ?valueName?
  tree0 export formatName ?switches ...?
  tree0 find nodeName ?switches ...?
  tree0 findchild nodeName label
  tree0 firstchild nodeName
  tree0 get nodeName ?valueName? ?defValue?
  tree0 import formatName ?switches ...?
  tree0 index label|list
  tree0 insert parentNode ?switches ...?
  tree0 isancestor node1 node2
  tree0 isbefore node1 node2
  tree0 isleaf nodeName
  tree0 isroot nodeName
  tree0 keys nodeName ?nodeName...?
  tree0 label nodeName ?newLabel?
  tree0 lappend nodeName valueName ?value ...?
  tree0 lastchild nodeName
  tree0 lindex nodeName valueName index
  tree0 linsert nodeName valueName index ?value...?
  tree0 llength nodeName valueName
  tree0 lrange nodeName valueName first last
  tree0 lreplace nodeName valueName first last ?value...?
  tree0 move nodeName destNode ?switches ...?
  tree0 names nodeName ?valueName?
  tree0 next nodeName
  tree0 nextsibling nodeName
  tree0 notify args ...
  tree0 parent nodeName
  tree0 path ?args ...?
  tree0 position ?switches ...? nodeName...
  tree0 previous nodeName
  tree0 prevsibling nodeName
  tree0 replace nodeName destNode
  tree0 restore nodeName ?switches ...?
  tree0 root 
  tree0 set nodeName ?valueName value ...?
  tree0 size nodeName
  tree0 sort nodeName ?switches ...?
  tree0 tag args ...
  tree0 trace args ...
  tree0 type nodeName valueName
  tree0 unset nodeName ?valueName ...?}}

test tree.41 {tree0 insert (wrong # args)} {
    list [catch {tree0 insert} msg] $msg
} {1 {wrong # args: should be "tree0 insert parentNode ?switches ...?"}}

test tree.42 {tree0 insert badParent} {
    list [catch {tree0 insert badParent} msg] $msg
} {1 {can't find tag or id "badParent" in ::tree0}}

test tree.43 {tree0 insert 1000} {
    list [catch {tree0 insert 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree0}}

test tree.44 {tree0 insert 0} {
    list [catch {tree0 insert 0} msg] $msg
} {0 1}

test tree.45 {tree0 insert 0} {
    list [catch {tree0 insert 0} msg] $msg
} {0 2}

test tree.46 {tree0 insert root} {
    list [catch {tree0 insert root} msg] $msg
} {0 3}

test tree.47 {tree0 insert all} {
    list [catch {tree0 insert all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.48 {tree0 insert 0 -after (no arg)} {
    list [catch {tree0 insert 0 -after} msg] $msg
} {1 {value for "-after" missing}}

test tree.49 {tree0 insert 0 -after badPosition} {
    list [catch {tree0 insert 0 -after badPosition} msg] $msg
} {1 {can't find tag or id "badPosition" in ::tree0}}

test tree.50 {tree0 insert 0 -after -1} {
    list [catch {tree0 insert 0 -after -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree0}}

test tree.51 {tree0 insert 0 -after 1000} {
    list [catch {tree0 insert 0 -after 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree0}}

test tree.52 {tree0 insert 0 -before (no arg)} {
    list [catch {tree0 insert 0 -before} msg] $msg
} {1 {value for "-before" missing}}

test tree.53 {tree0 insert 0 -before badPosition} {
    list [catch {tree0 insert 0 -before badPosition} msg] $msg
} {1 {can't find tag or id "badPosition" in ::tree0}}

test tree.54 {tree0 insert 0 -before -1} {
    list [catch {tree0 insert 0 -before -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree0}}

test tree.55 {tree0 insert 0 -before 1000} {
    list [catch {tree0 insert 0 -before 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree0}}

test tree.56 {tree0 insert 0 -after last} {
    list [catch {tree0 insert 0 -after [tree0 lastchild 0]} msg] $msg
} {0 4}

test tree.57 {tree0 insert 0 -tags myTag} {
    list [catch {tree0 insert 0 -tags myTag} msg] $msg
} {0 5}

test tree.58 {tree0 insert 0 -tags {myTag1 myTag2} } {
    list [catch {tree0 insert 0 -tags {myTag1 myTag2}} msg] $msg
} {0 6}

test tree.59 {tree0 insert 0 -tags root} {
    list [catch {tree0 insert 0 -tags root} msg] $msg
} {1 {can't add reserved tag "root"}}

test tree.60 {tree0 insert 0 -tags (missing arg)} {
    list [catch {tree0 insert 0 -tags} msg] $msg
} {1 {value for "-tags" missing}}

test tree.61 {tree0 insert 0 -label myLabel -tags thisTag} {
    list [catch {tree0 insert 0 -label myLabel -tags thisTag} msg] $msg
} {0 8}

test tree.62 {tree0 insert 0 -label (missing arg)} {
    list [catch {tree0 insert 0 -label} msg] $msg
} {1 {value for "-label" missing}}

test tree.63 {tree0 insert 1 -tags thisTag} {
    list [catch {tree0 insert 1 -tags thisTag} msg] $msg
} {0 9}

test tree.64 {tree0 insert 1 -data key (missing value)} {
    list [catch {tree0 insert 1 -data key} msg] $msg
} {1 {missing value for "key"}}

test tree.65 {tree0 insert 1 -data {key value}} {
    list [catch {tree0 insert 1 -data {key value}} msg] $msg
} {0 11}

test tree.66 {tree0 insert 1 -data {key1 value1 key2 value2}} {
    list [catch {tree0 insert 1 -data {key1 value1 key2 value2}} msg] $msg
} {0 12}

test tree.67 {get} {
    list [catch {
	tree0 get 12
    } msg] $msg
} {0 {key1 value1 key2 value2}}

test tree.68 {tree0 children} {
    list [catch {tree0 children} msg] $msg
} {1 {wrong # args: should be "tree0 children nodeName ?switches ...?"}}

test tree.69 {tree0 children 0} {
    list [catch {tree0 children 0} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test tree.70 {tree0 children root} {
    list [catch {tree0 children root} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test tree.71 {tree0 children 1} {
    list [catch {tree0 children 1} msg] $msg
} {0 {9 11 12}}

test tree.72 {tree0 children 1 -nocomplain} {
    list [catch {tree0 children 1 -nocomplain} msg] $msg
} {0 {9 11 12}}

test tree.73 {tree0 children badNode -nocomplain} {
    list [catch {tree0 children badNode -nocomplain} msg] $msg
} {0 {}}

test tree.74 {tree0 children badNode} {
    list [catch {tree0 children badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}


test tree.75 {tree0 insert myTag} {
    list [catch {tree0 insert myTag} msg] $msg
} {0 13}

test tree.76 {tree0 index myTag} {
    list [catch {tree0 index myTag} msg] $msg
} {0 5}

test tree.77 {tree0 children 5} {
    list [catch {tree0 children 5} msg] $msg
} {0 13}

test tree.78 {tree0 children -1 -nocomplain} {
    list [catch {tree0 children -1 -nocomplain} msg] $msg
} {0 {}}

test tree.79 {tree0 children badNode -nocomplain} {
    list [catch {tree0 children badNode -nocomplain} msg] $msg
} {0 {}}

test tree.80 {tree0 children myTag} {
    list [catch {tree0 children myTag} msg] $msg
} {0 13}

test tree.81 {tree0 children root} {
    list [catch {tree0 children root} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test tree.82 {tree0 children root -from 1} {
    list [catch {tree0 children root -from 1} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test tree.83 {tree0 children root -to 6 } {
    list [catch {tree0 children root -to 6} msg] $msg
} {0 {1 2 3 4 5 6}}

test tree.84 {tree0 children root -from 7 } {
    list [catch {tree0 children root -from 7} msg] $msg
} {1 {can't find tag or id "7" in ::tree0}}

test tree.85 {tree0 children root -to 7 } {
    list [catch {tree0 children root -to 7} msg] $msg
} {1 {can't find tag or id "7" in ::tree0}}

test tree.86 {tree0 children root -from 2} {
    list [catch {tree0 children root -from 2} msg] $msg
} {0 {2 3 4 5 6 8}}

test tree.87 {tree0 children root -from all} {
    list [catch {tree0 children root -from all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.88 {tree0 children root -to all} {
    list [catch {tree0 children root -to all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.89 {tree0 children root -from root} {
    list [catch {tree0 children root -from root} msg] $msg
} {1 {bad -from switch: node is not a child of "root"}}

test tree.90 {tree0 children root -to root} {
    list [catch {tree0 children root -to root} msg] $msg
} {1 {bad -to switch: node is not a child of "root"}}

test tree.91 {tree0 children root -from 1 -to 3} {
    list [catch {tree0 children root -from 1 -to 3} msg] $msg
} {0 {1 2 3}}

test tree.92 {tree0 children root -from -1} {
    list [catch {tree0 children root -from -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree0}}

test tree.93 {tree0 children root -to -1} {
    list [catch {tree0 children root -to -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree0}}

test tree.94 {tree0 firstchild (missing arg)} {
    list [catch {tree0 firstchild} msg] $msg
} {1 {wrong # args: should be "tree0 firstchild nodeName"}}

test tree.95 {tree0 firstchild badNode} {
    list [catch {tree0 firstchild badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.96 {tree0 firstchild all} {
    list [catch {tree0 firstchild all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.97 {tree0 firstchild root} {
    list [catch {tree0 firstchild root} msg] $msg
} {0 1}

test tree.98 {tree0 lastchild (missing arg)} {
    list [catch {tree0 lastchild} msg] $msg
} {1 {wrong # args: should be "tree0 lastchild nodeName"}}

test tree.99 {tree0 lastchild badNode} {
    list [catch {tree0 lastchild badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.100 {tree0 lastchild all} {
    list [catch {tree0 lastchild all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.101 {tree0 lastchild root} {
    list [catch {tree0 lastchild root} msg] $msg
} {0 8}

test tree.102 {tree0 nextsibling (missing arg)} {
    list [catch {tree0 nextsibling} msg] $msg
} {1 {wrong # args: should be "tree0 nextsibling nodeName"}}

test tree.103 {tree0 nextsibling 1)} {
    list [catch {tree0 nextsibling 1} msg] $msg
} {0 2}

test tree.104 {tree0 nextsibling 2)} {
    list [catch {tree0 nextsibling 2} msg] $msg
} {0 3}

test tree.105 {tree0 nextsibling 3)} {
    list [catch {tree0 nextsibling 3} msg] $msg
} {0 4}

test tree.106 {tree0 nextsibling 4)} {
    list [catch {tree0 nextsibling 4} msg] $msg
} {0 5}

test tree.107 {tree0 nextsibling 5)} {
    list [catch {tree0 nextsibling 5} msg] $msg
} {0 6}

test tree.108 {tree0 nextsibling 6)} {
    list [catch {tree0 nextsibling 6} msg] $msg
} {0 8}

test tree.109 {tree0 nextsibling 8)} {
    list [catch {tree0 nextsibling 8} msg] $msg
} {0 -1}

test tree.110 {tree0 nextsibling all)} {
    list [catch {tree0 nextsibling all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.111 {tree0 nextsibling badTag)} {
    list [catch {tree0 nextsibling badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::tree0}}

test tree.112 {tree0 nextsibling -1)} {
    list [catch {tree0 nextsibling -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree0}}

test tree.113 {tree0 prevsibling 2)} {
    list [catch {tree0 prevsibling 2} msg] $msg
} {0 1}

test tree.114 {tree0 prevsibling 1)} {
    list [catch {tree0 prevsibling 1} msg] $msg
} {0 -1}

test tree.115 {tree0 prevsibling -1)} {
    list [catch {tree0 prevsibling -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree0}}

test tree.116 {tree0 root)} {
    list [catch {tree0 root} msg] $msg
} {0 0}

test tree.117 {tree0 root badArg)} {
    list [catch {tree0 root badArgs} msg] $msg
} {1 {wrong # args: should be "tree0 root "}}

test tree.118 {tree0 parent (missing arg))} {
    list [catch {tree0 parent} msg] $msg
} {1 {wrong # args: should be "tree0 parent nodeName"}}

test tree.119 {tree0 parent root)} {
    list [catch {tree0 parent root} msg] $msg
} {0 -1}

test tree.120 {tree0 parent 1)} {
    list [catch {tree0 parent 1} msg] $msg
} {0 0}

test tree.121 {tree0 parent myTag)} {
    list [catch {tree0 parent myTag} msg] $msg
} {0 0}

test tree.122 {tree0 next (missing arg))} {
    list [catch {tree0 next} msg] $msg
} {1 {wrong # args: should be "tree0 next nodeName"}}

test tree.123 {tree0 next} {
    list [catch {tree0 next} msg] $msg
} {1 {wrong # args: should be "tree0 next nodeName"}}

test tree.124 {tree0 next badNode} {
    list [catch {tree0 next badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.125 {tree0 next (extra arg))} {
    list [catch {tree0 next root root} msg] $msg
} {1 {wrong # args: should be "tree0 next nodeName"}}

test tree.126 {tree0 next all} {
    list [catch {tree0 next all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.127 {tree0 next root} {
    list [catch {tree0 next root} msg] $msg
} {0 1}

test tree.128 {tree0 next 1)} {
    list [catch {tree0 next 1} msg] $msg
} {0 9}

test tree.129 {tree0 next 2)} {
    list [catch {tree0 next 2} msg] $msg
} {0 3}

test tree.130 {tree0 next 3)} {
    list [catch {tree0 next 3} msg] $msg
} {0 4}

test tree.131 {tree0 next 4)} {
    list [catch {tree0 next 4} msg] $msg
} {0 5}

test tree.132 {tree0 next 5)} {
    list [catch {tree0 next 5} msg] $msg
} {0 13}

test tree.133 {tree0 next 6)} {
    list [catch {tree0 next 6} msg] $msg
} {0 8}

test tree.134 {tree0 next 8)} {
    list [catch {tree0 next 8} msg] $msg
} {0 -1}

test tree.135 {tree0 previous} {
    list [catch {tree0 previous} msg] $msg
} {1 {wrong # args: should be "tree0 previous nodeName"}}

test tree.136 {tree0 previous badNode} {
    list [catch {tree0 previous badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.137 {tree0 previous all} {
    list [catch {tree0 previous all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.138 {tree0 previous 1)} {
    list [catch {tree0 previous 1} msg] $msg
} {0 0}

test tree.139 {tree0 previous 0)} {
    list [catch {tree0 previous 0} msg] $msg
} {0 -1}

test tree.140 {tree0 previous 8)} {
    list [catch {tree0 previous 8} msg] $msg
} {0 6}

test tree.141 {tree0 depth (no arg))} {
    list [catch {tree0 depth} msg] $msg
} {0 2}

test tree.142 {tree0 depth badNode} {
    list [catch {tree0 depth badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.143 {tree0 depth all} {
    list [catch {tree0 depth all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.144 {tree0 depth root))} {
    list [catch {tree0 depth root} msg] $msg
} {0 0}

test tree.145 {tree0 depth myTag))} {
    list [catch {tree0 depth myTag} msg] $msg
} {0 1}

test tree.146 {tree0 depth myTag))} {
    list [catch {tree0 depth myTag} msg] $msg
} {0 1}

test tree.147 {tree0 dump (missing arg)))} {
    list [catch {tree0 dump} msg] $msg
} {1 {wrong # args: should be "tree0 dump nodeName ?switches ...?"}}

test tree.148 {tree0 dump root -version 2.0} {
    list [catch {tree0 dump root -version 2.0} msg] $msg
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

test tree.149 {tree0 dump 1 -version 2.0} {
    list [catch {tree0 dump 1 -version 2.0} msg] $msg
} {0 {# V2.0
-1 1 {node1} {} {}
1 9 {node1 node9} {} {thisTag}
1 11 {node1 node11} {key value} {}
1 12 {node1 node12} {key1 value1 key2 value2} {}
}}

test tree.150 {tree0 dump this -version 2.0} {
    list [catch {tree0 dump myTag -version 2.0} msg] $msg
} {0 {# V2.0
-1 5 {node5} {} {myTag}
5 13 {node5 node13} {} {}
}}

test tree.151 {tree0 dump 1 badSwitch} {
    list [catch {tree0 dump 1 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -data data
   -file fileName
   -version versionNum
   -notags }}

test tree.152 {tree0 dump 11 -version 2.0} {
    list [catch {tree0 dump 11 -version 2.0} msg] $msg
} {0 {# V2.0
-1 11 {node11} {key value} {}
}}

test tree.153 {tree0 dump all} {
    list [catch {tree0 dump all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.154 {tree0 dump all} {
    list [catch {tree0 dump all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.155 {tree0 dump 0 -file test.dump} {
    list [catch {tree0 dump 0 -file test.dump} msg] $msg
} {0 {}}

test tree.156 {tree0 get 9} {
    list [catch {tree0 get 9} msg] $msg
} {0 {}}

test tree.157 {tree0 get all} {
    list [catch {tree0 get all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.158 {tree0 get root} {
    list [catch {tree0 get root} msg] $msg
} {0 {}}

test tree.159 {tree0 get 9 key} {
    list [catch {tree0 get root} msg] $msg
} {0 {}}

test tree.160 {tree0 get 12} {
    list [catch {tree0 get 12} msg] $msg
} {0 {key1 value1 key2 value2}}

test tree.161 {tree0 get 12 key1} {
    list [catch {tree0 get 12 key1} msg] $msg
} {0 value1}

test tree.162 {tree0 get 12 key2} {
    list [catch {tree0 get 12 key2} msg] $msg
} {0 value2}

test tree.163 {tree0 get 12 key1 defValue } {
    list [catch {tree0 get 12 key1 defValue} msg] $msg
} {0 value1}

test tree.164 {tree0 get 12 key100 defValue } {
    list [catch {tree0 get 12 key100 defValue} msg] $msg
} {0 defValue}

test tree.165 {tree0 index (missing arg) } {
    list [catch {tree0 index} msg] $msg
} {1 {wrong # args: should be "tree0 index label|list"}}

test tree.166 {tree0 index 0 10 (extra arg) } {
    list [catch {tree0 index 0 10} msg] $msg
} {1 {wrong # args: should be "tree0 index label|list"}}

test tree.167 {tree0 index 0} {
    list [catch {tree0 index 0} msg] $msg
} {0 0}

test tree.168 {tree0 index root} {
    list [catch {tree0 index root} msg] $msg
} {0 0}

test tree.169 {tree0 index all} {
    list [catch {tree0 index all} msg] $msg
} {0 -1}

test tree.170 {tree0 index myTag} {
    list [catch {tree0 index myTag} msg] $msg
} {0 5}

test tree.171 {tree0 index thisTag} {
    list [catch {tree0 index thisTag} msg] $msg
} {0 -1}

test tree.172 {tree0 is (no args)} {
    list [catch {tree0 is} msg] $msg
} {1 {ambiguous operation "is" matches:  isancestor isbefore isleaf isroot}}

test tree.173 {tree0 isbefore} {
    list [catch {tree0 isbefore} msg] $msg
} {1 {wrong # args: should be "tree0 isbefore node1 node2"}}

test tree.174 {tree0 isbefore 0 10 20} {
    list [catch {tree0 isbefore 0 10 20} msg] $msg
} {1 {wrong # args: should be "tree0 isbefore node1 node2"}}

test tree.175 {tree0 isbefore 0 12} {
    list [catch {tree0 isbefore 0 12} msg] $msg
} {0 1}

test tree.176 {tree0 isbefore 12 0} {
    list [catch {tree0 isbefore 12 0} msg] $msg
} {0 0}

test tree.177 {tree0 isbefore 0 0} {
    list [catch {tree0 isbefore 0 0} msg] $msg
} {0 0}

test tree.178 {tree0 isbefore root 0} {
    list [catch {tree0 isbefore root 0} msg] $msg
} {0 0}

test tree.179 {tree0 isbefore 0 all} {
    list [catch {tree0 isbefore 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.180 {tree0 isancestor} {
    list [catch {tree0 isancestor} msg] $msg
} {1 {wrong # args: should be "tree0 isancestor node1 node2"}}

test tree.181 {tree0 isancestor 0 12 20} {
    list [catch {tree0 isancestor 0 12 20} msg] $msg
} {1 {wrong # args: should be "tree0 isancestor node1 node2"}}

test tree.182 {tree0 isancestor 0 12} {
    list [catch {tree0 isancestor 0 12} msg] $msg
} {0 1}

test tree.183 {tree0 isancestor 12 0} {
    list [catch {tree0 isancestor 12 0} msg] $msg
} {0 0}

test tree.184 {tree0 isancestor 1 2} {
    list [catch {tree0 isancestor 1 2} msg] $msg
} {0 0}

test tree.185 {tree0 isancestor root 0} {
    list [catch {tree0 isancestor root 0} msg] $msg
} {0 0}

test tree.186 {tree0 isancestor 0 all} {
    list [catch {tree0 isancestor 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.187 {tree0 isroot (missing arg)} {
    list [catch {tree0 isroot} msg] $msg
} {1 {wrong # args: should be "tree0 isroot nodeName"}}

test tree.188 {tree0 isroot 0 20 (extra arg)} {
    list [catch {tree0 isroot 0 20} msg] $msg
} {1 {wrong # args: should be "tree0 isroot nodeName"}}

test tree.189 {tree0 isroot 0} {
    list [catch {tree0 isroot 0} msg] $msg
} {0 1}

test tree.190 {tree0 isroot 12} {
    list [catch {tree0 isroot 12} msg] $msg
} {0 0}

test tree.191 {tree0 isroot 1} {
    list [catch {tree0 isroot 1} msg] $msg
} {0 0}

test tree.192 {tree0 isroot root} {
    list [catch {tree0 isroot root} msg] $msg
} {0 1}

test tree.193 {tree0 isroot all} {
    list [catch {tree0 isroot all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.194 {tree0 isleaf (missing arg)} {
    list [catch {tree0 isleaf} msg] $msg
} {1 {wrong # args: should be "tree0 isleaf nodeName"}}

test tree.195 {tree0 isleaf 0 20 (extra arg)} {
    list [catch {tree0 isleaf 0 20} msg] $msg
} {1 {wrong # args: should be "tree0 isleaf nodeName"}}

test tree.196 {tree0 isleaf 0} {
    list [catch {tree0 isleaf 0} msg] $msg
} {0 0}

test tree.197 {tree0 isleaf 12} {
    list [catch {tree0 isleaf 12} msg] $msg
} {0 1}

test tree.198 {tree0 isleaf 1} {
    list [catch {tree0 isleaf 1} msg] $msg
} {0 0}

test tree.199 {tree0 isleaf root} {
    list [catch {tree0 isleaf root} msg] $msg
} {0 0}

test tree.200 {tree0 isleaf all} {
    list [catch {tree0 isleaf all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.201 {tree0 isleaf 1000} {
    list [catch {tree0 isleaf 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree0}}

test tree.202 {tree0 isleaf badTag} {
    list [catch {tree0 isleaf badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::tree0}}

test tree.203 {tree0 set (missing arg)} {
    list [catch {tree0 set} msg] $msg
} {1 {wrong # args: should be "tree0 set nodeName ?valueName value ...?"}}

test tree.204 {tree0 set badNode (missing arg)} {
    list [catch {tree0 set badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.205 {tree0 set 0 (missing arg)} {
    list [catch {tree0 set 0} msg] $msg
} {0 {}}

test tree.206 {tree0 set 0 key (missing arg)} {
    list [catch {tree0 set 0 key} msg] $msg
} {1 {missing value for "key"}}

test tree.207 {tree0 set 0 key value} {
    list [catch {tree0 set 0 key value} msg] $msg
} {0 {}}

test tree.208 {tree0 set 0 key1 value1 key2 value2 key3 value3} {
    list [catch {tree0 set 0 key1 value1 key2 value2 key3 value3} msg] $msg
} {0 {}}

test tree.209 {tree0 set 0 key1 value1 key2 (missing arg)} {
    list [catch {tree0 set 0 key1 value1 key2} msg] $msg
} {1 {missing value for "key2"}}

test tree.210 {tree0 set 0 key value} {
    list [catch {tree0 set 0 key value} msg] $msg
} {0 {}}

test tree.211 {tree0 set 0 key1 value1 key2 (missing arg)} {
    list [catch {tree0 set 0 key1 value1 key2} msg] $msg
} {1 {missing value for "key2"}}

test tree.212 {tree0 set all} {
    list [catch {tree0 set all} msg] $msg
} {0 {}}

test tree.213 {tree0 set all abc 123} {
    list [catch {tree0 set all abc 123} msg] $msg
} {0 {}}

test tree.214 {tree0 set root} {
    list [catch {tree0 set root} msg] $msg
} {0 {}}

test tree.215 {blt::tree create} {
    list [catch {blt::tree create myTree} msg] $msg
} {0 ::myTree}

test tree.216 {myTree set root abc 012} {
    list [catch {myTree set root abc 012} msg] $msg
} {0 {}}

test tree.217 {myTree get root abc} {
    list [catch {myTree get root abc} msg] $msg
} {0 012}

test tree.218 {myTree append root abc 345} {
    list [catch {myTree append root abc 345} msg] $msg
} {0 {}}

test tree.219 {myTree get root abc} {
    list [catch {myTree get root abc} msg] $msg
} {0 012345}

test tree.220 {myTree append root abc (missing arg)} {
    list [catch {myTree append root abc} msg] $msg
} {0 {}}

test tree.221 {myTree append root abc 678 9AB CD EF} {
    list [catch {myTree append root abc 678 9AB CD EF} msg] $msg
} {0 {}}

test tree.222 {myTree get root abc} {
    list [catch {myTree get root abc} msg] $msg
} {0 0123456789ABCDEF}

# Append with no values creates an empty value.
test tree.223 {myTree append root def} {
    list [catch {myTree append root def} msg] $msg
} {0 {}}

test tree.224 {myTree append root myArr(0)} {
    list [catch {myTree append root myArr(0)} msg] $msg
} {0 {}}

test tree.225 {myTree append root myArr(0) First} {
    list [catch {myTree append root myArr(0) First } msg] $msg
} {0 {}}

test tree.226 {myTree append root myArr(0) Second} {
    list [catch {myTree append root myArr(0) Second } msg] $msg
} {0 {}}

test tree.227 {myTree append root myArr(0) Third} {
    list [catch {myTree append root myArr(0) Third } msg] $msg
} {0 {}}

test tree.228 {myTree get root} {
    list [catch {myTree get root myArr} msg] $msg
} {0 {0 FirstSecondThird}}

test tree.229 {myTree get root myArr(0)} {
    list [catch {myTree get root myArr(0)} msg] $msg
} {0 FirstSecondThird}

test tree.230 {myTree get root myArr(badIndex)} {
    list [catch {myTree get root myArr(badIndex)} msg] $msg
} {1 {can't find an element "badIndex" in array "myArr" in tree "::myTree"}}

test tree.231 {myTree get root badArr(badIndex)} {
    list [catch {myTree get root badArr(badIndex)} msg] $msg
} {1 {can't find a value "badArr" in tree "::myTree"}}

test tree.232 {myTree set root myArr(1) 0} {
    list [catch {myTree set root myArr(1) 0 } msg] $msg
} {0 {}}

test tree.233 {myTree set root anotherArr(1) 0} {
    list [catch {myTree set root anotherArr(1) 0 } msg] $msg
} {0 {}}

test tree.234 {myTree lappend root myArr(0) Fourth Fifth Sixth} {
    list [catch {myTree lappend root myArr(0) Fourth Fifth Sixth } msg] $msg
} {0 {}}

test tree.235 {myTree get root myArr} {
    list [catch {myTree get root myArr} msg] $msg
} {0 {0 {FirstSecondThird Fourth Fifth Sixth} 1 0}}

test tree.236 {myTree unset root myArr(1)} {
    list [catch {myTree unset root myArr(1)} msg] $msg
} {0 {}}

test tree.237 {myTree unset root myArr(0)} {
    list [catch {myTree unset root myArr(0)} msg] $msg
} {0 {}}

test tree.238 {tree0 unset} {
    list [catch {tree0 unset} msg] $msg
} {1 {wrong # args: should be "tree0 unset nodeName ?valueName ...?"}}

test tree.239 {tree0 unset badNode} {
    list [catch {tree0 unset badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.240 {tree0 unset badNode badValue} {
    list [catch {tree0 unset badNode badValue} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.241 {tree0 unset root badValue} {
    list [catch {tree0 unset root badValue} msg] $msg
} {0 {}}

test tree.242 {tree0 unset root badValue(0)} {
    list [catch {tree0 unset root badValue(0)} msg] $msg
} {0 {}}

test tree.243 {tree0 set root myScalar 1} {
    list [catch {tree0 set root myScalar 1} msg] $msg
} {0 {}}

test tree.244 {tree0 set root myArray(0) 0} {
    list [catch {tree0 set root myArray(0) 0} msg] $msg
} {0 {}}

test tree.245 {tree0 set root myArray(1) 1} {
    list [catch {tree0 set root myArray(1) 1} msg] $msg
} {0 {}}

test tree.246 {tree0 unset root myScalar} {
    list [catch {tree0 unset root myScalar} msg] $msg
} {0 {}}

test tree.247 {tree0 unset root myArray(0)} {
    list [catch {tree0 unset root myArray(0)} msg] $msg
} {0 {}}

test tree.248 {tree0 unset root myArray(badElem)} {
    list [catch {tree0 unset root myArray(badElem)} msg] $msg
} {1 {can't find array element "badElem" in value "myArray"}}

test tree.249 {tree0 get root} {
    list [catch {tree0 get root} msg] $msg
} {0 {key value key1 value1 key2 value2 key3 value3 abc 123 myArray {1 1}}}

test tree.250 {tree0 unset root myArray} {
    list [catch {tree0 unset root myArray} msg] $msg
} {0 {}}

test tree.251 {tree0 get root} {
    list [catch {tree0 get root} msg] $msg
} {0 {key value key1 value1 key2 value2 key3 value3 abc 123}}


test tree.252 {tree0 set root myArray(1) 1} {
    list [catch {tree0 set root myArray(1) 1} msg] $msg
} {0 {}}

test tree.253 {tree0 set root myArray(2) 2} {
    list [catch {tree0 set root myArray(2) 2} msg] $msg
} {0 {}}

test tree.254 {tree0 set root myArray(3) 3} {
    list [catch {tree0 set root myArray(3) 3} msg] $msg
} {0 {}}

test tree.255 {tree0 set root myScalar abc} {
    list [catch {tree0 set root myScalar abc} msg] $msg
} {0 {}}

test tree.256 {tree0 unset root myArray(1) myArray(3) myScalar} {
    list [catch {tree0 unset root myArray(1) myArray(3) myScalar} msg] $msg
} {0 {}}

test tree.257 {tree0 get root} {
    list [catch {tree0 get root} msg] $msg
} {0 {key value key1 value1 key2 value2 key3 value3 abc 123 myArray {2 2}}}


test tree.258 {tree0 unset root myArray} {
    list [catch {tree0 unset root myArray} msg] $msg
} {0 {}}

test tree.259 {tree0 get root} {
    list [catch {tree0 get root} msg] $msg
} {0 {key value key1 value1 key2 value2 key3 value3 abc 123}}



# Not an error because unsetting all the elements of an array still leaves
# the value.
test tree.260 {myTree get root myArr} {
    list [catch {myTree get root myArr} msg] $msg
} {0 {}}

# Unsetting all the elements of an array still leaves the value
test tree.261 {myTree get root } {
    list [catch {myTree get root} msg] $msg
} {0 {abc 0123456789ABCDEF def {} myArr {} anotherArr {1 0}}}

test tree.262 {myTree unset root myArr} {
    list [catch {myTree unset root myArr} msg] $msg
} {0 {}}

test tree.263 {myTree unset root anotherArr} {
    list [catch {myTree unset root anotherArr} msg] $msg
} {0 {}}

test tree.264 {myTree get root anotherArr} {
    list [catch {myTree get root anotherArr} msg] $msg
} {1 {can't find a value "anotherArr" in tree "::myTree"}}


test tree.265 {myTree append root def} {
    list [catch {myTree append root def 0} msg] $msg
} {0 {}}

test tree.266 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 0}

test tree.267 {myTree lappend} {
    list [catch {myTree lappend} msg] $msg
} {1 {wrong # args: should be "myTree lappend nodeName valueName ?value ...?"}}

test tree.268 {myTree lappend badNode} {
    list [catch {myTree lappend badNode} msg] $msg
} {1 {wrong # args: should be "myTree lappend nodeName valueName ?value ...?"}}

test tree.269 {myTree lappend badNode xyz} {
    list [catch {myTree lappend badNode xyz} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.270 {myTree lappend root def} {
    list [catch {myTree lappend root def 1 2 3 4} msg] $msg
} {0 {}}

test tree.271 {myTree lappend root myArr(0) 1 2 3} {
    list [catch {myTree lappend root myArr(0) 1 2 3 4} msg] $msg
} {0 {}}

test tree.272 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 {0 1 2 3 4}}

test tree.273 {myTree lappend root ghi} {
    list [catch {myTree lappend root ghi} msg] $msg
} {0 {}}

test tree.274 {myTree get root} {
    list [catch {myTree get root} msg] $msg
} {0 {abc 0123456789ABCDEF def {0 1 2 3 4} myArr {0 {1 2 3 4}} ghi {}}}

test tree.275 {myTree set root ghi} {
    list [catch {myTree set root ghi 1} msg] $msg
} {0 {}}

test tree.276 {myTree set root myArr(0)} {
    list [catch {myTree set root myArr(0) "a b c d e"} msg] $msg
} {0 {}}

test tree.277 {myTree lappend root ghi} {
    list [catch {myTree lappend root ghi 2} msg] $msg
} {0 {}}

test tree.278 {myTree lindex root} {
    list [catch {myTree lindex root} msg] $msg
} {1 {wrong # args: should be "myTree lindex nodeName valueName index"}}

test tree.279 {myTree lindex root def} {
    list [catch {myTree lindex root def} msg] $msg
} {1 {wrong # args: should be "myTree lindex nodeName valueName index"}}

test tree.280 {myTree lindex badNode def 0} {
    list [catch {myTree lindex badNode def 0} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.281 {myTree lindex root badValue 0} {
    list [catch {myTree lindex root badValue 0} msg] $msg
} {1 {can't find a value "badValue" in tree "::myTree"}}

test tree.282 {myTree lindex root def badIndex} {
    list [catch {myTree lindex root def badIndex} msg] $msg
} {1 {expected integer but got "badIndex"}}

test tree.283 {myTree lindex root def end} {
    list [catch {myTree lindex root def end} msg] $msg
} {0 4}

test tree.284 {myTree lindex root def -1} {
    list [catch {myTree lindex root def -1} msg] $msg
} {1 {bad value "-1": can't be negative}}

test tree.285 {myTree lindex root def 100} {
    list [catch {myTree lindex root def 100} msg] $msg
} {0 {}}

test tree.286 {myTree unset root ghi} {
    list [catch {myTree unset root ghi} msg] $msg
} {0 {}}

test tree.287 {myTree lappend root ghi} {
    list [catch {myTree lappend root ghi} msg] $msg
} {0 {}}

test tree.288 {myTree lindex root ghi 0} {
    list [catch {myTree lindex root ghi 0} msg] $msg
} {0 {}}

test tree.289 {myTree lindex root myArr 1} {
    list [catch {myTree lindex root myArr 1} msg] $msg
} {0 {a b c d e}}

test tree.290 {myTree lindex root myArr(0) 0} {
    list [catch {myTree lindex root myArr(0) 0} msg] $msg
} {0 a}

test tree.291 {myTree lindex root myArr(0) end} {
    list [catch {myTree lindex root myArr(0) end} msg] $msg
} {0 e}

test tree.292 {myTree linsert} {
    list [catch {myTree linsert} msg] $msg
} {1 {wrong # args: should be "myTree linsert nodeName valueName index ?value...?"}}

test tree.293 {myTree linsert badNode} {
    list [catch {myTree linsert badNode} msg] $msg
} {1 {wrong # args: should be "myTree linsert nodeName valueName index ?value...?"}}

test tree.294 {myTree linsert badNode badValue} {
    list [catch {myTree linsert badNode badValue} msg] $msg
} {1 {wrong # args: should be "myTree linsert nodeName valueName index ?value...?"}}

test tree.295 {myTree linsert badNode badValue 0} {
    list [catch {myTree linsert badNode badValue 0} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.296 {myTree linsert root badValue 0} {
    list [catch {myTree linsert root badValue 0} msg] $msg
} {1 {can't find a value "badValue" in tree "::myTree"}}

test tree.297 {myTree linsert root def badIndex} {
    list [catch {myTree linsert root def badIndex} msg] $msg
} {1 {expected integer but got "badIndex"}}

test tree.299 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 {0 1 2 3 4}}

test tree.300 {myTree linsert root def 0 zero} {
    list [catch {myTree linsert root def 0 zero} msg] $msg
} {0 {}}

test tree.301 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 {zero 0 1 2 3 4}}

test tree.302 {myTree linsert root def 0 x y z} {
    list [catch {myTree linsert root def 0 x y z} msg] $msg
} {0 {}}

test tree.303 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 {x y z zero 0 1 2 3 4}}

test tree.304 {myTree linsert root def end 5} {
    list [catch {myTree linsert root def end 5} msg] $msg
} {0 {}}

test tree.305 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 {x y z zero 0 1 2 3 5 4}}

test tree.306 {myTree linsert root def end} {
    list [catch {myTree linsert root def end} msg] $msg
} {0 {}}

test tree.307 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 {x y z zero 0 1 2 3 5 4}}

test tree.308 {myTree get root myArr(0)} {
    list [catch {myTree get root myArr(0)} msg] $msg
} {0 {a b c d e}}

test tree.309 {myTree linsert root myArr(0) 0 x y z } {
    list [catch {myTree linsert root myArr(0) 0 x y z} msg] $msg
} {0 {}}

test tree.310 {myTree get root myArr(0)} {
    list [catch {myTree get root myArr(0)} msg] $msg
} {0 {x y z a b c d e}}

test tree.311 {myTree set root myArr(0)} {
    list [catch {myTree set root myArr(0) {a b c d e}} msg] $msg
} {0 {}}

test tree.312 {myTree linsert root myArr2(0) 0 a b c } {
    list [catch {myTree linsert root myArr2(0) 0 a b c } msg] $msg
} {1 {can't find a value "myArr2(0)" in tree "::myTree"}}

test tree.313 {myTree linsert root myScalar 0 a b c } {
    list [catch {myTree linsert root myScalar 0 a b c } msg] $msg
} {1 {can't find a value "myScalar" in tree "::myTree"}}

test tree.314 {myTree set root def "0 1 2 3 4"} {
    list [catch {myTree set root def "0 1 2 3 4"} msg] $msg
} {0 {}}

test tree.315 {myTree llength} {
    list [catch {myTree llength} msg] $msg
} {1 {wrong # args: should be "myTree llength nodeName valueName"}}

test tree.316 {myTree llength root} {
    list [catch {myTree llength root} msg] $msg
} {1 {wrong # args: should be "myTree llength nodeName valueName"}}

test tree.317 {myTree llength badNode def} {
    list [catch {myTree llength badNode def} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.318 {myTree llength root badValue} {
    list [catch {myTree llength root badValue} msg] $msg
} {1 {can't find a value "badValue" in tree "::myTree"}}

test tree.319 {myTree llength root def} {
    list [catch {myTree llength root def} msg] $msg
} {0 5}

test tree.320 {myTree llength root myArr} {
    list [catch {myTree llength root myArr} msg] $msg
} {0 2}

test tree.321 {myTree llength root ghi} {
    list [catch {myTree llength root ghi} msg] $msg
} {0 0}

test tree.322 {myTree llength root myArr(0)} {
    list [catch {myTree llength root myArr(0)} msg] $msg
} {0 5}

test tree.323 {myTree lrange root} {
    list [catch {myTree lrange root} msg] $msg
} {1 {wrong # args: should be "myTree lrange nodeName valueName first last"}}

test tree.324 {myTree lrange root def} {
    list [catch {myTree lrange root def} msg] $msg
} {1 {wrong # args: should be "myTree lrange nodeName valueName first last"}}

test tree.325 {myTree lrange badNode def 0 0} {
    list [catch {myTree lrange badNode def 0 0} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.326 {myTree lrange root badValue 0 0} {
    list [catch {myTree lrange root badValue 0 0} msg] $msg
} {1 {can't find a value "badValue" in tree "::myTree"}}

test tree.327 {myTree lrange root def badIndex 0} {
    list [catch {myTree lrange root def badIndex 0} msg] $msg
} {1 {expected integer but got "badIndex"}}

test tree.328 {myTree lrange root def 0 badIndex} {
    list [catch {myTree lrange root def 0 badIndex} msg] $msg
} {1 {expected integer but got "badIndex"}}

test tree.329 {myTree lrange root def 0 0} {
    list [catch {myTree lrange root def 0 0} msg] $msg
} {0 0}

test tree.330 {myTree lrange root def end end} {
    list [catch {myTree lrange root def end end} msg] $msg
} {0 4}

test tree.331 {myTree lrange root def 0 end} {
    list [catch {myTree lrange root def 0 end} msg] $msg
} {0 {0 1 2 3 4}}

test tree.332 {myTree lrange root def -1 end} {
    list [catch {myTree lrange root def -1 end} msg] $msg
} {1 {bad value "-1": can't be negative}}

test tree.333 {myTree lrange root def 100 100} {
    list [catch {myTree lrange root def 100 100} msg] $msg
} {0 {}}

test tree.334 {myTree lrange root def 0 100} {
    list [catch {myTree lrange root def 0 100} msg] $msg
} {0 {0 1 2 3 4}}

test tree.335 {myTree lreplace} {
    list [catch {myTree lreplace} msg] $msg
} {1 {wrong # args: should be "myTree lreplace nodeName valueName first last ?value...?"}}

test tree.336 {myTree lreplace badNode} {
    list [catch {myTree lreplace badNode} msg] $msg
} {1 {wrong # args: should be "myTree lreplace nodeName valueName first last ?value...?"}}

test tree.337 {myTree lreplace badNode badValue} {
    list [catch {myTree lreplace badNode badValue} msg] $msg
} {1 {wrong # args: should be "myTree lreplace nodeName valueName first last ?value...?"}}

test tree.338 {myTree lreplace badNode badValue 0 0} {
    list [catch {myTree lreplace badNode badValue 0 0} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.339 {myTree lreplace root badValue 0 0} {
    list [catch {myTree lreplace root badValue 0 0} msg] $msg
} {1 {can't find a value "badValue" in tree "::myTree"}}

test tree.340 {myTree lreplace root def badIndex 0} {
    list [catch {myTree lreplace root def badIndex 0} msg] $msg
} {1 {expected integer but got "badIndex"}}

test tree.341 {myTree lreplace root def 0 badIndex} {
    list [catch {myTree lreplace root def 0 badIndex} msg] $msg
} {1 {expected integer but got "badIndex"}}

test tree.342 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 {0 1 2 3 4}}

test tree.343 {myTree lreplace root def 0 0 zero} {
    list [catch {myTree lreplace root def 0 0 zero} msg] $msg
} {0 {}}

test tree.344 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 {zero 1 2 3 4}}

test tree.345 {myTree lreplace root def 0 0 x y z} {
    list [catch {myTree lreplace root def 0 0 x y z} msg] $msg
} {0 {}}

test tree.346 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 {x y z 1 2 3 4}}

test tree.347 {myTree lreplace root def 0 2} {
    list [catch {myTree lreplace root def 0 2} msg] $msg
} {0 {}}

test tree.348 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 {1 2 3 4}}

test tree.349 {myTree lreplace root def end end} {
    list [catch {myTree lreplace root def end end} msg] $msg
} {0 {}}

test tree.350 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 {1 2 3}}

test tree.351 {myTree get root myArr(0)} {
    list [catch {myTree get root myArr(0)} msg] $msg
} {0 {a b c d e}}

test tree.352 {myTree lreplace root myArr(0) 0 0 x y z } {
    list [catch {myTree lreplace root myArr(0) 0 0 x y z} msg] $msg
} {0 {}}

test tree.353 {myTree get root myArr(0)} {
    list [catch {myTree get root myArr(0)} msg] $msg
} {0 {x y z b c d e}}

test tree.354 {myTree set root myArr(0)} {
    list [catch {myTree set root myArr(0) {a b c d e}} msg] $msg
} {0 {}}

test tree.355 {myTree lreplace root myArr2(0) 0 0 a b c } {
    list [catch {myTree lreplace root myArr2(0) 0 0 a b c } msg] $msg
} {1 {can't find a value "myArr2(0)" in tree "::myTree"}}

test tree.356 {myTree lreplace root myScalar 0 0 a b c } {
    list [catch {myTree lreplace root myScalar 0 0 a b c } msg] $msg
} {1 {can't find a value "myScalar" in tree "::myTree"}}

test tree.357 {myTree set root def "0 1 2 3 4"} {
    list [catch {myTree set root def "0 1 2 3 4"} msg] $msg
} {0 {}}

test tree.358 {myTree label} {
    list [catch {myTree label} msg] $msg
} {1 {wrong # args: should be "myTree label nodeName ?newLabel?"}}


test tree.359 {myTree label badNode} {
    list [catch {myTree label badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.360 {myTree label badNode newLabel extraArg} {
    list [catch {myTree label badNode newLabel extraArg} msg] $msg
} {1 {wrong # args: should be "myTree label nodeName ?newLabel?"}}

test tree.361 {myTree label root} {
    list [catch {myTree label root} msg] $msg
} {0 {}}

test tree.362 {myTree label root abc} {
    list [catch {myTree label root abc} msg] $msg
} {0 abc}

test tree.363 {myTree label root} {
    list [catch {myTree label root} msg] $msg
} {0 abc}

test tree.364 {myTree exists} {
    list [catch {myTree exists} msg] $msg
} {1 {wrong # args: should be "myTree exists nodeName ?valueName?"}}

test tree.365 {myTree exists badNode} {
    list [catch {myTree exists badNode} msg] $msg
} {0 0}

test tree.366 {myTree exists badNode badValue} {
    list [catch {myTree exists badNode badValue} msg] $msg
} {0 0}

test tree.367 {myTree exists root} {
    list [catch {myTree exists root} msg] $msg
} {0 1}

test tree.368 {myTree exists root ghi} {
    list [catch {myTree exists root ghi} msg] $msg
} {0 1}

test tree.369 {myTree exists root myArr} {
    list [catch {myTree exists root myArr} msg] $msg
} {0 1}

test tree.370 {myTree exists root myArr(0)} {
    list [catch {myTree exists root myArr(0)} msg] $msg
} {0 1}

test tree.371 {myTree exists root myArr(badElem)} {
    list [catch {myTree exists root myArr(badElem)} msg] $msg
} {0 0}

test tree.372 {myTree exists root badValue} {
    list [catch {myTree exists root badValue} msg] $msg
} {0 0}

test tree.373 {myTree unset root ghi} {
    list [catch {myTree unset root ghi} msg] $msg
} {0 {}}

test tree.374 {myTree degree} {
    list [catch {myTree degree} msg] $msg
} {1 {wrong # args: should be "myTree degree nodeName"}}

test tree.375 {myTree degree badNode} {
    list [catch {myTree degree badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.376 {myTree degree badNode extraArg} {
    list [catch {myTree degree badNode extraArg} msg] $msg
} {1 {wrong # args: should be "myTree degree nodeName"}}

test tree.377 {myTree degree root} {
    list [catch {myTree degree root} msg] $msg
} {0 0}

test tree.378 {myTree degree 1} {
    list [catch {myTree degree 1} msg] $msg
} {1 {can't find tag or id "1" in ::myTree}}

test tree.379 {myTree insert 0} {
    list [catch {myTree insert 0 -label child1} msg] $msg
} {0 1}

test tree.380 {myTree degree 1} {
    list [catch {myTree degree 1} msg] $msg
} {0 0}

test tree.381 {myTree degree root} {
    list [catch {myTree degree root} msg] $msg
} {0 1}

test tree.382 {myTree findchild badNode} {
    list [catch {myTree findchild badNode} msg] $msg
} {1 {wrong # args: should be "myTree findchild nodeName label"}}

test tree.383 {myTree findchild badNode badChild extraArg} {
    list [catch {myTree findchild badNode badChild extraArg} msg] $msg
} {1 {wrong # args: should be "myTree findchild nodeName label"}}

test tree.384 {myTree findchild root badChild } {
    list [catch {myTree findchild root badChild} msg] $msg
} {0 -1}

test tree.385 {myTree findchild root child1} {
    list [catch {myTree findchild root child1} msg] $msg
} {0 1}

test tree.386 {myTree findchild root 1} {
    list [catch {myTree findchild root 1} msg] $msg
} {0 -1}

test tree.387 {myTree findchild all child1} {
    list [catch {myTree findchild all child1} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.388 {myTree names} {
    list [catch {myTree names} msg] $msg
} {1 {wrong # args: should be "myTree names nodeName ?valueName?"}}

test tree.389 {myTree names badNode} {
    list [catch {myTree names badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.390 {myTree names root} {
    list [catch {myTree names root} msg] $msg
} {0 {abc def myArr}}

test tree.391 {myTree names all} {
    list [catch {myTree names all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.392 {myTree get root myArr} {
    list [catch {myTree get root myArr} msg] $msg
} {0 {0 {a b c d e}}}

test tree.393 {myTree names root badValue} {
    list [catch {myTree names root badValue} msg] $msg
} {1 {can't find a value "badValue" in tree "::myTree"}}

test tree.394 {myTree names root badValue extraArg} {
    list [catch {myTree names root badValue extraArg} msg] $msg
} {1 {wrong # args: should be "myTree names nodeName ?valueName?"}}

test tree.395 {myTree names root myArr} {
    list [catch {myTree names root myArr} msg] $msg
} {0 0}

test tree.396 {myTree type} {
    list [catch {myTree type} msg] $msg
} {1 {wrong # args: should be "myTree type nodeName valueName"}}

test tree.397 {myTree type badNode} {
    list [catch {myTree type badNode} msg] $msg
} {1 {wrong # args: should be "myTree type nodeName valueName"}}

test tree.398 {myTree type badNode badValue} {
    list [catch {myTree type badNode badValue} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.399 {myTree type root badValue} {
    list [catch {myTree type root badValue} msg] $msg
} {1 {can't find a value "badValue" in tree "::myTree"}}

test tree.400 {myTree type root myArr} {
    list [catch {myTree type root myArr} msg] $msg
} {0 blt_array}

test tree.401 {myTree set root myNumber} {
    list [catch {myTree set root myNumber 1.0} msg] $msg
} {0 {}}

test tree.402 {myTree type root myNumber} {
    list [catch {myTree type root myNumber} msg] $msg
} {0 string}

test tree.403 {myTree set root myNumber} {
    list [catch {myTree set root myNumber [expr 1.0]} msg] $msg
} {0 {}}

test tree.404 {myTree type root myNumber} {
    list [catch {myTree type root myNumber} msg] $msg
} {0 double}

test tree.405 {myTree set root myNumber} {
    list [catch {myTree set root myNumber [expr 10000]} msg] $msg
} {0 {}}

test tree.406 {myTree type root myNumber} {
    list [catch {myTree type root myNumber} msg] $msg
} {0 int}

test tree.407 {myTree set root myNumber} {
    list [catch {myTree set root myNumber [expr yes]} msg] $msg
} {0 {}}

test tree.408 {myTree type root myNumber} {
    list [catch {myTree type root myNumber} msg] $msg
} {0 booleanString}

test tree.409 {myTree tag add myTag root} {
    list [catch {myTree tag add myTag root} msg] $msg
} {0 {}}

test tree.410 {myTree dup} {
    list [catch {myTree dup} msg] $msg
} {1 {wrong # args: should be "myTree dup nodeName"}}

test tree.411 {myTree dup badNode} {
    list [catch {myTree dup badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.412 {myTree dup root} {
    list [catch {myTree dup root} msg] $msg
} {0 ::tree1}

test tree.413 {myTree dump root} {
    list [catch {myTree dump root} msg] $msg
} {0 {# V3.0
n abc -1 0
d abc 0123456789ABCDEF
d def {0 1 2 3 4}
d myArr {0 {a b c d e}}
d myNumber yes
t myTag
n child1 0 1
}}

test tree.414 {::tree1 dump root} {
    list [catch {::tree1 dump root} msg] $msg
} {0 {# V3.0
n abc -1 0
d abc 0123456789ABCDEF
d def {0 1 2 3 4}
d myArr {0 {a b c d e}}
d myNumber yes
t myTag
n child1 0 1
}}

test tree.415 {myTree export badFormat} {
    list [catch {myTree export badFormat} msg] $msg
} {1 {can't export "badFormat": format not registered}}

test tree.416 {myTree import badFormat} {
    list [catch {myTree import badFormat} msg] $msg
} {1 {can't import "badFormat": format not registered}}

test tree.417 {package require blt_tree_xml} {
    list [catch {package require blt_tree_xml} msg] $msg
} {0 3.0}

test tree.418 {package require blt_tree_json} {
    list [catch {package require blt_tree_json} msg] $msg
} {0 3.0}

test tree.419 {myTree export badFmt} {
    list [catch {myTree export badFmt} msg] $msg
} {1 {can't export "badFmt": format not registered}}

test tree.420 {myTree export json} {
    list [catch {myTree export json} msg] $msg
} {0 {{
  "abc" : "0123456789ABCDEF", 
  "def" : "0 1 2 3 4", 
  "myArr" : "0 {a b c d e}", 
  "myNumber" : "yes", 
  "child1" : {
  }
}
}}

test tree.421 {myTree export json -help} {
    list [catch {myTree export json -help} msg] $msg
} {1 {The following switches are available:
   -data data
   -file fileName
   -root node}}

test tree.422 {myTree export json -badSwitch} {
    list [catch {myTree export json -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -data data
   -file fileName
   -root node}}

test tree.423 {myTree export json -root badNode} {
    list [catch {myTree export json -root badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.424 {myTree export json -root (missing arg)} {
    list [catch {myTree export json -root} msg] $msg
} {1 {value for "-root" missing}}

test tree.425 {myTree export json -root 1} {
    list [catch {myTree export json -root 1} msg] $msg
} {0 {{
}
}}

test tree.426 {myTree export json -file (missing arg)} {
    list [catch {myTree export json -file} msg] $msg
} {1 {value for "-file" missing}}

test tree.427 {myTree export json -file badDir/badFile} {
    list [catch {myTree export json -file badDir/badFile} msg] $msg
} {1 {couldn't open "badDir/badFile": no such file or directory}}

MakeTestFile noPerms.json 0500 ""

test tree.428 {myTree export json -file testDir/noPerms.json} {
    list [catch {
	myTree export json -file testDir/noPerms.json
    } msg] $msg
} {1 {couldn't open "testDir/noPerms.json": permission denied}}

file delete testDir/noPerms.json

test tree.429 {myTree export json -file testDir/myTree.json} {
    list [catch {
	myTree export json -file testDir/myTree.json
	ReadAndDeleteFile testDir/myTree.json
    } msg] $msg
} {0 {{
  "abc" : "0123456789ABCDEF", 
  "def" : "0 1 2 3 4", 
  "myArr" : "0 {a b c d e}", 
  "myNumber" : "yes", 
  "child1" : {
  }
}
}}

test tree.430 {myTree export json -data (missing arg)} {
    list [catch {myTree export json -data} msg] $msg
} {1 {value for "-data" missing}}

test tree.431 {myTree export json -data badNs::badVar} {
    list [catch {myTree export json -data badNs::badVar} msg] $msg
} {1 {can't set "badNs::badVar": parent namespace doesn't exist}}

set badVar(0) 1
test tree.432 {myTree export json -data badVar} {
    list [catch {myTree export json -data badVar} msg] $msg 
} {1 {can't set "badVar": variable is array}}

test tree.433 {myTree export json -data myData} {
    list [catch {
	myTree export json -data myData
	set myData
    } msg] $msg
} {0 {{
  "abc" : "0123456789ABCDEF", 
  "def" : "0 1 2 3 4", 
  "myArr" : "0 {a b c d e}", 
  "myNumber" : "yes", 
  "child1" : {
  }
}
}}


test tree.434 {myTree export xml -help} {
    list [catch {myTree export xml -help} msg] $msg
} {1 {The following switches are available:
   -data data
   -declaration 
   -file fileName
   -hideroot 
   -indent number
   -root node}}

test tree.435 {myTree export xml -badSwitch} {
    list [catch {myTree export xml -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -data data
   -declaration 
   -file fileName
   -hideroot 
   -indent number
   -root node}}

test tree.436 {myTree export xml -root badNode} {
    list [catch {myTree export xml -root badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.437 {myTree export xml -root (missing arg)} {
    list [catch {myTree export xml -root} msg] $msg
} {1 {value for "-root" missing}}

test tree.438 {myTree export xml} {
    list [catch {myTree export xml} msg] $msg
} {0 {
<abc abc="0123456789ABCDEF" def="0 1 2 3 4" myArr="0 {a b c d e}" myNumber="yes">
 <child1></child1>
</abc>
}}

test tree.439 {myTree export xml -root 1} {
    list [catch {myTree export xml -root 1} msg] $msg
} {0 {
 <child1></child1>
}}

test tree.440 {myTree export xml -file (missing arg)} {
    list [catch {myTree export xml -file} msg] $msg
} {1 {value for "-file" missing}}

test tree.441 {myTree export xml -file badDir/badFile} {
    list [catch {myTree export xml -file badDir/badFile} msg] $msg
} {1 {couldn't open "badDir/badFile": no such file or directory}}

MakeTestFile noPerms.xml 0500 ""

test tree.442 {myTree export xml -file testDir/noPerms.xml} {
    list [catch {
	myTree export xml -file testDir/noPerms.xml
    } msg] $msg
} {1 {couldn't open "testDir/noPerms.xml": permission denied}}

file delete testDir/noPerms.xml

test tree.443 {myTree export xml -file testDir/myTree.xml} {
    list [catch {
	myTree export xml -file testDir/myTree.xml
	ReadAndDeleteFile testDir/myTree.xml
    } msg] $msg
} {0 {
<abc abc="0123456789ABCDEF" def="0 1 2 3 4" myArr="0 {a b c d e}" myNumber="yes">
 <child1></child1>
</abc>
}}

test tree.444 {myTree export xml -data (missing arg)} {
    list [catch {myTree export xml -data} msg] $msg
} {1 {value for "-data" missing}}

test tree.445 {myTree export xml -data badNs::badVar} {
    list [catch {myTree export xml -data badNs::badVar} msg] $msg
} {1 {can't set "badNs::badVar": parent namespace doesn't exist}}

set badVar(0) 1
test tree.446 {myTree export xml -data badVar} {
    list [catch {myTree export xml -data badVar} msg] $msg 
} {1 {can't set "badVar": variable is array}}

test tree.447 {myTree export xml -data myData} {
    list [catch {
	myTree export xml -data myData
	set myData
    } msg] $msg
} {0 {
<abc abc="0123456789ABCDEF" def="0 1 2 3 4" myArr="0 {a b c d e}" myNumber="yes">
 <child1></child1>
</abc>
}}

test tree.448 {myTree export xml -badSwitch} {
    list [catch {myTree export xml -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -data data
   -declaration 
   -file fileName
   -hideroot 
   -indent number
   -root node}}

# FIXME: don't think values for root should be returned.
test tree.449 {myTree export xml -hideroot} {
    list [catch {myTree export xml -hideroot} msg] $msg
} {0 {  abc="0123456789ABCDEF" def="0 1 2 3 4" myArr="0 {a b c d e}" myNumber="yes"
<child1></child1> 
}}

test tree.450 {myTree export xml -declaration} {
    list [catch {myTree export xml -declaration} msg] $msg
} {0 {<?xml version='1.0' encoding='utf-8'?>
<abc abc="0123456789ABCDEF" def="0 1 2 3 4" myArr="0 {a b c d e}" myNumber="yes">
 <child1></child1>
</abc>
}}

test tree.451 {myTree export xml -indent 4} {
    list [catch {myTree export xml -indent 4} msg] $msg
} {0 {
<abc abc="0123456789ABCDEF" def="0 1 2 3 4" myArr="0 {a b c d e}" myNumber="yes">
    <child1></child1>
</abc>
}}

test tree.452 {myTree import} {
    list [catch {myTree import} msg] $msg
} {0 {xml json}}

test tree.453 {myTree import badFmt} {
    list [catch {myTree import badFmt} msg] $msg
} {1 {can't import "badFmt": format not registered}}

test tree.454 {myTree import json} {
    list [catch {myTree import json} msg] $msg
} {1 {must specify either -file or -data switch}}

test tree.455 {blt::tree destroy ::tree1} {
    list [catch {blt::tree destroy ::tree1} msg] $msg
} {0 {}}


test tree.456 {tree0 restore stuff} {
    list [catch {
	set data [tree0 dump root -version 2.0]
	blt::tree create
	tree1 restore root -data $data
	set data [tree1 dump root -version 2.0]
	blt::tree destroy tree1
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

test tree.457 {tree0 restore 0 -file test.dump} {
    list [catch {
	blt::tree create
	tree1 restore root -file test.dump
	set data [tree1 dump root -version 2.0]
	blt::tree destroy tree1
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


test tree.458 {tree0 unset 0 key1} {
    list [catch {tree0 unset 0 key1} msg] $msg
} {0 {}}

test tree.459 {tree0 get 0} {
    list [catch {tree0 get 0} msg] $msg
} {0 {key value key2 value2 key3 value3 abc 123}}

test tree.460 {tree0 unset 0 key2 key3} {
    list [catch {tree0 unset 0 key2 key3} msg] $msg
} {0 {}}

test tree.461 {tree0 get 0} {
    list [catch {tree0 get 0} msg] $msg
} {0 {key value abc 123}}

test tree.462 {tree0 unset 0} {
    list [catch {tree0 unset 0} msg] $msg
} {0 {}}

test tree.463 {tree0 unset 0 (all)} {
    list [catch {eval tree0 unset 0 [tree0 names 0]} msg] $msg
} {0 {}}

test tree.464 {tree0 get 0} {
    list [catch {tree0 get 0} msg] $msg
} {0 {}}

test tree.465 {tree0 unset all abc} {
    list [catch {tree0 unset all abc} msg] $msg
} {0 {}}

test tree.466 {tree0 restore stuff} {
    list [catch {
	set data [tree0 dump root -version 2.0]
	blt::tree create tree1
	tree1 restore root -data $data
	set data [tree1 dump root -version 2.0]
	blt::tree destroy tree1
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

test tree.467 {tree0 restore (missing arg)} {
    list [catch {tree0 restore} msg] $msg
} {1 {wrong # args: should be "tree0 restore nodeName ?switches ...?"}}

test tree.468 {tree0 restore 0 badSwitch} {
    list [catch {tree0 restore 0 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -data data
   -file fileName
   -notags 
   -overwrite }}


test tree.469 {tree0 restore 0 {} arg (extra arg)} {
    list [catch {tree0 restore 0 {} arg} msg] $msg
} {1 {unknown switch ""
The following switches are available:
   -data data
   -file fileName
   -notags 
   -overwrite }}


test tree.470 {tree0 size (missing arg)} {
    list [catch {tree0 size} msg] $msg
} {1 {wrong # args: should be "tree0 size nodeName"}}

test tree.471 {tree0 size 0} {
    list [catch {tree0 size 0} msg] $msg
} {0 12}

test tree.472 {tree0 size all} {
    list [catch {tree0 size all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.473 {tree0 size 0 10 (extra arg)} {
    list [catch {tree0 size 0 10} msg] $msg
} {1 {wrong # args: should be "tree0 size nodeName"}}

test tree.474 {tree0 delete (no args)} {
    list [catch {tree0 delete} msg] $msg
} {0 {}}

test tree.475 {tree0 delete badNode} {
    list [catch {tree0 delete badNode} msg] $msg
} {1 {can't find tag or id "badNode" to delete in ::tree0}}

test tree.476 {tree0 delete 11} {
    list [catch {tree0 delete 11} msg] $msg
} {0 {}}

test tree.477 {tree0 delete 11} {
    list [catch {tree0 delete 11} msg] $msg
} {1 {can't find tag or id "11" in ::tree0}}

test tree.478 {tree0 delete 9 12} {
    list [catch {tree0 delete 9 12} msg] $msg
} {0 {}}

test tree.479 {tree0 dump 0 -version 2.0} {
    list [catch {tree0 dump 0 -version 2.0} msg] $msg
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

test tree.480 {delete all} {
    list [catch {
	set data [tree0 dump root -version 2.0]
	blt::tree create
	tree1 restore root -data $data
	tree1 delete all
	set data [tree1 dump root -version 2.0]
	blt::tree destroy tree1
	set data
	} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
}}

test tree.481 {delete all all} {
    list [catch {
	set data [tree0 dump root -version 2.0]
	blt::tree create
	tree1 restore root -data $data
	tree1 delete all all
	set data [tree1 dump root -version 2.0]
	blt::tree destroy tree1
	set data
	} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
}}

test tree.482 {tree0 apply (missing arg)} {
    list [catch {tree0 apply} msg] $msg
} {1 {wrong # args: should be "tree0 apply nodeName ?switches ...?"}}

test tree.483 {tree0 apply 0} {
    list [catch {tree0 apply 0} msg] $msg
} {0 {}}

test tree.484 {tree0 apply 0 -badSwitch} {
    list [catch {tree0 apply 0 -badSwitch} msg] $msg
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

test tree.485 {tree0 apply badTag} {
    list [catch {tree0 apply badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::tree0}}

test tree.486 {tree0 apply all} {
    list [catch {tree0 apply all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.487 {tree0 apply myTag -precommand lappend} {
    list [catch {
	set mylist {}
	tree0 apply myTag -precommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {5 13}}

test tree.488 {tree0 apply root -precommand lappend} {
    list [catch {
	set mylist {}
	tree0 apply root -precommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {0 1 2 3 4 5 13 6 8}}

test tree.489 {tree0 apply -postcommand} {
    list [catch {
	set mylist {}
	tree0 apply root -postcommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.490 {tree0 apply -precommand -postcommand} {
    list [catch {
	set mylist {}
	tree0 apply root -precommand {lappend mylist} \
		-postcommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {0 1 1 2 2 3 3 4 4 5 13 13 5 6 6 8 8 0}}

test tree.491 {tree0 apply root -precommand lappend -depth 1} {
    list [catch {
	set mylist {}
	tree0 apply root -precommand {lappend mylist} -depth 1
	set mylist
    } msg] $msg
} {0 {0 1 2 3 4 5 6 8}}


test tree.492 {tree0 apply root -precommand -depth 0} {
    list [catch {
	set mylist {}
	tree0 apply root -precommand {lappend mylist} -depth 0
	set mylist
    } msg] $msg
} {0 0}

test tree.493 {tree0 apply root -precommand -tag myTag} {
    list [catch {
	set mylist {}
	tree0 apply root -precommand {lappend mylist} -tag myTag
	set mylist
    } msg] $msg
} {0 5}


test tree.494 {tree0 apply root -precommand -key key1} {
    list [catch {
	set mylist {}
	tree0 set myTag key1 0.0
	tree0 apply root -precommand {lappend mylist} -key key1
	tree0 unset myTag key1
	set mylist
    } msg] $msg
} {0 5}

test tree.495 {tree0 apply root -postcommand -regexp node.*} {
    list [catch {
	set mylist {}
	tree0 set myTag key1 0.0
	tree0 apply root -precommand {lappend mylist} -regexp {node5} 
	tree0 unset myTag key1
	set mylist
    } msg] $msg
} {0 5}

test tree.496 {tree0 find (missing arg)} {
    list [catch {tree0 find} msg] $msg
} {1 {wrong # args: should be "tree0 find nodeName ?switches ...?"}}

test tree.497 {tree0 find 0} {
    list [catch {tree0 find 0} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.498 {tree0 find root} {
    list [catch {tree0 find root} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.499 {tree0 find 0 -glob node*} {
    list [catch {tree0 find root -glob node*} msg] $msg
} {0 {1 2 3 4 13 5 6}}

test tree.500 {tree0 find 0 -glob nobody} {
    list [catch {tree0 find root -glob nobody} msg] $msg
} {0 {}}

test tree.501 {tree0 find 0 -regexp {node[0-3]}} {
    list [catch {tree0 find root -regexp {node[0-3]}} msg] $msg
} {0 {1 2 3 13}}

test tree.502 {tree0 find 0 -regexp {.*[A-Z].*}} {
    list [catch {tree0 find root -regexp {.*[A-Z].*}} msg] $msg
} {0 8}

test tree.503 {tree0 find 0 -exact myLabel} {
    list [catch {tree0 find root -exact myLabel} msg] $msg
} {0 8}

test tree.504 {tree0 find 0 -exact myLabel -invert} {
    list [catch {tree0 find root -exact myLabel -invert} msg] $msg
} {0 {1 2 3 4 13 5 6 0}}


test tree.505 {tree0 find 3 -exact node3} {
    list [catch {tree0 find 3 -exact node3} msg] $msg
} {0 3}

test tree.506 {tree0 find 0 -nocase -exact mylabel} {
    list [catch {tree0 find 0 -nocase -exact mylabel} msg] $msg
} {0 8}

test tree.507 {tree0 find 0 -nocase} {
    list [catch {tree0 find 0 -nocase} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.508 {tree0 find 0 -path -nocase -glob *node1* } {
    list [catch {tree0 find 0 -path -nocase -glob *node1*} msg] $msg
} {0 {1 13}}

test tree.509 {tree0 find 0 -count 5 } {
    list [catch {tree0 find 0 -count 5} msg] $msg
} {0 {1 2 3 4 13}}

test tree.510 {tree0 find 0 -count -5 } {
    list [catch {tree0 find 0 -count -5} msg] $msg
} {1 {bad value "-5": can't be negative}}

test tree.511 {tree0 find 0 -count badValue } {
    list [catch {tree0 find 0 -count badValue} msg] $msg
} {1 {expected integer but got "badValue"}}

test tree.512 {tree0 find 0 -count badValue } {
    list [catch {tree0 find 0 -count badValue} msg] $msg
} {1 {expected integer but got "badValue"}}

test tree.513 {tree0 find 0 -leafonly} {
    list [catch {tree0 find 0 -leafonly} msg] $msg
} {0 {1 2 3 4 13 6 8}}

test tree.514 {tree0 find 0 -leafonly -glob {node[18]}} {
    list [catch {tree0 find 0 -glob {node[18]} -leafonly} msg] $msg
} {0 1}

test tree.515 {tree0 find 0 -depth 0} {
    list [catch {tree0 find 0 -depth 0} msg] $msg
} {0 0}

test tree.516 {tree0 find 0 -depth 1} {
    list [catch {tree0 find 0 -depth 1} msg] $msg
} {0 {1 2 3 4 5 6 8 0}}

test tree.517 {tree0 find 0 -depth 2} {
    list [catch {tree0 find 0 -depth 2} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.518 {tree0 find 0 -depth 20} {
    list [catch {tree0 find 0 -depth 20} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.519 {tree0 find 1 -depth 0} {
    list [catch {tree0 find 1 -depth 0} msg] $msg
} {0 1}

test tree.520 {tree0 find 1 -depth 1} {
    list [catch {tree0 find 1 -depth 1} msg] $msg
} {0 1}

test tree.521 {tree0 find 1 -depth 2} {
    list [catch {tree0 find 1 -depth 2} msg] $msg
} {0 1}

test tree.522 {tree0 find all} {
    list [catch {tree0 find all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.523 {tree0 find badTag} {
    list [catch {tree0 find badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::tree0}}

test tree.524 {tree0 find 0 -addtag hi} {
    list [catch {tree0 find 0 -addtag hi} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.525 {tree0 find 0 -addtag all} {
    list [catch {tree0 find 0 -addtag all} msg] $msg
} {1 {can't add reserved tag "all"}}

test tree.526 {tree0 find 0 -addtag root} {
    list [catch {tree0 find 0 -addtag root} msg] $msg
} {1 {can't add reserved tag "root"}}

test tree.527 {tree0 find 0 -exec {lappend list} -leafonly} {
    list [catch {
	set list {}
	tree0 find 0 -exec {lappend list} -leafonly
	set list
	} msg] $msg
} {0 {1 2 3 4 13 6 8}}

test tree.528 {tree0 find 0 -tag root} {
    list [catch {tree0 find 0 -tag root} msg] $msg
} {0 0}

test tree.529 {tree0 find 0 -tag myTag} {
    list [catch {tree0 find 0 -tag myTag} msg] $msg
} {0 5}

test tree.530 {tree0 find 0 -tag badTag} {
    list [catch {tree0 find 0 -tag badTag} msg] $msg
} {0 {}}

test tree.531 {tree0 tag (missing args)} {
    list [catch {tree0 tag} msg] $msg
} {1 {wrong # args: should be "tree0 tag args ..."}}

test tree.532 {tree0 tag badOp} {
    list [catch {tree0 tag badOp} msg] $msg
} {1 {bad operation "badOp": should be one of...
  tree0 tag add tag ?nodeName...?
  tree0 tag delete tag nodeName...
  tree0 tag exists tag ?nodeName?
  tree0 tag forget tag...
  tree0 tag get nodeName ?pattern...?
  tree0 tag names ?nodeName...?
  tree0 tag nodes tag ?tag...?
  tree0 tag set nodeName tag...
  tree0 tag unset nodeName tag...}}

test tree.533 {tree0 tag add} {
    list [catch {tree0 tag add} msg] $msg
} {1 {wrong # args: should be "tree0 tag add tag ?nodeName...?"}}

test tree.534 {tree0 tag add newTag} {
    list [catch {tree0 tag add newTag} msg] $msg
} {0 {}}

test tree.535 {tree0 tag add tag badNode} {
    list [catch {tree0 tag add tag badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.536 {tree0 tag add newTag root} {
    list [catch {tree0 tag add newTag root} msg] $msg
} {0 {}}

test tree.537 {tree0 tag add newTag all} {
    list [catch {tree0 tag add newTag all} msg] $msg
} {0 {}}

test tree.538 {tree0 tag add tag2 0 1 2 3 4} {
    list [catch {tree0 tag add tag2 0 1 2 3 4} msg] $msg
} {0 {}}

test tree.539 {tree0 tag exists tag2} {
    list [catch {tree0 tag exists tag2} msg] $msg
} {0 1}

test tree.540 {tree0 tag exists tag2 0} {
    list [catch {tree0 tag exists tag2 0} msg] $msg
} {0 1}

test tree.541 {tree0 tag exists tag2 5} {
    list [catch {tree0 tag exists tag2 5} msg] $msg
} {0 0}

test tree.542 {tree0 tag exists badTag} {
    list [catch {tree0 tag exists badTag} msg] $msg
} {0 0}

test tree.543 {tree0 tag exists badTag 1000} {
    list [catch {tree0 tag exists badTag 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree0}}

test tree.544 {tree0 tag add tag2 0 1 2 3 4 1000} {
    list [catch {tree0 tag add tag2 0 1 2 3 4 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree0}}

test tree.545 {tree0 tag names} {
    list [catch {tree0 tag names} msg] [lsort $msg]
} {0 {all hi myTag myTag1 myTag2 newTag root tag2 thisTag}}

test tree.546 {tree0 tag names badNode} {
    list [catch {tree0 tag names badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.547 {tree0 tag names all} {
    list [catch {tree0 tag names all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.548 {tree0 tag names root} {
    list [catch {tree0 tag names root} msg] [lsort $msg]
} {0 {all hi newTag root tag2}}

test tree.549 {tree0 tag names 0 1} {
    list [catch {tree0 tag names 0 1} msg] [lsort $msg]
} {0 {all hi newTag root tag2}}

test tree.550 {tree0 tag nodes (missing arg)} {
    list [catch {tree0 tag nodes} msg] $msg
} {1 {wrong # args: should be "tree0 tag nodes tag ?tag...?"}}

test tree.551 {tree0 tag nodes root badTag} {
    # It's not an error to use bad tag.
    list [catch {tree0 tag nodes root badTag} msg] $msg
} {0 {}}

test tree.552 {tree0 tag nodes root tag2} {
    list [catch {tree0 tag nodes root tag2} msg] [lsort $msg]
} {0 {0 1 2 3 4}}

test tree.553 {tree0 tag set} {
    list [catch {tree0 tag set} msg] $msg
} {1 {wrong # args: should be "tree0 tag set nodeName tag..."}}

test tree.554 {tree0 tag set badNode} {
    list [catch {tree0 tag set badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.555 {tree0 tag set root root} {
    list [catch {tree0 tag set root root} msg] $msg
} {1 {can't add reserved tag "root"}}

test tree.556 {tree0 tag set root all} {
    list [catch {tree0 tag set root all} msg] $msg
} {1 {can't add reserved tag "all"}}

test tree.557 {tree0 tag set root 100} {
    list [catch {tree0 tag set root 100} msg] $msg
} {1 {bad tag "100": can't be a number}}

test tree.558 {tree0 tag set root myTag} {
    list [catch {tree0 tag set root myTag} msg] $msg
} {0 {}}

test tree.559 {tree0 tag get} {
    list [catch {tree0 tag get} msg] $msg
} {1 {wrong # args: should be "tree0 tag get nodeName ?pattern...?"}}

test tree.560 {tree0 tag get badNode} {
    list [catch {tree0 tag get badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.561 {tree0 tag get root badTag} {
    list [catch {tree0 tag get root badTag} msg] $msg
} {0 {}}

test tree.562 {tree0 tag get root} {
    list [catch {tree0 tag get root} msg] $msg
} {0 {root hi tag2 newTag myTag all}}

test tree.563 {tree0 tag get root myTag} {
    list [catch {tree0 tag get root myTag} msg] $msg
} {0 myTag}

test tree.564 {tree0 tag get root root} {
    list [catch {tree0 tag get root root} msg] $msg
} {0 root}

test tree.565 {tree0 tag get root all} {
    list [catch {tree0 tag get root all} msg] $msg
} {0 all}

test tree.566 {tree0 tag names root} {
    list [catch {tree0 tag names root} msg] $msg
} {0 {all root hi myTag newTag tag2}}

test tree.567 {tree0 tag unset} {
    list [catch {tree0 tag unset} msg] $msg
} {1 {wrong # args: should be "tree0 tag unset nodeName tag..."}}

test tree.568 {tree0 tag unset badNode} {
    list [catch {tree0 tag unset badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}


test tree.569 {tree0 tag unset root} {
    list [catch {tree0 tag unset root} msg] $msg
} {0 {}}

test tree.570 {tree0 tag unset root badTag} {
    list [catch {tree0 tag unset root badTag} msg] $msg
} {0 {}}

test tree.571 {tree0 tag names root} {
    list [catch {tree0 tag names root} msg] $msg
} {0 {all root hi myTag newTag tag2}}

test tree.572 {tree0 tag unset root myTag} {
    list [catch {tree0 tag unset root myTag} msg] $msg
} {0 {}}

test tree.573 {tree0 tag names root} {
    list [catch {tree0 tag names root} msg] $msg
} {0 {all root hi newTag tag2}}

test tree.574 {tree0 ancestor (missing arg)} {
    list [catch {tree0 ancestor} msg] $msg
} {1 {wrong # args: should be "tree0 ancestor node1 node2"}}

test tree.575 {tree0 ancestor 0 (missing arg)} {
    list [catch {tree0 ancestor 0} msg] $msg
} {1 {wrong # args: should be "tree0 ancestor node1 node2"}}

test tree.576 {tree0 ancestor 0 10} {
    list [catch {tree0 ancestor 0 10} msg] $msg
} {1 {can't find tag or id "10" in ::tree0}}

test tree.577 {tree0 ancestor 0 4} {
    list [catch {tree0 ancestor 0 4} msg] $msg
} {0 0}

test tree.578 {tree0 ancestor 1 8} {
    list [catch {tree0 ancestor 1 8} msg] $msg
} {0 0}

test tree.579 {tree0 ancestor root 0} {
    list [catch {tree0 ancestor root 0} msg] $msg
} {0 0}

test tree.580 {tree0 ancestor 8 8} {
    list [catch {tree0 ancestor 8 8} msg] $msg
} {0 8}

test tree.581 {tree0 ancestor 0 all} {
    list [catch {tree0 ancestor 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.582 {tree0 ancestor 7 9} {
    list [catch {
	set n1 1; set n2 1;
	for { set i 0 } { $i < 4 } { incr i } {
	    set n1 [tree0 insert $n1]
	    set n2 [tree0 insert $n2]
	}
	tree0 ancestor $n1 $n2
	} msg] $msg
} {0 1}

test tree.583 {tree0 path (missing arg)} {
    list [catch {tree0 path} msg] $msg
} {1 {wrong # args: should be "tree0 path ?args ...?"}}

test tree.584 {tree0 path badArg} {
    list [catch {tree0 path badArg} msg] $msg
} {1 {bad operation "badArg": should be one of...
  tree0 path create pathName ?switches ...?
  tree0 path parse pathName ?switches ...?
  tree0 path print nodeName ?switches ...?
  tree0 path separator ?sepString?}}

test tree.585 {tree0 path print root} {
    list [catch {tree0 path print root} msg] $msg
} {0 {}}

test tree.586 {tree0 path print 0} {
    list [catch {tree0 path print 0} msg] $msg
} {0 {}}

test tree.587 {tree0 path print 15} {
    list [catch {tree0 path print 15} msg] $msg
} {0 {node1 node15}}

test tree.588 {tree0 path print 15} {
    list [catch {tree0 path print 15 -separator /} msg] $msg
} {0 /node1/node15}

test tree.589 {tree0 path print 16} {
    list [catch {tree0 path print 16 -separator /} msg] $msg
} {0 /node1/node14/node16}

test tree.590 {tree0 path parse /} {
    list [catch {tree0 path parse / -separator /} msg] $msg
} {0 0}

test tree.591 {tree0 path parse /node1} {
    list [catch {tree0 path parse /node1 -separator /} msg] $msg
} {0 1}

test tree.592 {tree0 path parse /node1/node14} {
    list [catch {tree0 path parse /node1/node14 -separator /} msg] $msg
} {0 14}

test tree.593 {tree0 path parse } {
    list [catch {tree0 path parse /node1/node14/node16 -separator /} msg] $msg
} {0 16}

test tree.594 {tree0 path parse } {
    list [catch {tree0 path parse /node1/node14/node16/ -separator /} msg] $msg
} {0 16}

test tree.595 {tree0 path parse } {
    list [catch {tree0 path parse //node1//node14//node16// -separator /} msg] $msg
} {0 16}

test tree.596 {tree0 path parse } {
    list [catch {tree0 path parse ::node1::node14::node16 -separator ::} msg] $msg
} {0 16}

test tree.597 {tree0 path parse } {
    list [catch {tree0 path parse /node1/node14/node16 -separator /} msg] $msg
} {0 16}

test tree.598 {tree0 path parse } {
    list [catch {tree0 path parse /node1/node14/node16/ -separator /} msg] $msg
} {0 16}

test tree.599 {tree0 path parse } {
    list [catch {tree0 path parse //node1//node14//node16// -separator /} msg] $msg
} {0 16}

test tree.600 {tree0 path parse } {
    list [catch {tree0 path parse ::node1::node14::node16 -separator ::} msg] $msg
} {0 16}

test tree.601 {tree0 path parse ""} {
    list [catch {tree0 path parse {}} msg] $msg
} {0 0}

test tree.602 {tree0 path parse node1 } {
    list [catch {tree0 path parse node1} msg] $msg
} {0 1}

test tree.603 {tree0 path parse {node1 node14}} {
    list [catch {tree0 path parse {node1 node14}} msg] $msg
} {0 14}

test tree.604 {tree0 path parse } {
    list [catch {tree0 path parse {node1 node14 node16}} msg] $msg
} {0 16}


test tree.605 {tree0 path print all} {
    list [catch {tree0 path print all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.606 {tree0 path print 0 badSwitch} {
    list [catch {tree0 path print 0 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -from node
   -separator char
   -noleadingseparator }}


test tree.607 {tree0 tag forget} {
    list [catch {tree0 tag forget} msg] $msg
} {1 {wrong # args: should be "tree0 tag forget tag..."}}

test tree.608 {tree0 tag forget badTag} {
    list [catch {
	tree0 tag forget badTag
	lsort [tree0 tag names]
    } msg] $msg
} {0 {all hi myTag myTag1 myTag2 newTag root tag2 thisTag}}

test tree.609 {tree0 tag forget all} {
    list [catch {tree0 tag forget all} msg] $msg
} {1 {can't forget reserved tag "all"}}

test tree.610 {tree0 tag forget root} {
    list [catch {tree0 tag forget root} msg] $msg
} {1 {can't forget reserved tag "root"}}

test tree.611 {tree0 tag forget 100} {
    list [catch {tree0 tag forget 100} msg] $msg
} {1 {bad tag "100": can't be a number}}

test tree.612 {tree0 tag forget hi} {
    list [catch {
	tree0 tag forget hi
	lsort [tree0 tag names]
    } msg] $msg
} {0 {all myTag myTag1 myTag2 newTag root tag2 thisTag}}

test tree.613 {tree0 tag forget tag1 tag2} {
    list [catch {
	tree0 tag forget myTag1 myTag2
	lsort [tree0 tag names]
    } msg] $msg
} {0 {all myTag newTag root tag2 thisTag}}

test tree.614 {tree0 tag forget all} {
    list [catch {
	tree0 tag forget all
	lsort [tree0 tag names]
    } msg] $msg
} {1 {can't forget reserved tag "all"}}

test tree.615 {tree0 tag forget root} {
    list [catch {
	tree0 tag forget root
	lsort [tree0 tag names]
    } msg] $msg
} {1 {can't forget reserved tag "root"}}

test tree.616 {tree0 tag delete} {
    list [catch {tree0 tag delete} msg] $msg
} {1 {wrong # args: should be "tree0 tag delete tag nodeName..."}}

test tree.617 {tree0 tag delete tag} {
    list [catch {tree0 tag delete tag} msg] $msg
} {1 {wrong # args: should be "tree0 tag delete tag nodeName..."}}

test tree.618 {tree0 tag delete tag 0} {
    list [catch {tree0 tag delete tag 0} msg] $msg
} {0 {}}

test tree.619 {tree0 tag delete root 0} {
    list [catch {tree0 tag delete root 0} msg] $msg
} {1 {can't delete reserved tag "root"}}

test tree.620 {tree0 move} {
    list [catch {tree0 move} msg] $msg
} {1 {wrong # args: should be "tree0 move nodeName destNode ?switches ...?"}}

test tree.621 {tree0 move 0} {
    list [catch {tree0 move 0} msg] $msg
} {1 {wrong # args: should be "tree0 move nodeName destNode ?switches ...?"}}

test tree.622 {tree0 move 0 0} {
    list [catch {tree0 move 0 0} msg] $msg
} {1 {can't move root node}}

test tree.623 {tree0 move 0 badNode} {
    list [catch {tree0 move 0 badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree0}}

test tree.624 {tree0 move 0 all} {
    list [catch {tree0 move 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.625 {tree0 move 1 0 -before 2} {
    list [catch {
	tree0 move 1 0 -before 2
	tree0 children 0
    } msg] $msg
} {0 {1 2 3 4 5 6 8}}

test tree.626 {tree0 move 1 0 -after 2} {
    list [catch {
	tree0 move 1 0 -after 2
	tree0 children 0
    } msg] $msg
} {0 {2 1 3 4 5 6 8}}

test tree.627 {tree0 move 1 2} {
    list [catch {
	tree0 move 1 2
	tree0 children 0
    } msg] $msg
} {0 {2 3 4 5 6 8}}

test tree.628 {tree0 move 0 2} {
    list [catch {tree0 move 0 2} msg] $msg
} {1 {can't move root node}}

test tree.629 {tree0 move 1 17} {
    list [catch {tree0 move 1 17} msg] $msg
} {1 {can't move node: "1" is an ancestor of "17"}}

test tree.630 {tree0 attach} {
    list [catch {tree0 attach} msg] $msg
} {1 {wrong # args: should be "tree0 attach treeName ?switches ...?"}}

test tree.631 {tree0 attach badTree} {
    list [catch {tree0 attach badTree} msg] $msg
} {1 {can't find a tree named "badTree"}}


test tree.632 {tree0 attach tree2 badArg} {
    list [catch {tree0 attach tree2 badArg} msg] $msg
} {1 {unknown switch "badArg"
The following switches are available:
   -newtags }}


test tree.633 {tree1 attach tree0 -newtags} {
    list [catch {
	blt::tree create
	tree1 attach tree0 -newtags
	tree1 dump 0 -version 2.0
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

test tree.634 {tree1 attach tree0} {
    list [catch {
	blt::tree create
	tree1 attach tree0
	tree1 dump 0 -version 2.0
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

test tree.635 {tree1 attach ""} {
    list [catch {tree1 attach ""} msg] $msg
} {0 {}}


test tree.636 {blt::tree destroy tree1} {
    list [catch {blt::tree destroy tree1} msg] $msg
} {0 {}}

test tree.637 {tree0 find root -badSwitch} {
    list [catch {tree0 find root -badSwitch} msg] $msg
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

test tree.638 {tree0 find root -order} {
    list [catch {tree0 find root -order} msg] $msg
} {1 {value for "-order" missing}}

test tree.639 {tree0 find root ...} {
    list [catch {tree0 find root -order preorder -order postorder -order inorder} msg] $msg
} {0 {20 18 16 14 1 21 19 17 15 2 0 3 4 13 5 6 8}}

test tree.640 {tree0 find root -order preorder} {
    list [catch {tree0 find root -order preorder} msg] $msg
} {0 {0 2 1 14 16 18 20 15 17 19 21 3 4 5 13 6 8}}

test tree.641 {tree0 find root -order postorder} {
    list [catch {tree0 find root -order postorder} msg] $msg
} {0 {20 18 16 14 21 19 17 15 1 2 3 4 13 5 6 8 0}}

test tree.642 {tree0 find root -order inorder} {
    list [catch {tree0 find root -order inorder} msg] $msg
} {0 {20 18 16 14 1 21 19 17 15 2 0 3 4 13 5 6 8}}

test tree.643 {tree0 find root -order breadthfirst} {
    list [catch {tree0 find root -order breadthfirst} msg] $msg
} {0 {0 2 3 4 5 6 8 1 13 14 15 16 17 18 19 20 21}}

test tree.644 {tree0 set all key1 myValue} {
    list [catch {tree0 set all key1 myValue} msg] $msg
} {0 {}}

test tree.645 {tree0 set 15 key1 123} {
    list [catch {tree0 set 15 key1 123} msg] $msg
} {0 {}}

test tree.646 {tree0 set 16 key1 1234 key2 abc} {
    list [catch {tree0 set 16 key1 123 key2 abc} msg] $msg
} {0 {}}

test tree.647 {tree0 find root -key } {
    list [catch {tree0 find root -key} msg] $msg
} {1 {value for "-key" missing}}

test tree.648 {tree0 find root -key noKey} {
    list [catch {tree0 find root -key noKey} msg] $msg
} {0 {}}

test tree.649 {tree key root} {
    list [catch { tree0 keys root } msg] $msg
} {0 key1}

test tree.650 {tree key 20 16} {
    list [catch { tree0 keys 20 16 } msg] $msg
} {0 {key1 key2}}

test tree.651 {tree0 find root -key key1} {
    list [catch {tree0 find root -key key1} msg] $msg
} {0 {20 18 16 14 21 19 17 15 1 2 3 4 13 5 6 8 0}}

test tree.652 {tree0 find root -key key2} {
    list [catch {tree0 find root -key key2} msg] $msg
} {0 16}

test tree.653 {tree0 find root -key key2 -exact notThere } {
    list [catch {tree0 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test tree.654 {tree0 find root -key key1 -glob notThere } {
    list [catch {tree0 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test tree.655 {tree0 find root -key badKey -regexp notThere } {
    list [catch {tree0 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test tree.656 {tree0 find root -key key1 -glob 12*} {
    list [catch {tree0 find root -key key1 -glob 12*} msg] $msg
} {0 {16 15}}

test tree.657 {tree0 sort} {
    list [catch {tree0 sort} msg] $msg
} {1 {wrong # args: should be "tree0 sort nodeName ?switches ...?"}}

test tree.658 {tree0 sort all} {
    list [catch {tree0 sort all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.659 {tree0 sort -recurse} {
    list [catch {tree0 sort -recurse} msg] $msg
} {1 {can't find tag or id "-recurse" in ::tree0}}

test tree.660 {tree0 sort 0} {
    list [catch {tree0 sort 0} msg] $msg
} {0 {8 2 3 4 5 6}}

test tree.661 {tree0 sort 0 -recurse} {
    list [catch {tree0 sort 0 -recurse} msg] $msg
} {0 {0 8 1 2 3 4 5 6 13 14 15 16 17 18 19 20 21}}

test tree.662 {tree0 sort 0 -decreasing -key} {
    list [catch {tree0 sort 0 -decreasing -key} msg] $msg
} {1 {value for "-key" missing}}

test tree.663 {tree0 sort 0 -re} {
    list [catch {tree0 sort 0 -re} msg] $msg
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


test tree.664 {tree0 sort 0 -decreasing} {
    list [catch {tree0 sort 0 -decreasing} msg] $msg
} {0 {6 5 4 3 2 8}}

test tree.665 {tree0 sort 0} {
    list [catch {
	set list {}
	foreach n [tree0 sort 0] {
	    lappend list [tree0 label $n]
	}	
	set list
    } msg] $msg
} {0 {myLabel node2 node3 node4 node5 node6}}

test tree.666 {tree0 sort 0 -decreasing} {
    list [catch {tree0 sort 0 -decreasing} msg] $msg
} {0 {6 5 4 3 2 8}}


test tree.667 {tree0 sort 0 -decreasing -key} {
    list [catch {tree0 sort 0 -decreasing -key} msg] $msg
} {1 {value for "-key" missing}}

test tree.668 {tree0 sort 0 -decreasing -key key1} {
    list [catch {tree0 sort 0 -decreasing -key key1} msg] $msg
} {0 {8 6 5 4 3 2}}

test tree.669 {tree0 sort 0 -decreasing -recurse -key key1} {
    list [catch {tree0 sort 0 -decreasing -recurse -key key1} msg] $msg
} {0 {15 16 0 1 2 3 4 5 6 8 13 14 17 18 19 20 21}}

test tree.670 {tree0 sort 0 -decreasing -key key1} {
    list [catch {
	set list {}
	foreach n [tree0 sort 0 -decreasing -key key1] {
	    lappend list [tree0 get $n key1]
	}
	set list
    } msg] $msg
} {0 {myValue myValue myValue myValue myValue myValue}}


test tree.671 {tree0 index 1->firstchild} {
    list [catch {tree0 index 1->firstchild} msg] $msg
} {0 14}

test tree.672 {tree0 index root->firstchild} {
    list [catch {tree0 index root->firstchild} msg] $msg
} {0 2}

test tree.673 {tree0 label root->parent} {
    list [catch {tree0 label root->parent} msg] $msg
} {1 {can't find tag or id "root->parent" in ::tree0}}

test tree.674 {tree0 index root->parent} {
    list [catch {tree0 index root->parent} msg] $msg
} {0 -1}

test tree.675 {tree0 index root->lastchild} {
    list [catch {tree0 index root->lastchild} msg] $msg
} {0 8}

test tree.676 {tree0 index root->next} {
    list [catch {tree0 index root->next} msg] $msg
} {0 2}

test tree.677 {tree0 index root->previous} {
    list [catch {tree0 index root->previous} msg] $msg
} {0 -1}

test tree.678 {tree0 label root->previous} {
    list [catch {tree0 label root->previous} msg] $msg
} {1 {can't find tag or id "root->previous" in ::tree0}}

test tree.679 {tree0 index 1->previous} {
    list [catch {tree0 index 1->previous} msg] $msg
} {0 2}

test tree.680 {tree0 label root->badModifier} {
    list [catch {tree0 label root->badModifier} msg] $msg
} {1 {can't find tag or id "root->badModifier" in ::tree0}}

test tree.681 {tree0 index root->badModifier} {
    list [catch {tree0 index root->badModifier} msg] $msg
} {0 -1}

test tree.682 {tree0 index root->firstchild->parent} {
    list [catch {tree0 index root->firstchild->parent} msg] $msg
} {0 0}

test tree.683 {tree0 trace} {
    list [catch {tree0 trace} msg] $msg
} {1 {wrong # args: should be one of...
  tree0 trace create nodeName valueName how command ?-whenidle?
  tree0 trace delete traceName ...
  tree0 trace info traceName
  tree0 trace names ?pattern ...?}}

test tree.684 {tree0 trace create} {
    list [catch {tree0 trace create} msg] $msg
} {1 {wrong # args: should be "tree0 trace create nodeName valueName how command ?-whenidle?"}}

test tree.685 {tree0 trace create root} {
    list [catch {tree0 trace create root} msg] $msg
} {1 {wrong # args: should be "tree0 trace create nodeName valueName how command ?-whenidle?"}}

test tree.686 {tree0 trace create root * } {
    list [catch {tree0 trace create root * } msg] $msg
} {1 {wrong # args: should be "tree0 trace create nodeName valueName how command ?-whenidle?"}}

test tree.687 {tree0 trace create root * rwuc} {
    list [catch {tree0 trace create root * rwuc} msg] $msg
} {1 {wrong # args: should be "tree0 trace create nodeName valueName how command ?-whenidle?"}}

test tree.688 {tree0 trace create root * badFlags Doit} {
    list [catch {tree0 trace create root * badFlags Doit} msg] $msg
} {1 {unknown flag in "badFlags"}}

test tree.689 {tree0 trace create root * rcuw Doit -badSwitch} {
    list [catch {tree0 trace create root * rcuw Doit -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -whenidle }}


proc Doit args { global mylist; lappend mylist $args }

test tree.690 {tree0 trace create all newKey rwuc Doit} {
    list [catch {tree0 trace create all newKey rwuc Doit} msg] $msg
} {0 trace0}

test tree.691 {tree0 trace names} {
    list [catch {tree0 trace names} msg] $msg
} {0 trace0}

test tree.692 {tree0 trace names badPattern} {
    list [catch {tree0 trace names badPattern} msg] $msg
} {0 {}}

test tree.693 {tree0 trace names t*} {
    list [catch {tree0 trace names t*} msg] $msg
} {0 trace0}

test tree.694 {tree0 trace info trace0} {
    list [catch {tree0 trace info trace0} msg] $msg
} {0 {all newKey rwuc Doit}}

test tree.695 {test create trace} {
    list [catch {
	set mylist {}
	tree0 set all newKey 20
	set mylist
	} msg] $msg
} {0 {{::tree0 0 newKey wc} {::tree0 2 newKey wc} {::tree0 1 newKey wc} {::tree0 14 newKey wc} {::tree0 16 newKey wc} {::tree0 18 newKey wc} {::tree0 20 newKey wc} {::tree0 15 newKey wc} {::tree0 17 newKey wc} {::tree0 19 newKey wc} {::tree0 21 newKey wc} {::tree0 3 newKey wc} {::tree0 4 newKey wc} {::tree0 5 newKey wc} {::tree0 13 newKey wc} {::tree0 6 newKey wc} {::tree0 8 newKey wc}}}

test tree.696 {test read trace} {
    list [catch {
	set mylist {}
	tree0 get root newKey
	set mylist
	} msg] $msg
} {0 {{::tree0 0 newKey r}}}

test tree.697 {test write trace} {
    list [catch {
	set mylist {}
	tree0 set all newKey 21
	set mylist
	} msg] $msg
} {0 {{::tree0 0 newKey w} {::tree0 2 newKey w} {::tree0 1 newKey w} {::tree0 14 newKey w} {::tree0 16 newKey w} {::tree0 18 newKey w} {::tree0 20 newKey w} {::tree0 15 newKey w} {::tree0 17 newKey w} {::tree0 19 newKey w} {::tree0 21 newKey w} {::tree0 3 newKey w} {::tree0 4 newKey w} {::tree0 5 newKey w} {::tree0 13 newKey w} {::tree0 6 newKey w} {::tree0 8 newKey w}}}

test tree.698 {test unset trace} {
    list [catch {
	set mylist {}
	tree0 set all newKey 21
	set mylist
	} msg] $msg
} {0 {{::tree0 0 newKey w} {::tree0 2 newKey w} {::tree0 1 newKey w} {::tree0 14 newKey w} {::tree0 16 newKey w} {::tree0 18 newKey w} {::tree0 20 newKey w} {::tree0 15 newKey w} {::tree0 17 newKey w} {::tree0 19 newKey w} {::tree0 21 newKey w} {::tree0 3 newKey w} {::tree0 4 newKey w} {::tree0 5 newKey w} {::tree0 13 newKey w} {::tree0 6 newKey w} {::tree0 8 newKey w}}}

test tree.699 {tree0 trace delete} {
    list [catch {tree0 trace delete} msg] $msg
} {0 {}}

test tree.700 {tree0 trace delete badId} {
    list [catch {tree0 trace delete badId} msg] $msg
} {1 {unknown trace "badId"}}

test tree.701 {tree0 trace delete trace0} {
    list [catch {tree0 trace delete trace0} msg] $msg
} {0 {}}

test tree.702 {test create trace} {
    list [catch {
	set mylist {}
	tree0 set all newKey 20
	set mylist
	} msg] $msg
} {0 {}}

test tree.703 {test unset trace} {
    list [catch {
	set mylist {}
	tree0 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}


test tree.704 {tree0 notify} {
    list [catch {tree0 notify} msg] $msg
} {1 {wrong # args: should be one of...
  tree0 notify create ?switches ...? command
  tree0 notify delete ?notifyName ...?
  tree0 notify info notifyName
  tree0 notify names ?pattern ...?}}

test tree.705 {tree0 notify create} {
    list [catch {tree0 notify create} msg] $msg
} {1 {wrong # args: should be "tree0 notify create ?switches ...? command"}}

test tree.706 {tree0 notify create -allevents} {
    list [catch {tree0 notify create -allevents Doit} msg] $msg
} {0 notify0}

test tree.707 {tree0 notify info notify0} {
    list [catch {tree0 notify info notify0} msg] $msg
} {0 {notify0 {-create -delete -move -sort -relabel} {Doit}}}

test tree.708 {tree0 notify info badId} {
    list [catch {tree0 notify info badId} msg] $msg
} {1 {unknown notify name "badId"}}

test tree.709 {tree0 notify info} {
    list [catch {tree0 notify info} msg] $msg
} {1 {wrong # args: should be "tree0 notify info notifyName"}}

test tree.710 {tree0 notify names} {
    list [catch {tree0 notify names} msg] $msg
} {0 notify0}


test tree.711 {test create notify} {
    list [catch {
	set mylist {}
	tree0 insert 1 -tags test
	set mylist
	} msg] $msg
} {0 {{-create 22}}}

test tree.712 {test move notify} {
    list [catch {
	set mylist {}
	tree0 move 8 test
	set mylist
	} msg] $msg
} {0 {{-move 8}}}

test tree.713 {test sort notify} {
    list [catch {
	set mylist {}
	tree0 sort 0 -reorder 
	set mylist
	} msg] $msg
} {0 {{-sort 0}}}

test tree.714 {test relabel notify} {
    list [catch {
	set mylist {}
	tree0 label test "newLabel"
	set mylist
	} msg] $msg
} {0 {{-relabel 22}}}

test tree.715 {test delete notify} {
    list [catch {
	set mylist {}
	tree0 delete test
	set mylist
	} msg] $msg
} {0 {{-delete 8} {-delete 22}}}


test tree.716 {tree0 notify delete badId} {
    list [catch {tree0 notify delete badId} msg] $msg
} {1 {unknown notify name "badId"}}


test tree.717 {test create notify} {
    list [catch {
	set mylist {}
	tree0 set all newKey 20
	set mylist
	} msg] $msg
} {0 {}}

test tree.718 {test delete notify} {
    list [catch {
	set mylist {}
	tree0 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}

test tree.719 {test delete notify} {
    list [catch {
	set mylist {}
	tree0 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}

test tree.720 {tree0 copy} {
    list [catch {tree0 copy} msg] $msg
} {1 {wrong # args: should be "tree0 copy parentNode ?treeName? nodeName ?switches ...?"}}

test tree.721 {tree0 copy root} {
    list [catch {tree0 copy root} msg] $msg
} {1 {wrong # args: should be "tree0 copy parentNode ?treeName? nodeName ?switches ...?"}}

test tree.722 {tree0 copy root 14} {
    list [catch {tree0 copy root 14} msg] $msg
} {0 23}

test tree.723 {tree0 copy 14 root} {
    list [catch {tree0 copy 14 root} msg] $msg
} {0 24}

test tree.724 {tree0 copy 14 root -recurse} {
    list [catch {tree0 copy 14 root -recurse} msg] $msg
} {1 {can't make cyclic copy: source node is an ancestor of the destination}}

test tree.725 {tree0 copy 3 2 -recurse -tags} {
    list [catch {tree0 copy 3 2 -recurse -tags} msg] $msg
} {0 25}

test tree.726 {tree1 copy 3 2 -tree badTree} {
    list [catch {tree0 copy 3 2 -tree badTree} msg] $msg
} {1 {can't find a tree named "badTree"}}

test tree.727 {tree1 copy 3 2 -tree tree0} {
    list [catch {tree0 copy 3 2->badModifier -tree tree0} msg] $msg
} {1 {can't find tag or id "2->badModifier" in ::tree0}}

test tree.728 {copy tree to tree -recurse} {
    list [catch {
	blt::tree create tree1
	foreach node [tree0 children root] {
	    tree1 copy root $node -recurse  -tree tree0
	}
	foreach node [tree0 children root] {
	    tree1 copy root $node -recurse  -tree tree0
	}
	tree1 dump root -version 2.0
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

test tree.729 {tree dir (no recurse flag)} {
    list [catch {
	file delete -force ./testdir
	file mkdir ./testdir/dir1
	file mkdir ./testdir/dir2
	file mkdir ./testdir/dir3
	file copy defs ./testdir/dir1
	set tree [blt::tree create]
	$tree dir 0 ./testdir \
	    -fields { perms type } \
	    -pattern defs \
	    -recurse
	set contents [$tree dump 0 -version 2.0]
	blt::tree destroy $tree
	file delete -force ./testdir
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 2 {{} dir1} {perms 493 type directory} {}
2 3 {{} dir1 defs} {perms 420 type file} {}
}}


test tree.730 {tree dir -recurse} {
    list [catch {
	file delete -force ./testdir
	file mkdir ./testdir/.dir0
	file mkdir ./testdir/dir1/dir2
	file copy defs ./testdir/dir1/dir2
	set tree [blt::tree create]
	$tree dir 0 ./testdir  -recurse -fields { perms type } -type f
	set contents [$tree dump 0 -version 2.0]
	blt::tree destroy $tree
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} dir1} {perms 493 type directory} {}
1 2 {{} dir1 dir2} {perms 493 type directory} {}
2 3 {{} dir1 dir2 defs} {perms 420 type file} {}
}}

test tree.731 {tree dir -recurse} {
    list [catch {
	file delete -force ./testdir
	file mkdir ./testdir
	file copy defs ./testdir
	set tree [blt::tree create]
	$tree dir 0 ./testdir -recurse -fields { size perms type }
	set contents [$tree dump 0 -version 2.0]
	blt::tree destroy $tree
	file delete -force ./testdir
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} defs} {size 2894 perms 420 type file} {}
}}

test tree.732 {tree dir (default settings, no -recurse)} {
    list [catch {
	file delete -force ./testdir
	file mkdir ./testdir/dir1
	file copy defs ./testdir/dir1
	set tree [blt::tree create]
	$tree dir 0 ./testdir -fields { perms type }
	set contents [$tree dump 0 -version 2.0]
	blt::tree destroy $tree
	file delete -force ./testdir
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} dir1} {perms 493 type directory} {}
}}

test tree.733 {tree dir -type "file pipe"} {
    list [catch {
	set tree [blt::tree create]
	$tree dir 0 /dev -fields { size perms type } -type "file pipe"
	set contents [$tree dump 0 -version 2.0]
	blt::tree destroy $tree
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 1 {{} core} {size 140737477881856 perms 256 type file} {}
0 2 {{} stderr} {size 0 perms 384 type fifo} {}
0 3 {{} stdout} {size 0 perms 384 type fifo} {}
0 4 {{} initctl} {size 0 perms 384 type fifo} {}
}}

test tree.734 {tree dir -type link -recurse} {
    list [catch {
	file delete -force ./testdir
	file mkdir ./testdir/dir1
	file copy defs ./testdir/dir1
	file link -symbolic [pwd]/testdir/mylink [pwd]/testdir/dir1/defs 
	set tree [blt::tree create]
	$tree dir 0 ./testdir -fields { size perms type } -type "link" -recurse
	set contents [$tree dump 0 -version 2.0]
	blt::tree destroy $tree
	file delete -force ./testdir
	set contents
    } msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
0 2 {{} mylink} {size 2894 perms 420 type file} {}
}}


exit 0

# Missing tests.
# import
# position
# replacevalues
# path separator, parse, create
# move
# insert inode > 0
# dump
# restore





