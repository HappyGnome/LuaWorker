
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

#include "LuaTestState.h"
#include "CppUnitTest.h"
#include<thread>
#include<iostream>

// Windows memory APIs
#include<processthreadsapi.h>
#include <psapi.h>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace Tests;

namespace Benchmarks
{
	TEST_CLASS(Benchmarks)
	{
	private:
		void DoBenchmark1(const char* stepTestString)
		{
			LuaTestState lua;
			HANDLE thisProc = GetCurrentProcess();
			PPROCESS_MEMORY_COUNTERS memCounters1, memCounters2;
			bool memDataOK;

			lua.DoTestFile("Common.lua");
			lua.DoTestFile("Benchmark1.lua");

			std::this_thread::sleep_for(0.5s);
			Assert::IsTrue(lua.DoTestString("return Step1()", 500ms), L"Step1");
			std::this_thread::sleep_for(1s);
			
			memDataOK = GetProcessMemoryInfo(thisProc, memCounters1, sizeof(memCounters1));

			std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock().now();
			Assert::IsTrue(lua.DoTestString(stepTestString, 3600s), L"Step2");
			std::chrono::high_resolution_clock::time_point end = std::chrono::high_resolution_clock().now();

			memDataOK = memDataOK && GetProcessMemoryInfo(thisProc, memCounters2, sizeof(memCounters2));

			std::stringstream ss;

			ss << "Elapsed: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << "ms";
			
			Logger::WriteMessage(ss.str().c_str());
			//TODO: log memory
		}


	public:
		
		TEST_METHOD(BenchmarkCoroutineNoData)
		{
			DoBenchmark1("return Step2NoData()");		
		}

		TEST_METHOD(BenchmarkCoroutineInt)
		{
			DoBenchmark1("return Step2Int()");		
		}
		
		TEST_METHOD(BenchmarkCoroutineString)
		{
			DoBenchmark1("return Step2String()");		
		}

		TEST_METHOD(BenchmarkCoroutineBool)
		{
			DoBenchmark1("return Step2Bool()");		
		}

		TEST_METHOD(BenchmarkCoroutineTable1)
		{
			DoBenchmark1("return Step2Table1()");		
		}

		TEST_METHOD(BenchmarkCoroutineTable2)
		{
			DoBenchmark1("return Step2Table2()");		
		}
	};
}
