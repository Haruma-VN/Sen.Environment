#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::RTON
{

    // Use DataStream

    using namespace Definition;

    // Use simdjson

    using namespace simdjson;

    struct Encode
    {
        
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
            std::map<std::string_view, int> r0x90_stringpool;
            std::map<std::string_view, int> r0x92_stringpool;
            int r0x90_index;
            int r0x92_index;
            DataStreamView sen;

        protected:
            inline auto encode_rton(
                ondemand::document &json
            ) -> DataStreamView
            {
                sen.close();
                sen.writeStringView(RTON_head);
                sen.writeUint32(RTON_vesion);
                auto object = static_cast<ondemand::object>(json.get_object());
                write_object(object);
                sen.writeStringView(RTON_end);
                return sen;
        }

        inline auto write_object(
            ondemand::object object
        ) -> void
        {
            for (auto field : object)
            {
                write_string(field.unescaped_key());
                write_value(field.value());
            }
            sen.writeUint8(object_end);
            return;
        }

        inline auto write_array(
            ondemand::array array
        ) -> void
        {
            sen.writeUint8(array_start);
            sen.writeVarInt32((int)array.count_elements());
            for (auto child : array)
            {
                write_value(child.value());
            }
            sen.writeUint8(array_end);
            return;
        }

        inline auto write_value(
            ondemand::value element
        ) -> void
        {
            switch (element.type())
            {
            case ondemand::json_type::object:
            {
                sen.writeUint8(object_begin);
                write_object(element.get_object());
                break;
            }
            case ondemand::json_type::array:
            {
                sen.writeUint8(array_begin);
                write_array(element.get_array());
                break;
            }
            case ondemand::json_type::boolean:
            {
                sen.writeBoolean(static_cast<bool>(element.get_bool()));
                break;
            }
            case ondemand::json_type::null:
            {
                sen.writeUint8(null_byte);
                break;
            }
            case ondemand::json_type::string:
            {
                write_string(static_cast<std::string_view>(element.get_string()));
                break;
            }
            case ondemand::json_type::number:
            {
                if (element.is_integer())
                {
                    write_num(static_cast<int64_t>(element.get_int64()));
                }
                else
                {
                    write_float(static_cast<double>(element.get_double()));
                }
                break;
            }
            default:
            {
                throw Exception("invalid value", std::source_location::current(), "write_value");
            }
            }
            return;
        }

        inline auto constexpr write_float(
            double num
        ) -> void
        {
            if (num == 0.0)
            {
                sen.writeUint8(0x23);
            }
            else
            {
                if (static_cast<float>(num) == num)
                {
                    sen.writeUint8(0x22);
                    sen.writeFloat(num);
                }
                else
                {
                    sen.writeUint8(0x42);
                    sen.writeDouble(num);
                }
            }
            return;
        }

        inline auto constexpr write_num(
            int64_t num
        ) -> void
        {
            if (num == 0)
            {
                sen.writeUint8(0x21);
            }
            else if (0 <= num and num <= 2097151)
            {
                sen.writeUint8(0x24);
                sen.writeVarInt32(num);
            }
            else if (-1048576 <= num and num <= 0)
            {
                sen.writeUint8(0x25);
                sen.writeZigZag32(num);
            }
            else if (-2147483648 <= num and num <= 2147483648)
            {
                sen.writeUint8(0x20);
                sen.writeInt32(num);
            }
            else if (0 <= num and num <= 4294967295)
            {
                sen.writeUint8(0x26);
                sen.writeUint32(num);
            }
            else if (0 <= num and num <= 562949953421311)
            {
                sen.writeUint8(0x44);
                sen.writeVarInt64(num);
            }
            else if (-281474976710656 <= num and num <= 0)
            {
                sen.writeUint8(0x45);
                sen.writeZigZag64(num);
            }
            else if (-9223372036854775808 <= num and num <= 9223372036854775807)
            {
                sen.writeUint8(0x40);
                sen.writeInt64(num);
            }
            else if (0 <= num and num > 9223372036854775807)
            {
                sen.writeUint8(0x46);
                sen.writeUint64(num);
            }
            else if (0 <= num)
            {
                sen.writeUint8(0x44);
                sen.writeZigZag32(num);
            }
            else
            {
                sen.writeUint8(0x45);
                sen.writeZigZag64(num);
            }
            return;
        }

        inline auto constexpr write_string(
            std::string_view str
        ) -> void
        {
            if (str == star)
            {
                sen.writeUint8(0x02);
            }
            else if (write_binary(str))
                return;
            else if (write_RTID(str))
                return;
            else if (is_ascii(str))
            {
                if (r0x90_stringpool.contains(str))
                {
                    sen.writeUint8(0x91);
                    sen.writeVarInt32(r0x90_stringpool[str]);
                }
                else
                {
                    sen.writeUint8(0x90);
                    sen.writeStringViewByVarInt32(str);
                    r0x90_stringpool.insert({str, r0x90_index++});
                }
            }
            else
            {
                if (r0x92_stringpool.contains(str))
                {
                    sen.writeUint8(0x93);
                    sen.writeVarInt32(r0x92_stringpool[str]);
                }
                else
                {
                    sen.writeUint8(0x92);
                    sen.writeVarInt32(get_utf8_size(str));
                    sen.writeStringViewByVarInt32(str);
                    r0x92_stringpool.insert({str, r0x92_index++});
                }
            }
            return;
        }

        inline auto constexpr get_utf8_size(
            std::string_view q
        ) noexcept -> std::size_t
        {
            auto utf8_size = static_cast<std::size_t>(0);
            for (auto & i : q)
            {
                if (i <= 0b01111111){
                    ++utf8_size;
                }
                else if (i >= 0b11000010 && i <= 0b11011111){
                    ++utf8_size;
                }
                else if (i >= 0b11100001 && i <= 0b11101111){
                    ++utf8_size;
                }
                else if (i >= 0b11110000 && i <= 0b11110111){
                    ++utf8_size;
                }
            }
            return utf8_size;
        }

        inline auto constexpr is_ascii(
            std::string_view str
        ) noexcept -> bool
        {
            /*
            for (auto &c : str) {
                if ((u_char)c > 127) {
                    return false;
                }
            }
            */
            if (get_utf8_size(str) != str.length()) {
                return false;
            }
            return true;
        }

        inline auto write_RTID(
            std::string_view str
        ) -> bool
        {
            auto p_str = String{str};
            if (str.starts_with(rtid_begin) and str.ends_with(rtid_close))
            {
                if (str == rtid_0)
                {
                    sen.writeUint8(null_byte);
                    return true;
                }
                auto new_str = p_str.slice(5, p_str.length() - 1);
                if (new_str.indexOf(rtid_seperator) != -1)
                {
                    auto name_str = new_str.split(rtid_seperator);
                    auto dot_count = static_cast<std::size_t>(std::count(name_str[0].begin(), name_str[0].end(), '.'));
                    sen.writeUint8(0x83);
                    if (dot_count == 2)
                    {
                        auto int_str = String{name_str[0]}.split(rtid_dot);
                        sen.writeUint8(0x02);
                        sen.writeVarInt32(name_str[1].length());
                        sen.writeStringByVarInt32(name_str[1]);
                        sen.writeVarInt32(std::stoi(int_str[1]));
                        sen.writeVarInt32(std::stoi(int_str[0]));
                        auto hex_string = int_str[2];
                        std::reverse(hex_string.begin(), hex_string.end());
                        auto hex_int = static_cast<int>(strtol(hex_string.c_str(), NULL, 16));
                        sen.writeInt32(hex_int);
                    }
                    else
                    {
                        sen.writeUint8(0x03);
                        sen.writeVarInt32(name_str[1].length());
                        sen.writeStringByVarInt32(name_str[1]);
                        sen.writeVarInt32(name_str[0].length());
                        sen.writeStringByVarInt32(name_str[0]);
                    }
                    return true;
                }
            }
            return false;
        }

        inline auto write_binary(
            std::string_view str_v
        ) -> bool
        {
            auto p_str = String{str_v};
            if (str_v.starts_with(binary_begin) and str_v.ends_with(binary_end))
            {
                auto index = p_str.lastIndexOf(binary_seperator);
                if (index == -1)
                {
                    return false;
                }
                auto v = 0;
                try
                {
                    v = std::stoi(p_str.value.substr(index + 3, p_str.length() - 1));
                }
                catch (int err)
                {
                    return false;
                }
                sen.writeUint8(0x87);
                sen.writeUint8(0x00);
                sen.writeStringViewByVarInt32(str_v.substr(9, index));
                sen.writeVarInt32(v);
                return true;
            }
            return false;
        }

    public:

        explicit Encode(

        ) noexcept = default;

        ~Encode(

        ) = default;

        inline static auto instance(

        ) -> Encode&
        {
            static auto INSTANCE = Encode{};
            return INSTANCE;
        }

        inline static auto encode_fs(
            std::string_view source,
            std::string_view destination
        ) -> void
        {
            auto parser = ondemand::parser{};
            auto str = padded_string::load(source);
            auto json = static_cast<ondemand::document>(parser.iterate(str));
            Encode::instance().encode_rton(json).out_file(destination);
            return;
        }

        // ---------------------------------------------

        inline static auto encode_fs_as_multiple_threads(
            const std::vector<std::vector<std::string>> & paths
        ) -> void 
        {
            auto threads = std::vector<std::thread>{};
            auto file_mutexes = std::map<std::string, std::mutex>{};
            for (const auto & data : paths) {
                threads.emplace_back([=, &file_mutexes]() { 
                auto lock_source = std::lock_guard<std::mutex>(file_mutexes[data[0]]);
                auto lock_destination = std::lock_guard<std::mutex>(file_mutexes[data[1]]);
                    Encode::encode_fs(data[0], data[1]); 
                });
            }
            for (auto & thread : threads) {
                thread.join();
            }
            return;
        }
    };
}
