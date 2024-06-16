#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/reflection_object_notation/common.hpp"

namespace Sen::Kernel::Support::PopCap::ReflectionObjectNotation
{
    using namespace Definition;
    struct Decode : Common
    {
    protected:
        template <auto write_key>
        inline static auto exchange_value(
            DataStreamView &stream,
            JsonWriter &value,
            std::vector<std::string> &native_string_index,
            std::vector<std::string> &unicode_string_index,
            TypeIdentifierEnumeration::Type const &type) -> void
        {
            static_assert(write_key == true || write_key == false, "write_key must be true or false");
            switch (type)
            {
            case TypeIdentifierEnumeration::Type::boolean_false:
            {
                value.WriteBoolean(false);
                break;
            }
            case TypeIdentifierEnumeration::Type::boolean_true:
            {
                value.WriteBoolean(true);
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_signed_8:
            {
                value.WriteValue(stream.readInt8());
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_signed_8_zero:
            {
                value.WriteValue(static_cast<int8_t>(0));
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_unsigned_8:
            {
                value.WriteValue(stream.readUint8());
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_unsigned_8_zero:
            {
                value.WriteValue(static_cast<uint8_t>(0));
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_signed_16:
            {
                value.WriteValue(stream.readInt16());
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_signed_16_zero:
            {
                value.WriteValue(static_cast<int16_t>(0));
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_unsigned_16:
            {
                value.WriteValue(stream.readUint16());
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_unsigned_16_zero:
            {
                value.WriteValue(static_cast<uint16_t>(0));
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_signed_32:
            {
                value.WriteValue(stream.readInt32());
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_signed_32_zero:
            {
                value.WriteValue(static_cast<int32_t>(0));
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_unsigned_32:
            {
                value.WriteValue(stream.readUint32());
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_unsigned_32_zero:
            {
                value.WriteValue(static_cast<uint32_t>(0));
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_signed_64:
            {
                value.WriteValue(stream.readInt64());
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_signed_64_zero:
            {
                value.WriteValue(static_cast<int64_t>(0));
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_unsigned_64:
            {
                value.WriteValue(stream.readUint64());
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_unsigned_64_zero:
            {
                value.WriteValue(static_cast<uint64_t>(0));
                break;
            }
            case TypeIdentifierEnumeration::Type::floater_signed_32:
            {
                value.WriteValue(stream.readFloat());
                break;
            }
            case TypeIdentifierEnumeration::Type::floater_signed_32_zero:
            {
                value.WriteValue(static_cast<float>(0.0f));
                break;
            }
            case TypeIdentifierEnumeration::Type::floater_signed_64:
            {
                value.WriteValue(stream.readDouble());
                break;
            }
            case TypeIdentifierEnumeration::Type::floater_signed_64_zero:
            {
                value.WriteValue(static_cast<double>(0.0));
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_variable_length_unsigned_32:
            case TypeIdentifierEnumeration::Type::integer_variable_length_unsigned_32_equivalent:
            {
                value.WriteValue(stream.readVarInt32());
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_variable_length_signed_32:
            {
                value.WriteValue(stream.readZigZag32());
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_variable_length_unsigned_64:
            case TypeIdentifierEnumeration::Type::integer_variable_length_unsigned_64_equivalent:
            {
                value.WriteValue(stream.readVarInt64());
                break;
            }
            case TypeIdentifierEnumeration::Type::integer_variable_length_signed_64:
            {
                value.WriteValue(stream.readZigZag64());
                break;
            }
            case TypeIdentifierEnumeration::Type::string_native:
            {
                if constexpr (write_key)
                {
                    value.WritePropertyName(stream.readStringByVarInt32());
                }
                else
                {
                    value.WriteValue(stream.readStringByVarInt32());
                }
                break;
            }
            case TypeIdentifierEnumeration::Type::string_native_indexing:
            {

                auto content = stream.readStringByVarInt32();
                if constexpr (write_key)
                {
                    value.WritePropertyName(content);
                }
                else
                {
                    value.WriteValue(content);
                }
                native_string_index.emplace_back(content);
                break;
            }
            case TypeIdentifierEnumeration::Type::string_native_indexed:
            {
                auto index = stream.readVarInt32();
                if constexpr (write_key)
                {
                    value.WritePropertyName(native_string_index[index]);
                }
                else
                {
                    value.WriteValue(native_string_index[index]);
                }
                break;
            }
            case TypeIdentifierEnumeration::Type::string_unicode:
            {
                auto length = stream.readVarInt32();
                auto content = stream.readStringByVarInt32();
                if constexpr (write_key)
                {
                    value.WritePropertyName(content);
                }
                else
                {
                    value.WriteValue(content);
                }
                break;
            }
            case TypeIdentifierEnumeration::Type::string_unicode_indexing:
            {
                auto length = stream.readVarInt32();
                auto content = stream.readStringByVarInt32();
                if constexpr (write_key)
                {
                    value.WritePropertyName(content);
                }
                else
                {
                    value.WriteValue(content);
                }
                unicode_string_index.emplace_back(content);
                break;
            }
            case TypeIdentifierEnumeration::Type::string_unicode_indexed:
            {
                auto index = stream.readVarInt32();
                if constexpr (write_key)
                {
                    value.WritePropertyName(unicode_string_index[index]);
                }
                else
                {
                    value.WriteValue(unicode_string_index[index]);
                }
                break;
            }
            case TypeIdentifierEnumeration::Type::string_rtid:
            {
                switch (RTIDTypeIdentifierEnumeration::Type{stream.readUint8()})
                {
                case RTIDTypeIdentifierEnumeration::Type::null:
                {
                    if constexpr (write_key)
                    {
                        value.WritePropertyName(RTIDString::null);
                    }
                    else
                    {
                        value.WriteValue(RTIDString::null);
                    }
                    break;
                }
                case RTIDTypeIdentifierEnumeration::Type::uid:
                {
                    auto sheet_length = stream.readVarInt32();
                    auto sheet_content = stream.readStringByVarInt32();
                    auto uid_middle = stream.readVarInt32();
                    auto uid_first = stream.readVarInt32();
                    auto uid_last = stream.readUint32();
                    if constexpr (write_key)
                    {
                        value.WritePropertyName(fmt::format(RTIDString::uid, uid_first, uid_middle, uid_last, sheet_content));
                    }
                    else
                    {
                        value.WriteValue(fmt::format(RTIDString::uid, uid_first, uid_middle, uid_last, sheet_content));
                    }

                    break;
                }
                case RTIDTypeIdentifierEnumeration::Type::alias:
                {
                    auto sheet_length = stream.readVarInt32();
                    auto sheet_content = stream.readStringByVarInt32();
                    auto alias_length = stream.readVarInt32();
                    auto alias_content = stream.readStringByVarInt32();
                    if constexpr (write_key)
                    {
                        value.WritePropertyName(fmt::format(RTIDString::alias, alias_content, sheet_content));
                    }
                    else
                    {
                        value.WriteValue(fmt::format(RTIDString::alias, alias_content, sheet_content));
                    }

                    break;
                }
                default:
                    assert_conditional(false, fmt::format("{}. {}: {:02x}", Kernel::Language::get("popcap.rton.decode.invalid_bytecode"), Kernel::Language::get("offset"), static_cast<uint8_t>(type)), "exchange_value");
                }
            }
            case TypeIdentifierEnumeration::Type::string_rtid_null:
            {
                value.WriteValue(RTIDString::null);
                break;
            }
            case TypeIdentifierEnumeration::Type::array_begin:
            {
                value.WriteStartArray();
                assert_conditional(TypeIdentifierEnumeration::Type{stream.readUint8()} == TypeIdentifierEnumeration::Type::array_size, fmt::format("{} {:02x}. {}: {:02x}", Kernel::Language::get("popcap.rton.decode.invalid_rton_array_starts"), static_cast<uint8_t>(TypeIdentifierEnumeration::Type::array_size), Kernel::Language::get("offset"), stream.read_pos), "exchange_value");
                auto size = stream.readVarInt32();
                auto array_size = 0;
                while (true)
                {
                    auto value_type_identifier = TypeIdentifierEnumeration::Type{stream.readUint8()};
                    if (value_type_identifier == TypeIdentifierEnumeration::Type::array_end)
                    {
                        break;
                    }
                    ++array_size;
                    exchange_value<false>(stream, value, native_string_index, unicode_string_index, value_type_identifier);
                }
                assert(array_size == size);
                value.WriteEndArray();
                break;
            }
            case TypeIdentifierEnumeration::Type::object_begin:
            {
                value.WriteStartObject();
                while (true)
                {
                    auto key_type_identifier = TypeIdentifierEnumeration::Type{stream.readUint8()};
                    if (key_type_identifier == TypeIdentifierEnumeration::Type::object_end)
                    {
                        break;
                    }
                    exchange_value<true>(stream, value, native_string_index, unicode_string_index, key_type_identifier);
                    auto value_type_identifier = TypeIdentifierEnumeration::Type{stream.readUint8()};
                    exchange_value<false>(stream, value, native_string_index, unicode_string_index, value_type_identifier);
                }
                value.WriteEndObject();
            }
            default:
                assert_conditional(false, fmt::format("{}. {}: {:02x}", Kernel::Language::get("popcap.rton.decode.invalid_bytecode"), Kernel::Language::get("offset"), static_cast<uint8_t>(type)), "exchange_value");
            }
            return;
        }

    public:
        inline static auto process_whole(
            DataStreamView &stream,
            JsonWriter &value) -> void
        {
            assert_conditional(stream.readString(4) == k_magic_identifier, fmt::format("{}", Kernel::Language::get("popcap.rton.decode.invalid_rton_magic")), "process_whole");
            assert_conditional(stream.readUint32() == k_version, "invalid_rton_version", "process_whole"); // TODO: add to localization.
            auto native_string_index = std::vector<std::string>{};
            auto unicode_string_index = std::vector<std::string>{};
            exchange_value<false>(stream, value, native_string_index, unicode_string_index, TypeIdentifierEnumeration::Type::object_begin);
            assert_conditional(stream.readString(4) == k_done_identifier, "invalid_rton_done", "process_whole"); // TODO: add to localization.
            return;
        }

        inline static auto process_fs(
            std::string_view source,
            std::string_view definition) -> void
        {
            auto stream = DataStreamView{source};
            auto writer = JsonWriter{};
            writer.WriteIndent = true;
            process_whole(stream, writer);
            FileSystem::write_file(definition, writer.ToString());
            return;
        }
    };
}