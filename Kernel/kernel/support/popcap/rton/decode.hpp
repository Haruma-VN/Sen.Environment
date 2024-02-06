#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::RTON
{
    using namespace Definition;

    class Decode
    {
    protected:
        inline static constexpr auto magic = std::string_view{"RTON"};

        inline static constexpr auto empty_string = std::string_view{""};

        inline static constexpr auto rtid_0 = std::string_view{"RTID(0)"};

        inline static constexpr auto star = std::string_view{"*"};

        inline static const auto star_s = std::string{"*"};

        inline static const auto rtid_0_s = std::string{"RTID(0)"};

        inline static constexpr auto magic_count = 4;

        inline static constexpr auto version_count = 4;
        
        inline static constexpr auto position_increment = 1;

        inline static constexpr auto end_bytecode = 0xFF_byte;

        inline static constexpr auto array_byte_start = 0xFD_byte;

        inline static constexpr auto array_byte_end = 0xFE_byte;

        inline static constexpr auto rtid_0_sc = 0x0_byte;

        inline static constexpr auto rtid_1_sc = 0x1_byte;

        inline static constexpr auto rtid_2_sc = 0x2_byte;

        inline static constexpr auto rtid_3_sc = 0x3_byte;

        inline static constexpr auto star_s_bytecode = 0x2_byte;

        inline static constexpr auto varint32_string_bytecode = 0x81_byte;

        inline static constexpr auto varint32_varint32_string_bytecode = 0x82_byte;

        inline static constexpr auto rtid_bytecode = 0x83_byte;

        inline static constexpr auto rtid_0_s_bytecode = 0x84_byte;

        inline static constexpr auto binary_bytecode = 0x87_byte;

        inline static constexpr auto varint32_temp_string_bytecode = 0x90_byte;

        inline static constexpr auto varint32_indexed_string_bytecode = 0x91_byte;

        inline static constexpr auto varint32_int32_temp_string_bytecode = 0x92_byte;

        inline static constexpr auto varint32_indexed_string2_bytecode = 0x93_byte;

        inline static constexpr auto k_none_size = 0;

        DataStreamView sen;

        JsonWriter json_writer = JsonWriter{};

        std::vector<std::string> r0x90_list;

        std::vector<std::string> r0x92_list;

        // -----------------------------------------

        inline auto read_object(

        ) -> void
        {
            json_writer.WriteStartObject();
            auto bytecode = thiz.sen.readUint8();
            while (bytecode != end_bytecode)
            {
                const auto & property_name = thiz.read_bytecode_property(bytecode);
                json_writer.WritePropertyName(property_name);
                thiz.read_bytecode(thiz.sen.readUint8());
                bytecode = thiz.sen.readUint8();
            }
            json_writer.WriteEndObject();
            return;
        }

        // -----------------------------------------

        inline auto read_array(

        ) -> void
        {
            json_writer.WriteStartArray();
            if (thiz.sen.readUint8() != array_byte_start){
                throw Exception(fmt::format("{} {:02x}. {}: {:02x}", Kernel::Language::get("popcap.rton.decode.invalid_rton_array_starts"), array_byte_start, Kernel::Language::get("offset"), thiz.sen.read_pos), std::source_location::current(), "read_array");
            }
            for (const auto & i : Range<int32_t>(thiz.sen.readVarInt32()))
            {
                thiz.read_bytecode(thiz.sen.readUint8());
            }
            if (thiz.sen.readUint8() != array_byte_end){
                throw Exception(fmt::format("{} {:02x}. {}: {:02x}", Kernel::Language::get("popcap.rton.decode.invalid_rton_array_end"), array_byte_end, Kernel::Language::get("offset"), thiz.sen.read_pos), std::source_location::current(), "read_array");
            }
            json_writer.WriteEndArray();
            return;
        }

        // -----------------------------------------

        inline auto read_rtid(

        ) const -> std::string const
        {
            switch (thiz.sen.readUint8())
            {
                case 0x00:
                {
                    return rtid_0_s;
                }
                case 0x01:
                {
                    const auto value_0x1_2 = thiz.sen.readVarInt32();
                    const auto value_0x1_1 = thiz.sen.readVarInt32();
                    const auto x16_1 = thiz.sen.readUint32();
                    return fmt::format("RTID({}.{}.{:08x}@)", value_0x1_1, value_0x1_2, x16_1);
                }
                case 0x02:
                {
                    thiz.sen.readVarInt32();
                    const auto & str = thiz.sen.readStringByVarInt32();
                    const auto value_0x2_2 = thiz.sen.readVarInt32();
                    const auto value_0x2_1 = thiz.sen.readVarInt32();
                    const auto x16_2 = thiz.sen.readUint32();
                    return fmt::format("RTID({}.{}.{:08x}@{})", value_0x2_2, value_0x2_1, x16_2, str);
                }
                case 0x03:
                {
                    thiz.sen.readVarInt32();
                    const auto & str2 = thiz.sen.readStringByVarInt32();
                    thiz.sen.readVarInt32();
                    const auto & str1 = thiz.sen.readStringByVarInt32();
                    return fmt::format("RTID({}@{})", str1, str2);
                }
                default:
                {
                    throw Exception(fmt::format("{}. {}: {:02x}", Kernel::Language::get("popcap.rton.decode.invalid_rtid"), Kernel::Language::get("offset"), thiz.sen.read_pos), std::source_location::current(), "read_rtid");
                }
            }
        }

        // -----------------------------------------

        inline auto read_binary(

        ) const -> std::string const
        {
            // debug("read binary");
            thiz.sen.read_pos += position_increment;
            const auto & str = thiz.sen.readStringByVarInt32();
            const auto num = thiz.sen.readVarInt32();
            return fmt::format("BINARY({}, {})", str, num);
        }

        // -----------------------------------------

        inline auto read_bytecode(
            uint8_t bytecode
        ) -> void
        {
            switch (bytecode)
                {
                case 0x0:{
                    json_writer.WriteBoolean(false);
                    return;
                }
                case 0x1:{
                    json_writer.WriteBoolean(true);
                    return;
                }
                case 0x2:{
                    json_writer.WriteValue(star_s);
                    return;
                }
                case 0x8:{
                    json_writer.WriteValue(thiz.sen.readUint8());
                    return;
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
                    json_writer.WriteValue(k_none_size);
                    return;
                }
                case 0xA:{
                    json_writer.WriteValue(thiz.sen.readInt8());
                    return;
                }
                case 0x10:{
                    json_writer.WriteValue(thiz.sen.readInt16());
                    return;
                }
                case 0x12:{
                    json_writer.WriteValue(thiz.sen.readUint16());
                    return;
                }
                case 0x20:{
                    json_writer.WriteValue(thiz.sen.readInt32());
                    return;
                }
                case 0x22:{
                    json_writer.WriteValue(thiz.sen.readFloat());
                    return;
                }
                case 0x24:{
                    json_writer.WriteValue(thiz.sen.readVarInt32());
                    return;
                }
                case 0x25:{
                    json_writer.WriteValue(thiz.sen.readZigZag32());
                    return;
                }
                case 0x26:{
                    json_writer.WriteValue(thiz.sen.readUint32());
                    return;
                }
                case 0x28:{
                    json_writer.WriteValue(thiz.sen.readVarUint32());
                    return;
                }
                case 0x40:{
                    json_writer.WriteValue(thiz.sen.readInt64());
                    return;
                }
                case 0x42:{
                    json_writer.WriteValue(thiz.sen.readDouble());
                    return;
                }
                case 0x44:{
                    json_writer.WriteValue(thiz.sen.readVarInt64());
                    return;
                }
                case 0x45:{
                    json_writer.WriteValue(thiz.sen.readZigZag64());
                    return;
                }
                case 0x46:{
                    json_writer.WriteValue(thiz.sen.readUint64());
                    return;
                }
                case 0x48:{
                    json_writer.WriteValue(thiz.sen.readVarUint64());
                    return;
                }
                case 0x81:{
                    json_writer.WriteValue(thiz.sen.readStringByVarInt32());
                    return;
                }
                case 0x82:{
                    thiz.sen.readVarInt32();
                    json_writer.WriteValue(thiz.sen.readStringByVarInt32());
                    return;
                }
                case 0x83:{
                    json_writer.WriteValue(read_rtid());
                    return;
                }
                case 0x84:{
                    json_writer.WriteValue(rtid_0_s);
                    return;
                }
                case 0x85:{
                    read_object();
                    return;
                }
                case 0x86:{
                    read_array();
                    return;
                }
                case 0x87:{
                    json_writer.WriteValue(read_binary());
                    return;
                }
                case 0x90:{
                    auto temp_string = thiz.sen.readStringByVarInt32();
                    r0x90_list.emplace_back(temp_string);
                    json_writer.WriteValue(temp_string);
                    return;
                }
                case 0x91:{
                    json_writer.WriteValue(thiz.r0x90_list.at(thiz.sen.readVarInt32()));
                    return;
                }
                case 0x92:{
                    thiz.sen.readVarInt32();
                    auto temp_string = thiz.sen.readStringByVarInt32();
                    thiz.r0x92_list.emplace_back(temp_string);
                    json_writer.WriteValue(temp_string);
                    return;
                }
                case 0x93:{
                    json_writer.WriteValue(thiz.r0x92_list.at(thiz.sen.readVarInt32()));
                    return;
                }
                default:{
                    throw Exception(fmt::format("{}. {}: {:02x}", Kernel::Language::get("popcap.rton.decode.invalid_bytecode"), Kernel::Language::get("offset"), bytecode), std::source_location::current(), "read_bytecode");
                }
            }
        }

        // -----------------------------------------

        inline auto read_bytecode_property(
            uint8_t bytecode
        ) -> std::string const
        {
            switch (bytecode)
            {
                case star_s_bytecode:
                {
                    return star_s;
                }
                case varint32_string_bytecode:
                {
                    return thiz.sen.readStringByVarInt32();
                }
                case varint32_varint32_string_bytecode:
                {
                    thiz.sen.readVarInt32();
                    return thiz.sen.readStringByVarInt32();
                }
                case rtid_bytecode:
                {
                    return read_rtid();
                }
                case rtid_0_s_bytecode:
                {
                    return rtid_0_s;
                }
                case binary_bytecode:
                {
                    return read_binary();
                }
                case varint32_temp_string_bytecode:
                {
                    const auto & temp_string = thiz.sen.readStringByVarInt32();
                    thiz.r0x90_list.emplace_back(temp_string);
                    return temp_string;
                }
                case varint32_indexed_string_bytecode:
                {
                    return thiz.r0x90_list.at(thiz.sen.readVarInt32());
                }
                case varint32_int32_temp_string_bytecode:
                {
                    thiz.sen.readVarInt32();
                    const auto & temp_string = thiz.sen.readStringByVarInt32();
                    thiz.r0x92_list.emplace_back(temp_string);
                    return temp_string;
                }
                case varint32_indexed_string2_bytecode:
                {
                    return thiz.r0x92_list.at(thiz.sen.readVarInt32());
                }
                default:
                {
                    throw Exception(fmt::format("{}. {}: {:02x}", Kernel::Language::get("popcap.rton.decode.invalid_bytecode_property"), Kernel::Language::get("offset"), thiz.sen.read_pos), std::source_location::current(), "read_bytecode_property");
                }
            }
    }

    public:

        // -----------------------------------------

        explicit Decode(
            std::string_view source
        ) : sen(source)
        {

        }

        // -----------------------------------------

        explicit Decode(
            DataStreamView & it
        ) : sen(it)
        {

        }

        // -----------------------------------------

        ~Decode(

        ) = default;

        // -----------------------------------------

        inline auto decode_rton(

        ) -> std::string const
        {
            {
                const auto & magic = sen.readString(magic_count);
                if (magic != thiz.magic)
                {
                    throw Exception(fmt::format("{}", Kernel::Language::get("popcap.rton.decode.invalid_rton_magic")), std::source_location::current(), "decode_rton");
                }
            }
            {
                sen.readUint32();
            }
            json_writer.Clear();
            json_writer.WriteIndent = true;
            thiz.read_object();
            return json_writer.ToString();
        }

        // -----------------------------------------

        inline static auto decode_fs(
            std::string_view source,
            std::string_view destination
        ) -> void
        {
            auto c = std::unique_ptr<Decode>(new Decode{source});
            FileSystem::write_file(destination, c->decode_rton());
            return;
        }

        // -----------------------------------------

        inline static auto decrypt_fs(
            std::string_view source,
            std::string_view destination,
            std::string_view key,
            std::string_view iv
        ) -> void
        {
            auto source_buffer = DataStreamView{source};
            auto source_iv = DataStreamView{};
            source_iv.writeStringView(iv);
            fill_rijndael_block(source_buffer, source_iv);
            FileSystem::write_binary<unsigned char>(destination, 
                Encryption::Rijndael::decrypt(reinterpret_cast<char *>(source_buffer.getBytes(2, source_buffer.size()).data()), 
                key, iv, source_buffer.size() - 2, Sen::Kernel::Definition::Encryption::RijndaelMode::CBC)
            );
            return;
        }

        // -----------------------------------------

        inline static auto decrypt_and_decode_fs(
            std::string_view source,
            std::string_view destination,
            std::string_view key,
            std::string_view iv
        ) -> void
        {
            auto source_buffer = DataStreamView{source};
            auto source_iv = DataStreamView{};
            source_iv.writeStringView(iv);
            fill_rijndael_block(source_buffer, source_iv);
            auto sen = DataStreamView{Encryption::Rijndael::decrypt(reinterpret_cast<char *>(source_buffer.getBytes(2, source_buffer.size()).data()), key, iv, source_buffer.size() - 2, Sen::Kernel::Definition::Encryption::RijndaelMode::CBC)};
            auto rton = std::unique_ptr<Decode>(new Decode{ sen });
            FileSystem::write_file(destination, rton->decode_rton());
            return;
        }

        // ---------------------------------------------

        inline static auto decode_fs_as_multiple_threads(
            const std::vector<std::vector<std::string>> & paths
        ) -> void 
        {
            auto threads = std::vector<std::thread>{};
            auto file_mutexes = std::map<std::string, std::mutex>{};
            for (const auto & data : paths) {
                threads.emplace_back([=, &file_mutexes]() { 
                auto lock_source = std::lock_guard<std::mutex>(file_mutexes[data[0]]);
                auto lock_destination = std::lock_guard<std::mutex>(file_mutexes[data[1]]);
                    Decode::decode_fs(data[0], data[1]); 
                });
            }
            for (auto & thread : threads) {
                thread.join();
            }
            return;
        }
    };
}