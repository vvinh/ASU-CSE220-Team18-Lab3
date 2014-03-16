#!/bin/bash
#***************************************************************************************************************
# FILE: Makefile
#
# DESCRIPTION
# Make file for the Vigenere encryption/decryption project.
#
# AUTHOR INFORMATION
# Kevin R. Burger [KRB]
#
# Mailing Address:
# Computer Science & Engineering
# School of Computing, Informatics, and Decision Systems Engineering
# Arizona State University
# Tempe, AZ 85287-8809
#
# Email: burgerk@asu
# Web:   http://kevin.floorsoup.com
#
# MODIFICATION HISTORY:
# --------------------------------------------------------------------------------------------------------------
# 24 Jan 2012 [KRB] Initial revision.
# 10 Mar 2014 [Vivian Vinh] modified and reuse professor Kevin Berger's script from exam1 extra credit.
#***************************************************************************************************************

# -ansi   : Compile the code assuming it conforms to the ANSI C standard.
# -c      : Compile a .c file only to produce the .o file.
# -g      : Put debugging information in the .o file. Used by the GDB debugger.
# -O0     : Turn off all optimization. Necessary if you are going to debug using GDB.
# -Wall   : Turn on all warnings. Your code should compile with no errors or warnings.
CFLAGS = -ansi -c -g -O0 -Wall

# If you add or remove .c files to or from the projet, then update this macro accordingly.
SOURCES = main.c \
          print.c       \
          scanner.c

# Creates a macro named OBJECTS from SOURCES where each occurrence of .c in SOURCES is replaced by a .o in
# OBJECTS. For example, if SOURCES=File1.c File2.c File3.c then OBJECTS would be File1.o File2.o File3.o.
OBJECTS = $(SOURCES:.c=.o)

# This is the target of the makefile and also the name of the binary.
TARGET = vigenere

# This rule states that the TARGET (vigenere) depends on the OBJECTS, i.e., the binary depends on the .o
# object code files. Therefore, to build binary, make will check to make sure all of the object code files
# are up-to-date. If any of them are newer than the target, then that means one of the .c files was compiled
# after the last time the binary was built, and therefore, the binary needs to be rebuilt. The gcc command
# invokes the linker to link all of the object code files together the produce the binary as the output (the
# -o option names the output file).
$(TARGET): $(OBJECTS)
	gcc $(OBJECTS) -o $(TARGET)

# This rules states that a .o file depends on a .c file. Therefore, if a .c file has a newer timestamp than
# its corresponding .o file, then the .c file was changed since the last time it was compiled to produce a
# .o file. Therefore, the .c file has to be recompiled to bring the .o file up-to-date. The gcc command
# compiles the .c file using the options stored in the CFLAGS macro. $< is an automatic variable that refers
# to the prerequisite on the right hand side of the : symbol in the rule, i.e., the .c file. The $@ automatic
# variable refers to the .o file on the left hand side of the : symbol in the rule, i.e., the .o file.
%.o: %.c
	gcc $(CFLAGS) $< -o $@

# This rules states that a .d file depends on a .c file. Therefore, if a .c file has a newer timestamp than
# its corresponding .d file, then the .c file was changed since the last time the gcc -MM command was run to
# produce the .d file. The rm command deletes the old .d file, and then the gcc command creates a new .d file
# by using the -MM command line option. Note that the output of the -MM option is normally send to stdout
# so we redirect stdout to send the output to the .d file.
%.d: %.c
	rm -f $@; gcc -MM $< > $@

# Include all of the .d files into this location of the make file.
include $(SOURCES:.c=.d)

# A make file can have more than one target. When you type "make" at the Bash command line, the first target
# that is encountered in the make file is the default target and make will do what it can to build it. If you
# wish to have additional targets, you can define the target as a phony target. Now, typing "make clean" will
# cause make to build the "clean" target rather than the default target. The "clean" target cleans the project
# directory by deleting all of the .o files, all of the .d files, and the vigenere binary. Thus, it sets the
# directory back to containing just .c and .h files and the make file. After doing "make clean", a "make"
# command will cause the entire project to be rebuilt by recompiling every .c source code file.
.PHONY: clean
clean:
	rm -f $(OBJECTS)
	rm -f *.d
	rm -f $(TARGET)
