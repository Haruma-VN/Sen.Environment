#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/pvz2/definition.hpp"
#include "kernel/support/popcap/pvz2/common.hpp"
#include "kernel/support/popcap/animation/decode.hpp"
#include "kernel/support/popcap/animation/convert/to_flash.hpp"
#include "kernel/support/popcap/rton/decode.hpp"
#include "kernel/support/wwise/sound_bank/decode.hpp"
#include "kernel/support/texture/decode.hpp"

namespace Sen::Kernel::Support::PopCap::PvZ2
{
    using namespace Definition;

    class Decode
    {
    private:
    protected:
        std::unique_ptr<DataStreamView> stream;

    protected:
        // rton
        inline auto decode_rton(
            const std::string &resources_folder,
            const ResInfo<uint32_t> &res) const -> void
        {
            auto rton_decode = RTON::Decode{res.data};
            const auto json_path = res.path.substr(0, res.path.size() - 5) + ".json";
            Common::write_text_file(fmt::format("{}/{}", resources_folder, json_path), rton_decode.decode_rton());
            return;
        }

        // soundbank
        inline auto decode_soundbank(
            const std::string &resources_folder,
            const ResInfo<uint32_t> &res) const -> void
        {
            auto info = WWise::SoundBank::SoundBankInformation{};
            auto soundbank_decode = WWise::SoundBank::Decode{res.data};
            const auto destination = fmt::format("{}/{}", resources_folder, res.path.substr(0, res.path.size() - 4));
            soundbank_decode.process(destination, &info);
            Common::write_json(fmt::format("{}/definition.json", destination, res.path), info);
            return;
        }

        inline auto decode_file(
            const std::string &resources_folder,
            Info &data_info) const -> void
        {
            const auto rsg_data_size = stream->readUint32();
            const auto rsg_pos = stream->readUint32();
            auto rsg_data = stream->getBytes(rsg_pos, rsg_pos + rsg_data_size);
            const auto res_size = stream->readUint32();
            auto res_info = nlohmann::ordered_json::parse(stream->readString(res_size, static_cast<std::size_t>(stream->readUint32())));
            const auto subgroup_name = stream->readStringByEmpty();
            auto packet_info = PacketInfo<uint32_t>{};
            Common::rsg_unpack(rsg_data, &packet_info);
            auto &p_info = data_info.groups[subgroup_name];
            p_info.compression_flags = CompressionFlag(packet_info.flags);
            p_info.type = get_type(res_info["packet"]["type"].get<std::string>());
            for (const auto &[id, packet] : res_info["packet"]["data"].items())
            {
                const auto path = packet["path"].get<std::string>();
                const auto packet_type = get_type(packet["type"].get<std::string>());
                auto data_res_info = DataResInfo{packet_type, path};
                if (path == "!program")
                {
                    continue;
                }
                const auto ext_name = Path::getExtension(path);
                const auto i = Common::find_path_in_res(packet_info.res, path);
                if (packet_type == File && Common::compare_string(ext_name, ".rton"))
                {
                    data_res_info.type = Data;
                    data_res_info.path = fmt::format("{}/{}.json", Path::getParents(path), Path::getFileNameWithoutExtension(path));
                    decode_rton(resources_folder, packet_info.res[i]);
                }
                else if (packet_type == SoundBank || packet_type == DecodedSoundBank)
                {
                    if (Common::compare_string(ext_name, ".bnk"))
                    {
                        data_res_info.path = fmt::format("{}/{}", Path::getParents(path), Path::getFileNameWithoutExtension(path));
                        decode_soundbank(resources_folder, packet_info.res[i]);
                    }
                    else
                    {
                        throw Exception("not_a_soundbank", std::source_location::current(), "decode_file"); // TODO add localization;
                    }
                }
                else if (packet_type == File || packet_type == PrimeFont || packet_type == RenderEffect || packet_type == PopAnim || packet_type == Image)
                {
                    Common::write_bytes(fmt::format("{}/{}", resources_folder, path), packet_info.res[i].data);
                }
                else
                {
                    throw Exception("invaild_packet_type", std::source_location::current(), "decode_file"); // TODO add localization;
                }
                p_info.data[id] = data_res_info;
                packet_info.res.erase(packet_info.res.begin() + i);
            }
            data_info.groups[subgroup_name] = p_info;
            return;
        }

        inline auto decode_popanim(
            const std::string &popanim_destination,
            ResInfo<uint32_t> &res) const -> void
        {
            auto animation_decode = Animation::Decode<uint32_t>{res.data};
            animation_decode.process();
            FileSystem::create_directory(popanim_destination);
            auto record = Animation::Convert::RecordInfo{};
            auto popanim_decode = Animation::Convert::ToFlash{animation_decode.json};
            popanim_decode.process(popanim_destination, record, 1536); // always 1536.
            Common::write_json(fmt::format("{}/record.json", popanim_destination), record);
            return;
        }

        inline auto decode_image(
            const ResInfo<uint32_t> &res,
            uint32_t &format,
            uint32_t width,
            uint32_t height) const -> Definition::Image<int>
        {
            switch (format)
            {
            case 0x00000000:
            { // RGBA_8888

                return Texture::Decode::rgba_8888(res.data, width, height);
            }
            case 0xFFFFFFFF:
            { // ARGB_8888
                return Texture::Decode::argb_8888(res.data, width, height);
            }
            case 0x0000001E:
            { // RGBA_PVRTC_4BPP
                return Texture::Decode::rgba_pvrtc_4bpp(res.data, width, height);
            }
            case 0x00000093:
            { // RGB_ETC1_A_8 || RGB_ETC1_A_PALETTE
                const auto data_size = res.data.size();
                if (data_size == width * height / 4)
                {
                    format -= 1;
                    return Texture::Decode::rgb_etc1_a_palette(res.data, width, height);
                }
                else
                {
                    return Texture::Decode::rgb_etc1_a_8(res.data, width, height);
                }
            }
            case 0x00000094:
            { // RGBA_PVRTC_4BPP_A8
                return Texture::Decode::rgb_pvrtc_4bpp_a_8(res.data, width, height);
            }
            default:
            {
                throw Exception("invaild_image_format", std::source_location::current(), "decode_image");
            }
            }
        }

        inline auto split_image(
            const std::string &resources_folder,
            DataInfo &p_info,
            nlohmann::ordered_json &data,
            const Definition::Image<int> &image) const -> void
        {
            auto rectangle = std::vector<Definition::RectangleFileIO<int>>{};

            for (const auto &[id, info] : data.items())
            {
                const auto packet_type = get_type(info["type"].get<std::string>());
                assert_conditional(packet_type == Image, "invaild_sprite_image_type", "split_image"); // TODO add localization.
                auto path_list = String{info["path"].get<std::string>()}.split("/");
                path_list.erase(path_list.begin() + 1);
                // path_list[1] = "1536";
                auto file_name = fmt::format("library/media/{}.png", path_list.back());
                path_list.erase(path_list.end() - 1);
                path_list.emplace_back(file_name);
                auto path = String::join(path_list, "/");
                auto data_res_info = DataResInfo{packet_type, path};
                const auto sprite_path = fmt::format("{}/{}", resources_folder, path);
                rectangle.emplace_back(
                    Definition::RectangleFileIO<int>(
                        info["default"]["ax"].get<int>(),
                        info["default"]["ay"].get<int>(),
                        info["default"]["aw"].get<int>(),
                        info["default"]["ah"].get<int>(),
                        sprite_path));
                data_res_info.ptx_default_info = DataPTXinfo{info["default"]["x"].get<int>(), info["default"]["y"].get<int>()};
                p_info.data[id] = data_res_info;
            }
            std::for_each(rectangle.begin(), rectangle.end(), [&](auto c)
                          { Common::write_image(c.destination, Definition::Image<int>::cut(image, c)); });
            return;
        }

        inline auto decode_composite_group(
            const std::string &resources_folder,
            Info &data_info) const -> void
        {
            data_info.is_composite = true;
            const auto type = stream->readUint32();
            const auto rsg_data_size = stream->readUint32();
            const auto rsg_pos = stream->readUint32();
            auto format = type == 1 ? stream->readUint32() : -1;
            auto rsg_data = stream->getBytes(rsg_pos, rsg_pos + rsg_data_size);
            const auto res_size = stream->readUint32();
            auto res_info = nlohmann::ordered_json::parse(stream->readString(res_size, static_cast<std::size_t>(stream->readUint32())));
            const auto subgroup_name = stream->readStringByEmpty();
            auto packet_info = PacketInfo<uint32_t>{};
            Common::rsg_unpack(rsg_data, &packet_info);
            auto &p_info = data_info.groups[subgroup_name];
            p_info.compression_flags = CompressionFlag(packet_info.flags);
            if (type == 1)
            {
                p_info.type = Image;
                for (const auto &[id, packet] : res_info["packet"].items())
                {
                    const auto path = packet["path"].get<std::string>();
                    const auto packet_type = get_type(packet["type"].get<std::string>());
                    assert_conditional(path != "!program", "invaild_image_path", "split_image"); // TODO add localization.
                    const auto i = Common::find_path_in_res(packet_info.res, path + ".ptx");
                    assert_conditional(packet_type == Image, "not_a_image", "split_image"); // TODO add localization.
                    auto image = decode_image(packet_info.res[i], format, packet["dimension"]["width"].get<uint32_t>(), packet["dimension"]["height"].get<uint32_t>());
                    split_image(resources_folder, p_info, packet["data"], image);
                    packet_info.res.erase(packet_info.res.begin() + i);
                }
                p_info.format = format;
            }
            else
            {
                p_info.type = get_type(res_info["packet"]["type"].get<std::string>());
                for (const auto &[id, packet] : res_info["packet"]["data"].items())
                {
                    const auto path = packet["path"].get<std::string>();
                    const auto packet_type = get_type(packet["type"].get<std::string>());
                    auto data_res_info = DataResInfo{packet_type, path};
                    if (path == "!program")
                    {
                        p_info.data[id] = data_res_info;
                        continue;
                    }
                    const auto i = Common::find_path_in_res(packet_info.res, path);
                    if (packet_type == PopAnim)
                    {
                        auto path_list = String{path}.split("/");
                        path_list.erase(path_list.begin() + 1);
                        // path_list[1] = "1536";
                        path_list.pop_back(); 
                        const auto new_path = String::join(path_list, "/");
                        data_res_info.path = fmt::format("{}/{}", Path::getParents(new_path), Path::getFileNameWithoutExtension(new_path));
                        decode_popanim(fmt::format("{}/{}", resources_folder, new_path), packet_info.res[i]);
                    }
                    else
                    {
                        Common::write_bytes(fmt::format("{}/{}", resources_folder, path), packet_info.res[i].data);
                    }
                    packet_info.res.erase(packet_info.res.begin() + i);
                    p_info.data[id] = data_res_info;
                }
            }
            return;
        }

        inline auto process(
            std::string_view destination,
            Info &data_info) const -> void
        {
            assert_conditional(stream->readString(4) == data_magic, "invaild_data_magic", "process"); // TODO add localizaiton;
            assert_conditional(stream->readUint32() == 1, "invaild_data_version", "process");         // TODO add localizaiton;
            stream->read_pos += 8;
            const auto resources_folder = fmt::format("{}/resources", destination);
            const auto num_packet = stream->readUint32();
            for (const auto &i : Range(num_packet))
            {
                const auto type = stream->readUint32();
                if (type == 0)
                {
                    decode_file(resources_folder, data_info);
                }
                else if (type == 1)
                {
                    decode_composite_group(resources_folder, data_info);
                }
                else
                {
                    throw Exception("invaild_data_type", std::source_location::current(), "process"); // TODO add localizaiton;
                }
            }
            return;
        }

    public:
        explicit Decode(
            std::string_view source) : stream(std::make_unique<DataStreamView>(source))
        {
        }

        explicit Decode(
            const std::vector<uint8_t> &data) : stream(std::make_unique<DataStreamView>(data))
        {
        }

        ~Decode(

            ) = default;

        inline auto process_whole(
            std::string_view destination) const -> void
        {
            auto data_info = Info{};
            process(destination, data_info);
            Common::write_json(fmt::format("{}/info.json", destination), data_info);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto decode = Decode{source};
            decode.process_whole(destination);
            return;
        }
    };
}