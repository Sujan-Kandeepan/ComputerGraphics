# CS 3GC3 - Assignment 1

## Instructions to Compile

Run `make` within the current `A1` directory using the included `makefile` (ensuring `main.o` is included in the `$(PROGRAM_NAME)` rule) to compile and run `main.cpp` from Part A. Repeat with `main.o` changed to `paint.o` (may need to run `make clean`) in the `makefile` to compile and run `paint.cpp` from Part B.

## Additional Features in Part B

- Numbers '6' to '0' to change background colour of window
- Display text for point size/colour and background colour within window
- Ability to resize window and continue drawing on existing window
- Window always appears in center of screen instead of relative to corner
- Points drawn appear round instead of square for more natural look
- Usage instructions printed to terminal during program launch

## Some Resources Used for Part B

- <https://community.khronos.org/t/how-to-set-the-background-color/27016>
- <https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glClearColor.xhtml>
- <https://stackoverflow.com/questions/2181955/center-an-opengl-window-with-glut>
- <https://stackoverflow.com/questions/2183270/what-is-the-easiest-way-to-print-text-to-screen-in-opengl>
- <https://stackoverflow.com/questions/51973475/gl-points-draws-a-circle>
- <https://web.archive.org/web/20180301041827/https://prideout.net/archive/colors.php>
- Frequently referenced OpenGL/GLUT and C++ documentation/examples
- Sometimes referenced code examples from course slides and lectures.
