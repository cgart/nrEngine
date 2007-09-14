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
#include <nrEngine/IThread.h>
#include <nrEngine/EventManager.h>
#include <nrEngine/Log.h>
#include <boost/bind.hpp>

namespace nrEngine{

	//--------------------------------------------------------------------
	IThread::IThread()// : EventActor(std::string("Thread_") + boost::lexical_cast<std::string>(id))
	{
		//mThread = NULL;
		mThreadState = THREAD_STOP;
	}

	//--------------------------------------------------------------------
	IThread::~IThread()
	{
		// delete the thread object if it is not empty
		//if (mThread)
		//{
		//	delete mThread;
		//}
	}

	//--------------------------------------------------------------------
	void IThread::threadStart()
	{
		// Check if we have already a thread created
		if (mThread)
		{
			NR_Log(Log::LOG_KERNEL, Log::LL_WARNING, "IThread: the appropriate thread is already running!");
			return;
		}
		NR_Log(Log::LOG_KERNEL, "IThread: Create thread and start it");

		mThread.reset(new boost::thread(boost::bind(&IThread::run, this)));
		// initialise the attribute
		/*pthread_attr_init(&mThreadAttr);

		// create the joinable attribute for thsi thread
		pthread_attr_setdetachstate(&mThreadAttr, PTHREAD_CREATE_JOINABLE);

		// setup thread scheduling attribute
		pthread_attr_setschedpolicy(&mThreadAttr, SCHED_RR);
  		
		// now create a thread and let it run
		int res = pthread_create(&mThread, &mThreadAttr, IThread::run, (void *)this);
		if (res){
			NR_Log(Log::LOG_KERNEL, Log::LL_ERROR, "IThread: creation of a thread failed with error code %d", res);
			return;
		}*/
		mThreadState = THREAD_RUNNING;
   
		mThreadState = THREAD_RUNNING;
		mThread.reset(new boost::thread(boost::bind(IThread::run, this)));
		
	}

	//--------------------------------------------------------------------
	void IThread::threadStop()
	{
		// change state
		changeState(THREAD_STOP);

		// join the thread to the main process
		/*pthread_attr_destroy(&mThreadAttr);
		int res = pthread_join(mThread, NULL);
		if (res){
			NR_Log(Log::LOG_KERNEL, Log::LL_ERROR, "IThread: can not join running thread (error code %d)", res);
			return;
		}*/
		mThread->join();
	}

	//--------------------------------------------------------------------
	void IThread::threadSuspend()
	{
		changeState(THREAD_NEXT_SUSPEND);
	}

	//--------------------------------------------------------------------
	void IThread::threadResume()
	{
		changeState(THREAD_NEXT_RESUME);
	}

	//--------------------------------------------------------------------
	void IThread::changeState(ThreadState newState)
	{
		// first lock the mutex and then change the state
		/*try{
			boost::mutex::scoped_lock lock(mMutex);
			mThreadState = newState;
		}catch (boost::lock_error err) {}
		*/

		// lock the mutex change status and unlock it again
		//pthread_mutex_lock(&mMutex);
		mThreadState = newState;
		//pthread_mutex_unlock(&mMutex);
		
	}

	//--------------------------------------------------------------------
	void IThread::yield(IThread* mythread)
	{
		// use pthread to yield the timeslice
		//pthread_yield();
		boost::thread::yield();
	}

	//--------------------------------------------------------------------
	void IThread::run(void* _mythread)
	{
		// try to cast the given parameter to IThread pointer
		IThread* mythread = static_cast<IThread*>(_mythread);
		if (mythread == NULL)
		{
			NR_Log(Log::LOG_KERNEL, Log::LL_ERROR, "IThread: not valid parameter was specified for IThread::run(void*) method");
			return;
		}
		
		// now loop the thread until some messages occurs
		bool run = true;
		while (run){

			// kernel requested to suspend the thread
			if (mythread->mThreadState == THREAD_NEXT_SUSPEND)
			{
				// notice about suspending and go into sleep mode
				mythread->changeState(THREAD_SLEEPING);
				mythread->_noticeSuspend();

			// kernel requested to resume the execution
			}else if (mythread->mThreadState == THREAD_NEXT_RESUME)
			{
				// notice about resuming the work and start it again
				mythread->changeState(THREAD_RUNNING);
				mythread->_noticeResume();

			// kernel does not requested anything, so run the task
			}else if (mythread->mThreadState == THREAD_RUNNING)
			{
				mythread->_noticeUpdate();
			}

			// check for the stop message, then stop the thread
			// this is a reading mutex, so do not have to lock it
			run = mythread->mThreadState != THREAD_STOP;

			// we now yield the used timeslice for another threads
			yield(mythread);
		}
		
		// notice to stop the underlying task
		mythread->_noticeStop();

		// exit the thread
		//return NULL;
	}

}; // end namespace

