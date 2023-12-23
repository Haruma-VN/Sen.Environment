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
        auto decode_rton(
            SenBuffer &sen
        ) -> nlohmann::json
        {
            debug("decode rton");
            auto magic = sen.readString(4) == "RTON";
            if (!magic){
                throw std::runtime_error("Invaild RTON head");
            }
            sen.readBytesLE<uint8_t>(4);
            auto r0x90_list = nlohmann::json::array();
            auto r0x92_list = nlohmann::json::array();
            auto result = read_object(sen, r0x90_list, r0x92_list);
            return result;
        }

        inline auto read_object(
            SenBuffer &sen,
            nlohmann::json &r0x90_list,
            nlohmann::json &r0x92_list
        ) -> nlohmann::json
        {
            debug("read object");
            auto object_json = nlohmann::json::object();
            auto bytecode = sen.readUint8();
            while (bytecode != 0xFF)
            {
                auto property_name = read_bytecode_property(bytecode, sen, r0x90_list, r0x92_list);
                object_json += {property_name, ""};
                read_bytecode(bytecode, sen, r0x90_list, r0x92_list, object_json[property_name]);
            }
            return object_json;
        }

        inline auto read_array(
            SenBuffer &sen,
            nlohmann::json &r0x90_list,
            nlohmann::json &r0x92_list
        ) -> nlohmann::json
        {
            debug("read array");
            auto bytecode = sen.readUint8();
            if (bytecode != 0xFD)
                throw std::runtime_error("Invaild array start");
            auto num_array = sen.readVarInt32();
            auto array_json = nlohmann::json::array();
            for (auto i = 0; i < num_array; i++)
            {
                bytecode = sen.readUint8();
                array_json += 0;
                read_bytecode(bytecode, sen, r0x90_list, r0x92_list, array_json.at(i));
            }
            bytecode = sen.readUint8();
            if (bytecode != 0xFE)
                throw std::runtime_error("Invaild array end");
            return array_json;
        }

        inline auto read_RTID(
            SenBuffer &sen
        ) -> std::string
        {
            debug("read rtid");
            auto byte = sen.readUint8();
            switch (byte)
            {
            case 0x0:{
                return std::string{"RTID(0)"};
            }
            case 0x1:{
                auto value_0x1_2 = sen.readVarInt32();
                auto value_0x1_1 = sen.readVarInt32();
                auto x16_1 = sen.readUint32LE();
                return fmt::format("RTID({}.{}.{:08x}@)", value_0x1_1, value_0x1_2, x16_1);
            }
            case 0x2: {
                sen.readVarInt32();
                auto str = sen.readStringByVarInt32();
                auto value_0x2_2 = sen.readVarInt32();
                auto value_0x2_1 = sen.readVarInt32();
                auto x16_2 = sen.readUint32LE();
                return fmt::format("RTID({}.{}.{:08x}@{})", value_0x2_2, value_0x2_1, x16_2, str);
            }
            case 0x3: {
                sen.readVarInt32();
                auto str2 = sen.readStringByVarInt32();
                sen.readVarInt32();
                auto str1 = sen.readStringByVarInt32();
                return fmt::format("RTID({}@{})", str1, str2);
            }
            default:{
                throw std::runtime_error("Invaild RTID");
            }
            }
        }

        inline auto read_binary(
            SenBuffer &sen
        ) -> std::string
        {
            debug("read binary");
            sen.appendPosition(1);
            auto str = sen.readStringByVarInt32();
            auto num = sen.readVarInt32();
            return fmt::format("BINARY({}, {})", str, num);
        }

        inline auto read_bytecode(
            const uint8_t &bytecode,
            SenBuffer &sen,
            nlohmann::json &r0x90_list,
            nlohmann::json &r0x92_list,
            nlohmann::json json
        ) -> void
        {
            debug("read bytecode");
            std::string temp_string;
            switch (bytecode)
            {
            case 0x0:
                json = false;
                break;
            case 0x1:
                json = true;
                break;
            case 0x2:
                json = "*";
                break;
            case 0x8:
                json = sen.readUint8();
                break;
            case 0x9:
            case 0xB:
            case 0x11:
            case 0x13:
            case 0x21:
            case 0x23:
            case 0x27:
            case 0x41:
            case 0x43:
            case 0x47:
                json = 0;
                break;
            case 0xA:
                json = sen.readInt8();
                break;
            case 0x10:
                json = sen.readInt16LE();
                break;
            case 0x12:
                json = sen.readUint16LE();
                break;
            case 0x20:
                json = sen.readInt32LE();
                break;
            case 0x22:
                json = sen.readFloatLE();
                break;
            case 0x24:
                json = sen.readVarInt32();
                break;
            case 0x25:
                json = sen.readZigZag32();
                break;
            case 0x26:
                json = sen.readUint32LE();
                break;
            case 0x28:
                json = sen.readVarUInt32();
                break;
            case 0x40:
                json = sen.readInt64LE();
                break;
            case 0x42:
                json = sen.readDoubleLE();
                break;
            case 0x44:
                json = sen.readVarInt64();
                break;
            case 0x45:
                json = sen.readZigZag64();
                break;
            case 0x46:
                json = sen.readUint64LE();
                break;
            case 0x48:
                json = sen.readVarUInt64();
                break;
            case 0x81:
                json = sen.readStringByVarInt32();
                break;
            case 0x82:
                sen.readVarInt32();
                json = sen.readStringByVarInt32();
                break;
            case 0x83:
                json = read_RTID(sen);
                break;
            case 0x84:
                json = "RTID(0)";
                break;
            case 0x85:
                json = read_object(sen, r0x90_list, r0x92_list);
                break;
            case 0x86:
                json = read_array(sen, r0x90_list, r0x92_list);
                break;
            case 0x87:
                json = read_binary(sen);
                break;
            case 0x90:
                temp_string = sen.readStringByVarInt32();
                r0x90_list += temp_string;
                json = temp_string;
                break;
            case 0x91:
                json = r0x90_list.at(sen.readVarInt32());
                break;
            case 0x92:
                sen.readVarInt32();
                temp_string = sen.readStringByInt32();
                r0x92_list += temp_string;
                json = temp_string;
                break;
            case 0x93:
                json = r0x92_list.at(sen.readVarInt32());
                break;
            default:
                throw std::runtime_error("Invaild bytecode");
            }
        }

        inline auto read_bytecode_property(
            const uint8_t &bytecode,
            SenBuffer &sen,
            nlohmann::json &r0x90_list,
            nlohmann::json &r0x92_list) -> std::string
        {
            debug("read_bytecode_property");
            std::string temp_string;
            switch (bytecode)
            {
            case 0x2:
                return "*";
            case 0x81:
                return sen.readStringByVarInt32();
            case 0x82:
                sen.readVarInt32();
                return sen.readStringByVarInt32();
            case 0x83:
                return read_RTID(sen);
            case 0x84:
                return "RTID(0)";
            case 0x87:
                return read_binary(sen);
            case 0x90:
                temp_string = sen.readStringByVarInt32();
                r0x90_list += temp_string;
                return temp_string;
            case 0x91:
                return r0x90_list.at(sen.readVarInt32());
            case 0x92:
                sen.readVarInt32();
                temp_string = sen.readStringByInt32();
                r0x92_list += temp_string;
                return temp_string;
            case 0x93:
                return r0x92_list.at(sen.readVarInt32());
            default:
                throw std::runtime_error("Invaild bytecode property");
                break;
            }
        }

        public:

            static auto decode_fs(
                const std::string & source,
                const std::string & destination
            ) -> void
            {
                auto c = Decode{};
                auto sen = SenBuffer{source};
                auto result = c.decode_rton(sen);
                FileSystem::writeJson(destination, result);
                return;
            }

            static auto decrypt_fs(
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
            
            static auto decrypt_and_decode_fs(
                const std::string & source,
                const std::string & destination,
                const std::string & key,
                const std::string & iv
            ) -> void
            {
                auto source_buffer = SenBuffer{source};
                auto source_iv = SenBuffer{iv};
                fill_rijndael_block(source_buffer, source_iv);
                auto rton = Decode{};
                auto sen = SenBuffer{Sen::Kernel::Definition::Encryption::Rijndael::decrypt(reinterpret_cast<char *>(source_buffer.getBytes(0, source_buffer.size())), key, iv, source_buffer.size(), Sen::Kernel::Definition::Encryption::RijndaelMode::CBC)};
                FileSystem::writeBinary<unsigned char>(destination, rton.decode_rton(sen));
                return;
            }
    };
}