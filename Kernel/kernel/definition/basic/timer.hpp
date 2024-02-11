#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition {

	// Use C++ clock

	namespace clock = std::chrono;

	// Timer class

	class Timer {

		public:

			/**
			 * Start the timer
			*/

			inline static auto start(

			) -> clock::steady_clock::time_point const
			{
				return clock::high_resolution_clock::now();
			}

			/**
			 * Calculate time between two time passed
			*/

			inline static auto calculate(
				const clock::high_resolution_clock::time_point &before,
				const clock::high_resolution_clock::time_point &after
			) -> double const
			{
				return clock::duration<double>(after - before).count();
			}

			/**
			 * Sleep to wait for a thread
			*/
			template <typename T> requires std::is_integral<T>::value
			inline static auto sleep(
				T m
			) -> void
			{
				std::this_thread::sleep_for(clock::milliseconds(static_cast<long long>(m)));
				return;
			}

	};
	
}