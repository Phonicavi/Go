This is Brown, a simple go program.

Version:
1.0, released March 28 2004.

Main features:
* Plays randomly, but avoids filling small eyes.

* Talks the Go Text Protocol (GTP) version 2 on stdin/stdout.
* Small code base.

Secondary features:
* Follows the GTP version 2 specification strictly but makes uncommon
  choices where the specification leaves details open.
* Always correctly scores its own finished games. (Assuming simple ko
  rule and three passes to end the game.)

Author:
Gunnar Farnebäck
gunnar@lysator.liu.se

Installation:
Compile the three files brown.c, gtp.c, and interface.c together into
a binary. An example for gcc is included in the Makefile.

Running:
Start the program and enter GTP commands. For a smoother interface,
use a GTP aware GUI. More information about the protocol and available
software can be found at http://www.lysator.liu.se/~gunnar/gtp.

Algorithm:
All empty vertices are considered, except
1) suicide moves and illegal ko captures (simple ko rule),
2) vertices which would be suicide for the opponent, unless playing
   there captures at least one opponent stone.
The move is randomly chosen from this list of vertices. It passes if
the list is empty.

Files:
README        - This file
brown.cpp     - Go board logic and move generation
brown.h       - Header file
gtp.cpp       - GTP support code, imported from GNU Go
gtp.h         - GTP header file, imported from GNU Go
interface.cpp - GTP interface code and main() function
Makefile      - Simple makefile

License:
The MIT/X11 license. See the source code files for details.

Copyright:
The files gtp.c and gtp.h are taken from GNU Go and are copyright the
Free Software Foundation. The remaining files are copyright Gunnar
Farnebäck and are independent from GNU Go. Any similarities between
those files and code from GNU Go are due to constraints on the code
structure imposed by using gtp.c/gtp.h and the fact that the
corresponding code in GNU Go was written by the same person.
