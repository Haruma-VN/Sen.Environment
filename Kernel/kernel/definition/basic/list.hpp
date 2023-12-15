#pragma once

#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"

namespace Sen::Kernel {

	template <class T> 
	class List {
		public:

			// value
		
			std::vector<T> value;

			// constructor

			explicit List(
				const std::vector<T> & data
			) : value(data) 
			{

			}

			// constructor

			explicit List(
				const List &that
			) : value(that.value) 
			{

			}
			
			// constructor

			List(
				std::initializer_list<T> iList
			) : value(iList) 
			{

			}
			
			// constructor

			List() : value{} 
			{

			}

			
			// destructor

			~List(

			) = default;

			/**
			 * get current list size
			*/

			auto size(

			) const -> size_t
			{
				return thiz.value.size();
			}

			/**
			 * fill current list
			*/

			auto fill(
				size_t count, 
				const T &value
			) -> void 
			{
				thiz.value = std::vector<T>(count, value);
				return;
			}

			/**
			 * add to current list
			*/

			auto add(
				const T &value
			) -> void 
			{
				thiz.value.push_back(value);
			}

			/**
			 * pop in current list
			*/

			auto pop(

			) -> T 
			{
				auto back = thiz.value.back();
				thiz.value.pop_back();
				return back;
			}

			/**
			 * insert to current list
			*/

			auto insert(
				size_t location, 
				const T &value
			) -> void 
			{
				thiz.value.insert(thiz.value.begin() + location, value);
			}

			/**
			 * begin iterator
			*/

			auto begin(

			) -> decltype(value.begin()) 
			{
				return thiz.value.begin();
			}

			/**
			 * end iterator
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
			 * [i]
			*/

			auto operator[](
				size_t index
			) -> T&
			{
				return thiz.at(index);
			}

			/**
			 * ==
			*/

			auto operator ==(
				List & that
			) const -> bool 
			{
				return thiz.value == that.value;
			}

			/**
			 * !=
			*/

			auto operator !=(
				List & that
			) const -> bool 
			{
				return !(thiz == that);
			}

			/**
			 * +
			*/

			auto operator +(
				List<T> & that
			) const -> List
			{
				auto result = List(thiz);
				result.value.insert(result.value.end(), that.value.begin(), that.value.end());
				return result;
			}

			/**
			 * >>
			*/

			auto operator >> (
				const T &value
			) -> void
			{
				thiz.add(value);
				return;
			}

			/**
			 * >>
			*/

			friend auto operator >>(
				std::istream& is,
				List<T> &list
			) -> std::istream&
			{
				auto val = T{};
				is >> val;
				list.add(val);
				return is;
			}

			/**
			 * <<
			*/

			friend auto operator << (
				std::ostream& os, 
				List<T>& list
			) -> std::ostream&
			{
				os << "[";
				for (auto i = 0; i < list.size(); ++i)
				{
					if(i == list.size() - 1){
						os << list[i] << "";
					}
					else{
						os << list[i] << ", ";
					}
				}
				os << "]";
				return os;
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
				thiz.value.erase(thiz.value.begin() + index);
				return;
			}

			/**
			 * shift
			*/

			auto shift(

			) -> T 
			{
				auto front = thiz.value.front();
				thiz.value.erase(thiz.value.begin());
				return front;
			}

			/**
			 * unshift
			*/

			auto unshift(
				const T &value
			) -> void 
			{
				thiz.value.insert(thiz.value.begin(), value);
				return;
			}

			/**
			 * reverse
			*/

			auto reverse(

			) -> void 
			{
				std::reverse(thiz.value.begin(), thiz.value.end());
				return;
			}


			/**
			 * for each
			*/

			auto forEach(
				std::function<void(T& e, size_t i)> method
			) -> void 
			{
				for (auto i : Range<size_t>(thiz.value.size())) {
					method(thiz[i], i);
				}
			}

			/**
			 * for each
			*/

			auto forEach(
				std::function<void(T& e)> method
			) -> void 
			{
				for(auto & c : thiz.value){
					method(c);
				}
			}

			/**
			 * map
			*/

			template <typename P>
			auto map(
				std::function<P(T& e)> method
			) -> List<P> 
			{
				auto arr = std::vector<P>{};
				for(auto & c : thiz.value){
					arr.push_back(method(c));
				}
				return List<P>{arr};
			}

			/**
			 * map
			*/

			template <typename P>
			auto map(
				std::function<P(T& e, size_t index)> method
			) -> List<P> 
			{
				auto arr = std::vector<P>{};
				for(auto i : Range<size_t>(thiz.value.size())){
					arr.push_back(method(thiz.value[i], i));
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
				for(auto & c : thiz.value){
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
				for(auto i : Range<size_t>(thiz.value.size())){
					if(!method(thiz.value[i], i)){
						return false;
					}
				}
				return true;
			}

			/**
			 * sort
			*/

			template <typename P>
			auto sort(
				std::function<P(T& a, T& b)> method
			) -> void 
			{
				std::sort(thiz.value.begin(), thiz.value.end(), method);
				return;
			}

			/**
			 * some
			*/

			auto some(
				std::function<bool(T& e)> method
			) -> bool 
			{
				for(auto & c : thiz.value){
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
				for(auto i : Range<size_t>(value.size())){
					if(method(thiz.value[i], i)){
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
				for(auto & c : thiz.value){
					if(method(c)){
						list.add(c);
					}
				}
				return list;
			}

			/**
			 * filter
			*/

			auto filter(std::function<bool(
				T& e, size_t index)> method
			) -> List<T> 
			{
				auto list = List<T>{};
				for(auto i : Range<size_t>(value.size())){
					if(method(value[i], i)){
						list.add(value[i]);
					}
				}
				return list;
			}

	};
}