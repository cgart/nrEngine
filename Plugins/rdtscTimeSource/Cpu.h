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

#ifndef __NR_ENGINE_CPU_H_
#define __NR_ENGINE_CPU_H_

//----------------------------------------------------------------------------------
// Includes
//----------------------------------------------------------------------------------
#include <nrEngine/nrEngine.h>

using namespace nrEngine;

/**
* Cpu class represents a CPU found in the system where the engine
* is running. It can be used to get inforamtion about CPU features.
* It is also needed to setup high performance counter.
*
**/
class Cpu{
	public:
		/**
		* Standard constructor, setting up the cpu features to unknown
		**/
		Cpu();

		/**
		* Detect all information about the used CPU.
		* THe infromation about the CPU will be getted from cpuid
		* assembler function. This will full registers with cpu flags
		* which then will be readed and transformed into readable text.
		**/
		void detect();


		/**
		* Calculate the CPU-Speed through high performance timers.
		* Because the calculation is done through the rdtsc timer you are not able
		* to retrive the cpu speed information if such instruction is not supported
		* by your CPU.<br>
		* Because the calculation need some time to take, you can specify the parameter
		* here. Longer time calculation produce more representative results.<br>
		* You need to recalculate the speed if you using a CPU which does support
		* freqeuncy stepping. (on some motherboards it can also be done by FSB stepping).
		* So to get the timers working fine also by changing the frequency, you have
		* to poll for right speed sometimes.
		**/
		void calculateSpeed(uint32 calcTimeInMilliseconds = 100);

		/**
		* Run the rdtsc assembler instruction and return the tick value
		* to the given variable back.
		* If such an instruction is not supported, 0 will be given back
		**/
		void rdtsc(uint64& ticks);

		/**
		* Return the name of the Vendor of the CPU
		**/
		NR_FORCEINLINE const std::string& getVendor() const	{ return m_strVendor; }

		/**
		* Get the Name of the CPU i.e. K7, 486i, ...
		**/
		NR_FORCEINLINE const std::string& getName() const	{ return m_strName; }

		/**
		* Generate a string containing all data about the CPU
		**/
		NR_FORCEINLINE const std::string& getCPUDescription() const { return m_strDescr; }

		/**
		* Get  the speed in Hz how fast the CPU is
		**/
		NR_FORCEINLINE uint64 getSpeed()					{ return m_nSpeed; }

		/**
		* Get the speed in Mhz of the CPU
		**/
		NR_FORCEINLINE float32 getSpeedMhz()				{ return m_fSpeedMhz; }

		//! Is the high performance tick counter supported
		NR_FORCEINLINE bool isRDTSC()	{return m_bRDTSC;}

		//! Is MMX supported
		NR_FORCEINLINE bool isMMX()	{return m_bMMX; }

		//! Is MMX-Ex supported
		NR_FORCEINLINE bool isMMXEx()	{return m_bMMXEx; }

		//! Are SSE Extensions supported
		NR_FORCEINLINE bool isSSE()	{ return m_bSSE; }

		//! True if CPU supports SSE2 Extensions
		NR_FORCEINLINE bool isSSE2()	{ return m_bSSE2; }

		//! If the CPU support 3DNow! Extension, return true
		NR_FORCEINLINE bool is3dNow()	{ return m_b3DNow; }

		//! Extended 3DNow! Extensions support
		NR_FORCEINLINE bool is3dNowEx(){ return m_b3DNowEx; }

	private:

		/** CPUID Wrapper Funktion **/
		void _cpuid(uint32 function,
					uint32 &out_eax,
					uint32 &out_ebx,
					uint32 &out_ecx,
					uint32 &out_edx);

		/** Check whenever CPUID - Assembler feature is supported **/
		bool _cpuidSupported();

		//! CPU Vendor
		std::string             m_strVendor;

		//! CPU Name
		std::string             m_strName;

		//! Formatted description of the CPU
		std::string				m_strDescr;

		//! Speed of CPU in Hz
		uint64	m_nSpeed;

		//! Speed of CPU in Mhz
		float32	m_fSpeedMhz;

		//! High performance real timer support
		bool	m_bRDTSC;

		//! MMX support
		bool	m_bMMX;

		//! Extended MMX support
		bool	m_bMMXEx;

		//! Streaming SIMD Extensions
		bool	m_bSSE;

		//! Streaming SIMD Extensions 2
		bool	m_bSSE2;

		//! 3DNow! (AMD specific extensions)
		bool	m_b3DNowEx;

		//! 3DNow! (vendor independant)
		bool	m_b3DNow;

		//! Provide the scripts with information
		ScriptFunctionDef(getCpuString);

};

#endif

