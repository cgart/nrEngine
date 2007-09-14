TOPDIR= ../..

#-----------------------------------------------
# Include defs for defining the variables
#-----------------------------------------------
include $(TOPDIR)/Make/Makedefs

#-----------------------------------------------
# We have to built this files into the library
#-----------------------------------------------
CPPFILES= Package.cpp\
	Task.cpp
			
INCFILES = Package.h\
		Event.h\
		glfw.h
		
# create the target
TARGET=  $(TOPDIR)/lib/libnrEngineGLFW.so
INCPATH += -I./ -I$(TOPDIR)/include
CFLAGS += -fPIC
LDFLAGS = -shared -Llib -Wl,-Bstatic -lglfw -Wl,-Bdynamic

# define files for installation
INSTALL_DST_LIB = $(INST_LOCATION_LIB)
INSTALL_SRC_LIB = $(TARGET)
		
INSTALL_DST_INC = $(INST_LOCATION_INC)/nrEngine/Package/glfw
INSTALL_SRC_INC = $(INCFILES)

UNINSTALL_DIR = $(INST_LOCATION_INC)/nrEngine/Package/glfw $(INST_LOCATION_INC)/nrEngine/Package

#-----------------------------------------------
# Include rules for handling the objects
#-----------------------------------------------
include $(TOPDIR)/Make/Makerules
sinclude make.dep
