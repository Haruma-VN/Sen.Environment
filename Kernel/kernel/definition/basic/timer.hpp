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

			static auto start(

			) -> clock::steady_clock::time_point const
			{
				return clock::high_resolution_clock::now();
			}

			/**
			 * Calculate time between two time passed
			*/

			static auto calculate(
				const clock::high_resolution_clock::time_point &before,
				const clock::high_resolution_clock::time_point &after
			) -> double const
			{
				return clock::duration<double>(after - before).count();
			}

	};
	
}