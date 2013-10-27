#-------------------------------------------------------------------
#        D - F L A T   M A K E F I L E  -  Linux  
#  ( Based on the Makefile for Borland C++ 3.1 and 4.0 ) 
#-------------------------------------------------------------------

# Change history:  
# 26-Oct-2013 - First cut at conversion to Linux.  


all : memopad memopad.hlp

#------------------------------------------------
# NOTE: Set DRIVE to match where you installed your compiler
#------------------------------------------------
DRIVE = /usr/bin
#-------------------------------------------------------------------
#  Delete the TESTING macro to eliminate the Reload
#  Help file selection on the Help menu.
#-------------------------------------------------------------------
# TESTING = TESTING_DFLAT
#-------------------------------------------------------------------
#  This macro builds the full D-Flat system with all options enabled.
#  Comment it out for a minimum system or selectively
#  comment out the #defines at the top of dflat.h.
#-------------------------------------------------------------------
FULL = BUILD_FULL_DFLAT
#-------------------------------------------------------------------

MODEL = l
#------------------------------------------------
# NOTE: Delete the DEBUG and LINKDEBUG macros to 
# build without debugging information in the .EXE
#------------------------------------------------
DEBUG = -v
LINKDEBUG = /m /v
#------------------------------------------------
# NOTE: Temporary file space. Change to match
#       your computer. A RAM disk works best.
#------------------------------------------------
HEADERS=/home/andy/D-Flat/tcdef.sym
#------------------------------------------------
COMPILE = gcc -Od $(DEBUG) -D$(TESTING) -D$(FULL) -DBCPP -c -d -m$(MODEL) -H=$(HEADERS)
LINK= tlink $(LINKDEBUG) $(DRIVE)/lib/c0$(MODEL) 
LIBS= $(DRIVE)/lib/c$(MODEL)
#------------------------------------------------

.c .o:
	$(COMPILE) {$*.c }

memopad : memopad.o dialogs.o menus.o dflat.lib
	$(LINK) memopad dialogs menus,memopad,memopad,dflat $(LIBS)

dflat.lib :   window.o video.o message.o                     \
	mouse.o console.o textbox.o listbox.o          \
	normal.o config.o menu.o menubar.o popdown.o   \
	rect.o applicat.o keys.o sysmenu.o editbox.o   \
	dialbox.o button.o fileopen.o msgbox.o         \
	helpbox.o log.o lists.o statbar.o decomp.o     \
	combobox.o pictbox.o calendar.o barchart.o     \
	clipbord.o search.o dfalloc.o checkbox.o       \
	text.o radio.o box.o spinbutt.o  watch.o       \
	slidebox.o direct.o editor.o
	del dflat.lib
	tlib dflat @dflat.bld

huffc : huffc.o htree.o
	$(LINK) huffc htree,$*.exe,$*,$(LIBS)

fixhelp : fixhelp.o decomp.o
	$(LINK) fixhelp decomp,$*.exe,$*,$(LIBS)

memopad.hlp : memopad.txt huffc fixhelp
	huffc memopad.txt memopad.hlp
	fixhelp memopad


