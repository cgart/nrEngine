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


#include <nrEngine/Kernel.h>
#include <nrEngine/Profiler.h>
#include <nrEngine/events/KernelTaskEvent.h>
#include <nrEngine/EventManager.h>
#include <nrEngine/StdHelpers.h>
#include <nrEngine/Log.h>

namespace nrEngine {

	std::list< SharedPtr<ITask> >::iterator Kernel::_loopIterator;

	//-------------------------------------------------------------------------
	Kernel::Kernel(){
		taskList.clear();
		pausedTaskList.clear();
		lastTaskID = 0;
		bTaskStarted = false;
		bInitializedRoot = false;
		_bSystemTasksAccessable = false;
		sendEvents(true);
	}

	//-------------------------------------------------------------------------
	Kernel::~Kernel(){
		StopExecution();

		taskList.clear();
		pausedTaskList.clear();

		// Log that kernel is down
		NR_Log(Log::LOG_KERNEL, "Kernel subsystem is down");
	}

	//-------------------------------------------------------------------------
	void Kernel::lockSystemTasks()
	{
		_bSystemTasksAccessable = true;
	}

	//-------------------------------------------------------------------------
	void Kernel::unlockSystemTasks()
	{
		_bSystemTasksAccessable = false;
	}

	//-------------------------------------------------------------------------
	Result Kernel::OneTick()
	{

		// Profiling of the engine
		_nrEngineProfile("Kernel::OneTick");

		// start tasks if their are not started before
		prepareRootTask();

		// get iterator through our std::list
		PipelineIterator it;

		// iterate through all tasks and start them
		for (it = taskList.begin(); it != taskList.end(); it++){
			if ((*it)->getTaskState() == TASK_STOPPED)
				_taskStart(*it);
		}

		// resetup to the beginning
		it = taskList.begin();

		// we go always from teh first element in the list, because it is
		// the system root task
		_loopStartCycle();
		while (_loopGetNextTask(taskList.begin(), it, 0) == OK){
			SharedPtr<ITask>& t=(*it);

			if (t && !t->_taskCanKill){

				// if the task is running
				if (t->getTaskState() == TASK_RUNNING){

					// do some profiling
					char name[256];
					sprintf(name, "%s::update", t->getTaskName());
					_nrEngineProfile(name);

					t->updateTask();

					// check if the task should run only once
					if (t->getTaskProperty() & TASK_RUN_ONCE)
						RemoveTask(t->getTaskID());

				}
			}
		}
		_loopEndCycle();

		PipelineIterator thisIt;
		TaskId tempID;

		//loop again to remove dead tasks
		for( it = taskList.begin(); it != taskList.end();){

			SharedPtr<ITask> &t=(*it);
			bool killed = false;

			// kill task if we need this
			if(t && t->_taskCanKill){

				_taskStop(t);

				// remove the task
				tempID = t->getTaskID();
				NR_Log(Log::LOG_KERNEL, "Task (id=%d) removed", tempID);
				it = taskList.erase(it);
				killed = true;

			// check whenver order of the task was changed by outside
			}else if (t->_orderChanged){
				ChangeTaskOrder(t->getTaskID(), t->getTaskOrder());
			}

			if (!killed) it++;
		}

		// Now we yield the running thread, so that our system could still 
		// response
		IThread::yield();

		// return good result
		return OK;
	}


	//-------------------------------------------------------------------------
	void Kernel::prepareRootTask()
	{
		if (bInitializedRoot) return;
		bInitializedRoot = true;

		// get iterator through our std::list
		PipelineIterator it;

		NR_Log(Log::LOG_KERNEL, "Create a system root task in the kernel");

		// firstly we add a dummy system task, which depends on all others.
		// This task do nothing, but depends on the rest.
		// This is needed to get all thing worked
		mRootTask.reset(new EmptyTask());
		mRootTask->setTaskName("KernelRoot");
		mRootTask->setTaskType(TASK_SYSTEM);

		// Add the task as system task to the kernel
		lockSystemTasks();
			AddTask(mRootTask, ORDER_SYS_ROOT);
		unlockSystemTasks();

	}

	//-------------------------------------------------------------------------
	/*void Kernel::startTasks(){

		NR_Log(Log::LOG_KERNEL, Log::LL_DEBUG, "Start all kernel tasks");

		// start all tasks, which are not running at now
		for(it = taskList.begin(); it != taskList.end(); it++){
			_taskStart(*it);
		}

		bTaskStarted = true;
	}
	*/

	//-------------------------------------------------------------------------
	Result Kernel::_taskStart(SharedPtr<ITask>& task)
	{

		// no task given, so return error
		if (!task)
			return KERNEL_NO_TASK_FOUND;

		// check if the task is not a thread
		if (task->getTaskProperty() & TASK_IS_THREAD){

			SharedPtr<IThread> thread = boost::dynamic_pointer_cast<IThread, ITask>(task);
			thread->threadStart();
			task->setTaskState(TASK_RUNNING);
			task->onStartTask();

		}else{

			// check for the task states
			if (task->getTaskState() == TASK_STOPPED)
			{
				task->setTaskState(TASK_RUNNING);
				task->onStartTask();
			}else if (task->getTaskState() == TASK_PAUSED){
				return ResumeTask(task->getTaskID());
			}

		}

		// send a message about current task state
		if (bSendEvents){
			SharedPtr<Event> msg(new KernelStartTaskEvent(task->getTaskName(), task->getTaskID()));
			Engine::sEventManager()->emitSystem(msg);
		}

		// OK
		return OK;
	}

	//-------------------------------------------------------------------------
	Result Kernel::_taskStop(SharedPtr<ITask>& task)
	{
		try{
			// no task given, so return error
			if (!task)
				return KERNEL_NO_TASK_FOUND;

			// check if the task is not a thread
			if (task->getTaskProperty() & TASK_IS_THREAD){

				SharedPtr<IThread> thread = boost::dynamic_pointer_cast<IThread, ITask>(task);
				NR_Log(Log::LOG_KERNEL, "Stop thread/task \"%s\" (id=%d)", task->getTaskName(), task->getTaskID());
				thread->threadStop();

			}else{
				// stop the task
				NR_Log(Log::LOG_KERNEL, "Stop task \"%s\" (id=%d)",task->getTaskName(), task->getTaskID());
				task->stopTask();
			}

		} catch (...){
			return UNKNOWN_ERROR;
		}

		// send a message about current task state
		if (bSendEvents){
			SharedPtr<Event> msg(new KernelStopTaskEvent(task->getTaskName(), task->getTaskID()));
			Engine::sEventManager()->emitSystem(msg);
		}


		// OK
		return OK;

	}

	//-------------------------------------------------------------------------
	/*Result Kernel::StartTask(taskID id){

		SharedPtr<ITask> task = getTaskByID(id);

		if (!task) {
			NR_Log(Log::LOG_KERNEL, Log::LL_DEBUG, "Can not start task with id=%d, because such task does not exists", id);
			return KERNEL_NO_TASK_FOUND;
		}

		// check if the task is not a thread
		return _taskStart(task);
	}*/


	//-------------------------------------------------------------------------
	Result Kernel::Execute(){
		try{

			// Log that kernel is initialized
			NR_Log(Log::LOG_KERNEL, "Kernel subsystem is active, start main loop");

			// loop while we have tasks in our pipeline
			while (taskList.size()){
				OneTick();
			}

			NR_Log(Log::LOG_KERNEL, "Stop kernel main loop");

		} catch(...){
			return UNKNOWN_ERROR;
		}

		// ok
		return OK;

	}


	//-------------------------------------------------------------------------
	TaskId Kernel::AddTask (SharedPtr<ITask> t, TaskOrder order, TaskProperty proper){

		prepareRootTask();

		_nrEngineProfile("Kernel::AddTask");

		try {

			t->_taskOrder = order;

			// check whenever such task already exists
			std::list< SharedPtr<ITask> >::iterator it;
			for (it = taskList.begin(); it != taskList.end(); it++){
				if ((*it) == t || (*it)->getTaskID() == t->getTaskID() || strcmp((*it)->getTaskName(),t->getTaskName())==0){
					NR_Log(Log::LOG_KERNEL, "Cannot add task \"%s\" because same task was already added!", t->getTaskName());
					return 0;
				}
			}

			for (it = pausedTaskList.begin(); it != pausedTaskList.end(); it++){
				if ((*it) == t || (*it)->getTaskID() == t->getTaskID() || strcmp((*it)->getTaskName(),t->getTaskName())==0){
					NR_Log(Log::LOG_KERNEL, "Found same task in paused task std::list !");
					return 0;
				}
			}

			// check if the given order number is valid
			if (t->getTaskOrder() <= ORDER_SYS_LAST && t->getTaskType() == TASK_USER)
			{
				NR_Log(Log::LOG_KERNEL, "User task are not allowed to work on system order numbers");
				return 0;
			}

			// setup some data
			t->setTaskProperty(proper);
			t->setTaskState(TASK_STOPPED);
			t->setTaskID(++lastTaskID);

			// some debug info
			if (proper & TASK_IS_THREAD)
				NR_Log(Log::LOG_KERNEL, "Add Task \"%s\" as thread (id = %d)", t->getTaskName(), t->getTaskID());
			else
				NR_Log(Log::LOG_KERNEL, "Add Task \"%s\" at order = %s (id = %d)", t->getTaskName(), orderToString(int32(t->getTaskOrder())).c_str(), t->getTaskID());

			// init task and check its return code
			if (t->onAddTask() != OK){
				NR_Log(Log::LOG_KERNEL, "Cannot initalize Task because of Task internal error");
				return 0;
			}

			// find the place for our task according to its order
			for( it=taskList.begin(); it != taskList.end(); it++){
				SharedPtr<ITask> &comp = (*it);
				if (comp->getTaskOrder() >= t->getTaskOrder()) break;
			}

			// add into the list
			taskList.insert (it,t);

		} catch(...){
			return UNKNOWN_ERROR;
		}

		// if this is not a root task, then add dependency
		// also there is no dependency if this is a thread
		if (t->_taskOrder != ORDER_SYS_ROOT && !(t->getTaskProperty() & TASK_IS_THREAD))
			mRootTask->addTaskDependency(t);

		// return the id
		return t->getTaskID();
	}


	//-------------------------------------------------------------------------
	Result Kernel::RemoveTask  (TaskId id){

		if (id == 0) return OK;

		try{

			NR_Log(Log::LOG_KERNEL, "Remove task with id=%d", id);

			// find the task
			PipelineIterator it;

			// check whenever iterator is valid
			if (!_getTaskByID(id, it, TL_RUNNING | TL_SLEEPING)){
				NR_Log(Log::LOG_KERNEL, "No such Task (id=%d) found !!!", id);
				return KERNEL_NO_TASK_FOUND;
			}

			// check whenever we are allowed to remove the task
			if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM)
			{
				NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to remove this task!");
				return KERNEL_NO_RIGHTS;

			}else{
				// say task want to remove his self
				NR_Log(Log::LOG_KERNEL, "Prepare to die: \"%s\" (id=%d)", (*it)->getTaskName(), (*it)->getTaskID());
				(*it)->_taskCanKill = true;
			}

		} catch(...){
			return UNKNOWN_ERROR;
		}

		// OK
		return OK;
	}


	//-------------------------------------------------------------------------
	Result Kernel::SuspendTask  (TaskId id){

		try{

			NR_Log(Log::LOG_KERNEL, "Suspend task (id=%d)", id);

			// find the task
			PipelineIterator it;

			// check whenever iterator is valid
			if (!_getTaskByID(id, it)){
				NR_Log(Log::LOG_KERNEL, "No task with id=%d found", id);
				return KERNEL_NO_TASK_FOUND;
			}else{
				SharedPtr<ITask> &t = (*it);

				if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM){
					NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to suspend this task!");
					return KERNEL_NO_RIGHTS;
				}else{

					// suspend task
					Result res = t->onSuspendTask();
					if (res == OK){
						t->setTaskState(TASK_PAUSED);

						// before removing the task from task std::list move it to paused std::list
						// so we can guarantee that task object will be held in memory
						pausedTaskList.push_back(t);
						taskList.erase(it);
						NR_Log(Log::LOG_KERNEL, "Task id=%d is sleeping now", id);

						// send a message about current task state
						if (bSendEvents){
							SharedPtr<Event> msg(new KernelSuspendTaskEvent(t->getTaskName(), t->getTaskID()));
							Engine::sEventManager()->emitSystem(msg);
						}

					}else{
						NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "Task id=%d can not suspend! Check this!", id);
						return res;
					}
				}
			}

		} catch(...){
			return UNKNOWN_ERROR;
		}

		// OK
		return OK;
	}

	//-------------------------------------------------------------------------
	Result Kernel::ResumeTask  (TaskId id){

		try{

			NR_Log(Log::LOG_KERNEL, "Resume task (id=%d)", id);

			// find the task
			PipelineIterator it;

			// check whenever iterator is valid
			if (!_getTaskByID(id, it, TL_SLEEPING)){
				NR_Log(Log::LOG_KERNEL, "No task with id=%d found", id);
				return KERNEL_NO_TASK_FOUND;
			}

			SharedPtr<ITask> &t = (*it);
			if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM){
				NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to resume this task!");
				return KERNEL_NO_RIGHTS;
			}else{

				// resume the task
				Result res = t->onResumeTask();
				if (res == OK){
					t->setTaskState(TASK_RUNNING);

					//keep the order of priorities straight
					for( it = taskList.begin(); it != taskList.end(); it++){
						SharedPtr<ITask> &comp=(*it);
						if(comp->getTaskOrder() >= t->getTaskOrder()) break;
					}
					taskList.insert(it,t);

					// erase task from paused std::list. Therefor we have to find it in the std::list
					if (_getTaskByID(id, it, TL_SLEEPING)){
						pausedTaskList.erase(it);
					}

					// send a message about current task state
					if (bSendEvents){
						SharedPtr<Event> msg(new KernelResumeTaskEvent(t->getTaskName(), t->getTaskID()));
						Engine::sEventManager()->emitSystem(msg);
					}

				}else{
					NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "Task id=%d can not resume, so it stay in sleep mode", id);
					return res;
				}
			}

		} catch(...){
			return UNKNOWN_ERROR;
		}

		// OK
		return OK;
	}

	//-------------------------------------------------------------------------
	Result Kernel::StopExecution(){

		try{

			NR_Log(Log::LOG_KERNEL, "Stop the kernel subsystem");

			// iterate through all tasks and kill them
			for(PipelineIterator it = taskList.begin(); it != taskList.end(); it++){
				NR_Log(Log::LOG_KERNEL, "Prepare to die: \"%s\" (id=%d)", (*it)->getTaskName(), (*it)->getTaskID());
				(*it)->_taskCanKill=true;
			}

			// iterate also through all paused tasks and kill them also
			for(PipelineIterator it = pausedTaskList.begin(); it != pausedTaskList.end(); it++){
				NR_Log(Log::LOG_KERNEL, "Prepare to die: \"%s\" (id=%d)", (*it)->getTaskName(), (*it)->getTaskID());
				(*it)->_taskCanKill=true;
			}

			// Info if we do not have any tasks to kill
			if (!taskList.size() && !pausedTaskList.size())
				NR_Log(Log::LOG_KERNEL, "There is no more tasks to be killed !");

		} catch(...){
			return UNKNOWN_ERROR;
		}

		// OK
		return OK;

	}


	//-------------------------------------------------------------------------
	Result Kernel::ChangeTaskOrder(TaskId id, TaskOrder order){

		try{

			NR_Log(Log::LOG_KERNEL, "Change order of task (id=%d) to %d", id, int32(order));

			// find the task
			PipelineIterator it;

			// check whenever iterator is valid
			if (!_getTaskByID(id, it, TL_RUNNING | TL_SLEEPING)){
				NR_Log(Log::LOG_KERNEL, "No task with id=%d found", id);
				return KERNEL_NO_TASK_FOUND;
			}

			SharedPtr<ITask> &t = (*it);

			// check if the given order number is valid
			if (order <= ORDER_SYS_LAST && t->getTaskType() == TASK_USER)
			{
				NR_Log(Log::LOG_KERNEL, "User task are not allowed to work on system order numbers!");
				return KERNEL_NO_RIGHTS;
			}

			// check for task access rights
			if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM){
				NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to change the order of system task!");
				return KERNEL_NO_RIGHTS;
			}else{

				// change order of the task
				t->setTaskOrder(order);
				t->_orderChanged = false;

				// sort lists
				pausedTaskList.sort();
				taskList.sort();
			}

		} catch(...){
			return UNKNOWN_ERROR;
		}

		// OK
		return OK;
	}

	//-------------------------------------------------------------------------
	bool Kernel::_getTaskByID(TaskId id, PipelineIterator& jt, int32 useList){

		// get std::list we want to search in
		std::list< SharedPtr<ITask> > *li = NULL;

		// search in normal list
		if ((useList & TL_RUNNING) == TL_RUNNING){
			li = &taskList;

			// iterate through elements and search for task with same id
			PipelineIterator it;
			for ( it = li->begin(); it != li->end(); it++){
				if ((*it)->getTaskID() == id){
					jt = it;
					return true;
				}
			}
		}

		// search in paused list
		if ((useList & TL_SLEEPING) == TL_SLEEPING){
			li = &pausedTaskList;

			// iterate through elements and search for task with same id
			PipelineIterator it;
			for ( it = li->begin(); it != li->end(); it++){
				if ((*it)->getTaskID() == id){
					jt = it;
					return true;
				}
			}

		}

		// not found so return an end iterator
		return false;
	}

	//-------------------------------------------------------------------------
	bool Kernel::_getTaskByName(const std::string& name, PipelineIterator& it, int32 useList)
	{

		// get std::list we want to search in
		std::list< SharedPtr<ITask> > *li = NULL;

		// search in normal list
		if ((useList & TL_RUNNING) == TL_RUNNING){
			li = &taskList;

			// iterate through elements and search for task with same id
			PipelineIterator jt;
			for ( jt = li->begin(); jt != li->end(); jt++){
				if ((*jt)->getTaskName() == name){
					it = jt;
					return true;
				}
			}
		}

		// search in paused list
		if ((useList & TL_SLEEPING) == TL_SLEEPING){
			li = &pausedTaskList;

			// iterate through elements and search for task with same id
			PipelineIterator jt;
			for ( jt = li->begin(); jt != li->end(); jt++){
				if ((*jt)->getTaskName() == name){
					it = jt;
					return true;
				}
			}
		}

		// not found so return end iterator
		return false;
	}

	//-------------------------------------------------------------------------
	SharedPtr<ITask> Kernel::getTaskByID(TaskId id){
		try{

			// find the task
			PipelineIterator it;

			// check whenever iterator is valid
			if (!_getTaskByID(id, it, TL_RUNNING | TL_SLEEPING)){
				NR_Log(Log::LOG_KERNEL, "getTaskByID: No task with id=%d found", id);
				return SharedPtr<ITask>();
			}

			// check the rights
			if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM){
				NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to access to this task!");
				return SharedPtr<ITask>();
			}

			return *it;

		} catch(...){
			return SharedPtr<ITask>();
		}

	}


	//-------------------------------------------------------------------------
	SharedPtr<ITask> Kernel::getTaskByName(const std::string& name){
		try{

			// find the task
			PipelineIterator it;

			// check whenever iterator is valid
			if (!_getTaskByName(name, it, TL_RUNNING | TL_SLEEPING)){
				NR_Log(Log::LOG_KERNEL, "getTaskByName: No task with name=%s found", name.c_str());
				return SharedPtr<ITask>();
			}

			// check the rights
			if (!areSystemTasksAccessable() && (*it)->getTaskType() == TASK_SYSTEM){
				NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "You do not have rights to access to this task!");
				return SharedPtr<ITask>();
			}

			return *it;

		} catch(...){
			return SharedPtr<ITask>();
		}
	}

	//-------------------------------------------------------------------------
	Result Kernel::_loopStartCycle()
	{

		// Profiling of the engine
		_nrEngineProfile("Kernel::_loopStartCycle");

		// iterate through the tasks and mark them as non visited
		_loopIterator = taskList.begin();
		while (_loopIterator != taskList.end())
		{
			(*_loopIterator)->_taskGraphColor = 0;
			_loopIterator ++;
		}

		_loopIterator = pausedTaskList.begin();
		while (_loopIterator != pausedTaskList.end())
		{
			(*_loopIterator)->_taskGraphColor = 0;
			_loopIterator ++;
		}


		_loopIterator = taskList.begin();

		return OK;
	}

	//-------------------------------------------------------------------------
	Result Kernel::_loopGetNextTask(PipelineIterator it, PipelineIterator& result, int depth)
	{
		// Profiling of the engine
		_nrEngineProfile("Kernel::_loopGetNextTask");

		// check whenever this is the end
		if (it == taskList.end()) return KERNEL_END_REACHED;

		// get the task in this iterator
		const SharedPtr<ITask>& t = *it;

		if (t->_taskGraphColor == 1) return KERNEL_CIRCULAR_DEPENDENCY;
		if (t->_taskGraphColor == 2) return KERNEL_END_REACHED;

		t->_taskGraphColor = 1;

		// now check if the task has got childrens
		if (t->_taskDependencies.size() > 0)
		{

			// do for each child call this function recursively
			/*for (uint32 i = 0; i < t->_taskDependencies.size(); i++)
			{
				PipelineIterator jt;
				if (!_getTaskByID(t->_taskDependencies[i], jt, TL_RUNNING | TL_SLEEPING)) return KERNEL_TASK_MISSING;

				// check whenever the child was already visited
				if ((*jt)->_taskGraphColor != 2){
					Result ret = _loopGetNextTask(jt, result, depth+1);

					// we return OK only if we have not found any circularity
					// and we are on the top (root-node)
					if (ret != OK){
						t->_taskGraphColor = 0;
						if (depth == 0 && ret != KERNEL_CIRCULAR_DEPENDENCY) return OK;
						return ret;
					}
				}
			}*/

			// iterate through all dependencies and check them
			std::list<SharedPtr<ITask> >::iterator jt = t->_taskDependencies.begin();
			for (; jt != t->_taskDependencies.end(); jt ++)
			{
				// check whenever the child was already visited
				if ((*jt)->_taskGraphColor != 2){
					Result ret = _loopGetNextTask(jt, result, depth+1);

					// we return OK only if we have not found any circularity
					// and we are on the top (root-node)
					if (ret != OK){
						t->_taskGraphColor = 0;
						if (depth == 0 && ret != KERNEL_CIRCULAR_DEPENDENCY) return OK;
						return ret;
					}
				}

			}

		// we do not have childs, so this is a leaf node
		}

		// all our chlds are visited, so return the node itself
		t->_taskGraphColor = 2;
		result = it;
		return KERNEL_LEAF_TASK;

	}

	//-------------------------------------------------------------------------
	Result Kernel::_loopEndCycle()
	{
		return OK;
	}

}; //namespace nrEngine

