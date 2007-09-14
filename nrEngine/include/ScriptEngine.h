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


#ifndef _NR_SCRIPT_ENGINE_H_
#define _NR_SCRIPT_ENGINE_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"

//! Declare a scripting function as static memeber of class (c++ only)  \ingroup script
#define ScriptFunctionDef(name)\
 	public: static nrEngine::ScriptResult name (const std::vector<std::string>&, const std::vector<nrEngine::ScriptParam>&)

//! Declare a scripting function out of the class class (c++ only)  \ingroup script
#define ScriptFunctionDec(name, class)\
 	nrEngine::ScriptResult class::name (const std::vector<std::string>& args, const std::vector<nrEngine::ScriptParam>& param)

namespace nrEngine{


	//! Each script function can return variable argument as a result \ingroup script
	typedef VarArg ScriptResult;

	//! The real functions accept this default parameter \ingroup script
	typedef boost::any ScriptParam;

	//! We define each function that can be called from the scripts as this type \ingroup script
	typedef boost::function<ScriptResult (const std::vector<std::string>&, const std::vector<ScriptParam>&)> ScriptFunctor;

	//! Script engine is a glue code between the scripts and the engine
	/**
	 * The script engine is a main class representing the glue code between
	 * the scripting languages (from plugins or whatever) and the game engine.
	 * The game engine will register functions by the script engine, so
	 * you get an access to this functions in your scripts. So the script
	 * engine class is something like an engine's API for the scripts.
	 *
	 * In the future we want to improve the functionality of the scripting
	 * engine, by adding the possibility to use meta classes. So the scripts
	 * could create new classes based on engine's.
	 *
	 * \ingroup script
	**/
	class _NRExport ScriptEngine{
		public:

			/**
			 * Cast a script parameter, which is of type "any", to any other type.
			 * Note: we use here any_cast, so you have to worry about catching the
			 * error throws
			 **/
			template<class T>
			static T parameter_cast(const ScriptParam& p){
				return boost::any_cast<T>(p);
			}

			/**
			* Register new functions, that can be called from scripts.
			* The functions registered here are called through call() method.
			*
			* You can also specify one default parameter, that will be passed
			* to the called function (e.g. pointer to your object). The parameter is
			* of the boost type "any". So it does not have certain type.
			*
			* @param name Name of the function, must be unique
			* @param func Function itself (i.e. boost::function)
			* @param param Parameters you want to be stored to pass later to the function
			*
			* @return either OK or an error code if there is some problems
			**/
			Result add(const std::string& name, ScriptFunctor func, const std::vector<ScriptParam>& param = std::vector<ScriptParam>());

			/**
			 * Using variable parameter arguments.
			 * @see add()
			 **/
			Result add(const std::string& name, ScriptFunctor func, const VarArg& v);

			/**
			* Delete already registered function from the api database.
			*
			* @param name name of the function to be deregistered
			*
			* @return either OK or an error code
			**/
			Result del(const std::string& name);

			/**
			* Call a certain function from the database whithin the given parameters.
			* First script engine database will look if there is a function
			* whithin the given name. Then if such could be found the functor
			* according to the function will be called. As first parameter it sends
			* the user predefined parameters, stored in the database. As second
			* it will pass the string parameters coming from the script or from
			* the subroutine called this method.
			*
			* @param name Unique name of hte function to be called
			* @param args Argument list of arguments given to the function (like in
			* 				the console, first must always be the function name)
			**/
			ScriptResult call(const std::string& name, const std::vector<std::string>& args = std::vector<std::string>());

			/**
			 * Load a new script from the given file. The script will be loaded
			 * by the resource manager, which will try to find the appropriate
			 * loader for this type of scripts. If there is no such loader could
			 * be found, so null will be returned. Otherwise you get a resource
			 * pointer to the script. The resource pointer garanty that the according
			 * script will be used in the way like all other resources. This means if
			 * a script is unloaded,so EmptyScript will be used. You do not have
			 * to care about given pointers, because resource pointers have similar
			 * behaviours as smart pointers.
			 *
			 * The function does load the scripts in the memory, so you do not have
			 * to carry about the returned pointer, if you do not want to. Loaded scripts
			 * are still accessable by their names.
			 * 
			 * @param name Name for the script.
			 * @param fileName Name of the file containign the script.
			 *
			 * @return ResourcePtr<IScript> to the script. 
			 **/
			ResourcePtr<IScript> load(const std::string& name, const std::string& fileName);

			/**
			 * This method will execute script of a certain name loaded before
			 * with loadScript() method.
			 *
			 * @param name Unique name for the script
			 * @param runOnce If true the script will be executed only one time
			 * @param immediate If true the script will be run immediately. If false
			 * the script will be added as a task into the kernel, and will be started
			 * in the next execution cycle.
			 *
			 * @return Returning code of IScript::execute() method.
			 **/
			Result execute(const std::string& name, bool runOnce = true, bool immediate = false);

			/**
			 * Combined method of load() and execute(). See documentation for them
			 * instead
			 **/
			Result execute(const std::string& name, const std::string& fileName, bool runOnce = true, bool immediate = false);

			/**
			 * Get count of functions registered in the database
			 **/
			NR_FORCEINLINE uint32 getFunctionCount() const { return mDatabase.size(); }

			/**
			 * Return an functor element of a certain index from the database.
			 * Any functor element is a pair of the functor and script parameters
			 * defined by the user.
			 **/
			const std::string& getFunction(uint32 index, ScriptFunctor& functor);
					
		private:

			//! Run not by name, but by pointer
			Result _execute(const ResourcePtr<IScript>& ptr, bool runOnce = true, bool immediate = false);

			//! Only engine is allowed to create the instances
			friend class Engine;

			//! Allocate memory and define all default values
			ScriptEngine();

			//! Release used memory and close the scripting engine
			~ScriptEngine();

			//! This is our database holding the registered functions
			typedef std::map<std::string, std::pair<ScriptFunctor, std::vector<ScriptParam> > > FunctionDatabase;

			//! Here we store our registered functions
			FunctionDatabase mDatabase;

			//! Check whenever a function with the given name is already in the database
			bool isRegistered(const std::string& name);

			//! Get  the function according to the given name
			FunctionDatabase::iterator get(const std::string& name);


			ScriptFunctionDef(scriptLoad);
			ScriptFunctionDef(scriptRun);
			ScriptFunctionDef(scriptLoadAndRun);
			ScriptFunctionDef(scriptCall);
			
	};


};

#endif
