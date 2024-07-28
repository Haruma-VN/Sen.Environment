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
			FlashPackage &flash_package,
			ExtraInfo const &extra,
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
						auto label_name = element.substr(6_size, element.size() - 10_size);
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
			AnimationImage &image,
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

		inline static auto check_and_take(
			int &value,
			int const &data) -> void
		{
			if (value < data)
			{
				value = data;
			}
			return;
		}

		template <auto sprite_type>
		inline static auto exchange_sprite_document(
			std::string const &sprite_name,
			PackageLibrary &package_library,
			FrameNodeStructure &frame_node_structure,
			XMLDocument &document) -> void
		{
			static_assert(sizeof(sprite_type) == sizeof(SpriteType));
			auto dom_symbol_item = document.FirstChildElement("DOMSymbolItem");
			assert_conditional(dom_symbol_item != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMSymbolItem")), sprite_name), "exchange_sprite_document");
			if constexpr (sprite_type == SpriteType::sprite)
			{
				auto name = fmt::format("sprite/{}", sprite_name);
				auto name_child = dom_symbol_item->FindAttribute("name");
				assert_conditional((name_child != nullptr || name == std::string{name_child->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_name")), sprite_name), "exchange_sprite_document");
			}
			if constexpr (sprite_type == SpriteType::label)
			{
				auto name = fmt::format("label/{}", sprite_name);
				auto name_child = dom_symbol_item->FindAttribute("name");
				assert_conditional((name_child != nullptr || name == std::string{name_child->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_name")), sprite_name), "exchange_sprite_document");
			}
			if constexpr (sprite_type == SpriteType::main_sprite)
			{
				auto name_child = dom_symbol_item->FindAttribute("name");
				assert_conditional((name_child != nullptr || name_child->Value() == "main_sprite"), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_name")), sprite_name), "exchange_sprite_document");
			}
			auto timeline = dom_symbol_item->FirstChildElement("timeline");
			assert_conditional(timeline != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_timeline")), sprite_name), "exchange_sprite_document");
			auto dom_timeline = timeline->FirstChildElement("DOMTimeline");
			assert_conditional(dom_timeline != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMtimeline")), sprite_name), "exchange_sprite_document");
			auto dom_name = dom_timeline->FindAttribute("name");
			assert_conditional((dom_name != nullptr || sprite_name == std::string{dom_name->Value()}), String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_name")), sprite_name), "exchange_sprite_document");
			auto layers = dom_timeline->FirstChildElement("layers");
			assert_conditional(layers != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_layers")), sprite_name), "exchange_sprite_document");
			auto dom_layer_index = k_begin_index;
			auto last_empty_frame = k_begin_index_int;
			auto last_node_frame = k_begin_index_int;
			for (auto dom_layer = layers->LastChildElement("DOMLayer"); dom_layer != nullptr; dom_layer = dom_layer->PreviousSiblingElement("DOMLayer"))
			{
				assert_conditional(dom_layer != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMLayer")), sprite_name), "exchange_sprite_document");
				auto frames = dom_layer->FirstChildElement("frames");
				assert_conditional(frames != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_frames")), sprite_name), "exchange_sprite_document");
				auto &frame_node_list = frame_node_structure[dom_layer_index];
				++dom_layer_index;
				for (auto dom_frame = frames->FirstChildElement("DOMFrame"); dom_frame != nullptr; dom_frame = dom_frame->NextSiblingElement("DOMFrame"))
				{
					assert_conditional(dom_frame != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMFrame")), sprite_name), "exchange_sprite_document");
					auto frame_index = static_cast<int>(std::stoi(dom_frame->FindAttribute("index")->Value()));
					auto m_duration = dom_frame->FindAttribute("duration");
					auto frame_duration = static_cast<int>(std::stoi((m_duration ? m_duration->Value() : "1")));
					auto elements = dom_frame->FirstChildElement("elements");
					auto dom_symbol_instance = elements->FirstChildElement("DOMSymbolInstance");
					if (dom_symbol_instance == nullptr)
					{
						continue;
					}
					auto library_item_name = std::string{dom_symbol_instance->FindAttribute("libraryItemName")->Value()};
					auto name_match = std::regex("(image|sprite)");
					auto match_result = std::smatch{};
					auto regex_result = std::regex_search(library_item_name, match_result, name_match);
					assert_conditional(regex_result, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_name_match")), sprite_name), "exchange_sprite_document");
					assert_conditional(match_result[0] == match_result[1], String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.mismatch_name_match")), sprite_name), "exchange_sprite_document");
					auto is_sprite = match_result[0].str() == "sprite";
					auto resource_name = library_item_name.substr(match_result[0].str().size() + 1_size);
					auto resource = k_native_index;
					auto resource_index = k_begin_index;
					for (auto &[l_name, e] : (is_sprite ? package_library.sprite : package_library.image))
					{
						if (resource_name == l_name)
						{
							resource = static_cast<int>(resource_index);
							break;
						}
						++resource_index;
					}
					assert_conditional(resource != k_native_index, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.cannot_find_resource_name")), resource_name), "exchange_sprite_document"); // TODO: add to locailzation.
					auto frame_node = FrameNode{
						.index = frame_index,
						.duration = frame_duration,
						.resource = resource,
						.sprite = is_sprite};
					auto matrix = dom_symbol_instance->FirstChildElement("matrix");
					if (matrix != nullptr)
					{
						auto x_matrix = matrix->FirstChildElement("Matrix");
						assert_conditional(x_matrix != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_matrix")), sprite_name), "exchange_sprite_document");
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
						assert_conditional(x_color != nullptr, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_sprite_color")), sprite_name), "exchange_sprite_document");
						exchange_color(x_color, frame_node.color);
					}
					else
					{
						frame_node.color = k_initial_color;
					}
					frame_node_list.emplace_back(frame_node);
				}
			}
			for (auto &element : frame_node_structure)
			{
				auto layer_index = element.first;
				if (frame_node_structure[layer_index].size() == k_none_size)
				{
					frame_node_structure.erase(layer_index);
				}
			}
			return;
		}

		inline static auto exchange_frame_node(
			std::vector<AnimationFrame> &frame_list,
			FrameNodeStructure &frame_node_structure) -> void
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
						assert_conditional(frame_node.resource == model.resource && frame_node.sprite == model.sprite, "invalid_instance", "exchange_frame_node");
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

		inline static auto exchange_label_info(
			tsl::ordered_map<std::string, LabelInfo> &label,
			XMLDocument &document) -> void
		{
			auto dom_document = document.FirstChildElement("DOMDocument");
			auto dom_timeline = dom_document->FirstChildElement("timelines")->FirstChildElement("DOMTimeline");
			assert_conditional(std::string_view{dom_timeline->FindAttribute("name")->Value()} == "animation"_sv, fmt::format("{}", Language::get("popcap.animation.from_flash.document_name_must_be_animation")), "exchange_dom_document");
			auto label_layer = dom_timeline->FirstChildElement("layers")->FirstChildElement("DOMLayer");
			assert_conditional(std::string_view{label_layer->FindAttribute("name")->Value()} == "label"_sv, "label_layer_name_must_be_label", "exchange_dom_document");
			auto label_frames = label_layer->FirstChildElement("frames");
			auto frame_count = k_begin_index_int;
			for (auto dom_frame = label_frames->FirstChildElement("DOMFrame"); dom_frame != nullptr; dom_frame = dom_frame->NextSiblingElement("DOMFrame"))
			{
				assert_conditional(dom_frame != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMFrame")), "exchange_dom_document");
				auto frame_index = std::stoi(dom_frame->FindAttribute("index")->Value());
				auto frame_duration = std::stoi(dom_frame->FindAttribute("duration")->Value());
				frame_count += frame_duration;
				auto label_name = dom_frame->FindAttribute("name");
				assert_conditional(label_name != nullptr, "label_name_can_not_null", "exchange_dom_document"); // TODO: add to localization.
				auto &label_info = label[std::string{label_name->Value()}];
				label_info.start = frame_index;
				label_info.duration = frame_duration;
			}
			return;
		}

		inline static auto exchange_dom_document(
			SexyAnimation &definition,
			tsl::ordered_map<std::string, LabelInfo> const &label,
			XMLDocument &document) -> void
		{
			auto dom_document = document.FirstChildElement("DOMDocument");
			auto frame_rate = document.FirstChildElement("DOMDocument")->FindAttribute("frameRate");
			definition.frame_rate = std::stoi(frame_rate != nullptr ? frame_rate->Value() : "24");
			auto width = document.FirstChildElement("DOMDocument")->FindAttribute("width");
			auto height = document.FirstChildElement("DOMDocument")->FindAttribute("height");
			definition.size = AnimationSize(std::stoi(width != nullptr ? width->Value() : "390"), std::stoi(height != nullptr ? height->Value() : "390"));
			auto dom_timeline = dom_document->FirstChildElement("timelines")->FirstChildElement("DOMTimeline");
			assert_conditional(std::string_view{dom_timeline->FindAttribute("name")->Value()} == "animation"_sv, fmt::format("{}", Language::get("popcap.animation.from_flash.document_name_must_be_animation")), "exchange_dom_document");
			/*
			auto label_layer = dom_timeline->FirstChildElement("layers")->FirstChildElement("DOMLayer");
			assert_conditional(std::string_view{label_layer->FindAttribute("name")->Value()} == "label"_sv, "label_layer_name_must_be_label", "exchange_dom_document"); // TODO: add to localization.
			auto label_frames = label_layer->FirstChildElement("frames");
			auto frame_count = k_begin_index_int;
			for (auto dom_frame = label_frames->FirstChildElement("DOMFrame"); dom_frame != nullptr; dom_frame = dom_frame->NextSiblingElement("DOMFrame")) {
				assert_conditional(dom_frame != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMFrame")), "exchange_dom_document");
				auto frame_index = std::stoi(dom_frame->FindAttribute("index")->Value());
				auto frame_duration = std::stoi(dom_frame->FindAttribute("duration")->Value());
				frame_count += frame_duration;
				auto label_name = dom_frame->FindAttribute("name");
				assert_conditional(label_name != nullptr, "label_name_can_not_null", "exchange_dom_document"); // TODO: add to localization.
				definition.main_sprite.frame[frame_index].label = std::string{label_name->Value()};
			}
			*/
			auto frame_count = k_begin_index_int;
			for (auto &[label_name, label_info] : label)
			{
				frame_count += label_info.duration;
				definition.main_sprite.frame[label_info.start].label = label_name;
			}
			auto action_layer = dom_timeline->FirstChildElement("layers")->FirstChildElement("DOMLayer")->NextSiblingElement("DOMLayer");
			assert_conditional(std::string_view{action_layer->FindAttribute("name")->Value()} == "action"_sv, "action_layer_name_must_be_action", "exchange_dom_document"); // TODO: add to localization.
			auto action_frames = action_layer->FirstChildElement("frames");
			for (auto dom_frame = action_frames->FirstChildElement("DOMFrame"); dom_frame != nullptr; dom_frame = dom_frame->NextSiblingElement("DOMFrame"))
			{
				assert_conditional(dom_frame != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMFrame")), "exchange_dom_document");
				auto frame_index = std::stoi(dom_frame->FindAttribute("index")->Value());
				auto action_script = dom_frame->FirstChildElement("Actionscript");
				if (action_script == nullptr)
				{
					continue;
				}
				auto script = action_script->FirstChildElement("script");
				assert_conditional(script != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_dom_script")), "exchange_dom_document");
				auto script_text = script->FirstChild()->ToText();
				assert_conditional(script_text->CData(), fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_script_cdata")), "exchange_dom_document");
				auto script_list = String{Common::trim(std::string{script_text->Value()})}.split(";");
				for (auto &command : script_list)
				{
					command = trim(command);
					if (command == "stop()")
					{
						definition.main_sprite.frame[frame_index].stop = true;
						continue;
					};
					auto pattern = std::regex("fscommand\\(\"(.*)\", \"(.*)\"\\)");
					auto matches = std::smatch{};
					auto match_result = std::regex_match(command, matches, pattern);
					assert_conditional(match_result, String::format(fmt::format("{}", Language::get("popcap.animation.from_flash.invalid_command")), command), "exchange_dom_document");
					definition.main_sprite.frame[frame_index].command.emplace_back(AnimationCommand{matches[1], matches[2]});
				}
			}
			auto instance_layer = action_layer->NextSiblingElement("DOMLayer");
			assert_conditional(std::string_view{instance_layer->FindAttribute("name")->Value()} == "instance"_sv, "instance_layer_name_must_be_instance", "exchange_dom_document"); // TODO: add to localization.
			auto instance_frames = instance_layer->FirstChildElement("frames");
			if (frame_count > definition.main_sprite.frame.size())
			{
				definition.main_sprite.frame.resize(frame_count);
			}
			for (auto dom_frame = instance_frames->FirstChildElement("DOMFrame"); dom_frame != nullptr; dom_frame = dom_frame->NextSiblingElement("DOMFrame"))
			{
				assert_conditional(dom_frame != nullptr, fmt::format("{}", Language::get("popcap.animation.from_flash.sprite_has_no_DOMFrame")), "exchange_dom_document");
				auto frame_index = std::stoi(dom_frame->FindAttribute("index")->Value());
				auto frame_duration = std::stoi(dom_frame->FindAttribute("duration")->Value());
				frame_count -= frame_duration;
			}
			assert_conditional(frame_count == k_begin_index_int, "main_frame_has_no_vaild_length", "exchange_dom_document");
			if (!definition.main_sprite.frame.back().stop)
			{
				definition.main_sprite.frame.back().stop = true; // fix last stop to true.
			}
			return;
		}

		inline static auto exchange_label(
			tsl::ordered_map<std::string, FrameNodeStructure> const &label_frame_structure,
			tsl::ordered_map<std::string, LabelInfo> &label,
			FrameNodeStructure &frame_node_structure) -> void
		{
			auto first_label = label.begin()->first;
			for (auto &[layer_index, frame_node_list] : label_frame_structure.at(first_label))
			{
				frame_node_structure[frame_node_structure.size()] = frame_node_list;
			}
			for (auto &[label_name, label_info] : label)
			{
				if (label_name == first_label)
				{
					continue;
				}
				auto start_index = label_info.start;
				auto &label_frame_node = label_frame_structure.at(label_name);
				auto min_layer_to_insert = k_begin_index_int;
				auto skip_sort = false;
				for (auto &[layer_index, label_node_list] : label_frame_node)
				{
					auto insert_frame_before = false;
					if (!skip_sort)
					{
						auto &first_frame_in_label_node = label_node_list.front();
						for (auto &[layer_index_k, frame_node_list] : frame_node_structure)
						{
							auto &last_frame_in_node = frame_node_list.back();
							auto last_index = last_frame_in_node.index + last_frame_in_node.duration;
							if (last_index != start_index)
							{
								continue;
							}
							if (last_frame_in_node.sprite == first_frame_in_label_node.sprite && last_frame_in_node.resource == first_frame_in_label_node.resource)
							{
								if (layer_index_k < min_layer_to_insert)
								{
									skip_sort = true;
									break;
								}
								min_layer_to_insert = layer_index_k;
								for (auto &frame_node : label_node_list)
								{
									frame_node_structure[layer_index_k].emplace_back(frame_node);
									auto &last_frame_node = frame_node_structure[layer_index_k][frame_node_structure[layer_index_k].size() - 1];
									last_frame_node.index += start_index;
								}
								insert_frame_before = true;
								break;
							}
						}
					}
					if (!insert_frame_before)
					{
						skip_sort = true;
						auto &insert_frame = frame_node_structure[frame_node_structure.size()];
						insert_frame.insert(insert_frame.end(), label_node_list.begin(), label_node_list.end());
						for (auto &frame_node : insert_frame)
						{
							frame_node.index += start_index;
						}
					}
				}
			}
			return;
		}

		inline static auto add_frame_if_need(
			std::vector<AnimationFrame> &frame_list,
			int const &last_label_frame) -> void
		{
			auto frame_list_last_index = frame_list.size();
			if (last_label_frame > frame_list_last_index)
			{
				frame_list.resize(last_label_frame);
				auto before_last_frame = frame_list[frame_list_last_index - 1];
				for (auto &add : before_last_frame.append)
				{
					if (!check_element_in_vector<int>(frame_list[frame_list_last_index].remove, add.index))
					{
						frame_list[frame_list_last_index].remove.emplace_back(add.index);
					}
				}
				for (auto &change : before_last_frame.change)
				{
					if (!check_element_in_vector<int>(frame_list[frame_list_last_index].remove, change.index))
					{
						frame_list[frame_list_last_index].remove.emplace_back(change.index);
					}
				}
			}
			return;
		}

		inline static auto exchange_definition(
			SexyAnimation &definition,
			ExtraInfo const &extra,
			FlashPackage &flash_package) -> void
		{
			auto &package_library = flash_package.library;
			exchange_simple_definition(definition, extra);
			exchange_default_extra(extra);
			for (auto &[image_name, _v] : package_library.image)
			{
				assert_conditional(extra.image.contains(image_name), fmt::format("cannot find image: {} in data", image_name), "exchange_definition"); // TODO: add to localization.
				auto &image_value = extra.image.at(image_name);
				auto image = AnimationImage{
					.name = !image_value.name.empty() ? image_value.name : image_name,
					.id = image_value.id,
					.size = AnimationSize{
						static_cast<double>(image_value.size.width),
						static_cast<double>(image_value.size.height)}};
				exchange_image_document(image, image_name, package_library.image[image_name]);
				definition.image.emplace_back(image);
			}
			for (auto &element : package_library.sprite)
			{
				auto &sprite_name = element.first;
				exchange_sprite_document<SpriteType::sprite>(sprite_name, package_library, package_library.frame_node, package_library.sprite[sprite_name]);
				auto sprite = AnimationSprite(sprite_name);
				exchange_frame_node(sprite.frame, package_library.frame_node);
				exchange_sprite_duration(sprite);
				definition.sprite.emplace_back(sprite);
			}
			exchange_label_info(package_library.label, flash_package.document);
			auto last_label_frame = package_library.label.size() == k_none_size ? k_begin_index_int : package_library.label.back().second.start + package_library.label.back().second.duration;
			if constexpr (split_label)
			{
				auto label_frame_node = tsl::ordered_map<std::string, FrameNodeStructure>{};
				for (auto &element : package_library.label)
				{
					auto &label_name = element.first;
					exchange_sprite_document<SpriteType::label>(label_name, package_library, label_frame_node[label_name], package_library.label[label_name].document);
				}
				exchange_label(label_frame_node, package_library.label, package_library.frame_node);
			}
			else
			{
				exchange_sprite_document<SpriteType::main_sprite>(std::string{"main_sprite"}, package_library, package_library.frame_node, package_library.main_sprite);
			}
			exchange_frame_node(definition.main_sprite.frame, package_library.frame_node);
			add_frame_if_need(definition.main_sprite.frame, last_label_frame);
			exchange_sprite_duration(definition.main_sprite);
			return;
		}

		inline static auto exchange_sprite_duplicate(
			std::map<std::string, SpriteInfo> const &sprite,
			std::vector<AnimationSprite> &sprite_list) -> void
		{
			for (auto &[sprite_name, sprite_info] : sprite)
			{
				for (auto &sprite_name_changed : sprite_info)
				{
					for (auto &element : sprite_list)
					{
						if (element.name == sprite_name_changed)
						{
							element.name = sprite_name;
							break;
						}
					}
				}
			}
			return;
		}

	public:
		inline static auto process_whole(
			SexyAnimation &definition,
			ExtraInfo const &extra,
			std::string_view source) -> void
		{
			static_assert(split_label == true || split_label == false, "split_label must be true or false");
			auto flash_package = FlashPackage{};
			load_flash_package(flash_package, extra, source);
			exchange_definition(definition, extra, flash_package);
			exchange_dom_document(definition, flash_package.library.label, flash_package.document);
			exchange_sprite_duplicate(extra.sprite, definition.sprite);
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination) -> void
		{
			static_assert(split_label == true || split_label == false, "split_label must be true or false");
			auto definition = SexyAnimation{};
			auto extra = *FileSystem::read_json(fmt::format("{}/data.json", source));
			process_whole(definition, extra, source);
			FileSystem::write_json(destination, definition);
			return;
		}
	};

	using ConvertFromFlashWithMainSprite = FromFlash<false>;

	using ConvertFromFlashWithLabel = FromFlash<true>;
}