
=========
blt::tree
=========

------------------------------------
Create and manage tree data objects.
------------------------------------

.. include:: man.rst
.. include:: toc.rst

SYNOPSIS
--------

**blt::tree create** ?\ *treeName*\ ?

**blt::tree destroy** ?\ *treeName* ... ?

**blt::tree load** *fmtName* *dir*

**blt::tree names** ?\ *pattern* ... ?

**blt::tree diff** *treeName1* *treeName2* ?\ *switches* ... ?

DESCRIPTION
-----------

The **blt::tree** command creates tree data objects.  A *tree object* is
general ordered tree of nodes.  Each node has both a label and optionally 
one or more variables.  Nodes can value different values; node data is
not homogenous. The tree is associated with a TCL command that you
can use to access and modify the its structure and data. Tree objects can
also be managed via a C API.

SYNTAX
------

**blt::tree create** ?\ *treeName*\ ?  
  Creates a new tree object.  The name of the new tree object is returned.
  If no *treeName* argument is present, then the name of the tree is
  automatically generated in the form "tree0", "tree1", etc.  If the
  substring "#auto" is found in *treeName*, it is automatically substituted
  by a generated name.  For example, the name ".foo.#auto.bar" will be
  translated to ".foo.tree0.bar".

  A new TCL command (by the same name as the tree) is created.  Another TCL
  command or tree object can not already exist as *treeName*.  If the TCL
  command is deleted, the tree will also be freed.  The new tree will
  contain just a root node.  Note that trees are by default, created in the
  current namespace, not the global namespace, unless *treeName* contains a
  namespace qualifier, such as "fred::myTree".

**blt::tree destroy** ?\ *treeName* ... ?
  Releases one of more trees.  The TCL command associated with *treeName* is
  also removed.  Trees are reference counted.  The internal tree data object
  isn't destroyed until no one else is using the tree.

**blt::tree load** *fmtName* *dir*
  Dynamically loads the named tree import/export module.  *FmtName* is the
  name of the format of the data imported or exported. *Dir* is the directory
  where the dynamically loadable module can be found. This command is used
  internally to load tree modules for importing and exporting data.

**blt::tree names** ?\ *pattern*\ ... ?
  Returns the names of all the BLT trees.  If one or more *pattern*
  arguments are provided, then the name of any tree matching *pattern* will
  be returned. *Pattern* is a **glob**\ -style pattern.

**blt::tree diff**  *treeName1* *treeName2* ?\ *switches* ... ?
  Compares the two given trees and returns the number of differences.
  See `COMPARING TREES`_ for more details.

REFERENCING TREE NODES
----------------------

A tree object is a hierarchy of nodes. The nodes may be referenced in two
ways: by node ID or by tag.

*nodeId*
  Each node has a unique serial number that is assigned to that node when
  it's created. The number identifies the node.  It never changes 
  and node IDs are not re-used.

*tag*
  A node may also have any number of tags associated with it.  A tag is
  just a string of characters, and it may take any form except that of
  an integer.  For example, "x123" is valid, but "123"
  isn't.  The same tag may be associated with many different nodes.
  This is commonly done to group nodes in various interesting ways.

  There are two built-in tags

  **all**
    Every node in the tree implicitly has this tag.  It may be used to
    invoke operations on all the nodes in the tree.

  **root**
    The tag "root" is managed automatically by the tree object. It applies
    to the node currently set as root.

When specifying nodes in tree object commands, if the specifier is an
integer, it is assumed to refer to a single node ID.  If the
specifier is not an integer, then it refers to any node with that tag.

The symbol *nodeName* is used below for arguments that specify a node
either by its ID or a tag that selects zero or more nodes.  Many
tree commands only operate on a single node at a time; if *nodeName* is
specified in a way that names multiple items, then an error "refers to more
than one node" is generated.

You can also add one or more modifiers the a node ID or tag.
Modifiers specify a relationship to the node.  For example,
"root->firstchild" selects the first subtree of the root node.  The node
modifiers are listed below.

**firstchild**
   Selects the first child of the node.  

**lastchild**
  Selects the last child of the node.  

**next**
  Selects the next node in preorder to the node.  

**nextsibling**
  Selects the next sibling of the node.  

**parent**
  Selects the parent of the node.  

**previous**
  Selects the previous node in preorder to the node.  

**prevsibling**
  Selects the previous sibling of the node.  

*nodeLabel*
 Selects the child node whose label is *nodeLabel*.  Enclosing *label* in
 quotes indicates to always search for a node by its label (for example,
 even if the node is labeled "parent").

Modifiers can can be chained. For example "10->parent->firstchild" looks
for the node with an ID of 10, then its parent, and then the
parent's first child node.  It's an error the node can't be found.  For
example, **lastchild** and **firstchild** will generate errors if the node
has no children.  The exception to this is the **index** operation.  You
can use **index** to test if a modifier is valid.

TREE OPERATIONS
---------------

After you create a tree object, you can use its TCL command to query or
modify it.  The general form is

  *treeName* *operation* ?\ *arg*\ ?...

Both *operation* and its arguments determine the exact behavior of the
command.  The operations available for trees are listed below.

*treeName* **ancestor** *nodeName1* *nodeName2*
  Returns the mutual ancestor of *nodeName1* and *nodeName2*. *NodeName1*
  and *nodeName2* can be a node ID or a tag (like "root") but may not
  reference multiple nodes. Note that the ancestor can be one of the two
  nodes.  For example, if *nodeName1* and *nodeName2* are the same nodes,
  their ancestor is *nodeName1*.

*treeName* **append** *nodeName* *varName* ?\ *string* ... ?
  Appends one or more strings to the variable *varName* in *nodeName*.
  *NodeName* is a node ID or tag and may refer to more than one node (like
  "all"). *String* is an arbitrary TCL string. This command provides an
  efficient way to build up long string values incrementally.  If *varName*
  doesn't already exist it is automatically created. The new value stored
  in *varName* is returned.

*treeName* **apply** *nodeName* ?\ *switches* ... ?
  Runs commands for all nodes matching the criteria given by *switches* for
  the subtree designated by *nodeName*. *NodeName* is a node ID or tag but
  may not reference multiple nodes.  By default all nodes match, but you
  can set switches to narrow the match.  This operation differs from
  **find** in two ways: 1) TCL commands can be invoked both pre- and
  post-traversal of a node and 2) the tree is always traversed in depth
  first order.

  The **-exact**, **-glob**, and **-regexp** switches indicate both what
  kind of pattern matching to perform and the pattern.  By default each
  pattern will be compared with the node label.  You can set more than one
  of these switches.  If any of the patterns match (logical or), the node
  matches.  If the **-key** switch is used, it designates the variable to
  be matched.  *Switches* may be any of the following.

  **-depth** *numLevels*
    Descend at most *numLevels* (a non-negative integer) levels. For
    example, if *numLevels* is "1", this means only test to the children of
    *nodeName*.

  **-exact** *labelString*
    Test each node label against the string *labelString*.  

  **-glob** *pattern*
    Test each node label against *pattern* using global pattern matching.
    *Pattern* is a **glob**\ -style pattern.
    Matching is done in a fashion similar to that used by the C-shell.

  **-invert**
    Select non-matching nodes.  Any node that *doesn't* match the given
    criteria will be selected.

  **-key** *varName*
    If pattern matching is selected (using the **-exact**, **-glob**, or
    **-regexp** switches), compare the variables keyed by
    *varName* instead of the node's label.  If no pattern matching
    switches are set, then any node with this data key will match.

  **-leafonly**
    Only test nodes with no children.

  **-nocase**
    Ignore case when matching patterns.

  **-path**
    Use the node's full path when comparing nodes.  The node's full path is
    a list of labels, starting from the root of each ancestor and the node
    itself.

  **-precommand** *cmdPrefix*
    Invoke *cmdPrefix* for each matching node.  Before *cmdPrefix* is invoked,
    the node ID node is appended.  You can control processing by the
    return value of *cmdPrefix*.  If *cmdPrefix* generates an error, processing
    stops and the **find** operation returns an error.  But if *cmdPrefix*
    returns **break**, then processing stops, no error is generated.  If
    *cmdPrefix* returns **continue**, then processing stops on that subtree
    and continues on the next.

  **-postcommand** *cmdPrefix*
    Invoke *cmdPrefix* for each matching node.  Before *cmdPrefix* is invoked,
    the node ID is appended.  You can control processing by the
    return value of *cmdPrefix*.  If *cmdPrefix* generates an error, processing
    stops and the **find** operation returns an error.  But if *cmdPrefix*
    returns **break**, then processing stops, no error is generated.  If
    *cmdPrefix* returns **continue**, then processing stops on that subtree
    and continues on the next.

  **-regexp** *patternString*
    Test each node using *patternString* as a regular expression pattern.

  **-tag** *tagName*
    Only test nodes that have the tag *tagName*.

*treeName* **attach** *treeObject* ?\ *switches* ... ?
  Attaches to an existing tree object *treeObject*.  The current tree
  associated with *treeName* is discarded.  In addition, the current set of
  tags, notifier events, and traces are removed. *Switches* may be any of
  the following.

  **-newtags** 
    By default, the tree will share the tags of the attached tree. If this
    flag is present, the tree will start with an empty tag table.

*treeName* **children** *parentName* ?\ *switches* ... ?
  Returns a list of the node IDs of the children of the node
  *parentName*.  *ParentName* is a node ID or a tag (like "root")
  but may not reference multiple nodes.  *Switches* may be any of the
  following.
  
  **-from** *nodeName* 
    Sets the starting range of children.  *NodeName* is a node ID or a tag
    (like "root") but may not reference multiple nodes. It must be a child
    node of *parentName*. By default, the starting point is the first
    child.

  **-nocomplain**
    If *parentName* is not a valid node ID or tag, return an empty
    list instead of generating an error.

  **-to** *nodeName*
    Sets the end of the range of children.  *NodeName* is a node
    ID or a tag (like "root") but may not reference multiple
    nodes. It must be a child node of *parentName*. By default, the
    end point is the last child.

*treeName* **copy** *destParentNode* ? *srcNode* ?\ *switches*  ... ?
  Makes a copy of *srcNode* in *destParentNode*. Both nodes *srcNode* and
  *destParentNode* must already exist. *DestParentNode* is a node in
  *treeName*. The ID of the new node in *treeName* is returned.  By
  default *srcNode* is a node in *treeName*.  *Switches* may be any of the
  following.

  **-label** *nodeLabel*
    Label the new node as *nodeLabel*.  By default, the new node will
    have the same label as *srcNode*.

  **-overwrite**
    Overwrite nodes that already exist.  Normally new nodes are always created,
    even if there already exists a node by the same label in *destParentNode*.

  **-recurse**
    Recursively copy all the nodes under *srcNode* as well.  In this case,
    *srcNode* can't be an ancestor of *destParentNode* as it would result in a
    cycle.

  **-tags**
    Copy tags from *srcNode* to the new node.  The default is to not
    copy tags.

  **-tree** *srcTreeName*
    Specifies that *srcNode* belongs to tree *srcTreeName* instead of
    *treeName*. *SrcTreeName* is the name of another tree object.

*treeName* **degree** *nodeName* 
  Returns the number of children of *nodeName*.

*treeName* **delete** ?\ *nodeName* ... ?
  Recursively deletes zero or more nodes from the tree.  The node and all
  its descendants are removed.  The one exception is the root node.  In
  this case, only its descendants are removed.  The root node will remain.
  Any tags or traces on the nodes are released.   *NodeName* is a node 
  ID or a tag and may refer to multiple nodes.

*treeName* **depth** *nodeName* 
  Returns the depth of the node. *NodeName* is a node ID or a tag but
  may not reference multiple nodes. The depth is the number of levels from
  the node to the root of the tree.  The depth of the root node is 0.

*treeName* **dir** *nodeName* *path* ?\ *switches* ... ?
  Loads the directory listing of *path* into the tree at node *nodeName*.
  *NodeName* is a node ID or a tag (like "root") but may not reference
  multiple nodes.
  
  The following switches are available:

  **-fields** *list* 
    Specifies what directory information is to be added as fields in the
    tree.  *FieldsList* is a TCL list of field names.  The valid field
    names is listed below.

    **size**
      Adds a decimal string giving the size of file name in bytes.
    **mode**
      Adds a decimal string giving the mode of the file or directory.
      The 12 bits corresponding to the mask 07777 are the file mode bits
      and the least significant 9 bits (0777) are the file permission bits.
    **type**
      Adds the type of the file or directory. The type of file name
      will be one of "file", "directory", "characterSpecial",
      "blockSpecial", "fifo", "link", or "socket".
    **uid**
      Adds the user ID of the owner of the file or directory.
    **gid**
      Adds the group ID of the owner of the file or directory.
    **atime**
      Adds a decimal string giving the time at which file name was
      last accessed.   
    **ctime**
      Adds a decimal string giving the time at which the status of file
      name was changed. Status may be changed by writing or by setting
      inode information (i.e., owner, group, link count, mode, etc.).
    **mtime**
      Adds a decimal string giving the time at which file name was
      last modified.   
    **all**
      Adds all the above fields.

  **-hidden**   
    Include hidden files (files that start with a ".")

  **-ignorehiddendirs**   
    Don't include directories that start with a "." when recursively
    tranversing directories (see the **-recurse** switch).

  **-nocase**   
    Ignore case when matching patterns using the **-patterns** switch.
    
  **-patterns** *patternList*
    Only include files and directories that match one or more patterns.
    *PatternList* is a TCL list of **glob**\-style patterns.  (such as
    "*.jpg").  Matching is done in a fashion similar to that used by the
    TCL **glob** command.

  **-permissions** *permString*
    Only include files or directories that have the one or more of the
    permssions in *permString*.  *PermString* is a string that may contain
    any of the following letters.

    **r**
      Entry is readable by the user.
    **w**
      Entry is writable by the user.
    **x**
      Entry is executable by the user.

  **-readonly**
    Only include files and directories that are readable but not writable
    by the user.

  **-recurse** 
    If *path* is a directory, recusively adds entries for files and
    subdirectories into the tree.

  **-type** *typeList*
    Only include files or directories that have the match types in
    *typeList*.  *TypeList* may contain any of the following.

    **block**
      Type of entry is block (buffered) special.
    **character**
      Type of entry is character (unbuffered) special.
    **directory**
      Type of entry is a directory.
    **file**
      Type of entry is a regular file.
    **link**
      Type of entry is a link.  
    **pipe**
      Type of entry is a pipe (fifo).  
    **socket**
      Type of entry is a socket.

*treeName* **dump** *nodeName* ?\ *switches* ... ?
  Returns a list of the paths and respective data for *nodeName* and its
  descendants. *NodeName* is a node ID or a tag (like "root") but may
  not reference multiple nodes.  The subtree designated by *nodeName* is
  traversed returning the following information for each node: 1) the
  node's path relative to *nodeName*, 2) a sublist key value pairs
  representing the node's variables, and 3) a sublist of tags.  This list
  returned can be used later to copy or restore the tree with the
  **restore** operation.  The following switches are available:

  **-file** *fileName*
    Write the dump information to the file *fileName*.

  **-data** *varName*
    Saves the dump information in the TCL variable *varName*.

*treeName* **dup** *nodeName* 
  FIXME:
  
*treeName* **exists** *nodeName* ?\ *varName*\ ?
  Indicates if *nodeName* exists in the tree. *NodeName* is a node ID or a
  tag (like "root") but may not reference multiple nodes.  If a *varName*
  argument is present then this command indicates if a variable *varName*
  exists in *nodeName*.

*treeName* **export** *dataFormat* ?\ *switches*  ... ?
  Exports the tree contents into *dataFormat*. *DataFormat* is the format
  of the exported data.  See `TREE FORMATS`_ for what file formats are
  available.

*treeName* **find** *nodeName* ?\ *switches* ... ? 
  Finds for all nodes matching the criteria given by *switches* for the
  subtree designated by *nodeName*.  A list of the selected nodes is
  returned.  By default all nodes match, but you can set switches to narrow
  the match.

  The **-exact**, **-glob**, and **-regexp** switches indicate both what
  kind of pattern matching to perform and the pattern.  By default each
  pattern will be compared with the node label.  You can set more than one
  of these switches.  If any of the patterns match (logical or), the node
  matches.  If the **-key** switch is used, it designates the variable to
  be matched.

  The order in which the nodes are traversed is controlled by the
  **-order** switch.  The possible orderings are **preorder**,
  **postorder**, **inorder**, and **breadthfirst**.  The default is
  **postorder**.

  *Switches* may be any of the following.

  **-addtag** *tagName* 
    Add the tag *tagName* to each selected node.  

  **-count** *number*
    Stop processing after *number* (a positive integer) matches. 

  **-depth** *numLevels*
    Descend at most *numLevels* (a non-negative integer) levels For
    example, if *numLevels* is "1" this means only apply the tests to the
    children of *nodeName*.

  **-exact** *labelString*
    Matches each node with the label *labelString*.  

  **-exclude** *pattern*
    Exclude matching nodes that are in *nodeList*.  *NodeList* is a list of
    node IDs.  Only the node itself is excluded.  The descendants of
    excluded nodes are not automatically excluded.

  **-exec** *cmdPrefix*
    Invokes a TCL command *cmdPrefix* for each matching node.  Before
    *cmdPrefix* is invoked, the node ID is appended.  The return code of
    *cmdPrefix* controls how processing continues.

    **ok**
      Processing continues normally.
    
    **error**
      If  *cmdPrefix* generates an error, processing stops and the
      **find** operation returns with an error.

    **break**
      Processing stops, but no error is generated.

    **continue**
      Processing stops on that subtree and continues on the next.

  **-glob** *patternString*
    Test each node to *patternString*. *PatternString* is a **glob**\-style
    pattern matching string.  Matching is done in a fashion similar to that
    used by the C-shell.

  **-invert**
    Select non-matching nodes.  Any node that *doesn't* match the given
    criteria will be selected.

  **-key** *varName*
    Compare the names of the variable keyed by *varName* instead of
    the node's label. If no pattern is given (**-exact**, **-glob**, or
    **-regexp** switches), then any node with this data key will match.

  **-leafonly**
    Only test nodes with no children.

  **-nocase**
    Ignore case when matching patterns.

  **-nodes** *nodeList*
    Specifies nodes to be examined when searching. This overrides the
    *nodeName*, **-include**, and **-exclude** options. *NodeList* is a
    list of node IDs. Only the nodes in the list are examined. The
    descendants of an included node are not automatically included.

  **-order** *traversalOrder* 
    Traverse the tree and process nodes according to
    *traversalOrder*. *TraversalOrder* can be one of the following.

    **breadthfirst**
      Process the node and the subtrees at each sucessive level. Each node
      on a level is processed before going to the next level.

    **inorder**
      Recursively process the nodes of the first subtree, the node itself,
      and any the remaining subtrees.

    **postorder**
     Recursively process all subtrees before the node.

    **preorder**
      Recursively process the node first, then any subtrees.

  **-path**
    Use the node's full path when comparing nodes.

  **-regexp** *patternString*
    Test each node using *patternString* as a regular expression pattern.

  **-tag** *tagName*
    Only test nodes that have the tag *tagName*.

*treeName* **findchild** *nodeName* *label*
  Searches for a child node with the label *label* in the parent
  *nodeName*. *NodeName* is a node ID or a tag (like "root") but may
  not reference multiple nodes. The ID of the child node is returned if
  found.  Otherwise "-1" is returned.

*treeName* **firstchild** *nodeName* 
  Returns the node ID of the first child in the *nodeName*'s list of subtrees.
  *NodeName* is a node ID or a tag (like "root") but may not reference
  multiple nodes.  If *nodeName* is a leaf (has no children), then "-1" is
  returned.

*treeName* **get** *nodeName* ?\ *varName*\ ? ?\ *defaultValue*\ ?
  Returns a list the variables and their values in *nodeName*.  *NodeName*
  is a node ID or a tag (like "root") but may not reference multiple
  nodes.  If *varName* is present, then only the value for that particular
  variable is returned.  It's normally an error if *nodeName* does not
  contain the variable *varName*.  But if you provide a *defaultValue*
  argument, this value is returned instead (*nodeName* will still not
  contain *varName*).  This feature can be used to access a variable of
  *nodeName* without first testing if it exists.  This operation may
  trigger **read** data traces.

*treeName* **import** *format* ?\ *switches* ... ?
  Imports the tree contents into *format*. *Format* is the format of the
  imported data.  See `TREE FORMATS`_ for what file formats are available.

*treeName* **index** *nodeName*
  Returns the node ID of *nodeName*.  *NodeName* is a node id or a tag
  (like "root") but may not reference multiple nodes.  If *nodeName* does
  not represent a valid node ID or tag, or has modifiers that are invalid,
  then "-1" is returned.

*treeName* **insert** *parentNode* ?\ *switches* ... ? 
  Inserts a new node into parent node *parentNode*.  The ID of the new node
  is returned. *Switches* may be any of the following.

  **-after** *childNode* 
    Insert the new node after the node *childNode*.  *ChildNode* must be a
    child of *parentNode*.

  **-before** *childNode* 
    Insert the new node before the node *childNode*.  *ChildNode* must be a
    child of *parentNode*.

  **-data** *dataList*
    Sets the value for each variable in *dataList* for the new
    node. *DataList* is a list of key-value pairs.

  **-label** *nodeLabel* 
    Designates the label of the node as *nodeLabel*.  By default, nodes
    are labeled as "node0", "node1", etc.

  **-node** *id* 
    Designates the node ID for the node.  Normally new IDs are automatically
    generated.  This allows you to create a node with a specific ID.  It is
    an error if the ID is already used by another node in the tree.

  **-tags** *tagList*
    Adds each tag in *tagList* to the new node. *TagList* is a list of
    tags, so be careful if a tag has embedded spaces.

*treeName* **isancestor** *node1* *node2*
  Indicates if *node1* is an ancestor of *node2*.  Returns "1" if true and
  "0" otherwise.

*treeName* **isbefore**  *node1* *node2*
  Indicates if *node1* is before *node2* in depth first traversal.  Returns
  "1" if true and "0" otherwise.

*treeName* **isleaf** *nodeName*
  Indicates if *nodeName* is a leaf (it has no subtrees).  Returns "1" if
  true and "0" otherwise.

*treeName* **isroot** *nodeName*
  Indicates if *nodeName* is the designated root.  This can be changed by
  the **chroot** operation.  Returns "1" if true and "0" otherwise.

*treeName* **keys** ?\ *nodeName*...\ ?
  FIXME:  
  Returns the variable names for one or more nodes.

*treeName* **label** *nodeName* ?\ *newLabel*\ ?
  Returns the label of the node designated by *nodeName*.  If *newLabel* is
  present, the node is relabeled using it as the new label.

*treeName* **lappend** *nodeName* *varName* ?\ *value* ... ?
  Appends one or more values to the current value for *varName* in
  *nodeName*.  *VarName is the name of a variable in *nodeName*.
  
*treeName* **lastchild** *nodeName*
  Returns the node ID of the last child in the *nodeName*'s list of subtrees.
  If *nodeName* is a leaf (has no children), then "-1" is returned.

*treeName* **move** *nodeName* *parentNode* ?\ *switches* ... ?
  Moves *nodeName* into *parentNode*. *NodeName* is appended to the list
  children of *parentNode*.  *NodeName* can not be an ancestor of
  *parentNode*.  *Switches* may be any of the following.

  **-after** *childNode* 
    Position *nodeName* after *childNode*.  The node *childNode* must be a
    child of *newParent*.

  **-at** *number* 
    Inserts *nodeName* into *parentNode*\'s list of children at 
    position *number*. 

  **-before** *childNode* 
    Position *nodeName* before *childNode*.  The node *childNode* must be a
    child of *parentNode*.

*treeName* **names** *nodeName* ?\ *varName*\ ?
  Returns the names of the variables present for node *nodeName*.  If
  *varName* is given, then *varName* is an array variable and the names of
  the array elements are returned.

*treeName* **next** *nodeName*
  Returns the next node from *nodeName* in a preorder traversal.  If
  *nodeName* is the last node in the tree, then "-1" is returned.

*treeName* **nextsibling** *nodeName*
  Returns the node representing the next subtree from *nodeName* in its
  parent's list of children.  If *nodeName* is the last child, then "-1" is
  returned.

*treeName* **notify create** ?\ *switches* ... ? *cmdPrefix* ?\ *args* ... ?
  Creates a notifier for the tree.  A notify token in the form
  "notify0", "notify1", etc.  is returned.

  *CmdPrefix* and *args* are saved and invoked whenever the tree structure
  is changed (according to *switches*). Two arguments are appended to
  *cmdPrefix* and *args* before it's invoked: the ID of the node and a
  string representing the type of event that occured.  One of more switches
  can be set to indicate the events that are of interest.  *Switches* may
  be any of the following.

  **-create** 
    Invoke *cmdPrefix* whenever a new node has been added.

  **-delete**
    Invoke *cmdPrefix* whenever a node has been deleted.

  **-move**
    Invoke *cmdPrefix* whenever a node has been moved.

  **-node** *nodeName*
    Only watch *nodeName**.

  **-sort**
    Invoke *cmdPrefix* whenever the tree has been sorted and reordered.

  **-tag** *tagName*
    Watch nodes that has the tag *tagName*.
    
  **-relabel**
    Invoke *cmdPrefix* whenever a node has been relabeled.

  **-allevents**
    Invoke *cmdPrefix* whenever any of the above events occur.

  **-whenidle**
    When an event occurs don't invoke *cmdPrefix* immediately, but queue it to
    be run the next time the event loop is entered and there are no events to
    process.  If subsequent events occur before the event loop is entered,
    *cmdPrefix* will still be invoked only once.

*treeName* **notify delete** *notifyName* 
  Deletes one or more notifiers from the tree.  *NotifyName* is a name
  returned by the **notify create** operation.

*treeName* **notify info** *notifyName*
  Returns information about the notify event *notifyName*.  *NotifyName* is
  a name returned by the **notify create** operation.  The information is
  the same as what was specified for the **notify create** operation.  It
  consists of the notify name, a sublist of event flags (it's in the same
  form as *flags*) and, the command prefix.

*treeName* **notify names**
  Returns a list of names for all the current notifiers.

*treeName* **parent** *nodeName*
  Returns the parent node of *nodeName*.  If *nodeName* is the root of the
  tree, then "-1" is returned.

*treeName* **path cget** *option*
  Returns the current value of the path configuration option given by *option*.
  *Option* may be any option described below for the **path configure**
  operation.
  
*treeName* **path configure** ?\ *option* *value* ... ?
  Queries or modifies the path configuration options.  If *option*
  isn't specified, a list describing the current options is
  returned.  If *option* is specified, but not *value*, then a list
  describing *option* is returned.  If one or more *option* and *value*
  pairs are specified, then for each pair, the option *option* is set to
  *value*.  The following options are valid.

  **-includeroot** 
    Indicates to include the root node (see **-root** option) name when
    printing the path to a node.  By default, the root node is not
    included.
  
  **-nocomplain** 
    When creating a path, indicates to return "-1" instead of generating
    an error if any of ancestors of the path can not be found.
  
  **-parents** 
    When creating a path, indicates to create ancestor nodes if they don't
    exist.  By default, it's an error if any parent of *path* can't be found.
  
  **-root** *rootNode*
    Specifies the root node for the path. *RootNode* is a node ID or a tag
    (like "root") but may not reference multiple nodes.  The default is "root".
    
  **-separator**  *sepString*
    Specifies the separator for path components.  If *sepString* is "",
    this means the path is a TCL list. The default is "".
  
*treeName* **path create** *path* 
  Creates a new node described by *path*. By default, *path* is a list of
  node labels.  But if the path option **-separator** has been defined to be
  a non-empty separator, *path* is string of node labels separated by the
  separator. 

*treeName* **path parse** *path* 
  Returns the node ID of the node described by *path*.  By default, *path* is a
  list of node labels.  But if the path option **-separator** has been
  defined to be a non-empty separator, *path* is string of
  node labels separated by the separator.

*treeName* **path print** *nodeName* 
  Returns the path to *nodeName* from the root.  The root is by default of
  the tree. 

*treeName* **position** *nodeName*
  Returns the position of the node in its parent's list of children.
  Positions are numbered from 0.  The position of the root node is
  always 0.

*treeName* **previous** *nodeName*
  Returns the previous node from *nodeName* in a preorder traversal.  If
  *nodeName* is the root of the tree, then "-1" is returned.

*treeName* **prevsibling** *nodeName*
  Returns the node representing the previous subtree from *nodeName* in its
  parent's list of children.  If *nodeName* is the first child, then "-1"
  is returned.

*treeName* **restore** *nodeName* ?\ *switches* ... ?
  Performs the inverse function of the **dump** operation, restoring nodes
  to the tree. The format of *dataString* is exactly what is returned by
  the **dump** operation.  It's a list containing information for each node
  to be restored.  The information consists of 1) the relative path of the
  node, 2) a sublist of key value pairs representing the node's data,
  and 3) a list of tags for the node.  Nodes are created starting from
  *nodeName*. Nodes can be listed in any order.  If a node's path describes
  ancestor nodes that do not already exist, they are automatically created.
  *Switches* may be any of the following.

  **-overwrite**
    Overwrite nodes that already exist.  Normally nodes are always created,
    even if there already exists a node by the same name.  This switch
    indicates to add or overwrite the node's variables.

  **-file** *fileName*
    Read the dump information from the file *fileName*.

  **-data** *dataString*
    Reads the dump information from *dataString*.

*treeName* **root** ?\ *rootNode*\ ?
  Sets or gets the root node of the tree.  If no *rootNode* argument is
  present, this command returns the node ID of the root node.  Normally
  this is "0".  If a *rootNode* argument is provided, it will become the
  new root of the tree. This lets you temporarily work within a subset of
  the tree. Changing the root affects operations such as **next**,
  **path**, **previous**, etc.

*treeName* **search** ?\ *switches* ... ? 
  Searches for all nodes matching the criteria given by *switches* for the
  subtree designated by *nodeName*.  A list of the selected nodes is
  returned.  By default all nodes match, but you can set switches to narrow
  the match.

  The **-label**, **-path**, **-tag**, **value**, and **-variable**
  switches indicate both what item to match and the kind of pattern
  matching to perform.

  **-label** *pattern*
    Compares node labels with *pattern*. 

  **-path** *pattern*
    Compares node paths with *pattern*.   

  **-tag** *pattern*
    Compares tag names with *pattern*.   

  **-variable** *pattern*
    Compares variable names with *pattern*. 

  **-value** *pattern*
    Compares variable values with *pattern*.  

  *Pattern* can be a single string (such as "fred") or a TCL list in the
  form "*string* *patternKeyWords*\ ...". *String* is a string to be
  matched.  *PatternKeyWords* are flags that specify how the pattern is to
  matched.  They are:

  **exact**
    Match the pattern exactly. This is the default.

  **glob** 
    Treat the pattern as a **glob**\-style pattern string.  Matching is
    done in a fashion similar to that used by the C-shell.

  **nocase**
    Ignore case when matching patterns.

  **regexp**
    Treat the pattern as a regular expression pattern.

  By default case-sensitive, exact pattern comparisons are made.  

  More than one switch or any kind may be used to define multiple tests
  (such as "-label a -label b -label c").  Multiple tests of the the same
  type are consider *or*-ed, while tests of different types are *and*-ed.
  For example "-label a -label b -label c" would match any the node whose
  label is "a", "b", or "c".  But "-label a -variable b -value c" would only
  match a node with a label "a" that also has a variable "b" whose value is
  "c".

  The order in which the nodes are traversed is controlled by the
  **-order** switch.  The possible orderings are **preorder**,
  **postorder**, **inorder**, and **breadthfirst**.  The default is
  **postorder**.

    **breadthfirst**
      Process the node and the subtrees at each sucessive level. Each node
      on a level is processed before going to the next level.

    **inorder**
      Recursively process the nodes of the first subtree, the node itself,
      and any the remaining subtrees.

    **postorder**
     Recursively process all subtrees before the node.

    **preorder**
      Recursively process the node first, then any subtrees.

  Other *switches* may be any of the following.

  **-addtag** *tagName* 
    Add the tag *tagName* to each selected node.  

  **-count** *number*
    Stop processing after *number* (a positive integer) matches. 

  **-depth** *numLevels*
    Descend at most *numLevels* (a non-negative integer) levels For
    example, if *numLevels* is "1" this means only apply the tests to the
    children of *nodeName*.

  **-command** *cmdPrefix*
    Invokes a TCL command *cmdPrefix* for each matching node.  Before
    *cmdPrefix* is invoked, the node ID is appended.  The return code of
    *cmdPrefix* controls how processing continues.

    **ok**
      Processing continues normally.
    
    **error**
      If  *cmdPrefix* generates an error, processing stops and the
      **search** operation returns an error.

    **break**
      Processing stops, but no error is generated.

    **continue**
      Processing stops on that subtree and continues on the next.

  **-invert**
    Select non-matching nodes.  Any node that *doesn't* match the given
    criteria will be selected.

  **-leafonly**
    Only test nodes with no children.

  **-nodes** *nodeList*
    Specifies nodes to be examined when searching. *NodeList* is a
    list of node IDs. Only the nodes in the list are examined. The
    descendants of an included node are not automatically examined.

  **-order** *traversalOrder* 
    Traverse the tree and process nodes according to *traversalOrder*.

  **-root** *nodeName*
     Specifies the root node of search.  The default is the root of the
     tree.

*treeName* **set** *nodeName* ?\ *varName* *value* ... ?
  Sets zero or more variables in *nodeName*.  *NodeName* is a node ID or tag
  and may refer to more than one node (like "all").  *VarName* is the name
  of a variable and *value* is its respective value.  This operation may
  trigger **write** and **create** data traces.

*treeName* **size** *nodeName*
  Returns the number of nodes in the subtree whose root is *nodeName*. This
  includes *nodeName* and all its descendants. For example, the size of a
  leaf node is 1. *NodeName* is a node ID or tag (like "root") but may not
  reference multriple nodes.

*treeName* **sort** *nodeName* ?\ *switches* ... ? 
  Sorts the subtree starting at *nodeName*.  The following switches are
  available:

  **-ascii** 
    Compare strings using ASCII collation order.

  **-command** *cmdPrefix*
    Specifies a TCL command to be used to comparison nodes.  *CmdPrefix* is
    a TCL command that when executed will have two node IDs appended to it
    as additional arguments.  The command should compare the nodes,
    returning 1 if the first node is greater than the second, -1 is the
    second is greater than the first, and 0 is both nodes are equal.

  **-decreasing**
    Sort in decreasing order (largest items come first).

  **-dictionary**
    Compare strings using a dictionary-style comparison.  This is the same
    as **-ascii** except (a) case is ignored except as a tie-breaker
    and (b) if two strings contain embedded numbers, the numbers compare as
    integers, not characters.  For example, if **-dictionary** is set,
    bigBoy sorts between bigbang and bigboy, and x10y sorts between x9y and
    x11y.

  **-integer**
    Compare the nodes as integers.  This is useful for comparing variables
    that are integers (see the **-key** switch).

  **-key** *varName*
    Sort based upon the node's variable keyed by *varName*. Normally
    nodes are sorted according to their label.

  **-path**
    Compare the full path of each node.  The default is to compare only its
    label.

  **-real**
    Compare the nodes as real numbers. This is useful for comparing variables
    that are real numbers (see the **-key** switch).

  **-recurse**
    Recursively sort the entire subtree rooted at *nodeName*.

  **-reorder** 
    Recursively sort subtrees for each node.

    Warning: This changes the order of the nodes in the tree.  Without this
    switch a list of sorted nodes is returned but the actual of the
    nodes is unchanged.

*treeName* **tag add** *tagName* ?\ *nodeName* ... ?
  Adds the tag to one of more nodes. *TagName* is an arbitrary string that
  can not start with a number.

*treeName* **tag delete** *tagName* ?\ *nodeName* ... ?
  Deletes the tag from one or more nodes.  

*treeName* **tag exists** *tagName* ?\ *nodeName*\ ?
  Returns whether a tags exists in the tree.  If a *nodeName* argument is
  present, only if *tagName* is used by *nodeName* is "1" returned, "0"
  otherwise.

*treeName* **tag forget** *tagName*
  Removes the tag *tagName* from all nodes.  It's not an error if no nodes
  are tagged as *tagName*.

*treeName* **tag get** *nodeName* ?\ *pattern* ... ?
  Returns the tag names for a given node.  If one of more pattern
  arguments are provided, then only those matching tags are returned.

*treeName* **tag names** ?\ *nodeName*\ ?
  Returns a list of tags used by the tree.  If a *nodeName* argument is
  present, only those tags used by *nodeName* are returned.

*treeName* **tag nodes** *tagName*
  Returns a list of nodes that have the tag.  If no node is tagged as
  *tagName*, then an empty string is returned.

*treeName* **tag set** *nodeName* ?\ *tagName* ... ?
  Sets one or more tags for a given node.  Tag names can't start with a
  digit (to distinquish them from node IDs) and can't be a reserved
  tag ("root" or "all").

*treeName* **tag unset** *nodeName* ?\ *tagName* ... ?
  Removes one or more tags from a given node. Tag names that don't exist or
  are reserved ("root" or "all") are silently ignored.

*treeName* **trace create** *nodeName* *varName* *ops* *cmdPrefix*
  Creates a trace for variable *varName* in *nodeName*.  *NodeName* can
  refer to more than one node (like "all"). If *nodeName*
  is a tag, any node with that tag can possibly trigger a trace, invoking
  *cmdPrefix*.  *CmdPrefix* is a TCL command prefix, typically a procedure
  name.  Whenever a trace is triggered, four arguments are appended to
  *cmdPrefix* before it is invoked: *treeName*, node ID, *varName* and,
  *ops*.  Note that no nodes need have the variable *varName*.  A trace
  name in the form "trace0", "trace1", etc.  is returned.

  *Ops* indicates which operations are of interest, and consists of one or
  more of the following letters:

  **r**
    Invoke *cmdPrefix* whenever *varName* is read. Both read and write
    traces are temporarily disabled when *cmdPrefix* is executed.

  **w**
    Invoke *cmdPrefix* whenever *varName* is written.  Both read and
    write traces are temporarily disabled when *cmdPrefix* is executed.

  **c**
    Invoke *cmdPrefix* whenever *varName* is created.

  **u** 
    Invoke *cmdPrefix* whenever *varName* is unset.  Variables are
    typically unset with the **unset** operation.  Variables are also
    unset when the tree is released, but all traces are disabled prior to
    that.

*treeName* **trace delete** ?\ *traceName* ... ?
  Deletes one of more traces.  *TraceName* is the name of trace created by
  the **trace create** operation.

*treeName* **trace info** *traceName* 
  Returns information about the trace *traceName*.  *TraceName* is the name
  of trace previously created by the **trace create** operation.  The
  information is the same as what was specified for the **trace create**
  operation.  It consists of the node ID or tag, variable name, a string of
  letters indicating the operations that are traced (it's in the same form
  as *ops*) and, the command prefix.

*treeName* **trace names**
  Returns a list of names for all the current traces.

*treeName* **type** *nodeName* *varName*
  Returns the type of the variable *varName* in the node *nodeName*.

*treeName* **unset** *nodeName* ?\ *varName* ... ?
  Removes zero or more variables from *nodeName*. *NodeName* may be a tag
  that represents several nodes.  *VarName* is the name of the variable
  to be removed.  It's not an error if *nodeName* does not contain
  a variable *varName*.  This operation may trigger **unset** data traces.

COMPARING TREES
---------------

**blt::tree diff** *treeName1* *treeName2* ?\ *switches* ... ?
  Compares the two given trees and returns the number of differences.
  *TreeName1* and *treeName2* are names of BLT trees.  *TreeName1* and
  *treeName2* may be the same tree.

  *Switches* may be any of the following.

  **-command** *cmdString*
     Specifies a TCL command *cmdString* to be invoked when comparing nodes.

  **-root1** *nodeName*
    Specifies the node from where to start the comparison in *treeName1*.
    The default is the root of *treeName1*.

  **-root2** *nodeName*
    Specifies the node from where to start the comparison in *treeName2*.
    The default is the root of *treeName1*.

  **-variable** *varName*
     Specifies a TCL variable *varName* to be filled with the detailed
     differences between the two trees.  

TREE FORMATS
------------

Handlers for various tree formats can be loaded using the TCL **package**
mechanism.  There are two formats supported: "xml" and "json".

**json**
~~~~~~~~

To use the JSON handler you must first require the package.

  **package require blt_tree_json**

Then the following **import** and **export** commands become available.

*treeName* **import json** ?\ *switches* ... ?
  Imports the JSON data into the tree.  Either the **-file** or **-data**
  switch must be specified, but not both.  *Switches* can be any of the
  following.

  **-file** *fileName*
    Read the JSON file *fileName* to load the tree.

  **-data** *dataString*
    Read the JSON information from *dataString*.

  **-root** *nodeName*
    Load the JSON information into the tree starting at *nodeName*.  The
    default is the root node of the tree.

  JSON objects are converted into tree nodes tagged as "json_object".  Its
  members are subnodes.  JSON arrays are also converted into tree nodes
  tagged as "json_array".  Its members are subnodes whose labels are in the
  form "_indexN".  Simple JSON values (strings, numbers, booleans) are
  converted into tree nodes.  The key is the both the node label and a
  variable name.  The value is stored in the variable.
  
*treeName* **export json** ?\ *switches* ... ?
  Exports the tree as JSON data. If no **-file** or **-data** switch
  is provided, the XML output is returned as the result of this command.
  The following export switches are supported.

  **-file** *fileName*
    Write the tree to the JSON file *fileName*.

  **-data** *varName*
    Write the tree in JSON format to the TCL variable *varName*.

  **-jsontree** 
    Indicates that the tree should be treated like a tree that was imported
    from a JSON file.  That is the simple values are contained in their own
    nodes and arrays are tagged by "json_array".  Normally, simple values are
    tree node variables node and arrays are produced only for TCL list
    objects.  This can be used to produce and compare a JSON output file
    versus its imported file.

  **-root** *nodeName*
    Write the tree starting from *nodeName*.  The default is the root 
    node of the tree.

  This command does not know if a node and its children represent and array
  or object.  Arrays are used when you don't care about the member name
  (index).  You can indicate if a node is an array by 1) tagging the node
  with the tag "json_array" and 2) using the **-jsontree** switch described
  above.  If a node is tagged as an array, both its variables and the
  labels of its children are ignored.
  
**xml**
~~~~~~~

To use the XML handler you must first require the package.

  **package require blt_tree_xml**

Then the following **import** and **export** commands become available.

*treeName* **import xml** ?\ *switches* ... ?
  Imports the XML data into the tree. Either the **-file** or **-data**
  switch must be specified, but not both.  *Switches* can be any of the
  following.

  **-all** 
    Import all XML features.

  **-attributes** *boolean*
    If true, import XML attributes.  The default is "1".

  **-cdata** *boolean*
    If true, import CDATA.  The default is "1".

  **-comments** *boolean*
    If true, import XML comments.  The default is "0".

  **-convertcdata** *boolean*
    If true, convert CDATA into a single value.  The default is "0".

  **-data** *dataString*
    Read the JSON information from *dataString*. It is an error
    to set both the **-file** and **-data** switches.

  **-declaration**  *boolean*
    If true, import XML declarations.  The default is "0".

  **-extref**  *boolean*
    If true, import XML external references.  The default is "0".

  **-file** *fileName*
    Read the JSON file *fileName* to load the tree. It is an error
    to set both the **-file** and **-data** switches.

  **-locations**  *boolean*
    If true, import XML locations.  The default is "0".

  **-namespace**  *boolean*
    If true, import XML namespaces.  The default is "0".

  **-overwrite**  *boolean*
    If true, overwrite tree nodes is they already exist.  
    The default is "0".

  **-processinginstructions**  *boolean*
    If true, import XML processing instructions.  The default is "0".

  **-root** *nodeName*
    Load the XML information into the tree starting at *nodeName*.  The
    default is the root node of the tree.

  **-trimwhitespace**  *boolean*
    If true, trim white space from XML character data.  The default is "0".

*treeName* **export xml** ?\ *switches* ... ?
  Exports the tree as XML data. If no **-file** or **-data** switch is
  provided, the XML output is returned as the result of this command.
  *Switches* can be any of the following.

  **-data** *varName*
    Writes XML to the TCL variable *varName*.

  **-declaration** 
    Adds an XML version and encoding declaration at the top of the XML data.

  **-file** *fileName*
    Writes XML to the file *fileName*.

  **-hideroot** 
    Indicates to not output an XML tag for the root node. 

  **-indent** *numChars*
    Specifies the number of characters to indent for each level of XML tag.
    The default is "1".
    
  **-root** *nodeName*
    Specifies the topmost node.  By default it is the root of *treeName*.

EXAMPLE
-------

FIXME

C API
-----

#include <bltTree.h>

int **Blt_Tree_Create**\ (Tcl_Interp *\ *interp*, const char *\ *treeName*, Blt_Tree \* *treePtr*)
  Creates a new tree data object with the given name. *TreeName* is the
  name of the new tree object.  The form of *treeName* is the same as the
  **blt::tree create** operation.  Returns a token to the new tree data
  object. The tree will initially contain only a root node.

Blt_TreeNode **Blt_Tree_CreateNode**\ (Blt_Tree *tree*, Blt_TreeNode *parent*, const char *\ *nodeLabel*, Blt_TreeNode *before*)
  Creates a new child node in *parent*.  The new node is
  initially empty, but variables can be added with **Blt_Tree_SetVariable**.
  Each node has a serial number that identifies it within the tree.  No two
  nodes in the same tree will ever have the same ID.  You can find a node's
  ID with **Blt_Tree_NodeId**.

  The label of the node is *nodeLabel*.  If *nodeLabel* is NULL, a label in
  the form ""node0"", ""node1"", etc. will automatically be generated.
  *NodeLabel* can be any string.  Labels do not need to be unique.  A
  parent can contain two nodes with the same label. Nodes can be relabeled
  using **Blt_Tree_RelabelNode**.

  The position of the new node in the list of children is determined by
  *before*. *Before* is a child node of *parent*.  The new node will be
  inserted before this node.  If *before* is NULL, then the new
  node is appended onto the end of the parent's list.

Blt_TreeNode **Blt_Tree_DeleteNode**\ (Blt_Tree *tree*, Blt_TreeNode *node*)
  Deletes a given node and all it descendants.  *Node* is the node to be
  deleted.  The node and its descendant nodes are deleted.  Each node's
  variables are deleted also.  The reference count of the Tcl_Obj is
  decremented.

  Since all tree objects must contain at least a root node, the root node
  itself can't be deleted unless the tree is released and
  destroyed. Therefore you can clear a tree by deleting its root, but the
  root node will remain until the tree is destroyed.

int **Blt_Tree_Exists**\ (Tcl_Interp *\ *interp*, const char *\ *treeName*)
  Indicates if a tree data object exists by the given name.
  If the tree exists 1 is returned, 0 otherwise.

Blt_TreeNode **Blt_Tree_GetNode**\ (Blt_Tree *tree*, long *number*)
  Finds the node upon its serial number. The node is searched using the
  serial number. If no node with that ID exists in *tree* then NULL is
  returned.

int **Blt_Tree_GetToken**\ (Tcl_Interp *\ *interp*, const char *\ *treeName*, Blt_Tree *\ *treePtr*)
  Obtains A Token To A Tree Data Object.  *TreeName* is the name of an
  existing tree data object.  The pointer to the tree is returned via
  *tokenPtr*.

const char * \ **Blt_Tree_Name**\ (Blt_Tree *tree*)
  Returns the name of the tree data object.

unsigned int **Blt_Tree_NodeId**\ (Blt_TreeNode *node*)
  Returns the node serial number of *node*.  

int **Blt_Tree_ReleaseToken**\ (Blt_Tree *tree*)
  Releases the token associated with a tree data object.  Only when all no
  when else is using the tree data object will the data object itself will
  be freed.

KEYWORDS
--------

tree, treeview, widget

COPYRIGHT
---------

2015 George A. Howlett. All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

 1) Redistributions of source code must retain the above copyright
    notice, this list of conditions and the following disclaimer.
 2) Redistributions in binary form must reproduce the above copyright
    notice, this list of conditions and the following disclaimer in
    the documentation and/or other materials provided with the distribution.
 3) Neither the name of the authors nor the names of its contributors may
    be used to endorse or promote products derived from this software
    without specific prior written permission.
 4) Products derived from this software may not be called "BLT" nor may
    "BLT" appear in their names without specific prior written permission
    from the author.

THIS SOFTWARE IS PROVIDED ''AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES,
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY
AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.


  
