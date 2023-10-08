/*****************************************************************************\
*
*  Copyright 2023 HappyGnome
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

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests
{
	TEST_CLASS(LuaTests)
	{
	public:
		
		TEST_METHOD(WorkerStatus)
		{
			LuaTestState lua;

			lua.DoTestFile("Common.lua");
			lua.DoTestFile("WorkerStatus.lua");

			Assert::IsTrue(lua.DoTestString("return Step1()"), L"Step1");
			Assert::IsTrue(lua.DoTestString("return Step2()"), L"Step2");
			std::this_thread::sleep_for(1s);
			Assert::IsTrue(lua.DoTestString("return Step3()"), L"Step3");
			Assert::IsTrue(lua.DoTestString("return Step4()"), L"Step4");
		}

		TEST_METHOD(TaskSleep)
		{
			LuaTestState lua;

			lua.DoTestFile("Common.lua");
			lua.DoTestFile("TaskSleep.lua");

			std::this_thread::sleep_for(0.5s);
			Assert::IsTrue(lua.DoTestString("return Step1()"), L"Step1");
			Assert::IsTrue(lua.DoTestString("return Step2()"), L"Step2");
			std::this_thread::sleep_for(1s);
			Assert::IsTrue(lua.DoTestString("return Step3()"), L"Step3");
			Assert::IsTrue(lua.DoTestString("return Step4()"), L"Step4");
			Assert::IsTrue(lua.DoTestString("return Step5()"), L"Step5");
		}

		TEST_METHOD(TaskError)
		{
			LuaTestState lua;

			lua.DoTestFile("Common.lua");
			lua.DoTestFile("TaskError.lua");

			std::this_thread::sleep_for(0.5s);
			Assert::IsTrue(lua.DoTestString("return Step1()"), L"Step1");
			Assert::IsTrue(lua.DoTestString("return Step2()"), L"Step2");
			Assert::IsTrue(lua.DoTestString("return Step3()"), L"Step3");
			Assert::IsFalse(lua.DoTestString("return Step4()"), L"Step4");
			Assert::IsTrue(lua.DoTestString("return Step5()",1.5s,0.5s), L"Step5");
		}

		TEST_METHOD(TaskDoString)
		{
			LuaTestState lua;

			lua.DoTestFile("Common.lua");
			lua.DoTestFile("TaskDoString.lua");

			std::this_thread::sleep_for(0.5s);
			Assert::IsTrue(lua.DoTestString("return Step1()"), L"Step1");
			Assert::IsTrue(lua.DoTestString("return Step2()"), L"Step2");
			Assert::IsTrue(lua.DoTestString("return Step3()", 500ms), L"Step3");
		}

		TEST_METHOD(TaskDoFile)
		{
			LuaTestState lua;

			lua.DoTestFile("Common.lua");
			lua.DoTestFile("TaskDoFile.lua");

			std::this_thread::sleep_for(0.5s);
			Assert::IsTrue(lua.DoTestString("return Step1()", 500ms), L"Step1");
			Assert::IsTrue(lua.DoTestString("return Step2()", 500ms), L"Step2");
			Assert::IsTrue(lua.DoTestString("return Step3()", 500ms), L"Step3");
			Assert::IsTrue(lua.DoTestString("return Step4()", 500ms), L"Step4");
		}

		TEST_METHOD(TaskInfiniteLoop)
		{
			LuaTestState lua;

			lua.DoTestFile("Common.lua");
			lua.DoTestFile("TaskInfiniteLoop.lua");

			std::this_thread::sleep_for(0.5s);
			Assert::IsTrue(lua.DoTestString("return Step1()", 500ms), L"Step1");
			Assert::IsTrue(lua.DoTestString("return Step2()", 1000ms), L"Step2");
			Assert::IsFalse(lua.DoTestString("return Step3()", 500ms), L"Step3");
			Assert::IsTrue(lua.DoTestString("return Step4()", 500ms), L"Step4");
			Assert::IsTrue(lua.DoTestString("return Step5()", 500ms), L"Step5");
		}

		TEST_METHOD(TaskInfiniteCCalls)
		{
			LuaTestState lua;

			lua.DoTestFile("Common.lua");
			lua.DoTestFile("TaskInfiniteCCalls.lua");

			std::this_thread::sleep_for(0.5s);
			Assert::IsTrue(lua.DoTestString("return Step1()", 500ms), L"Step1");
			Assert::IsTrue(lua.DoTestString("return Step2()", 2500ms,2000ms), L"Step2");
			Assert::IsFalse(lua.DoTestString("return Step3()", 2500ms), L"Step3");
			Assert::IsTrue(lua.DoTestString("return Step4()", 500ms), L"Step4");
		}

		
		TEST_METHOD(WorkerCustomErrorStack)
		{
			LuaTestState lua;

			lua.DoTestFile("Common.lua");
			lua.DoTestFile("WorkerCustomErrorStack.lua");

			std::this_thread::sleep_for(0.5s);
			Assert::IsTrue(lua.DoTestString("return Step1()", 100ms), L"Step1");
			Assert::IsFalse(lua.DoTestString("return Step2()", 100ms), L"Step2");
			Assert::IsTrue(lua.DoTestString("return Step3()", 100ms), L"Step3");
		}
	};
}
