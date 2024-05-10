#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/animation/definition.hpp"
#include "kernel/support/popcap/animation/convert/definition.hpp"

namespace Sen::Kernel::Support::PopCap::Animation::Convert
{

	class ToFlash : public Common
	{

	private:
		using XMLDocument = tinyxml2::XMLDocument;

		using XMLElement = tinyxml2::XMLElement;

		using RecordInfo = RecordInfo;

		using Common = Common;

		using Animation = SexyAnimation;

		using FrameList = FrameList;

	protected:
		template <auto point, typename T>
			requires std::is_integral<T>::value or std::is_floating_point<T>::value
		inline static auto to_fixed(
			T number) -> std::string
		{
			static_assert(sizeof(point) == sizeof(int));
			auto stream = std::ostringstream{};
			stream << std::fixed << std::setprecision(static_cast<std::streamsize>(point)) << number;
			return stream.str();
		}

		template <typename T>
			requires std::is_integral<T>::value or std::is_floating_point<T>::value
		inline static auto variant_to_standard(
			std::array<T, 6> &transform,
			const std::vector<T> &variant_transform) -> void
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
			{
				throw Exception(fmt::format("{}", Language::get("popcap.animation.from_animation.invalid_transform")), std::source_location::current(), "variant_to_standard");
			}
			}
		}
		inline static auto constexpr convert_transform = variant_to_standard<double>;

		template <typename T>
			requires std::is_integral<T>::value or std::is_floating_point<T>::value
		inline static auto valid_color(
			std::array<T, 4> &color,
			const std::vector<T> &base_color) -> void
		{
			if (!base_color.empty())
			{
				for (auto i : Range<int>(4))
				{
					if (base_color[i] != initial_color[i])
					{
						color = {base_color[0], base_color[1], base_color[2], base_color[3]};
						return;
					}
				}
			}
			return;
		}

		inline static auto constexpr copy_color = valid_color<double>;

		template <typename T>
		inline static auto has_duplicates(
			const std::vector<T> &vec) -> int
		{
			auto unique_elements = std::set<T>{};
			for (const auto &i : Range<int>(vec.size()))
			{
				const auto &element = vec[i];
				if (unique_elements.count(element) > 0)
				{
					return i;
				}
				unique_elements.insert(element);
			}
			return -1;
		}

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
		inline auto write_image(
			const AnimationImage &image,
			XMLDocument *document,
			T scale) -> void
		{
			assert_conditional(image.transform.size() == 6, "transform size must be 6 to write image", "write_image");
			auto DOMSymbolItem = document->NewElement("DOMSymbolItem");
			DOMSymbolItem->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
			DOMSymbolItem->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
			DOMSymbolItem->SetAttribute("name", fmt::format("image/{}", image.id).data());
			DOMSymbolItem->SetAttribute("symbolType", "graphic");
			auto timeline = document->NewElement("timeline");
			auto DOMTimeline = document->NewElement("DOMTimeline");
			DOMTimeline->SetAttribute("name", image.id.data());
			auto layers = document->NewElement("layers");
			auto DOMLayer = document->NewElement("DOMLayer");
			auto frames = document->NewElement("frames");
			auto DOMFrame = document->NewElement("DOMFrame");
			DOMFrame->SetAttribute("index", 0);
			auto elements = document->NewElement("elements");
			auto DOMBitmapInstance = document->NewElement("DOMBitmapInstance");
			DOMBitmapInstance->SetAttribute("libraryItemName", fmt::format("media/{}", image.name).data());
			auto matrix = document->NewElement("matrix");
			auto Matrix = document->NewElement("Matrix");
			Matrix->SetAttribute("a", to_fixed<6, double>(image.transform[0] * scale).data());
			Matrix->SetAttribute("b", to_fixed<6, double>(image.transform[1]).data());
			Matrix->SetAttribute("c", to_fixed<6, double>(image.transform[2]).data());
			Matrix->SetAttribute("d", to_fixed<6, double>(image.transform[3] * scale).data());
			Matrix->SetAttribute("tx", to_fixed<6, double>(image.transform[4]).data());
			Matrix->SetAttribute("ty", to_fixed<6, double>(image.transform[5]).data());
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
		inline auto write_sprite(
			const std::string &name,
			std::map<int, std::vector<FrameNode>> &frame_node_list,
			const std::vector<std::string> &sprite_name_list,
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
			frame_node_list.erase(0);
			auto name_layer_index = 1;
			for (const auto &[layer_index, frame_node] : frame_node_list)
			{
				auto DOMLayer = document->NewElement("DOMLayer");
				auto frames = document->NewElement("frames");
				for (const auto &node : frame_node)
				{
					const auto &transform = node.transform;
					const auto &base_color = node.color;
					const auto &resource = node.resource;
					auto DOMFrame = document->NewElement("DOMFrame");
					DOMFrame->SetAttribute("index", fmt::format("{}", node.index).data());
					DOMFrame->SetAttribute("duration", fmt::format("{}", node.duration).data());
					auto elements = document->NewElement("elements");
					if (check_base_frame(transform, base_color, resource))
					{
						auto DOMSymbolInstance = document->NewElement("DOMSymbolInstance");
						if (node.sprite)
						{
							DOMSymbolInstance->SetAttribute("libraryItemName", fmt::format("sprite/{}", sprite_name_list[resource]).data());
							DOMSymbolInstance->SetAttribute("firstFrame", fmt::format("{}", node.first_frame).data());
						}
						else
						{
							DOMSymbolInstance->SetAttribute("libraryItemName", fmt::format("image/{}", animation.image[resource].id).data());
						}
						DOMSymbolInstance->SetAttribute("symbolType", "graphic");
						DOMSymbolInstance->SetAttribute("loop", "loop");
						auto matrix = document->NewElement("matrix");
						auto Matrix = document->NewElement("Matrix");
						Matrix->SetAttribute("a", to_fixed<6, double>(transform[0]).data());
						Matrix->SetAttribute("b", to_fixed<6, double>(transform[1]).data());
						Matrix->SetAttribute("c", to_fixed<6, double>(transform[2]).data());
						Matrix->SetAttribute("d", to_fixed<6, double>(transform[3]).data());
						Matrix->SetAttribute("tx", to_fixed<6, double>(transform[4]).data());
						Matrix->SetAttribute("ty", to_fixed<6, double>(transform[5]).data());
						auto color = document->NewElement("color");
						auto Color = document->NewElement("Color");
						Color->SetAttribute("redMultiplier", to_fixed<6, double>(base_color[0]).data());
						Color->SetAttribute("greenMultiplier", to_fixed<6, double>(base_color[1]).data());
						Color->SetAttribute("blueMultiplier", to_fixed<6, double>(base_color[2]).data());
						Color->SetAttribute("alphaMultiplier", to_fixed<6, double>(base_color[3]).data());
						matrix->InsertEndChild(Matrix);
						color->InsertEndChild(Color);
						DOMSymbolInstance->InsertEndChild(matrix);
						DOMSymbolInstance->InsertEndChild(color);
						elements->InsertEndChild(DOMSymbolInstance);
					}
					DOMFrame->InsertEndChild(elements);
					frames->InsertEndChild(DOMFrame);
				}
				if (frame_node.size() == 0)
				{
					continue;
				}
				DOMLayer->SetAttribute("name", fmt::format("{}", name_layer_index).data());
				DOMLayer->InsertEndChild(frames);
				layers->InsertFirstChild(DOMLayer);
				++name_layer_index;
			}
			DOMTimeline->InsertEndChild(layers);
			timeline->InsertEndChild(DOMTimeline);
			DOMSymbolItem->InsertEndChild(timeline);
			document->InsertEndChild(DOMSymbolItem);
			return;
		}

		inline auto check_base_frame(
			const std::array<double, 6> &transform,
			const std::array<double, 4> &base_color,
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
				{
					return true;
				}
			}
			if (resource != -1)
			{
				return true;
			}
			return false;
		}

		inline auto write_document(
			const FrameList &frame_list,
			const std::vector<std::string> &sprite_name_list,
			XMLDocument *document) -> void
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
			for (auto &folder : folders_list)
			{
				auto DOMFolderItem = document->NewElement("DOMFolderItem");
				DOMFolderItem->SetAttribute("name", folder.data());
				DOMFolderItem->SetAttribute("isExpanded", "true");
				folders->InsertEndChild(DOMFolderItem);
			}
			DOMDocument->InsertEndChild(folders);
			auto media = document->NewElement("media");
			auto symbols = document->NewElement("symbols");
			for (auto &image : animation.image)
			{
				auto DOMBitmapItem = document->NewElement("DOMBitmapItem");
				DOMBitmapItem->SetAttribute("name", fmt::format("media/{}", image.name).data());
				DOMBitmapItem->SetAttribute("href", fmt::format("media/{}.png", image.name).data());
				media->InsertEndChild(DOMBitmapItem);
				auto Include = document->NewElement("Include");
				Include->SetAttribute("href", fmt::format("image/{}.xml", image.id).data());
				symbols->InsertEndChild(Include);
			}
			DOMDocument->InsertEndChild(media);
			for (const auto &sprite_name : sprite_name_list)
			{
				auto Include = document->NewElement("Include");
				Include->SetAttribute("href", fmt::format("sprite/{}.xml", sprite_name).data());
				symbols->InsertEndChild(Include);
			}
			auto flow_layer = document->NewElement("DOMLayer");
			flow_layer->SetAttribute("name", "flow");
			auto flow_frames = document->NewElement("frames");
			for (const auto &action_name : frame_list.action_name_list)
			{
				auto Include = document->NewElement("Include");
				Include->SetAttribute("href", fmt::format("action/{}.xml", action_name).data());
				symbols->InsertEndChild(Include);
				auto start_index = frame_list.action_list.at(action_name).start_index;
				auto end_index = frame_list.action_list.at(action_name).end_index;
				auto DOMFrame = document->NewElement("DOMFrame");
				DOMFrame->SetAttribute("index", fmt::format("{}", start_index).data());
				DOMFrame->SetAttribute("duration", fmt::format("{}", (end_index - start_index)).data());
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
			auto &main_frame = animation.main_sprite.frame;
			auto prev_end_index = 0;
			auto write_command = false;
			auto write_command_pos = 0;
			auto command_layer = document->NewElement("DOMLayer");
			command_layer->SetAttribute("name", "command");
			auto command_frames = document->NewElement("frames");
			for (auto i : Range<int>(main_frame.size()))
			{
				auto command_script = std::string{};
				if (main_frame[i].stop)
				{
					command_script += "stop();";
					write_command = true;
				}
				for (const auto &command : main_frame[i].command)
				{
					command_script += fmt::format("fscommand(\"{}\", \"{}\");", command.parameter, command.command);
					write_command = true;
				}
				command_script.substr(0, command_script.size() - 1);
				if (write_command)
				{
					if ((i - prev_end_index) > 0)
					{
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
					if (write_command_pos != main_frame.size())
					{
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

		template <auto is_action>
		inline auto decode_frame_list(
			const AnimationSprite &sprite,
			FrameList &frame_list) -> void
		{
			static_assert(is_action == true or is_action == false, "is_action is a boolean value");
			static_assert(sizeof(is_action) == sizeof(bool));
			auto sprite_model = std::map<int, Model>{};
			auto &frame_node_list = frame_list.frame_node_list;
			auto frame_length = static_cast<int>(sprite.frame.size());
			frame_node_list[0] = std::vector<FrameNode>{FrameNode{
				0, frame_length, -1}};
			auto main_label = std::string{};
			for (auto i : Range<int>(frame_length))
			{
				if constexpr (is_action)
				{
					const auto &label = sprite.frame[i].label;
					if (!label.empty())
					{
						if (!main_label.empty())
						{
							frame_list.action_list[main_label].end_index = i;
						}
						main_label = label;
					}
					if (!main_label.empty())
					{
						if (!frame_list.action_list.contains(main_label))
						{
							frame_list.action_list[main_label] = Action{i, 0};
							frame_list.action_name_list.emplace_back(main_label);
						}
						if (i == (frame_length - 1))
						{
							frame_list.action_list[main_label].end_index = i + 1;
						}
					}
				}
				for (const auto &remove_index : sprite.frame[i].remove)
				{
					sprite_model[remove_index].state = 0_byte;
				}
				for (const auto &append : sprite.frame[i].append)
				{
					const auto &index = append.index;
					sprite_model[index] = Model{2_byte, append.resource, append.sprite, i, i};
					frame_node_list[index + 1] = std::vector<FrameNode>{};
					if (i > 0)
					{
						frame_node_list[index + 1].emplace_back(FrameNode{0, i, -1});
					}
				}
				for (const auto &move : sprite.frame[i].change)
				{
					const auto &index = move.index;
					// state is true;
					sprite_model[index].state = 1_byte;
					convert_transform(sprite_model[index].transform, move.transform);
					copy_color(sprite_model[index].color, move.color);
				}
				auto model_keys = Map::keys<int, Model>(sprite_model);
				for (const auto &index : model_keys)
				{
					const auto &layer_index = index + 1;
					if constexpr (is_action)
					{
						if (!main_label.empty())
						{
							if (!includes(frame_list.action_list[main_label].frame_index, layer_index))
							{
								frame_list.action_list[main_label].frame_index.emplace_back(layer_index);
							}
						}
					}
					auto &model = sprite_model[index];
					// state not null
					if (model.state != 2_byte)
					{
						if (frame_node_list[layer_index].size() > 0)
						{
							frame_node_list[layer_index].back().duration = model.frame_duration;
						}
					}
					// state is true;
					if (model.state == 1_byte)
					{
						const auto &first_frame = model.sprite ? static_cast<int>((i - model.frame_start) % animation.sprite[model.resource].frame.size()) : 0;
						auto frame_node = FrameNode{i, 0, model.resource};
						frame_node.sprite = model.sprite;
						frame_node.first_frame = first_frame;
						frame_node.transform = model.transform;
						frame_node.color = model.color;
						frame_node_list[layer_index].emplace_back(frame_node);
						model.state = 2_byte;
						model.frame_duration = 0;
					}
					// state is false;
					if (model.state == 0_byte)
					{
						sprite_model.erase(index);
					}
					++model.frame_duration;
				}
			}
			auto model_keys = Map::keys<int, Model>(sprite_model);
			for (const auto &index : model_keys)
			{
				auto &model = sprite_model[index];
				frame_node_list[(index + 1)].back().duration = model.frame_duration;
				sprite_model.erase(index);
			}
			return;
		}

		inline auto write_action(
			const FrameList &frame_list,
			std::map<std::string, std::map<int, std::vector<FrameNode>>> &action_node_list) -> void
		{
			const auto &frame_node_list = frame_list.frame_node_list;
			for (const auto &label : frame_list.action_name_list)
			{
				const auto &action_node = frame_list.action_list.at(label);
				const auto &start_index = action_node.start_index;
				const auto &end_index = action_node.end_index;
				auto first_frame = std::map<int, std::vector<FrameNode>>{};
				first_frame.emplace(0, std::vector<FrameNode>{});
				action_node_list.emplace(label, first_frame);
				auto last_frames_list = std::vector<int>{};
				auto last_index = 0;
				for (const auto &i : Range<int>(frame_node_list.size()))
				{
					if (!includes(action_node.frame_index, i))
						continue;
					auto action_frame_node = std::vector<FrameNode>{};
					const auto &frame_node = frame_node_list.at(i);
					for (const auto &k : Range<int>(frame_node.size()))
					{
						const auto &index = frame_node[k].index;
						const auto &duration = frame_node[k].duration;
						if (index + duration <= start_index)
							continue;
						if (index >= end_index)
							break;
						auto frame_template = frame_node[k];
						if (index + duration > end_index)
						{
							if (index < end_index)
							{
								if (index < start_index)
								{
									frame_template.index = start_index;
									frame_template.duration -= duration - end_index - index + start_index;
								}
								else
								{
									frame_template.duration -= duration + index - end_index;
								}
							}
							frame_template.index -= start_index;
							action_frame_node.emplace_back(frame_template);
						}
						else
						{
							if (index < start_index)
							{
								frame_template.index = start_index;
								frame_template.duration -= start_index - index;
							}
							frame_template.index -= start_index;
							action_frame_node.emplace_back(frame_template);
						}
						last_frames_list.emplace_back(frame_template.index + frame_template.duration);
					}
					action_node_list[label][i] = action_frame_node;
					last_index = i;
				}
				if (last_frames_list.size() > 0)
				{
					auto last_frame = *std::max_element(last_frames_list.begin(), last_frames_list.end());
					if (last_frame < end_index - start_index)
					{
						action_node_list[label][(last_index + 1)] = std::vector<FrameNode>{FrameNode{0, end_index - start_index, -1}};
					}
				}
				else
				{
					action_node_list[label][(last_index + 1)] = std::vector<FrameNode>{FrameNode{0, end_index - start_index, -1}};
				}
			}
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

		inline auto process(
			std::string_view destination,
			RecordInfo &record,
			int resolution) -> void
		{
			auto scale_ratio = 1200.0f / static_cast<float>(resolution);
			record.version = animation.version;
			record.resolution = resolution;
			for (const auto &image : animation.image)
			{
				record.image[image.id] = ImageInfo{.name = image.name, .size = image.size};
				auto image_document = XMLDocument{};
				write_image<float>(image, &image_document, scale_ratio);
				FileSystem::write_xml(fmt::format("{}/library/image/{}.xml", destination, image.id), &image_document);
			}
			auto sprite_name_list = std::vector<std::string>{};
			for (const auto &sprite : animation.sprite)
			{
				sprite_name_list.emplace_back(sprite.name);
			}
			auto sprite_writed_bank = std::vector<std::string>{};
			for (const auto &i : Range(animation.sprite.size()))
			{
				auto frame_list = FrameList{};
				decode_frame_list<false>(animation.sprite[i], frame_list);
				auto &sprite_name = sprite_name_list[i];
				try
				{
					FileSystem::write_file(fmt::format("{}/library/sprite/{}.xml", destination, sprite_name), "test");
				}
				catch (Exception e)
				{
					if (!record.sprite.contains(animation.sprite[i].name))
					{
						record.sprite[animation.sprite[i].name] = std::vector<std::string>{};
					}
					sprite_name.erase(std::remove_if(sprite_name.begin(), sprite_name.end(), [](char c)
													 { return c == '\\' || c == '/' || c == ':' || c == '*' || c == '?' || c == '<' || c == '>' || c == '|';}),
									  sprite_name.end());
					sprite_name = fmt::format("{}_{}", sprite_name, record.sprite[animation.sprite[i].name].size() + 1);
					record.sprite[animation.sprite[i].name].emplace_back(sprite_name);
				}
				if (check_element_in_vector<std::string, std::string>(sprite_writed_bank, sprite_name) || sprite_name.empty())
				{
					if (!record.sprite.contains(animation.sprite[i].name))
					{
						record.sprite[animation.sprite[i].name] = std::vector<std::string>{};
					}
					if (sprite_name.empty())
					{
						sprite_name = "sprite";
					}
					sprite_name = fmt::format("{}_{}", sprite_name, record.sprite[animation.sprite[i].name].size() + 1);
					record.sprite[animation.sprite[i].name].emplace_back(sprite_name);
				}
				auto sprite_document = XMLDocument{};
				write_sprite<false>(sprite_name, frame_list.frame_node_list, sprite_name_list, &sprite_document);
				FileSystem::write_xml(fmt::format("{}/library/sprite/{}.xml", destination, sprite_name), &sprite_document);
				sprite_writed_bank.emplace_back(sprite_name);
			}
			auto frame_list = FrameList{};
			decode_frame_list<true>(animation.main_sprite, frame_list);
			auto action_node_list = std::map<std::string, std::map<int, std::vector<FrameNode>>>{};
			write_action(frame_list, action_node_list);
			if (has_duplicates(frame_list.action_name_list) != -1)
			{
				throw Exception(fmt::format("{}: {}", Language::get("popcap.animation.convert.to_flash.label_duplicate"), frame_list.action_name_list[has_duplicates(frame_list.action_name_list)]), std::source_location::current(), "process");
			}
			for (const auto &label : frame_list.action_name_list)
			{
				auto action_document = XMLDocument{};
				write_sprite<true>(label, action_node_list.at(label), sprite_name_list, &action_document);
				FileSystem::write_xml(fmt::format("{}/library/action/{}.xml", destination, label), &action_document);
			}
			{
				auto dom_document = XMLDocument{};
				write_document(frame_list, sprite_name_list, &dom_document);
				FileSystem::write_xml(fmt::format("{}/DomDocument.xml", destination), &dom_document);
			}
			FileSystem::write_file(fmt::format("{}/main.xfl", destination), "PROXY-CS5");
			FileSystem::create_directory(fmt::format("{}/library/media", destination));
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination,
			int resolution) -> void
		{
			auto convert = ToFlash{*FileSystem::read_json(source)};
			auto record = RecordInfo{};
			convert.process(destination, record, resolution);
			FileSystem::write_json(fmt::format("{}/record.json", destination), record);
			return;
		}
	};
}