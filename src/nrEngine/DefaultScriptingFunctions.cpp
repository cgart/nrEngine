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

namespace nrEngine{

	//------------------------------------------------------------------------
	// Helper class for default scripting functions
	//------------------------------------------------------------------------
	class DefaultScriptingFunctions
	{
		public:
			static void addMethods()
			{
				Engine::sScriptEngine()->add("set", set);
				Engine::sScriptEngine()->add("get", get);
				Engine::sScriptEngine()->add("list", list);
			}


			static void delMethods()
			{
				Engine::sScriptEngine()->del("set");
				Engine::sScriptEngine()->del("get");
				Engine::sScriptEngine()->del("list");
			}
		
		private:

			/**
			 * Set new value to specified property in the property database
			 **/
			ScriptFunctionDef(set);

			/**
			 * Get value of specified property
			 **/
			ScriptFunctionDef(get);

			/**
			 * List all properties in specified group
			 **/
			ScriptFunctionDef(list);
			
	};

	//----------------------------------------------------------------------------------
	ScriptFunctionDec(set, DefaultScriptingFunctions)
	{
		// check parameter count
		if (args.size() < 3){
			return ScriptResult(std::string("Not valid parameter count! Parameters (value, name, [group])"));
		}

		// get parameters
		std::string value, name, group;
		value = args[1];
		name  = args[2];
		if (args.size() > 3) group = args[3];

		// set new value to the property
		Engine::sPropertyManager()->set(Property(name, value), name, group);
		return ScriptResult();
	}
	
	//----------------------------------------------------------------------------------
	ScriptFunctionDec(get, DefaultScriptingFunctions)
	{
		// check parameter count
		if (args.size() < 1){
			return ScriptResult(std::string("Not valid parameter count! Parameters (name, [group])"));
		}

		// get parameters
		std::string name, group;
		name  = args[1];
		if (args.size() > 2) group = args[2];

		// set new value to the property
		Property& p = Engine::sPropertyManager()->getProperty(name, group);
		return ScriptResult(p.getValue());
	}

	//----------------------------------------------------------------------------------
	ScriptFunctionDec(list, DefaultScriptingFunctions)
	{
		// get parameters
		std::string group;
		if (args.size() > 1) group = args[1];

		// get list of all properties from the db
		const PropertyList& l = Engine::sPropertyManager()->getPropertyList(group);

		// iterate through the list
		ScriptResult res;
		for (PropertyList::const_iterator it = l.begin(); it != l.end(); it++){
			res , it->getValue();
		}
		
		// set new value to the property
		return res;
	}

};

