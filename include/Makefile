TOPDIR= ../..

#-----------------------------------------------
# Include defs for defining the variables
#-----------------------------------------------
include $(TOPDIR)/Make/Makedefs

#-----------------------------------------------
# We have to built this files into the library
#-----------------------------------------------
INCFILES=   Kernel.h\
			Engine.h\
			nrEngine.h\
			Log.h\
			Exception.h\
			ITask.h\
			KeySym.h\
			StdHeaders.h\
			Prerequisities.h\
			Property.h\
			PropertyManager.h\
			Platform.h\
			Priority.h\
			Clock.h\
			TimeSource.h\
			TimeSourceVirtual.h\
			Timer.h\
			ITimeObserver.h\
			StdHelpers.h\
			Result.h\
			Profiler.h\
			ResourceHolder.h\
			ResourceManager.h\
			ResourcePtr.h\
			ResourceLoader.h\
			ResourceSystem.h\
			Resource.h\
			Plugin.h\
			PluginLoader.h\
			IStream.h\
			ScriptEngine.h\
			ScriptLoader.h\
			Script.h\
			ScriptConnector.h\
			IScript.h\
			VariadicArgument.h\
			EventManager.h\
			EventChannel.h\
			EventActor.h\
			Event.h\
			EventFactory.h\
			SmartPtr.h\
			Package.h\
			GetTime.h\
			IThread.h\
			IStream.h\
			FileStream.h\
			FileStreamLoader.h\
			FileSystemManager.h\
			IFileSystem.h
		
# define files for installation
INSTALL_DST_INC = $(INST_LOCATION_INC)/nrEngine
INSTALL_SRC_INC = $(INCFILES)
UNINSTALL_DIR = $(INST_LOCATION_INC)/nrEngine

#-----------------------------------------------
# Include rules for handling the objects
#-----------------------------------------------
include $(TOPDIR)/Make/Makerules
