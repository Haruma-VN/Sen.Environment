#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition
{
    namespace UTF8Json
    {
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
            inline static auto backslash_pattern = std::regex("\\\\");
            inline static auto newline_pattern = std::regex("\n");
            inline static auto carriage_pattern = std::regex("\r");
            inline static auto tab_pattern = std::regex("\t");
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
        public:
            // Unnecessary.
            inline static auto Validate(std::string propertyName) -> void
            {
                return;
            }

            inline static auto EscapeString(std::string &value) -> void
            {
                /*
                std::string output;
                value.reserve(value.size());
                for (const char c : value)
                {
                    switch (c)
                    {
                    case '\a':
                        output += "\\a";
                        break;
                    case '\b':
                        output += "\\b";
                        break;
                    case '\f':
                        output += "\\f";
                        break;
                    case '\n':
                        output += "\\n";
                        break;
                    case '\r':
                        output += "\\r";
                        break;
                    case '\t':
                        output += "\\t";
                        break;
                    case '\v':
                        output += "\\v";
                        break;
                    default:
                        output += c;
                        break;
                    }
                }
                value = output;
                */
                // faster algorithm, but need hex convertion.
                if (value.find(JsonConstants::BackSlash) != std::string::npos)
                {
                    value = std::regex_replace(value, JsonConstants::backslash_pattern, "\\\\");
                }
                if (value.find(JsonConstants::LineFeed) != std::string::npos)
                {
                    value = std::regex_replace(value, JsonConstants::newline_pattern, "\\n");
                }
                if (value.find(JsonConstants::CarriageReturn) != std::string::npos)
                {
                    value = std::regex_replace(value, JsonConstants::carriage_pattern, "\\r");
                }
                if (value.find(JsonConstants::Tab) != std::string::npos)
                {
                    value = std::regex_replace(value, JsonConstants::tab_pattern, "\\t");
                }
                /*
                if (value.find(JsonConstants::FormFeed) != std::string::npos)
                {
                    std::regex regexPattern("\f");
                    value = std::regex_replace(value, regexPattern, "\\f");
                }
                */
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
            int mutable _currentDepth = 0;

            int mutable _indentLength = 1;

            inline auto WriteStart(std::string token) const -> void
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
                thiz._currentDepth++;
                return;
            }

            /*
                Writes the UTF-8 property name (as a JSON string) as the first part of a name/value pair of a JSON object.
            */

            inline auto WriteStringPropertyName(std::string propertyName) const -> void
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
                    thiz._baseString += propertyName;
                    thiz._baseString += JsonConstants::Quote;
                    thiz._baseString += JsonConstants::KeyValueSeparator;
                }
                thiz._currentDepth &= JsonConstants::RemoveFlagsBitMask;
                return;
            }

            inline auto WriteStringValue(std::string_view value) const -> void
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
                    thiz._baseString += value;
                    thiz._baseString += JsonConstants::Quote;
                }
                thiz.SetFlagToAddListSeparatorBeforeNextItem();
                return;
            }

            inline auto WriteSimpleIndented(std::string value) const -> void
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

            inline auto WriteSimpleValue(std::string value) const -> void
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

            inline auto WriteEnd(std::string token) const -> void
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
                    thiz._currentDepth--;
                }
                return;
            }

            inline auto WriteStringIndented(std::string_view value) const -> void
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
                thiz._baseString += value;
                thiz._baseString += JsonConstants::Quote;
                return;
            }

            inline auto WriteStringIndentedPropertyName(std::string_view propertyName) const -> void
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
                thiz._baseString += propertyName;
                thiz._baseString += JsonConstants::Quote;
                thiz._baseString += JsonConstants::KeyValueSeparator;
                thiz._baseString += JsonConstants::Space;
                return;
            }

            inline auto WriteEndIndented(std::string_view token) const -> void
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
            inline auto WritePropertyName(std::string propertyName) const -> void
            {
                // Throw an error if property name is null.
                if (propertyName.empty())
                {
                }
                JsonWriterHelper::EscapeString(propertyName);
                thiz.WriteStringPropertyName(propertyName);
                thiz._tokenType = PropertyName;
                return;
            }
            /*!
                Writes the property name (as a JSON string) as the first part of a name/value pair of a JSON object.
                @param[in] properyName The name of the property to write.
            */
            template <typename T>
            inline auto WritePropertyName(T propertyName) const -> void
            {
                auto name = std::string{"null"};
                if (!propertyName.empty())
                {
                    name = std::to_string(propertyName);
                }
                JsonWriterHelper::EscapeString(name);
                thiz.WriteStringPropertyName(name);
                thiz._tokenType = PropertyName;
                return;
            }
            /*!
                Writes the pre-encoded text value (as a JSON string) as an element of a JSON.
                @param[in] value The JSON-encoded value to write.
            */
            inline auto WriteValue(std::string value) const -> void
            {
                // Write Null
                if (value.empty())
                {
                    thiz.WriteNull();
                    return;
                }
                JsonWriterHelper::EscapeString(value);
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
