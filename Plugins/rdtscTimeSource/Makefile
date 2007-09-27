TOPDIR= ../..

#-----------------------------------------------
# Include defs for defining the variables
#-----------------------------------------------
include $(TOPDIR)/Make/Makedefs

#-----------------------------------------------
# We have to built this files into the library
#-----------------------------------------------
CPPFILES = Plugin.cpp\
	TimeSource.cpp\
	Cpu.cpp
			
# some definitions
TARGET = rdtscTimeSource.so
INCPATH += -I$(TOPDIR)/include
CFLAGS +=
LIBS = 
LDFLAGS = $(LIBPATH) -shared 
#-L/usr/local/gcc-4.1.1-static/lib -Wl,-Bstatic -lstdc++ -lgcc -lm -Wl,-Bdynamic -llua

# installation definitions
INSTALL_DST_LIB = $(INST_LOCATION_LIB)/nrEngine-$(VERSION)
INSTALL_SRC_LIB = $(TARGET)
UNINSTALL_DIR = $(INST_LOCATION_LIB)/nrEngine-$(VERSION)

#-----------------------------------------------
# Include rules for handling the objects
#-----------------------------------------------
include $(TOPDIR)/Make/Makerules
sinclude make.dep
