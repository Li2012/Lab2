### CSE532S Fall 2011 Provided Makefile

#		Replace this with your name, leaving the quotes
PROJECT_TITLE	= CSE532S Lab1
LASTNAME		= BENJAMIN
FIRSTNAME		= ALEX
RETURN_MAIL     = abenjamin@wustl.edu

#		The names of the executable files
EXECUTABLE1	= Server
EXECUTABLE2	= Client

#		Source files you want to compile
#		(NOTE: Don't include header (.h) files, or source files
#		that contain function template or class template member
#		function definitions.)
CMPL_COMM	= Codes.cpp DataWrapper.cpp Card.cpp
CMPL_SRCS1	= ClientHandler.cpp Dealer.cpp Deck.cpp Game.cpp Player.cpp Server.cpp ServerHandler.cpp
CMPL_SRCS2	= Client.cpp ConnectInfo.cpp Player_Svc_Handler.cpp PlayerConnection.cpp Poker.cpp Hand.cpp Discard.cpp

#               Source files containing *only* function template or class 
#		template member function definitions: note that you
#               should always provide separate source files for template 
#               and non-template definitions (and not mix both kinds in one file).
TMPL_SRCS1	= 
TMPL_SRCS2	=

#               Header files
HEADER_COMMON	 = Card.h Codes.h Const.h DataWrapper.h Lib.h Stream.h
HEADER_FILES1    = ClientHandler.h Dealer.h Deck.h Game.h Player.h ServerHandler.h
HEADER_FILES2    = ConnectInfo.h Player_Svc_Handler.h PlayerConnection.h Poker.h Hand.h Discard.h

TEST_SCRIPTS	= 
#               Other files to turn in (Makefile, readme, output files, etc.)
OTHER_FILES     = Makefile README.txt

#               Any special flags that should be set for compilation
SPECIAL_FLAGS  =  -DTEMPLATES_MUST_INCLUDE_SOURCE  # -DDEBUG

#               Please change this if you use a different file extension
OBJS1     = $(CMPL_SRCS1:.cpp=.o) $(CMPL_COMM:.cpp=.o)
OBJS2     = $(CMPL_SRCS2:.cpp=.o) $(CMPL_COMM:.cpp=.o)
                                                                              #
#################### CHANGE ANYTHING BELOW THIS LINE AT YOUR OWN RISK ###################

#		C++ compiler
CXX		= g++

#		Submissions are normally sent to the grader account
GRADERS		= cse532@cec.wustl.edu
MAILTO		= $(GRADERS)


#               All your source files (needed for executable dependency)
USER_SRCS1        = $(CMPL_SRCS1) $(TMPL_SRCS1) $(HEADER_FILES1) $(CMPL_COMM) $(HEADER_COMMON)
USER_SRCS2        = $(CMPL_SRCS2) $(TMPL_SRCS2) $(HEADER_FILES2) $(CMPL_COMM) $(HEADER_COMMON)
USER_SRCS         = ${USER_SRCS1} ${USER_SRCS2}

#               Provided source files (i.e., History files, etc)
PROVIDED_SRCS   =

#               All your source files (needed for executable dependency)
ALL_SRCS        = $(USER_SRCS) $(PROVIDED_SRCS)

#               All files to turn in (including readme, output files, etc)
ALL_FILES       = $(USER_SRCS) $(TEST_SCRIPTS) $(OTHER_FILES)

ZIP_FILE		= CSE_532_Lab_1.zip

# 		libraries used on Linux
LIBS    =       -lACE  -ldl

# 		libraries used on Solaris
#LIBS    =       -lACE  -lnsl  -lsocket 

#               The name of the previewer (pageview or ghostview)
PREVIEWER = /usr/openwin/bin/pageview -right

#               Any define flags that should be set for conditional compilation
DEFFLAGS  = -DUNIX -D_REENTRANT

#               Any -I directories with .h files that should be included
INCFLAGS  =     -I$(ACE_ROOT) -Icommon

#               Flags that are specific to SUN object code
SUNFLAGS  =    #-misalign

#               Any -L directories in which there are .so files that should
#               be linked
LIBLOCFLAGS     = -L$(ACE_ROOT)/ace -L./

#               Any general flags that should be set for the compiler
#               NOTE: to optimize fully, change -g to -O4
CXXFLAGS  =     -Wall -W -g $(SPECIAL_FLAGS) $(DEFFLAGS) $(INCFLAGS)

#               The concatenation of all the flags that the compiler should get
CCFLAGS = $(SUNFLAGS) $(CXXFLAGS)

#				Flags for linker
LDFLAGS = $(LIBLOCFLAGS) $(LIBS)

###
# Targets
###

all: $(EXECUTABLE1) $(EXECUTABLE2)

#       Construct the executable
$(EXECUTABLE1): $(OBJS1)
	$(CXX) -o $(EXECUTABLE1) $(LDFLAGS) $(OBJS1)

#       Construct the executable
$(EXECUTABLE2): $(OBJS2)
	$(CXX) -o $(EXECUTABLE2) $(LDFLAGS) $(OBJS2)

#		Remove all junk that might be lying around
clean:
	-rm -f $(OBJS1) $(OBJS2) core* *.bak *~ $(ZIP_FILE)
	-rm -fr TEST_TURNIN

#		Also remove the previously built executables
realclean: clean
	-rm -f $(EXECUTABLE1) $(EXECUTABLE2)

$(ZIP_FILE): $(ALL_FILES)
	-rm -f $(ZIP_FILE)
	-zip -r $(ZIP_FILE) $(ALL_FILES)

test_turnin: $(ZIP_FILE)
	-rm -rf TEST_TURNIN
	@if [ $(LASTNAME) = 'YOUR LAST NAME GOES HERE' -o \
	      $(FIRSTNAME) = 'YOUR FIRST NAME GOES HERE' ]; \
          then echo 'ERROR: you must edit your Makefile to set LASTNAME/FIRSTNAME!'; \
	else \
	  (test -d TEST_TURNIN || mkdir TEST_TURNIN; cd TEST_TURNIN; \
           unzip ../$(ZIP_FILE); /bin/rm ../$(ZIP_FILE)); \
	  echo ' '; \
	  echo 'You just _tested_ turnin, you did _not_ turn anything in!!!!'; \
	  echo 'Please make sure that the contents of the TEST_TURNIN'; \
	  echo 'directory are exactly what you want to turn in!'; \
	  echo 'Then, use "make turnin" to actually turn in your assignment.'; \
	fi

turnin: $(ZIP_FILE)
	@if [ $(LASTNAME) = 'YOUR LAST NAME GOES HERE' -o \
	      $(FIRSTNAME) = 'YOUR FIRST NAME GOES HERE' -o \
	      $(RETURN_MAIL) = 'YOUR CEC EMAIL GOES HERE' ]; \
          then echo 'ERROR: you must edit your Makefile to set LASTNAME/FIRSTNAME/RETURN_EAIL!'; \
	else \
		cat TURNINMSG \
		| /usr/bin/mutt -s "$(PROJECT_TITLE): $(LASTNAME), $(FIRSTNAME)" -a $(ZIP_FILE) $(MAILTO); \
	fi

#       Rule to return the graded lab.
return: $(ZIP_FILE)
	head -n 100 README \
	| /usr/bin/mutt -s "$(PROJECT_TITLE): returned to $(LASTNAME), $(FIRSTNAME)" -a $(ZIP_FILE) $(RETURN_MAIL);

#		Preview the .h and .c files
preview:
	$(ENSCRIPT) -2Gr \
        -b$(LASTNAME)", "$(FIRSTNAME)" : $(LOGNAME)@cec.wustl.edu" \
        -p - $(ALL_FILES) | $(PREVIEWER) -

#		Change the Makefile to reflect the correct dependencies.
depend:
	-rm -f ccdep
	-rm -f eddep
	$(CXX) -M $(CCFLAGS) $(CMPL_SRCS1) $(CMPL_SRCS2) > ccdep
	sed -n '1,/^# DO NOT DELETE THIS LINE/p' Makefile > eddep
	echo \#\#\# >> eddep
	cat ccdep >> eddep
	cp Makefile Makefile.bak
	mv eddep Makefile
	rm ccdep

.SUFFIXES: .cpp .o
.cpp.o:
	$(CXX) $(CCFLAGS) -o $@ -c $<
#.cpp:
#	$(LINK.cc) $(LDFLAGS) -o $@ $< $(LDLIBS)

#main.o: $(ALL_SRCS) Makefile

###
# OBJECT FILE DEPENDENCIES FOLLOW.
#
# DO NOT DELETE THIS LINE -- make depend uses it
###
