
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

#ifndef _BENCHMARKS_H_
#define _BENCHMARKS_H_
#pragma once

namespace Benchmarks
{
	class Benchmarks
	{
	private:
		void DoBenchmark1(const char* stepTestString);
		

	public:
		
		void BenchmarkCoroutineNoData( );

		void BenchmarkCoroutineInt( );
		
		void BenchmarkCoroutineString( );

		void BenchmarkCoroutineBool( );

		void BenchmarkCoroutineTable1( );

		void BenchmarkCoroutineTable2( );

		void BenchmarkCoroutineTable2OneWay();

		void BenchmarkCoroutineStringOneWay();

	};
}

#endif
