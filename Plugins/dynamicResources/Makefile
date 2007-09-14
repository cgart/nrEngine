TOPDIR= ../..

#-----------------------------------------------
# Include defs for defining the variables
#-----------------------------------------------
include $(TOPDIR)/Make/Makedefs

#-----------------------------------------------
# We have to built this files into the library
#-----------------------------------------------
CPPFILES = Plugin.cpp\
	  Monitor.cpp

# if we are on linux system, then add this
ifeq ($(OS), Linux)
	CPPFILES += inotify-cxx.cpp
endif

			
# some definitions
TARGET = dynamicResources.so
INCPATH += -I../include
CFLAGS += -fPIC
LIBS = 
#LDFLAGS = $(LIBPATH) -shared -llua
#-L/usr/local/gcc-4.1.1-static/lib -Wl,-Bstatic -lstdc++ -lgcc -lm -Wl,-Bdynamic -llua

# installation definitions
INSTALL_DST_LIB = $(INST_LOCATION_LIB)/nrEngine
INSTALL_SRC_LIB = $(TARGET)
UNINSTALL_DIR = $(INST_LOCATION_LIB)/nrEngine

#-----------------------------------------------
# Include rules for handling the objects
#-----------------------------------------------
include $(TOPDIR)/Make/Makerules
sinclude make.dep
