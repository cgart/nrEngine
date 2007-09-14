/***************************************************************************
 *                                                                         *
 *   (c) Art Tevs, MPI Informatik Saarbruecken                             *
 *       mailto: <tevs@mpi-sb.mpg.de>                                      *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/


#ifndef _NR_SCRIPT_INTERFACE_RESOURCE__H_
#define _NR_SCRIPT_INTERFACE_RESOURCE__H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Resource.h"
#include "ITask.h"

/*!
 * \defgroup script Engine Scripting
 *
 * Scripting is our form of data driven design of our engine. Scripts
 * are also resources that are loaded through specified loader. Scripts are
 * used to program the engine from the outside of the source code. You are
 * able to program the engine in the runtime by executing the scripts through
 * a console provided by the engine. To understand what the scripts are good for
 * use appropriate literature from the internet.
 *
 * Our engine is designed to be very expandable and generic, so we are not use
 * specific kind of script language. nrEngine does only provide an interface
 * for a script classes that are implemented by plugins. The main idea of such
 * an interface is to create a binding between the script language and the engine.
 *
 * The plugins classes have to know how to bind the specific script language with
 * the script interface of the engine. The console will acquire a script object to
 * be used for the console scripting. This script object is provided by the plugins
 * so it uses that language which is implemented in the plugin.
 *
 * If there no plugins providing a scripting language so the default one is used.
 * The default scripting language does not support anything except of some function calls.
 * This very simple languge can be used to load another script language or
 * to setup some properties of the engine.
 *
 * Because there is thousands of scripting languages outside and they can be bounded
 * on different ways we do not provide any bounding routine. If you using any scripting
 * language in your application, than you have to bind your code with the script code by your
 * self. Mostly this is done through metaprogramming or through preproccessing
 * routines on the scripting language. Therefor providing such an interface through
 * plugins for the engine seems not to be managable. Therefor all default functions used
 * in our engine are registered by the script engine. By adding or deleting any
 * script function there will be an event. Your glue-code has to catch this event to
 * take care off registering or derigistering this function in your scripting language.
 *
 * For example you want to use lua in your application. You use some 3rd party tools
 * like tolua which does generate a c++ code form pkg files. This newly generated files
 * can be used to provide your c++ classes in lua. Now to provide the engine's scripting
 * functions, which are registered by the ScriptEngine you have to write a wrapper which will
 * call ScriptEngine::call() method if appropriate function was called from lua.
 *
 * Probalby in later version we can provide some binding to you (loadtime plugins), which
 * does provide this functionality. But at now there is no other way to do this.
 **/

namespace nrEngine{


	//! Script interface for any kind of script languages
	/**
	* This is an interface for any kind of scripting language provided by the plugins.
	* This interface is used to executed strings typed in the console or readed from
	* the scripting files.
	*
	* We also define each script as an task in the kernel. So the script will be
	* updated in each tick by the kernel and will be unloaded from the task queue as soon
	* as the script ends his execution. Using of scripts as a task has the advantage, that
	* we are able to run scripts with a certain priority/order number. So the script_Task can
	* run before certain application task or after them.
	*
	* Each script can not only be executed, but if specify any functions or routines,
	* so this routines could be explicitely called. You can also specify arguments
	* for such a routine. Therefor the IScript interface does only store
	* the arguments and their type as a string. Derived class should read this
	* and pass the parameters in the right way. If a derived script language
 	* does not support any arguments or function calling so just ignore them.
	* Passed parameters will be automaticaly removed from the waiting list as
	* soon as the function from the script will be called.
	*
	* @see IResource, ITask
	* \ingroup script
	**/
 	class _NRExport IScript : public IResource, public ITask{
		public:
			//! You can also push a list of arguments to the function directly
			typedef std::list<std::pair<std::string, std::string> > ArgumentList;

			/**
			 * Returns the content of the script file
			 **/
			NR_FORCEINLINE const std::string& getContent() const { return mContent; }

			/**
			* Load a script from string. The given string should contain
			* the script in the appropriate language, so it can be executed.
			*
			* @param str String containing the script
			* @return either OK or:
			* 		- SCRIPT_PARSE_ERROR
			**/
			virtual Result loadFromString(const std::string& str) = 0;

			/**
			* Get short description of the last error.
			**/
			std::string popLastError();

			/**
			 * Execute the script. The script will be executed completely, before
			 * go back to the system run. Execution is done by adding the script to
			 * the kernel's pipeline. In the next cycle the script will be then executed.
			 *
			 * @param runOnce If true, so the script method run() will be called only
			 * 			once. If you specify false, so run() will be called in each
			 * 			kernel cycle.
			 *
			 * NOTE: If you want to execute script immidiately, use forceExecute() instead
			 **/
			Result execute(bool runOnce = true);

			/**
			 * Force starting the script. Script will be added to the kernel
			 * and started immeiately after that, by calling the run() method.
			 * Afer that the script will be updated in each cycle if not otherwise
			 * specified.
			 *
			 * @see execute()
			 **/
			Result forceExecute(bool runOnce = true);

			/**
			 * By calling the execute() method you will execute the whole script completely.
			 * This is usefull if you do not interested in the results of the scripts, but
			 * in its handling. However if you wish to call a certain function from the script
			 * (only if function calling is supported by the scripting language), so you
			 * have to use this method.
			 *
			 * Here a certain function from the script will be called. The result of this function
			 * will be given back in a generic type ScriptResult. You have to know how
			 * to interpret the results given back by the script. If any error occurs the
			 * error message will be logged and an exception will be thrown. Catch
			 * the exception to handle the errors.
			 *
			 * @param funcName Name of the function to call
			 *
			 * @return result from the script execution converted into generic
			 * ScriptResult object.
			 *
			 * NOTE: To pass parameters to function use pushArgument()
			 **/
			ScriptResult call(const std::string& funcName);

			/**
			 * User application can also specify certain arguments for functions
			 * in the script. The derived class implementing specific script
			 * language must read out this values before starting the script.
			 * The derived class should also know how to pass them to the script
			 * language and how to parse them. If a script does not support function
			 * calling or argument binding, so this values can be ignored.
			 *
			 * @param funcName Name of the function/routine in the script for which one
			 * 					the parameters are specified
			 * @param type String containing the name of the type of the argument
			 * @param value String containing the value of the argument.
			 *
			 * @return Number of arguments currently stored for the given function including
			 * 			the new argument.
			 *
			 * NOTE: Push arguments from the first to the last.
			 **/
			uint32 pushArgument(const std::string& funcName, const std::string& type, const std::string& value);

			/**
			 * Same as pushArgument() but set argument from the list. Already pushed
			 * arguments will be removed !
			 **/
			uint32 setArguments(const std::string& funcName, const ArgumentList& args);

			/**
			 * Derived class should use this function to pop the first parameter
			 * from the list. The argument type and the value will be stored in
			 * the given variables. The function does return the number of
			 * remaining parameters. If function returns 0, so no parameters are
			 * available.
			 *
			 * @param funcName Name of the function for which the arguments are retrieved
			 * @param type Here the type of the argument will be stored
			 * @param value Here the value of the argument will be stored
			 *
			 * @return Number of remaining arguments for the function. If the resulting
			 * 			value is -1, so no parameters for the function exists
			 *
			 * NOTE: The arguments are returned from the first to the last
			 **/
			int32 popArgument(const std::string& funcName, std::string& type, std::string& value);

		protected:

			/**
			 * Run the script. This method must be overriden by the derived classes.
			 * run will be called by the execution of the script. However you can not
			 * be sure if the script executed completely or only stepwise by this method,
			 * this depends only on the implementation. Read appropriate documentation of
			 * script language implementation.
			 **/
			virtual Result run() = 0;

			/**
			 * Derived class can overload this method if a implemented script language
			 * does support function calling. Default behaviour of this method
			 * is not to call any function or to execute a script. Overload
			 * this if you wish to have another behaviour.
			 *
			 * @param funcName name of the function to call
			 * @param result Store here the result of a call of a function
			 * @return OK if no error during the execution occurs or an error code.
			 * 			If any error code will be returned so an exception will
			 * 			be thrown.
			 **/
			virtual Result callfunc(const std::string& funcName, ScriptResult& result);

			/**
			 * If any error occurs stor here the error messages.
			 * The base class will deliver them to the user. Also the base
			 * class does implement error message stack, so returned messages are
			 * automaticaly removed from the message stack.
			 **/
			void pushErrorMessage(const std::string& msg);

			/**
			 * Initialize the script interface
			 * Specify also the script type name, so we know of which
			 * type is the given script
			 **/
			IScript(const std::string&);

			/**
			 * Virtual destructor
			 **/
			virtual ~IScript();

			/**
			 * Run one step from the script. This method will be automaticaly
			 * called from the engine's kernel.
			 **/
			Result updateTask();

			/**
			 * Method is called from kernel, when script starts its execution
			 **/
			Result onStartTask();

			//! Here we store the whole script as a string
			std::string mContent;

			/**
			 * Bypass the IResource::unloadRes() function, so that derived
			 * class can overload them
			 **/
			virtual Result unloadResource() {return OK;}
			virtual Result reloadResource(PropertyList* params) {return OK;}

			//! Check if script should only run once
			bool shouldRunOnce() const { return mbRunOnce; }

			//! Should the script run only once (no loop)
			void setRunOnce(bool b) { mbRunOnce = b; }

			/**
			 * This method will be called to check from base class to
			 * check if run completes. If derived class returns false,
			 * so script will be removed from the kernel
			 **/
			virtual bool hasCommands() = 0;

			/**
			 * This method will be called if a script is gonna be executed.
			 * Override this method to make some initializations for example.
			 **/
			virtual void onStartScript() {};

			/**
			 * Call this method to manually specify, that a script is
			 * loaded and can be executed. The method should be called
			 * from loadFromString() method by derived classes
			 **/
			void notifyLoaded();

		private:

			//! Should this script run once
			bool mbRunOnce;

			//! Store here error messages
			std::list<std::string> mErrorMsg;

			typedef std::map<std::string, ArgumentList>		ArgumentDatabase;

			//! Here we store the arguments for the script
			ArgumentDatabase mArguments;

	};


	//! Empty script object that just do nothing(idle).
	/**
	 * Empty script objects do nothing. They idle.
	 * \ingroup script
	 **/
	class _NRExport EmptyScript : public IScript{
	public:

		//! Constructor does not call anything
		EmptyScript(const std::string& typeName);

		//! Release used memory
		~EmptyScript();

		//! Loading an empty script from file does not affect anything
		Result loadFromString(const std::string& str);

		//! Executing of empty script does return immideately
		Result run();
	private:
		bool hasCommands() { return false; }
	};

};

#endif
