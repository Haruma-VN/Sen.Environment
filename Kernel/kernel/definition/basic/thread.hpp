#pragma once

#include "kernel/definition/macro.hpp"
#include "kernel/definition/library.hpp"
#include "kernel/definition/assert.hpp"

namespace Sen::Kernel {
	#pragma region concept

template <typename T> 
concept IsValidArguments = true;

#pragma endregion

	template <typename T, typename ...Args> requires IsValidArguments<T> && (IsValidArguments<Args> && ...)
	class ThreadManager {
		public:
			using ThreadCount = std::size_t;
			using ThreadLimit = ThreadCount;
			using CurrentThread = std::size_t;
			using Mutex = std::mutex;
		private:
			template <typename MType>
			using List = std::vector<MType>;
		protected:
			List<std::function<T(Args...)>> threads{};
			CurrentThread _current_pointer{};
			Mutex g_mutex{};
		public:
			ThreadManager(
			) = default;

			~ThreadManager(
			) = default;

			ThreadManager(
				ThreadManager&& that
			) = delete;

			auto operator =(
				ThreadManager&& that
			)->ThreadManager & = delete;

			auto emplace_back (
				std::function<T(Args...)> function
			) -> void
			{
				auto lock = std::lock_guard<std::mutex>(this->g_mutex);
				this->threads.emplace_back(function);
				return;
			}

			auto emplace_front (
				std::function<T(Args...)> function
			) -> void
			{
				auto lock = std::lock_guard<std::mutex>(this->g_mutex);
				this->threads.embrace(this->threads.begin(), function);
				return;
			}

			auto pop_back (
			) -> void
			{
				auto lock = std::lock_guard<std::mutex>(this->g_mutex);
				this->threads.pop_back();
				return;
			}

			auto clear (
			) -> void
			{
				this->threads.clear();
				return;
			}

			auto pop_front (
			) -> void
			{
				auto lock = std::lock_guard<std::mutex>(this->g_mutex);
				this->threads.erase(this->threads.begin());
				return;
			}

			auto yield (
			) -> void
			{
				assert_conditional(this->_current_pointer < this->size(), fmt::format("Out of Thread to process"), "yield");
				auto process = std::thread([&]() {
					this->threads.at(this->_current_pointer++)();
				});
				process.join();
				return;
			}

			auto yield_all (
			) -> void
			{
				assert_conditional(this->_current_pointer == 0, fmt::format("Thread need to be set at 0 to yield all"), "yield_all");
				auto processes = List<std::thread>{};
				for (auto& f : this->threads) {
					processes.emplace_back(f);
				}
				for (auto& process : processes) {
					if (process.joinable()) {
						process.join();
					}
				}
				this->_current_pointer = this->threads.size();
				return;
			}

			auto run_all (
			) -> void
			{
				assert_conditional(this->_current_pointer == 0, fmt::format("Thread need to be set at 0 to run all"), "run_all");
				for (auto& f : this->threads) {
					f();
				}
				return;
			}

			auto current_pointer (
			) -> CurrentThread
			{
				auto lock = std::lock_guard<std::mutex>(this->g_mutex);
				return this->_current_pointer;
			}

			auto size (
			) -> ThreadCount
			{
				auto lock = std::lock_guard<std::mutex>(this->g_mutex);
				return this->threads.size();
			}

			auto capacity (
			) -> ThreadLimit
			{
				auto lock = std::lock_guard<std::mutex>(this->g_mutex);
				return this->threads.capacity();
			}

			auto add (
				ThreadCount index,
				std::function<T(Args...)> function
			) -> void
			{
				this->threads.insert(this->threads.begin() + index, function);
				return;
			}

			auto remove(
				ThreadCount index
			) -> void
			{
				this->threads.erase(this->threads.begin() + index);
				return;
			}

			auto is_empty (
			) -> bool
			{
				return this->threads.empty();
			}

			auto set_pointer (
				ThreadCount index
			) -> void
			{
				assert_conditional(index < this->threads.size(), "Thread need to be set at 0 to yield all", "set_pointer");
				this->_current_pointer = index;
				return;
			}
	};
}