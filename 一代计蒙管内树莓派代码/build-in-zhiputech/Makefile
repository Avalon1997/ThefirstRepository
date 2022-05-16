OBJDIR=${PWD}/obj
DLLDIR=${PWD}/bin
BINDIR=${PWD}/bin
LIBDIR=${PWD}/lib
LIBSQLITE=


COMM_INCLUDES=-I${PWD}/include


# Compiler Macro definitions
AR   = ar 
CXX  = g++
CC   = gcc 
LD   = g++
MAKE = make
GM   = gmake
YACC = yacc
LEX  = lex

#Compiler Flags definitions
ARFLAGS  	= -vr
CFLAGS   	= -g 
CXXFLAGS 	= -c -fpic -w -g  -std=c++11
LDCFLAGS 	= -G -z defs 
LDCXXFLAGS 	=-shared
CLDFLAGS 	= -g
CXXLDFLAGS 	= -g -G
BINLIB 		= 
THREAD_LIB 	= -lpthread
SOCKET_LIB 	= 
DYLOAD_LIB 	= 
STRIP=xscale_be-strip

#---------------------------- Macro definitions ------------------------------
SRCDIR=./src
SRC_LIST = \
	main.c \
	EasyCommon.cpp \
	serialCtrl.c \
	dispatchProcess.c \
	spectrumProcess.c \
	servoCtrl.c \
	temperatureCtrl.c \
	pingProcess.c \
	otherProcess.c \
	systemProcess.c \
	EasyError.cpp \
	EasyConfig.cpp \
	serial.cc \
	/impl/unix.cc \
	/impl/list_ports/list_ports_linux.cc 


OBJ_LIST = \
	$(OBJDIR)/main.o \
	$(OBJDIR)/EasyCommon.o \
	$(OBJDIR)/serialCtrl.o \
	$(OBJDIR)/dispatchProcess.o \
	$(OBJDIR)/spectrumProcess.o \
	$(OBJDIR)/servoCtrl.o \
	$(OBJDIR)/temperatureCtrl.o \
	$(OBJDIR)/pingProcess.o \
	$(OBJDIR)/otherProcess.o \
	$(OBJDIR)/systemProcess.o \
	$(OBJDIR)/EasyError.o \
	$(OBJDIR)/EasyConfig.o \
	$(OBJDIR)/serial.o \
	$(OBJDIR)/unix.o \
	$(OBJDIR)/list_ports_linux.o
	



COMM_LIB = -L${DLLDIR}  -lwiringPi -lavs
OUTBIN = ${BINDIR}/water-monitor-daq

#------------------------ Target file dependencies ---------------------------
all:\
	${OUTBIN}

${OUTBIN}: ${OBJ_LIST}  
	${CXX} -o $@ $(OBJ_LIST) ${CXXLIB} ${COMM_LIB} ${THREAD_LIB} ${DYLOAD_LIB}  ${SOCKET_LIB} -g
		


#------------------------ Include file dependencies --------------------------
depend:
	@(mkdepend   -p${OBJDIR}/ -o.o -Tlibp101 ${COMM_INCLUDES} ${SRC_LIST})>/dev/null 2>&1
	@rm -f makefile.bak

clean:
	rm -f ${OBJ_LIST}
	rm -f ${OUTBIN}

	
$(OBJDIR)/main.o: ${SRCDIR}/main.c
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?
	
$(OBJDIR)/EasyCommon.o: ${SRCDIR}/EasyCommon.cpp
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?
	
$(OBJDIR)/serialCtrl.o: ${SRCDIR}/serialCtrl.c
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?

$(OBJDIR)/dispatchProcess.o: ${SRCDIR}/dispatchProcess.c
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?
	
$(OBJDIR)/spectrumProcess.o: ${SRCDIR}/spectrumProcess.c
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?
	
$(OBJDIR)/servoCtrl.o: ${SRCDIR}/servoCtrl.c
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?
	
$(OBJDIR)/temperatureCtrl.o: ${SRCDIR}/temperatureCtrl.c
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?

$(OBJDIR)/pingProcess.o: ${SRCDIR}/pingProcess.c
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?
	
$(OBJDIR)/otherProcess.o: ${SRCDIR}/otherProcess.c
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?

$(OBJDIR)/systemProcess.o: ${SRCDIR}/systemProcess.c
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?

$(OBJDIR)/EasyError.o: ${SRCDIR}/EasyError.cpp
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?

$(OBJDIR)/EasyConfig.o: ${SRCDIR}/EasyConfig.cpp
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?

$(OBJDIR)/serial.o: ${SRCDIR}/serial.cc
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?
	
$(OBJDIR)/unix.o: ${SRCDIR}/impl/unix.cc
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?
	
$(OBJDIR)/list_ports_linux.o: ${SRCDIR}/impl/list_ports/list_ports_linux.cc
	${CXX} -c ${CXXFLAGS} ${COMM_INCLUDES} -o $@ $?

# DO NOT DELETE THIS LINE -- make depend depends on it.

