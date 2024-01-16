#pragma once

#include "kernel/definition/utility.hpp"

namespace Sen::Kernel::Support::PopCap::RTON
{
    using namespace Definition;

    class Encode
    {
    protected:

        inline auto encode_rton(
            const nlohmann::ordered_json & json
        ) -> DataStreamView
        {
            auto r0x90_stringpool = nlohmann::ordered_json::object();
            auto r0x92_stringpool = nlohmann::ordered_json::object();
            auto r0x90_index = 0;
            auto r0x92_index = 0;
            auto sen = DataStreamView{};
            sen.writeString("RTON");
            sen.writeUint32(0x01);
            write_object(json, sen, r0x90_stringpool, r0x92_stringpool, r0x90_index, r0x92_index);
            sen.writeString("DONE");
            return sen;
        }

        inline auto write_object(
            const nlohmann::ordered_json &json,
            DataStreamView &sen,
            nlohmann::ordered_json &r0x90_stringpool,
            nlohmann::ordered_json &r0x92_stringpool,
            int & r0x90_index,
            int & r0x92_index
        ) -> void
        {
            for (auto &[key, value] : json.items())
            {
                write_string(key, sen, r0x90_stringpool, r0x92_stringpool, r0x90_index, r0x92_index);
                write_value(value, sen, r0x90_stringpool, r0x92_stringpool, r0x90_index, r0x92_index);
            }
            sen.writeUint8(0xFF);
            return;
        }

        inline auto write_array(
            const nlohmann::ordered_json & json,
            DataStreamView & sen,
            nlohmann::ordered_json & r0x90_stringpool,
            nlohmann::ordered_json & r0x92_stringpool,
            int & r0x90_index,
            int & r0x92_index
        ) -> void
        {
            sen.writeUint8(0xFD);
            auto array_length = json.size();
            sen.writeVarInt32(array_length);
            for (auto value : json)
            {
                write_value(value, sen, r0x90_stringpool, r0x92_stringpool, r0x90_index, r0x92_index);
            }
            sen.writeUint8(0xFE);
            return;
        }

        inline auto write_value(
            const nlohmann::ordered_json & value,
            DataStreamView & sen,
            nlohmann::ordered_json & r0x90_stringpool,
            nlohmann::ordered_json & r0x92_stringpool,
            int & r0x90_index,
            int & r0x92_index
        ) -> void
        {
            using value_t = nlohmann::ordered_json::value_t;
            switch (value.type())
            {
            case value_t::object:{
                sen.writeUint8(0x85);
                write_object(value, sen, r0x90_stringpool, r0x92_stringpool, r0x90_index, r0x92_index);
                break;
            }
            case value_t::array:{
                sen.writeUint8(0x86);
                write_array(value, sen, r0x90_stringpool, r0x92_stringpool, r0x90_index, r0x92_index);
                break;
            }
            case value_t::boolean:{
                auto bool_value = value.get<bool>();
                sen.writeBoolean(bool_value);
                break;
            }
            case value_t::null:{
                sen.writeUint8(0x84);
                break;
            }
            case value_t::string:{
                write_string(value.get<string>(), sen, r0x90_stringpool, r0x92_stringpool, r0x90_index, r0x92_index);
                break;
            }
            case value_t::number_integer:{
                write_num(value.get<long>(), sen);
                break;
            }
            case value_t::number_unsigned:{
                write_num(value.get<u_long>(), sen);
                break;
            }
            case value_t::number_float:{
                write_float(value, sen);
                break;
            }
            default:{
                throw Exception("invalid value");
            }
            }
            return;
        }

        inline auto write_float(
            const nlohmann::ordered_json & num,
            DataStreamView & sen
        ) -> void
        {
            auto f64 = num.get<double>();
            if (num == 0.0)
            {
                sen.writeUint8(0x23);
            }
            else
            {
                auto f32 = num.get<float>();
                if ((double)f32 == f64)
                {
                    sen.writeUint8(0x22);
                    sen.writeFloat(f32);
                }
                else
                {
                    sen.writeUint8(0x42);
                    sen.writeDouble(f64);
                }
            }
            return;
        }

        inline auto write_num(
            const size_t & num, 
            DataStreamView &sen
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

        inline auto write_string(
            const std::string & str,
            DataStreamView & sen,
            nlohmann::ordered_json & r0x90_stringpool,
            nlohmann::ordered_json & r0x92_stringpool,
            int & r0x90_index,
            int & r0x92_index
        ) -> void
        {
            if (str == "*")
            {
                sen.writeUint8(2);
            }
            else if (write_binary(str, sen))
                return;
            else if (write_RTID(str, sen))
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
                    sen.writeStringByVarInt32(str);
                    r0x90_stringpool += {str, r0x90_index++};
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
                    sen.writeStringByVarInt32(str);
                    r0x92_stringpool += {str, r0x92_index++};
                }
            }
            return;
        }

        inline auto is_ascii(
            const std::string & str
        ) -> bool
        {
            for (auto &c : str)
            {
                if ((int)c > 127)
                    return false;
            };
            return true;
        }

        inline auto write_RTID(
            const std::string & str, 
            DataStreamView &sen
        ) -> bool
        {
            auto p_str = String{str};
            if (p_str.startsWith(std::string{"RTID("}) and p_str.endsWith(std::string{")"}))
            {
                if (str == std::string{"RTID(0)"})
                {
                    sen.writeUint8(0x84);
                    return true;
                }
                auto new_str = p_str.slice(5, p_str.length() - 1);
                if (new_str.indexOf(std::string{"@"}) != -1)
                {
                    auto name_str = new_str.split(std::string{"@"});
                    auto s = std::string {"132AB.78.BB"};
                    auto re = std::regex("\\.", std::regex_constants::egrep);
                    auto c = std::distance(std::sregex_token_iterator(s.begin(), s.end(), re), std::sregex_token_iterator());
                    sen.writeUint8(0x83);
                    if (c == 2)
                    {
                        auto int_str = String{name_str[0]}.split(".");
                        sen.writeUint8(0x02);
                        sen.writeVarInt32(name_str[1].length());
                        sen.writeStringByVarInt32(name_str[1]);
                        sen.writeVarInt32(std::stoi(int_str[1]));
                        sen.writeVarInt32(std::stoi(int_str[0]));
                        auto hex_string = int_str[2];
                        std::reverse(hex_string.begin(), hex_string.end());
                        auto hex_int = (int)strtol(hex_string.c_str(), NULL, 16);
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
            const std::string & str, 
            DataStreamView &sen
        ) -> bool
        {
            auto p_str = String{str};
            if (p_str.startsWith("$BINARY(\"") && p_str.endsWith(")"))
            {
                auto index = p_str.lastIndexOf("\", ");
                if (index == -1)
                {
                    return false;
                }
                auto v = 0;
                try
                {
                    v = std::stoi(str.substr(index + 3, str.length() - 1));
                }
                catch (int err)
                {
                    return false;
                }
                auto m_string = str.substr(9, index);
                sen.writeUint8(0x87);
                sen.writeUint8(0);
                sen.writeStringByVarInt32(m_string);
                sen.writeVarInt32(v);
                return true;
            }
            return false;
        }

    public:
        static auto encode_fs(
            const std::string & source,
            const std::string & destination
        ) -> void
        {
            return;
        }
    };
}
