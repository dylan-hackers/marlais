#
# Marlais Makefile
#

# see COPYRIGHT for use

#####
# initialization file (change this to suite your installation)
#
INIT_FILE=/usr/local/share/marlais/init.dylan

#INIT_FILE_FLAG = -DINIT_FILE=\"$(INIT_FILE)\"
INIT_FILE_FLAG = -DINIT_FILE=\"$(INIT_FILE)\" -DINFIX_INIT_FILE

######################################
### Operating System Configuration ###
#

# Flag to distinguish Solaris from SunOS 4
# Uncomment if your system runs Solaris
#
# SYS_FLAG = -D__SunOS_5__
SYS_FLAG =

# Use following if your c library does not include strcasecmp
#
# STRCASECMP = -DNO_STRCASECMP
STRCASECMP_FLAG =

# Just in case you don't have sh, change this, but beware the comments
# below like #NOT-NT (which probably cause problems with csh)
#
SHELL = /bin/sh

# If your system has no readline support, remove this
#
READLINE_FLAGS = -DHAVE_READLINE#NOT-NT

##########################################
### Interpreter Customization Features ###
#

# use compact object representation (you might want
# to comment this out and completely remake when
# debugging)
#
SMALL_OBJECTS_FLAG= -DSMALL_OBJECTS

# string to mark output of the interpreter
OUTPUT_MARKER_FLAG = -DOUTPUT_MARKER='"=> "'
#OUTPUT_MARKER_FLAG = -DOUTPUT_MARKER='""'

#
# optimize tail calls if OPTIMIZE_TAIL_CALLS is defined
OPTIMIZE_TAIL_CALL_FLAG = -DOPTIMIZE_TAIL_CALLS

#
# cache methods in an attempt to improve GF invocation speed.
METHOD_CACHING_FLAG = -DUSE_METHOD_CACHING

# Determine class precedence algorithm to use
#
# L*Loops precedence
PRECEDENCE_FLAG = -DUSE_LL_PRECEDENCE
#
# CLOS precedence
#PRECEDENCE_FLAG = -DUSE_CLOS_PRECEDENCE

##########################################

VERSION = 0.6.4-io-beta
RLOG_VERSION = marlais_0_6_4_io_beta
PREVIOUS_VERSION = 0.6.4
PREVIOUS_RLOG_VERSION = marlais_0_6_4
PREVIOUS_VERSION_DIR = $(PROGRAM)-$(PREVIOUS_VERSION)
PATCHFILE = $(PROGRAM)-$(PREVIOUS_VERSION)-$(VERSION).diff
INCDIRS = gc

MARLAIS_FLAGS = $(SYS_FLAG) \
	$(STRCASECMP_FLAG) \
	$(INIT_FILE_FLAG) \
	$(READLINE_FLAGS) \
	$(SMALL_OBJECTS_FLAG) \
	$(OPTIMIZE_TAIL_CALL_FLAG) \
	$(OUTPUT_MARKER_FLAG) \
	$(MISC_FLAGS) \
	$(METHOD_CACHING_FLAG) \
	$(PRECEDENCE_FLAG) \
	 -DVERSION=\"$(VERSION)\"

CC=gcc

CFLAGS= -g -I$(INCDIRS) $(MARLAIS_FLAGS) $(CEXTRAS)

LIBS=-lncurses -lreadline -lm

FLEXFLAGS = -I
BYACCFLAGS = -d -v -t

DISTDIR = /tmp/marlais-$(VERSION)
GCDIST = ../gc

OBJS = 	alloc.o \
	apply.o \
	array.o \
	boolean.o \
	bytestring.o \
	character.o \
	class.o \
	classprec.o\
	deque.o \
	dylan.tab.o \
	error.o \
	env.o \
	eval.o \
	file.o \
	foreign_ptr.o \
	function.o \
	keyword.o \
	lex.yy.o \
	list.o \
	main.o \
	misc.o \
	number.o \
	object.o \
	parse.o \
	prim.o \
	print.o \
	read.o \
	sequence.o \
	slot.o \
	stream.o \
	symbol.o \
	syntax.o \
	sys.o \
	table.o \
	values.o \
	vector.o

GCOBJS = $(INCDIRS)/gc.a

CSRCS = alloc.c \
	apply.c \
	array.c \
	boolean.c \
	bytestring.c \
	character.c \
	class.c \
	classprec.c \
	classprec-clos.c \
	classprec-ll.c \
	deque.c \
	dylan.tab.c \
	error.c \
	env.c \
	eval.c \
	file.c \
	function.c \
	foreign_ptr.c \
	keyword.c \
	lex.yy.c \
	list.c \
	main.c \
	misc.c \
	number.c \
	object.c \
	parse.c \
	prim.c \
	print.c \
	read.c \
	sequence.c \
	slot.c \
	stream.c \
	symbol.c \
	syntax.c \
	sys.c \
	table.c \
	values.c \
	vector.c

SRCS = 	$(CSRCS) \
	dylan.tab.h \
	alloc.h \
	apply.h \
	array.h \
	boolean.h \
	bytestring.h \
	character.h \
	class.h \
	classprec.h \
	classprec-clos.h \
	classprec-ll.h \
	deque.h \
	dylan_lexer.h \
	error.h \
	env.h \
	eval.h \
	file.h \
	function.h \
	foreign_ptr.h \
	keyword.h \
	list.h \
	misc.h \
	number.h \
	object.h \
	parse.h \
	prim.h \
	print.h \
	read.h \
	sequence.h \
	slot.h \
	stream.h \
	symbol.h \
	syntax.h \
	sys.h \
	table.h \
	values.h \
	vector.h \
	yystype.h \
	globaldefs.h \
	globals.h

OTHER_C_FILES = \
	foreign.c \
	foreign.h \
	macintosh.c \
	macintosh.h

OTHER_NONC_FILES = \
	ACKNOWLEDGMENTS \
	ADDED \
	BUGS \
	COPYING.LIB \
	COPYRIGHT \
	DIFFERENCES \
	HACKING \
	INSTALL \
	MACHINES \
	KNOWN-BUGS \
	PORTING \
	README \
	ARGUMENTS \
	ChangeLog \
	Makefile \
	init.dylan \
	dylan.l \
	dylan.y \
	foreign.dyl \
	marlais.el \
	dylan-hilit19.el \
	.indent.pro\
	NT_MAKEFILE

OTHER_FILES = $(OTHER_C_FILES) $(OTHER_NONC_FILES)

NO_RCS_FILES = \
	ChangeLog \
	dylan.tab.c \
	dylan.tab.h \
	lex.yy.c \
	NT_MAKEFILE

PROGRAM = marlais

$(PROGRAM): $(OBJS) $(GCOBJS)
	$(CC) $(CFLAGS) -o $(PROGRAM) $ $(OBJS) $(GCOBJS) $(LIBS)

# a target for building parser/scanner on the Mac under MachTen.
mac: dylan.tab.c lex.yy.c
	mactext dylan.tab.[ch] lex.yy.c

lex.yy.c: dylan.l dylan.tab.h
	flex $(FLEXFLAGS) dylan.l

dylan.tab.h: dylan.tab.c

dylan.tab.c: dylan.y
	yacc $(BYACCFLAGS) dylan.y
	mv y.tab.c dylan.tab.c
	mv y.tab.h dylan.tab.h

$(INCDIRS)/gc.a:
	cd $(INCDIRS); $(MAKE) CC=$(CC)

NT_MAKEFILE: Makefile
	mv NT_MAKEFILE NT_MAKEFILE.bak
	sed -e '/#NOT-NT/s/^/# /' -e '/#NT/s/^# //' <Makefile >NT_MAKEFILE

clean:
	rm -rf $(OBJS)

real_clean: clean
	rm dylan.tab.c lex.yy.c
	cd $(INCDIRS); $(MAKE) clean


depend:
	awk '/^# dependencies/ {exit 0;} {print $0;} END{print "# dependencies";}' <Makefile >Makefile.new
	gcc $(CFLAGS) -I$(INCDIRS) -MM *.c >> Makefile.new
	mv Makefile Makefile.bak
	mv Makefile.new Makefile

dist:	depend $(SRCS) $(OTHER_FILES) $(DISTSUBDIRS)
	mkdir -p $(DISTDIR)
	cp -pr $(OTHER_FILES) $(SRCS) $(DISTDIR)
	cp -pr $(GCDIST) $(DISTDIR)/gc

ftp:	$(DISTDIR)
	(cd $(DISTDIR)/.. ; \
	tar cvf $(FTPDIR)/$(PROGRAM)-$(VERSION).tar $(PROGRAM)-$(VERSION) ; \
	gzip <$(FTPDIR)/$(PROGRAM)-$(VERSION).tar >$(FTPDIR)/$(PROGRAM)-$(VERSION).tar.gz ; \
	compress <$(FTPDIR)/$(PROGRAM)-$(VERSION).tar >$(FTPDIR)/$(PROGRAM)-$(VERSION).tar.Z ; \
	shar  $(DISTDIR) > $(FTPDIR)/$(PROGRAM)-$(VERSION).shar ; \
	)

patch:
	(cd $(DISTDIR)/.. ; diff -rc $(PREVIOUS_VERSION_DIR) $(DISTDIR) >$(PATCHFILE))

checkin:
	for x in $(SRCS) $(OTHER_C_FILES);\
	 do (indent $$x; ci -l $$x); done ;\
	ci -l $(OTHER_NONC_FILES); \
	for x in $(NO_RCS_FILES); do (rm -f RCS/$$x,v); done

ChangeLog: RCS/*
	rlog -r$(PREVIOUS_RLOG_VERSION): RCS/* > ChangeLog

# dependencies
alloc.o : alloc.c alloc.h object.h env.h symbol.h $(INCDIRS)/gc.h error.h 
apply.o : apply.c apply.h object.h alloc.h env.h class.h symbol.h eval.h error.h \
  function.h keyword.h list.h print.h prim.h syntax.h table.h values.h vector.h 
array.o : array.c array.h object.h alloc.h env.h error.h list.h number.h prim.h \
  symbol.h 
boolean.o : boolean.c boolean.h object.h prim.h 
bytestring.o : bytestring.c bytestring.h object.h alloc.h env.h character.h \
  error.h number.h prim.h symbol.h 
character.o : character.c character.h object.h number.h prim.h 
class.o : class.c class.h symbol.h object.h alloc.h env.h apply.h array.h boolean.h \
  bytestring.h classprec.h classprec-ll.h deque.h error.h eval.h function.h globaldefs.h \
  keyword.h list.h number.h prim.h slot.h table.h values.h vector.h 
classprec-clos.o : classprec-clos.c classprec.h classprec-ll.h object.h class.h \
  symbol.h error.h list.h print.h 
classprec-ll.o : classprec-ll.c classprec.h classprec-ll.h object.h class.h \
  symbol.h error.h list.h 
classprec.o : classprec.c classprec-ll.c classprec.h classprec-ll.h object.h \
  class.h symbol.h error.h list.h 
deque.o : deque.c deque.h object.h error.h list.h prim.h symbol.h 
dylan.tab.o : dylan.tab.c boolean.h object.h bytestring.h error.h $(INCDIRS)/gc.h globaldefs.h \
  list.h number.h symbol.h table.h vector.h yystype.h dylan_lexer.h 
env.o : env.c env.h object.h alloc.h bytestring.h class.h symbol.h error.h eval.h \
  function.h keyword.h list.h prim.h print.h table.h 
error.o : error.c error.h object.h alloc.h env.h apply.h bytestring.h class.h \
  symbol.h eval.h list.h parse.h prim.h print.h read.h yystype.h dylan_lexer.h 
eval.o : eval.c eval.h object.h env.h alloc.h apply.h error.h list.h print.h \
  syntax.h 
file.o : file.c file.h object.h dylan_lexer.h env.h eval.h error.h foreign_ptr.h \
  list.h parse.h prim.h read.h 
foreign.o : foreign.c foreign.h object.h prim.h 
foreign_ptr.o : foreign_ptr.c foreign_ptr.h object.h 
function.o : function.c function.h object.h apply.h class.h symbol.h env.h error.h \
  eval.h keyword.h list.h number.h prim.h table.h values.h vector.h 
keyword.o : keyword.c keyword.h object.h bytestring.h list.h prim.h symbol.h 
lex.yy.o : lex.yy.c alloc.h object.h env.h bytestring.h character.h dylan.tab.h \
  globaldefs.h list.h number.h symbol.h yystype.h dylan_lexer.h 
list.o : list.c list.h object.h apply.h boolean.h error.h number.h \
  prim.h symbol.h 
macintosh.o : macintosh.c macintosh.h object.h /local/lib/gcc-lib/sparc-sun-sunos4.1.3/2.4.5/include/string.h \
  number.h values.h error.h foreign_ptr.h 
main.o : main.c alloc.h object.h env.h apply.h array.h boolean.h bytestring.h \
  character.h class.h symbol.h deque.h dylan_lexer.h error.h eval.h file.h function.h \
  globals.h globaldefs.h keyword.h list.h misc.h number.h parse.h print.h read.h \
  slot.h syntax.h stream.h sys.h table.h values.h vector.h 
misc.o : misc.c misc.h object.h alloc.h env.h prim.h 
number.o : number.c number.h object.h prim.h values.h 
object.o : object.c object.h error.h number.h 
parse.o : parse.c parse.h object.h error.h print.h yystype.h dylan_lexer.h 
prim.o : prim.c prim.h object.h alloc.h env.h bytestring.h error.h list.h symbol.h 
print.o : print.c print.h object.h apply.h character.h error.h eval.h env.h \
  list.h prim.h slot.h 
read.o : read.c read.h object.h bytestring.h character.h dylan_lexer.h dylan.tab.h \
  error.h file.h list.h number.h parse.h prim.h symbol.h vector.h 
slot.o : slot.c slot.h object.h apply.h class.h symbol.h error.h eval.h env.h \
  globaldefs.h keyword.h list.h prim.h vector.h 
stream.o : stream.c stream.h object.h error.h prim.h 
symbol.o : symbol.c symbol.h object.h alloc.h env.h bytestring.h 
syntax.o : syntax.c syntax.h object.h alloc.h env.h apply.h boolean.h bytestring.h \
  class.h symbol.h error.h eval.h keyword.h list.h function.h misc.h number.h \
  print.h table.h values.h 
sys.o : sys.c sys.h object.h bytestring.h error.h number.h prim.h values.h 
table.o : table.c table.h object.h alloc.h env.h apply.h error.h \
  list.h number.h prim.h symbol.h 
values.o : values.c values.h object.h alloc.h env.h error.h list.h prim.h 
vector.o : vector.c vector.h object.h alloc.h env.h error.h list.h \
  number.h prim.h symbol.h 
