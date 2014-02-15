# iwatch

A very simple utility to execute a command when a file changes.

Usage:

	iwatch file... command

The command is executed via /bin/sh so you can use shell piping, chaining, etc.

Examples:

- `iwatch *.c *.h "make tests"`  
  Runs `make tests` when you save a C or header file in the current directory.
  
- `iwatch tree.c tree.h tests/tree_test.c "make tests/tree_test && ./tests/tree_test"`  
  Rebuilds and runs the test case when `tree.c`, `tree.h` or the test case itself is saved.
  
- `iwatch paper.html paper.css "prince --style paper.css paper.html"`  
  Transforms the HTML document to a PDF using PrinceXML and the supplied stylesheet.
  
- `iwatch paper.md paper.css "markdown paper.md && prince --style paper.css paper.html"`  
  Converts a markdown document to HTML (different tool) and then to PDF. If your PDF viewer
  updates opened files automatically (Evince does) you see the finished PDF as soon as you
  save the markdown file.


## Installation

Compile it with the command `make iwatch` and put the `iwatch` binary somewhere in your $PATH.