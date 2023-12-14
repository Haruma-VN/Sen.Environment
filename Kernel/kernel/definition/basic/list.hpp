#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel {


	/**
	 * Array class: Only for dynamic array
	*/

	template <class T> 
	class List {
		public:

			// value

			std::vector<T> value;

			// constructor

			explicit List(
				const std::vector<T> & data
			)
			{
				thiz.value.insert(thiz.value.end(), data.begin(), data.end());
			}
			
			// constructor

			explicit List(
				const List &that
			) : List{that.value}
			{

			}

			// constructor

			List(
				std::initializer_list<T> iList
			) {
				for (const auto & e : iList) {
					thiz.value.push_back(e);
				}
			}

			// constructor

			List(

			) : value{}
			{

			}

			// destructor

			~List(

			) = default;

			/**
			 * size
			*/

			auto size(
				
			) -> size_t
			{
				return thiz.value.size();
			}

			/**
			 * fill
			*/

			auto fill(
				size_t count,
				const T &value
			) -> void
			{
				for(auto i : Range<size_t>(count)){
					thiz.value.push_back(value);
				}
				return;
			}

			/**
			 * add
			*/

			auto add(
				const T &value
			) -> void
			{
				thiz.value.push_back(value);
				return;
			}

			/**
			 * pop
			*/

			auto pop(

			) -> T
			{
				return thiz.value.pop_back();
			}

			/**
			 * insert
			*/

			auto insert(
				size_t location,
				const T &value
			) -> void
			{
				thiz.value.insert(thiz.value.begin() + location, value);
				return;
			}

			/**
			 * begin
			*/

			auto begin(

			) -> decltype(value.begin()) 
			{
				return thiz.value.begin();
			}

			/**
			 * end
			*/

			auto end(

			) -> decltype(value.end())
			{
				return thiz.value.end();
			}

			/**
			 * at
			*/

			auto at(
				size_t index
			) -> T&
			{
				return thiz.value.at(index);
			}

			/**
			 * index
			*/

			auto operator [](
				size_t index
			) -> T&
			{
				return thiz.at(index);
			}

			/**
			 * == operator
			*/

			auto operator ==(
				List & that
			) -> bool
			{
				if(thiz.size() != that.size())
				{
					return false;
				}
				for(auto i : Range<size_t>(thiz)){
					if(thiz[i] != that[i]){
						return false;
					}
				}
				return true;
			}

			/**
			 * != operator
			*/

			auto operator !=(
				List & that
			) -> bool
			{
				return !(thiz == that);
			}

			/**
			 * + operator
			*/

			auto operator +(
				List<T> & that
			) -> List<T>
			{
				auto list = std::vector<T>{};
				for(auto & c : thiz){
					list.push_back(c);
				}
				for(auto & c : that){
					list.push_back(c);
				}
				return List<T>{list};
			}

			/**
			 * clear
			*/

			auto clear(

			) -> void
			{
				thiz.value.clear();
				return;
			}

			/**
			 * erase
			*/

			auto erase(
				size_t index
			) -> void
			{
				thiz.value.erase(thiz.begin() + index);
			}

			/**
			 * shift
			*/

			auto shift(

			) -> T
			{
				auto c = thiz.at(0);
				thiz.erase(thiz.begin());
				return c;
			}
			
			/**
			 * unshift
			*/

			auto unshift(
				const T &value
			) -> void
			{
				thiz.insert(thiz.begin(), value);
				return;
			}

			/**
			 * reverse
			*/

			auto reverse(

			) -> void
			{
				std::reverse(thiz.begin(), thiz.end());
				return;
			}
			
			// for each
			
			auto forEach(
				std::function<void(T& e, size_t i)> method
			) -> void
			{
				for (auto i = 0; i < thiz.size(); ++i)
				{
					method(thiz[i], i);
				}
				return;
			}

			// for each

			auto forEach(
				std::function<void(T& e)> method
			) -> void
			{
				for(auto & c : thiz.value){
					method(c);
				}
				return;
			}

			// map

			template <typename P>
			auto map(
				std::function<P(T& e)> method
			) -> List<P>
			{
				auto arr = std::vector<P>{};
				for(auto & c : thiz){
					arr.push_back(method(c));
				}
				return List<P>{arr};
			}

			
			// map

			template <typename P>
			auto map(
				std::function<P(T& e, size_t index)> method
			) -> List<P>
			{
				auto arr = std::vector<P>{};
				for(auto i : Range<size_t>(thiz.value)){
					arr.push_back(method(thiz[i], i));
				}
				return List<P>{arr};
			}

			/**
			 * every
			*/

			auto every(
				std::function<bool(T& e)> method
			) -> bool
			{
				for(auto & c : thiz){
					if(!method(c)){
						return false;
					}
				}
				return true;
			}

			/**
			 * every
			*/

			auto every(
				std::function<bool(T& e, size_t index)> method
			) -> bool
			{
				for(auto & i : Range<size_t>(thiz)){
					if(!method(thiz[i], i)){
						return false;
					}
				}
				return true;
			}

			/**
			 * Sort method
			*/

			template <typename P>
			auto sort(
				std::function<P(T& a, T& b)> method
			) -> void
			{
				std::sort(thiz.begin(), thiz.end(), method);
				return;
			}

			/**
			 * some
			*/

			auto some(
				std::function<bool(T& e)> method
			) -> bool
			{
				for(auto & c : thiz){
					if(method(c)){
						return true;
					}
				}
				return false;
			}

			/**
			 * some
			*/

			auto some(
				std::function<bool(T& e, size_t index)> method
			) -> bool
			{
				for(auto i : Range<size_t>(thiz)){
					if(method(thiz[i], i)){
						return true;
					}
				}
				return false;
			}

			/**
			 * filter
			*/

			auto filter(
				std::function<bool(T& e)> method
			) -> List<T>
			{
				auto list = List<T>{};
				for(auto & c : thiz){
					if(method(c)){
						list.add(c);
					}
				}
				return list;
			}

			/**
			 * filter
			*/

			auto filter(
				std::function<bool(T& e, size_t index)> method
			) -> List<T>
			{
				auto list = List<T>{};
				for(auto i : Range<size_t>(thiz)){
					if(method(thiz[i], i)){
						list.add(thiz[i]);
					}
				}
				return list;
			}

	};
}