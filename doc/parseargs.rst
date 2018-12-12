
==============
blt::parseargs
==============

----------------------------
Command line argument parser
----------------------------

.. include:: man.rst
.. include:: toc.rst

SYNOPSIS
--------

**blt::parseargs create** ?\ *parserName*\ ?

**blt::parseargs destroy** ?\ *parserName* ... ?

**blt::parseargs exists**  *parserName*

**blt::parseargs names** ?\ *pattern* ... ?

DESCRIPTION
-----------

The **blt::parseargs** command creates a new TCL command to parse
command-line arguments.  A *parseargs* object represents a set of
arguments.  Arguments specify how the words of the command line are
interpreted.

Flags or options are arguments that start with a designated prefix
character (such as "-").  Options may occur anywhere in the command line
(after the program name).  The parser processes options and their values
first.  Options may take a different number of words for their values.

Alternatively, positional arguments do not start with a prefix character.
They are processed and assigned in the order in which they were created
using the leftover words after options have been processed.

By default all arguments are optional.  Setting the argument's
**-required** flag specifies that the argument is required.  It is an error
if the command line does not contain that argument.  You can also make
options (arguments that start with prefix characters) required.

Any words that do not match an option or a positional argument are returned
in a TCL list.  So you can have the parser handle a first pass of parsing
and then parse the remaining words however you wish.

SYNTAX
------

**blt::parseargs create** ?\ *parserName*\ ?    ?\ *switches* ... ?
  Creates a new parser object.  The name of the new parser object is
  returned.  If no *parserName* argument is present, then the name of the
  parseargs is automatically generated in the form "parseargs0",
  "parseargs1", etc.

  If the substring "#auto" is found in *parserName*, it is automatically
  substituted by a generated name.  For example, the name ".foo.#auto.bar"
  will be translated to ".foo.parseargs0.bar".

  A new TCL command (by the same name as the parser) is created.  Another
  TCL command or parseargs object can not already exist as *parserName*.
  If the TCL command is deleted, the parser will also be freed.  [Note that
  parsers are by default, created in the current namespace, not the global
  namespace, unless *parserName* contains a namespace qualifier, such as
  "fred::myParseargs".]

  *Switches* can be any of the following.
  
    **-abbreviations** *boolean*
      If true, indicates to accept abbreviations of long argument names
      when parsing the command line.  The default is "0".

    **-default** *string*
      Specifies the default value for arguments.  This is the value of
      optional arguments if they are not specified on the command line.
      *String* is an arbitrary text string. It does not have to match the
      type of the argument. The default is "".

    **-description** *string*
      Specifies the description for the command that is displayed in the
      help message.  *String* is an arbitrary text string printed immediately
      after the command's usage line. The string is printed word by word
      and automatically wraps at 75 characters. The default is "".

    **-epilog** *string*
      Specified the text string displayed in the help message after the
      usage, description, and arguments.  *String* is an arbitrary text
      string.  It is printed verbatim and newlines are retained. The default
      is "".

    **-error** *errorList*
      Specifies the types of conditions that should generate errors in the
      parser. *ErrorList* is a TCL list of error conditions.  It can contain
      any combination of the following.

      **badoption**
	Generate an error if an word that looks like an option is not found
	in the parser's list of arguments.  

      **extraargs**
	Generate an error the parser does not match all words. Any word
	that's left over will trigger an error.

      The default is "badoption".

    **-prefixchars** *charString*
      Specifies the set of characters used to prefix options. The default
      is "-+".

    **-program** *programName*
      Specifies the name of the command to be displayed in the help usage.
      If *programName* is "", then the name of the script or executable is
      printed.  The default is "".

    **-usage** *usageString*
      Specifies the usage line to be displayed in the help usage.  
      If *programName* is "", then the usage is generated from the program
      name and the commands arguments.  The default is "".

**blt::parseargs destroy** ?\ *parserName* ... ?
  Deletes zero or more parsers.  *ParserName* is the name of the parser
  returned by the **create** operation.  The TCL command associated with
  *parserName* is also removed.

**blt::parseargs exists** *parserName*
  Indicates if the parser object *parserName* exists. *ParserName* is the name
  of a parser object created by the **create** operation. Returns "1"
  if the named parser exists, "0" otherwise.

**blt::parseargs names** ?\ *pattern*\ ... ?
  Returns the names of all the command-line parsers.  If one or more
  *pattern* arguments are provided, then the name of any parser matching
  *pattern* will be returned. *Pattern* is a **glob**\ -style pattern.


OPTIONS VS. POSITIONAL ARGUMENTS
--------------------------------

An option is an argument whose long or short name starts with a special
prefix character such as "-" or "+".  The option must have its long
(--list) or short (-l) name set.  A short name is the prefix character and
a single character.

 ::

     -f myFile
     
Both short and long names can have multiple prefix characters, but do not
have to start with two prefix characters.

  ::

     --file myFile
     -file myFile
     
If the prefix character is "-" or "+" the name can not a number.

  ::

     -1 myFile
     -1000 myFile
     +2 myFile
     
An option may take a set number or variable number of words for its value.

  ::

    -files myFile1 myFile2 myFile3
    -f myFile1 myFile2 myFile3
     
If the option takes a predefined number of words, the words may be any
arbitrary string.  If the option takes a variable number of arguments, the
number of words is determined by the either the end of the words or if the
next word looks like an option.

  ::

     -numbers 1 2 3 4 5 -letters a b c d e 

You can also specify argument whose values look like options.

  ::

     --command ls -l --color=auto

Options and positional arguments are by default optional, but you can make
them required.  An error will be automatically generated when an option or
positional argument is not specified on the command line.

PARSER OPERATIONS
-----------------

After you create a parseargs object, you can use its TCL command to query
or modify it.  The general form is

  *parserName* *operation* ?\ *arg*\ ?...

Both *operation* and its arguments determine the exact behavior of the
command.  The operations available for parseargss are listed below.

*parserName* **add** *argName*  ?\ *switches* ... ?
  Adds a new argument. *ArgName* is the name of the argument. It is used
  with other parser operations to reference the argument.  The following
  switches are valid.

  **-action**  *actionName*
    Specifies how to process argument values.  *ActionName* can be
    one of the following.

    **append**
      Append the value. If the argument is found on the command-line
      more than once, new values will be appended to a list of values.

    **store** 
      Set the value. For arguments that take no values (**-nargs** is 0),
      the default value is used.  If the argument is found on the
      command-line more than once, the new value will replace the old.

    **store_false**
      Set the current value to false. This replaces the old value.

    **store_true**
      Set the current value to true.  This replaces the old value.

    **help**
      Return the help message.  Parsing of the argument stops and the
      help message is returned. 

    The default is "store".
    
  **-allowprefixchars**  *boolean*
    Indicates that the values for the argument may start with prefix
    characters.  Normally the number of values an argument takes is checked
    by seeing if the value looks like an option.  If *boolean* is true,
    even valid options (such as "--help") will be interpreted as values.
    The default is "0".

  **-command**  *cmdPrefix*
    Specifies a TCL command to be invoked if the argument is processed by
    the parser. *CmdPrefix* is called with an extra argument (the argument's
    current value) that is appended to the end.  The result of the command is
    the new value for the argument. If *cmdPrefix* is "", then no
    command is invoked. The default is "".
    
  **-choices**  *choiceList*
    Specifies a TCL list of possible values for the argument.  *ChoiceList*
    is the set of possible values.  Any value for this argument must be in
    this set. If *choiceList* is "", then no check is done.  The default is
    "".
     
  **-default**  *string*
    Specifies the default value for the argument if it is optional.  This
    value is also be used as the argument's value if the argument takes no
    values (see the **-nargs** option).  If this switch is not set, then
    the argument uses to the parser's option **-default** string.

  **-destination**  *destArgName*
    Specifies the name of another argument where to store the argument's
    current value.  This is typically used when you want different
    arguments to append to the same list.  *DestArgName* is the name of
    argument returned by the **add** operation.  If *destArgName* is "",
    then values are store in *argName*.  The default is "".
    
  **-exclude**  *excludeList*
    Specifies the names of arguments that are mutually exclusive to
    *argName*.  *ArgName* can not be set with any of argument in
    *excludeList*.  *ExcludeList* is a TCL list of argument names. If
    *excludeList* is "", then there is no exclusion.  The default is "".

  **-help**  *helpString*
    Specifies the help message specific to *argName* that is displayed
    in the generated help message.

  **-long**  *longName*
    Specifies the long switch name for the argument. *LongName* is the name
    of the switch.  *LongName* must start with one othe parser's defined
    prefix characters (see the parser's **-prefixchars** option).  If only
    one prefix character is specified, the characters after the prefix
    character can not be a number (such as "-1000").  If *longName* is "",
    then no argument long name is defined.  The default is "".

  **-metavar**  *string*
    Specifies the name or type of the argument when displayed in the
    help message.  

  **-max**  *maxValue*
    Specifies the maximum value accepted. The option is used only for
    "integer" and "double" arguments. *MaxValue* is the maximum number
    allowed for argument's value.  An error is returned if the parsed value
    of *argName* is greater than *maxValue*.  No checking is performed to
    verify that **-min** and **-max** form a valid range.
    
  **-min**  *minValue*
    Specifies the minimum value accepted. The option is used only for
    "integer" and "double" arguments.  *MinValue* is the minimum number
    allowed for argument's value.  An error is returned if the parsed value
    of *argName* is less than *minValue*.  No checking is performed to
    verify that **-min** and **-max** form a valid range.

  **-nargs**  *argCount*
    Specifies the number of words the argument will take for its value.
    *ArgCount* can be one of the following.
    
    **?**
      Specifies that the argument may take either no value or one value.
      If no value is present, the argument will be set to its default
      or specified value (see the **-default** and **-value** options).

    **\***
      Specifies that the argument may take zero or more words.  If no value
      is present, the argument will be set to its default or specified
      value (see the **-default** and **-value** options).

    **+** 
      Specifies that the argument may take one or more words.  It is an
      error if no values are present (the next argument is a switch or it's
      the last word on the command line).

    **last** 
      Specifies that this is the last option on the command line. All
      remaining words are treated as values even if they look like options.

    *numArgs*
      Specifies the number of words for the argument. *NumArgs* is
      a non-negative integer.

    The default is "1".
    
  **-required**  *boolean*
    Indicates that the argument is required.  The *parse* operation will
    return an error if the argument is not set.
    
  **-short**  *shortName*
    Specifies the short switch name for the argument. *ShortName* is the
    name of the switch.  It is typically the prefix character and one other
    letter (such as "-x").  *ShortName* must start with one othe parser's
    defined prefix characters (see the parser's **-prefixchars** option).
    In addition, the character after the prefix character can not be a
    number (such as "-1").  If *shortName* is "", then no argument
    short name is defined.  The default is "".
    
  **-type**  *typeName*
    Specifies the type of values acceptable for the argument.  

    **boolean** 
      Specifies the type of values to be booleans.  Any value in a form
      accepted by **Tcl_GetBoolean** is valid.

    **double** 
      Specifies the type of values as real number.  Argument values will
      be checked to verify they are valid numbers.

    **float** 
      Same as **double**.

    **integer**
      Specifies the type of values as integers.  Argument values will
      be checked to verify they are valid integers.

    **number** 
      Same as **double**.

    **string** 
      Specifies the type of values to be strings.

    The default type is "string".

  **-value**  *value*
    Specifies the value for the argument when the argument takes no values.
    This overrides the default value specified (see the **-default**
    option).  If *value* is "", then the default value is used.  The
    default is "".

  **-variable**  *varName*
    Specifies the name of a TCL variable to be set with the value of this
    argument.  If *varName* is "", then the variable is not set.
    The default is "".

*parserName* **argument cget** *argName* *option*
  Returns the current value of the argument configuration option given by
  *option*. *ArgName* is the name of the argument created by the
  **add** operation. *Option* and may have any of the values accepted
  by the **configure** operation.  They are described in the **add**
  operation above.

*parserName* **argument configure** *argName* ?\ *option* *value* ... ?
  Queries or modifies the argument configuration options for
  *argName*. *ArgName* is the name of argument returned by the
  **add** operation.

  If no options are specified, a list describing all of the available
  options for *argName* is returned.  If *option* is specified with no
  *value*, then this command returns a list describing the one named option
  (this list will be identical to the corresponding sublist of the value
  returned if no *option* is specified).  If one or more *option*\ -*value*
  pairs are specified, then this command modifies the given argument
  option(s) to have the given value(s); in this case the command returns
  the empty string.  See the **add** operation for what *option* and
  *value* pairs are valid.

*parserName* **cget** *argName* *option*
  Returns the current value of the argument configuration option given by
  *option*. *ArgName* is the name of the argument created by the
  **add** operation. *Option* and may have any of the values accepted
  by the **configure** operation.  They are described in the **add**
  operation above.

*parserName* **configure** *argName* ?\ *option* *value* ... ?
  Queries or modifies the argument configuration options for
  *argName*. *ArgName* is the name of argument returned by the
  **add** operation.

  If no options are specified, a list describing all of the available
  options for *argName* is returned.  If *option* is specified with no
  *value*, then this command returns a list describing the one named option
  (this list will be identical to the corresponding sublist of the value
  returned if no *option* is specified).  If one or more *option*\ -*value*
  pairs are specified, then this command modifies the given argument
  option(s) to have the given value(s); in this case the command returns
  the empty string.  See the **create** operation of **blt::parseargs** for
  what *option* and *value* pairs are valid.

*parserName* **delete** ?\ *argName*  ... ?
  Deletes zero or more arguments from the parser. *ArgName* is the name of
  the argument returned by the **add** operation.

*parserName* **exists** *argName* 
  Indicates if *argName* exists in the parser. *ArgName*
  is the name of the argument returned by the **add** operation. Returns
  "1" if *argName* exists, "0" otherwise.

*parserName* **get** ?\ *argName*\ ?  ?\ *defValue*\ ?
  Returns the current value for *argName*.  *ArgName* is the name of the
  argument returned by the **add** operation.  If *argName* is not given,
  then a name-value list of arguments and their values is returned.

  If a value has not been set for *argName*, the default value is returned.
  This is either the parser's default value (see the parser's **-default**
  switch) or the one designated for the argument (see the argument's
  **-default** switch).  You can also provide a *defValue* argument,
  this value is returned instead (*argName* will still not have a default
  value).

*parserName* **help** 
  Returns the help message for the parser.

*parserName* **ischanged** *argName* 
  Indicates if *argName* was modified by the last **parse** operation.
  *ArgName* is the name of the argument returned by the **add**
  operation. Returns "1" if *argName* was modeified, "0" otherwise.

*parserName* **parse**  *argList* ?\ *arrayName*\ ?
  Parses the *argList*, extracting the known arguments, returning the
  remaining arguments. *ArgList* is a TCL list of words representing the
  command line (without the command name).  If an *arrayName* argument is
  given, it is the name of a TCL array variable that will map the argument
  names and their respective current values.

*parserName* **reset** 
  Resets the parser by resetting all the current argument values to their
  defaults (this is the value specified by the **-default** switch).

*parserName* **restore**  *list* 
  Sets the current value for the given arguments.  *List* is a TCL list
  of name-value pairs of the argument name and its new current value.
  It is an error is the named argument does not exist.  

*parserName* **save** 
  Returns the current values for all arguments.  This command returns a TCL
  list of name-value pairs of the argument name and its current value.  If
  the current value is not set, the default argument is returned.

*parserName* **set** ?\ *argName*  *value* ...\ ?
  Sets the current value for the given argument.  *ArgName* is the
  name of the argument returned by the **add** operation. There may be 
  several pairs or argument names and values.
  
EXAMPLES
--------

  FIXME: need several examples.

DIFFERENCES WITH ARGPARSE
-------------------------

 1) You can have long switches that start with a single prefix character.
    (-long).
 2) Short switches can be more than one character long (such as "-bg").
    There can be no combinations of short switches in a single word such as
    "-abcd".  Unlike long switches, short switches are matched exactly,
    regardless of the **--abbrevations** flag.  Short switches cannot
    start with a number (such as -1).
 3) Equal signs in arguments (such as -long=value -s=0) are not supported.
    The value must be a separate word (--long value).  Likewise, short
    switches values must be a separate word (such as "-s 0"). No abutted
    values such as "-s0" or "-s=0". 
 4) const is called value.
 5) By default, it is not an error to have left over words on the parsed
    command-line.
 6) A switch is anything that starts with a prefix character and follows
    the above rule. If the prefix character is a '-', it can't start with
    a number.  Everything thing else is a possible value.  

 +-----------+-----------+----------+
 |           | ParseArgs | argparse |
 +-----------+-----------+----------+
 | -10       | value     | value    | 
 +-----------+-----------+----------+
 | -abc      | switch    | switch   |
 +-----------+-----------+----------+
 | -1,2      | value     | switch   |
 +-----------+-----------+----------+

KEYWORDS
--------

parseargs

COPYRIGHT
---------

2018 George A. Howlett. All rights reserved.

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
