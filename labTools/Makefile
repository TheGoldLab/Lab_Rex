# Makefile for MNS useful libraries labTools.lib

ifndef DEBUG
DEBUG = -g #-DLAB_DEBUG
endif

ifndef OPTIM
OPTIM = -O
endif

PLATFORM=gcc_ntox86

CC = qcc
LD = qcc

CFLAGS  = -V$(PLATFORM) -w -c $(OPTIM) $(DEBUG) # -I../hdr

# directories
L = ../lib
H = ../hdr

# this is what we're making
OUTPUT = liblt.a

# ADD NEW OBJECTS HERE -- INCLUDING NEW TASKS
OBJ = dioLT.o dotsX.o ecodeLT.o paradigm_rec.o \
	property_list.o random.o safe_alloc.o task.o \
	task_utilities.o timerLT.o toys.o udp.o \
	Tasks/task_asl.o		\
	Tasks/task_fix.o		\
	Tasks/task_ft.o			\
	Tasks/task_dots.o		\
	Tasks/task_dotRT_ARdt.o		\
	Tasks/task_dotsreg_LD.o		\
	Tasks/task_dotRT_LD20070201.o	\
	Tasks/task_mgs_LD20070116.o	\
	Tasks/task_vgs_LD20070201.o	\

# typing 'make' makes the library
all:	$L/$(OUTPUT)

# the library depends on the list of objects
# make the library out of the objects
$L/$(OUTPUT):	$(OBJ)
	$(LD) -V$(PLATFORM) $(OBJ) -A $L/$(OUTPUT)

clean:
	rm *.o
	rm Tasks/*.o

dioLT.o: dioLT.h ../hdr/pcmsg.h ../sset/ldev.h

dotsX.o: dotsX.h toys.h udp.h

ecodeLT.o: ecodeLT.h toys.h ../hdr/ecode.h

paradgim_rec.o: paradgim_rec.h dotsX.h property_list.h toys.h

property_list.o: property_list.h toys.h

random.o: random.h

safe_alloc.o: safe_alloc.h

task.o: task.h paradigm_rec.h property_list.h toys.h

task_utilities.o: task_utilities.h paradigm_rec.h property_list.h

timerLT.o: timerLT.h

toys.o: toys.h safe_alloc.h

udp.o: udp.c udp.h
	gcc -c udp.c $(DEBUG) -o udp.o
#	gcc udp.o ../../../lib/libsocket.a ../../../lib/libc.a -o udp

# TASK-SPECIFIC RULES GO BELOW
Tasks/task_asl.o: paradigm_rec.h task.h task_utilities.h

Tasks/task_fix.o: paradigm_rec.h task.h task_utilities.h

Tasks/task_ft.o: paradigm_rec.h task.h task_utilities.h

Tasks/task_dots.o: paradigm_rec.h task.h task_utilities.h

Tasks/task_dotRT_ARdt.o	: paradigm_rec.h task.h task_utilities.h

Tasks/task_dotsreg_LD.o	: paradigm_rec.h task.h task_utilities.h

Tasks/task_dotRT_LD20070201.o : paradigm_rec.h task.h task_utilities.h

Tasks/task_mgs_LD20070116.o : paradigm_rec.h task.h task_utilities.h

Tasks/task_vgs_LD20070201.o : paradigm_rec.h task.h task_utilities.h
