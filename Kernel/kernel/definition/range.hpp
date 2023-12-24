#pragma once

#include <type_traits>

namespace Sen::Kernel {

	#pragma region range

	/**
	 * Region usage:
	 * for(auto i : Range<data type>(size)){
	 * 		std::cout << i << std::endl;
	 * }
	 * is equals to
	 * for(auto i = 0; i < size; i++)
	 * {
	 * 		std::cout << i << std::endl;
	 * }
	 *
	 * Constructor:
	 * (end)
	 * (end, step)
	 * (start, end, step)
	 */

	template <typename T>
	class Range {

		// test generic

		static_assert(std::is_arithmetic<T>::value, "T must be numeric type implementation");

		public:

			/**
			 * Iterator class
			*/

			class iterator {

				// index

				T i;

				// step

				T step;

			public:

				// default constructor

				iterator(
					T start, 
					T step
				) : i(start), step(step) 
				{

				}

				// operator: !=

				auto operator != (
					const iterator& that
				) const -> bool 
				{ 
					return i != that.i; 
				}

				// operator ++

				auto operator++ (

				) -> const iterator& 
				{ 
					this->i += step; 
					return *this; 
				}

				auto operator* (

				) const -> T 
				{ 
					return this->i; 
				}
			};

			/**
			 * Range constructor
			 * Provide end
			*/

			Range(
				T end
			) : start_(0), end_(end), step_(1) 
			{

			}

			/**
			 * Range constructor
			 * Provide end and step
			*/

			Range(
				T end,
				T step
			) : start_(0), end_(end), step_(step) 
			{

			}

			/**
			 * Range constructor
			 * Provide start, end and step
			*/

			Range(
				T start, 
				T end, 
				T step
			) : start_(start), end_(end), step_(step) 
			{

			}

			// begin

			inline auto begin(

			) const -> iterator 
			{ 
				return iterator(start_, step_); 
			}

			// end

			inline auto end(

			) const -> iterator 
			{ 
				return iterator(end_, step_); 
			}

		private:
			T start_, end_, step_;
	};

	#pragma endregion

}
