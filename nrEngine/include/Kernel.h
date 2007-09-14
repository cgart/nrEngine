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

/*!
 * \defgroup kernel Kernel (Heart) of the nrEngine
 *
 * As you know each operating system has got a kernel. This kernel run tasks
 * (user programms) to allow to do a lot of things. So our engine is nothing
 * else as a little operating system. Here we implement kernel of the engine.
 * This kernel runs tasks in there priority order, so you can run for example
 * display task as last task and input as first. Kernel has also support to freeze
 * tasks (suspend) and resume them from sleeping. To run application written
 * for nrEngine it is better if you use this system. Actually kernel is nothing else
 * than the old style game loop. Kernel runs tasks in a one big loop until there
 * is no more tasks.
 *
 */

#ifndef _NR_C_KERNEL_H_
#define _NR_C_KERNEL_H_


//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "ITask.h"


namespace nrEngine {

	//! Heart of our engine system, where tasks are running
	/**
	* \par
	* Kernel - is the heart of our engine. Kernel is something like the oldschool
	* Game Loop. You can add, remove, suspend or resume tasks from kernel's pipeline
	* Tasks would be executed in their order - smaller order number comes before
	* greater order number.
	*
	* \par
	* What should tasks supports:
	*	- They should implement the \a ITask Interface
	*	- They should be loaded and be ready to start before adding to kernels pipeline
	*	- In each game cycle \a ITask::updateTask() Method will be executed
	*	- By suspending or resuming the Tasks \a ITask::onSuspendTask()/onResumeTask() Method will be executed
	*	- Before task will be removed \a ITask::stopTask() Method will executed
	*	- Each taskID is unique and is greater than 0
	*	- There is no TaskId wich will be given twice
	*
	* \par
	*	Tasks can depends on each other, so one task should be updated before another one.
	*	This can be defined through task dependencies functions. Sometimes you also
	*	need to execute a task more than once per one kernel tick. This also can be
	*	specified by executing of appropriate functions.<br>
	*	Sleeping tasks will not be sorted in into the kernel's pipeline if any task
	*	will be added, that depends on such one. If the task will be resumed than the sorting
	*	will be done. So be warned that it is possible to retrieve a circularity
	*	error code during the waking up.
	*
	* \par
	* 	In our Engine their exisists system and user tasks. System tasks are not accessable through the
	*	user, because they are essential for engine's work. So to allow the engine to use
	*	same functions for task access, we define a boolean lock variable. If this variable
	*	is true, so all accesses to the kernel are done through the engine. The kernel
	*	can give then all tasks engine ask about. Locking of the kernel can only be done
	*	through the engine, so the user is not able to do the locking.<br>
	*	Better way to implement this is to have other programming language that allows
	*	"sandboxes" or similar techniques to distinguish between engine's own and user calls.
	*
	* \note: Try to optimise this kernel by implementing O(1) Scheduler or something
	*		 else if kernel loop/pipeline need a lot of time for updating.
	*		 Better if you implement supporting of multithreaded Kernel, so we can
	*		 support Multiple-CPU-System to increase game speed
	* \ingroup kernel
	**/
	class _NRExport Kernel{
	public:

		/**
		 * Define if a kernel should send special task events on the
		 * engines default channel. Task events are used to inform the application
		 * and the engine, that state of certain tasks is changed.
		 *
		 * Actually the engine does not need this events, cause engine's modules
		 * does not depend on each other, so they do not need this information.
		 * However your application could require this messages to react to
		 * certain conditions, i.e. clock task is sleeping
		 *
		 * @param bSend if true the events would be sent, if false so kernel is silent
		 **/
		void sendEvents(bool bSend = true) { bSendEvents = bSend; }

		/**
		 * If true, so the kernel is currently inform teh application about
		 * task state changes by sending messages over the engine's default communication
		 * channel.
		 **/
		bool isSendingEvents() const { return bSendEvents; }

		/**
		 * Executes the kernel (old school main loop :-)
		 * Before main loop is started all tasks will be intialized by calling task
		 * function \a ITask::taskStart() . If taskStart returns error, so this task will be
		 * excluded from execution task list.
		 * \return OK or error code:
		 * 		- ERROR_UNKNOWN for unknown error
		 *
		 * \par
		 *		If you get an error code back, so you can probably found more useful information
		 *		in a log file filled out by the engine's kernel.
		 **/
		Result Execute();

		/**
		 * Executes all task according to their order. After that
		 * return back. If you call this function for the first time,
		 * so the task will start before updating.
		 * \see ITask manual to understand how tasks works
		 **/
		Result OneTick();


		/**
		 * Add the given task into our kernel pipeline (main loop)
		 * Before task will be added it's \a ITask::taskInit()) function will be executed
		 * The returned task id number can be used to access to the task through kernel.
		 *
		 *
		 * \param task - is a smart pointer to an object implementing ITask-Interface
		 * \param order Order number for this task (default is ORDER_NORMAL)
		 * \param property Specifiy the special property of the task
		 * \return task id of added task or 0 if task could not be added
		 * 
		 **/
		TaskId AddTask (SharedPtr<ITask> task, TaskOrder order = ORDER_NORMAL, TaskProperty property = TASK_NONE);

		/**
		 * Remove the task from our game loop (pipeline).
		 *
		 * \param id - id of a task to be removed
		 * \return OK or error code:
		 * 		- ERROR_UNKNOWN for unknown error
		 *		- KERNEL_NO_TASK_FOUND if no such task was found
		 *		- KERNEL_NO_RIGHTS if you try to remove a system task
		 **/
		Result RemoveTask	(TaskId id);


		/**
		* This method will start a task with given id. If task could be started,
		* so it will be updated in next cycles. Otherwise task state will be set
		* to stopped. If the task is already running it will be not started again.
		* If task is paused, so it will be resumed through \a ResumeTask() method.
		*
		* @param id Unique id of the task to be started
		* @return either OK or:
		*		- UNKNOWN_ERROR
		*		- KERNEL_NO_TASK_FOUND
		*		- KERNEL_NO_RIGHTS if you try to remove a system task
		**/
		//Result StartTask	(TaskId id);


		/**
		 * Suspend task to prevent it from update. Task will get to sleep.
		 *
		 * \param id - id of a task to be suspended
		 * \return OK or error code:
		 * 		- ERROR_UNKNOWN for unknown error
		 *		- KERNEL_NO_TASK_FOUND if such a task was not found
		 *		- KERNEL_NO_RIGHTS if you try to remove a system task
		 **/
		Result SuspendTask (TaskId id);

		/**
		 * Resume task from sleeping.
		 *
		 * \param id - id of a task to waik
		 * \return OK or error code:
		 * 		- ERROR_UNKNOWN for unknown error
		 *		- KERNEL_NO_TASK_FOUND if such a task was not found
		 *		- KERNEL_NO_RIGHTS if you try to remove a system task
		 **/
		Result ResumeTask  (TaskId id);


		/**
		 * Remove and kill all tasks from the kernel's task list. If no tasks are in the
		 * list, so the kernel will stop executing.
		 *
		 * \return OK or error code:
		 * 		- ERROR_UNKNOWN for unknown error
		 **/
		Result StopExecution();


		/**
		 * Changes the order number of task with given id
		 * \param id  id of a task
		 * \param order  New order number for the task
		 * \return OK or error code:
		 * 		- ERROR_UNKNOWN for unknown error
		 *		- KERNEL_NO_TASK_FOUND if such a task was not found
		 *		- KERNEL_NO_RIGHTS if you try to remove a system task
		 **/
		Result ChangeTaskOrder(TaskId id, TaskOrder order = ORDER_NORMAL);


		/**
		 * Returns smart pointer to a task with the given id.
		 *
		 * \param id ID of the task
		 * \return smart pointer to the task or NULL if no such task found or task is system task
		 **/
		SharedPtr<ITask> getTaskByID(TaskId id);

		/**
		 * Get the task wich has the same name as the given one.
		 *
		 * \param name Unique name of the task
		 * \return smart poitner to the task or to the NULL if no such task found or task is system task
		**/
		SharedPtr<ITask> getTaskByName(const std::string& name);

	protected:

		//! Here kernel does store all currently running tasks
		std::list< SharedPtr<ITask> > taskList;

		//! Here kernel store all tasks that are sleeping now.
		std::list< SharedPtr<ITask> > pausedTaskList;

		//! Get information about lock state of the kernel
		bool areSystemTasksAccessable() { return _bSystemTasksAccessable; }

	private:
		
		/**
		 * Define the kernel's list names containing tasks.
		 **/
		enum {
			//! Task that are currently running are stored here
			TL_RUNNING = 1 << 1,

			//! Tasks that are sleeping are stored in this list
			TL_SLEEPING = 1 << 2

		};

		//! Engine is allowed to create instances of that object
		friend class Engine;

		/**
		 * Clear all lists and initialize internal variables.
		 **/
		Kernel();

		/**
		 * Release all used memory and remove all tasks from the kernel. Each task is
		 * added to the kernel's taks list through shared pointers, so by removing
		 * tasks of the kernel's list the pointer reference number will be decreased.
		 *
		 * The destructor is defined as virtual, so you can create your own kernel class
		 * if you have to.
		 */
		~Kernel();


		typedef std::list< SharedPtr<ITask> >::iterator PipelineIterator;

		/**
		 * Find the task by task ID.
		 *
		 * \param id is the id of task
		 * \param it Iterator pointing to the element
		 * \param useList declare in which lists should be searched for the task
		 * \return true if such one was found
		 * \note This Function runs in O(N) so optimize this if you know how
		**/
		bool _getTaskByID(TaskId id, PipelineIterator& it, int32 useList = TL_RUNNING);

		/**
		 * Find the task by task's name.
		 *
		 * \param name is the name of task
		 * \param it Iterator pointing to the element
		 * \param useList declare in which lists should be searched for the task
		 * \return true if such one was found
		 * \note This Function runs in O(N) so optimize this if you know how
		 **/
		bool _getTaskByName(const std::string& name, PipelineIterator& it, int32 useList = TL_RUNNING);

		/**
		 * Get the list containing all smart pointers of tasks that are in kernel's
		 * execution list.
		 **/
		const std::list< SharedPtr<ITask> >& getTaskList(){return taskList;}


		/**
		 * Get list of all paused tasks that are running in the kernel. Paused tasks are tasks
		 * which were suspended through \a SuspendTask() method.
		 **/
		const std::list< SharedPtr<ITask> >& getPausedTaskList(){return pausedTaskList;}

		/**
		* Solving of dependencies is needed to bring up the task
		* list in the right order. Each task that depends on another one, should be
		* later in the list, as the one on which depends.
		*
		* \param retTasks In this vector there will be task ids stored according to the
		*				error code given back (for OK no task ids, circularity - all task ids
		*				that builds the circle, task missing - task ids which are missing, ...)
		*
		* \return either OK or:
		*			- KERNEL_CIRCUIT_DEPENDENCIES
		*			- KERNEL_TASK_MISSING
		*			- BAD_PARAMETERS if the given pointer is null or the vector does contain data
		**/
		//Result _solveDependencies(std::vector<taskID>* retTasks);

		/**
		* This method will start a one cycle in the kernel loop. So it will prepare
		* the variables needed by the next task function, which will return next task
		* in the kernel's task pipeline
		* \return error code if any occurs
		**/
		Result _loopStartCycle();

		/**
		* This method will give us the next task in the kernel's pipeline which should be updated.
		* This function also solve the dependencies of the tasks by the depth first search
		* algorithm. So you will get an iterator from the kernel'S task list containing the
		* task to be updated as next. If iterator is equal to the end, so there is no more
		* tasks in the kernel's list
		* \return error code if any occurs (i.e. circularity or task missing)
		**/
		Result _loopGetNextTask(PipelineIterator it, PipelineIterator& result, int depth = 0);

		/**
		* This method should be called after the cycle is ended. This will bring the
		* variables in the right state, so works can be done again later.
		**/
		Result _loopEndCycle();

		/**
		 * Prepare the kernel to run tasks. This will create a root task and add
		 * all necessary dependencies.
		 **/
		void prepareRootTask();


		/**
		 * This function will start each task and set it's state to running,
		 * if starting was successful otherwise the task will not run.
		 **/
		void startTasks();

		//! Lock the kernel, so kernel gices us access to system tasks
		void lockSystemTasks();

		//! Unlock the kernel, so it close access to the system tasks
		void unlockSystemTasks();

		//! Try to start a given task
		Result _taskStart(SharedPtr<ITask>& task);

		//! Stop the given task
		Result _taskStop(SharedPtr<ITask>& task);

		//! Last given task id. Is used to generate new ids for newly added tasks
		TaskId lastTaskID;

		//! If true, so we know that all tasks are started and a list of the tasks is sorted. Only for internal use.
		bool bTaskStarted;

		//! Check whenever we have already added a root task
		bool bInitializedRoot;

		//! Root task
		SharedPtr<ITask>	mRootTask;
		
		//! If it is true, so the kernel is locked for engine access. All next operations until unlock, can access to system tasks
		bool _bSystemTasksAccessable;

		//! Used by the _loop* Methods
		static PipelineIterator _loopIterator;

		//! Should kernel send events if states of tasks are changed
		bool bSendEvents;
	};

}; // end Namespace

#endif	//_NR...
