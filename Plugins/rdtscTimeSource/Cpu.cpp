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
#include "Cpu.h"

//----------------------------------------------------------------------------------
// Defines
// bit flags set by cpuid when called with register eax set to 1
//----------------------------------------------------------------------------------
#define MMX_SUPPORTED			(1 << 23)
#define SSE_SUPPORTED			(1 << 25)
#define SSE2_SUPPORTED		(1 << 26)
#define RDTSC_SUPPORTED		(1 << 4)

#define GET_EXTENDED_VALUES				0x80000000
#define EXTENDED_VALUES_SUPPORTED	0x80000001

// AMD specific
#define AMD_3DNOW_EXT_SUPPORTED	(1 << 30)
#define AMD_3DNOW_SUPPORTED			(1 << 31)
#define AMD_MMX_EX_SUPPORTED		(1 << 24)

//--------------------------------------------------------------------
ScriptFunctionDec(getCpuString, Cpu){

	Cpu* cpu = ScriptEngine::parameter_cast<Cpu*>(param[0]);

	return ScriptResult(cpu->m_strDescr);
}


//--------------------------------------------------------------------
Cpu::Cpu() : m_strVendor("Unkown"),m_strName("Unkown")
{
	m_nSpeed	= 0;
	m_fSpeedMhz  = 0.0f;
	m_bMMX		= false;
	m_bSSE		= false;
	m_bSSE2		= false;
	m_b3DNowEx= false;
	m_b3DNow	= false;
	m_bRDTSC  = false;
}

//--------------------------------------------------------------------
void Cpu::detect()
{
	// detection can only be done, if cpuid is supported
	if (!_cpuidSupported())
	{
		m_strDescr = "Can not retrieve CPU Information, because cpuid instruction is not supported";
		return;
	}

	// local variables;
	char szVendor[13];
	uint32 eax = 0;
	uint32 ebx = 0;
	uint32 edx = 0;
	uint32 unused = 0;

	// Get the CPU - Vendor name
	_cpuid(0, eax, ebx, unused, edx);
	*(uint32 *)(szVendor) = ebx;
	*(uint32 *)(szVendor + 4) = edx;
	*(uint32 *)(szVendor + 8) = unused;
	szVendor[12] = '\0';
	m_strVendor = szVendor;

	// now check for intel's extensions
	_cpuid(1, eax, ebx, unused, edx);
	m_bMMX      = ((edx & MMX_SUPPORTED) != 0);
	m_bSSE      = ((edx & SSE_SUPPORTED) != 0);
	m_bSSE2     = ((edx & SSE2_SUPPORTED) != 0);
	m_bRDTSC		= ((edx & RDTSC_SUPPORTED) != 0);

	// check for AMD Specific extensions
	_cpuid(GET_EXTENDED_VALUES, eax, ebx, unused, edx);
	if(eax >= EXTENDED_VALUES_SUPPORTED)
	{
		_cpuid(EXTENDED_VALUES_SUPPORTED, eax, ebx, unused, edx);
		m_b3DNow = ((edx & AMD_3DNOW_SUPPORTED) != 0);
		m_b3DNowEx = ((edx & AMD_3DNOW_EXT_SUPPORTED) != 0);
		m_bMMXEx = ((edx & AMD_MMX_EX_SUPPORTED) != 0);
	}

	// calculate the speed
	calculateSpeed();

	// Now generate a formatted description of the CPU
	char buffer[2056];
	sprintf(buffer, "CPU %s running at %f MHz, ", getVendor().c_str(), getSpeedMhz());
	m_strDescr = buffer;

	m_strDescr += "Features: ";
	if (isRDTSC()) m_strDescr += "rdtsc ";
	if (isMMX()) m_strDescr += "MMX ";
	if (isMMXEx()) m_strDescr += "MMXext ";
	if (isSSE()) m_strDescr += "SSE ";
	if (isSSE2()) m_strDescr += "SSE2 ";
	if (is3dNow()) m_strDescr += "3DNow! ";
	if (is3dNowEx()) m_strDescr += "3DNowExt! ";

}

//--------------------------------------------------------------------
void Cpu::calculateSpeed(uint32 calcTimeInMilliseconds)
{
	// Code is borrowed from SDL-Library - Thanks for That!
	uint64	nStart, nEnd;
	struct timeval tv_start, tv_end;
	int64 usec_delay;

	// get time through the rdtsc
	rdtsc(nStart);
	gettimeofday(&tv_start, NULL);
	nrEngine::sleep(calcTimeInMilliseconds);
	rdtsc(nEnd);
	gettimeofday(&tv_end, NULL);
	usec_delay = 1000000L * (tv_end.tv_sec - tv_start.tv_sec) +
													(tv_end.tv_usec - tv_start.tv_usec);

	// calculate the speed of the processor
	m_fSpeedMhz = static_cast<float32>((nEnd - nStart)) / static_cast<float32>(usec_delay);
	m_nSpeed = static_cast<uint64>(m_fSpeedMhz * 1000.0f);

}


//--------------------------------------------------------------------
void Cpu::_cpuid(uint32 function, uint32 &out_eax, uint32 &out_ebx, uint32 &out_ecx, uint32 &out_edx)
{
#if NR_PLATFORM == NR_PLATFORM_WIN32 && NR_COMPILER == NR_COMPILER_MSVC
	_asm
	{
		cpuid
		mov [out_eax], eax
		mov [out_ebx], ebx
		mov [out_ecx], ecx
		mov [out_edx], edx
	}
#else
	asm volatile(
	"						\
		push %%ebx;			\
		cpuid;				\
		mov %%ebx, %%esi;	\
		pop %%ebx			\
	"
		: 	"=a"(out_eax),
			"=r"(out_ebx),
			"=c"(out_ecx),
			"=d"(out_edx)
		: 	"a"(function));
#endif
}

//--------------------------------------------------------------------
bool Cpu::_cpuidSupported()
{
	try {
	#if NR_PLATFORM == NR_PLATFORM_WIN32 && NR_COMPILER == NR_COMPILER_MSVC
		_asm {
				xor eax, eax
				cpuid
		}
	#else
		asm volatile("xor %eax, %eax; cpuid;");
	#endif
	}catch (...) {
		return false;
	}

	return true;
}

//--------------------------------------------------------------------
void Cpu::rdtsc(uint64& ticks)
{
	// check whenever we got a rdtsc support
	if (!isRDTSC()){
		ticks = 0;
		return;
	}

	// 2x32 bit variables to hold the ticks
	uint32 tickLow, tickHigh;

	// get the ticks
	try{
	#if NR_PLATFORM == NR_PLATFORM_WIN32 && NR_COMPILER == NR_COMPILER_MSVC

		_asm {
				rdtsc
				mov [tickLow], eax
				mov [tickHigh], edx
		}
	#else
		asm volatile("rdtsc" : "=a"(tickLow), "=d"(tickHigh));
	#endif
	}catch(...){
		ticks = 0;
		m_bRDTSC = false;
	}

	// now combine the two values to produce a single 64 bit value
	ticks = 0;
	ticks |= tickHigh;
	ticks <<= 32;
	ticks |= tickLow;

}

