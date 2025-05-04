

/*****************************************************************************\
*
*  Copyright 2025 HappyGnome
*
*  Licensed under the Apache License, Version 2.0 (the "License");
*  you may not use this file except in compliance with the License.
*  You may obtain a copy of the License at
*
*  http ://www.apache.org/licenses/LICENSE-2.0
*
*  Unless required by applicable law or agreed to in writing, software
*  distributed under the License is distributed on an "AS IS" BASIS,
*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*  See the License for the specific language governing permissions and
*  limitations under the License.
*
\*****************************************************************************/

#include "Benchmarks.h"
#include "LuaTestState.h"

#include<assert.h>
#include<thread>
#include<iostream>

// Windows memory APIs
#include<windows.h>
#include<processthreadsapi.h>
#include <psapi.h>


namespace Benchmarks
{
		void Benchmarks::DoBenchmark1(const char* stepTestString)
		{
			LuaTestState lua;
			HANDLE thisProc = GetCurrentProcess();
			PROCESS_MEMORY_COUNTERS memCounters1, memCounters2, memCounters3;
			bool memDataOK;

			lua.DoTestFile("Common.lua");
			lua.DoTestFile("Benchmark1.lua");

			std::this_thread::sleep_for(0.5s);
			if (!lua.DoTestString("return Step1()")) {
				std::cout << "Step1 failed"<<std::endl;
				return;
			}
			std::this_thread::sleep_for(1s);
			
			memDataOK = GetProcessMemoryInfo(thisProc, &memCounters1, sizeof(memCounters1));

			std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock().now();
			if (not lua.DoTestString(stepTestString))
			{
				std::cout << "Step2 failed"<<std::endl;
				return;

			}
			std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock().now();

			memDataOK = memDataOK && GetProcessMemoryInfo(thisProc, &memCounters2, sizeof(memCounters2));

			if (!lua.DoTestString("return Step3()"))
			{
				std::cout << "Step3.1 failed"<<std::endl;
			}
			lua.RunGcNow();
			if (!lua.DoTestString("return Step3()"))
			{
				std::cout << "Step3.2 failed"<<std::endl;
			}

			memDataOK = memDataOK && GetProcessMemoryInfo(thisProc, &memCounters3, sizeof(memCounters3));

			std::cout << "Elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms" << std::endl;
			if (memDataOK)
			{
				std::cout	<< "Mem Before: Pk Page:" << memCounters1.PeakPagefileUsage << " Peak WkSet:" << memCounters1.PeakWorkingSetSize << " Cur WkSet:" << memCounters1.WorkingSetSize<< std::endl;
				std::cout 	<< "Mem After : Pk Page:" << memCounters2.PeakPagefileUsage << " Peak WkSet:" << memCounters2.PeakWorkingSetSize << " Cur WkSet:" << memCounters2.WorkingSetSize<<std::endl;
				std::cout 	<< "Mem PostGC: Pk Page:" << memCounters3.PeakPagefileUsage << " Peak WkSet:" << memCounters3.PeakWorkingSetSize << " Cur WkSet:" << memCounters3.WorkingSetSize<<std::endl;
;
			}
			
		}


		
		void Benchmarks::BenchmarkCoroutineNoData( )
		{
			std::cout << "BenchmarkCoroutineNoData" << std::endl;
			DoBenchmark1("return Step2NoData()");		
		}

		void Benchmarks::BenchmarkCoroutineInt( )
		{
			std::cout << "BenchmarkCoroutineInt" << std::endl;
			DoBenchmark1("return Step2Int()");		
		}
		
		void Benchmarks::BenchmarkCoroutineString( )
		{
			std::cout << "BenchmarkCoroutineString" << std::endl;
			DoBenchmark1("return Step2String()");		
		}

		void Benchmarks::BenchmarkCoroutineBool( )
		{
			std::cout << "BenchmarkCoroutineBool" << std::endl;
			DoBenchmark1("return Step2Bool()");		
		}

		void Benchmarks::BenchmarkCoroutineTable1( )
		{
			std::cout << "BenchmarkCoroutineTable1" << std::endl;
			DoBenchmark1("return Step2Table1()");		
		}

		void Benchmarks::BenchmarkCoroutineTable2( )
		{
			std::cout << "BenchmarkCoroutineTable2" << std::endl;
			DoBenchmark1("return Step2Table2()");		
		}

		void Benchmarks::BenchmarkCoroutineTable2OneWay( )
		{
			std::cout << "BenchmarkCoroutineTable2OneWay" << std::endl;
			DoBenchmark1("return Step2Table2OneWay()");		
		}

		void Benchmarks::BenchmarkCoroutineStringOneWay( )
		{
			std::cout << "BenchmarkCoroutineStringOneWay" << std::endl;
			DoBenchmark1("return Step2StringOneWay()");		
		}
}
