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
#include <nrEngine/Engine.h>
#include <nrEngine/Profiler.h>
#include <nrEngine/Exception.h>
#include <nrEngine/TimeSource.h>

namespace nrEngine{

	//--------------------------------------------------------------------
	Profile::Profile(const std::string& name, bool isSystemProfile) : mName(name), mSystemProfile(isSystemProfile)
	{
		Engine::sProfiler()->beginProfile(mName, isSystemProfile);
	}
	
	//--------------------------------------------------------------------
	Profile::~Profile()
	{
		Engine::sProfiler()->endProfile(mName, mSystemProfile);
	}
	
	
	
	//--------------------------------------------------------------------
	Profiler::Profiler(SharedPtr<TimeSource> timeSource)
	{
		mTimeSource = timeSource;
		mTotalFrameTime = 0;
		mEnabled = mNewEnableState = true; // the profiler starts out as enabled
		mEnableStateChangePending = false;
		mEngineProfileEnabled = false;
	}
	
	//--------------------------------------------------------------------
	Profiler::~Profiler()
	{
		// clear all our lists
		mProfiles.clear();
		mProfileFrame.clear();
		mProfileHistoryMap.clear();
		mProfileHistory.clear();
	}	
	
	//--------------------------------------------------------------------
	void Profiler::beginProfile(const std::string& profileName, bool isSystemProfile) {
	
		// if the profiler is enabled
		if (!mEnabled || (isSystemProfile && !mEngineProfileEnabled))
			return;
	
		// need a timer to profile!
		NR_ASSERT (mTimeSource && "Time Source is not set!");

		// empty string is reserved for the root
		NR_ASSERT ((profileName != "") && ("Profile name can't be an empty string"));
	
		// make sure this profile isn't being used more than once
		/*ProfileStack::iterator iter;
		for (iter = mProfiles.begin(); iter != mProfiles.end(); iter++)
			if ((*iter).name == profileName)	
				break;
		*/
				
		//NR_ASSERT ((iter == mProfiles.end()) && ("This profile name is already being used"));

		// profile instance
		ProfileInstance p;
	
		// this is the root, it has no parent
		if (mProfiles.empty()) {
	
			p.parent = "";
	
		// otherwise peek at the stack and use the top as the parent
		} else {
	
			ProfileInstance parent = mProfiles.back();
			p.parent = parent.name;
	
		}
		
		// some iterator variables
		ProfileFrameList::iterator fIter;
		ProfileHistoryList::iterator hIter;
	
		// we check to see if this profile has been called in the frame before
		for (fIter = mProfileFrame.begin(); fIter != mProfileFrame.end(); fIter++)
			if ((*fIter).name == profileName)
				break;
		
		// if it hasn't been called before, set its position in the stack
		if (fIter == mProfileFrame.end()) {
			ProfileFrame f;
			f.name = profileName;
			f.frameTime = 0;
			f.frameTotalTime = 0;
			f.calls = 0;
			f.hierarchicalLvl = (uint32) mProfiles.size();
			mProfileFrame.push_back(f);
		}
	
		// we check to see if this profile has been called in the app before
		ProfileHistoryMap::iterator histMapIter;
		histMapIter = mProfileHistoryMap.find(profileName);
	
		// if not we add a profile with just the name into the history
		if (histMapIter == mProfileHistoryMap.end()) {
			ProfileHistory h;
			h.name = profileName;
			h.numCallsThisFrame = 0;
			h.totalTime = 0;
			h.totalCalls = 0;
			h.maxTime = 0;
			h.minTime = 1;
			h.hierarchicalLvl = p.hierarchicalLvl;
			h.currentTime = 0;
			h.realTotalTime = 0;
			h.realTime = 0;
			h.isSystemProfile = isSystemProfile;
			
			// we add this to the history
			hIter = mProfileHistory.insert(mProfileHistory.end(), h);
	
			// for quick look-ups, we'll add it to the history map as well
			mProfileHistoryMap.insert(std::pair< std::string, ProfileHistoryList::iterator>(profileName, hIter));
		}
	
		// add the stats to this profile and push it on the stack
		// we do this at the very end of the function to get the most
		// accurate timing results
		p.name = profileName;
		p.isSystemProfile = isSystemProfile;
		p.currTime = mTimeSource->getTime();
		p.accum = 0;
		p.hierarchicalLvl = (uint32) mProfiles.size();
		mProfiles.push_back(p);
	}
	

	//--------------------------------------------------------------------
	void Profiler::endProfile(const std::string& profileName, bool isSystemProfile) {
	
		// if the profiler is enabled
		if (!mEnabled || (isSystemProfile && !mEngineProfileEnabled))
			return;
		
		// need a timer to profile!
		NR_ASSERT (mTimeSource && "Timer not set!");
	
		// get the end time of this profile
		// we do this as close the beginning of this function as possible
		// to get more accurate timing results
		float64 endTime = mTimeSource->getTime();
	
		// empty string is reserved for designating an empty parent
		NR_ASSERT ((profileName != "") && ("Profile name can't be an empty string"));
		
		// stack shouldnt be empty
		NR_ASSERT (!mProfiles.empty() && ("You have to begin any profile before stop it"));
	
		// get the start of this profile
		ProfileInstance bProfile;
		bProfile = mProfiles.back();
		mProfiles.pop_back();
	
		// calculate the elapsed time of this profile
		float64 timeElapsed = endTime - bProfile.currTime;
	
		// update parent's accumalator if it isn't the root
		if (bProfile.parent != "") {
			// find the parent
			ProfileStack::iterator iter;
			for(iter = mProfiles.begin(); iter != mProfiles.end(); iter++)	
				if ((*iter).name == bProfile.parent)
					break;
	
			// the parent should be found 
			NR_ASSERT(iter != mProfiles.end());
	
			// add this profile's time to the parent's accumulator
			(*iter).accum += timeElapsed;	
		}
	
		// we find the profile in this frame
		ProfileFrameList::iterator iter;
		for (iter = mProfileFrame.begin(); iter != mProfileFrame.end(); iter++)
			if ((*iter).name == bProfile.name)
				break;
	
		// we subtract the time the children profiles took from this profile
		(*iter).frameTime += timeElapsed - bProfile.accum;
		(*iter).frameTotalTime += timeElapsed;
		(*iter).calls++;
	
		// the stack is empty and all the profiles have been completed
		// we have reached the end of the frame so process the frame statistics
		if (mProfiles.empty()) {
	
			// we know that the time elapsed of the main loop is the total time the frame took
			mTotalFrameTime = timeElapsed;
	
			// we got all the information we need, so process the profiles
			// for this frame
			processFrameStats();
	
			// clear the frame stats for next frame
			mProfileFrame.clear();
		
			// if the profiler received a request to be enabled or disabled
			// we reached the end of the frame so we can safely do this
			if (mEnableStateChangePending) {
				changeEnableState();
			}
	
		}
	
	}
	
	//--------------------------------------------------------------------
	void Profiler::processFrameStats() {
	
		// if the profiler is enabled
		if (!mEnabled)
			return;

		ProfileFrameList::iterator frameIter;
		ProfileHistoryList::iterator historyIter;
	
		// we set the number of times each profile was called per frame to 0
		// because not all profiles are called every frame
		for (historyIter = mProfileHistory.begin(); historyIter != mProfileHistory.end(); historyIter++)
			(*historyIter).numCallsThisFrame = 0;
	
		// iterate through each of the profiles processed during this frame
		for (frameIter = mProfileFrame.begin(); frameIter != mProfileFrame.end(); frameIter++) {
	
			std::string s = (*frameIter).name;
	
			// use our map to find the appropriate profile in the history
			historyIter = (*mProfileHistoryMap.find(s)).second;
	
			// extract the frame stats
			float64 frameTime = (*frameIter).frameTime;
			uint32 calls = (*frameIter).calls;
			uint32 lvl = (*frameIter).hierarchicalLvl;
	
			// calculate what percentage of frame time this profile took
			float32 framePercentage = static_cast<float32>(frameTime) / static_cast<float32>(mTotalFrameTime);
	
			// update the profile stats
			(*historyIter).currentTime = framePercentage;
			(*historyIter).totalTime += framePercentage;
			(*historyIter).totalCalls++;
			(*historyIter).numCallsThisFrame = calls;
			(*historyIter).hierarchicalLvl = lvl;
			(*historyIter).realTime += frameTime;
			(*historyIter).realTotalTime += (*frameIter).frameTotalTime;
			
			// if we find a new minimum for this profile, update it
			if ((framePercentage) < ((*historyIter).minTime)) {	
				(*historyIter).minTime = framePercentage;
			}
	
			// if we find a new maximum for this profile, update it
			if ((framePercentage) > ((*historyIter).maxTime)) {	
				(*historyIter).maxTime = framePercentage;
			}
	
		}
	
	}

	//-----------------------------------------------------------------------
	void Profiler::setEnable(bool enable)
	{

		// check whenever we get new state
		if (mEnabled != enable){
			mEnableStateChangePending = true;
			mNewEnableState = enable;		
		}
		
	}
	
	//-----------------------------------------------------------------------
	void Profiler::reset() {
	
		ProfileHistoryList::iterator iter;
		for (iter = mProfileHistory.begin(); iter != mProfileHistory.end(); iter++) {
			(*iter).currentTime = (*iter).maxTime = (*iter).totalTime = 0;
			(*iter).numCallsThisFrame = (*iter).totalCalls = 0;
			(*iter).minTime = 1;	
		}
	
	}

	//--------------------------------------------------------------------
	void Profiler::logResults(Log::LogTarget lt){

		// if the profiler is enabled
		if (!mEnabled)
			return;
		
		// sort the history so we get profiles that need more time on the top
		mProfileHistory.sort();
				
		ProfileHistoryList::iterator iter;
	
		NR_Log(lt,  "--------------------------------------Profiler Results------------------------------");
		NR_Log(lt,  "| Name                        | Avg(\%) | Max(\%) | Min(\%) | Time sec     |  Calls   |");
		NR_Log(lt,  "------------------------------------------------------------------------------------");

		// log system results
		NR_Log(lt,  "| System Profiles                                                                  |");
		for (iter = mProfileHistory.begin(); iter != mProfileHistory.end(); iter++)
		{
			// only for system profiles
			if (iter->isSystemProfile){
				// create an indent that represents the hierarchical order of the profile
				std::string indent = "   ";
				//for (uint32 i = 0; i < (*iter).hierarchicalLvl; i++)
				//	indent = indent + " --";
				indent += (*iter).name;
				indent.resize(27, ' ');
	
				logLine(indent.c_str(), iter->minTime, iter->maxTime, iter->realTime, iter->totalTime, iter->totalCalls);
			}
		}
		
		// log application results
		NR_Log(lt,  "|----------------------------------------------------------------------------------|");
		NR_Log(lt,  "| Application Profiles                                                             |");
		for (iter = mProfileHistory.begin(); iter != mProfileHistory.end(); iter++)
		{
			// only for application profiles
			if (!iter->isSystemProfile){
				// create an indent that represents the hierarchical order of the profile
				std::string indent = "   ";
				//for (uint32 i = 0; i < (*iter).hierarchicalLvl; i++)
				//	indent = indent + " --";
				indent += (*iter).name;
				indent.resize(27, ' ');
	
				logLine(indent.c_str(), iter->minTime, iter->maxTime, iter->realTime, iter->totalTime, iter->totalCalls);
			}
		}
	
		NR_Log(lt,  "|                                                                                  |");
		NR_Log(lt,  "------------------------------------------------------------------------------------");

	}

	//-----------------------------------------------------------------------
	void Profiler::logLine(const char* name, float32 min, float32 max, float32 frameTime, float32 totalTime, uint32 totalCalls)
	{
		// create some strings containing the statistics
		char avg[32];
		sprintf(avg, "%2.2f", (totalTime / totalCalls) * 100.0f);

		char smin[32];
		sprintf(smin, "%2.2f", min * 100.0f);

		char smax[32];
		sprintf(smax, "%2.2f", max * 100.0f);

		char tim[32];
		sprintf(tim, "%4.6f", frameTime);

		char cls[32];
		sprintf(cls, "%d", totalCalls);

		char ttm[32];
		sprintf(ttm, "%4.6f", totalTime);
		
		NR_Log(Log::LOG_APP, "| %s |  %-6s|  %-6s|  %-6s| %-13s| %-9s|", name, avg, smax, smin, tim, cls);
	}
	

	//-----------------------------------------------------------------------
	void Profiler::changeEnableState() {	
		mEnabled = mNewEnableState;
		mEnableStateChangePending = false;
	}

	
	//-----------------------------------------------------------------------
	bool Profiler::ProfileHistory::operator <(const Profiler::ProfileHistory& p)
	{
		float my_avg = totalTime / totalCalls;
		float it_avg = p.totalTime / p.totalCalls;
		return (it_avg < my_avg);
	}

}; // end namespace

