#pragma once

#include "kernel/support/popcap/animation/definition.hpp"
#include "kernel/support/popcap/animation/convert/definition.hpp"
#include "kernel/support/popcap/animation/convert/common.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert
{
	template <auto split_label>
	class FromFlash : public Common
	{
	protected:
		inline static auto load_flash_package(
			typename FlashPackage &flash_package,
			typename ExtraInfo const &extra,
			std::string_view source) -> void
		{
			for (auto &[image_name, image_value] : extra.image)
			{
				auto &image_document = flash_package.library.image[image_name];
				FileSystem::read_xml(fmt::format("{}/library/image/{}.xml", source, image_name), &image_document);
			}
			auto &document = flash_package.document;
			FileSystem::read_xml(fmt::format("{}/DOMDocument.xml", source), &document);
			auto symbols_elements = document.FirstChildElement("DOMDocument")->FirstChildElement("symbols");
			assert_conditional(symbols_elements != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.document_has_no_symbols")), "load_flash_package");
			for (auto child = symbols_elements->FirstChildElement("Include"); child != nullptr; child = child->NextSiblingElement("Include"))
			{
				assert_conditional(child != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_symbols_include")), "load_flash_package");
				auto element = std::string{child->FirstAttribute()->Value()};
				if (element.starts_with("sprite"))
				{
					auto sprite_name = element.substr(7_size, element.size() - 11_size);
					auto &sprite_document = flash_package.library.sprite[sprite_name];
					FileSystem::read_xml(fmt::format("{}/library/sprite/{}.xml", source, sprite_name), &sprite_document);
				}
				if constexpr (split_label)
				{
					if (element.starts_with("label"))
					{
						auto label_name = element.substr(7_size, element.size() - 11_size);
						auto &label_document = flash_package.library.label[label_name].document;
						FileSystem::read_xml(fmt::format("{}/library/label/{}.xml", source, label_name), &label_document);
					}
				}
			}
			if constexpr (!split_label)
			{
				auto &main_document = flash_package.library.main_sprite;
				FileSystem::read_xml(fmt::format("{}/library/main_sprite.xml", source), &main_document);
			}
			return;
		}

		inline static auto exchange_image_document(
			typename AnimationImage &image,
			std::string const &image_name,
			XMLDocument &value) -> void
		{
			auto dom_symbol_item = value.FirstChildElement("DOMSymbolItem");
			assert_conditional(dom_symbol_item != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMSymbolItem")), image_name), "exchange_image_document");
			auto image_child = dom_symbol_item->FindAttribute("name");
			assert_conditional((image_child != nullptr && fmt::format("image/{}", image_name) == std::string{image_child->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_name")), image_name), "exchange_image_document");
			auto timeline = dom_symbol_item->FirstChildElement("timeline");
			assert_conditional(timeline != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_timeline")), image_name), "exchange_image_document");
			auto dom_timeline = timeline->FirstChildElement("DOMTimeline");
			assert_conditional(dom_timeline != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMtimeline")), image_name), "exchange_image_document");
			auto document_name = dom_timeline->FindAttribute("name");
			assert_conditional((document_name != nullptr && image_name == std::string{document_name->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_name")), image_name), "exchange_image_document");
			auto layers = dom_timeline->FirstChildElement("layers");
			assert_conditional(layers != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_layers")), image_name), "exchange_image_document");
			auto dom_layer = layers->FirstChildElement("DOMLayer");
			assert_conditional(dom_layer != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMLayer")), image_name), "exchange_image_document");
			auto frames = dom_layer->FirstChildElement("frames");
			assert_conditional(frames != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_frames")), image_name), "exchange_image_document");
			auto dom_frame = frames->FirstChildElement("DOMFrame");
			assert_conditional(dom_frame != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMframe")), image_name), "exchange_image_document");
			auto elements = dom_frame->FirstChildElement("elements");
			assert_conditional(elements != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_elements")), image_name), "exchange_image_document");
			auto dom_bitmap_instance = elements->FirstChildElement("DOMBitmapInstance");
			assert_conditional(dom_bitmap_instance != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_DOMBitmapInstance")), image_name), "exchange_image_document");
			auto media_name = std::string{dom_bitmap_instance->FindAttribute("libraryItemName")->Value()}.substr(6_size);
			assert_conditional(media_name == image.name, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_name")), image_name), "exchange_image_document");
			auto matrix = dom_bitmap_instance->FirstChildElement("matrix");
			auto image_transform_matrix = Transform{};
			if (matrix == nullptr)
			{
				image_transform_matrix = k_initial_transform;
			}
			else
			{
				assert_conditional(matrix != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.image_has_no_matrix")), image_name), "exchange_image_document");
				auto transform_matrix = matrix->FirstChildElement("Matrix");
				assert_conditional(transform_matrix != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_image_matrix")), image_name), "exchange_image_document");
				exchange_transform(transform_matrix, image_transform_matrix);
				image_transform_matrix[0] /= k_media_scale_ratio;
				image_transform_matrix[3] /= k_media_scale_ratio;
			}
			k_version < 2 ? exchange_transform_from_standard_to_rotate(image_transform_matrix, image.transform) : exchange_tranform_by_copy(image_transform_matrix, image.transform);
			return;
		}

		template <auto sprite_type>
		inline static auto exchange_sprite_document(
			std::string const &sprite_name,
			PackageLibrary &package_library,
			XMLDocument &document) -> void
		{
			static_assert(sizeof(sprite_type) == sizeof(SpriteType));
			auto dom_symbol_item = document.FirstChildElement("DOMSymbolItem");
			assert_conditional(dom_symbol_item != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMSymbolItem")), sprite_name), "exchange_frame_node");
			if constexpr (sprite_type == SpriteType::sprite)
			{
				auto name = fmt::format("sprite/{}", sprite_name);
				auto name_child = dom_symbol_item->FindAttribute("name");
				assert_conditional((name_child != nullptr || name == std::string{name_child->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_name")), sprite_name), "exchange_frame_node");
			}
			if constexpr (sprite_type == SpriteType::label)
			{
				auto name = fmt::format("label/{}", sprite_name);
				auto name_child = dom_symbol_item->FindAttribute("name");
				assert_conditional((name_child != nullptr || name == std::string{name_child->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_name")), sprite_name), "exchange_frame_node");
			}
			if constexpr (sprite_type == SpriteType::main_sprite)
			{
				auto name_child = dom_symbol_item->FindAttribute("name");
				assert_conditional((name_child != nullptr || name_child->Value() == "main_sprite"), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_name")), sprite_name), "exchange_frame_node");
			}
			auto timeline = dom_symbol_item->FirstChildElement("timeline");
			assert_conditional(timeline != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_timeline")), sprite_name), "exchange_frame_node");
			auto dom_timeline = timeline->FirstChildElement("DOMTimeline");
			assert_conditional(dom_timeline != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMtimeline")), sprite_name), "exchange_frame_node");
			auto dom_name = dom_timeline->FindAttribute("name");
			assert_conditional((dom_name != nullptr || sprite_name == std::string{dom_name->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_name")), sprite_name), "exchange_frame_node");
			auto layers = dom_timeline->FirstChildElement("layers");
			assert_conditional(layers != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_layers")), sprite_name), "exchange_frame_node");
			auto &frame_node_structure = package_library.frame_node;
			auto dom_layer_index = static_cast<int>(k_begin_index);
			for (auto dom_layer = layers->LastChildElement("DOMLayer"); dom_layer != nullptr; dom_layer = dom_layer->PreviousSiblingElement("DOMLayer"))
			{
				assert_conditional(dom_layer != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMLayer")), sprite_name), "exchange_frame_node");
				auto frames = dom_layer->FirstChildElement("frames");
				assert_conditional(frames != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_frames")), sprite_name), "exchange_frame_node");
				auto &frame_node_list = frame_node_structure[dom_layer_index];
				++dom_layer_index;
				for (auto dom_frame = frames->FirstChildElement("DOMFrame"); dom_frame != nullptr; dom_frame = dom_frame->NextSiblingElement("DOMFrame"))
				{
					assert_conditional(dom_frame != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMFrame")), sprite_name), "exchange_frame_node");
					auto frame_index = static_cast<int>(std::stoi(dom_frame->FindAttribute("index")->Value()));
					auto m_duration = dom_frame->FindAttribute("duration");
					auto frame_duration = static_cast<int>(std::stoi((m_duration ? m_duration->Value() : "1")));
					auto elements = dom_frame->FirstChildElement("elements");
					auto dom_symbol_instance = elements->FirstChildElement("DOMSymbolInstance");
					if (dom_symbol_instance == nullptr)
					{
						//frame_node_list.emplace_back(FrameNode{.index = k_native_index});
						continue;
					}
					auto library_item_name = std::string{dom_symbol_instance->FindAttribute("libraryItemName")->Value()};
					auto name_match = std::regex("(image|sprite)");
					auto match_result = std::smatch{};
					auto regex_result = std::regex_search(library_item_name, match_result, name_match);
					assert_conditional(regex_result, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_name_match")), sprite_name), "exchange_frame_node");
					assert_conditional(match_result[0] == match_result[1], String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.mismatch_name_match")), sprite_name), "exchange_frame_node");
					auto is_sprite = match_result[0].str() == "sprite";
					auto resource_name = library_item_name.substr(match_result[0].str().size() + 1_size);
					auto resource = k_native_index;
					if (is_sprite)
					{
						auto resource_index = k_begin_index;
						for (auto &[sprite_name, e] : package_library.sprite)
						{
							if (resource_name == sprite_name)
							{
								resource = static_cast<int>(resource_index);
								break;
							}
							++resource_index;
						}
					}
					else
					{
						for (auto image_index : Range(package_library.image.size()))
						{
							auto image_name = fmt::format("image_{}", image_index + 1_size);
							if (resource_name == image_name)
							{
								resource = static_cast<int>(image_index);
								break;
							}
						}
					}
					assert_conditional(resource != k_native_index, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.cannot_find_sprite")), sprite_name), "exchange_frame_node");
					auto frame_node = FrameNode{
						.index = frame_index,
						.duration = frame_duration,
						.resource = resource,
						.sprite = is_sprite};
					auto matrix = dom_symbol_instance->FirstChildElement("matrix");
					if (matrix != nullptr)
					{
						auto x_matrix = matrix->FirstChildElement("Matrix");
						assert_conditional(x_matrix != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_matrix")), sprite_name), "exchange_frame_node");
						exchange_transform(x_matrix, frame_node.transform);
					}
					else
					{
						frame_node.transform = k_initial_transform;
					}
					auto color_child = dom_symbol_instance->FirstChildElement("color");
					if (color_child != nullptr)
					{
						auto x_color = color_child->FirstChildElement("Color");
						assert_conditional(x_color != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_color")), sprite_name), "exchange_frame_node");
						exchange_color(x_color, frame_node.color);
					}
					else
					{
						frame_node.color = k_initial_color;
					}
					frame_node_list.emplace_back(frame_node);
				}
			}
			return;
		}

		inline static auto exchange_frame_node(
			typename std::vector<AnimationFrame> &frame_list,
			typename FrameNodeStructure &frame_node_structure) -> void
		{
			auto model = Model{};
			auto model_clear = [&]() -> void
			{
				model.index = k_begin_index_int;
				model.state = State::state_null;
				model.resource = k_begin_index_int;
				model.frame_start = k_begin_index_int;
				model.frame_duration = k_begin_index_int;
				model.sprite = k_false;
				model.transform = k_initial_transform;
				model.color = k_initial_color;
				return;
			};
			model_clear();
			auto layer_count = static_cast<int>(k_begin_index);
			for (auto &[layer_index, frame_node_list] : frame_node_structure)
			{
				auto close_current_model_if_need = [&]() -> void
				{
					if (model.state != State::state_null)
					{
						auto frame_index = model.frame_start + model.frame_duration;
						if (frame_index >= frame_list.size())
						{
							frame_list.resize(static_cast<size_t>(frame_index + 1));
						}
						auto &target_frame = frame_list[frame_index];
						target_frame.remove.emplace_back(static_cast<int>(model.index));
						model_clear();
					}
				};
				for (auto &frame_node : frame_node_list)
				{
					auto frame_index = frame_node.index;
					auto frame_duration = frame_node.duration;
					/*
					if (frame_index == k_native_index) {
						close_current_model_if_need();
						continue;
					}
					*/
					if (frame_index >= frame_list.size())
					{
						frame_list.resize(static_cast<size_t>(frame_index + 1));
					}
					auto &target_frame = frame_list[frame_index];
					if (model.state == State::state_null)
					{
						model.state = State::state_true;
						model.index = layer_count;
						model.resource = frame_node.resource;
						model.sprite = frame_node.sprite;
						model.frame_start = frame_index;
						model.frame_duration = frame_duration;
						model.color = k_initial_color;
						target_frame.append.emplace_back(
							AnimationAppend{
								.index = model.index,
								.resource = static_cast<uint16_t>(frame_node.resource),
								.sprite = frame_node.sprite});
						++layer_count;
					}
					else
					{
						assert_conditional(frame_node.resource == model.resource && frame_node.sprite == model.sprite, "invaild_instance", "exchange_frame_node");
					}
					model.frame_start = frame_index;
					model.frame_duration = frame_duration;
					auto &color = frame_node.color;
					auto color_is_changed = !(model.color[0] == color[0] && model.color[1] == color[1] && model.color[2] == color[2] && model.color[3] == color[3]);
					auto frame_change = AnimationChange{.index = model.index};
					exchange_transform_from_standard_to_variant(frame_node.transform, frame_change.transform);
					if (color_is_changed)
					{
						model.color = frame_node.color;
						frame_change.color = frame_node.color;
					}
					target_frame.change.emplace_back(frame_change);
				}
				close_current_model_if_need();
			}
			frame_list.erase(frame_list.end() - 1);
			frame_node_structure.clear();
			return;
		}

		inline static auto exchange_sprite_duration(
			AnimationSprite &sprite) -> void
		{
			sprite.work_area.duration = static_cast<int16_t>(sprite.frame.size() - 1_size);
			return;
		}

		inline static auto exchange_definition(
			typename SexyAnimation &definition,
			typename ExtraInfo const &extra,
			typename PackageLibrary &package_library) -> void
		{

			exchange_simple_definition(definition, extra);
			exchange_default_extra(extra);
			for (auto image_index : Range(definition.image.size()))
			{
				auto image_name = fmt::format("image_{}", image_index + 1_size);
				exchange_image_document(definition.image[image_index], image_name, package_library.image[image_name]);
			}
			for (auto &element : package_library.sprite)
			{
				auto &sprite_name = element.first;
				exchange_sprite_document<SpriteType::sprite>(sprite_name, package_library, package_library.sprite[sprite_name]);
				auto sprite = AnimationSprite(sprite_name);
				exchange_frame_node(sprite.frame, package_library.frame_node);
				exchange_sprite_duration(sprite);
				definition.sprite.emplace_back(sprite);
			}
			if constexpr (split_label)
			{
				//exchange_sprite_document<SpriteType::sprite>(sprite_name, package_library);
			}
			{
				exchange_sprite_document<SpriteType::main_sprite>("main_sprite", package_library, package_library.main_sprite);
				exchange_frame_node(definition.main_sprite.frame, package_library.frame_node);
			}
			exchange_sprite_duration(definition.main_sprite);
			return;
		}

	public:
		inline static auto process_whole(
			typename SexyAnimation &definition,
			typename ExtraInfo const &extra,
			std::string_view source) -> void
		{
			static_assert(split_label == true || split_label == false, "split_label must be true or false");
			auto flash_package = FlashPackage{};
			load_flash_package(flash_package, extra, source);
			exchange_definition(definition, extra, flash_package.library);
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination) -> void
		{
			static_assert(split_label == true || split_label == false, "split_label must be true or false");
			auto definition = SexyAnimation{};
			auto extra = *FileSystem::read_json(fmt::format("{}/extra.json", source));
			process_whole(definition, extra, source);
			FileSystem::write_json(destination, definition);
			return;
		}
	};

	using ConvertFromFlashWithMainSprite = FromFlash<false>;

	using ConvertFromFlashWithLabel = FromFlash<true>;
}