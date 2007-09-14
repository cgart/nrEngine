
#include <nrEngine/nrEngine.h>

using namespace nrEngine;

class A
{
	public:

		ScriptFunctionDef(test1);
		ScriptFunctionDef(test2);
		ScriptFunctionDef(test3);
		ScriptFunctionDef(test4);
		ScriptFunctionDef(test5);
		ScriptFunctionDef(test6);

};

ScriptFunctionDec(test1, A)
{
	printf("1\n");
	return ScriptResult();
}

ScriptFunctionDec(test2, A)
{
	printf("2\n");
	return ScriptResult();
}

ScriptFunctionDec(test3, A)
{
	printf("3\n");
	return ScriptResult();
}

ScriptFunctionDec(test4, A)
{
	printf("4\n");
	return ScriptResult();
}
ScriptFunctionDec(test5, A)
{
	printf("5\n");
	return ScriptResult();
}
ScriptFunctionDec(test6, A)
{
	printf("6\n");
	return ScriptResult();
}

int main (int argc, char* argv[])
{
	// init
	Engine::sEngine()->initializeLog("./");
	Engine::sEngine()->initializeEngine();
	Engine::sLog()->setLevel(Log::LL_CHATTY);

	// Initialize script functions
	Engine::sScriptEngine()->add("test1", A::test1);
	Engine::sScriptEngine()->add("test2", A::test2);
	Engine::sScriptEngine()->add("test3", A::test3);
	Engine::sScriptEngine()->add("test4", A::test4);
	Engine::sScriptEngine()->add("test5", A::test5);
	Engine::sScriptEngine()->add("test6", A::test6);

	// load script
	ResourcePtr<IScript> scr = Engine::sScriptEngine()->load("Test", "test.nrscript");
	scr->execute();

	// execute the tasks
	for (int i=0; i < 100; i++)
	{
		Engine::sKernel()->OneTick();
	}

	// release used data
	Engine::release();

	return 0;
}
