#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/reflection_object_notation/common.hpp"

namespace Sen::Kernel::Support::PopCap::ReflectionObjectNotation
{
    using namespace Definition;

    using namespace simdjson;

    class Encode : Common
    {
    protected:
        inline static auto exchange_string(
            DataStreamView &stream,
            std::string_view value,
            std::unordered_map<std::string_view, int> &native_string_index,
            std::unordered_map<std::string_view, int> &unicode_string_index) -> void
        {
            auto is_rtid = false;
            if (value.size() > "RTID(0)"_sv.size() && value.starts_with("RTID("_sv) && value.ends_with(")"_sv))
            {
                is_rtid = true;
                if (value == RTIDString::null)
                {
                    stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::string_rtid_null));
                }
                else
                {
                    stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::string_rtid));
                    auto content = value.substr("RTID"_sv.size(), value.size() - ")"_sv.size());
                    auto at_position = content.find("@_sv");
                    if (at_position != std::string::npos)
                    {
                        auto head_content = content.substr(0, at_position);
                        auto tail_content = content.substr(at_position, value.size() - 1);
                        if (static_cast<size_t>(std::count(head_content.begin(), head_content.end(), '.')) == size_t{2})
                        {
                            stream.writeUint8(static_cast<uint8_t>(RTIDTypeIdentifierEnumeration::Type::uid));
                            auto sheet_length = compute_utf8_character_length(tail_content);
                            stream.writeVarInt32(sheet_length);
                            stream.writeStringByVarInt32(tail_content);
                            auto uid_first = int32_t{};
                            auto uid_middle = int32_t{};
                            auto uid_last = int32_t{};
                            auto scan_result = std::sscanf(head_content.data(), "%u.%u.%x", &uid_first, &uid_middle, &uid_last);
                            assert(scan_result == 3);
                            stream.writeVarInt32(uid_middle);
                            stream.writeVarInt32(uid_first);
                            stream.writeInt32(uid_last);
                        }
                        else
                        {
                            stream.writeUint8(static_cast<uint8_t>(RTIDTypeIdentifierEnumeration::Type::alias));
                            auto sheet_length = compute_utf8_character_length(tail_content);
                            stream.writeVarInt32(sheet_length);
                            stream.writeStringByVarInt32(tail_content);
                            auto alias_length = compute_utf8_character_length(head_content);
                            stream.writeVarInt32(alias_length);
                            stream.writeStringByVarInt32(head_content);
                        }
                    }
                }
            }
            if (!is_rtid)
            {
                if (auto string_length = compute_utf8_character_length(value) == value.size())
                {
                    if (auto indexed_string = native_string_index.find(value); indexed_string != native_string_index.end())
                    {
                        stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::string_native_indexed));
                        stream.writeVarInt32(indexed_string->second);
                    }
                    else
                    {
                        stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::string_native_indexing));
                        stream.writeStringByVarInt32(value);
                        native_string_index[value] = native_string_index.size();
                    }
                }
                else
                {
                    if (auto indexed_string = unicode_string_index.find(value); indexed_string != unicode_string_index.end())
                    {
                        stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::string_unicode_indexed));
                        stream.writeVarInt32(static_cast<int>(std::distance(unicode_string_index.begin(), indexed_string)));
                    }
                    else
                    {
                        stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::string_unicode_indexing));
                        stream.writeVarInt32(string_length);
                        stream.writeStringByVarInt32(value);
                        unicode_string_index[value] = unicode_string_index.size();
                    }
                }
            }
            return;
        }

        inline static auto exchange_number(
            DataStreamView &stream,
            ondemand::value value) -> void
        {
            if (value.is_negative())
            {
                if (value.is_integer())
                {
                    auto negative_value = static_cast<int64_t>(value.get_int64());
                    if (negative_value == static_cast<int64_t>(0))
                    {
                        stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::integer_unsigned_8_zero));
                    }
                    else if (negative_value + 0x40000000ll >= 0ll)
                    {
                        stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::integer_variable_length_signed_32));
                        stream.writeZigZag32(negative_value);
                    }
                    else if ((std::numeric_limits<int32_t>::min)() <= negative_value)
                    {
                        stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::integer_signed_32));
                        stream.writeInt32(negative_value);
                    }
                    else if ((std::numeric_limits<int64_t>::min)() <= negative_value)
                    {
                        stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::integer_signed_64));
                        stream.writeInt64(negative_value);
                    }
                    else
                    {
                        stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::integer_variable_length_signed_64));
                        stream.writeZigZag64(negative_value);
                    }
                }
                else
                {
                    if (static_cast<double>(value.get_double()) == static_cast<double>(0.0))
                    {
                        stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::floater_signed_64_zero));
                    }
                    else
                    {
                        stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::floater_signed_64));
                        stream.writeDouble(static_cast<double>(value.get_double()));
                    }
                }
            }
            else if (value.is_integer())
            {
                auto integer_unsigned_value = static_cast<uint64_t>(value.get_uint64());
                if (integer_unsigned_value == 0_ul)
                {
                    stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::integer_unsigned_8_zero));
                }
                else if (integer_unsigned_value < (std::numeric_limits<int32_t>::max)())
                {
                    stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::integer_variable_length_unsigned_32));
                    stream.writeVarInt32(integer_unsigned_value);
                }
                else if (integer_unsigned_value < (std::numeric_limits<uint32_t>::max)())
                {
                    stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::integer_unsigned_32));
                    stream.writeUint32(integer_unsigned_value);
                }
                else if (integer_unsigned_value < (std::numeric_limits<uint64_t>::max)())
                {
                    stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::integer_unsigned_64));
                    stream.writeInt64(integer_unsigned_value);
                }
                else
                {
                    stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::integer_variable_length_unsigned_64));
                    stream.writeVarInt64(integer_unsigned_value);
                }
            }
            else
            {
                if (static_cast<double>(value.get_double()) == static_cast<double>(0.0))
                {
                    stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::floater_signed_64_zero));
                }
                else
                {
                    stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::floater_signed_64));
                    stream.writeDouble(static_cast<double>(value.get_double()));
                }
            }
            return;
        }

        template <auto remove_start_object = false>
        inline static auto exchange_value(
            DataStreamView &stream,
            ondemand::value value,
            std::unordered_map<std::string_view, int> &native_string_index,
            std::unordered_map<std::string_view, int> &unicode_string_index) -> void
        {
            static_assert(remove_start_object == true || remove_start_object == false, "remove_start_object must be true or false");
            switch (value.type())
            {
            case ondemand::json_type::null:
            {
                assert_conditional(false, "type is null", "exchange_value");
                break;
            }
            case ondemand::json_type::boolean:
            {
                stream.writeBoolean(static_cast<bool>(value.get_bool()));
                break;
            }
            case ondemand::json_type::string:
            {
                exchange_string(stream, static_cast<std::string_view>(value.get_string()), native_string_index, unicode_string_index);
                break;
            }
            case ondemand::json_type::number:
            {
                exchange_number(stream, value);
                break;
            }
            case ondemand::json_type::array:
            {
                stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::array_begin));
                stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::array_size));
                stream.writeVarInt32(static_cast<int>(value.count_elements()));
                for (auto element : value.get_array())
                {
                    exchange_value<false>(stream, element.value(), native_string_index, unicode_string_index);
                }
                stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::array_end));
                break;
            }
            case ondemand::json_type::object:
            {
                if constexpr (!remove_start_object)
                {
                    stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::object_begin));
                }
                for (auto element : value.get_object())
                {
                    exchange_string(stream, static_cast<std::string_view>(element.unescaped_key()), native_string_index, unicode_string_index);
                    exchange_value<false>(stream, element.value(), native_string_index, unicode_string_index);
                }
                stream.writeUint8(static_cast<uint8_t>(TypeIdentifierEnumeration::Type::object_end));
                break;
            }
            default:
            {
                assert_conditional(false, "invalid_type", "exchange_value"); // TODO: add to localization.
            }
            }
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            std::string const &content) -> void
        {
            auto ondemand_parser = ondemand::parser{};
            auto padded_string = simdjson::padded_string(content);
            auto document = static_cast<ondemand::document>(ondemand_parser.iterate(padded_string));
            stream.writeString(k_magic_identifier);
            stream.writeUint32(k_version);
            auto native_string_index = std::unordered_map<std::string_view, int>{};
            auto unicode_string_index = std::unordered_map<std::string_view, int>{};
            exchange_value<true>(stream, document.get_value(), native_string_index, unicode_string_index);
            stream.writeString(k_done_identifier);
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view destination) -> void
        {
            auto stream = DataStreamView{};
            process_whole(stream, FileSystem::read_file(source));
            stream.out_file(destination);
            return;
        }
    };

}