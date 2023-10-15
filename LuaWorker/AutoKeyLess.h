///*****************************************************************************\
//*
//*  Copyright 2023 HappyGnome
//*
//*  Licensed under the Apache License, Version 2.0 (the "License");
//*  you may not use this file except in compliance with the License.
//*  You may obtain a copy of the License at
//*
//*  http ://www.apache.org/licenses/LICENSE-2.0
//*
//*  Unless required by applicable law or agreed to in writing, software
//*  distributed under the License is distributed on an "AS IS" BASIS,
//*  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//*  See the License for the specific language governing permissions and
//*  limitations under the License.
//*
//\*****************************************************************************/
//
////
//#ifndef _AUTO_KEY_LESS_H_
//#define _AUTO_KEY_LESS_H_
//#pragma once
//
//#include<memory>
//
//namespace AutoKeyCollections
//{
//
//	/// <summary>
//	/// Like std::less, but for heterogeneous types
//	/// </summary>
//	/// <typeparam name="A"></typeparam>
//	/// <typeparam name="B"></typeparam>
//	template <typename A, typename B>
//	class less
//	{
//	public:
//		bool operator()(const A& a, const B& b)
//		{
//			return a < b;
//		}
//
//		/*bool operator()(const std::unique_ptr<A>& a, const B& b)
//		{
//			return a == nullptr || *a < b;
//		}
//
//		bool operator()(const B& b, const std::unique_ptr<A>& a)
//		{
//			return a != nullptr && b < *a;
//		}
//
//		bool operator()(const std::unique_ptr<A>& a, const std::unique_ptr<B>& b)
//		{
//			return a == nullptr || (b != nullptr && *a < *b);
//		}*/
//	};
//
//	/*/// <summary>
//	/// Like std::less, but for heterogeneous types
//	/// </summary>
//	/// <typeparam name="A"></typeparam>
//	/// <typeparam name="B"></typeparam>
//	template <typename A, typename B>
//	class less_deref
//	{
//	public:
//		bool operator()(const A& a, const B& b)
//		{
//			return *a < b;
//		}
//
//		bool operator()(const B& b, const A& a)
//		{
//			return b < *a;
//		}
//	};*/
//};
//#endif