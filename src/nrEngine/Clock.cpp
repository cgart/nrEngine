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
#include <nrEngine/Clock.h>
#include <nrEngine/Log.h>

//----------------------------------------------------------------------------------
// Defines
//----------------------------------------------------------------------------------
#define DEFAULT_OBSERVER_NAME "__no_name__"


namespace nrEngine{
	
	//----------------------------------------------------------------------------------
	ScriptFunctionDec(scriptResetClock, Clock)
	{
		// check parameter count
		if (args.size() < 1)
		{
			return ScriptResult(std::string("resetClock([timeToReset = 0, [reset_all_timers = 1]]) - Not valid parameter count!"));
		}

		if (param.size() == 0)
		{
			return ScriptResult(std::string("resetClock() was not properly registered before. Clock was not passed!"));
		}
		
		// get clock 
		Clock* clock = nrEngine::ScriptEngine::parameter_cast<Clock* >(param[0]);
		if (clock == NULL)
		{
			return ScriptResult(std::string("resetClock() was not properly registered before. Clock was not passed!"));		
		}
		
		// get parameters
		bool shouldResetTimers = true;
		float64 timeToReset = 0;
		if (args.size() > 1) timeToReset = boost::lexical_cast<float64>(args[1]);
		if (args.size() > 2) shouldResetTimers = boost::lexical_cast<bool>(args[2]);
		
		// reset the clock 
		clock->reset(timeToReset, shouldResetTimers);

		return ScriptResult();
	}

	//------------------------------------------------------------------------
	Clock::Clock() : ITask(){
		
		setTaskName("SystemClock");
		currentTime = 0;
		frameTime = 0;
		frameNumber = 0;
		
		sourceStartValue = 0;
		sourceLastValue = 0;
		sourceLastRealValue = 0;
		
		useFixRate = false;
		fixFrameRate = 0;
		useMaxRate = false;
		maxFrameRate = 0;
		lastCurrentTime = 0;

		// add default frame time	
		setFrameWindow (7);
		frameTime = _getFilteredFrameDuration();
		
		timeSource.reset();
		realTimeSource.reset(new TimeSource());
		
		// setup some other values
		nextSyncTime = 0;
		syncInterval = 0;
		
	}
		
	//------------------------------------------------------------------------
	Clock::~Clock(){
		stopTask();
	}
	
	//------------------------------------------------------------------------	
	void Clock::reset(float64 resetToTime, bool resetAllObservers)
	{
		// check if we have to reset observers
		if (resetAllObservers)
		{
			NR_Log(Log::LOG_ENGINE, "Clock: Reset time observers");
			ObserverList::iterator it = observers.begin();
			for (; it != observers.end(); it++) (*it)->resetObserver(resetToTime);
		}
		
		// reset the clock 
		lastCurrentTime = resetToTime;
		currentTime = resetToTime;
		frameTime = frameDefaultTime;
		//frameNumber = 0;
		realFrameTime = frameDefaultTime;
		nextSyncTime = lastCurrentTime + static_cast<float64>(syncInterval) / 1000.0;
		
		// reset time source
		setTimeSource(timeSource);
		if (timeSource != NULL) timeSource->reset(resetToTime);
		
		// reset the frame history
		setFrameWindow(frameFilteringWindow, frameDefaultTime);
		
		NR_Log(Log::LOG_ENGINE, "Clock: Reset clock to %f", resetToTime);		
	}

	//------------------------------------------------------------------------	
	void Clock::setTimeSource(SharedPtr<TimeSource> timeSource)
	{
		this->timeSource = timeSource;
	
		if (timeSource != NULL){
			sourceStartValue = timeSource->getTime();
			sourceLastValue = sourceStartValue;
		}
	}
	
	//------------------------------------------------------------------------	
	void Clock::setSyncInterval(uint32 milliseconds)
	{
		this->syncInterval = milliseconds;
	}
	
	//------------------------------------------------------------------------
	bool Clock::isAnySourceBounded(){
		return (timeSource != NULL);
	}
	
	//------------------------------------------------------------------------
	SharedPtr<Timer> Clock::createTimer(){
	
		// create timer as observer
		SharedPtr<ITimeObserver> timer(new Timer(*this));
		
		// add him to the list
		int32 id = addObserver(timer);
		
		if (id == 0){
			NR_Log(Log::LOG_ENGINE, Log::LL_WARNING, "Clock::createTimer(): Can not add timer to the observer list");
		}
		
		// set the id of the timer
		timer->_observerID = id;
		NR_Log(Log::LOG_ENGINE, Log::LL_DEBUG, "Clock::createTimer(): New timer created (id=%d)", id);
		
		// return created timer
		return ::boost::dynamic_pointer_cast<Timer, ITimeObserver>(timer);
		
	}
	
	//------------------------------------------------------------------------
	Result Clock::onStartTask()
	{
		// register function by the manager
		std::vector<nrEngine::ScriptParam> param;
		param.push_back(this);
		Engine::sScriptEngine()->add("resetClock", scriptResetClock, param);
		
		return OK;
	}

	//------------------------------------------------------------------------
	Result Clock::stopTask()
	{
	
		timeSource.reset();
		
		for (uint32 i=0; i < observers.size(); i++){
			observers[i].reset();
		}
		observers.clear();
		
		Engine::sScriptEngine()->del("resetClock");
		
		return OK;
	}
	
	//------------------------------------------------------------------------
	Result Clock::updateTask(){

		// check whenever a time source is bound
		if (timeSource == NULL) return CLOCK_NO_TIME_SOURCE;

		timeSource->notifyNextFrame();
		
		// calculate exact frame duration time
		float64 exactFrameDuration = _getExactFrameDuration();
		_addToFrameHistory(exactFrameDuration);
		
		// get filtered frame time
		frameTime = _getFilteredFrameDuration();
		frameNumber ++;

		// check if we use frame bounder
		do {
			currentTime = timeSource->getTime();

			// get through the list of connected observers and notify them
			ObserverList::iterator it;
			for (it = observers.begin(); it != observers.end(); ++it){
				(*it)->notifyTimeObserver();
			}

			// check if we need to sync the clock
			if (syncInterval > 0 && nextSyncTime < currentTime)
			{
				timeSource->sync();
				nextSyncTime = currentTime + static_cast<float64>(syncInterval) / 1000.0;
			}

		} while (useMaxRate && (currentTime - lastCurrentTime) < invMaxFrameRate);

		// set the last current time we used to the current time
		lastCurrentTime = currentTime;
		
		// OK
		return OK;
	}

	//------------------------------------------------------------------------
	float64 Clock::getTime() const{
		return currentTime;
	}
	
	//------------------------------------------------------------------------
	float32 Clock::getFrameInterval() const{
		return (float32)frameTime;
	}
	
	//------------------------------------------------------------------------
	float32 Clock::getRealFrameInterval() const
	{
		return (float32)realFrameTime;
	}

	//------------------------------------------------------------------------
	float32 Clock::getRealFrameRate() const
	{
		return 1.0f / getRealFrameInterval();
	}

	//------------------------------------------------------------------------
	int32 Clock::getFrameNumber() const{
		return frameNumber;
	}
	
	//------------------------------------------------------------------------
	float32 Clock::getFrameRate() const{
		return 1.0f / getFrameInterval();
	}
		
	//------------------------------------------------------------------------
	int32 Clock::addObserver(SharedPtr<ITimeObserver> timeObserver){
		
		int32 id = 0;
		
		if (timeObserver != NULL){
			observers.push_back (timeObserver);
			id = observers.size();
			observerIDList[std::string(DEFAULT_OBSERVER_NAME) + ::boost::lexical_cast< std::string >(id)] = id;
			observers[observers.size()-1]->_observerID = id;
		}
		
		return id;
	}
	
	//------------------------------------------------------------------------
	Result Clock::removeObserver(int32 observerID){
		
		int32 id = observerID - 1;
		if (id <= 0 || id+1 >= static_cast<int32>(observers.size())
				|| observerIDList.find(std::string(DEFAULT_OBSERVER_NAME) + 
				::boost::lexical_cast< std::string >(observerID)) == observerIDList.end()){
			return CLOCK_OBSERVER_NOT_FOUND;
		}
		
		// remove it from the list
		observers.erase(observers.begin() + id);
		
		// also clean the map enty for this id
		observerIDList.erase(observerIDList.find( std::string(DEFAULT_OBSERVER_NAME) + 
				::boost::lexical_cast< std::string >(observerID) ));
				
		// OK
		return OK;
	}
		
	//------------------------------------------------------------------------
	Result Clock::addObserver(const std::string& obsName,SharedPtr<ITimeObserver> timeObserver)
	{
	
		// check whenever such an observer already exists
		if (observerIDList[obsName]){
			return CLOCK_OBSERVER_ALREADY_ADDED;
		}
		
		// add observer	
		if (timeObserver != NULL && obsName.length() > 0){
			observers.push_back (timeObserver);
			observerIDList[obsName] = observers.size(); 
			observers[observers.size()-1]->_observerID = observers.size();
		}
		
		return OK;
					
	}
	
	//------------------------------------------------------------------------
	Result Clock::removeObserver(const std::string& obsName){
	
		if (observerIDList[obsName]){
			int32 id = observerIDList[obsName] - 1;
			observers.erase(observers.begin() + id);
		}else{
			return CLOCK_OBSERVER_NOT_FOUND;
		}
		
		return OK;
	}
	
	//------------------------------------------------------------------------
	void Clock::setFrameWindow(int32 frameCount, float32 defaultFrameTime){
		
		frameFilteringWindow = frameCount > 1 ? frameCount : 1;
		frameDefaultTime = defaultFrameTime;
		frameDurationHistory.clear ();
		frameDurationHistory.push_back(frameDefaultTime);
	}
	
	//------------------------------------------------------------------------
	void Clock::setFixFrameRate(bool setFixRate, float32 fixFrameRate){
		useFixRate = setFixRate;
		this->fixFrameRate = fixFrameRate;
	}
		
	//------------------------------------------------------------------------
	void Clock::setMaxFrameRate(bool set, float32 maxFPS)
	{
		useMaxRate = set;
		this->maxFrameRate = maxFPS;
		invMaxFrameRate = 1.0 / (float64)(maxFPS);
	}

	//------------------------------------------------------------------------
	float64 Clock::_getExactFrameDuration (){
		
		
		float64 sourceTime = timeSource->getTime();
		float64 sourceRealTime = realTimeSource->getTime();
		
		realFrameTime = sourceRealTime - sourceLastRealValue;
		
		sourceLastValue = sourceTime;
		sourceLastRealValue = sourceRealTime;
		
		/*if (frameDuration > 0.200){
			frameDuration = frameDurationHistory.back();
		}else if (frameDuration < 0){
			frameDuration = 0;
		}*/

		if (useFixRate || timeSource == NULL){
			return 1.0f / (fixFrameRate);
		}

		return realFrameTime;
	}
	
	//------------------------------------------------------------------------
	void Clock::_addToFrameHistory (float64 exactFrameDuration){
		frameDurationHistory.push_back (exactFrameDuration);
		if (frameDurationHistory.size () > (uint32) frameFilteringWindow)
			frameDurationHistory.pop_front ();
	}
	
	//------------------------------------------------------------------------
	float64 Clock::_getFilteredFrameDuration () const{
		
		if (useFixRate){
			return 1.0f/(fixFrameRate);
		}
		
		float64 totalFrameTime = 0;
	
		std::deque<float64>::const_iterator it;
		for (it=frameDurationHistory.begin();it != frameDurationHistory.end(); ++it){
			totalFrameTime += *it;
		}
		
		return totalFrameTime/static_cast<float64>(frameDurationHistory.size());
	}
	
}; // end namespace

#undef DEFAULT_OBSERVER_NAME
