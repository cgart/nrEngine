TOPDIR= ../..

#-----------------------------------------------
# Include defs for defining the variables
#-----------------------------------------------
include $(TOPDIR)/Make/Makedefs

#-----------------------------------------------
# Create vpath directives
#-----------------------------------------------
#VPATH+= events ../include

#-----------------------------------------------
# We have to built this files into the library
#-----------------------------------------------
CPPFILES= Clock.cpp\
		Engine.cpp\
		Exception.cpp\
		Event.cpp\
		EventFactory.cpp\
		EventManager.cpp\
		EventActor.cpp\
		EventChannel.cpp\
		FileStream.cpp\
		FileStreamLoader.cpp\
		GetTime.cpp\
		IFileSystem.cpp\
		IScript.cpp\
		IStream.cpp\
		ITask.cpp\
		IThread.cpp\
		Kernel.cpp\
		Log.cpp\
		Plugin.cpp\
		PluginLoader.cpp\
		Profiler.cpp\
		Property.cpp\
		PropertyManager.cpp\
		Resource.cpp\
		ResourceHolder.cpp\
		ResourceLoader.cpp\
		ResourceManager.cpp\
		ResourcePtr.cpp\
		ScriptConnector.cpp\
		Script.cpp\
		ScriptEngine.cpp\
		ScriptLoader.cpp\
		StdHelpers.cpp\
		Timer.cpp\
		TimeSource.cpp\
		TimeSourceVirtual.cpp\
		VariadicArgument.cpp\
		KernelEvent.cpp

# define used vairables
TARGET = $(TOPDIR)/lib/libnrEngine.so
INCPATH += -I$(TOPDIR)/include
CFLAGS += -fPIC
LIBS += -ldl -lboost_thread

# define files for installation
INSTALL_DST_LIB = $(INST_LOCATION_LIB)
INSTALL_SRC_LIB = $(TARGET)

#-----------------------------------------------
# Include rules for handling the objects
#-----------------------------------------------
include $(TOPDIR)/Make/Makerules
sinclude make.dep
