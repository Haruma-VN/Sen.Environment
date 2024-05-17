#pragma once

#include "kernel/support/popcap/animation/definition.hpp"
#include "kernel/support/popcap/animation/convert/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert
{

	class FromFlash : public Common
	{
	private:
		using XMLDocument = tinyxml2::XMLDocument;

		using XMLElement = tinyxml2::XMLElement;

		using RecordInfo = RecordInfo;

		using Common = Common;

		using Animation = SexyAnimation;

		using FrameList = FrameList;

	public:

		Animation animation;

	protected:

		struct DocumentSymbol
		{
			std::vector<std::string> image;
			std::vector<std::string> sprite;
			std::vector<std::string> action;
		};

		struct AnimFrameNode
		{
			std::vector<AnimationFrame> frames;
			int layer_length;
		};

		struct ActionFrameNode
		{
		public:
			int index;
			int duration;
			bool use_transform = false;
			std::vector<double> transform;
			bool use_color = false;
			std::array<double, 4> color;

			explicit ActionFrameNode(

				) = default;

			explicit constexpr ActionFrameNode(
				int index,
				int duration) : index(index), duration(duration)
			{
			}

			~ActionFrameNode(

				) = default;
		};

	protected:
		template <typename T>
		inline auto get_index(
			const std::vector<T> &v,
			T k) -> int
		{
			static_assert(sizeof(T) != 0);
			auto it = std::find(v.begin(), v.end(), k);
			if (it != v.end())
			{
				return static_cast<int>(it - v.begin());
			}
			return -1;
		}

		template <typename T, typename E>
		inline static auto check_element_in_vector(
			const std::vector<T> &v,
			const E &e) -> bool
		{
			static_assert(sizeof(T) != 0);
			static_assert(sizeof(E) != 0);
			if (std::find(v.begin(), v.end(), e) != v.end())
			{
				return true;
			}
			return false;
		}

		inline static auto constexpr includes = check_element_in_vector<double, double>;

		template <typename T>
		inline static auto parse_transform_for_image(
			std::vector<T> &transform,
			XMLElement *Matrix,
			int resolution) -> void
		{
			static_assert(sizeof(T) <= sizeof(double));
			// const auto &resolution_list = std::vector<double>{0.78125, 1.5625, 1.875, 3.125, 1.0};
			
			/*
			if (includes(resolution_list, a_matrix)) {
				a_matrix = 1.0;
			}
			if (includes(resolution_list, d_matrix)) {
				d_matrix = 1.0;
			}
			*/
		    auto scale_ratio = 1200.0f / static_cast<float>(resolution);
			auto a = Matrix->FindAttribute("a");
			auto b = Matrix->FindAttribute("b");
			auto c = Matrix->FindAttribute("c");
			auto d = Matrix->FindAttribute("d");
			auto tx = Matrix->FindAttribute("tx");
			auto ty = Matrix->FindAttribute("ty");
			transform = {(std::stod((a ? (a->Value()) : "1")) / scale_ratio), std::stod((b ? b->Value() : "0")), std::stod((c ? c->Value() : "0")), (std::stod((d ? (d->Value()) : "1")) / scale_ratio), std::stod((tx ? tx->Value() : "0")), std::stod((ty ? ty->Value() : "0"))};
			return;
		}
		inline static auto constexpr copy_transform_for_image = parse_transform_for_image<double>;

		template <typename T>
		inline static auto parse_transform(
			std::vector<T> &transform,
			XMLElement *Matrix) -> void
		{
			static_assert(sizeof(T) <= sizeof(double));
			auto a = Matrix->FindAttribute("a");
			auto d = Matrix->FindAttribute("d");
			auto b = Matrix->FindAttribute("b");
			auto c = Matrix->FindAttribute("c");
			auto tx = Matrix->FindAttribute("tx");
			auto ty = Matrix->FindAttribute("ty");
			transform = {std::stod((a ? a->Value() : "1")), std::stod((b ? b->Value() : "0")), std::stod((c ? c->Value() : "0")), std::stod((d ? d->Value() : "1")), std::stod((tx ? tx->Value() : "0")), std::stod((ty ? ty->Value() : "0"))};
			return;
		}
		inline static auto constexpr copy_transform = parse_transform<double>;

		template <typename T>
		inline static auto transform_calculator(
			std::vector<T> &transform) -> void
		{
			static_assert(sizeof(T) <= sizeof(double));
			if (transform[0] == transform[3] && transform[1] == -transform[2])
			{
				if (transform[0] == 1.0 && transform[1] == 0.0)
				{
					transform = {transform[4], transform[5]};
					return;
				}
				const auto &acos = std::acos(transform[0]);
				const auto &asin = std::asin(transform[1]);
				if (std::abs(acos) - std::abs(asin) <= 1e-2)
				{
					transform = {asin, transform[4], transform[5]};
					return;
				}
			}
			return;
		}
		inline static auto constexpr calculator = transform_calculator<double>;

		template <typename T>
		inline static auto parse_color(
			std::array<T, 4> &color,
			XMLElement *Color) -> void
		{
			static_assert(sizeof(T) <= sizeof(double));
			//
			auto r = Color->FindAttribute("redMultiplier");
			auto g = Color->FindAttribute("greenMultiplier");
			auto b = Color->FindAttribute("blueMultiplier");
			auto a = Color->FindAttribute("alphaMultiplier");
			auto r_pos = Color->FindAttribute("redOffset");
			auto g_pos = Color->FindAttribute("greenOffset");
			auto b_pos = Color->FindAttribute("blueOffset");
			auto a_pos = Color->FindAttribute("alphaOffset");
			color = {
				color_compute(std::stod(r ? r->Value() : "1"), std::stod(r_pos ? r_pos->Value() : "0")),
				color_compute(std::stod(g ? g->Value() : "1"), std::stod(g_pos ? g_pos->Value() : "0")),
				color_compute(std::stod(b ? b->Value() : "1"), std::stod(b_pos ? b_pos->Value() : "0")),
				color_compute(std::stod(a ? a->Value() : "1"), std::stod(a_pos ? a_pos->Value() : "0"))};
			return;
		}
		inline static auto constexpr copy_color = parse_color<double>;

		template <typename T>
		inline static auto parse_color_compute(T mutil, T offset) -> T
		{
			static_assert(sizeof(T) <= sizeof(double));
			return static_cast<T>((std::max)(0.0, (std::min)(255.0, mutil * 255.0 + offset)) / 255.0);
		}
		inline static auto constexpr color_compute = parse_color_compute<double>;

		template <typename T>
		inline static auto mix_transform(
			std::vector<T> &source,
			const std::vector<T> &change) -> void
		{
			static_assert(sizeof(T) <= sizeof(double));
			source = {
				change[0] * source[0] + change[2] * source[1],
				change[1] * source[0] + change[3] * source[1],
				change[0] * source[2] + change[2] * source[3],
				change[1] * source[2] + change[3] * source[3],
				change[0] * source[4] + change[2] * source[5] + change[4],
				change[1] * source[4] + change[3] * source[5] + change[5],
			};
			return;
		}

		inline static auto constexpr mix_transform_double = mix_transform<double>;

		inline static auto mix_color(
			std::vector<double> &source,
			const std::array<double, 4> &change) -> void
		{
			source = {change[0] * source[0], change[1] * source[1], change[2] * source[2], change[3] * source[3]};
			return;
		}

		inline static auto trim_left_in_place(
			std::string &str
		) -> std::string
		{
			auto i = 0;
			while (i < str.size() && isspace(str[i]))
			{
				++i;
			};
			return str.erase(0, i);
		}

		inline static auto trim(
			const std::string &str
		) -> std::string
		{
			return std::regex_replace(str, std::regex("(^[ ]+)|([ ]+$)"),"");
		}

	protected:
		inline auto read_symbols_include(
			DocumentSymbol &dom_symbol_list,
			XMLDocument *document) -> void
		{
			auto symbols_elements = document->FirstChildElement("DOMDocument")->FirstChildElement("symbols");
			if (symbols_elements == nullptr)
			{
				throw Exception(fmt::format("{}", Language::get("popcap.animation.from_flash.document_has_no_symbols")), std::source_location::current(), "read_symbols_include");
			}
			for (auto child = symbols_elements->FirstChildElement("Include"); child != nullptr; child = child->NextSiblingElement("Include"))
			{
				if (child == nullptr)
				{
					throw Exception(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_symbols_include")), std::source_location::current(), "read_symbols_include");
				}
				auto item = std::string{child->FirstAttribute()->Value()};
				if (item.starts_with("image"))
				{
					dom_symbol_list.image.emplace_back(item.substr(6, item.size() - 10));
				}
				if (item.starts_with("sprite"))
				{
					dom_symbol_list.sprite.emplace_back(item.substr(7, item.size() - 11));
				}
				if (item.starts_with("action"))
				{
					dom_symbol_list.action.emplace_back(item.substr(7, item.size() - 11));
				}
			}
			return;
		}

		inline auto parse_image(
			const std::string &image_id,
			const RecordInfo &record,
			XMLDocument *document
		) -> void
		{
			auto DOMSymbolItem = document->FirstChildElement("DOMSymbolItem");
			if (DOMSymbolItem == nullptr)
			{
				
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMSymbolItem")), image_id), std::source_location::current(), "parse_image");
			}
			auto image_child = DOMSymbolItem->FindAttribute("name");
			if (image_child == nullptr or fmt::format("image/{}", image_id) != std::string{image_child->Value()})
			{
				
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_name")), image_id), std::source_location::current(), "parse_image");
			}
			auto timeline = DOMSymbolItem->FirstChildElement("timeline");
			if (timeline == nullptr)
			{
				
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_timeline")), image_id), std::source_location::current(), "parse_image");
			}
			auto DOMTimeline = timeline->FirstChildElement("DOMTimeline");
			if (DOMTimeline == nullptr)
			{
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMtimeline")), image_id), std::source_location::current(), "parse_image");
			}
			auto id_name = DOMTimeline->FindAttribute("name");
			if (id_name == nullptr or image_id != std::string{id_name->Value()})
			{
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_name")), image_id), std::source_location::current(), "parse_image");
			}
			auto layers = DOMTimeline->FirstChildElement("layers");
			if (layers == nullptr)
			{
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_layers")), image_id), std::source_location::current(), "parse_image");
			}
			auto DOMLayer = layers->FirstChildElement("DOMLayer");
			if (DOMLayer == nullptr)
			{
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMLayer")), image_id), std::source_location::current(), "parse_image");
			}
			auto frames = DOMLayer->FirstChildElement("frames");
			if (frames == nullptr)
			{
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_frames")), image_id), std::source_location::current(), "parse_image");
			}
			auto DOMFrame = frames->FirstChildElement("DOMFrame");
			if (DOMFrame == nullptr)
			{
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMframe")), image_id), std::source_location::current(), "parse_image");
			}
			auto elements = DOMFrame->FirstChildElement("elements");
			if (elements == nullptr)
			{
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_elements")), image_id), std::source_location::current(), "parse_image");
			}
			auto DOMBitmapInstance = elements->FirstChildElement("DOMBitmapInstance");
			if (DOMBitmapInstance == nullptr)
			{
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMBitmapInstance")), image_id), std::source_location::current(), "parse_image");
			}
			auto image_name = std::string{DOMBitmapInstance->FindAttribute("libraryItemName")->Value()}.substr(6);
			auto animation_image_name = record.image.at(image_id).name;
			if (!image_name.starts_with("TMP"))
			{
				if (image_name != animation_image_name)
				{
					throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_name")), image_id), std::source_location::current(), "parse_image");
				}
			}
			auto matrix = DOMBitmapInstance->FirstChildElement("matrix");
			if (matrix == nullptr)
			{
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_matrix")), image_id), std::source_location::current(), "parse_image");
			}
			auto Matrix = matrix->FirstChildElement("Matrix");
			if (Matrix == nullptr)
			{
				throw Exception(String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_matrix")), image_id), std::source_location::current(), "parse_image");
			}
			auto animation_image = AnimationImage{};
			animation_image.name = animation_image_name;
			animation_image.id = image_id;
			animation_image.size = record.image.at(image_id).size;
			copy_transform_for_image(animation_image.transform, Matrix, record.resolution);
			animation.image.emplace_back(animation_image);
			return;
		}

		inline auto get_action_index(
			tsl::ordered_map<std::string, ActionFrameNode> &frame_node,
			XMLDocument *document
		) -> void
		{
			auto timelines_elements = document->FirstChildElement("DOMDocument")->FirstChildElement("timelines");
			assert_conditional(timelines_elements != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.document_has_no_timelines")), "get_action_index");
			auto DOMTimeline = timelines_elements->FirstChildElement("DOMTimeline");
			assert_conditional(DOMTimeline != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.document_has_no_DOMTimeline")), "get_action_index");
			auto layers = DOMTimeline->FirstChildElement("layers");
			assert_conditional(layers != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.document_has_no_layers")), "get_action_index");
			auto DOMLayer_Flow = layers->FirstChildElement("DOMLayer");
			assert_conditional(DOMLayer_Flow != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.document_has_no_DOMLayer")), "get_action_index");
			assert_conditional((std::string{DOMLayer_Flow->FindAttribute("name")->Value()} == "flow"), fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_document_flow")), "get_action_index");
			auto frames = DOMLayer_Flow->FirstChildElement("frames");
			assert_conditional(frames != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.document_has_no_frames")), "get_action_index");
			for (auto DOMFrame = frames->FirstChildElement("DOMFrame"); DOMFrame != nullptr; DOMFrame = DOMFrame->NextSiblingElement("DOMFrame"))
			{
				assert_conditional(DOMFrame != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.document_has_no_DOMFrame")), "get_action_index");

				const auto &frame_index = std::stoi(DOMFrame->FindAttribute("index")->Value());
				auto m_duration = DOMFrame->FindAttribute("duration");
				const auto &frame_duration = std::stoi((m_duration ? m_duration->Value() : "1"));
				auto frame_name = DOMFrame->FindAttribute("name");
				assert_conditional(frame_name != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.document_has_no_frame_name")), "get_action_index");
				auto labelType = DOMFrame->FindAttribute("labelType");
				assert_conditional(labelType != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_action_label")), "get_action_index");
				auto elements = DOMFrame->FirstChildElement("elements");
				assert_conditional(elements != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.dom_frame_has_no_elements")), "get_action_index");
				auto DOMSymbolInstance = elements->FirstChildElement("DOMSymbolInstance");
				assert_conditional(DOMSymbolInstance != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.dom_frame_has_no_DOMSymbolInstance")), "get_action_index");
				auto action_name = std::string{frame_name->Value()};
				assert_conditional((fmt::format("action/{}", action_name) == std::string{DOMSymbolInstance->FindAttribute("libraryItemName")->Value()}), fmt::format("{}", Language::get("popcap.animation.from_flash.invaild_action_index")), "get_action_index");
				frame_node[action_name] = ActionFrameNode{frame_index, frame_duration};
				auto matrix = DOMSymbolInstance->FirstChildElement("matrix");
				if (matrix != nullptr)
				{
					auto Matrix = matrix->FirstChildElement("Matrix");
					assert_conditional(Matrix, fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_matrix")), "get_action_index");
					auto transform = std::vector<double>{};
					copy_transform(transform, Matrix);
					for (const auto &i : Range<int>(6))
					{
						if (transform[i] != initial_transform[i])
						{
							frame_node[action_name].use_transform = true;
							break;
						}
					}
					if (frame_node[action_name].use_transform)
					{
						frame_node[action_name].transform = transform;
					}
				}
				auto color = DOMSymbolInstance->FirstChildElement("color");
				if (color)
				{
					auto ColorT = color->FirstChildElement("Color");
					assert_conditional(ColorT != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_color")), "get_action_index");
					auto base_color = std::array<double, 4>{};
					copy_color(base_color, ColorT);
					for (const auto &i : Range<int>(4))
					{
						if (base_color[i] != initial_color[i])
						{
							frame_node[action_name].use_color = true;
							break;
						}
					}
					if (frame_node[action_name].use_color)
					{
						frame_node[action_name].color = base_color;
					}
				}
			}
			return;
		}

		template <auto is_action>
		inline auto parse_sprite(
			AnimFrameNode &frame_node,
			const std::string &name,
			XMLDocument *document,
			const DocumentSymbol &dom_symbol_list
		) -> void
		{
			static_assert(is_action == true or is_action == false, "is_action must be true or false");
			static_assert(sizeof(is_action) == sizeof(bool), "is_action must be boolean wrapper");
			auto DOMSymbolItem = document->FirstChildElement("DOMSymbolItem");
			assert_conditional(DOMSymbolItem != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMSymbolItem")), name), "parse_sprite");
			if constexpr (is_action)
			{
				auto action_name = fmt::format("action/{}", name);
				auto name_child = DOMSymbolItem->FindAttribute("name");
				assert_conditional((name_child != nullptr or action_name == std::string{name_child->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_name")), name), "parse_sprite");
			}
			{
				auto sprite_name = fmt::format("sprite/{}", name);
				auto name_child = DOMSymbolItem->FindAttribute("name");
				assert_conditional((name_child != nullptr or sprite_name == std::string{name_child->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_name")), name), "parse_sprite");
			}
			auto timeline = DOMSymbolItem->FirstChildElement("timeline");
			assert_conditional(timeline != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_timeline")), name), "parse_sprite");
			auto DOMTimeline = timeline->FirstChildElement("DOMTimeline");
			assert_conditional(DOMTimeline != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMtimeline")), name), "parse_sprite");
			auto dom_name = DOMTimeline->FindAttribute("name");
			assert_conditional((dom_name != nullptr or name == std::string{dom_name->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_name")), name), "parse_sprite");
			auto layers = DOMTimeline->FirstChildElement("layers");
			assert_conditional(layers != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_layers")), name), "parse_sprite");
			auto &frame_list = frame_node.frames;
			auto use_empty_last_layer = false;
			auto model = Model{};
			auto layer_count = 0;
			for (auto DOMLayer = layers->LastChildElement("DOMLayer"); DOMLayer; DOMLayer = DOMLayer->PreviousSiblingElement("DOMLayer"))
			{
				assert_conditional(DOMLayer != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMLayer")), name), "parse_sprite");
				auto frames = DOMLayer->FirstChildElement("frames");
				assert_conditional(frames != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_frames")), name), "parse_sprite");
				for (auto DOMFrame = frames->FirstChildElement("DOMFrame"); DOMFrame != nullptr; DOMFrame = DOMFrame->NextSiblingElement("DOMFrame"))
				{
					assert_conditional(DOMFrame != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMFrame")), name), "parse_sprite");
					const auto &frame_index = std::stoi(DOMFrame->FindAttribute("index")->Value());
					auto m_duration = DOMFrame->FindAttribute("duration");
					const auto &frame_duration = std::stoi((m_duration ? m_duration->Value() : "1"));
					auto elements = DOMFrame->FirstChildElement("elements");
					auto DOMSymbolInstance = elements->FirstChildElement("DOMSymbolInstance");
					if (DOMSymbolInstance == nullptr)
					{
						if (DOMLayer->PreviousSiblingElement("DOMLayer") == nullptr) {
							const auto &read_blank_duration = frame_duration - frame_list.size() + 1;
							if (read_blank_duration > 0 and DOMFrame->NextSiblingElement("DOMFrame") == nullptr) {
								use_empty_last_layer = true;
								for (const auto &i : Range<int>(read_blank_duration)) {
									frame_list.emplace_back(AnimationFrame{});
								}
								continue;
							}
						}
						close_current_model(model, frame_list);
						continue;
					}
					auto libraryItemName = std::string{DOMSymbolInstance->FindAttribute("libraryItemName")->Value()};
					auto name_match = std::regex("(image|sprite)");
					auto match_result = std::smatch{};
					auto regex_result = std::regex_search(libraryItemName, match_result, name_match);
					assert_conditional(regex_result,String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_name_match")), name), "parse_sprite");
					assert_conditional(match_result[0] == match_result[1], String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.mismatch_name_match")), name), "parse_sprite");
					auto is_sprite = match_result[0].str() == "sprite";
					auto resource_name = libraryItemName.substr(match_result[0].str().size() + 1);
					auto resource = is_sprite ? get_index<std::string>(dom_symbol_list.sprite, resource_name) : get_index<std::string>(dom_symbol_list.image, resource_name);
					assert_conditional(resource != -1, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.cannot_find_sprite")), name), "parse_sprite");
					if (frame_index + 1 > frame_list.size())
					{
						for (const auto &i : Range<int>((frame_index + 1) - (frame_list.size())))
						{
							frame_list.emplace_back(AnimationFrame{});
						}
					}
					if (!model.use_model)
					{
						model.use_model = true;
						model.state = 1_byte;
						model.index = layer_count;
						model.resource = resource;
						model.sprite = is_sprite;
						model.frame_start = frame_index;
						model.frame_duration = frame_duration;
						//
						frame_list[frame_index].append.emplace_back(
							AnimationAppend{
								.index = layer_count,
								.resource = resource,
								.sprite = is_sprite});
						++layer_count;
					}

					model.frame_start = frame_index;
					model.frame_duration = frame_duration;
					auto frame_move = AnimationMove{};
					auto matrix = DOMSymbolInstance->FirstChildElement("matrix");
					if (matrix == nullptr)
					{
						if constexpr (is_action)
						{
							frame_move.transform = {1.0, 0.0, 0.0, 1.0, 0.0, 0.0};
						}
						else
						{
							frame_move.transform = {0.0, 0.0};
						}
					}
					else
					{
						auto Matrix = matrix->FirstChildElement("Matrix");
						assert_conditional(Matrix, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_matrix")), name), "parse_sprite");
						if constexpr (is_action)
						{
							copy_transform(frame_move.transform, Matrix);
						}
						else
						{
							copy_transform(frame_move.transform, Matrix);
							calculator(frame_move.transform);
						}
					}
					auto color = DOMSymbolInstance->FirstChildElement("color");
					if (color != nullptr)
					{
						auto ColorT = color->FirstChildElement("Color");
						assert_conditional(ColorT != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_color")), name), "parse_sprite");
						auto base_color = std::array<double, 4>{};
						copy_color(base_color, ColorT);
						if (!(model.color[0] == base_color[0] && model.color[1] == base_color[1] && model.color[2] == base_color[2] && model.color[3] == base_color[3]))
						{
							model.color = base_color;
							frame_move.color = {base_color[0], base_color[1], base_color[2], base_color[3]};
						};
					}
					frame_move.index = model.index;
					frame_list[frame_index].change.emplace_back(frame_move);
				}
				close_current_model(model, frame_list);
			}
			frame_node.layer_length = use_empty_last_layer ? layer_count - 1 : layer_count;
			if (frame_list.size() > 0) {
				frame_list.pop_back();
			}
			return;
		}

		inline auto close_current_model(
			Model &model,
			std::vector<AnimationFrame> &frame_list) -> void
		{
			if (model.use_model)
			{
				auto index = model.frame_start + model.frame_duration;
				if (index + 1 > frame_list.size())
				{
					for (const auto &i : Range<int>((index + 1) - (frame_list.size())))
					{
						frame_list.emplace_back(AnimationFrame{});
					}
				}
				frame_list[index].remove.emplace_back(model.index);
				model.use_model = false;
			}
			return;
		}

		inline auto merge_action(
			std::map<std::string, AnimFrameNode> &action_map,
			const tsl::ordered_map<std::string, ActionFrameNode> &action_index
		) -> void
		{
			auto label_remove = 0;
			auto start_index = 0;
			auto remove_list = std::vector<int>{};
			auto write_frame = false;
			for (const auto &[action_name, action_info] : action_index)
			{
				auto &action_frames = action_map.at(action_name).frames;
				const auto &duration = action_info.duration;
				const auto &action_length = action_frames.size();
				if (action_length != duration)
				{
					if (duration > action_length)
					{
						for (const auto &i : Range<int>(duration - action_length))
						{
							auto new_add_frames = action_frames[i];
							new_add_frames.append.clear();
							action_frames.emplace_back(new_add_frames);
						}
					}
					else
					{
						action_frames.erase(action_frames.begin() + duration, action_frames.begin() + duration + (action_length - duration));
					}
				}
				for (const auto &i : Range<int>(action_frames.size()))
				{
					for (const auto &k : Range<int>(action_frames[i].remove.size()))
					{
						action_frames[i].remove[k] += start_index;
						remove_list.emplace_back(action_frames[i].remove[k]);
					}
					for (const auto &k : Range<int>(action_frames[i].append.size()))
					{
						action_frames[i].append[k].index += start_index;
					}
					for (const auto &k : Range<int>(action_frames[i].change.size()))
					{
						action_frames[i].change[k].index += start_index;
						if (action_info.use_transform)
						{
							mix_transform_double(action_frames[i].change[k].transform, action_info.transform);
							calculator(action_frames[i].change[k].transform);
						}
						else
						{
							calculator(action_frames[i].change[k].transform);
						}
						if (action_info.use_color)
						{
							if (action_frames[i].change[k].color.empty())
							{
								action_frames[i].change[k].color = {action_info.color[0], action_info.color[1], action_info.color[2], action_info.color[3]};
							}
							else
							{
								mix_color(action_frames[i].change[k].color, action_info.color);
							}
						}
					}
				}
				if (write_frame)
				{
					for (const auto &i : Range<int>(label_remove, start_index, 1))
					{
						if (!check_element_in_vector<int>(action_frames[0].remove, i) && !check_element_in_vector<int>(remove_list, i))
						{
							action_frames[0].remove.emplace_back(i);
						}
					}
					label_remove = start_index;
				}
				write_frame = true;
				start_index += action_map.at(action_name).layer_length;
				std::copy(action_frames.begin(), action_frames.end(), std::back_inserter(animation.main_sprite.frame));
			}
			return;
		}

		inline auto parse_dom_document(
			const tsl::ordered_map<std::string, ActionFrameNode> &action_index,
			XMLDocument *document
		) -> void
		{
			auto frameRate = document->FirstChildElement("DOMDocument")->FindAttribute("frameRate");
			animation.frame_rate = std::stoi(frameRate ? frameRate->Value() : "24");
			auto width = document->FirstChildElement("DOMDocument")->FindAttribute("width");
			auto height = document->FirstChildElement("DOMDocument")->FindAttribute("height");
			animation.size = AnimationSize{
				std::stoi(width ? width->Value() : "290"), std::stoi(height ? height->Value() : "390")};
			auto DOMTimeline = document->FirstChildElement("DOMDocument")->FirstChildElement("timelines")->FirstChildElement("DOMTimeline");
			assert_conditional("animation" == std::string{DOMTimeline->FindAttribute("name")->Value()}, fmt::format("{}", Language::get("popcap.animation.from_flash.document_name_must_be_animation")), "parse_dom_document");
			for (const auto &[label, v] : action_index)
			{
				animation.main_sprite.frame[v.index].label = label;
			}
			auto DOMLayer = document->FirstChildElement("DOMDocument")->FirstChildElement("timelines")->FirstChildElement("DOMTimeline")->FirstChildElement("layers")->FirstChildElement("DOMLayer");
			auto DOMLayer_command = DOMLayer->NextSiblingElement("DOMLayer");
			assert_conditional("command" == std::string{DOMLayer_command->FindAttribute("name")->Value()}, fmt::format("{}", Language::get("popcap.animation.from_flash.document_name_must_be_command")), "parse_dom_document");
			auto frames = DOMLayer_command->FirstChildElement("frames");
			const auto &last_frames = std::stoi(frames->LastChildElement("DOMFrame")->FindAttribute("index")->Value());
			for (const auto &i : Range<int>(last_frames - (animation.main_sprite.frame.size() - 1)))
			{
				animation.main_sprite.frame.emplace_back(AnimationFrame{});
			}
			for (auto DOMFrame = frames->FirstChildElement("DOMFrame"); DOMFrame; DOMFrame = DOMFrame->NextSiblingElement("DOMFrame"))
			{
				assert_conditional(DOMFrame, fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMFrame")), "parse_dom_document");
				const auto &frame_index = std::stoi(DOMFrame->FindAttribute("index")->Value());
				auto Actionscript = DOMFrame->FirstChildElement("Actionscript");
				if (Actionscript == nullptr) {
					continue;
				}
				auto script = Actionscript->FirstChildElement("script");
				assert_conditional(script != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_dom_script")), "parse_dom_document");
				auto script_text = script->FirstChild()->ToText();
				assert_conditional(script_text->CData(), fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_script_cdata")), "parse_dom_document");
				auto script_list = String{trim(std::string{script_text->Value()})}.split(";");
				for (auto &command : script_list)
				{
					command = trim(command);
					if (command == "stop()")
					{
						animation.main_sprite.frame[frame_index].stop = true;
						continue;
					};
					auto pattern = std::regex("fscommand\\(\"(.*)\", \"(.*)\"\\)");
					auto matches = std::smatch{};
					auto match_result = std::regex_match(command, matches, pattern);
					assert_conditional(match_result, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_command")), command), "parse_dom_document");
					animation.main_sprite.frame[frame_index].command.emplace_back(AnimationCommand{matches[1], matches[2]});
				}
			}
			return;
		}

	public:
		explicit FromFlash() = default;

		~FromFlash(

			) = default;

		FromFlash(
			FromFlash &&that) = delete;

		auto operator=(
			FromFlash &&that)
			-> FromFlash & = delete;

		inline auto process(
			std::string_view source,
			const RecordInfo &record_info)
			-> void
		{
			auto dom_document = XMLDocument{};
			FileSystem::read_xml(fmt::format("{}/DomDocument.xml", source), &dom_document);
			auto dom_symbol_list = DocumentSymbol{};
			read_symbols_include(dom_symbol_list, &dom_document);
			if (record_info.version > 6 || record_info.version < 1)
			{
				throw Exception(fmt::format("{}: {}", Language::get("popcap.animation.from_flash.invalid_version"), record_info.version), std::source_location::current(), "process");
			}
			for (const auto &image_id : dom_symbol_list.image)
			{
				auto image_document = XMLDocument{};
				FileSystem::read_xml(fmt::format("{}/library/image/{}.xml", source, image_id), &image_document);
				parse_image(image_id, record_info, &image_document);
			}
			for (const auto &sprite_name : dom_symbol_list.sprite)
			{
				auto sprite_document = XMLDocument{};
				FileSystem::read_xml(fmt::format("{}/library/sprite/{}.xml", source, sprite_name), &sprite_document);
				auto frame_node = AnimFrameNode{};
				parse_sprite<false>(frame_node, sprite_name, &sprite_document, dom_symbol_list);
				animation.sprite.emplace_back(AnimationSprite{
					sprite_name, AnimationWorkArea{0, static_cast<int>(frame_node.frames.size())}, frame_node.frames});
			}
			auto action_map = std::map<std::string, AnimFrameNode>{};
			for (const auto &action_name : dom_symbol_list.action)
			{
				auto action_document = XMLDocument{};
				FileSystem::read_xml(fmt::format("{}/library/action/{}.xml", source, action_name), &action_document);
				auto frame_node = AnimFrameNode{};
				parse_sprite<true>(frame_node, action_name, &action_document, dom_symbol_list);
				action_map[action_name] = frame_node;
			}
			auto action_index = tsl::ordered_map<std::string, ActionFrameNode>{};
			get_action_index(action_index, &dom_document);
			merge_action(action_map, action_index);
			animation.version = record_info.version;
			animation.main_sprite.work_area.duration = animation.main_sprite.frame.size();
			parse_dom_document(action_index, &dom_document);
			for (const auto &[name, list]: record_info.sprite) {
				for (const auto &sprite_name : list) {
					for (auto &sprite : animation.sprite) {
						if (sprite_name == sprite.name) {
							sprite.name = name;
							break;
						}
					}
				}
			}
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination
		) -> void
		{
			auto convert = FromFlash{};
			auto record_info = *FileSystem::read_json(fmt::format("{}/record.json", source));
			convert.process(source, record_info);
			FileSystem::write_json(destination, convert.animation);
			return;
		}
	};
}