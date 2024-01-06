#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::RenderEffects {

	#pragma region Block1

	struct Block1 {
		public:
			uint32_t unknown_1;
			uint32_t unknown_2;
			uint32_t unknown_3;
			uint32_t unknown_4;
			uint32_t unknown_5;
			uint32_t unknown_6;

			explicit Block1(

			) = default;

			explicit Block1(
				uint32_t unknown_1,
				uint32_t unknown_2,
				uint32_t unknown_3,
				uint32_t unknown_4,
				uint32_t unknown_5,
				uint32_t unknown_6
			) : unknown_1(unknown_1), unknown_2(unknown_2), unknown_3(unknown_3),
			unknown_4(unknown_4), unknown_5(unknown_5), unknown_6(unknown_6)
			{

			}

			~Block1(

			) = default;
	};

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const Block1 & block
	) -> void
	{
		json = nlohmann::ordered_json{
			{"unknown_1", block.unknown_1},
			{"unknown_2", block.unknown_2},
			{"unknown_3", block.unknown_3},
			{"unknown_4", block.unknown_4},
			{"unknown_5", block.unknown_5},
			{"unknown_6", block.unknown_6}
		};
		return;
	}

	inline static auto from_json(
		const nlohmann::ordered_json & json, 
		Block1 & block
	) -> void
	{
		json.at("unknown_1").get_to(block.unknown_1);
		json.at("unknown_2").get_to(block.unknown_2);
		json.at("unknown_3").get_to(block.unknown_3);
		json.at("unknown_4").get_to(block.unknown_4);
		json.at("unknown_5").get_to(block.unknown_5);
		json.at("unknown_6").get_to(block.unknown_6);
		return;
	}
		

	#pragma endregion

	#pragma region Block2

	struct Block2 {
		public:
			uint32_t unknown_1;
			uint32_t unknown_2;

			explicit Block2(

			) = default;

			explicit Block2(
				uint32_t unknown_1,
				uint32_t unknown_2
			) :  unknown_1(unknown_1), unknown_2(unknown_2)
			{

			}

			~Block2(

			) = default;
	};

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const Block2 & block
	) -> void
	{
		json = nlohmann::ordered_json{
			{"unknown_1", block.unknown_1},
			{"unknown_2", block.unknown_2}
		};
		return;
	}

	inline static auto from_json(
		const nlohmann::ordered_json & json, 
		Block2 & block
	) -> void
	{
		json.at("unknown_1").get_to(block.unknown_1);
		json.at("unknown_2").get_to(block.unknown_2);
		return;
	}

	#pragma endregion

	#pragma region Block3

	struct Block3 {
		public:
			uint32_t unknown_2;
			std::string string;

			explicit Block3(
				uint32_t unknown_2,
				const std::string & string
			) : unknown_2(unknown_2), string(string)
			{ 

			}

			explicit Block3(

			) = default;

			~Block3(

			) = default;
	};

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const Block3 & block
	) -> void
	{
		json = nlohmann::ordered_json{
			{"unknown_2", block.unknown_2},
			{"string", block.string}
		};
		return;
	}

	inline static auto from_json(
		const nlohmann::ordered_json & json, 
		Block3 & block
	) -> void
	{
		json.at("unknown_2").get_to(block.unknown_2);
		json.at("string").get_to(block.string);
		return;
	}

	#pragma endregion

	#pragma region Block4

	struct Block4 {
		public:
			uint32_t unknown_1;
			uint32_t unknown_2;
			uint32_t unknown_3;
			uint32_t unknown_4;
			uint32_t unknown_5;

			explicit Block4(

			) = default;

			explicit Block4(
				uint32_t unknown_1,
				uint32_t unknown_2,
				uint32_t unknown_3,
				uint32_t unknown_4,
				uint32_t unknown_5
			) : unknown_1(unknown_1), unknown_2(unknown_2), unknown_3(unknown_3),
			unknown_4(unknown_4), unknown_5(unknown_5)
			{

			}

			~Block4(

			) = default;
	};

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const Block4 & block
	) -> void
	{
		json = nlohmann::ordered_json{
			{"unknown_1", block.unknown_1},
			{"unknown_2", block.unknown_2},
			{"unknown_3", block.unknown_3},
			{"unknown_4", block.unknown_4},
			{"unknown_5", block.unknown_5}
		};
		return;
	}

	inline static auto from_json(
		const nlohmann::ordered_json & json, 
		Block4 & block
	) -> void
	{
		json.at("unknown_1").get_to(block.unknown_1);
		json.at("unknown_2").get_to(block.unknown_2);
		json.at("unknown_3").get_to(block.unknown_3);
		json.at("unknown_4").get_to(block.unknown_4);
		json.at("unknown_5").get_to(block.unknown_5);
		return;
	}

	#pragma endregion

	#pragma region Block5

	struct Block5 {
		public:
			uint32_t unknown_1;
			uint32_t unknown_2;
			uint32_t unknown_3;
			uint32_t unknown_4;
			uint32_t unknown_5;
			uint32_t unknown_6;
			uint32_t unknown_7;

			explicit Block5(

			) = default;

			explicit Block5(
				uint32_t unknown_1,
				uint32_t unknown_2,
				uint32_t unknown_3,
				uint32_t unknown_4,
				uint32_t unknown_5,
				uint32_t unknown_6,
				uint32_t unknown_7
			) : unknown_1(unknown_1), unknown_2(unknown_2), unknown_3(unknown_3),
			unknown_4(unknown_4), unknown_5(unknown_5), unknown_6(unknown_6), unknown_7(unknown_7)
			{

			}

			~Block5(

			) = default;
	};

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const Block5 & block
	) -> void
	{
		json = nlohmann::ordered_json{
			{"unknown_1", block.unknown_1},
			{"unknown_2", block.unknown_2},
			{"unknown_3", block.unknown_3},
			{"unknown_4", block.unknown_4},
			{"unknown_5", block.unknown_5},
			{"unknown_6", block.unknown_6},
			{"unknown_7", block.unknown_7}
		};
		return;
	}

	inline static auto from_json(
		const nlohmann::ordered_json & json, 
		Block5 & block
	) -> void
	{
		json.at("unknown_1").get_to(block.unknown_1);
		json.at("unknown_2").get_to(block.unknown_2);
		json.at("unknown_3").get_to(block.unknown_3);
		json.at("unknown_4").get_to(block.unknown_4);
		json.at("unknown_5").get_to(block.unknown_5);
		json.at("unknown_6").get_to(block.unknown_6);
		json.at("unknown_7").get_to(block.unknown_7);
		return;
	}
		

	#pragma endregion

	#pragma region Block6

	struct Block6 {
		public:
			uint32_t unknown_1;
			uint32_t unknown_2;
			uint32_t unknown_3;
			uint32_t unknown_4;
			uint32_t unknown_5;

			explicit Block6(

			) = default;

			explicit Block6(
				uint32_t unknown_1,
				uint32_t unknown_2,
				uint32_t unknown_3,
				uint32_t unknown_4,
				uint32_t unknown_5
			) : unknown_1(unknown_1), unknown_2(unknown_2), unknown_3(unknown_3),
			unknown_4(unknown_4), unknown_5(unknown_5)
			{

			}

			~Block6(

			) = default;
	};

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const Block6 & block
	) -> void
	{
		json = nlohmann::ordered_json{
			{"unknown_1", block.unknown_1},
			{"unknown_2", block.unknown_2},
			{"unknown_3", block.unknown_3},
			{"unknown_4", block.unknown_4},
			{"unknown_5", block.unknown_5}
		};
		return;
	}

	inline static auto from_json(
		const nlohmann::ordered_json & json, 
		Block6 & block
	) -> void
	{
		json.at("unknown_1").get_to(block.unknown_1);
		json.at("unknown_2").get_to(block.unknown_2);
		json.at("unknown_3").get_to(block.unknown_3);
		json.at("unknown_4").get_to(block.unknown_4);
		json.at("unknown_5").get_to(block.unknown_5);
		return;
	}

	#pragma endregion

	#pragma region Block7

	struct Block7 {
		public:
			uint32_t unknown_1;
			uint32_t unknown_2;

			explicit Block7(

			) = default;

			explicit Block7(
				uint32_t unknown_1,
				uint32_t unknown_2
			) : unknown_1(unknown_1), unknown_2(unknown_2)
			{

			}

			~Block7(

			) = default;
	};

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const Block7 & block
	) -> void
	{
		json = nlohmann::ordered_json{
			{"unknown_1", block.unknown_1},
			{"unknown_2", block.unknown_2}
		};
		return;
	}

	inline static auto from_json(
		const nlohmann::ordered_json & json, 
		Block7 & block
	) -> void
	{
		json.at("unknown_1").get_to(block.unknown_1);
		json.at("unknown_2").get_to(block.unknown_2);
		return;
	}

	#pragma endregion

	#pragma region Block8

	struct Block8 {
		public:
			uint32_t unknown_1;
			uint32_t unknown_2;
			uint32_t unknown_3;
			uint32_t unknown_4;
			uint32_t unknown_5;

			explicit Block8(

			) = default;

			explicit Block8(
				uint32_t unknown_1,
				uint32_t unknown_2,
				uint32_t unknown_4,
				uint32_t unknown_5,
				uint32_t unknown_3
			) : unknown_1(unknown_1), unknown_2(unknown_2), unknown_3(unknown_3),
			unknown_4(unknown_4), unknown_5(unknown_5)
			{

			}

			~Block8(

			) = default;
	};

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const Block8 & block
	) -> void
	{
		json = nlohmann::ordered_json{
			{"unknown_1", block.unknown_1},
			{"unknown_2", block.unknown_2},
			{"unknown_3", block.unknown_3},
			{"unknown_4", block.unknown_4},
			{"unknown_5", block.unknown_5}
		};
		return;
	}

	inline static auto from_json(
		const nlohmann::ordered_json & json, 
		Block8 & block
	) -> void
	{
		json.at("unknown_1").get_to(block.unknown_1);
		json.at("unknown_2").get_to(block.unknown_2);
		json.at("unknown_3").get_to(block.unknown_3);
		json.at("unknown_4").get_to(block.unknown_4);
		json.at("unknown_5").get_to(block.unknown_5);
		return;
	}

	#pragma endregion

	#pragma region RenderEffects 	

	struct PopCapRenderEffects
	{
		public:
			std::vector<Block1> block_1;
			std::vector<Block2> block_2;
			std::vector<Block3> block_3;
			std::vector<Block4> block_4;
			std::vector<Block5> block_5;
			std::vector<Block6> block_6;
			std::vector<Block7> block_7;
			std::vector<Block8> block_8;

			explicit PopCapRenderEffects(

			) = default;

			explicit PopCapRenderEffects(
				std::vector<Block1> block_1,
				std::vector<Block2> block_2,
				std::vector<Block3> block_3,
				std::vector<Block4> block_4,
				std::vector<Block5> block_5,
				std::vector<Block6> block_6,
				std::vector<Block7> block_7,
				std::vector<Block8> block_8
			)
			{
				thiz.block_1 = std::move(block_1);
				thiz.block_2 = std::move(block_2);
				thiz.block_3 = std::move(block_3);
				thiz.block_4 = std::move(block_4);
				thiz.block_5 = std::move(block_5);
				thiz.block_6 = std::move(block_6);
				thiz.block_7 = std::move(block_7);
				thiz.block_8 = std::move(block_8);
			}

			~PopCapRenderEffects(

			) = default;
	};

	inline static auto to_json(
		nlohmann::ordered_json & json,
		const PopCapRenderEffects & p
	) -> void
	{
		json = nlohmann::ordered_json{
			{"block_1", p.block_1},
			{"block_2", p.block_2},
			{"block_3", p.block_3},
			{"block_4", p.block_4},
			{"block_5", p.block_5},
			{"block_6", p.block_6},
			{"block_7", p.block_7},
			{"block_8", p.block_8}
		};
		return;
	}

	inline static auto from_json(
		const nlohmann::ordered_json & json,
		PopCapRenderEffects & p
	) -> void
	{
		json.at("block_1").get_to(p.block_1);
		json.at("block_2").get_to(p.block_2);
		json.at("block_3").get_to(p.block_3);
		json.at("block_4").get_to(p.block_4);
		json.at("block_5").get_to(p.block_5);
		json.at("block_6").get_to(p.block_6);
		json.at("block_7").get_to(p.block_7);
		json.at("block_8").get_to(p.block_8);
		return;
	}

	#pragma endregion
}