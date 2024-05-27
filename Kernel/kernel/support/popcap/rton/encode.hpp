#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::RTON
{
    using namespace Definition;

    using namespace simdjson;

    class Encode
    {
    private:
        using Rijndael = Sen::Kernel::Definition::Encryption::Rijndael;

    private:
        inline static constexpr auto RTON_head = std::string_view{"RTON"};

        inline static constexpr auto RTON_end = std::string_view{"DONE"};

        inline static constexpr auto star = std::string_view{"*"};

        inline static constexpr auto empty_string = std::string_view{""};

        inline static constexpr auto rtid_0 = std::string_view{"RTID(0)"};

        inline static constexpr auto rtid_begin = std::string_view{"RTID("};

        inline static constexpr auto rtid_close = std::string_view{")"};

        inline static constexpr auto rtid_seperator = std::string_view{"@"};

        inline static constexpr auto rtid_dot = std::string_view{"."};

        inline static constexpr auto binary_begin = std::string_view{"$BINARY(\""};

        inline static constexpr auto binary_end = std::string_view{")"};

        inline static constexpr auto binary_seperator = std::string_view{"\", "};

        inline static constexpr auto RTON_vesion = 0x01_byte;

        inline static constexpr auto object_begin = 0x85_byte;

        inline static constexpr auto object_end = 0xFF_byte;

        inline static constexpr auto array_begin = 0x86_byte;

        inline static constexpr auto array_start = 0xFD_byte;

        inline static constexpr auto array_end = 0xFE_byte;

        inline static constexpr auto null_byte = 0x84_byte;

        std::map<std::string_view, int> mutable r0x90_stringpool;

        std::map<std::string_view, int> mutable r0x92_stringpool;

        int mutable r0x90_index = 0;

        int mutable r0x92_index = 0;

    protected:
        inline auto write_rtid(
            std::string_view str) const -> bool
        {
            auto p_str = String{str};
            if (str.starts_with(rtid_begin) and str.ends_with(rtid_close))
            {
                if (str == rtid_0)
                {
                    sen->writeUint8(null_byte);
                    return true;
                }
                auto new_str = p_str.slice(5, p_str.length() - 1);
                if (new_str.indexOf(rtid_seperator) != -1)
                {
                    auto name_str = new_str.split(rtid_seperator);
                    auto dot_count = static_cast<std::size_t>(std::count(name_str[0].begin(), name_str[0].end(), '.'));
                    sen->writeUint8(0x83);
                    if (dot_count == 2)
                    {
                        auto int_str = String{name_str[0]}.split(rtid_dot);
                        sen->writeUint8(0x02);
                        sen->writeVarInt32(name_str[1].length());
                        sen->writeStringByVarInt32(name_str[1]);
                        sen->writeVarInt32(std::stoi(int_str[1]));
                        sen->writeVarInt32(std::stoi(int_str[0]));
                        auto &hex_string = int_str[2];
                        std::reverse(hex_string.begin(), hex_string.end());
                        auto hex_int = static_cast<int>(std::strtol(hex_string.data(), NULL, 16));
                        sen->writeInt32(hex_int);
                    }
                    else
                    {
                        sen->writeUint8(0x03);
                        sen->writeVarInt32(name_str[1].length());
                        sen->writeStringByVarInt32(name_str[1]);
                        sen->writeVarInt32(name_str[0].length());
                        sen->writeStringByVarInt32(name_str[0]);
                    }
                    return true;
                }
            }
            return false;
        }

        inline auto write_binary(
            std::string_view str_v) const -> bool
        {
            auto p_str = String{str_v};
            if (str_v.starts_with(binary_begin) && str_v.ends_with(binary_end))
            {
                auto index = p_str.lastIndexOf(binary_seperator);
                if (index == -1)
                {
                    return false;
                }
                auto v = 0;
                try
                {
                    v = std::stoi(p_str.value.substr(static_cast<size_t>(index) + 3, p_str.length() - 1));
                }
                catch (int err)
                {
                    return false;
                }
                sen->writeUint8(0x87);
                sen->writeUint8(0x00);
                sen->writeStringByVarInt32(str_v.substr(9, index));
                sen->writeVarInt32(v);
                return true;
            }
            return false;
        }

        inline auto constexpr get_utf8_size(
            std::string_view q) const -> int
        {
            auto utf8_size = static_cast<int>(0);
            for (auto &i : q)
            {
                if (i <= 0b01111111)
                {
                    ++utf8_size;
                }
                else if (i >= 0b11000010 && i <= 0b11011111)
                {
                    ++utf8_size;
                }
                else if (i >= 0b11100001 && i <= 0b11101111)
                {
                    ++utf8_size;
                }
                else if (i >= 0b11110000 && i <= 0b11110111)
                {
                    ++utf8_size;
                }
            }
            return utf8_size;
        }

        inline auto constexpr is_ascii(
            std::string_view str) const -> bool
        {
            if (get_utf8_size(str) != str.size()) {
                return false;
            }
            /*
            for (auto &c : str)
            {
                if (static_cast<uint8_t>(c) > 127_byte)
                {
                    return false;
                }
            }
            */
            return true;
        }

        inline auto write_string(
            std::string_view str) const -> void
        {
            if (str == star)
            {
                sen->writeUint8(0x02);
            }
            else if (write_binary(str))
            {
                return;
            }
            else if (write_rtid(str))
            {
                return;
            }
            else if (is_ascii(str))
            {
                if (r0x90_stringpool.contains(str))
                {
                    sen->writeUint8(0x91);
                    sen->writeVarInt32(r0x90_stringpool.at(str));
                }
                else
                {
                    sen->writeUint8(0x90);
                    sen->writeStringByVarInt32(str);
                    r0x90_stringpool[str] = r0x92_index++;
                }
                return;
            }
            else
            {
                if (r0x92_stringpool.contains(str))
                {
                    sen->writeUint8(0x93);
                    sen->writeVarInt32(r0x92_stringpool.at(str));
                }
                else
                {
                    sen->writeUint8(0x92);
                    sen->writeVarInt32(get_utf8_size(str));
                    sen->writeStringByVarInt32(str);
                    r0x92_stringpool[str] = r0x92_index++;
                }
                return;
            }
        }

        inline auto write_array(
            ondemand::array array) const -> void
        {
            sen->writeUint8(array_start);
            sen->writeVarInt32(static_cast<int>(array.count_elements()));
            for (auto child : array)
            {
                write_value(child.value());
            }
            sen->writeUint8(array_end);
            return;
        }

        inline auto write_number(
            ondemand::value element) const -> void
        {

            if (element.is_negative())
            {
                auto negative_value = static_cast<int64_t>(element.get_int64());
                if (negative_value == 0)
                {
                    sen->writeUint8(0x21);
                }
                else if (negative_value + 0x40000000 >= 0)
                {
                    sen->writeUint8(0x25);
                    sen->writeZigZag32(negative_value);
                }
                else if (-2147483648 <= negative_value)
                {
                    sen->writeUint8(0x20);
                    sen->writeInt32(negative_value);
                }
                else if (-9223372036854775808 <= negative_value)
                {
                    sen->writeUint8(0x40);
                    sen->writeInt64(negative_value);
                }
                else
                {
                    sen->writeUint8(0x45);
                    sen->writeZigZag64(negative_value);
                }
            }
            else if (element.is_integer())
            {
                auto uint64_value = static_cast<uint64_t>(element.get_uint64());
                if (uint64_value == 0)
                {
                    sen->writeUint8(0x21);
                }
                else if (uint64_value <= 2147483647)
                {
                    sen->writeUint8(0x24);
                    sen->writeVarInt32(uint64_value);
                }
                else if (uint64_value <= 4294967295)
                {
                    sen->writeUint8(0x26);
                    sen->writeUint32(uint64_value);
                }
                else if (uint64_value > 9223372036854775807)
                {
                    sen->writeUint8(0x46);
                    sen->writeUint64(uint64_value);
                }
                else
                {
                    sen->writeUint8(0x44);
                    sen->writeVarInt64(static_cast<int64_t>(uint64_value));
                }
            }
            else
            {
                auto double_value = static_cast<double>(element.get_double());
                if (double_value == 0.0)
                {
                    sen->writeUint8(0x23);
                }
                else if (static_cast<float>(double_value) == double_value) {
                    sen->writeUint8(0x22);
                    sen->writeFloat(double_value);
                }
                else
                {
                    sen->writeUint8(0x42);
                    sen->writeDouble(double_value);
                }
            }
            return;
        }

        inline auto write_value(
            ondemand::value element) const -> void
        {
            switch (element.type())
            {
            case ondemand::json_type::object:
            {
                sen->writeUint8(object_begin);
                write_object(static_cast<ondemand::object>(element.get_object()));
                break;
            }
            case ondemand::json_type::array:
            {
                sen->writeUint8(array_begin);
                write_array(static_cast<ondemand::array>(element.get_array()));
                break;
            }
            case ondemand::json_type::boolean:
            {
                sen->writeBoolean(static_cast<bool>(element.get_bool()));
                break;
            }
            case ondemand::json_type::null:
            {
                sen->writeUint8(null_byte);
                break;
            }
            case ondemand::json_type::string:
            {
                write_string(static_cast<std::string_view>(element.get_string()));
                break;
            }
            case ondemand::json_type::number:
            {
                write_number(element);
                break;
            }
            default:
            {
                throw Exception("invalid value", std::source_location::current(), "write_value");
            }
            }
            return;
        }

        inline auto write_object(
            ondemand::object object) const -> void
        {
            for (auto field : object)
            {
                write_string(static_cast<std::string_view>(field.unescaped_key()));
                write_value(field.value());
            }
            sen->writeUint8(object_end);
        }

        inline auto reset() const -> void
        {
            r0x90_stringpool.clear();
            r0x92_stringpool.clear();
            r0x90_index = 0;
            r0x92_index = 0;
            return;
        }

    public:
        std::unique_ptr<DataStreamView> sen;

        // ---------------------------------------------

        explicit Encode(

            ) : sen(std::make_unique<DataStreamView>())
        {
        }

        // ---------------------------------------------

        ~Encode(

            ) = default;

        // ---------------------------------------------

        inline auto encode_rton(
            ondemand::document &json) -> void
        {
            reset();
            sen->writeString(RTON_head);
            sen->writeUint32(RTON_vesion);
            auto object = static_cast<ondemand::object>(json.get_object());
            write_object(object);
            sen->writeString(RTON_end);
            return;
        }

        inline static auto encode_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto parser = ondemand::parser{};
            auto str = padded_string::load(source);
            auto json = static_cast<ondemand::document>(parser.iterate(str));
            auto encode = Encode{};
            encode.encode_rton(json);
            encode.sen->out_file(destination);
            return;
        }

        // -----------------------------------------

        inline static auto encrypt(
            DataStreamView &source_buffer,
            DataStreamView *destination,
            std::string_view key,
            std::string_view iv) -> void
        {
            fill_rijndael_block(source_buffer, iv);
            destination->writeUint8(0x10);
            destination->writeUint8(0x00);
            destination->writeBytes(
                Rijndael::encrypt<std::size_t, Rijndael::Mode::CBC>(
                    reinterpret_cast<char *>(source_buffer.getBytes(0, source_buffer.size()).data()),
                    key,
                    iv,
                    source_buffer.size()));
            return;
        }

        // ---------------------------------------------

        inline static auto encrypt_fs(
            std::string_view source,
            std::string_view destination,
            std::string_view key,
            std::string_view iv) -> void
        {
            auto source_buffer = DataStreamView{source};
            auto encrypted_data = DataStreamView{};
            encrypt(source_buffer, &encrypted_data, key, iv);
            encrypted_data.out_file(destination);
            return;
        }

        // ---------------------------------------------

        inline static auto encode_and_encrypt_fs(
            std::string_view source,
            std::string_view destination,
            std::string_view key,
            std::string_view iv) -> void
        {
            auto source_buffer = DataStreamView{source};
            auto parser = ondemand::parser{};
            auto str = padded_string::load(source);
            auto json = static_cast<ondemand::document>(parser.iterate(str));
            auto encoder = Encode{};
            encoder.encode_rton(json);
            auto view = DataStreamView{};
            encrypt(*encoder.sen, &view, key, iv);
            view.out_file(destination);
            return;
        }

        // ---------------------------------------------

        inline static auto encode_fs_as_multiple_threads(
            const std::vector<std::vector<std::string>> &paths) -> void
        {
            auto threads = std::vector<std::thread>{};
            auto file_mutexes = std::map<std::string, std::mutex>{};
            for (const auto &data : paths)
            {
                threads.emplace_back([=, &file_mutexes]()
                                     { 
                auto lock_source = std::lock_guard<std::mutex>(file_mutexes[data[0]]);
                auto lock_destination = std::lock_guard<std::mutex>(file_mutexes[data[1]]);
                    Encode::encode_fs(data[0], data[1]); });
            }
            for (auto &thread : threads)
            {
                thread.join();
            }
            return;
        }
    };
}