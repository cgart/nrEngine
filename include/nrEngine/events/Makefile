TOPDIR= ../../..
		
#-----------------------------------------------
# Include defs for defining the variables
#-----------------------------------------------
include $(TOPDIR)/Make/Makedefs

#-----------------------------------------------
# We have to built this files into the library
#-----------------------------------------------
INCFILES=\
	EngineEvent.h\
	KernelEvent.h\
	KernelTaskEvent.h
		
# define files for installation
INSTALL_DST_INC = $(INST_LOCATION_INC)/nrEngine/events
INSTALL_SRC_INC = $(INCFILES)
UNINSTALL_DIR = $(INST_LOCATION_INC)/nrEngine/events

#-----------------------------------------------
# Include rules for handling the objects
#-----------------------------------------------
include $(TOPDIR)/Make/Makerules
