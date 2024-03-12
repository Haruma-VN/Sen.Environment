#pragma once

#include "kernel/support/popcap/animation/definition.hpp"
#include "kernel/support/popcap/animation/convert/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert
{

	class ToFlash : public Common
	{

	private:
		using XMLDocument = tinyxml2::XMLDocument;

		using XMLElement = tinyxml2::XMLElement;

		using ExtraInfo = ExtraInfo;

		using Common = Common;

		using Animation = SexyAnimation;

		using FrameList = FrameList;

	protected:
		template <auto point, typename T>
			requires std::is_integral<T>::value or std::is_floating_point<T>::value
		inline static auto to_fixed(
			T number) -> std::string
		{
			auto stream = std::ostringstream{};
			stream << std::fixed << std::setprecision(static_cast<std::streamsize>(point)) << number;
			return stream.str();
		}

		inline static auto variant_to_standard(
			std::array<double, 6> &transform,
			const std::vector<double> &variant_transform) -> void
		{
			switch (variant_transform.size())
			{
			case 2:
			{
				transform = {1.0f, 0.0f, 0.0f, 1.0f, variant_transform[0], variant_transform[1]};
				break;
			}
			case 3:
			{
				auto cos = Math::cos(variant_transform[0]);
				auto sin = Math::sin(variant_transform[0]);
				transform = {cos, sin, -sin, cos, variant_transform[1], variant_transform[2]};
				break;
			}
			case 6:
			{
				transform = {variant_transform[0], variant_transform[1], variant_transform[2], variant_transform[3], variant_transform[4], variant_transform[5]};
				break;
			}
			default:
				throw Exception(fmt::format("{}", Language::get("popcap.animation.from_animation.invalid_transform")));
			}
		}
		inline static auto constexpr convert_transform = variant_to_standard;

		template <typename T, typename E>
		inline static auto check_element_in_vector(
			const std::vector<T> &v,
			const E &e) -> bool
		{
			if (std::find(v.begin(), v.end(), e) != v.end())
			{
				return true;
			}
			return false;
		}

		inline static auto constexpr includes = check_element_in_vector<int, int>;

	protected:
		Animation animation;

	protected:
		template <typename T>
			requires std::is_integral<T>::value or std::is_floating_point<T>::value
		auto write_image(
			XMLDocument *document,
			std::string_view name,
			nlohmann::ordered_json &image,
			T scale) -> void
		{
			assert_conditional(image["transform"].get<std::vector<float>>().size() == 6, "transform size must be 6 to write image", "write_image");
			auto DOMSymbolItem = document->NewElement("DOMSymbolItem");
			DOMSymbolItem->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
			DOMSymbolItem->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
			DOMSymbolItem->SetAttribute("name", fmt::format("image/{}", name).data());
			DOMSymbolItem->SetAttribute("symbolType", "graphic");
			auto timeline = document->NewElement("timeline");
			auto DOMTimeline = document->NewElement("DOMTimeline");
			DOMTimeline->SetAttribute("name", name.data());
			auto layers = document->NewElement("layers");
			auto DOMLayer = document->NewElement("DOMLayer");
			auto frames = document->NewElement("frames");
			auto DOMFrame = document->NewElement("DOMFrame");
			DOMFrame->SetAttribute("index", 0);
			auto elements = document->NewElement("elements");
			auto DOMBitmapInstance = document->NewElement("DOMBitmapInstance");
			DOMBitmapInstance->SetAttribute("libraryItemName", fmt::format("media/{}", image["name"].get<std::string>()).data());
			auto matrix = document->NewElement("matrix");
			auto Matrix = document->NewElement("Matrix");
			Matrix->SetAttribute("a", to_fixed<6, double>(image["transform"][0].get<double>() * scale).c_str());
			Matrix->SetAttribute("b", to_fixed<6, double>(image["transform"][1].get<double>()).data());
			Matrix->SetAttribute("c", to_fixed<6, double>(image["transform"][2].get<double>()).data());
			Matrix->SetAttribute("d", to_fixed<6, double>(image["transform"][3].get<double>() * scale).data());
			Matrix->SetAttribute("tx", to_fixed<6, double>(image["transform"][4].get<double>()).data());
			Matrix->SetAttribute("ty", to_fixed<6, double>(image["transform"][5].get<double>()).data());
			matrix->InsertEndChild(Matrix);
			DOMBitmapInstance->InsertEndChild(matrix);
			elements->InsertEndChild(DOMBitmapInstance);
			DOMFrame->InsertEndChild(elements);
			frames->InsertEndChild(DOMFrame);
			DOMLayer->InsertEndChild(frames);
			layers->InsertEndChild(DOMLayer);
			DOMTimeline->InsertEndChild(layers);
			timeline->InsertEndChild(DOMTimeline);
			DOMSymbolItem->InsertEndChild(timeline);
			document->InsertEndChild(DOMSymbolItem);
			return;
		}

		template <auto is_action>
		auto decode_frame_list(
			const AnimationSprite &animation_sprite,
			nlohmann::ordered_json &animation_sprite_map,
			const std::vector<std::string> &animation_sprite_name_list,
			FrameList &frame_list
		) -> void
		{
			static_assert(is_action == true or is_action == false, "is_action is a boolean value");
			auto sprite_model = std::unordered_map<int, Model>{};
			auto frame_length = animation_sprite.frame.size();
			auto& frame_node_list = frame_list.frame_node_list;
			frame_node_list.emplace(
				0, std::vector<FrameNode>{FrameNode{
					   0, static_cast<int>(frame_length),
					   -1, false, -1}});
			auto main_label = std::string{};
			for (auto i : Range<int>(frame_length))
			{
				if constexpr (is_action)
				{
					auto label = animation_sprite.frame[i].label;
					if (!label.empty())
					{
						debug(label);
						if (!main_label.empty())
						{
							frame_list.action_list[main_label].duration = i;
						}
						main_label = std::move(label);
					}
					if (!frame_list.action_list.contains(main_label) && !main_label.empty())
					{
						frame_list.action_list.emplace(main_label, Action{i, 0});
					}
					if (i == frame_length - 1 && !main_label.empty())
					{
						frame_list.action_list[main_label].duration = i + 1;
					}
				}
				auto frame_remove_length = animation_sprite.frame[i].remove.size();
				for (auto k : Range<int>(frame_remove_length))
				{
					sprite_model[animation_sprite.frame[i].remove[k]].state = 0;
				}
				auto frame_additives_length = animation_sprite.frame[i].append.size();
				for (auto k : Range<int>(frame_additives_length))
				{
					auto append_index = animation_sprite.frame[i].append[k].index;
					sprite_model.emplace(
						append_index,
						Model{
							0, -1, animation_sprite.frame[i].append[k].resource,
							animation_sprite.frame[i].append[k].sprite,
							i, i});
					frame_node_list.emplace(
						(append_index + 1), std::vector<FrameNode>{});
					if (i > 0)
					{
						frame_node_list[(append_index + 1)].emplace_back(FrameNode{
							0, i, -1, false, -1});
					}
				}
				auto frame_moves_length = animation_sprite.frame[i].change.size();
				for (auto k : Range<int>(frame_moves_length))
				{
					auto layer_index = animation_sprite.frame[i].change[k].index;
					sprite_model[layer_index].state = 1;
					auto transform = std::array<double, 6>{};
					convert_transform(transform, animation_sprite.frame[i].change[k].transform);
					sprite_model[layer_index].transform = transform;
					if (!animation_sprite.frame[i].change[k].color.empty())
					{
						if (animation_sprite.frame[i].change[k].color[0] != initial_color[0] ||
							animation_sprite.frame[i].change[k].color[1] != initial_color[1] ||
							animation_sprite.frame[i].change[k].color[2] != initial_color[2] ||
							animation_sprite.frame[i].change[k].color[3] != initial_color[3])
						{
							sprite_model[layer_index].color[0] = animation_sprite.frame[i].change[k].color[0];
							sprite_model[layer_index].color[1] = animation_sprite.frame[i].change[k].color[1];
							sprite_model[layer_index].color[2] = animation_sprite.frame[i].change[k].color[2];
							sprite_model[layer_index].color[3] = animation_sprite.frame[i].change[k].color[3];
						}
					}
				}
			    //	debug("get_info");
				for (auto &[layer_index, model_layer] : sprite_model)
				{
					auto anim_layer_index = layer_index + 1;
					if constexpr (is_action)
					{
						if (!main_label.empty())
						{
							if (!includes(frame_list.action_list[main_label].frame_index, anim_layer_index))
							{
								frame_list.action_list[main_label].frame_index.emplace_back(anim_layer_index);
							}
						}
					}
					if (model_layer.state != -1)
					{
						auto frame_node_length = frame_node_list.at(anim_layer_index).size();
						if (frame_node_length > 0)
						{
							frame_node_list.at(anim_layer_index).at((frame_node_length - 1)).duration = model_layer.frame_duration;
						}
					}
					if (model_layer.state == 1)
					{
						// debug("start_first_frame");
						auto first_frame = 0;
						if (model_layer.sprite)
						{
							auto s_name = animation_sprite_name_list[model_layer.resource];
							first_frame = (i - model_layer.frame_start) % animation_sprite_map.at(s_name)["frame"].size();
						}
						auto frame_node = FrameNode{i, 0, model_layer.resource, model_layer.sprite, static_cast<int>(first_frame)};
						// debug("first_frame_ok");
						frame_node.transform = model_layer.transform;
						frame_node.color = model_layer.color;
						frame_node_list.at(anim_layer_index).emplace_back(frame_node);
						model_layer.state = -1;
						model_layer.frame_duration = 0;
					}
					if (model_layer.state == 0)
					{
						sprite_model.erase(layer_index);
					}
					++model_layer.frame_duration;
				}
			}
			// debug("run_ok");
			for (auto &[layer_index, model_layer] : sprite_model)
			{
				frame_node_list[(layer_index + 1)].back().duration = model_layer.frame_duration;
				sprite_model.erase(layer_index);
			}
			return;
		}

		template <auto is_action>
		auto write_sprite(
			const std::unordered_map<int, std::vector<FrameNode>> &frame_node_list,
			const std::vector<std::string> &animation_sprite_name_list,
			const std::vector<std::string> &animation_image_id_list,
			const std::string &name,
			XMLDocument *document) -> void
		{
			static_assert(is_action == true or is_action == false, "is_action is a boolean value");
			auto DOMSymbolItem = document->NewElement("DOMSymbolItem");
			DOMSymbolItem->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
			DOMSymbolItem->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
			if constexpr (is_action)
			{
				DOMSymbolItem->SetAttribute("name", fmt::format("action/{}", name).data());
			}
			else
			{
				DOMSymbolItem->SetAttribute("name", fmt::format("sprite/{}", name).data());
			}
			DOMSymbolItem->SetAttribute("symbolType", "graphic");
			auto timeline = document->NewElement("timeline");
			auto DOMTimeline = document->NewElement("DOMTimeline");
			DOMTimeline->SetAttribute("name", name.data());
			auto layers = document->NewElement("layers");
			auto layer_index_list = std::vector<int>{};
			for (auto &[i, o] : frame_node_list)
			{
				layer_index_list.emplace_back(i);
			}
			std::reverse(layer_index_list.begin(), layer_index_list.end());

			layer_index_list.pop_back();
			auto layer_length = layer_index_list.size();
			for (auto &layer_index : layer_index_list)
			{
				auto DOMLayer = document->NewElement("DOMLayer");
				auto &frame_node = frame_node_list.at(layer_index);
				auto frames = document->NewElement("frames");
				for (auto node : frame_node)
				{
					auto &transform = node.transform;
					auto &base_color = node.color;
					auto &resource = node.resource;
					auto DOMFrame = document->NewElement("DOMFrame");
					DOMFrame->SetAttribute("index", fmt::format("{}", node.index).data());
					DOMFrame->SetAttribute("duration", fmt::format("{}", node.duration).data());
					auto elements = document->NewElement("elements");
					if (check_base_frame(transform, base_color, resource))
					{
						auto DOMSymbolInstance = document->NewElement("DOMSymbolInstance");
						if (node.sprite)
						{
							DOMSymbolInstance->SetAttribute("libraryItemName", fmt::format("sprite/{}", animation_sprite_name_list[resource]).data());
							DOMSymbolInstance->SetAttribute("firstFrame", fmt::format("{}", node.first_frame).data());
						}
						else
						{
							DOMSymbolInstance->SetAttribute("libraryItemName", fmt::format("image/{}", animation_image_id_list[resource]).data());
						}
						DOMSymbolInstance->SetAttribute("symbolType", "graphic");
						DOMSymbolInstance->SetAttribute("loop", "loop");
						auto matrix = document->NewElement("matrix");
						auto Matrix = document->NewElement("Matrix");
						Matrix->SetAttribute("a", to_fixed<6, double>(transform[0]).c_str());
						Matrix->SetAttribute("b", to_fixed<6, double>(transform[1]).c_str());
						Matrix->SetAttribute("c", to_fixed<6, double>(transform[2]).c_str());
						Matrix->SetAttribute("d", to_fixed<6, double>(transform[3]).c_str());
						Matrix->SetAttribute("tx", to_fixed<6, double>(transform[4]).c_str());
						Matrix->SetAttribute("ty", to_fixed<6, double>(transform[5]).c_str());
						auto color = document->NewElement("color");
						auto Color = document->NewElement("Color");
						Color->SetAttribute("redMultiplier", to_fixed<6, double>(base_color[0]).c_str());
						Color->SetAttribute("greenMultiplier", to_fixed<6, double>(base_color[1]).c_str());
						Color->SetAttribute("blueMultiplier", to_fixed<6, double>(base_color[2]).c_str());
						Color->SetAttribute("alphaMultiplier", to_fixed<6, double>(base_color[3]).c_str());
						matrix->InsertEndChild(Matrix);
						color->InsertEndChild(Color);
						DOMSymbolInstance->InsertEndChild(matrix);
						DOMSymbolInstance->InsertEndChild(color);
						elements->InsertEndChild(DOMSymbolInstance);
					}
					DOMFrame->InsertEndChild(elements);
					frames->InsertEndChild(DOMFrame);
				}
				DOMLayer->SetAttribute("name", fmt::format("{}", layer_length).data());
				DOMLayer->InsertEndChild(frames);
				layers->InsertEndChild(DOMLayer);
				--layer_length;
			}
			DOMTimeline->InsertEndChild(layers);
			timeline->InsertEndChild(DOMTimeline);
			DOMSymbolItem->InsertEndChild(timeline);
			document->InsertEndChild(DOMSymbolItem);
			return;
		}

		auto check_base_frame(
			const std::array<double, 6Ui64> &transform,
			const std::array<double, 4Ui64> &base_color,
			int resource) -> bool
		{
			for (auto i : Range<int>(transform.size()))
			{
				if (transform[i] != Common::initial_transform[i])
					return true;
			}
			for (auto i : Range<int>(base_color.size()))
			{
				if (base_color[i] != Common::initial_color[i])
					return true;
			}
			if (resource != -1)
				return true;
			return false;
		}

		auto write_action(
			const FrameList &frame_list,
			std::unordered_map<std::string, std::unordered_map<int, std::vector<FrameNode>>> &action_node_list
		)
		{
			auto &action_list = frame_list.action_list;
			const auto &frame_node_list = frame_list.frame_node_list;
			for (auto &[label, action_node]: action_list) {
				auto start_index = action_node.start_index;
				auto action_duration = action_node.duration;
				auto first_frame = std::unordered_map<int, std::vector<FrameNode>>{};
				first_frame.emplace(0, std::vector<FrameNode>{});
				action_node_list.emplace(label, first_frame);
				auto last_frames_list = std::vector<int>{};
				auto last_index = 0;
				for (auto i : Range<int>(frame_node_list.size())) {
					auto cnt = count(action_node.frame_index.begin(), action_node.frame_index.end(), i); 
					if (!(cnt > 0)) continue;
					auto action_frame_node = std::vector<FrameNode>{};
				    const auto &frame_node = frame_node_list.at(i);
					for (auto k : Range<int>(frame_node.size())) {
						auto index = frame_node.at(k).index;
						auto duration = frame_node.at(k).duration;
						if (index + duration <= start_index) continue;
						if (index >= action_duration) break;
						auto frame_node_template = FrameNode{
							frame_node.at(k).index,
							frame_node.at(k).duration,
							frame_node.at(k).resource,
							frame_node.at(k).sprite,
							frame_node.at(k).first_frame,
						};
						frame_node_template.transform = frame_node.at(k).transform;
						frame_node_template.color = frame_node.at(k).color;
						if (index + duration > action_duration) {
							if (index < action_duration) {
								if (index < start_index) {
									frame_node_template.index = start_index;
									frame_node_template.duration -= duration - action_duration - index + start_index;
								}
								else {
									frame_node_template.duration -= duration + index - action_duration;
								}
							}
							frame_node_template.index -= start_index;
							action_frame_node.emplace_back(frame_node_template);
						}
						else {
							if (index < start_index) {
								frame_node_template.index = start_index;
								frame_node_template.duration -= start_index - index;
							}
							frame_node_template.index -= start_index;
							action_frame_node.emplace_back(frame_node_template);
						}
						last_frames_list.emplace_back(frame_node_template.index + frame_node_template.duration);
					}
					action_node_list.at(label).emplace(
						i, action_frame_node
					);
					last_index = i;
				}
				auto last_frame = *std::max_element(last_frames_list.begin(), last_frames_list.end());
				if (last_frame < action_duration - start_index) {
					action_node_list.at(label).emplace(
						(last_index + 1), std::vector<FrameNode>{
							FrameNode{
								0, action_duration - start_index, -1, false, -1
							}
						}
					);
				}
			}
			return;
		}

		auto write_document(
			const std::unordered_map<std::string, Action> & action_list,
			const std::vector<std::string> &animation_sprite_name_list,
			const std::vector<std::string> &animation_image_name_list,
			XMLDocument *document
		)
		{
			auto DOMDocument = document->NewElement("DOMDocument");
			DOMDocument->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
			DOMDocument->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
			DOMDocument->SetAttribute("frameRate", fmt::format("{}", animation.frame_rate).data());
			DOMDocument->SetAttribute("width", fmt::format("{}", animation.size.width).data());
			DOMDocument->SetAttribute("height", fmt::format("{}", animation.size.height).data());
			DOMDocument->SetAttribute("xflVersion", "2.971");
			auto folders = document->NewElement("folders");
			auto folders_list = std::vector<std::string>{"media", "action", "image", "sprite"};
			for (const auto & folder : folders_list) {
				auto DOMFolderItem = document->NewElement("DOMFolderItem");
				DOMFolderItem->SetAttribute("name", folder.data());
				DOMFolderItem->SetAttribute("isExpanded", "true");
				folders->InsertEndChild(DOMFolderItem);
			}
			DOMDocument->InsertEndChild(folders);
			auto media = document->NewElement("media");
			for (const auto & [i, v] : animation.image.items()) {
				auto DOMBitmapItem = document->NewElement("DOMBitmapItem");
				auto media_name = v["name"].get<std::string>();
				DOMBitmapItem->SetAttribute("name", fmt::format("media/{}", media_name).data());
				DOMBitmapItem->SetAttribute("href",  fmt::format("media/{}.png", media_name).data());
				media->InsertEndChild(DOMBitmapItem);
			}
			DOMDocument->InsertEndChild(media);
			auto symbols = document->NewElement("symbols");
			for (const auto & image_name : animation_image_name_list) {
				auto Include = document->NewElement("Include");
				Include->SetAttribute("href", fmt::format("image/{}.xml", image_name).data());
				symbols->InsertEndChild(Include);
			}
			for (const auto & sprite_name : animation_sprite_name_list) {
				auto Include = document->NewElement("Include");
				Include->SetAttribute("href", fmt::format("sprite/{}.xml", sprite_name).data());
				symbols->InsertEndChild(Include);
			}
			auto flow_layer = document->NewElement("DOMLayer");
			flow_layer->SetAttribute("name", "flow");
			auto flow_frames = document->NewElement("frames");
			for (const auto &[action_name, v]: action_list) {
				auto Include = document->NewElement("Include");
				Include->SetAttribute("href",  fmt::format("action/{}.xml", action_name).data());
				symbols->InsertEndChild(Include);
				auto start_index = v.start_index;
				auto action_duration = v.duration;
				auto DOMFrame = document->NewElement("DOMFrame");
				DOMFrame->SetAttribute("index", fmt::format("{}", start_index).data());
				DOMFrame->SetAttribute("duration", fmt::format("{}", (action_duration - start_index)).data());
				DOMFrame->SetAttribute("name", fmt::format("{}", action_name).data());
				DOMFrame->SetAttribute("labelType", "name");
				auto elements = document->NewElement("elements");
				auto DOMSymbolInstance = document->NewElement("DOMSymbolInstance");
				DOMSymbolInstance->SetAttribute("libraryItemName", ("action/" + action_name).data());
				DOMSymbolInstance->SetAttribute("symbolType", "graphic");
				DOMSymbolInstance->SetAttribute("loop", "loop");
				elements->InsertEndChild(DOMSymbolInstance);
				DOMFrame->InsertEndChild(elements);
				flow_frames->InsertEndChild(DOMFrame);
			}
			DOMDocument->InsertEndChild(symbols);
			flow_layer->InsertEndChild(flow_frames);
			auto & main_frame = animation.main_sprite.frame;
			auto prev_end_index = 0;
			auto write_command = false;
			auto write_command_pos = 0;
			auto command_layer = document->NewElement("DOMLayer");
			command_layer->SetAttribute("name", "command");
			auto command_frames = document->NewElement("frames");
			for (auto i : Range<int>(main_frame.size())) {
				auto command_script = std::string{};
				if (main_frame.at(i).stop) {
					command_script += "stop();";
					write_command = true;
				}
				for (auto k : Range<int>(main_frame.at(i).command.size())) {
					auto command = fmt::format("fscommand(""{}", "{}"")", main_frame.at(i).command.at(k).parameter, main_frame.at(i).command.at(k).command);
					command_script += command;
					write_command = true;
				}	
				if (write_command) {
					if ((i - prev_end_index) > 0) {
						auto DOMFrame = document->NewElement("DOMFrame");
						DOMFrame->SetAttribute("index", fmt::format("{}", prev_end_index).data());
						DOMFrame->SetAttribute("duration", fmt::format("{}", (i - prev_end_index)).data());
						auto elements = document->NewElement("elements");
						DOMFrame->InsertEndChild(elements);
						command_frames->InsertEndChild(DOMFrame);
					}
					prev_end_index = i;
					auto DOMFrame = document->NewElement("DOMFrame");
					DOMFrame->SetAttribute("index", fmt::format("{}", prev_end_index).data());
					auto Actionscript = document->NewElement("Actionscript");
					auto script = document->NewElement("script");
					auto cdata = document->NewText(command_script.data());
					cdata->SetCData(true);
					script->InsertEndChild(cdata);
					auto elements = document->NewElement("elements");
					Actionscript->InsertEndChild(script);
					DOMFrame->InsertEndChild(Actionscript);
					DOMFrame->InsertEndChild(elements);
					command_frames->InsertEndChild(DOMFrame);
					++prev_end_index;
					write_command_pos = prev_end_index;
                    write_command = false;
				}
				if (i == (main_frame.size() - 1)) 
				{
					if (write_command_pos != main_frame.size()) {
						auto DOMFrame = document->NewElement("DOMFrame");
						DOMFrame->SetAttribute("index", fmt::format("{}", prev_end_index).data());
						DOMFrame->SetAttribute("duration", fmt::format("{}", (main_frame.size() - prev_end_index)).data());
						auto elements = document->NewElement("elements");
						DOMFrame->InsertEndChild(elements);
						command_frames->InsertEndChild(DOMFrame);
					}
				}
			}
			command_layer->InsertEndChild(command_frames);
			auto timelines = document->NewElement("timelines");
			auto DOMTimeline = document->NewElement("DOMTimeline");
			DOMTimeline->SetAttribute("name", "animation");
			auto layers = document->NewElement("layers");
			layers->InsertEndChild(flow_layer);
			layers->InsertEndChild(command_layer);
			DOMTimeline->InsertEndChild(layers);
			timelines->InsertEndChild(DOMTimeline);
			DOMDocument->InsertEndChild(timelines);
			document->InsertEndChild(DOMDocument);
			return;
		}

		auto to_animation_sprite(
			const nlohmann::ordered_json &json,
			AnimationSprite &anim)
		{
			json.at("description").get_to(anim.description);
			json.at("work_area").get_to(anim.work_area);
			json.at("frame").get_to(anim.frame);
			return;
		}

	public:
		explicit ToFlash(
			const SexyAnimation &that) : animation(that)
		{
		}

		~ToFlash(

			) = default;

		ToFlash(
			ToFlash &&that) = delete;

		auto operator=(
			ToFlash &&that) -> ToFlash & = delete;

		auto process(
			std::string_view destination,
			int resolution) -> void
		{
			auto scale_ratio = 1200.0f / static_cast<float>(resolution);
			auto extra_info = ExtraInfo{
				.version = animation.version,
			};
			for (auto &[key, value] : animation.image.items())
			{
				extra_info.group[key] = nlohmann::ordered_json{
					{
						"name",
						value["name"],
					},
					{
						"size",
						value["size"],
					},
				};
				auto image_document = XMLDocument{};
				write_image(&image_document, key, value, scale_ratio);
				FileSystem::write_xml(fmt::format("{}/library/image/{}.xml", destination, key), &image_document);
			}
			auto animation_sprite_name_list = Object::keys(animation.sprite);
			auto animation_image_name_list = Object::keys(animation.image);
			for (const auto &sprite_name : animation_sprite_name_list)
			{
				auto frame_list = FrameList{};
				auto animation_sprite = AnimationSprite{};
				to_animation_sprite(animation.sprite.at(sprite_name), animation_sprite);
				debug(sprite_name);
				decode_frame_list<false>(animation_sprite, animation.sprite, animation_sprite_name_list, frame_list);
				auto sprite_document = XMLDocument{};
				write_sprite<false>(frame_list.frame_node_list, animation_sprite_name_list, animation_image_name_list, sprite_name, &sprite_document);
				FileSystem::write_xml(fmt::format("{}/library/sprite/{}.xml", destination, sprite_name), &sprite_document);
			}
			auto frame_list = FrameList{};
			decode_frame_list<true>(animation.main_sprite, animation.sprite, animation_sprite_name_list, frame_list);
			 auto action_node_list = std::unordered_map<std::string, std::unordered_map<int, std::vector<FrameNode>>>{};
			write_action(frame_list, action_node_list);
			for (const auto &[label, value]: action_node_list) {
				auto action_documet = XMLDocument{};
				write_sprite<true>(value, animation_sprite_name_list, animation_image_name_list, label, &action_documet);
				FileSystem::write_xml(fmt::format("{}/library/action/{}.xml", destination, label), &action_documet);
			}
			auto dom_document = XMLDocument{};
			write_document(frame_list.action_list, animation_sprite_name_list, animation_image_name_list, &dom_document);
			FileSystem::write_xml(fmt::format("{}/DomDocument.xml", destination), &dom_document);
			FileSystem::write_file(fmt::format("{}/main.xfl", destination), "PROXY-CS5");
			FileSystem::write_json(fmt::format("{}/extra.json", destination), extra_info);
			FileSystem::create_directory(fmt::format("{}/library/media", destination));
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination,
			int resolution) -> void
		{
			auto convert = ToFlash{*FileSystem::read_json(source)};
			convert.process(destination, resolution);
			return;
		}
	};
}