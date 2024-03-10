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
			T number
		) -> std::string
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
				transform = {1.0f, 0.0f, 0.0f, 0.0f, variant_transform[0], variant_transform[1]};
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
			const std::vector<T> & v,
			const E & e
		) -> bool
		 {
			if (std::find(v.begin(), v.end(), e) != v.end()) {
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

		auto decode_frame_list(
			nlohmann::ordered_json &animation_sprite,
			nlohmann::ordered_json &animation_sprite_map,
			std::vector<std::string> &animation_sprite_name_list,
			std::map<int, std::vector<FrameNode>>& frame_node_list,
			std::map<std::string, Action>& action_list
		) -> void
		{
			auto sprite_model = std::map<int, Model>{};
			auto frame_length = animation_sprite["frame"].size();
			frame_node_list.emplace(
				0, std::vector<FrameNode>{FrameNode{
					   0, static_cast<int>(frame_length),
					   -1, false, -1}});
			auto main_label = std::string{};
			for (auto i : Range<int>(frame_length))
			{
				auto label = animation_sprite["frame"][i]["label"].get<std::string>();
				if (!label.empty())
				{
					if (main_label.empty())
					{
						action_list[main_label].duration = i;
					}
					main_label = std::move(label);
				}
				if (!(action_list.contains(main_label) && main_label.empty()))
				{
					action_list.emplace(main_label, Action{i, 0});
				}
				if (i == frame_length - 1 && !main_label.empty())
				{
					action_list[main_label].duration = i + 1;
				}
				auto frame_remove_length = animation_sprite["frame"][i]["remove"].size();
				for (auto k : Range<int>(frame_remove_length))
				{
					sprite_model[animation_sprite["frame"][i]["remove"][k]].state = 0;
				}
				auto frame_additives_length = animation_sprite_map["frame"][i]["append"].size();
				for (auto k : Range<int>(frame_additives_length))
				{
					auto append_index = animation_sprite_map["frame"][i]["append"][k]["index"].get<int>();
					sprite_model.emplace(
						append_index,
						Model{
							0, -1, animation_sprite_map["frame"][i]["append"][k]["resource"].get<int>(),
							animation_sprite_map["frame"][i]["append"][k]["sprite"].get<bool>(),
							i, i});
					frame_node_list.emplace(
						(append_index + 1), std::vector<FrameNode>{});
					if (i > 0)
					{
						frame_node_list[(append_index + 1)].emplace_back(FrameNode{
							0, i, -1, false, -1});
					}
				}
				auto frame_moves_length = animation_sprite_map["frame"][i]["change"].size();
				for (auto k : Range<int>(frame_moves_length))
				{
					auto layer_index = animation_sprite_map["frame"][i]["change"][k]["index"].get<int>();
					sprite_model[layer_index].state = 1;
					auto transform = std::array<double, 6>{};
					convert_transform(transform, animation_sprite_map["frame"][i]["change"][k]["transform"].get<std::vector<double>>());
					sprite_model[layer_index].transform = transform; 
					if (!animation_sprite_map["frame"][i]["change"][k]["color"].is_null()) {
						if (animation_sprite_map["frame"][i]["change"][k]["color"][0].get<double>() != initial_color[0] ||
							animation_sprite_map["frame"][i]["change"][k]["color"][1].get<double>() != initial_color[1] ||
							animation_sprite_map["frame"][i]["change"][k]["color"][2].get<double>() != initial_color[2] ||
							animation_sprite_map["frame"][i]["change"][k]["color"][3].get<double>() != initial_color[3] 
						) {
							sprite_model[layer_index].color = animation_sprite_map["frame"][i]["change"][k]["color"].get<std::array<double, 4>>();
						}
					}
				}
				for (auto &[layer_index, model_layer] : sprite_model) {
					auto anim_layer_index = layer_index + 1;
					if (!main_label.empty()) {
						if (!includes(action_list[main_label].frame_index, anim_layer_index)) {
							action_list[main_label].frame_index.emplace_back(anim_layer_index);
						}
					}
					if (model_layer.state != -1) {
						auto frame_node_length = frame_node_list[anim_layer_index].size();
						if (frame_node_length > 0) {
							frame_node_list[anim_layer_index][frame_node_length].duration = model_layer.frame_duration;
						}
					}
					if (model_layer.state == 1) {
						auto first_frame = model_layer.sprite ? (i - model_layer.frame_start) % 
						animation_sprite_map[animation_sprite_name_list[model_layer.resource]]["frame"].size() : 0;
						auto frame_node = FrameNode{i, 0, model_layer.resource, model_layer.sprite, static_cast<int>(first_frame)};
						frame_node.transform = model_layer.transform;
						frame_node.color = model_layer.color;
						frame_node_list[anim_layer_index].emplace_back(frame_node);
						model_layer.state = -1;
						model_layer.frame_duration = 0;
					}
					if (model_layer.state == 0) {
						sprite_model.erase(layer_index);
					}
					++model_layer.frame_duration;
				}
			}
			for (auto &[layer_index, model_layer] : sprite_model) {
				auto frame_node_length = frame_node_list[(layer_index + 1)].size();
				frame_node_list[(layer_index + 1)][(frame_node_length - 1)].duration = model_layer.frame_duration;
				sprite_model.erase(layer_index);
			}
			return;
		}

		// template <auto is_action>
		// auto write_sprite(
		// 	std::map<int, std::vector<FrameNode>>& frame_node_list, 
		// 	std::vector<std::string>& animation_sprite_name_list, 
		// 	std::vector<std::string>& animation_image_id_list, 
		// 	std::string &name,
		// 	XMLDocument* document
		// ) -> void
		// {
		// 	static_assert(is_action == true or is_action == false, "is_action is a boolean value");
		// 	auto DOMSymbolItem = document->NewElement("DOMSymbolItem");
		// 	DOMSymbolItem->SetAttribute("xmlns:xsi", "http://www.w3.org/2001/XMLSchema-instance");
		// 	DOMSymbolItem->SetAttribute("xmlns", "http://ns.adobe.com/xfl/2008/");
		// 	if constexpr (is_action) {
		// 		DOMSymbolItem->SetAttribute("name", fmt::format("action/{}", name).data());
		// 	}
		// 	else {
		// 		DOMSymbolItem->SetAttribute("name", fmt::format("sprite/{}", name).data());
		// 	}
		// 	DOMSymbolItem->SetAttribute("symbolType", "graphic");
		// 	auto timeline = document->NewElement("timeline");
		// 	auto DOMTimeline = document->NewElement("DOMTimeline");
		// 	DOMTimeline->SetAttribute("name", name.data());
		// 	auto layers = document->NewElement("layers");
		// 	auto keys = Map::keys<int, FrameNode>(frame_node_list);
		// 	for (auto i = keys.size(); i > 0; --i)
		// 	{
		// 		for (auto & node : frame_node_list[keys[i]])
		// 		{
		// 			auto& transform = node.transform;
        //             auto& base_color = node.color;
        //             auto& resource = node.resource;
		// 			auto sprite_dom_frame = document->NewElement("DOMFrame");
		// 			sprite_dom_frame->SetAttribute("index", node.index);
		// 			sprite_dom_frame->SetAttribute("duration", node.duration);
		// 			auto elements = document->NewElement("elements");
		// 			auto DOMSymbolInstance = document->NewElement("DOMSymbolInstance");
		// 		}
		// 	}
		// 	DOMTimeline->InsertEndChild(layers);
		// 	timeline->InsertEndChild(DOMTimeline);
		// 	DOMSymbolItem->InsertEndChild(timeline);
		// 	document->InsertEndChild(DOMSymbolItem);
		// 	return;
		// }

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
			int resolution
		) -> void
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
			auto keys = Object::keys(animation.sprite);
			auto images = Object::keys(animation.image);
			for (auto &[sprite_name, sprite_value] : animation.sprite.items())
			{
				auto frame_list = FrameList{};
				decode_frame_list(sprite_value, animation.sprite, keys, frame_list.frame_node_list, frame_list.action_list);
				auto sprite_document = XMLDocument{};
			}
			return;
		}

		inline static auto process_fs(
			std::string_view source,
			std::string_view destination,
			int resolution
		) -> void
		{
			auto convert = ToFlash{*FileSystem::read_json(source)};
			convert.process(destination, resolution);
			return;
		}
	};
}