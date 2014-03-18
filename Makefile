.SUFFIXES: .cpp .hpp
# amol
# Programs
SHELL 	= bash
CC     	= g++
LD	= ld
RM 	= rm
ECHO	= /bin/echo
CAT	= cat
PRINTF	= printf
SED	= sed
DOXYGEN = doxygen
######################################
# Project Name (generate executable with this name)
TARGET = cs296_26_exe
TARGET2 = cs296_26_exelib

# Project Paths
PROJECT_ROOT=./
EXTERNAL_ROOT=$(PROJECT_ROOT)/external
SRCDIR = $(PROJECT_ROOT)/src
OBJDIR = $(PROJECT_ROOT)/myobjs
BINDIR = $(PROJECT_ROOT)/mybins
LIBDIR = $(PROJECT_ROOT)/mylibs
DOCDIR = $(PROJECT_ROOT)/doc
EXTER_SRC=$(EXTERNAL_ROOT)/src
REQ_1=./external/include/Box2D/Box2D.h
REQ_2=./external/lib/libBox2D.a

# Library Paths
BOX2D_ROOT=$(EXTERNAL_ROOT)
GLUI_ROOT=/usr
GL_ROOT=/usr/include/

#Libraries
LIBS = -lBox2D -lglui -lglut -lGLU -lGL
SHARED_LIB =true

# Compiler and Linker flags
CPPFLAGS =-g -O3 -Wall -fno-strict-aliasing
CPPFLAGS+=-I $(BOX2D_ROOT)/include -I $(GLUI_ROOT)/include
LDFLAGS+=-L $(BOX2D_ROOT)/lib/ -L $(GLUI_ROOT)/lib

######################################

NO_COLOR=\e[0m
OK_COLOR=\e[1;32m
ERR_COLOR=\e[1;31m
WARN_COLOR=\e[1;33m
MESG_COLOR=\e[1;34m
FILE_COLOR=\e[1;37m

OK_STRING="[OK]"
ERR_STRING="[ERRORS]"
WARN_STRING="[WARNINGS]"
OK_FMT="${OK_COLOR}%30s\n${NO_COLOR}"
ERR_FMT="${ERR_COLOR}%30s\n${NO_COLOR}"
WARN_FMT="${WARN_COLOR}%30s\n${NO_COLOR}"
######################################

SRCS := $(wildcard $(SRCDIR)/*.cpp)
INCS := $(wildcard $(SRCDIR)/*.hpp)
OBJS := $(SRCS:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
MYLIB_FILES := $(filter-out $(OBJDIR)/main.o,$(OBJS))

######################################
.PHONY: all setup doc clean distclean
all: setup exe exelib

setup:
	@$(ECHO) "Setting up compilation..."
	@mkdir -p myobjs
	@mkdir -p mybins
	@mkdir -p mylibs
	@echo "Checking If Box2D Is Installed"
	@if [[ -f $(REQ_1) && -f $(REQ_2) ]]; then \
	echo "BOX2D is already installed!!!!"; \
	else \
	echo "Installing Box 2D...."; \
	cd $(EXTER_SRC);\
	tar -xvzf Box2D.tgz; \
	cd Box2D; \
	mkdir -p build296; \
	cd build296; \
	cmake ../; \
	make; \
	make install; \
	fi 


exe: setup $(OBJS)
	@$(PRINTF) "$(MESG_COLOR)Building executable:$(NO_COLOR) $(FILE_COLOR) %16s$(NO_COLOR)" "$(notdir $(TARGET))"
	@$(CC) -o $(TARGET) $(LDFLAGS) $(OBJS) $(LIBS) 2> temp.log || touch temp.err
	@mv $(TARGET) ./mybins/$(TARGET)
	@if test -e temp.err; \
	then $(PRINTF) $(ERR_FMT) $(ERR_STRING) && $(CAT) temp.log; \
	elif test -s temp.log; \
	then $(PRINTF) $(WARN_FMT) $(WARN_STRING) && $(CAT) temp.log; \
	else $(PRINTF) $(OK_FMT) $(OK_STRING); \
	fi;
	@$(RM) -f temp.log temp.err

-include -include $(OBJS:.o=.d)

$(OBJS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	@$(PRINTF) "$(MESG_COLOR)Compiling: $(NO_COLOR) $(FILE_COLOR) %25s$(NO_COLOR)" "$(notdir $<)"
	@$(CC) -fPIC $(CPPFLAGS) -c $< -o $@ -MD 2> temp.log || touch temp.err
	@if test -e temp.err; \
	then $(PRINTF) $(ERR_FMT) $(ERR_STRING) && $(CAT) temp.log; \
	elif test -s temp.log; \
	then $(PRINTF) $(WARN_FMT) $(WARN_STRING) && $(CAT) temp.log; \
	else printf "${OK_COLOR}%30s\n${NO_COLOR}" "[OK]"; \
	fi;
	@$(RM) -f temp.log temp.err

MakeLibrary: setup $(OBJS)
	@if [ $(SHARED_LIB) == false ]; then \
	echo "creating static library";\
	ar -cvq $(LIBDIR)/libCS296test.a $(MYLIB_FILES);\
	else \
	echo "creating shared library";\
	$(CC) -shared -Wl,-soname,$(CURDIR)/mylibs/libCS296test.so -o $(CURDIR)/mylibs/libCS296test.so $(MYLIB_FILES) -lc; \
	fi;

exelib:	MakeLibrary
	@ if test "$(SHARED_LIB)" = "TRUE"; \
	then $(CC) -o ./mybins/cs296_26_exelib $(LDFLAGS) -L ./external/lib/ ./myobjs/main.o ./mylibs/libCS296test.so $(LIBS); \
	else $(CC) -o ./mybins/cs296_26_exelib $(LDFLAGS) -L ./mylibs/ -L ./external/lib/ ./myobjs/main.o -lCS296test $(LIBS); \
	fi;

clean:
	@$(ECHO) -n "Cleaning up..."
	@$(RM) -rf $(OBJDIR) $(LIBDIR) $(BINDIR) *~ $(DEPS) $(SRCDIR)/*~
	@$(ECHO) "Done"

distclean:
	@$(ECHO) -n "Cleaning up..."
	@$(RM) -rf $(OBJDIR) $(LIBDIR) $(BINDIR) *~ $(DEPS) $(SRCDIR)/*~
	@$(RM) -rf ./external/include/*
	@$(RM) -rf ./external/lib/*
	@$(RM) -rf ./external/src/Box2D	
	@$(ECHO) "Done"


