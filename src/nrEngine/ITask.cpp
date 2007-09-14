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


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/ITask.h>
#include <nrEngine/Log.h>
#include <nrEngine/Kernel.h>

namespace nrEngine{

	//--------------------------------------------------------------------
	ITask::ITask() : IThread(){
		init();
	}

	//--------------------------------------------------------------------
	ITask::ITask(const std::string& name) : IThread(), _taskName(name)
	{
		init();
	}

	//--------------------------------------------------------------------
	void ITask::init(){
		_taskCanKill = false;
		_taskOrder = ORDER_NORMAL;
		_taskID = 0;
		_taskState = TASK_STOPPED;
		_orderChanged = false;
		_taskType = TASK_USER;
		_taskGraphColor = 0;
		_taskProperty = TASK_NONE;
	}
	
	//--------------------------------------------------------------------
	ITask::~ITask(){

	}

	//--------------------------------------------------------------------
	bool ITask::operator <  (const ITask &t){
		return this->_taskOrder < t._taskOrder;
	}

	//--------------------------------------------------------------------
	bool ITask::operator == (const ITask &t){
		return this->_taskOrder == t._taskOrder;
	}

	//--------------------------------------------------------------------
	bool ITask::operator <= (const ITask &t){
		return (*this < t) || (*this == t);
	}

	//--------------------------------------------------------------------
	bool ITask::operator >  (const ITask &t){
		return !(*this <= t) ;
	}

	//--------------------------------------------------------------------
	bool ITask::operator >= (const ITask &t){
		return !(*this < t) || (*this == t);
	}

	//--------------------------------------------------------------------
	bool ITask::operator != (const ITask &t){
		return !(*this == t);
	}

	//--------------------------------------------------------------------
	void ITask::setTaskOrder(TaskOrder order){
		_taskOrder = order;
		_orderChanged = true;
	}
	//--------------------------------------------------------------------
	void ITask::setTaskType(TaskType type){
		this->_taskType = type;
	}

	//--------------------------------------------------------------------
	void ITask::setTaskID(TaskId id){
		this->_taskID = id;
	}

	//--------------------------------------------------------------------
	void ITask::setTaskState(TaskState state){
		this->_taskState = state;
	}
	
	//--------------------------------------------------------------------
 	void ITask::setTaskProperty(TaskProperty proper){
		this->_taskProperty = proper;
	}

	//--------------------------------------------------------------------
	void ITask::setTaskName(const std::string& name){
		_taskName = name;
	}

	struct _taskSort : std::less<SharedPtr<ITask> >
	{
		public:
			bool operator () (const SharedPtr<ITask>& a, const SharedPtr<ITask>& b)
			{
				return (*a) < (*b);
			}
	};

	//--------------------------------------------------------------------
	Result ITask::addTaskDependency(SharedPtr<ITask> task)
	{
		// add the task into the list of dependencies
		_taskDependencies.push_back(task);

		// sort the dependency list 
		_taskDependencies.sort(_taskSort());
		
		// debug info
		NR_Log(Log::LOG_KERNEL, Log::LL_DEBUG, "Task %s depends now on task %s", getTaskName(), task->getTaskName());

		return OK;
	}
	
	//--------------------------------------------------------------------
	Result ITask::addTaskDependency(TaskId id)
	{
		// get task from kernel based on id
		SharedPtr<ITask> task = Engine::sKernel()->getTaskByID(id);
		
		if (task != NULL)
		{
			return addTaskDependency(task);
		}

		return KERNEL_NO_TASK_FOUND;
	}

	//--------------------------------------------------------------------
	Result ITask::addTaskDependency(const std::string& name)
	{
		// get task from kernel based on id
		SharedPtr<ITask> task = Engine::sKernel()->getTaskByName(name);
		
		if (task != NULL)
		{
			return addTaskDependency(task);
		}

		return KERNEL_NO_TASK_FOUND;
	}
	//--------------------------------------------------------------------
	/*Result ITask::addTaskDependency(const ITask& task)
	{
		return addTaskDependency(task.getTaskID());
	}

	//--------------------------------------------------------------------
	Result ITask::addTaskDependency(const ITask* pTask)
	{
		return addTaskDependency(pTask->getTaskID());
	}
	*/
	
	//--------------------------------------------------------------------
	void ITask::_noticeSuspend(){
		onSuspendTask();
	}

	//--------------------------------------------------------------------
	void ITask::_noticeResume(){
		onResumeTask();
	}

	//--------------------------------------------------------------------
	void ITask::_noticeUpdate(){
		updateTask();
	}

	//--------------------------------------------------------------------
	void ITask::_noticeStop(){
		stopTask();
	}

}; // end namespace

