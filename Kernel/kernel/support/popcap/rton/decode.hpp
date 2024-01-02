#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::RTON
{
    using namespace Definition;

#define fill_rijndael_block(raw, iv)                                            \
    auto padding = raw.size() - ((raw.size() + iv.size() - 1) % iv.size() + 1); \
    raw.writeNull(padding);

    class Decode
    {
    protected:
        inline static constexpr auto magic = std::string_view{"RTON"};

        inline static constexpr auto empty_string = std::string_view{""};

        inline static constexpr auto rtid_0 = std::string_view{"RTID(0)"};

        inline static constexpr auto star = std::string_view{"*"};

        SenBuffer sen;

        nlohmann::ordered_json r0x90_list = nlohmann::json::array_t();

        nlohmann::ordered_json r0x92_list = nlohmann::json::array_t();

        Decode(
            const std::string & source
        ) : sen(source)
        {

        }

        Decode(
            const SenBuffer & it
        ) : sen(it)
        {

        }

        ~Decode(

        ) = default;

        inline auto decode_rton(

        ) -> nlohmann::ordered_json
        {
            {
                if (sen.readString(4) != magic)
                {
                    throw std::runtime_error("Invaild RTON head");
                }
            }
            {
                sen.readBytesLE<uint8_t>(4);
            }
            return thiz.read_object();
        }

        inline auto read_object(

        ) -> nlohmann::ordered_json
        {
            auto object_json = nlohmann::ordered_json::object();
            auto bytecode = thiz.sen.readUint8();
            while (bytecode != 0xFF)
            {
                auto property_name = thiz.read_bytecode_property(bytecode);
                object_json[property_name] = empty_string;
                thiz.read_bytecode(thiz.sen.readUint8(), object_json[property_name]);
                bytecode = sen.readUint8();
            }
            return object_json;
        }

        inline auto read_array(

        ) -> nlohmann::ordered_json
        {
            // debug("read array");
            auto bytecode = thiz.sen.readUint8();
            if (bytecode != 0xFD){
                throw std::runtime_error("Invaild array start");
            }
            auto num_array = thiz.sen.readVarInt32();
            auto array_json = nlohmann::ordered_json::array();
            for (auto i : Range<int32_t>(num_array))
            {
                bytecode = thiz.sen.readUint8();
                array_json += 0;
                read_bytecode(bytecode, array_json.at(i));
            }
            bytecode = thiz.sen.readUint8();
            if (bytecode != 0xFE){
                throw std::runtime_error("Invaild array end");
            }
            return array_json;
        }

        inline auto read_RTID(

        ) -> std::string
        {
            switch (thiz.sen.readUint8())
            {
            case 0x0:
            {
                return std::string{"RTID(0)"};
            }
            case 0x1:
            {
                auto value_0x1_2 = thiz.sen.readVarInt32();
                auto value_0x1_1 = thiz.sen.readVarInt32();
                auto x16_1 = thiz.sen.readUint32LE();
                return fmt::format("RTID({}.{}.{:08x}@)", value_0x1_1, value_0x1_2, x16_1);
            }
            case 0x2:
            {
                thiz.sen.readVarInt32();
                auto str = thiz.sen.readStringByVarInt32();
                auto value_0x2_2 = thiz.sen.readVarInt32();
                auto value_0x2_1 = thiz.sen.readVarInt32();
                auto x16_2 = thiz.sen.readUint32LE();
                return fmt::format("RTID({}.{}.{:08x}@{})", value_0x2_2, value_0x2_1, x16_2, str);
            }
            case 0x3:
            {
                thiz.sen.readVarInt32();
                auto str2 = thiz.sen.readStringByVarInt32();
                thiz.sen.readVarInt32();
                auto str1 = thiz.sen.readStringByVarInt32();
                return fmt::format("RTID({}@{})", str1, str2);
            }
            default:
            {
                throw std::runtime_error("Invaild RTID");
            }
            }
        }

        inline auto read_binary(

        ) -> std::string
        {
            // debug("read binary");
            thiz.sen.appendPosition(1);
            auto str = thiz.sen.readStringByVarInt32();
            auto num = thiz.sen.readVarInt32();
            return fmt::format("BINARY({}, {})", str, num);
        }

        inline auto read_bytecode(
            uint8_t bytecode,
            nlohmann::ordered_json &json
        ) -> void
        {
            // debug("read bytecode");
            switch (bytecode)
            {
            case 0x0:{
                json = false;
                break;
            }
            case 0x1:{
                json = true;
                break;
            }
            case 0x2:{
                json = star;
                break;
            }
            case 0x8:{
                json = thiz.sen.readUint8();
                break;
            }
            case 0x9:
            case 0xB:
            case 0x11:
            case 0x13:
            case 0x21:
            case 0x23:
            case 0x27:
            case 0x41:
            case 0x43:
            case 0x47:{
                json = 0;
                break;
            }
            case 0xA:{
                json = thiz.sen.readInt8();
                break;
            }
            case 0x10:{
                json = thiz.sen.readInt16LE();
                break;
            }
            case 0x12:{
                json = thiz.sen.readUint16LE();
                break;
            }
            case 0x20:{
                json = thiz.sen.readInt32LE();
                break;
            }
            case 0x22:{
                json = thiz.sen.readFloatLE();
                break;
            }
            case 0x24:{
                json = thiz.sen.readVarInt32();
                break;
            }
            case 0x25:{
                json = thiz.sen.readZigZag32();
                break;
            }
            case 0x26:{
                json = thiz.sen.readUint32LE();
                break;
            }
            case 0x28:{
                json = thiz.sen.readVarUInt32();
                break;
            }
            case 0x40:{
                json = thiz.sen.readInt64LE();
                break;
            }
            case 0x42:{
                json = thiz.sen.readDoubleLE();
                break;
            }
            case 0x44:{
                json = thiz.sen.readVarInt64();
                break;
            }
            case 0x45:{
                json = thiz.sen.readZigZag64();
                break;
            }
            case 0x46:{
                json = thiz.sen.readUint64LE();
                break;
            }
            case 0x48:{
                json = thiz.sen.readVarUInt64();
                break;
            }
            case 0x81:{
                json = thiz.sen.readStringByVarInt32();
                break;
            }
            case 0x82:{
                thiz.sen.readVarInt32();
                json = thiz.sen.readStringByVarInt32();
                break;
            }
            case 0x83:{
                json = read_RTID();
                break;
            }
            case 0x84:{
                json = rtid_0;
                break;
            }
            case 0x85:{
                json = read_object();
                break;
            }
            case 0x86:{
                json = read_array();
                break;
            }
            case 0x87:{
                json = read_binary();
                break;
            }
            case 0x90:{
                auto temp_string = thiz.sen.readStringByVarInt32();
                r0x90_list += temp_string;
                json = temp_string;
                break;
            }
            case 0x91:{
                json = thiz.r0x90_list.at(thiz.sen.readVarInt32());
                break;
            }
            case 0x92:{
                thiz.sen.readVarInt32();
                auto temp_string = thiz.sen.readStringByInt32();
                thiz.r0x92_list.emplace_back(temp_string);
                json = temp_string;
                break;
            }
            case 0x93:{
                json = thiz.r0x92_list.at(thiz.sen.readVarInt32());
                break;
            }
            default:{
                throw std::runtime_error("Invaild bytecode");
            }
            }
        }

        inline auto read_bytecode_property(
            uint8_t bytecode
        ) -> std::string
        {
            // debug("read_bytecode_property");
            switch (bytecode)
            {
                case 0x2:{
                    // debug(0x2);
                    return std::string{"*"};
                }
                case 0x81:{
                    // debug(0x81);
                    return thiz.sen.readStringByVarInt32();
                }
                case 0x82:{
                    // debug(0x82);
                    thiz.sen.readVarInt32();
                    return thiz.sen.readStringByVarInt32();
                }
                case 0x83:{
                    // debug(0x83);
                    return read_RTID();
                }
                case 0x84:{
                    // debug(0x84);
                    return std::string{"RTID(0)"};
                }
                case 0x87:{
                    // debug(0x87);
                    return read_binary();
                }
                case 0x90:{
                    // debug(0x90);
                    auto temp_string = thiz.sen.readStringByVarInt32();
                    // debug(temp_string);
                    thiz.r0x90_list.emplace_back(temp_string);
                    return temp_string;
                }
                case 0x91:{
                    // debug(0x91);
                    return thiz.r0x90_list.at(thiz.sen.readVarInt32()).get<std::string>();
                }
                case 0x92:{
                    // debug(0x92);
                    thiz.sen.readVarInt32();
                    auto temp_string = thiz.sen.readStringByInt32();
                    thiz.r0x92_list.emplace_back(temp_string);
                    return temp_string;
                }
                case 0x93:{
                    // debug(0x93);
                    return thiz.r0x92_list.at(thiz.sen.readVarInt32()).get<std::string>();
                }
                default:{
                    throw std::runtime_error("Invaild bytecode property");
                }
            }
        }

    public:
        inline static auto decode_fs(
            const std::string & source,
            const std::string & destination
        ) -> void
        {
            // debug("decode_fs");
            auto c = Decode{source};
            auto result = c.decode_rton();
            FileSystem::writeJson(destination, result);
            return;
        }

        inline static auto decrypt_fs(
            const std::string & source,
            const std::string & destination,
            const std::string & key,
            const std::string & iv
        ) -> void
        {
            auto source_buffer = SenBuffer{source};
            auto source_iv = SenBuffer{iv};
            fill_rijndael_block(source_buffer, source_iv);
            FileSystem::writeBinary<unsigned char>(destination, Sen::Kernel::Definition::Encryption::Rijndael::decrypt(reinterpret_cast<char *>(source_buffer.getBytes(0, source_buffer.size())), key, iv, source_buffer.size(), Sen::Kernel::Definition::Encryption::RijndaelMode::CBC));
            return;
        }

        inline static auto decrypt_and_decode_fs(
            const std::string & source,
            const std::string & destination,
            const std::string & key,
            const std::string & iv
        ) -> void
        {
            auto source_buffer = SenBuffer{source};
            auto source_iv = SenBuffer{iv};
            fill_rijndael_block(source_buffer, source_iv);
            auto sen = SenBuffer{Sen::Kernel::Definition::Encryption::Rijndael::decrypt(reinterpret_cast<char *>(source_buffer.getBytes(0, source_buffer.size())), key, iv, source_buffer.size(), Sen::Kernel::Definition::Encryption::RijndaelMode::CBC)};
            auto rton = Decode{sen};
            FileSystem::writeBinary<unsigned char>(destination, rton.decode_rton());
            return;
        }
    };
}