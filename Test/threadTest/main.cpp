 
#include <nrEngine/nrEngine.h>
#include <cmath>

using namespace nrEngine;

class Task : public ITask
{
	private:
		int n;
		int c;
	public:
		Task(int i) : ITask(), n(i), c(0)
		{
			char name[256];
			sprintf(name, "thread-%d", i);
			setTaskName(name);
		}

		Result updateTask()
		{
			c++;
			//float res = 1.0;
			if (n == 0){
				//for (int i=0; i < 1000000; i++)
				{
				//	res = cos(sin(c) * sin(c) * float(i) / 1000.0);
				}
				float startTime = Engine::sClock()->getTime();
				while (Engine::sClock()->getTime() < startTime + 2.0){}
				printf("0\n");
			}
			if (n == 1){
				float startTime = Engine::sClock()->getTime();
				while (Engine::sClock()->getTime() < startTime + 1.0){}
				printf("1\n");
			}
			
			printf("Thread: %d - %d\n", n, c);
			return OK;
		}
};


int main (int argc, char* argv[])
{
	Engine::sEngine()->initializeLog("./");
	Engine::sEngine()->initializeEngine();

	// create some task and add them as a thread
	for (int i=0; i < 3; i++)
	{
		SharedPtr<ITask> task (new Task(i));
		Engine::sKernel()->AddTask(task, ORDER_NORMAL, TASK_IS_THREAD);
	}
	SharedPtr<ITask> task (new Task(999));
	Engine::sKernel()->AddTask(task, ORDER_NORMAL, TASK_IS_THREAD);
	
	// execute the tasks
	float startTime = Engine::sClock()->getTime();
	while (Engine::sClock()->getTime() < startTime + 60.0)
	{
		Engine::sKernel()->OneTick();
	}
	
	// release used data
	Engine::release();

	return 0;
}
