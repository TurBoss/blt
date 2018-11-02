
package require BLT

if {[info procs test] != "test"} {
    source defs
}

if [file exists ../library] {
    set blt_library ../library
}

#set VERBOSE 1
set DIFF 1

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
} {0 ::tree1.suffix}

test tree.4 {tree create prefix.#auto} {
    list [catch {blt::tree create prefix.#auto} msg] $msg
} {0 ::prefix.tree2}

test tree.5 {tree create prefix.#auto.suffix} {
    list [catch {blt::tree create prefix.#auto.suffix} msg] $msg
} {0 ::prefix.tree3.suffix}

test tree.6 {tree create prefix.#auto.suffix.#auto} {
    list [catch {blt::tree create prefix.#auto.suffix.#auto} msg] $msg
} {0 ::prefix.tree4.suffix.#auto}

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

test tree.14 {tree destroy [tree names *tree*]} {
    list [catch {eval blt::tree destroy [blt::tree names *tree*]} msg] $msg
} {0 {}}

test tree.15 {create} {
    list [catch {blt::tree create} msg] $msg
} {0 ::tree5}

test tree.16 {create} {
    list [catch {blt::tree create} msg] $msg
} {0 ::tree6}

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

test tree.22 {create} {
    list [catch {blt::tree create} msg] $msg
} {0 ::tree7}

test tree.23 {delete tree7} {
    list [catch {blt::tree destroy tree7} msg] $msg
} {0 {}}

test tree.24 {tree create (bad namespace)} {
    list [catch {blt::tree create badName::fred} msg] $msg
} {1 {unknown namespace "badName"}}

test tree.25 {tree create (wrong # args)} {
    list [catch {blt::tree create a b} msg] $msg
} {1 {wrong # args: should be "blt::tree create ?treeName?"}}

test tree.26 {tree names} {
    list [catch {blt::tree names} msg] [lsort $msg]
} {0 {::fred ::tree5 ::tree6}}

test tree.27 {tree names pattern)} {
    list [catch {blt::tree names ::tree*} msg] [lsort $msg]
} {0 {::tree5 ::tree6}}

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

test tree.35 {tree destroy tree5 tree6} {
    list [catch {blt::tree destroy tree5 tree6} msg] $msg
} {0 {}}

test tree.36 {exists tree0} {
    list [catch {blt::tree exists tree5} msg] $msg
} {0 0}

test tree.37 {exists tree1} {
    list [catch {blt::tree exists tree6} msg] $msg
} {0 0}


test tree.38 {create} {
    list [catch {blt::tree create} msg] $msg
} {0 ::tree8}

test tree.39 {tree8} {
    list [catch {tree8} msg] $msg
} {1 {wrong # args: should be one of...
  tree8 ancestor node1 node2
  tree8 append nodeName varName ?value ...?
  tree8 apply nodeName ?switches ...?
  tree8 attach treeName ?switches ...?
  tree8 children nodeName ?switches ...?
  tree8 copy parentNode ?treeName? nodeName ?switches ...?
  tree8 degree nodeName
  tree8 delete ?nodeName ...?
  tree8 depth ?nodeName?
  tree8 dir nodeName path ?switches ...?
  tree8 dump nodeName ?switches ...?
  tree8 dup nodeName
  tree8 exists nodeName ?varName?
  tree8 export formatName ?switches ...?
  tree8 find nodeName ?switches ...?
  tree8 findchild nodeName label
  tree8 firstchild nodeName
  tree8 get nodeName ?varName? ?defValue?
  tree8 import formatName ?switches ...?
  tree8 index label|list
  tree8 insert parentNode ?switches ...?
  tree8 isancestor node1 node2
  tree8 isbefore node1 node2
  tree8 isleaf nodeName
  tree8 isroot nodeName
  tree8 keys nodeName ?nodeName...?
  tree8 label nodeName ?newLabel?
  tree8 lappend nodeName varName ?value ...?
  tree8 lastchild nodeName
  tree8 lindex nodeName varName index
  tree8 linsert nodeName varName index ?value...?
  tree8 llength nodeName varName
  tree8 lrange nodeName varName first last
  tree8 lreplace nodeName varName first last ?value...?
  tree8 move nodeName destNode ?switches ...?
  tree8 names nodeName ?varName?
  tree8 next nodeName
  tree8 nextsibling nodeName
  tree8 notify args ...
  tree8 parent nodeName
  tree8 path ?args ...?
  tree8 position ?switches ...? nodeName...
  tree8 previous nodeName
  tree8 prevsibling nodeName
  tree8 replace nodeName destNode
  tree8 restore nodeName ?switches ...?
  tree8 root 
  tree8 set nodeName ?varName value ...?
  tree8 size nodeName
  tree8 sort nodeName ?switches ...?
  tree8 tag args ...
  tree8 trace args ...
  tree8 type nodeName varName
  tree8 unset nodeName ?varName ...?}}

test tree.40 {tree8 badOp} {
    list [catch {tree8 badOp} msg] $msg
} {1 {bad operation "badOp": should be one of...
  tree8 ancestor node1 node2
  tree8 append nodeName varName ?value ...?
  tree8 apply nodeName ?switches ...?
  tree8 attach treeName ?switches ...?
  tree8 children nodeName ?switches ...?
  tree8 copy parentNode ?treeName? nodeName ?switches ...?
  tree8 degree nodeName
  tree8 delete ?nodeName ...?
  tree8 depth ?nodeName?
  tree8 dir nodeName path ?switches ...?
  tree8 dump nodeName ?switches ...?
  tree8 dup nodeName
  tree8 exists nodeName ?varName?
  tree8 export formatName ?switches ...?
  tree8 find nodeName ?switches ...?
  tree8 findchild nodeName label
  tree8 firstchild nodeName
  tree8 get nodeName ?varName? ?defValue?
  tree8 import formatName ?switches ...?
  tree8 index label|list
  tree8 insert parentNode ?switches ...?
  tree8 isancestor node1 node2
  tree8 isbefore node1 node2
  tree8 isleaf nodeName
  tree8 isroot nodeName
  tree8 keys nodeName ?nodeName...?
  tree8 label nodeName ?newLabel?
  tree8 lappend nodeName varName ?value ...?
  tree8 lastchild nodeName
  tree8 lindex nodeName varName index
  tree8 linsert nodeName varName index ?value...?
  tree8 llength nodeName varName
  tree8 lrange nodeName varName first last
  tree8 lreplace nodeName varName first last ?value...?
  tree8 move nodeName destNode ?switches ...?
  tree8 names nodeName ?varName?
  tree8 next nodeName
  tree8 nextsibling nodeName
  tree8 notify args ...
  tree8 parent nodeName
  tree8 path ?args ...?
  tree8 position ?switches ...? nodeName...
  tree8 previous nodeName
  tree8 prevsibling nodeName
  tree8 replace nodeName destNode
  tree8 restore nodeName ?switches ...?
  tree8 root 
  tree8 set nodeName ?varName value ...?
  tree8 size nodeName
  tree8 sort nodeName ?switches ...?
  tree8 tag args ...
  tree8 trace args ...
  tree8 type nodeName varName
  tree8 unset nodeName ?varName ...?}}

test tree.41 {tree8 insert (wrong # args)} {
    list [catch {tree8 insert} msg] $msg
} {1 {wrong # args: should be "tree8 insert parentNode ?switches ...?"}}

test tree.42 {tree8 insert badParent} {
    list [catch {tree8 insert badParent} msg] $msg
} {1 {can't find tag or id "badParent" in ::tree8}}

test tree.43 {tree8 insert 1000} {
    list [catch {tree8 insert 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree8}}

test tree.44 {tree8 insert 0} {
    list [catch {tree8 insert 0} msg] $msg
} {0 1}

test tree.45 {tree8 insert 0} {
    list [catch {tree8 insert 0} msg] $msg
} {0 2}

test tree.46 {tree8 insert root} {
    list [catch {tree8 insert root} msg] $msg
} {0 3}

test tree.47 {tree8 insert all} {
    list [catch {tree8 insert all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.48 {tree8 insert 0 -after (no arg)} {
    list [catch {tree8 insert 0 -after} msg] $msg
} {1 {value for "-after" missing}}

test tree.49 {tree8 insert 0 -after badPosition} {
    list [catch {tree8 insert 0 -after badPosition} msg] $msg
} {1 {can't find tag or id "badPosition" in ::tree8}}

test tree.50 {tree8 insert 0 -after -1} {
    list [catch {tree8 insert 0 -after -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree8}}

test tree.51 {tree8 insert 0 -after 1000} {
    list [catch {tree8 insert 0 -after 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree8}}

test tree.52 {tree8 insert 0 -before (no arg)} {
    list [catch {tree8 insert 0 -before} msg] $msg
} {1 {value for "-before" missing}}

test tree.53 {tree8 insert 0 -before badPosition} {
    list [catch {tree8 insert 0 -before badPosition} msg] $msg
} {1 {can't find tag or id "badPosition" in ::tree8}}

test tree.54 {tree8 insert 0 -before -1} {
    list [catch {tree8 insert 0 -before -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree8}}

test tree.55 {tree8 insert 0 -before 1000} {
    list [catch {tree8 insert 0 -before 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree8}}

test tree.56 {tree8 insert 0 -after last} {
    list [catch {tree8 insert 0 -after [tree8 lastchild 0]} msg] $msg
} {0 4}

test tree.57 {tree8 insert 0 -tags myTag} {
    list [catch {tree8 insert 0 -tags myTag} msg] $msg
} {0 5}

test tree.58 {tree8 insert 0 -tags {myTag1 myTag2} } {
    list [catch {tree8 insert 0 -tags {myTag1 myTag2}} msg] $msg
} {0 6}

test tree.59 {tree8 insert 0 -tags root} {
    list [catch {tree8 insert 0 -tags root} msg] $msg
} {1 {can't add reserved tag "root"}}

test tree.60 {tree8 insert 0 -tags (missing arg)} {
    list [catch {tree8 insert 0 -tags} msg] $msg
} {1 {value for "-tags" missing}}

test tree.61 {tree8 insert 0 -label myLabel -tags thisTag} {
    list [catch {tree8 insert 0 -label myLabel -tags thisTag} msg] $msg
} {0 8}

test tree.62 {tree8 insert 0 -label (missing arg)} {
    list [catch {tree8 insert 0 -label} msg] $msg
} {1 {value for "-label" missing}}

test tree.63 {tree8 insert 1 -tags thisTag} {
    list [catch {tree8 insert 1 -tags thisTag} msg] $msg
} {0 9}

test tree.64 {tree8 insert 1 -data key (missing value)} {
    list [catch {tree8 insert 1 -data key} msg] $msg
} {1 {missing value for "key"}}

test tree.65 {tree8 insert 1 -data {key value}} {
    list [catch {tree8 insert 1 -data {key value}} msg] $msg
} {0 11}

test tree.66 {tree8 insert 1 -data {key1 value1 key2 value2}} {
    list [catch {tree8 insert 1 -data {key1 value1 key2 value2}} msg] $msg
} {0 12}

test tree.67 {get} {
    list [catch {
	tree8 get 12
    } msg] $msg
} {0 {key1 value1 key2 value2}}

test tree.68 {tree8 children} {
    list [catch {tree8 children} msg] $msg
} {1 {wrong # args: should be "tree8 children nodeName ?switches ...?"}}

test tree.69 {tree8 children 0} {
    list [catch {tree8 children 0} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test tree.70 {tree8 children root} {
    list [catch {tree8 children root} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test tree.71 {tree8 children 1} {
    list [catch {tree8 children 1} msg] $msg
} {0 {9 11 12}}

test tree.72 {tree8 children 1 -nocomplain} {
    list [catch {tree8 children 1 -nocomplain} msg] $msg
} {0 {9 11 12}}

test tree.73 {tree8 children badNode -nocomplain} {
    list [catch {tree8 children badNode -nocomplain} msg] $msg
} {0 {}}

test tree.74 {tree8 children badNode} {
    list [catch {tree8 children badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}


test tree.75 {tree8 insert myTag} {
    list [catch {tree8 insert myTag} msg] $msg
} {0 13}

test tree.76 {tree8 index myTag} {
    list [catch {tree8 index myTag} msg] $msg
} {0 5}

test tree.77 {tree8 children 5} {
    list [catch {tree8 children 5} msg] $msg
} {0 13}

test tree.78 {tree8 children -1 -nocomplain} {
    list [catch {tree8 children -1 -nocomplain} msg] $msg
} {0 {}}

test tree.79 {tree8 children badNode -nocomplain} {
    list [catch {tree8 children badNode -nocomplain} msg] $msg
} {0 {}}

test tree.80 {tree8 children myTag} {
    list [catch {tree8 children myTag} msg] $msg
} {0 13}

test tree.81 {tree8 children root} {
    list [catch {tree8 children root} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test tree.82 {tree8 children root -from 1} {
    list [catch {tree8 children root -from 1} msg] $msg
} {0 {1 2 3 4 5 6 8}}

test tree.83 {tree8 children root -to 6 } {
    list [catch {tree8 children root -to 6} msg] $msg
} {0 {1 2 3 4 5 6}}

test tree.84 {tree8 children root -from 7 } {
    list [catch {tree8 children root -from 7} msg] $msg
} {1 {can't find tag or id "7" in ::tree8}}

test tree.85 {tree8 children root -to 7 } {
    list [catch {tree8 children root -to 7} msg] $msg
} {1 {can't find tag or id "7" in ::tree8}}

test tree.86 {tree8 children root -from 2} {
    list [catch {tree8 children root -from 2} msg] $msg
} {0 {2 3 4 5 6 8}}

test tree.87 {tree8 children root -from all} {
    list [catch {tree8 children root -from all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.88 {tree8 children root -to all} {
    list [catch {tree8 children root -to all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.89 {tree8 children root -from root} {
    list [catch {tree8 children root -from root} msg] $msg
} {1 {bad -from switch: node is not a child of "root"}}

test tree.90 {tree8 children root -to root} {
    list [catch {tree8 children root -to root} msg] $msg
} {1 {bad -to switch: node is not a child of "root"}}

test tree.91 {tree8 children root -from 1 -to 3} {
    list [catch {tree8 children root -from 1 -to 3} msg] $msg
} {0 {1 2 3}}

test tree.92 {tree8 children root -from -1} {
    list [catch {tree8 children root -from -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree8}}

test tree.93 {tree8 children root -to -1} {
    list [catch {tree8 children root -to -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree8}}

test tree.94 {tree8 firstchild (missing arg)} {
    list [catch {tree8 firstchild} msg] $msg
} {1 {wrong # args: should be "tree8 firstchild nodeName"}}

test tree.95 {tree8 firstchild badNode} {
    list [catch {tree8 firstchild badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.96 {tree8 firstchild all} {
    list [catch {tree8 firstchild all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.97 {tree8 firstchild root} {
    list [catch {tree8 firstchild root} msg] $msg
} {0 1}

test tree.98 {tree8 lastchild (missing arg)} {
    list [catch {tree8 lastchild} msg] $msg
} {1 {wrong # args: should be "tree8 lastchild nodeName"}}

test tree.99 {tree8 lastchild badNode} {
    list [catch {tree8 lastchild badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.100 {tree8 lastchild all} {
    list [catch {tree8 lastchild all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.101 {tree8 lastchild root} {
    list [catch {tree8 lastchild root} msg] $msg
} {0 8}

test tree.102 {tree8 nextsibling (missing arg)} {
    list [catch {tree8 nextsibling} msg] $msg
} {1 {wrong # args: should be "tree8 nextsibling nodeName"}}

test tree.103 {tree8 nextsibling 1)} {
    list [catch {tree8 nextsibling 1} msg] $msg
} {0 2}

test tree.104 {tree8 nextsibling 2)} {
    list [catch {tree8 nextsibling 2} msg] $msg
} {0 3}

test tree.105 {tree8 nextsibling 3)} {
    list [catch {tree8 nextsibling 3} msg] $msg
} {0 4}

test tree.106 {tree8 nextsibling 4)} {
    list [catch {tree8 nextsibling 4} msg] $msg
} {0 5}

test tree.107 {tree8 nextsibling 5)} {
    list [catch {tree8 nextsibling 5} msg] $msg
} {0 6}

test tree.108 {tree8 nextsibling 6)} {
    list [catch {tree8 nextsibling 6} msg] $msg
} {0 8}

test tree.109 {tree8 nextsibling 8)} {
    list [catch {tree8 nextsibling 8} msg] $msg
} {0 -1}

test tree.110 {tree8 nextsibling all)} {
    list [catch {tree8 nextsibling all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.111 {tree8 nextsibling badTag)} {
    list [catch {tree8 nextsibling badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::tree8}}

test tree.112 {tree8 nextsibling -1)} {
    list [catch {tree8 nextsibling -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree8}}

test tree.113 {tree8 prevsibling 2)} {
    list [catch {tree8 prevsibling 2} msg] $msg
} {0 1}

test tree.114 {tree8 prevsibling 1)} {
    list [catch {tree8 prevsibling 1} msg] $msg
} {0 -1}

test tree.115 {tree8 prevsibling -1)} {
    list [catch {tree8 prevsibling -1} msg] $msg
} {1 {can't find tag or id "-1" in ::tree8}}

test tree.116 {tree8 root)} {
    list [catch {tree8 root} msg] $msg
} {0 0}

test tree.117 {tree8 root badArg)} {
    list [catch {tree8 root badArgs} msg] $msg
} {1 {wrong # args: should be "tree8 root "}}

test tree.118 {tree8 parent (missing arg))} {
    list [catch {tree8 parent} msg] $msg
} {1 {wrong # args: should be "tree8 parent nodeName"}}

test tree.119 {tree8 parent root)} {
    list [catch {tree8 parent root} msg] $msg
} {0 -1}

test tree.120 {tree8 parent 1)} {
    list [catch {tree8 parent 1} msg] $msg
} {0 0}

test tree.121 {tree8 parent myTag)} {
    list [catch {tree8 parent myTag} msg] $msg
} {0 0}

test tree.122 {tree8 next (missing arg))} {
    list [catch {tree8 next} msg] $msg
} {1 {wrong # args: should be "tree8 next nodeName"}}

test tree.123 {tree8 next} {
    list [catch {tree8 next} msg] $msg
} {1 {wrong # args: should be "tree8 next nodeName"}}

test tree.124 {tree8 next badNode} {
    list [catch {tree8 next badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.125 {tree8 next (extra arg))} {
    list [catch {tree8 next root root} msg] $msg
} {1 {wrong # args: should be "tree8 next nodeName"}}

test tree.126 {tree8 next all} {
    list [catch {tree8 next all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.127 {tree8 next root} {
    list [catch {tree8 next root} msg] $msg
} {0 1}

test tree.128 {tree8 next 1)} {
    list [catch {tree8 next 1} msg] $msg
} {0 9}

test tree.129 {tree8 next 2)} {
    list [catch {tree8 next 2} msg] $msg
} {0 3}

test tree.130 {tree8 next 3)} {
    list [catch {tree8 next 3} msg] $msg
} {0 4}

test tree.131 {tree8 next 4)} {
    list [catch {tree8 next 4} msg] $msg
} {0 5}

test tree.132 {tree8 next 5)} {
    list [catch {tree8 next 5} msg] $msg
} {0 13}

test tree.133 {tree8 next 6)} {
    list [catch {tree8 next 6} msg] $msg
} {0 8}

test tree.134 {tree8 next 8)} {
    list [catch {tree8 next 8} msg] $msg
} {0 -1}

test tree.135 {tree8 previous} {
    list [catch {tree8 previous} msg] $msg
} {1 {wrong # args: should be "tree8 previous nodeName"}}

test tree.136 {tree8 previous badNode} {
    list [catch {tree8 previous badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.137 {tree8 previous all} {
    list [catch {tree8 previous all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.138 {tree8 previous 1)} {
    list [catch {tree8 previous 1} msg] $msg
} {0 0}

test tree.139 {tree8 previous 0)} {
    list [catch {tree8 previous 0} msg] $msg
} {0 -1}

test tree.140 {tree8 previous 8)} {
    list [catch {tree8 previous 8} msg] $msg
} {0 6}

test tree.141 {tree8 depth (no arg))} {
    list [catch {tree8 depth} msg] $msg
} {0 2}

test tree.142 {tree8 depth badNode} {
    list [catch {tree8 depth badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.143 {tree8 depth all} {
    list [catch {tree8 depth all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.144 {tree8 depth root))} {
    list [catch {tree8 depth root} msg] $msg
} {0 0}

test tree.145 {tree8 depth myTag))} {
    list [catch {tree8 depth myTag} msg] $msg
} {0 1}

test tree.146 {tree8 depth myTag))} {
    list [catch {tree8 depth myTag} msg] $msg
} {0 1}

test tree.147 {tree8 dump (missing arg)))} {
    list [catch {tree8 dump} msg] $msg
} {1 {wrong # args: should be "tree8 dump nodeName ?switches ...?"}}

test tree.148 {tree8 dump root -version 2.0} {
    list [catch {tree8 dump root -version 2.0} msg] $msg
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

test tree.149 {tree8 dump 1 -version 2.0} {
    list [catch {tree8 dump 1 -version 2.0} msg] $msg
} {0 {# V2.0
-1 1 node1 {} {}
1 9 {node1 node9} {} {thisTag}
1 11 {node1 node11} {key value} {}
1 12 {node1 node12} {key1 value1 key2 value2} {}
}}

test tree.150 {tree8 dump this -version 2.0} {
    list [catch {tree8 dump myTag -version 2.0} msg] $msg
} {0 {# V2.0
-1 5 node5 {} {myTag}
5 13 {node5 node13} {} {}
}}

test tree.151 {tree8 dump 1 badSwitch} {
    list [catch {tree8 dump 1 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -data data
   -file fileName
   -metadata list
   -version versionNum
   -notags }}

test tree.152 {tree8 dump 11 -version 2.0} {
    list [catch {tree8 dump 11 -version 2.0} msg] $msg
} {0 {# V2.0
-1 11 node11 {key value} {}
}}

test tree.153 {tree8 dump all} {
    list [catch {tree8 dump all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.154 {tree8 dump all} {
    list [catch {tree8 dump all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.155 {tree8 dump 0 -file test.dump} {
    list [catch {tree8 dump 0 -file test.dump} msg] $msg
} {0 {}}

test tree.156 {tree8 get 9} {
    list [catch {tree8 get 9} msg] $msg
} {0 {}}

test tree.157 {tree8 get all} {
    list [catch {tree8 get all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.158 {tree8 get root} {
    list [catch {tree8 get root} msg] $msg
} {0 {}}

test tree.159 {tree8 get 9 key} {
    list [catch {tree8 get root} msg] $msg
} {0 {}}

test tree.160 {tree8 get 12} {
    list [catch {tree8 get 12} msg] $msg
} {0 {key1 value1 key2 value2}}

test tree.161 {tree8 get 12 key1} {
    list [catch {tree8 get 12 key1} msg] $msg
} {0 value1}

test tree.162 {tree8 get 12 key2} {
    list [catch {tree8 get 12 key2} msg] $msg
} {0 value2}

test tree.163 {tree8 get 12 key1 defValue } {
    list [catch {tree8 get 12 key1 defValue} msg] $msg
} {0 value1}

test tree.164 {tree8 get 12 key100 defValue } {
    list [catch {tree8 get 12 key100 defValue} msg] $msg
} {0 defValue}

test tree.165 {tree8 index (missing arg) } {
    list [catch {tree8 index} msg] $msg
} {1 {wrong # args: should be "tree8 index label|list"}}

test tree.166 {tree8 index 0 10 (extra arg) } {
    list [catch {tree8 index 0 10} msg] $msg
} {1 {wrong # args: should be "tree8 index label|list"}}

test tree.167 {tree8 index 0} {
    list [catch {tree8 index 0} msg] $msg
} {0 0}

test tree.168 {tree8 index root} {
    list [catch {tree8 index root} msg] $msg
} {0 0}

test tree.169 {tree8 index all} {
    list [catch {tree8 index all} msg] $msg
} {0 -1}

test tree.170 {tree8 index myTag} {
    list [catch {tree8 index myTag} msg] $msg
} {0 5}

test tree.171 {tree8 index thisTag} {
    list [catch {tree8 index thisTag} msg] $msg
} {0 -1}

test tree.172 {tree8 is (no args)} {
    list [catch {tree8 is} msg] $msg
} {1 {ambiguous operation "is" matches:  isancestor isbefore isleaf isroot}}

test tree.173 {tree8 isbefore} {
    list [catch {tree8 isbefore} msg] $msg
} {1 {wrong # args: should be "tree8 isbefore node1 node2"}}

test tree.174 {tree8 isbefore 0 10 20} {
    list [catch {tree8 isbefore 0 10 20} msg] $msg
} {1 {wrong # args: should be "tree8 isbefore node1 node2"}}

test tree.175 {tree8 isbefore 0 12} {
    list [catch {tree8 isbefore 0 12} msg] $msg
} {0 1}

test tree.176 {tree8 isbefore 12 0} {
    list [catch {tree8 isbefore 12 0} msg] $msg
} {0 0}

test tree.177 {tree8 isbefore 0 0} {
    list [catch {tree8 isbefore 0 0} msg] $msg
} {0 0}

test tree.178 {tree8 isbefore root 0} {
    list [catch {tree8 isbefore root 0} msg] $msg
} {0 0}

test tree.179 {tree8 isbefore 0 all} {
    list [catch {tree8 isbefore 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.180 {tree8 isancestor} {
    list [catch {tree8 isancestor} msg] $msg
} {1 {wrong # args: should be "tree8 isancestor node1 node2"}}

test tree.181 {tree8 isancestor 0 12 20} {
    list [catch {tree8 isancestor 0 12 20} msg] $msg
} {1 {wrong # args: should be "tree8 isancestor node1 node2"}}

test tree.182 {tree8 isancestor 0 12} {
    list [catch {tree8 isancestor 0 12} msg] $msg
} {0 1}

test tree.183 {tree8 isancestor 12 0} {
    list [catch {tree8 isancestor 12 0} msg] $msg
} {0 0}

test tree.184 {tree8 isancestor 1 2} {
    list [catch {tree8 isancestor 1 2} msg] $msg
} {0 0}

test tree.185 {tree8 isancestor root 0} {
    list [catch {tree8 isancestor root 0} msg] $msg
} {0 0}

test tree.186 {tree8 isancestor 0 all} {
    list [catch {tree8 isancestor 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.187 {tree8 isroot (missing arg)} {
    list [catch {tree8 isroot} msg] $msg
} {1 {wrong # args: should be "tree8 isroot nodeName"}}

test tree.188 {tree8 isroot 0 20 (extra arg)} {
    list [catch {tree8 isroot 0 20} msg] $msg
} {1 {wrong # args: should be "tree8 isroot nodeName"}}

test tree.189 {tree8 isroot 0} {
    list [catch {tree8 isroot 0} msg] $msg
} {0 1}

test tree.190 {tree8 isroot 12} {
    list [catch {tree8 isroot 12} msg] $msg
} {0 0}

test tree.191 {tree8 isroot 1} {
    list [catch {tree8 isroot 1} msg] $msg
} {0 0}

test tree.192 {tree8 isroot root} {
    list [catch {tree8 isroot root} msg] $msg
} {0 1}

test tree.193 {tree8 isroot all} {
    list [catch {tree8 isroot all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.194 {tree8 isleaf (missing arg)} {
    list [catch {tree8 isleaf} msg] $msg
} {1 {wrong # args: should be "tree8 isleaf nodeName"}}

test tree.195 {tree8 isleaf 0 20 (extra arg)} {
    list [catch {tree8 isleaf 0 20} msg] $msg
} {1 {wrong # args: should be "tree8 isleaf nodeName"}}

test tree.196 {tree8 isleaf 0} {
    list [catch {tree8 isleaf 0} msg] $msg
} {0 0}

test tree.197 {tree8 isleaf 12} {
    list [catch {tree8 isleaf 12} msg] $msg
} {0 1}

test tree.198 {tree8 isleaf 1} {
    list [catch {tree8 isleaf 1} msg] $msg
} {0 0}

test tree.199 {tree8 isleaf root} {
    list [catch {tree8 isleaf root} msg] $msg
} {0 0}

test tree.200 {tree8 isleaf all} {
    list [catch {tree8 isleaf all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.201 {tree8 isleaf 1000} {
    list [catch {tree8 isleaf 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree8}}

test tree.202 {tree8 isleaf badTag} {
    list [catch {tree8 isleaf badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::tree8}}

test tree.203 {tree8 set (missing arg)} {
    list [catch {tree8 set} msg] $msg
} {1 {wrong # args: should be "tree8 set nodeName ?varName value ...?"}}

test tree.204 {tree8 set badNode (missing arg)} {
    list [catch {tree8 set badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.205 {tree8 set 0 (missing arg)} {
    list [catch {tree8 set 0} msg] $msg
} {0 {}}

test tree.206 {tree8 set 0 key (missing arg)} {
    list [catch {tree8 set 0 key} msg] $msg
} {1 {missing value for "key"}}

test tree.207 {tree8 set 0 key value} {
    list [catch {tree8 set 0 key value} msg] $msg
} {0 {}}

test tree.208 {tree8 set 0 key1 value1 key2 value2 key3 value3} {
    list [catch {tree8 set 0 key1 value1 key2 value2 key3 value3} msg] $msg
} {0 {}}

test tree.209 {tree8 set 0 key1 value1 key2 (missing arg)} {
    list [catch {tree8 set 0 key1 value1 key2} msg] $msg
} {1 {missing value for "key2"}}

test tree.210 {tree8 set 0 key value} {
    list [catch {tree8 set 0 key value} msg] $msg
} {0 {}}

test tree.211 {tree8 set 0 key1 value1 key2 (missing arg)} {
    list [catch {tree8 set 0 key1 value1 key2} msg] $msg
} {1 {missing value for "key2"}}

test tree.212 {tree8 set all} {
    list [catch {tree8 set all} msg] $msg
} {0 {}}

test tree.213 {tree8 set all abc 123} {
    list [catch {tree8 set all abc 123} msg] $msg
} {0 {}}

test tree.214 {tree8 set root} {
    list [catch {tree8 set root} msg] $msg
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
} {1 {can't find a variable "badArr" in tree "::myTree" at node 0}}

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

test tree.238 {tree8 unset} {
    list [catch {tree8 unset} msg] $msg
} {1 {wrong # args: should be "tree8 unset nodeName ?varName ...?"}}

test tree.239 {tree8 unset badNode} {
    list [catch {tree8 unset badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.240 {tree8 unset badNode badValue} {
    list [catch {tree8 unset badNode badValue} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.241 {tree8 unset root badValue} {
    list [catch {tree8 unset root badValue} msg] $msg
} {0 {}}

test tree.242 {tree8 unset root badValue(0)} {
    list [catch {tree8 unset root badValue(0)} msg] $msg
} {0 {}}

test tree.243 {tree8 set root myScalar 1} {
    list [catch {tree8 set root myScalar 1} msg] $msg
} {0 {}}

test tree.244 {tree8 set root myArray(0) 0} {
    list [catch {tree8 set root myArray(0) 0} msg] $msg
} {0 {}}

test tree.245 {tree8 set root myArray(1) 1} {
    list [catch {tree8 set root myArray(1) 1} msg] $msg
} {0 {}}

test tree.246 {tree8 unset root myScalar} {
    list [catch {tree8 unset root myScalar} msg] $msg
} {0 {}}

test tree.247 {tree8 unset root myArray(0)} {
    list [catch {tree8 unset root myArray(0)} msg] $msg
} {0 {}}

test tree.248 {tree8 unset root myArray(badElem)} {
    list [catch {tree8 unset root myArray(badElem)} msg] $msg
} {1 {can't find array element "badElem" in variable "myArray"}}

test tree.249 {tree8 get root} {
    list [catch {tree8 get root} msg] $msg
} {0 {key value key1 value1 key2 value2 key3 value3 abc 123 myArray {1 1}}}

test tree.250 {tree8 unset root myArray} {
    list [catch {tree8 unset root myArray} msg] $msg
} {0 {}}

test tree.251 {tree8 get root} {
    list [catch {tree8 get root} msg] $msg
} {0 {key value key1 value1 key2 value2 key3 value3 abc 123}}


test tree.252 {tree8 set root myArray(1) 1} {
    list [catch {tree8 set root myArray(1) 1} msg] $msg
} {0 {}}

test tree.253 {tree8 set root myArray(2) 2} {
    list [catch {tree8 set root myArray(2) 2} msg] $msg
} {0 {}}

test tree.254 {tree8 set root myArray(3) 3} {
    list [catch {tree8 set root myArray(3) 3} msg] $msg
} {0 {}}

test tree.255 {tree8 set root myScalar abc} {
    list [catch {tree8 set root myScalar abc} msg] $msg
} {0 {}}

test tree.256 {tree8 unset root myArray(1) myArray(3) myScalar} {
    list [catch {tree8 unset root myArray(1) myArray(3) myScalar} msg] $msg
} {0 {}}

test tree.257 {tree8 get root} {
    list [catch {tree8 get root} msg] $msg
} {0 {key value key1 value1 key2 value2 key3 value3 abc 123 myArray {2 2}}}


test tree.258 {tree8 unset root myArray} {
    list [catch {tree8 unset root myArray} msg] $msg
} {0 {}}

test tree.259 {tree8 get root} {
    list [catch {tree8 get root} msg] $msg
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
} {1 {can't find a variable "anotherArr" in tree "::myTree" at node 0}}


test tree.265 {myTree append root def} {
    list [catch {myTree append root def 0} msg] $msg
} {0 {}}

test tree.266 {myTree get root def} {
    list [catch {myTree get root def} msg] $msg
} {0 0}

test tree.267 {myTree lappend} {
    list [catch {myTree lappend} msg] $msg
} {1 {wrong # args: should be "myTree lappend nodeName varName ?value ...?"}}

test tree.268 {myTree lappend badNode} {
    list [catch {myTree lappend badNode} msg] $msg
} {1 {wrong # args: should be "myTree lappend nodeName varName ?value ...?"}}

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
} {1 {wrong # args: should be "myTree lindex nodeName varName index"}}

test tree.279 {myTree lindex root def} {
    list [catch {myTree lindex root def} msg] $msg
} {1 {wrong # args: should be "myTree lindex nodeName varName index"}}

test tree.280 {myTree lindex badNode def 0} {
    list [catch {myTree lindex badNode def 0} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.281 {myTree lindex root badValue 0} {
    list [catch {myTree lindex root badValue 0} msg] $msg
} {1 {can't find a variable "badValue" in tree "::myTree" at node 0}}

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
} {1 {wrong # args: should be "myTree linsert nodeName varName index ?value...?"}}

test tree.293 {myTree linsert badNode} {
    list [catch {myTree linsert badNode} msg] $msg
} {1 {wrong # args: should be "myTree linsert nodeName varName index ?value...?"}}

test tree.294 {myTree linsert badNode badValue} {
    list [catch {myTree linsert badNode badValue} msg] $msg
} {1 {wrong # args: should be "myTree linsert nodeName varName index ?value...?"}}

test tree.295 {myTree linsert badNode badValue 0} {
    list [catch {myTree linsert badNode badValue 0} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.296 {myTree linsert root badValue 0} {
    list [catch {myTree linsert root badValue 0} msg] $msg
} {1 {can't find a variable "badValue" in tree "::myTree" at node 0}}

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
} {1 {can't find a variable "myArr2(0)" in tree "::myTree" at node 0}}

test tree.313 {myTree linsert root myScalar 0 a b c } {
    list [catch {myTree linsert root myScalar 0 a b c } msg] $msg
} {1 {can't find a variable "myScalar" in tree "::myTree" at node 0}}

test tree.314 {myTree set root def "0 1 2 3 4"} {
    list [catch {myTree set root def "0 1 2 3 4"} msg] $msg
} {0 {}}

test tree.315 {myTree llength} {
    list [catch {myTree llength} msg] $msg
} {1 {wrong # args: should be "myTree llength nodeName varName"}}

test tree.316 {myTree llength root} {
    list [catch {myTree llength root} msg] $msg
} {1 {wrong # args: should be "myTree llength nodeName varName"}}

test tree.317 {myTree llength badNode def} {
    list [catch {myTree llength badNode def} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.318 {myTree llength root badValue} {
    list [catch {myTree llength root badValue} msg] $msg
} {1 {can't find a variable "badValue" in tree "::myTree" at node 0}}

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
} {1 {wrong # args: should be "myTree lrange nodeName varName first last"}}

test tree.324 {myTree lrange root def} {
    list [catch {myTree lrange root def} msg] $msg
} {1 {wrong # args: should be "myTree lrange nodeName varName first last"}}

test tree.325 {myTree lrange badNode def 0 0} {
    list [catch {myTree lrange badNode def 0 0} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.326 {myTree lrange root badValue 0 0} {
    list [catch {myTree lrange root badValue 0 0} msg] $msg
} {1 {can't find a variable "badValue" in tree "::myTree" at node 0}}

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
} {1 {wrong # args: should be "myTree lreplace nodeName varName first last ?value...?"}}

test tree.336 {myTree lreplace badNode} {
    list [catch {myTree lreplace badNode} msg] $msg
} {1 {wrong # args: should be "myTree lreplace nodeName varName first last ?value...?"}}

test tree.337 {myTree lreplace badNode badValue} {
    list [catch {myTree lreplace badNode badValue} msg] $msg
} {1 {wrong # args: should be "myTree lreplace nodeName varName first last ?value...?"}}

test tree.338 {myTree lreplace badNode badValue 0 0} {
    list [catch {myTree lreplace badNode badValue 0 0} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.339 {myTree lreplace root badValue 0 0} {
    list [catch {myTree lreplace root badValue 0 0} msg] $msg
} {1 {can't find a variable "badValue" in tree "::myTree" at node 0}}

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
} {1 {can't find a variable "myArr2(0)" in tree "::myTree" at node 0}}

test tree.356 {myTree lreplace root myScalar 0 0 a b c } {
    list [catch {myTree lreplace root myScalar 0 0 a b c } msg] $msg
} {1 {can't find a variable "myScalar" in tree "::myTree" at node 0}}

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
} {1 {wrong # args: should be "myTree exists nodeName ?varName?"}}

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
} {1 {wrong # args: should be "myTree names nodeName ?varName?"}}

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
} {1 {can't find a variable "badValue" in tree "::myTree" at node 0}}

test tree.394 {myTree names root badValue extraArg} {
    list [catch {myTree names root badValue extraArg} msg] $msg
} {1 {wrong # args: should be "myTree names nodeName ?varName?"}}

test tree.395 {myTree names root myArr} {
    list [catch {myTree names root myArr} msg] $msg
} {0 0}

test tree.396 {myTree type} {
    list [catch {myTree type} msg] $msg
} {1 {wrong # args: should be "myTree type nodeName varName"}}

test tree.397 {myTree type badNode} {
    list [catch {myTree type badNode} msg] $msg
} {1 {wrong # args: should be "myTree type nodeName varName"}}

test tree.398 {myTree type badNode badValue} {
    list [catch {myTree type badNode badValue} msg] $msg
} {1 {can't find tag or id "badNode" in ::myTree}}

test tree.399 {myTree type root badValue} {
    list [catch {myTree type root badValue} msg] $msg
} {1 {can't find a variable "badValue" in tree "::myTree" at node 0}}

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
} {0 ::tree9}

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

test tree.414 {::tree9 dump root} {
    list [catch {::tree9 dump root} msg] $msg
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

test tree.455 {blt::tree destroy ::tree9} {
    list [catch {blt::tree destroy ::tree9} msg] $msg
} {0 {}}


test tree.456 {tree8 restore stuff} {
    list [catch {
	set data [tree8 dump root -version 2.0]
	blt::tree create
	tree10 restore root -data $data
	set data [tree10 dump root -version 2.0]
	blt::tree destroy tree10
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

test tree.457 {tree8 restore 0 -file test.dump} {
    list [catch {
	blt::tree create
	tree11 restore root -file test.dump
	set data [tree11 dump root -version 2.0]
	blt::tree destroy tree11
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


test tree.458 {tree8 unset 0 key1} {
    list [catch {tree8 unset 0 key1} msg] $msg
} {0 {}}

test tree.459 {tree8 get 0} {
    list [catch {tree8 get 0} msg] $msg
} {0 {key value key2 value2 key3 value3 abc 123}}

test tree.460 {tree8 unset 0 key2 key3} {
    list [catch {tree8 unset 0 key2 key3} msg] $msg
} {0 {}}

test tree.461 {tree8 get 0} {
    list [catch {tree8 get 0} msg] $msg
} {0 {key value abc 123}}

test tree.462 {tree8 unset 0} {
    list [catch {tree8 unset 0} msg] $msg
} {0 {}}

test tree.463 {tree8 unset 0 (all)} {
    list [catch {eval tree8 unset 0 [tree8 names 0]} msg] $msg
} {0 {}}

test tree.464 {tree8 get 0} {
    list [catch {tree8 get 0} msg] $msg
} {0 {}}

test tree.465 {tree8 unset all abc} {
    list [catch {tree8 unset all abc} msg] $msg
} {0 {}}

test tree.466 {tree8 restore stuff} {
    list [catch {
	set data [tree8 dump root -version 2.0]
	blt::tree create tmp
	tmp restore root -data $data
	set data [tmp dump root -version 2.0]
	blt::tree destroy tmp
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

test tree.467 {tree8 restore (missing arg)} {
    list [catch {tree8 restore} msg] $msg
} {1 {wrong # args: should be "tree8 restore nodeName ?switches ...?"}}

test tree.468 {tree8 restore 0 badSwitch} {
    list [catch {tree8 restore 0 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -data data
   -file fileName
   -metadata varName
   -notags 
   -overwrite }}


test tree.469 {tree8 restore 0 {} arg (extra arg)} {
    list [catch {tree8 restore 0 {} arg} msg] $msg
} {1 {unknown switch ""
The following switches are available:
   -data data
   -file fileName
   -metadata varName
   -notags 
   -overwrite }}


test tree.470 {tree8 size (missing arg)} {
    list [catch {tree8 size} msg] $msg
} {1 {wrong # args: should be "tree8 size nodeName"}}

test tree.471 {tree8 size 0} {
    list [catch {tree8 size 0} msg] $msg
} {0 12}

test tree.472 {tree8 size all} {
    list [catch {tree8 size all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.473 {tree8 size 0 10 (extra arg)} {
    list [catch {tree8 size 0 10} msg] $msg
} {1 {wrong # args: should be "tree8 size nodeName"}}

test tree.474 {tree8 delete (no args)} {
    list [catch {tree8 delete} msg] $msg
} {0 {}}

test tree.475 {tree8 delete badNode} {
    list [catch {tree8 delete badNode} msg] $msg
} {1 {can't find tag or id "badNode" to delete in ::tree8}}

test tree.476 {tree8 delete 11} {
    list [catch {tree8 delete 11} msg] $msg
} {0 {}}

test tree.477 {tree8 delete 11} {
    list [catch {tree8 delete 11} msg] $msg
} {1 {can't find tag or id "11" in ::tree8}}

test tree.478 {tree8 delete 9 12} {
    list [catch {tree8 delete 9 12} msg] $msg
} {0 {}}

test tree.479 {tree8 dump 0 -version 2.0} {
    list [catch {tree8 dump 0 -version 2.0} msg] $msg
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
	set data [tree8 dump root -version 2.0]
	blt::tree create tmp
	tmp restore root -data $data
	tmp delete all
	set data [tmp dump root -version 2.0]
	blt::tree destroy tmp
	set data
	} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
}}

test tree.481 {delete all all} {
    list [catch {
	set data [tree8 dump root -version 2.0]
	blt::tree create tmp
	tmp restore root -data $data
	tmp delete all all
	set data [tmp dump root -version 2.0]
	blt::tree destroy tmp
	set data
	} msg] $msg
} {0 {# V2.0
-1 0 {{}} {} {}
}}

test tree.482 {tree8 apply (missing arg)} {
    list [catch {tree8 apply} msg] $msg
} {1 {wrong # args: should be "tree8 apply nodeName ?switches ...?"}}

test tree.483 {tree8 apply 0} {
    list [catch {tree8 apply 0} msg] $msg
} {0 {}}

test tree.484 {tree8 apply 0 -badSwitch} {
    list [catch {tree8 apply 0 -badSwitch} msg] $msg
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

test tree.485 {tree8 apply badTag} {
    list [catch {tree8 apply badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::tree8}}

test tree.486 {tree8 apply all} {
    list [catch {tree8 apply all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.487 {tree8 apply myTag -precommand lappend} {
    list [catch {
	set mylist {}
	tree8 apply myTag -precommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {5 13}}

test tree.488 {tree8 apply root -precommand lappend} {
    list [catch {
	set mylist {}
	tree8 apply root -precommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {0 1 2 3 4 5 13 6 8}}

test tree.489 {tree8 apply -postcommand} {
    list [catch {
	set mylist {}
	tree8 apply root -postcommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.490 {tree8 apply -precommand -postcommand} {
    list [catch {
	set mylist {}
	tree8 apply root -precommand {lappend mylist} \
		-postcommand {lappend mylist}
	set mylist
    } msg] $msg
} {0 {0 1 1 2 2 3 3 4 4 5 13 13 5 6 6 8 8 0}}

test tree.491 {tree8 apply root -precommand lappend -depth 1} {
    list [catch {
	set mylist {}
	tree8 apply root -precommand {lappend mylist} -depth 1
	set mylist
    } msg] $msg
} {0 {0 1 2 3 4 5 6 8}}


test tree.492 {tree8 apply root -precommand -depth 0} {
    list [catch {
	set mylist {}
	tree8 apply root -precommand {lappend mylist} -depth 0
	set mylist
    } msg] $msg
} {0 0}

test tree.493 {tree8 apply root -precommand -tag myTag} {
    list [catch {
	set mylist {}
	tree8 apply root -precommand {lappend mylist} -tag myTag
	set mylist
    } msg] $msg
} {0 5}


test tree.494 {tree8 apply root -precommand -key key1} {
    list [catch {
	set mylist {}
	tree8 set myTag key1 0.0
	tree8 apply root -precommand {lappend mylist} -key key1
	tree8 unset myTag key1
	set mylist
    } msg] $msg
} {0 5}

test tree.495 {tree8 apply root -postcommand -regexp node.*} {
    list [catch {
	set mylist {}
	tree8 set myTag key1 0.0
	tree8 apply root -precommand {lappend mylist} -regexp {node5} 
	tree8 unset myTag key1
	set mylist
    } msg] $msg
} {0 5}

test tree.496 {tree8 find (missing arg)} {
    list [catch {tree8 find} msg] $msg
} {1 {wrong # args: should be "tree8 find nodeName ?switches ...?"}}

test tree.497 {tree8 find 0} {
    list [catch {tree8 find 0} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.498 {tree8 find root} {
    list [catch {tree8 find root} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.499 {tree8 find 0 -glob node*} {
    list [catch {tree8 find root -glob node*} msg] $msg
} {0 {1 2 3 4 13 5 6}}

test tree.500 {tree8 find 0 -glob nobody} {
    list [catch {tree8 find root -glob nobody} msg] $msg
} {0 {}}

test tree.501 {tree8 find 0 -regexp {node[0-3]}} {
    list [catch {tree8 find root -regexp {node[0-3]}} msg] $msg
} {0 {1 2 3 13}}

test tree.502 {tree8 find 0 -regexp {.*[A-Z].*}} {
    list [catch {tree8 find root -regexp {.*[A-Z].*}} msg] $msg
} {0 8}

test tree.503 {tree8 find 0 -exact myLabel} {
    list [catch {tree8 find root -exact myLabel} msg] $msg
} {0 8}

test tree.504 {tree8 find 0 -exact myLabel -invert} {
    list [catch {tree8 find root -exact myLabel -invert} msg] $msg
} {0 {1 2 3 4 13 5 6 0}}


test tree.505 {tree8 find 3 -exact node3} {
    list [catch {tree8 find 3 -exact node3} msg] $msg
} {0 3}

test tree.506 {tree8 find 0 -nocase -exact mylabel} {
    list [catch {tree8 find 0 -nocase -exact mylabel} msg] $msg
} {0 8}

test tree.507 {tree8 find 0 -nocase} {
    list [catch {tree8 find 0 -nocase} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.508 {tree8 find 0 -path -nocase -glob *node1* } {
    list [catch {tree8 find 0 -path -nocase -glob *node1*} msg] $msg
} {0 {1 13}}

test tree.509 {tree8 find 0 -count 5 } {
    list [catch {tree8 find 0 -count 5} msg] $msg
} {0 {1 2 3 4 13}}

test tree.510 {tree8 find 0 -count -5 } {
    list [catch {tree8 find 0 -count -5} msg] $msg
} {1 {bad value "-5": can't be negative}}

test tree.511 {tree8 find 0 -count badValue } {
    list [catch {tree8 find 0 -count badValue} msg] $msg
} {1 {expected integer but got "badValue"}}

test tree.512 {tree8 find 0 -count badValue } {
    list [catch {tree8 find 0 -count badValue} msg] $msg
} {1 {expected integer but got "badValue"}}

test tree.513 {tree8 find 0 -leafonly} {
    list [catch {tree8 find 0 -leafonly} msg] $msg
} {0 {1 2 3 4 13 6 8}}

test tree.514 {tree8 find 0 -leafonly -glob {node[18]}} {
    list [catch {tree8 find 0 -glob {node[18]} -leafonly} msg] $msg
} {0 1}

test tree.515 {tree8 find 0 -depth 0} {
    list [catch {tree8 find 0 -depth 0} msg] $msg
} {0 0}

test tree.516 {tree8 find 0 -depth 1} {
    list [catch {tree8 find 0 -depth 1} msg] $msg
} {0 {1 2 3 4 5 6 8 0}}

test tree.517 {tree8 find 0 -depth 2} {
    list [catch {tree8 find 0 -depth 2} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.518 {tree8 find 0 -depth 20} {
    list [catch {tree8 find 0 -depth 20} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.519 {tree8 find 1 -depth 0} {
    list [catch {tree8 find 1 -depth 0} msg] $msg
} {0 1}

test tree.520 {tree8 find 1 -depth 1} {
    list [catch {tree8 find 1 -depth 1} msg] $msg
} {0 1}

test tree.521 {tree8 find 1 -depth 2} {
    list [catch {tree8 find 1 -depth 2} msg] $msg
} {0 1}

test tree.522 {tree8 find all} {
    list [catch {tree8 find all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.523 {tree8 find badTag} {
    list [catch {tree8 find badTag} msg] $msg
} {1 {can't find tag or id "badTag" in ::tree8}}

test tree.524 {tree8 find 0 -addtag hi} {
    list [catch {tree8 find 0 -addtag hi} msg] $msg
} {0 {1 2 3 4 13 5 6 8 0}}

test tree.525 {tree8 find 0 -addtag all} {
    list [catch {tree8 find 0 -addtag all} msg] $msg
} {1 {can't add reserved tag "all"}}

test tree.526 {tree8 find 0 -addtag root} {
    list [catch {tree8 find 0 -addtag root} msg] $msg
} {1 {can't add reserved tag "root"}}

test tree.527 {tree8 find 0 -exec {lappend list} -leafonly} {
    list [catch {
	set list {}
	tree8 find 0 -exec {lappend list} -leafonly
	set list
	} msg] $msg
} {0 {1 2 3 4 13 6 8}}

test tree.528 {tree8 find 0 -tag root} {
    list [catch {tree8 find 0 -tag root} msg] $msg
} {0 0}

test tree.529 {tree8 find 0 -tag myTag} {
    list [catch {tree8 find 0 -tag myTag} msg] $msg
} {0 5}

test tree.530 {tree8 find 0 -tag badTag} {
    list [catch {tree8 find 0 -tag badTag} msg] $msg
} {0 {}}

test tree.531 {tree8 tag (missing args)} {
    list [catch {tree8 tag} msg] $msg
} {1 {wrong # args: should be "tree8 tag args ..."}}

test tree.532 {tree8 tag badOp} {
    list [catch {tree8 tag badOp} msg] $msg
} {1 {bad operation "badOp": should be one of...
  tree8 tag add tag ?nodeName...?
  tree8 tag delete tag nodeName...
  tree8 tag exists tag ?nodeName?
  tree8 tag forget tag...
  tree8 tag get nodeName ?pattern...?
  tree8 tag names ?nodeName...?
  tree8 tag nodes tag ?tag...?
  tree8 tag set nodeName tag...
  tree8 tag unset nodeName tag...}}

test tree.533 {tree8 tag add} {
    list [catch {tree8 tag add} msg] $msg
} {1 {wrong # args: should be "tree8 tag add tag ?nodeName...?"}}

test tree.534 {tree8 tag add newTag} {
    list [catch {tree8 tag add newTag} msg] $msg
} {0 {}}

test tree.535 {tree8 tag add tag badNode} {
    list [catch {tree8 tag add tag badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.536 {tree8 tag add newTag root} {
    list [catch {tree8 tag add newTag root} msg] $msg
} {0 {}}

test tree.537 {tree8 tag add newTag all} {
    list [catch {tree8 tag add newTag all} msg] $msg
} {0 {}}

test tree.538 {tree8 tag add tag2 0 1 2 3 4} {
    list [catch {tree8 tag add tag2 0 1 2 3 4} msg] $msg
} {0 {}}

test tree.539 {tree8 tag exists tag2} {
    list [catch {tree8 tag exists tag2} msg] $msg
} {0 1}

test tree.540 {tree8 tag exists tag2 0} {
    list [catch {tree8 tag exists tag2 0} msg] $msg
} {0 1}

test tree.541 {tree8 tag exists tag2 5} {
    list [catch {tree8 tag exists tag2 5} msg] $msg
} {0 0}

test tree.542 {tree8 tag exists badTag} {
    list [catch {tree8 tag exists badTag} msg] $msg
} {0 0}

test tree.543 {tree8 tag exists badTag 1000} {
    list [catch {tree8 tag exists badTag 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree8}}

test tree.544 {tree8 tag add tag2 0 1 2 3 4 1000} {
    list [catch {tree8 tag add tag2 0 1 2 3 4 1000} msg] $msg
} {1 {can't find tag or id "1000" in ::tree8}}

test tree.545 {tree8 tag names} {
    list [catch {tree8 tag names} msg] [lsort $msg]
} {0 {all hi myTag myTag1 myTag2 newTag root tag2 thisTag}}

test tree.546 {tree8 tag names badNode} {
    list [catch {tree8 tag names badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.547 {tree8 tag names all} {
    list [catch {tree8 tag names all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.548 {tree8 tag names root} {
    list [catch {tree8 tag names root} msg] [lsort $msg]
} {0 {all hi newTag root tag2}}

test tree.549 {tree8 tag names 0 1} {
    list [catch {tree8 tag names 0 1} msg] [lsort $msg]
} {0 {all hi newTag root tag2}}

test tree.550 {tree8 tag nodes (missing arg)} {
    list [catch {tree8 tag nodes} msg] $msg
} {1 {wrong # args: should be "tree8 tag nodes tag ?tag...?"}}

test tree.551 {tree8 tag nodes root badTag} {
    # It's not an error to use bad tag.
    list [catch {tree8 tag nodes root badTag} msg] $msg
} {0 {}}

test tree.552 {tree8 tag nodes root tag2} {
    list [catch {tree8 tag nodes root tag2} msg] [lsort $msg]
} {0 {0 1 2 3 4}}

test tree.553 {tree8 tag set} {
    list [catch {tree8 tag set} msg] $msg
} {1 {wrong # args: should be "tree8 tag set nodeName tag..."}}

test tree.554 {tree8 tag set badNode} {
    list [catch {tree8 tag set badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.555 {tree8 tag set root root} {
    list [catch {tree8 tag set root root} msg] $msg
} {1 {can't add reserved tag "root"}}

test tree.556 {tree8 tag set root all} {
    list [catch {tree8 tag set root all} msg] $msg
} {1 {can't add reserved tag "all"}}

test tree.557 {tree8 tag set root 100} {
    list [catch {tree8 tag set root 100} msg] $msg
} {1 {bad tag "100": can't be a number}}

test tree.558 {tree8 tag set root myTag} {
    list [catch {tree8 tag set root myTag} msg] $msg
} {0 {}}

test tree.559 {tree8 tag get} {
    list [catch {tree8 tag get} msg] $msg
} {1 {wrong # args: should be "tree8 tag get nodeName ?pattern...?"}}

test tree.560 {tree8 tag get badNode} {
    list [catch {tree8 tag get badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.561 {tree8 tag get root badTag} {
    list [catch {tree8 tag get root badTag} msg] $msg
} {0 {}}

test tree.562 {tree8 tag get root} {
    list [catch {tree8 tag get root} msg] $msg
} {0 {root hi tag2 newTag myTag all}}

test tree.563 {tree8 tag get root myTag} {
    list [catch {tree8 tag get root myTag} msg] $msg
} {0 myTag}

test tree.564 {tree8 tag get root root} {
    list [catch {tree8 tag get root root} msg] $msg
} {0 root}

test tree.565 {tree8 tag get root all} {
    list [catch {tree8 tag get root all} msg] $msg
} {0 all}

test tree.566 {tree8 tag names root} {
    list [catch {tree8 tag names root} msg] $msg
} {0 {all root hi myTag newTag tag2}}

test tree.567 {tree8 tag unset} {
    list [catch {tree8 tag unset} msg] $msg
} {1 {wrong # args: should be "tree8 tag unset nodeName tag..."}}

test tree.568 {tree8 tag unset badNode} {
    list [catch {tree8 tag unset badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}


test tree.569 {tree8 tag unset root} {
    list [catch {tree8 tag unset root} msg] $msg
} {0 {}}

test tree.570 {tree8 tag unset root badTag} {
    list [catch {tree8 tag unset root badTag} msg] $msg
} {0 {}}

test tree.571 {tree8 tag names root} {
    list [catch {tree8 tag names root} msg] $msg
} {0 {all root hi myTag newTag tag2}}

test tree.572 {tree8 tag unset root myTag} {
    list [catch {tree8 tag unset root myTag} msg] $msg
} {0 {}}

test tree.573 {tree8 tag names root} {
    list [catch {tree8 tag names root} msg] $msg
} {0 {all root hi newTag tag2}}

test tree.574 {tree8 ancestor (missing arg)} {
    list [catch {tree8 ancestor} msg] $msg
} {1 {wrong # args: should be "tree8 ancestor node1 node2"}}

test tree.575 {tree8 ancestor 0 (missing arg)} {
    list [catch {tree8 ancestor 0} msg] $msg
} {1 {wrong # args: should be "tree8 ancestor node1 node2"}}

test tree.576 {tree8 ancestor 0 10} {
    list [catch {tree8 ancestor 0 10} msg] $msg
} {1 {can't find tag or id "10" in ::tree8}}

test tree.577 {tree8 ancestor 0 4} {
    list [catch {tree8 ancestor 0 4} msg] $msg
} {0 0}

test tree.578 {tree8 ancestor 1 8} {
    list [catch {tree8 ancestor 1 8} msg] $msg
} {0 0}

test tree.579 {tree8 ancestor root 0} {
    list [catch {tree8 ancestor root 0} msg] $msg
} {0 0}

test tree.580 {tree8 ancestor 8 8} {
    list [catch {tree8 ancestor 8 8} msg] $msg
} {0 8}

test tree.581 {tree8 ancestor 0 all} {
    list [catch {tree8 ancestor 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.582 {tree8 ancestor 7 9} {
    list [catch {
	set n1 1; set n2 1;
	for { set i 0 } { $i < 4 } { incr i } {
	    set n1 [tree8 insert $n1]
	    set n2 [tree8 insert $n2]
	}
	tree8 ancestor $n1 $n2
	} msg] $msg
} {0 1}

test tree.583 {tree8 path (missing arg)} {
    list [catch {tree8 path} msg] $msg
} {1 {wrong # args: should be "tree8 path ?args ...?"}}

test tree.584 {tree8 path badArg} {
    list [catch {tree8 path badArg} msg] $msg
} {1 {bad operation "badArg": should be one of...
  tree8 path create pathName ?switches ...?
  tree8 path parse pathName ?switches ...?
  tree8 path print nodeName ?switches ...?
  tree8 path separator ?sepString?}}

test tree.585 {tree8 path print root} {
    list [catch {tree8 path print root} msg] $msg
} {0 {}}

test tree.586 {tree8 path print 0} {
    list [catch {tree8 path print 0} msg] $msg
} {0 {}}

test tree.587 {tree8 path print 15} {
    list [catch {tree8 path print 15} msg] $msg
} {0 {node1 node15}}

test tree.588 {tree8 path print 15} {
    list [catch {tree8 path print 15 -separator /} msg] $msg
} {0 /node1/node15}

test tree.589 {tree8 path print 16} {
    list [catch {tree8 path print 16 -separator /} msg] $msg
} {0 /node1/node14/node16}

test tree.590 {tree8 path parse /} {
    list [catch {tree8 path parse / -separator /} msg] $msg
} {0 0}

test tree.591 {tree8 path parse /node1} {
    list [catch {tree8 path parse /node1 -separator /} msg] $msg
} {0 1}

test tree.592 {tree8 path parse /node1/node14} {
    list [catch {tree8 path parse /node1/node14 -separator /} msg] $msg
} {0 14}

test tree.593 {tree8 path parse } {
    list [catch {tree8 path parse /node1/node14/node16 -separator /} msg] $msg
} {0 16}

test tree.594 {tree8 path parse } {
    list [catch {tree8 path parse /node1/node14/node16/ -separator /} msg] $msg
} {0 16}

test tree.595 {tree8 path parse } {
    list [catch {tree8 path parse //node1//node14//node16// -separator /} msg] $msg
} {0 16}

test tree.596 {tree8 path parse } {
    list [catch {tree8 path parse ::node1::node14::node16 -separator ::} msg] $msg
} {0 16}

test tree.597 {tree8 path parse } {
    list [catch {tree8 path parse /node1/node14/node16 -separator /} msg] $msg
} {0 16}

test tree.598 {tree8 path parse } {
    list [catch {tree8 path parse /node1/node14/node16/ -separator /} msg] $msg
} {0 16}

test tree.599 {tree8 path parse } {
    list [catch {tree8 path parse //node1//node14//node16// -separator /} msg] $msg
} {0 16}

test tree.600 {tree8 path parse } {
    list [catch {tree8 path parse ::node1::node14::node16 -separator ::} msg] $msg
} {0 16}

test tree.601 {tree8 path parse ""} {
    list [catch {tree8 path parse {}} msg] $msg
} {0 0}

test tree.602 {tree8 path parse node1 } {
    list [catch {tree8 path parse node1} msg] $msg
} {0 1}

test tree.603 {tree8 path parse {node1 node14}} {
    list [catch {tree8 path parse {node1 node14}} msg] $msg
} {0 14}

test tree.604 {tree8 path parse } {
    list [catch {tree8 path parse {node1 node14 node16}} msg] $msg
} {0 16}


test tree.605 {tree8 path print all} {
    list [catch {tree8 path print all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.606 {tree8 path print 0 badSwitch} {
    list [catch {tree8 path print 0 badSwitch} msg] $msg
} {1 {unknown switch "badSwitch"
The following switches are available:
   -from node
   -separator char
   -noleadingseparator }}


test tree.607 {tree8 tag forget} {
    list [catch {tree8 tag forget} msg] $msg
} {1 {wrong # args: should be "tree8 tag forget tag..."}}

test tree.608 {tree8 tag forget badTag} {
    list [catch {
	tree8 tag forget badTag
	lsort [tree8 tag names]
    } msg] $msg
} {0 {all hi myTag myTag1 myTag2 newTag root tag2 thisTag}}

test tree.609 {tree8 tag forget all} {
    list [catch {tree8 tag forget all} msg] $msg
} {1 {can't forget reserved tag "all"}}

test tree.610 {tree8 tag forget root} {
    list [catch {tree8 tag forget root} msg] $msg
} {1 {can't forget reserved tag "root"}}

test tree.611 {tree8 tag forget 100} {
    list [catch {tree8 tag forget 100} msg] $msg
} {1 {bad tag "100": can't be a number}}

test tree.612 {tree8 tag forget hi} {
    list [catch {
	tree8 tag forget hi
	lsort [tree8 tag names]
    } msg] $msg
} {0 {all myTag myTag1 myTag2 newTag root tag2 thisTag}}

test tree.613 {tree8 tag forget tag1 tag2} {
    list [catch {
	tree8 tag forget myTag1 myTag2
	lsort [tree8 tag names]
    } msg] $msg
} {0 {all myTag newTag root tag2 thisTag}}

test tree.614 {tree8 tag forget all} {
    list [catch {
	tree8 tag forget all
	lsort [tree8 tag names]
    } msg] $msg
} {1 {can't forget reserved tag "all"}}

test tree.615 {tree8 tag forget root} {
    list [catch {
	tree8 tag forget root
	lsort [tree8 tag names]
    } msg] $msg
} {1 {can't forget reserved tag "root"}}

test tree.616 {tree8 tag delete} {
    list [catch {tree8 tag delete} msg] $msg
} {1 {wrong # args: should be "tree8 tag delete tag nodeName..."}}

test tree.617 {tree8 tag delete tag} {
    list [catch {tree8 tag delete tag} msg] $msg
} {1 {wrong # args: should be "tree8 tag delete tag nodeName..."}}

test tree.618 {tree8 tag delete tag 0} {
    list [catch {tree8 tag delete tag 0} msg] $msg
} {0 {}}

test tree.619 {tree8 tag delete root 0} {
    list [catch {tree8 tag delete root 0} msg] $msg
} {1 {can't delete reserved tag "root"}}

test tree.620 {tree8 move} {
    list [catch {tree8 move} msg] $msg
} {1 {wrong # args: should be "tree8 move nodeName destNode ?switches ...?"}}

test tree.621 {tree8 move 0} {
    list [catch {tree8 move 0} msg] $msg
} {1 {wrong # args: should be "tree8 move nodeName destNode ?switches ...?"}}

test tree.622 {tree8 move 0 0} {
    list [catch {tree8 move 0 0} msg] $msg
} {1 {can't move root node}}

test tree.623 {tree8 move 0 badNode} {
    list [catch {tree8 move 0 badNode} msg] $msg
} {1 {can't find tag or id "badNode" in ::tree8}}

test tree.624 {tree8 move 0 all} {
    list [catch {tree8 move 0 all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.625 {tree8 move 1 0 -before 2} {
    list [catch {
	tree8 move 1 0 -before 2
	tree8 children 0
    } msg] $msg
} {0 {1 2 3 4 5 6 8}}

test tree.626 {tree8 move 1 0 -after 2} {
    list [catch {
	tree8 move 1 0 -after 2
	tree8 children 0
    } msg] $msg
} {0 {2 1 3 4 5 6 8}}

test tree.627 {tree8 move 1 2} {
    list [catch {
	tree8 move 1 2
	tree8 children 0
    } msg] $msg
} {0 {2 3 4 5 6 8}}

test tree.628 {tree8 move 0 2} {
    list [catch {tree8 move 0 2} msg] $msg
} {1 {can't move root node}}

test tree.629 {tree8 move 1 17} {
    list [catch {tree8 move 1 17} msg] $msg
} {1 {can't move node: "1" is an ancestor of "17"}}

test tree.630 {tree8 attach} {
    list [catch {tree8 attach} msg] $msg
} {1 {wrong # args: should be "tree8 attach treeName ?switches ...?"}}

test tree.631 {tree8 attach badTree} {
    list [catch {tree8 attach badTree} msg] $msg
} {1 {can't find a tree named "badTree"}}


test tree.632 {tree8 attach tree2 badArg} {
    list [catch {tree8 attach tree2 badArg} msg] $msg
} {1 {unknown switch "badArg"
The following switches are available:
   -newtags }}


test tree.633 {tmp attach tree8 -newtags} {
    list [catch {
	set tree [blt::tree create]
	$tree attach tree8 -newtags
	set out [$tree dump 0 -version 2.0]
	blt::tree destroy $tree
	set out
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

test tree.634 {tessA attach tree8} {
    list [catch {
	blt::tree create treeA
	treeA attach tree8
	treeA dump 0 -version 2.0
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

test tree.635 {treeA attach ""} {
    list [catch {treeA attach ""} msg] $msg
} {0 {}}


test tree.636 {blt::tree destroy treeA} {
    list [catch {blt::tree destroy treeA} msg] $msg
} {0 {}}

test tree.637 {tree8 find root -badSwitch} {
    list [catch {tree8 find root -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -addtag tagName
   -count number
   -depth number
   -exact string
   -exclude pattern
   -exec command
   -expr exprString
   -glob pattern
   -invert 
   -key string
   -keyexact string
   -keyglob pattern
   -keyregexp pattern
   -leafonly 
   -mindepth number
   -nocase 
   -nodes nodeList
   -order orderName
   -path 
   -regexp pattern
   -tag tagList}}

test tree.638 {tree8 find root -order} {
    list [catch {tree8 find root -order} msg] $msg
} {1 {value for "-order" missing}}

test tree.639 {tree8 find root ...} {
    list [catch {tree8 find root -order preorder -order postorder -order inorder} msg] $msg
} {0 {20 18 16 14 1 21 19 17 15 2 0 3 4 13 5 6 8}}

test tree.640 {tree8 find root -order preorder} {
    list [catch {tree8 find root -order preorder} msg] $msg
} {0 {0 2 1 14 16 18 20 15 17 19 21 3 4 5 13 6 8}}

test tree.641 {tree8 find root -order postorder} {
    list [catch {tree8 find root -order postorder} msg] $msg
} {0 {20 18 16 14 21 19 17 15 1 2 3 4 13 5 6 8 0}}

test tree.642 {tree8 find root -order inorder} {
    list [catch {tree8 find root -order inorder} msg] $msg
} {0 {20 18 16 14 1 21 19 17 15 2 0 3 4 13 5 6 8}}

test tree.643 {tree8 find root -order breadthfirst} {
    list [catch {tree8 find root -order breadthfirst} msg] $msg
} {0 {0 2 3 4 5 6 8 1 13 14 15 16 17 18 19 20 21}}

test tree.644 {tree8 set all key1 myValue} {
    list [catch {tree8 set all key1 myValue} msg] $msg
} {0 {}}

test tree.645 {tree8 set 15 key1 123} {
    list [catch {tree8 set 15 key1 123} msg] $msg
} {0 {}}

test tree.646 {tree8 set 16 key1 1234 key2 abc} {
    list [catch {tree8 set 16 key1 123 key2 abc} msg] $msg
} {0 {}}

test tree.647 {tree8 find root -key } {
    list [catch {tree8 find root -key} msg] $msg
} {1 {value for "-key" missing}}

test tree.648 {tree8 find root -key noKey} {
    list [catch {tree8 find root -key noKey} msg] $msg
} {0 {}}

test tree.649 {tree key root} {
    list [catch { tree8 keys root } msg] $msg
} {0 key1}

test tree.650 {tree key 20 16} {
    list [catch { tree8 keys 20 16 } msg] $msg
} {0 {key1 key2}}

test tree.651 {tree8 find root -key key1} {
    list [catch {tree8 find root -key key1} msg] $msg
} {0 {20 18 16 14 21 19 17 15 1 2 3 4 13 5 6 8 0}}

test tree.652 {tree8 find root -key key2} {
    list [catch {tree8 find root -key key2} msg] $msg
} {0 16}

test tree.653 {tree8 find root -key key2 -exact notThere } {
    list [catch {tree8 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test tree.654 {tree8 find root -key key1 -glob notThere } {
    list [catch {tree8 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test tree.655 {tree8 find root -key badKey -regexp notThere } {
    list [catch {tree8 find root -key key2 -exact notThere } msg] $msg
} {0 {}}

test tree.656 {tree8 find root -key key1 -glob 12*} {
    list [catch {tree8 find root -key key1 -glob 12*} msg] $msg
} {0 {16 15}}

test tree.657 {tree8 sort} {
    list [catch {tree8 sort} msg] $msg
} {1 {wrong # args: should be "tree8 sort nodeName ?switches ...?"}}

test tree.658 {tree8 sort all} {
    list [catch {tree8 sort all} msg] $msg
} {1 {tag "all" refers to more than one node}}

test tree.659 {tree8 sort -recurse} {
    list [catch {tree8 sort -recurse} msg] $msg
} {1 {can't find tag or id "-recurse" in ::tree8}}

test tree.660 {tree8 sort 0} {
    list [catch {tree8 sort 0} msg] $msg
} {0 {8 2 3 4 5 6}}

test tree.661 {tree8 sort 0 -recurse} {
    list [catch {tree8 sort 0 -recurse} msg] $msg
} {0 {0 8 1 2 3 4 5 6 13 14 15 16 17 18 19 20 21}}

test tree.662 {tree8 sort 0 -decreasing -key} {
    list [catch {tree8 sort 0 -decreasing -key} msg] $msg
} {1 {value for "-key" missing}}

test tree.663 {tree8 sort 0 -re} {
    list [catch {tree8 sort 0 -re} msg] $msg
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


test tree.664 {tree8 sort 0 -decreasing} {
    list [catch {tree8 sort 0 -decreasing} msg] $msg
} {0 {6 5 4 3 2 8}}

test tree.665 {tree8 sort 0} {
    list [catch {
	set list {}
	foreach n [tree8 sort 0] {
	    lappend list [tree8 label $n]
	}	
	set list
    } msg] $msg
} {0 {myLabel node2 node3 node4 node5 node6}}

test tree.666 {tree8 sort 0 -decreasing} {
    list [catch {tree8 sort 0 -decreasing} msg] $msg
} {0 {6 5 4 3 2 8}}


test tree.667 {tree8 sort 0 -decreasing -key} {
    list [catch {tree8 sort 0 -decreasing -key} msg] $msg
} {1 {value for "-key" missing}}

test tree.668 {tree8 sort 0 -decreasing -key key1} {
    list [catch {tree8 sort 0 -decreasing -key key1} msg] $msg
} {0 {8 6 5 4 3 2}}

test tree.669 {tree8 sort 0 -decreasing -recurse -key key1} {
    list [catch {tree8 sort 0 -decreasing -recurse -key key1} msg] $msg
} {0 {15 16 0 1 2 3 4 5 6 8 13 14 17 18 19 20 21}}

test tree.670 {tree8 sort 0 -decreasing -key key1} {
    list [catch {
	set list {}
	foreach n [tree8 sort 0 -decreasing -key key1] {
	    lappend list [tree8 get $n key1]
	}
	set list
    } msg] $msg
} {0 {myValue myValue myValue myValue myValue myValue}}


test tree.671 {tree8 index 1->firstchild} {
    list [catch {tree8 index 1->firstchild} msg] $msg
} {0 14}

test tree.672 {tree8 index root->firstchild} {
    list [catch {tree8 index root->firstchild} msg] $msg
} {0 2}

test tree.673 {tree8 label root->parent} {
    list [catch {tree8 label root->parent} msg] $msg
} {1 {can't find tag or id "root->parent" in ::tree8}}

test tree.674 {tree8 index root->parent} {
    list [catch {tree8 index root->parent} msg] $msg
} {0 -1}

test tree.675 {tree8 index root->lastchild} {
    list [catch {tree8 index root->lastchild} msg] $msg
} {0 8}

test tree.676 {tree8 index root->next} {
    list [catch {tree8 index root->next} msg] $msg
} {0 2}

test tree.677 {tree8 index root->previous} {
    list [catch {tree8 index root->previous} msg] $msg
} {0 -1}

test tree.678 {tree8 label root->previous} {
    list [catch {tree8 label root->previous} msg] $msg
} {1 {can't find tag or id "root->previous" in ::tree8}}

test tree.679 {tree8 index 1->previous} {
    list [catch {tree8 index 1->previous} msg] $msg
} {0 2}

test tree.680 {tree8 label root->badModifier} {
    list [catch {tree8 label root->badModifier} msg] $msg
} {1 {can't find tag or id "root->badModifier" in ::tree8}}

test tree.681 {tree8 index root->badModifier} {
    list [catch {tree8 index root->badModifier} msg] $msg
} {0 -1}

test tree.682 {tree8 index root->firstchild->parent} {
    list [catch {tree8 index root->firstchild->parent} msg] $msg
} {0 0}

test tree.683 {tree8 trace} {
    list [catch {tree8 trace} msg] $msg
} {1 {wrong # args: should be one of...
  tree8 trace create nodeName varName how command ?-whenidle?
  tree8 trace delete traceName ...
  tree8 trace info traceName
  tree8 trace names ?pattern ...?}}

test tree.684 {tree8 trace create} {
    list [catch {tree8 trace create} msg] $msg
} {1 {wrong # args: should be "tree8 trace create nodeName varName how command ?-whenidle?"}}

test tree.685 {tree8 trace create root} {
    list [catch {tree8 trace create root} msg] $msg
} {1 {wrong # args: should be "tree8 trace create nodeName varName how command ?-whenidle?"}}

test tree.686 {tree8 trace create root * } {
    list [catch {tree8 trace create root * } msg] $msg
} {1 {wrong # args: should be "tree8 trace create nodeName varName how command ?-whenidle?"}}

test tree.687 {tree8 trace create root * rwuc} {
    list [catch {tree8 trace create root * rwuc} msg] $msg
} {1 {wrong # args: should be "tree8 trace create nodeName varName how command ?-whenidle?"}}

test tree.688 {tree8 trace create root * badFlags Doit} {
    list [catch {tree8 trace create root * badFlags Doit} msg] $msg
} {1 {unknown flag in "badFlags"}}

test tree.689 {tree8 trace create root * rcuw Doit -badSwitch} {
    list [catch {tree8 trace create root * rcuw Doit -badSwitch} msg] $msg
} {1 {unknown switch "-badSwitch"
The following switches are available:
   -whenidle }}


proc Doit args { global mylist; lappend mylist $args }

test tree.690 {tree8 trace create all newKey rwuc Doit} {
    list [catch {tree8 trace create all newKey rwuc Doit} msg] $msg
} {0 trace0}

test tree.691 {tree8 trace names} {
    list [catch {tree8 trace names} msg] $msg
} {0 trace0}

test tree.692 {tree8 trace names badPattern} {
    list [catch {tree8 trace names badPattern} msg] $msg
} {0 {}}

test tree.693 {tree8 trace names t*} {
    list [catch {tree8 trace names t*} msg] $msg
} {0 trace0}

test tree.694 {tree8 trace info trace0} {
    list [catch {tree8 trace info trace0} msg] $msg
} {0 {all newKey rwuc Doit}}

test tree.695 {test create trace} {
    list [catch {
	set mylist {}
	tree8 set all newKey 20
	set mylist
	} msg] $msg
} {0 {{::tree8 0 newKey wc} {::tree8 2 newKey wc} {::tree8 1 newKey wc} {::tree8 14 newKey wc} {::tree8 16 newKey wc} {::tree8 18 newKey wc} {::tree8 20 newKey wc} {::tree8 15 newKey wc} {::tree8 17 newKey wc} {::tree8 19 newKey wc} {::tree8 21 newKey wc} {::tree8 3 newKey wc} {::tree8 4 newKey wc} {::tree8 5 newKey wc} {::tree8 13 newKey wc} {::tree8 6 newKey wc} {::tree8 8 newKey wc}}}

test tree.696 {test read trace} {
    list [catch {
	set mylist {}
	tree8 get root newKey
	set mylist
	} msg] $msg
} {0 {{::tree8 0 newKey r}}}

test tree.697 {test write trace} {
    list [catch {
	set mylist {}
	tree8 set all newKey 21
	set mylist
	} msg] $msg
} {0 {{::tree8 0 newKey w} {::tree8 2 newKey w} {::tree8 1 newKey w} {::tree8 14 newKey w} {::tree8 16 newKey w} {::tree8 18 newKey w} {::tree8 20 newKey w} {::tree8 15 newKey w} {::tree8 17 newKey w} {::tree8 19 newKey w} {::tree8 21 newKey w} {::tree8 3 newKey w} {::tree8 4 newKey w} {::tree8 5 newKey w} {::tree8 13 newKey w} {::tree8 6 newKey w} {::tree8 8 newKey w}}}

test tree.698 {test unset trace} {
    list [catch {
	set mylist {}
	tree8 set all newKey 21
	set mylist
	} msg] $msg
} {0 {{::tree8 0 newKey w} {::tree8 2 newKey w} {::tree8 1 newKey w} {::tree8 14 newKey w} {::tree8 16 newKey w} {::tree8 18 newKey w} {::tree8 20 newKey w} {::tree8 15 newKey w} {::tree8 17 newKey w} {::tree8 19 newKey w} {::tree8 21 newKey w} {::tree8 3 newKey w} {::tree8 4 newKey w} {::tree8 5 newKey w} {::tree8 13 newKey w} {::tree8 6 newKey w} {::tree8 8 newKey w}}}

test tree.699 {tree8 trace delete} {
    list [catch {tree8 trace delete} msg] $msg
} {0 {}}

test tree.700 {tree8 trace delete badId} {
    list [catch {tree8 trace delete badId} msg] $msg
} {1 {unknown trace "badId"}}

test tree.701 {tree8 trace delete trace0} {
    list [catch {tree8 trace delete trace0} msg] $msg
} {0 {}}

test tree.702 {test create trace} {
    list [catch {
	set mylist {}
	tree8 set all newKey 20
	set mylist
	} msg] $msg
} {0 {}}

test tree.703 {test unset trace} {
    list [catch {
	set mylist {}
	tree8 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}


test tree.704 {tree8 notify} {
    list [catch {tree8 notify} msg] $msg
} {1 {wrong # args: should be one of...
  tree8 notify create ?switches ...? command
  tree8 notify delete ?notifyName ...?
  tree8 notify info notifyName
  tree8 notify names ?pattern ...?}}

test tree.705 {tree8 notify create} {
    list [catch {tree8 notify create} msg] $msg
} {1 {wrong # args: should be "tree8 notify create ?switches ...? command"}}

test tree.706 {tree8 notify create -allevents} {
    list [catch {tree8 notify create -allevents Doit} msg] $msg
} {0 notify0}

test tree.707 {tree8 notify info notify0} {
    list [catch {tree8 notify info notify0} msg] $msg
} {0 {notify0 {-create -delete -move -sort -relabel} {Doit}}}

test tree.708 {tree8 notify info badId} {
    list [catch {tree8 notify info badId} msg] $msg
} {1 {unknown notify name "badId"}}

test tree.709 {tree8 notify info} {
    list [catch {tree8 notify info} msg] $msg
} {1 {wrong # args: should be "tree8 notify info notifyName"}}

test tree.710 {tree8 notify names} {
    list [catch {tree8 notify names} msg] $msg
} {0 notify0}


test tree.711 {test create notify} {
    list [catch {
	set mylist {}
	tree8 insert 1 -tags test
	set mylist
	} msg] $msg
} {0 {{-create 22}}}

test tree.712 {test move notify} {
    list [catch {
	set mylist {}
	tree8 move 8 test
	set mylist
	} msg] $msg
} {0 {{-move 8}}}

test tree.713 {test sort notify} {
    list [catch {
	set mylist {}
	tree8 sort 0 -reorder 
	set mylist
	} msg] $msg
} {0 {{-sort 0}}}

test tree.714 {test relabel notify} {
    list [catch {
	set mylist {}
	tree8 label test "newLabel"
	set mylist
	} msg] $msg
} {0 {{-relabel 22}}}

test tree.715 {test delete notify} {
    list [catch {
	set mylist {}
	tree8 delete test
	set mylist
	} msg] $msg
} {0 {{-delete 8} {-delete 22}}}


test tree.716 {tree8 notify delete badId} {
    list [catch {tree8 notify delete badId} msg] $msg
} {1 {unknown notify name "badId"}}


test tree.717 {test create notify} {
    list [catch {
	set mylist {}
	tree8 set all newKey 20
	set mylist
	} msg] $msg
} {0 {}}

test tree.718 {test delete notify} {
    list [catch {
	set mylist {}
	tree8 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}

test tree.719 {test delete notify} {
    list [catch {
	set mylist {}
	tree8 unset all newKey
	set mylist
	} msg] $msg
} {0 {}}

test tree.720 {tree8 copy} {
    list [catch {tree8 copy} msg] $msg
} {1 {wrong # args: should be "tree8 copy parentNode ?treeName? nodeName ?switches ...?"}}

test tree.721 {tree8 copy root} {
    list [catch {tree8 copy root} msg] $msg
} {1 {wrong # args: should be "tree8 copy parentNode ?treeName? nodeName ?switches ...?"}}

test tree.722 {tree8 copy root 14} {
    list [catch {tree8 copy root 14} msg] $msg
} {0 23}

test tree.723 {tree8 copy 14 root} {
    list [catch {tree8 copy 14 root} msg] $msg
} {0 24}

test tree.724 {tree8 copy 14 root -recurse} {
    list [catch {tree8 copy 14 root -recurse} msg] $msg
} {1 {can't make cyclic copy: source node is an ancestor of the destination}}

test tree.725 {tree8 copy 3 2 -recurse -tags} {
    list [catch {tree8 copy 3 2 -recurse -tags} msg] $msg
} {0 25}

test tree.726 {tree9 copy 3 2 -tree badTree} {
    list [catch {tree8 copy 3 2 -tree badTree} msg] $msg
} {1 {can't find a tree named "badTree"}}

test tree.727 {tree9 copy 3 2 -tree tree8} {
    list [catch {tree8 copy 3 2->badModifier -tree tree8} msg] $msg
} {1 {can't find tag or id "2->badModifier" in ::tree8}}

test tree.728 {copy tree to tree -recurse} {
    list [catch {
	blt::tree create treeA
	foreach node [tree8 children root] {
	    treeA copy root $node -recurse  -tree tree8
	}
	foreach node [tree8 children root] {
	    treeA copy root $node -recurse  -tree tree8
	}
	treeA dump root -version 2.0
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
0 1 {{} dir1} {perms 493 type directory} {}
1 2 {{} dir1 defs} {perms 420 type file} {}
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
0 1 {{} defs} {size 3353 perms 420 type file} {}
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
0 2 {{} mylink} {size 3353 perms 420 type file} {}
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





