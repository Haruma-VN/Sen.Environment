#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition
{
    namespace UTF8Json
    {

        template <typename T> concept HasEmptyMethod = requires(T a) { { a.empty() } -> std::same_as<void>; };
        
        class JsonConstants
        {
        public:
            inline static constexpr auto OpenBrace = std::string_view{"{"};
            inline static constexpr auto CloseBrace = std::string_view{"}"};
            inline static constexpr auto OpenBracket = std::string_view{"["};
            inline static constexpr auto CloseBracket = std::string_view{"]"};
            inline static constexpr auto Space = std::string_view{" "};
            inline static constexpr auto CarriageReturn = std::string_view{"\r"};
            inline static constexpr auto LineFeed = std::string_view{"\n"};
            inline static constexpr auto Tab = std::string_view{"\t"};
            inline static constexpr auto ListSeparator = std::string_view{","};
            inline static constexpr auto KeyValueSeparator = std::string_view{":"};
            inline static constexpr auto Quote = std::string_view{"\""};
            inline static constexpr auto BackSlash = std::string_view{"\\"};
            inline static constexpr auto Slash = std::string_view{"/"};
            inline static constexpr auto BackSpace = std::string_view{"\b"};
            inline static constexpr auto FormFeed = std::string_view{"\f"};
            inline static constexpr auto Asterisk = std::string_view{"*"};
            inline static constexpr auto Colon = std::string_view{":"};
            inline static constexpr auto Period = std::string_view{"."};
            inline static constexpr auto Plus = std::string_view{"+"};
            inline static constexpr auto Hyphen = std::string_view{"-"};
            inline static constexpr auto UtcOffsetToken = std::string_view{"Z"};
            inline static constexpr auto TimePrefix = std::string_view{"T"};

            inline static constexpr auto Utf8BOM = std::string_view{"ï»¿"};
            inline static constexpr auto TrueValue = std::string_view{"true"};
            inline static constexpr auto FalseValue = std::string_view{"false"};
            inline static constexpr auto NullValue = std::string_view{"null"};

            inline static constexpr auto RemoveFlagsBitMask = 0x7FFFFFFF;

            inline static constexpr auto Backspace_Point = 0x08u;
            inline static constexpr auto Horizontal_Tab_Point = 0x09u;
            inline static constexpr auto Newline_Point = 0x0Au;
            inline static constexpr auto Formfeed_Point = 0x0Cu;
            inline static constexpr auto Carriage_Return_Point = 0x0Du;
            inline static constexpr auto Quotation_Mark_Point = 0x22u;
            inline static constexpr auto Reverse_Solidus_Point = 0x5Cu;
            

            /*
            inline static auto quote_pattern = std::regex("\"");
            inline static auto backslash_pattern = std::regex("\\\\");
            inline static auto newline_pattern = std::regex("\n");
            inline static auto carriage_pattern = std::regex("\r");
            inline static auto tab_pattern = std::regex("\t");
            inline static auto backspace_pattern = std::regex("\b");
            inline static auto formfeed_pattern = std::regex("\f");
            */
        };

        /// This enum defines the various JSON tokens that make up a JSON text and is used by
        enum JsonTokenType
        {
            // Do not re-order.
            // We rely on the ordering to quickly check things like IsTokenTypePrimitive
            /*
                Indicates that there is no value (as distinct from <see cref="Null"/>).

                This is the default token type if no data has been read by the <see cref="Utf8JsonReader"/>.
            */
            None,
            /*
                Indicates that the token type is the start of a JSON object.
            */
            StartObject,
            /*
                Indicates that the token type is the end of a JSON object.
            */
            EndObject,
            /*
                Indicates that the token type is the start of a JSON array.
            */
            StartArray,
            /*
                Indicates that the token type is the end of a JSON array.
            */
            EndArray,
            /*
                Indicates that the token type is a JSON property name.
            */
            PropertyName,
            /*
                Indicates that the token type is the comment string.
            */
            Comment,
            /*
                Indicates that the token type is a JSON string.
            */
            String,
            /*
                Indicates that the token type is a JSON number.
            */
            Number,
            /*
                Indicates that the token type is the JSON literal <c>true</c>.
            */
            TrueValue,
            /*
                Indicates that the token type is the JSON literal <c>false</c>.
            */
            FalseValue,
            /*
                Indicates that the token type is the JSON literal <c>null</c>.
            */
            NullValue,
        };

        struct JsonWriterHelper
        {

        private:
            inline static constexpr auto utf8_d = std::array<std::uint8_t, 400>{
                {
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 00..1F
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 20..3F
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 40..5F
                    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, // 60..7F
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, // 80..9F
                    7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, // A0..BF
                    8, 8, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, // C0..DF
                    0xA, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x3, 0x4, 0x3, 0x3,                 // E0..EF
                    0xB, 0x6, 0x6, 0x6, 0x5, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8, 0x8,                 // F0..FF
                    0x0, 0x1, 0x2, 0x3, 0x5, 0x8, 0x7, 0x1, 0x1, 0x1, 0x4, 0x6, 0x1, 0x1, 0x1, 0x1,                 // s0..s0
                    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1, 1, 1, 0, 1, 0, 1, 1, 1, 1, 1, 1, // s1..s2
                    1, 2, 1, 1, 1, 1, 1, 2, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, // s3..s4
                    1, 2, 1, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, // s5..s6
                    1, 3, 1, 1, 1, 1, 1, 3, 1, 3, 1, 1, 1, 1, 1, 1, 1, 3, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1  // s7..s8
                }};


        protected:
            inline static auto Decode(
                std::uint8_t &state,
                std::uint32_t &codepoint,
                const std::uint8_t byte
            ) -> std::uint8_t
            {
                assert_conditional(byte < utf8_d.size(), fmt::format("{}", Language::get("json_writer.byte_cannot_smaller_than_utf8_size")), "Decode");
                const std::uint8_t type = utf8_d[byte];
                codepoint = (state != 0_byte)
                                ? (byte & 0x3fu) | (codepoint << 6u)
                                : (0xFFu >> type) & (byte);
                auto index = 256u + static_cast<size_t>(state) * 16u + static_cast<size_t>(type);
                assert_conditional(index < utf8_d.size(), fmt::format("{}", Language::get("json_writer.index_cannot_smaller_than_utf8_size")), "Decode");
                state = utf8_d[index];
                return state;
            }

        public:
            // Unnecessary.
            /*
            inline static auto Validate(std::string propertyName) -> void
            {
                return;
            }
            */

            inline static auto WriteEscapeString(
                std::string &output,
                const std::string &s
            ) -> void
            {
                /*
                if (value.find(JsonConstants::BackSlash) != std::string::npos)
                {
                    value = std::regex_replace(value, JsonConstants::backslash_pattern, "\\\\");
                }
                */
                auto codepoint = std::uint32_t{0};
                auto state = 0_byte;
                auto bytes = 0;
                auto string_buffer = std::array<char, 512>{{}};
                for (const auto &i : Range<size_t>(s.size()))
                {
                    const auto byte = static_cast<std::uint8_t>(s[i]);
                    switch (Decode(state, codepoint, byte))
                    {
                    case 0_byte:
                    {
                        switch (codepoint)
                        {
                        case JsonConstants::Backspace_Point: // backspace
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = 'b';
                            break;
                        }

                        case JsonConstants::Horizontal_Tab_Point: // horizontal tab
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = 't';
                            break;
                        }

                        case JsonConstants::Newline_Point: // newline
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = 'n';
                            break;
                        }

                        case JsonConstants::Formfeed_Point: // formfeed
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = 'f';
                            break;
                        }

                        case JsonConstants::Carriage_Return_Point: // carriage return
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = 'r';
                            break;
                        }

                        case JsonConstants::Quotation_Mark_Point: // quotation mark
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = '\"';
                            break;
                        }

                        case JsonConstants::Reverse_Solidus_Point: // reverse solidus
                        {
                            string_buffer[bytes++] = '\\';
                            string_buffer[bytes++] = '\\';
                            break;
                        }
                        default:
                            if ((codepoint <= 0x1F))
                            {
                                if (codepoint <= 0xFFFF)
                                {
                                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
                                    static_cast<void>((std::snprintf)(string_buffer.data() + bytes, 7, "\\u%04x",
                                                                      static_cast<std::uint16_t>(codepoint)));
                                    bytes += 6;
                                }
                                else
                                {
                                    // NOLINTNEXTLINE(cppcoreguidelines-pro-type-vararg,hicpp-vararg)
                                    static_cast<void>((std::snprintf)(string_buffer.data() + bytes, 13, "\\u%04x\\u%04x",
                                                                      static_cast<std::uint16_t>(0xD7C0u + (codepoint >> 10u)),
                                                                      static_cast<std::uint16_t>(0xDC00u + (codepoint & 0x3FFu))));
                                    bytes += 12;
                                }
                            }
                            else
                            {
                                string_buffer[bytes++] += s[i];
                            }
                            break;
                        }
                        if (string_buffer.size() - bytes < 13)
                        {
                            output.append(string_buffer.data(), bytes);
                            bytes = 0;
                        }
                        break;
                    }
                    case 1_byte:
                    default:
                    {
                        throw Exception(String::format(fmt::format("{}", Language::get("json_writer.invalid_utf8_byte_at_index")), String::decimal_to_hexadecimal(bytes)), std::source_location::current(), "WriteEscapeString");
                        break;
                    }
                    }
                }
                if (state == 0_byte) {
                    if (bytes > 0) {
                        output.append(string_buffer.data(), bytes);
                    }
                }
                else {
                    throw Exception(String::format(fmt::format("{}", Language::get("json_writer.incomplete_utf8_string")), fmt::format("0x{}", String::decimal_to_hexadecimal(bytes))), std::source_location::current(), "WriteEscapeString");
                }
                return;
            }

            inline static auto WriteNewLine(std::string &output) -> void
            {
                // Write '\r\n' OR '\n', depending on OS
                #if LINUX
                    output += JsonConstants::CarriageReturn;
                #endif
                output += JsonConstants::LineFeed;
                return;
            }

            inline static auto WriteIndentation(std::string &output, int indent) -> void
            {
                for (auto i : Range<int>(indent))
                {
                    output += JsonConstants::Tab;
                }
                return;
            }
        };

        // maybe has bug hex string and byte string when convert to escape string.

        struct JsonWriter
        {
        private:
            /// baseString to read or write Json.
            std::string mutable _baseString;

            bool mutable _commentAfterNoneOrProperyName;
            /// default None.
            JsonTokenType mutable _tokenType;

            // The highest order bit of _currentDepth is used to discern whether we are writing the first item in a list or not.
            // if (_currentDepth >> 31) == 1, add a list separator before writing the item
            // else, no list separator is needed since we are writing the first item.
            std::size_t mutable _currentDepth = 0;

            std::size_t mutable _indentLength = 1;

            inline auto WriteStart(const std::string &token) const -> void
            {
                if (thiz.WriteIndent)
                {
                    thiz.WriteStartIndented(token);
                }
                else
                {
                    if (thiz._currentDepth < 0)
                    {
                        thiz._baseString += JsonConstants::ListSeparator;
                    }
                    thiz._baseString += token;
                }
                thiz._currentDepth &= JsonConstants::RemoveFlagsBitMask;
                ++thiz._currentDepth;
                return;
            }

            /*
                Writes the UTF-8 property name (as a JSON string) as the first part of a name/value pair of a JSON object.
            */

            inline auto WriteStringPropertyName(const std::string &propertyName) const -> void
            {
                if (thiz.WriteIndent)
                {
                    thiz.WriteStringIndentedPropertyName(propertyName);
                }
                else
                {
                    if (thiz._currentDepth < 0)
                    {
                        thiz._baseString += JsonConstants::ListSeparator;
                    }
                    thiz._baseString += JsonConstants::Quote;
                    JsonWriterHelper::WriteEscapeString(thiz._baseString, propertyName);
                    thiz._baseString += JsonConstants::Quote;
                    thiz._baseString += JsonConstants::KeyValueSeparator;
                }
                thiz._currentDepth &= JsonConstants::RemoveFlagsBitMask;
                return;
            }

            inline auto WriteStringValue(const std::string &value) const -> void
            {
                if (thiz.WriteIndent)
                {
                    thiz.WriteStringIndented(value);
                }
                else
                {
                    if (thiz._currentDepth < 0)
                    {
                        thiz._baseString += JsonConstants::ListSeparator;
                    }
                    thiz._baseString += JsonConstants::Quote;
                    JsonWriterHelper::WriteEscapeString(thiz._baseString, value);
                    thiz._baseString += JsonConstants::Quote;
                }
                thiz.SetFlagToAddListSeparatorBeforeNextItem();
                return;
            }

            inline auto WriteSimpleIndented(const std::string &value) const -> void
            {
                if (thiz._currentDepth < 0)
                {
                    thiz._baseString += JsonConstants::ListSeparator;
                }
                if (thiz._tokenType != PropertyName)
                {
                    if (thiz._tokenType != None)
                    {
                        JsonWriterHelper::WriteNewLine(thiz._baseString);
                    }
                    auto indent = thiz._currentDepth & JsonConstants::RemoveFlagsBitMask * thiz._indentLength;
                    JsonWriterHelper::WriteIndentation(thiz._baseString, indent);
                }
                thiz._baseString += value;
                return;
            }

            inline auto WriteSimpleValue(const std::string &value) const -> void
            {
                if (thiz.WriteIndent)
                {
                    thiz.WriteSimpleIndented(value);
                }
                else
                {
                    if (thiz._currentDepth < 0)
                    {
                        thiz._baseString += JsonConstants::ListSeparator;
                    }
                    thiz._baseString += value;
                }
                thiz.SetFlagToAddListSeparatorBeforeNextItem();
                return;
            }

            inline auto WriteEnd(const std::string &token) const -> void
            {
                if (thiz.WriteIndent)
                {
                    thiz.WriteEndIndented(token);
                }
                else
                {
                    thiz._baseString += token;
                }
                thiz.SetFlagToAddListSeparatorBeforeNextItem();
                if (thiz._currentDepth != 0)
                {
                    --thiz._currentDepth;
                }
                return;
            }

            inline auto WriteStringIndented(const std::string &value) const -> void
            {
                if (thiz._currentDepth < 0)
                {
                    thiz._baseString += JsonConstants::ListSeparator;
                }
                if (thiz._tokenType != PropertyName)
                {
                    if (thiz._tokenType != None)
                    {
                        JsonWriterHelper::WriteNewLine(thiz._baseString);
                    }
                    auto indent = thiz._currentDepth & JsonConstants::RemoveFlagsBitMask * thiz._indentLength;
                    JsonWriterHelper::WriteIndentation(thiz._baseString, indent);
                }
                thiz._baseString += JsonConstants::Quote;
                JsonWriterHelper::WriteEscapeString(thiz._baseString, value);
                thiz._baseString += JsonConstants::Quote;
                return;
            }

            inline auto WriteStringIndentedPropertyName(const std::string &propertyName) const -> void
            {
                auto indent = thiz._currentDepth & JsonConstants::RemoveFlagsBitMask * thiz._indentLength;
                if (thiz._currentDepth < 0)
                {
                    thiz._baseString += JsonConstants::ListSeparator;
                }
                if (thiz._tokenType != None)
                {
                    JsonWriterHelper::WriteNewLine(thiz._baseString);
                }
                JsonWriterHelper::WriteIndentation(thiz._baseString, indent);
                thiz._baseString += JsonConstants::Quote;
                JsonWriterHelper::WriteEscapeString(thiz._baseString, propertyName);
                thiz._baseString += JsonConstants::Quote;
                thiz._baseString += JsonConstants::KeyValueSeparator;
                thiz._baseString += JsonConstants::Space;
                return;
            }

            inline auto WriteEndIndented(const std::string &token) const -> void
            {
                if (thiz._tokenType == StartArray || thiz._tokenType == StartObject)
                {
                    thiz._baseString += token;
                }
                else
                {
                    auto indent = thiz._currentDepth & JsonConstants::RemoveFlagsBitMask * thiz._indentLength;
                    if (indent != -0)
                    {
                        indent -= thiz._indentLength;
                    }
                    JsonWriterHelper::WriteNewLine(thiz._baseString);
                    JsonWriterHelper::WriteIndentation(thiz._baseString, indent);
                    thiz._baseString += token;
                }
                return;
            }

            inline auto WriteStartIndented(std::string_view token) const -> void
            {
                if (thiz._currentDepth < 0)
                {
                    thiz._baseString += JsonConstants::ListSeparator;
                }
                if (thiz._tokenType != None && thiz._tokenType != PropertyName)
                {
                    JsonWriterHelper::WriteNewLine(thiz._baseString);
                    auto indent = thiz._currentDepth & JsonConstants::RemoveFlagsBitMask * thiz._indentLength;
                    JsonWriterHelper::WriteIndentation(thiz._baseString, indent);
                }
                thiz._baseString += token;
                return;
            }

            inline auto SetFlagToAddListSeparatorBeforeNextItem() const -> void
            {
                thiz._currentDepth |= 1 << 31;
                return;
            }

        public:
            // Prettify Json.
            bool mutable WriteIndent;
            /*
                Reset to default.
            */
            inline auto Clear() const -> void
            {
                thiz._baseString.clear();
                thiz.WriteIndent = false;
                thiz._currentDepth = 0;
                thiz._tokenType = None;
                return;
            }
            /*
                Writes the beginning of a JSON object.
            */
            inline auto WriteStartObject() const -> void
            {
                thiz.WriteStart(JsonConstants::OpenBrace.data());
                thiz._tokenType = StartObject;
                return;
            }
            /*
                Writes the beginning of a JSON array.
            */
            inline auto WriteStartArray() const -> void
            {
                thiz.WriteStart(JsonConstants::OpenBracket.data());
                thiz._tokenType = StartArray;
                return;
            }
            /*
                Writes the end of a JSON object.
            */
            inline auto WriteEndObject() const -> void
            {
                thiz.WriteEnd(JsonConstants::CloseBrace.data());
                thiz._tokenType = EndObject;
                return;
            }
            /*
                Writes the end of a JSON array.
            */
            inline auto WriteEndArray() const -> void
            {
                thiz.WriteEnd(JsonConstants::CloseBracket.data());
                thiz._tokenType = EndArray;
                return;
            }
            /*!
                Writes the property name (as a JSON string) as the first part of a name/value pair of a JSON object.
                @param[in] properyName The name of the property to write.
            */
            inline auto WritePropertyName(const std::string &propertyName) const -> void
            {
                thiz.WriteStringPropertyName(propertyName);
                thiz._tokenType = PropertyName;
                return;
            }
            /*!
                Writes the property name (as a JSON string) as the first part of a name/value pair of a JSON object.
                @param[in] properyName The name of the property to write.
            */
            template <HasEmptyMethod T> 
            inline auto WritePropertyName(T propertyName) const -> void
            {
                auto name = std::to_string(propertyName);
                thiz.WriteStringPropertyName(name);
                thiz._tokenType = PropertyName;
                return;
            }
            /*!
                Writes the pre-encoded text value (as a JSON string) as an element of a JSON.
                @param[in] value The JSON-encoded value to write.
            */
            inline auto WriteValue(const std::string &value) const -> void
            {
                thiz.WriteStringValue(value);
                thiz._tokenType = String;
                return;
            }
            /*!
                Writes the number value (as a JSON number) as an element of a JSON.
                @param[in] value The JSON-encoded value to write.
            */
            template <typename T>
                requires std::is_arithmetic_v<T>
            inline auto WriteValue(T value) const -> void
            {
                auto num_value = std::to_string(value);
                thiz.WriteSimpleValue(num_value);
                thiz._tokenType = Number;
                return;
            }

            /*!
                Writes the null value (as a JSON literal "null") as an element of a JSON.
            */
            inline auto WriteNull() const -> void
            {
                thiz.WriteSimpleValue(JsonConstants::NullValue.data());
                thiz._tokenType = NullValue;
                return;
            }
            /*!
                Write the boolean vaule (as a JSON literal "true" or "false") as an element of a JSON.
                @param[in] value The JSON-encoded boolean value to write.
            */
            inline auto WriteBoolean(bool boolean) const -> void
            {
                if (boolean)
                {
                    thiz.WriteSimpleValue(JsonConstants::TrueValue.data());
                    thiz._tokenType = TrueValue;
                }
                else
                {
                    thiz.WriteSimpleValue(JsonConstants::FalseValue.data());
                    thiz._tokenType = FalseValue;
                }
                return;
            }

            /*
                Returns the amount of strings written by the JsonWriter.
                That have been cleared to the base string.
            */
            inline auto ToString() const -> std::string
            {
                auto baseString = thiz._baseString;
                thiz.Clear();
                return baseString;
            }
        };

        // Unnecessary.
        /*
        struct JsonReader
        {
        private:
        public:
        };
        */
    }

    using JsonWriter = Sen::Kernel::Definition::UTF8Json::JsonWriter;
    // using JsonReader = Sen::Kernel::Definition::UTF8Json::JsonReader;

}
