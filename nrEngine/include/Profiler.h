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

/*
 * The code is borrowed from Ogre-Engine.
 * Many of the ideas for the profiler were borrowed from 
 * "Real-Time In-Game Profiling" by Steve Rabin which can be found in Game Programming
 * Gems 1.
 */

#ifndef __NR_ENGINE_PROFILER_H_
#define __NR_ENGINE_PROFILER_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include "Prerequisities.h"
#include "Log.h"

//----------------------------------------------------------------------------------
// Defines
//
// If you want to profile your application, so use these macros and define NR_PROFILING
//----------------------------------------------------------------------------------
#ifdef NR_APP_PROFILING
#   define NR_Profile( a )      ::nrEngine::Profile __nr_profile_instance( (a), false )
#   define NR_ProfileBegin( a ) ::nrEngine::Engine::sProfiler()->beginProfile( (a), false )
#   define NR_ProfileEnd( a )   ::nrEngine::Engine::sProfiler()->endProfile( (a), false )
#else
#   define NR_Profile( a )
#   define NR_ProfileBegin( a )
#   define NR_ProfileEnd( a )
#endif



//----------------------------------------------------------------------------------
// This macros is used to profile the engine and not the application
// Actually it is the same macro but here we separate the engine profiling from
// application profiling, so the user can either enable or disable one of them
//----------------------------------------------------------------------------------
#ifdef NR_ENGINE_PROFILING
#   define _nrEngineProfile( a )      Profile __nr_profile_instance( (a), true )
#   define _nrEngineProfileBegin( a ) Engine::sProfiler()->beginProfile( (a), true )
#   define _nrEngineProfileEnd( a )   Engine::sProfiler()->endProfile( (a), true )
#else
#   define _nrEngineProfile( a )
#   define _nrEngineProfileBegin( a )
#   define _nrEngineProfileEnd( a )
#endif


namespace nrEngine{

	//! Single profile processed by the profiler
	/**
	 * Our profiler does store such kind of profiles and manage them.
	 * Use macro NR_Profile(name)  instead of instantiating this objects
	 * directly. <br>
	 * Using of such kind of macros does help to profile a single scope:
	 * i.e.:
	 * <code>
	 * {
	 * 		NR_Profile("Scope");
	 * 		....
	 * }
	 * </code>
	 * \ingroup gp
	 **/
	class _NRExport Profile {
		public:
			
			/**
			 * Create an instance of this class and start profiling for this profile.
			 **/
			Profile(const std::string& name, bool isSystemProfile = false);
			
			/**
			 * Release used memory and stop the profiler for this profile.
			 **/
			~Profile();
			
		private:
			
			//! Name of the profile, must be unique
			std::string mName;

			//! Is this is a system profile
			bool mSystemProfile;
			
	};
	
	
	//! The profiler allows you to measure the performance of your code
	/**
	 * Profiler is a singleton object which is created by 
	 * initializing the engine. Profiler does use a time source provide
	 * system time to allow to measure the time differencies.<br>
	 * 
	 * Profiling is done stack wise. It means every time you starting
	 * a profile it will go onto the stack. Every time the profile is ending
	 * top most profile from the stack is removed and statistics is computed for it.
	 * Every time the stack is empty we compute the whole statistics for each profiles.
	 * The time in that the stack is not empty is called frame. We assume that you always
	 * profiles framewise.
	 * 
	 * \ingroup gp
	 **/
	class _NRExport Profiler {
		public:

			/**
			 * Begin of profiling. Please use NR_ProfileBegin(name) macro
			 * instead of this function, so it can be removed in the release
			 * version of you application
			 *
			 * \param name Unique name of the profile
			 * \param isSystemProfile Define if the profile is build for the engine
			 **/
			void beginProfile(const std::string& name, bool isSystemProfile = false);
			 
			/**
			 * End a profiling for the given profiling scope/name
			 * Please use NR_ProfileEnd(name) macro instead of directly
			 * calling this function.
			 * 
			 * \param name Unique name of the profile
			 * \param isSystemProfile Set to true if the profile is created for the engine
			 **/
			void endProfile(const std::string& name, bool isSystemProfile = false);
			
			/**
			 * Reset the profiler, so we clear all currently using
			 * profilers.
			 **/
			void reset();
			
			/**
			 * Log current profile statistics to the log
			 **/		
			void logResults(Log::LogTarget lt = Log::LOG_APP);
					
			/**
			 * Enable the profiling
			 **/
			NR_FORCEINLINE void enable() { setEnable ( true );  }
			
			/**
			 * Disable profiling
			 **/
			NR_FORCEINLINE void disable(){ setEnable ( false ); }
			
			/**
			 * Is profiler enabled
			 **/
			NR_FORCEINLINE bool isEnabled() { return mEnabled; }
			
			/**
			 * Set profiler enable/disable state
			 **/
			void setEnable(bool enable);
				 
			/**
			 * Enable or disable profiling of the system components.
			 * If you enable this all engine components calling the profiler
			 * will be counted in the statistics. Otherwise the engine
			 * does not produce any profile information
			 **/
			NR_FORCEINLINE void setEnableEngineProfiling(bool e) { mEngineProfileEnabled = e;}

			/**
			 * Get information about engine profiling status. If true, so engine's
			 * components are getting profiled. Otherwise no.
			 **/
			NR_FORCEINLINE bool isEnabledEngineProfiling() const { return mEngineProfileEnabled; }
			
		private:
  				  
			//! Only engine can create instance of profiler
			friend class Engine;

			/**
			 * Create a profiler object and use the given time source
			 * to retrive the time. Engine will create the object
			 * by itself and will specify a system time source object
			 * instance for profiler using
			 **/
			Profiler(SharedPtr<TimeSource> timeSource);

			/**
			 * Virtual destructor of the profiler object that allows you
			 * to derive your own profiler classes to overwrite some
			 * methods of the defulat one
			 **/
			~Profiler();

			//! Time source used to retrieve time
			SharedPtr<TimeSource>		mTimeSource;

			
			//! Represents an individual profile call
			struct ProfileInstance {
	
				//! The name of the profile
				std::string		name;
	
				//! The name of the parent, empty string if root
				std::string		parent;

				//! Define if the profile is a system profile
				bool				isSystemProfile;
				
				//! The time this profile was started
				float64				currTime;
	
				//! Represents the total time of all child profiles to subtract from this profile
				float64				accum;
	
				/// The hierarchical level of this profile, 0 being the root profile
				uint32				hierarchicalLvl;
			};
			
			//! Represents the total timing information of a profile since profiles can be called more than once each frame
			struct ProfileFrame {
				
				//! The name of the profile
				std::string	name;
			
				//! The time this profile has taken this frame
				float64			frameTime;
			
				//! The number of times this profile was called this frame
				uint32			calls;
			
				//! The hierarchical level of this profile, 0 being the main loop
				uint32			hierarchicalLvl;
			
				//! The total time incl. children this profile has taken this frame
				float64			frameTotalTime;
				
			};
			
			//! Represents a history of each profile during the duration of the app
			struct ProfileHistory {
	
				//! The name of the profile
				std::string	name;

				//! is system profile
				bool isSystemProfile;
				
				//! The current percentage of frame time this profile has taken
				float32	currentTime; // %
	
				//! The maximum percentage of frame time this profile has taken
				float32	maxTime; // %
	
				//! The minimum percentage of frame time this profile has taken
				float32	minTime; // %
	
				//! The number of times this profile has been called this frame
				uint32	numCallsThisFrame;
	
				//! The total percentage of frame time this profile has taken (used to calculate average)
				float32	totalTime; // %
	
				//! The total number of times this profile was called (used to calculate average)
				uint32	totalCalls;
	
				//! The hierarchical level of this profile, 0 being the root profile
				uint32	hierarchicalLvl;
	
				//! Time of the profile has taken for running
				float32 realTime;
				
				//! Total time incl. children of the profile
				float32 realTotalTime;
				
				//! Giving order on the profiles, to allows sorting
				bool operator <(const ProfileHistory& p);
				
			};
				
			
			typedef std::list<ProfileInstance>		ProfileStack;
			typedef std::list<ProfileFrame>			ProfileFrameList;
			typedef std::list<ProfileHistory>			ProfileHistoryList;
			typedef std::map< std::string, ProfileHistoryList::iterator> ProfileHistoryMap;
			typedef std::map< std::string, bool>		DisabledProfileMap;

			//! A stack for each individual profile per frame
			ProfileStack mProfiles;
	
			//! Accumulates the results of each profile per frame (since a profile can be called more than once a frame)
			ProfileFrameList mProfileFrame;
	
			//! Keeps track of the statistics of each profile
			ProfileHistoryList mProfileHistory;
	
			//! We use this for quick look-ups of profiles in the history list
			ProfileHistoryMap mProfileHistoryMap;

			//! The total time each frame takes
			float64 mTotalFrameTime;

			//! Wether we should profile the engine subsystem
			bool mEngineProfileEnabled;
			
			//! Whether this profiler is enabled
			bool mEnabled;
	
			//! Keeps track of whether this profiler has received a request to be enabled/disabled
			bool mEnableStateChangePending;
	
			//! Keeps track of the new enabled/disabled state that the user has requested which will be applied after the frame ends
			bool mNewEnableState;

			void processFrameStats();
			void changeEnableState();

			void logLine(const char* name, float32 min, float32 max, float32 frameTime, float32 totalTime, uint32 totalCalls);

	};

}; // end namespace

#endif

