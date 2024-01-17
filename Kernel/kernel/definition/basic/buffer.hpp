#pragma once

#include "kernel/definition/library.hpp"
#include "kernel/definition/macro.hpp"

namespace Sen::Kernel::Definition
{
    template <typename Type>
    concept CharacterOnView = std::is_same_v<Type, char> || std::is_same_v<Type, unsigned char>;
    namespace Buffer
    {
        template <bool use_big_endian>
        struct Stream
        {
        private:
            std::vector<std::uint8_t> mutable data;

            std::size_t mutable length;

            inline static auto constexpr buffer_size = static_cast<size_t>(8192);

            inline static auto constexpr close_file =  [](auto f){ if (f) fclose(f); };

        public:
            std::size_t mutable read_pos;

            std::size_t mutable write_pos;

            Stream() : read_pos(0), write_pos(0)
            {
            }

            Stream(
                const std::vector<std::uint8_t> &data) : data(std::move(data)), read_pos(0), write_pos(data.size()), length(data.size())
            {
                return;
            }

            Stream(
                std::string_view filepath
            ) : read_pos(0), write_pos(0)
            {
                auto file = std::unique_ptr<FILE, decltype(close_file)>(fopen(filepath.data(), "rb"), close_file);
                if (!file)
                {
                    throw Exception(fmt::format("{}: {}", Language::get("cannot_read_file") ,filepath));
                }
                fseek(file.get(), 0, SEEK_END);
                auto size = ftell(file.get());
                fseek(file.get(), 0, SEEK_SET);
                thiz.reserve((size_t)size + thiz.buffer_size);
                fread(thiz.data.data(), 1, size, file.get());
                thiz.length = size;
                thiz.write_pos = size;

            }

            Stream(
                const std::size_t &length) : read_pos(0), write_pos(length), length(length)
            {
                thiz.reserve(length + thiz.buffer_size);
                return;
            }

            ~Stream()
            {
                thiz.close();
            }

            inline auto fromString(
                const std::string &it) const -> void
            {
                thiz.close();
                thiz.data = std::vector<unsigned char>(it.begin(), it.end());
                thiz.length = thiz.data.size();
                thiz.write_pos = thiz.length;
                return;
            }

            inline auto get_length(

            ) const -> uint64_t
            {
                return thiz.length;
            }

            inline auto size() const -> uint64_t
            {
                return thiz.length;
            }

            inline auto capacity(

            ) const -> uint64_t
            {
                return thiz.data.size();
            }

            inline auto reserve(
                const std::size_t &capacity) const -> void
            {
                thiz.data.resize(capacity);
                return;
            }

            inline auto toBytes(

            ) const -> std::vector<std::uint8_t>
            {
                return thiz.get();
            }

            inline auto get(

            ) const -> const std::vector<std::uint8_t> &
            {
                return std::vector<unsigned char>(thiz.data.begin(), thiz.data.begin() + thiz.length);
            }

            inline auto get(
                const size_t &from,
                const size_t &to

            ) const -> std::vector<uint8_t>
            {
                if (from < 0 || to > thiz.data.size())
                {
                    throw Exception("Invalid vector size");
                }
                return std::vector<unsigned char>(thiz.data.begin() + from, thiz.data.begin() + to);
            }

            inline auto get_read_pos() const -> std::size_t
            {
                return thiz.read_pos;
            }

            inline auto get_write_pos() const -> std::size_t
            {
                return thiz.write_pos;
            }

            inline auto change_read_pos(std::size_t pos) const -> void
            {
                thiz.read_pos = pos;
                return;
            }

            inline auto change_write_pos(std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                return;
            }

            inline auto toString(

                ) -> std::string
            {
                std::stringstream ss;
                auto bytes = std::unique_ptr<uint8_t>(thiz.data.data());
                ss << bytes;
                return ss.str();
            }

            // insert to end of vector, write_pos will set to end.

            template <typename T>
                requires CharacterOnView<T>
            inline auto append(
                const std::vector<T> &m_data) const -> void
            {
                thiz.data.insert(thiz.data.begin() + thiz.length, m_data.begin(), m_data.end());
                thiz.length += m_data.size();
                thiz.write_pos = thiz.length;
                return;
            }

            template <typename T, size_t n>
                requires CharacterOnView<T>
            inline auto append(
                const std::array<T, n> &m_data) const -> void
            {
                thiz.data.insert(thiz.data.begin() + thiz.length, m_data.begin(), m_data.end());
                thiz.length += m_data.size();
                thiz.write_pos = thiz.length;
                return;
            }

            inline auto out_file(
                std::string_view path
            ) const -> void
            {
                {
                    auto filePath = std::filesystem::path(path);
                    if (filePath.has_parent_path())
                    {
                        std::filesystem::create_directories(filePath.parent_path());
                    }
                }
                auto file = std::unique_ptr<FILE, decltype(close_file)>(fopen(path.data(), "wb"), close_file);
                if (!file) {
                    throw Exception(fmt::format("Could not write file: {}", path));
                }
                fwrite(thiz.data.data(), 1, thiz.length, file.get());
                return;
            }

            inline auto writeUint8(
                std::uint8_t value) const -> void
            {
                this->template write_LE<std::uint8_t>(value);
                return;
            }

            inline auto writeUint8(
                std::uint8_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeUint8(value);
                return;
            }

            inline auto writeUint16(
                std::uint16_t value) const -> void
            {
                if constexpr (use_big_endian)
                {
                    this->template write_BE<std::uint16_t>(value);
                }
                else
                {
                    this->template write_LE<std::uint16_t>(value);
                }
                return;
            }

            inline auto writeUint16(
                std::uint16_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeUint16(value);
                return;
            }

            inline auto writeUint24(
                std::uint32_t value) const -> void
            {
                auto size = 3;
                if constexpr (use_big_endian)
                {
                    for (auto i : Range(size))
                    {
                        thiz.writeUint8((value >> ((size - 1 - i) * 8)) & 0xFF);
                    }
                }
                else
                {
                    for (auto i : Range(size))
                    {
                        thiz.writeUint8((value >> (i * 8)) & 0xFF);
                    }
                }
                return;
            }

            inline auto writeUint24(
                std::uint32_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeUint24(value);
                return;
            }

            inline auto writeUint32(
                std::uint32_t value) const -> void
            {
                if constexpr (use_big_endian)
                {
                    this->template write_BE<std::uint32_t>(value);
                }
                else
                {
                    this->template write_LE<std::uint32_t>(value);
                }
                return;
            }

            inline auto writeUint32(
                std::uint32_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeUint32(value);
                return;
            }

            inline auto writeUint64(
                std::uint64_t value) const -> void
            {
                if constexpr (use_big_endian)
                {
                    this->template write_BE<std::uint64_t>(value);
                }
                else
                {
                    this->template write_LE<std::uint64_t>(value);
                }
                return;
            }

            inline auto writeUint64(
                std::uint64_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeUint64(value);
                return;
            }

            inline auto writeInt8(
                std::int8_t value) const -> void
            {
                this->template write_LE<std::int8_t>(value);
                return;
            }

            inline auto writeInt8(
                std::int8_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeInt8(value);
                return;
            }

            inline auto writeInt16(
                std::int16_t value) const -> void
            {
                if constexpr (use_big_endian)
                {
                    this->template write_BE<std::int16_t>(value);
                }
                else
                {
                    this->template write_LE<std::int16_t>(value);
                }
                return;
            }

            inline auto writeInt16(
                std::int16_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeInt16(value);
                return;
            }

            inline auto writeInt24(
                std::int32_t value) const -> void
            {
                auto size = 3;
                if constexpr (use_big_endian)
                {
                    for (auto i : Range(size))
                    {
                        thiz.writeInt8((value >> ((size - 1 - i) * 8)) & 0xFF);
                    }
                }
                else
                {
                    for (auto i : Range(size))
                    {
                        thiz.writeInt8((value >> (i * 8)) & 0xFF);
                    }
                }
                return;
            }

            inline auto writeInt24(
                std::int32_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeInt24(value);
                return;
            }

            inline auto writeInt32(
                std::int32_t value) const -> void
            {
                if constexpr (use_big_endian)
                {
                    this->template write_BE<std::int32_t>(value);
                }
                else
                {
                    this->template write_LE<std::int32_t>(value);
                }
                return;
            }

            inline auto writeInt32(
                std::int32_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeInt32(value);
                return;
            }

            inline auto writeInt64(
                std::int64_t value) const -> void
            {
                if constexpr (use_big_endian)
                {
                    this->template write_BE<std::int64_t>(value);
                }
                else
                {
                    this->template write_LE<std::int64_t>(value);
                }
                return;
            }

            inline auto writeInt64(
                std::int64_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeInt64(value);
                return;
            }

            inline auto writeBytes(
                const std::vector<std::uint8_t> &inputBytes) const -> void
            {
                auto bytes = inputBytes;
                auto new_pos = thiz.write_pos + bytes.size();
                if (new_pos > thiz.capacity())
                {
                    thiz.reserve(new_pos + thiz.buffer_size);
                }
                if (new_pos > thiz.length)
                {
                    thiz.length = new_pos;
                }
                if constexpr (use_big_endian)
                {
                    std::reverse(bytes.begin(), bytes.end());
                }
                std::move(bytes.begin(), bytes.end(), thiz.data.begin() + thiz.write_pos);
                thiz.write_pos = new_pos;
                return;
            }

            inline auto writeBytes(
                const std::vector<std::uint8_t> &bytes,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeBytes(bytes);
                return;
            }

            template <class T>
            inline static auto set_raw_data(const T &val) -> std::vector<uint8_t>
            {
                auto res = std::vector<uint8_t> (sizeof(T));
                *(reinterpret_cast<T *>(res.data())) = val;
                return res;
            }

            inline auto writeFloat(
                float value) const -> void
            {
                auto res = set_raw_data(value);
                thiz.writeBytes(res);
                return;
            }

            inline auto writeFloat(
                float value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeFloat(value);
                return;
            }

            inline auto writeDouble(
                double value) const -> void
            {
                auto res = set_raw_data(value);
                thiz.writeBytes(res);
                return;
            }

            inline auto writeDouble(
                double value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeDouble(value);
                return;
            }

            inline auto writeChar(
                char value) const -> void
            {
                thiz.writeUint8(static_cast<uint8_t>(value));
                return;
            }

            inline auto writeChar(
                char value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeUint8(static_cast<uint8_t>(value));
                return;
            }

            inline auto writeVarInt32(
                std::int32_t value) const -> void
            {
                auto num = 0;
                for (num = (uint32_t)value; num >= 128; num >>= 7)
                {
                    thiz.writeUint8((uint8_t)(num | 0x80));
                }
                thiz.writeUint8((uint8_t)num);
                return;
            }

            inline auto writeVarInt32(
                std::int32_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeVarInt32(value);
                return;
            }

            inline auto writeVarInt64(
                std::int64_t value) const -> void
            {
                auto num = 0;
                for (num = (uint64_t)value; num >= 128; num >>= 7)
                {
                    thiz.writeUint8((uint8_t)(num | 0x80));
                }
                thiz.writeUint8((uint8_t)num);
                return;
            }

            inline auto writeVarInt64(
                std::int64_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeVarInt64(value);
                return;
            }

            inline auto writeZigZag32(
                std::int32_t value) const -> void
            {
                auto zigzag_num = (std::uint32_t)((value << 1) ^ (value >> 31));
                thiz.writeVarInt32(zigzag_num);
                return;
            }

            inline auto writeZigZag32(
                std::int32_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeZigZag32(value);
                return;
            }

            inline auto writeZigZag64(
                std::int64_t value) const -> void
            {
                auto zigzag_num = (std::uint64_t)((value << 1) ^ (value >> 31));
                thiz.writeVarInt64(zigzag_num);
                return;
            }

            inline auto writeZigZag64(
                std::int64_t value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeZigZag64(value);
                return;
            }

            inline auto writeStringView(
                 std::string_view str
            ) const -> void
            {
                auto new_pos = thiz.write_pos + str.size();
                if (new_pos > thiz.capacity())
                {
                    thiz.reserve(new_pos + thiz.buffer_size);
                }
                if (new_pos > thiz.length)
                {
                    thiz.length = new_pos;
                }
                for (auto &c : str)
                {
                    thiz.data[thiz.write_pos++] = (uint8_t)c;
                }
                return;
            }

            inline auto writeStringView(
                std::string_view str,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeString(str);
                return;
            }

            inline auto writeString(
                const std::string &str) const -> void
            {
                auto new_pos = thiz.write_pos + str.size();
                if (new_pos > thiz.capacity())
                {
                    thiz.reserve(new_pos + thiz.buffer_size);
                }
                if (new_pos > thiz.length)
                {
                    thiz.length = new_pos;
                }
                for (auto &c : str)
                {
                    thiz.data[thiz.write_pos++] = (uint8_t)c;
                }
                return;
            }

            inline auto writeString(
                const std::string &str,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeString(str);
                return;
            }

            inline auto writeStringFourByte(
                const std::string &str) const -> void
            {
                auto new_pos = thiz.write_pos + str.size() * 4;
                if (new_pos > thiz.capacity())
                {
                    thiz.reserve(new_pos + thiz.buffer_size);
                }
                for (auto &c : str)
                {
                    thiz.data[thiz.write_pos++] = (uint8_t)c;
                    thiz.data[thiz.write_pos++] = 0;
                    thiz.data[thiz.write_pos++] = 0;
                    thiz.data[thiz.write_pos++] = 0;
                }
                return;
            }

            inline auto writeStringFourByte(
                const std::string &str,
                std::size_t pos)
            {
                thiz.write_pos = pos;
                thiz.writeStringFourByte(str);
                return;
            }

            inline auto writeNull(
                std::size_t size) const -> void
            {
                if (size == 0)
                {
                    return;
                }
                auto new_pos = thiz.write_pos + size;
                if (new_pos > thiz.capacity())
                {
                    thiz.reserve(new_pos + thiz.buffer_size);
                }
                return;
            }

            inline auto writeNull(
                std::size_t size,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeNull(size);
                return;
            }

            inline auto writeCharByInt16(
                char value) const -> void
            {
                thiz.writeInt16(static_cast<int16_t>(value));
                return;
            }

            inline auto writeCharByInt16(
                char value,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeCharByInt16(value);
                return;
            }

            inline auto writeStringByUint8(
                const std::string &str) const -> void
            {
                if (str.empty())
                {
                    thiz.writeUint8(0);
                    return;
                }
                thiz.writeUint8(str.size());
                thiz.writeString(str);
                return;
            }

            inline auto writeStringByUint8(
                const std::string &str,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeStringByUint8(str);
                return;
            }

            inline auto writeStringByUint16(
                const std::string &str) const -> void
            {
                if (str.empty())
                {
                    thiz.writeUint16(0);
                    return;
                }
                thiz.writeUint16(str.size());
                thiz.writeString(str);
                return;
            }

            inline auto writeStringByUint16(
                const std::string &str,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeStringByUint16(str);
                return;
            }

            inline auto writeStringByUint32(
                const std::string &str) const -> void
            {
                if (str.empty())
                {
                    thiz.writeUint32(0);
                    return;
                }
                thiz.writeUint32(str.size());
                thiz.writeString(str);
                return;
            }

            inline auto writeStringByUint32(
                const std::string &str,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeStringByUint32(str);
                return;
            }

            inline auto writeStringByInt8(
                const std::string &str) const -> void
            {
                if (str.empty())
                {
                    thiz.writeInt8(0);
                    return;
                }
                thiz.writeInt8(str.size());
                thiz.writeString(str);
                return;
            }

            inline auto writeStringByInt8(
                const std::string &str,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeStringByInt8(str);
                return;
            }

            inline auto writeStringByInt16(
                const std::string &str) const -> void
            {
                if (str.empty())
                {
                    thiz.writeInt16(0);
                    return;
                }
                thiz.writeInt16(str.size());
                thiz.writeString(str);
                return;
            }

            inline auto writeStringByInt16(
                const std::string &str,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeStringByInt16(str);
                return;
            }

            inline auto writeStringByInt32(
                const std::string &str) const -> void
            {
                if (str.empty())
                {
                    thiz.writeInt32(0);
                    return;
                }
                thiz.writeInt32(str.size());
                thiz.writeString(str);
                return;
            }

            inline auto writeStringByInt32(
                const std::string &str,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeStringByInt32(str);
                return;
            }

            inline auto writeStringViewByVarInt32(
                std::string_view str
            ) const -> void
            {
                if (str.empty())
                {
                    thiz.writeVarInt32(0);
                    return;
                }
                thiz.writeVarInt32(str.size());
                thiz.writeStringView(str);
                return;
            }

            inline auto writeStringViewByVarInt32(
                std::string_view str,
                std::size_t pos
            ) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeStringByVarInt32(str);
                return;
            }

            inline auto writeStringByVarInt32(
                const std::string &str) const -> void
            {
                if (str.empty())
                {
                    thiz.writeVarInt32(0);
                    return;
                }
                thiz.writeVarInt32(str.size());
                thiz.writeString(str);
                return;
            }

            inline auto writeStringByVarInt32(
                const std::string &str,
                std::size_t pos) const -> void
            {
                thiz.write_pos = pos;
                thiz.writeStringByVarInt32(str);
                return;
            }

            inline auto writeStringByEmpty(
                const std::string &str) const -> void
            {
                if (str.empty())
                {
                    thiz.writeUint8(0);
                    return;
                }
                thiz.writeString(str);
                thiz.readUint8(0);
                return;
            }

            inline auto writeStringByEmpty(
                const std::string &str,
                std::size_t pos) const -> void
            {
                thiz.read_pos = pos;
                thiz.writeStringByEmpty(str);
                return;
            }

            inline auto writeBoolean(
                bool value) const -> void
            {
                if (value)
                {
                    thiz.writeUint8(0x01);
                }
                else
                {
                    thiz.writeUint8(0x00);
                }
                return;
            }

            inline auto operator[](
                size_t position) const -> uint8_t &
            {
                return this->data.at(position);
            }
            /*
            inline auto writeBytes_LE(
                std::uint8_t *bytes,
                std::size_t size) const -> void
            {
                thiz.data.insert(thiz.data.end(), bytes, bytes + size);
                thiz.write_pos += size;
                return;
            }

            inline auto writeBytes_BE(
                std::uint8_t *bytes,
                std::size_t size) const -> void
            {
                thiz.data.insert(thiz.data.end(), std::reverse_iterator(bytes + size), std::reverse_iterator(bytes));
                thiz.write_pos += size;
                return;
            }
            */

            template <typename T>
            inline auto write_LE(
                T value) const -> void
            {
                auto size = sizeof(T);
                auto new_pos = thiz.write_pos + size;
                // append capacity.
                if (new_pos > thiz.capacity())
                {
                    thiz.reserve(new_pos + thiz.buffer_size);
                }
                if (new_pos > thiz.length)
                {
                    thiz.length = new_pos;
                }
                for (auto i = 0; i < size; i++)
                {
                    thiz.data[thiz.write_pos++] = ((value >> (i * 8)) & 0xFF);
                }
                return;
            }

            template <typename T>
            inline auto write_BE(
                T value) const -> void
            {
                auto size = sizeof(T);
                auto new_pos = thiz.write_pos + size;
                // append capacity.
                if (new_pos > thiz.capacity())
                {
                    thiz.reserve(new_pos + thiz.buffer_size);
                }
                if (new_pos > thiz.length)
                {
                    thiz.length = new_pos;
                }
                for (auto i : Range(size))
                {
                    thiz.data[thiz.write_pos++] = ((value >> ((size - 1 - i) * 8)) & 0xFF);
                }
                return;
            }

            inline auto readUint8() const -> std::uint8_t
            {
                return this->template read<std::uint8_t>();
            }

            inline auto readUint8(std::size_t pos) const -> std::uint8_t
            {
                thiz.read_pos = pos;
                return thiz.readUint8();
            }

            inline auto readUint16() const -> std::uint16_t
            {
                if constexpr (use_big_endian)
                {
                    return reverseEndian(this->template read<std::uint16_t>());
                }
                else
                {
                    return this->template read<std::uint16_t>();
                }
            }

            inline auto readUint16(std::size_t pos) const -> std::uint16_t
            {
                thiz.read_pos = pos;
                return thiz.readUint16();
            }

            inline auto readUint24() const -> std::uint32_t
            {
                if constexpr (use_big_endian)
                {
                    return thiz.reverseEndian(this->template read_has<std::uint32_t>(3));
                }
                else
                {
                    return this->template read_has<std::uint32_t>(3);
                }
            }

            inline auto readUint24(std::size_t pos) const -> std::uint32_t
            {
                thiz.read_pos = pos;
                return thiz.readUint24();
            }

            inline auto readUint32() const -> std::uint32_t
            {
                if constexpr (use_big_endian)
                {
                    return reverseEndian(this->template read<std::uint32_t>());
                }
                else
                {
                    return this->template read<std::uint32_t>();
                }
            }

            inline auto readUint32(std::size_t pos) const -> std::uint32_t
            {
                thiz.read_pos = pos;
                return thiz.readUint32();
            }

            inline auto readUint64() const -> std::uint64_t
            {
                if constexpr (use_big_endian)
                {
                    return thiz.reverseEndian(this->template read<std::uint64_t>());
                }
                else
                {
                    return this->template read<std::uint64_t>();
                }
            }

            inline auto readUint64(std::size_t pos) const -> std::uint64_t
            {
                thiz.read_pos = pos;
                return thiz.readUint64();
            }

            inline auto readInt8() const -> std::int8_t
            {
                return this->template read<std::int8_t>();
            }

            inline auto readInt8(std::size_t pos) const -> std::int8_t
            {
                thiz.read_pos = pos;
                return thiz.readInt8();
            }

            inline auto readInt16() const -> std::int16_t
            {
                if constexpr (use_big_endian)
                {
                    return thiz.reverseEndian(this->template read<std::int16_t>());
                }
                else
                {
                    return this->template read<std::int16_t>();
                }
            }

            inline auto readInt16(std::size_t pos) const -> std::int16_t
            {
                thiz.read_pos = pos;
                return thiz.readInt16();
            }

            inline auto readInt24() const -> std::int32_t
            {
                if constexpr (use_big_endian)
                {
                    return thiz.reverseEndian(this->template read_has<std::int32_t>(3));
                }
                else
                {
                    return this->template read_has<std::int32_t>(3);
                }
            }

            inline auto readInt24(std::size_t pos) const -> std::int32_t
            {
                thiz.read_pos = pos;
                return thiz.readInt24();
            }

            inline auto readInt32() const -> std::int32_t
            {
                if constexpr (use_big_endian)
                {
                    return thiz.reverseEndian(this->template read<std::int32_t>());
                }
                else
                {
                    return this->template read<std::int32_t>();
                }
            }

            inline auto readInt32(std::size_t pos) const -> std::int32_t
            {
                thiz.read_pos = pos;
                return thiz.readInt32();
            }

            inline auto readInt64() const -> std::int64_t
            {
                if constexpr (use_big_endian)
                {
                    return thiz.reverseEndian(this->template read<std::int64_t>());
                }
                else
                {
                    return this->template read<std::int64_t>();
                }
            }

            inline auto readInt64(std::size_t pos) const -> std::int64_t
            {
                thiz.read_pos = pos;
                return thiz.readInt64();
            }

            inline auto readFloat() const -> float
            {
                return this->template read<float>();
            }

            inline auto readFloat(std::size_t pos) const -> float
            {
                thiz.read_pos = pos;
                return thiz.readFloat();
            }

            inline auto readDouble() const -> double
            {
                return this->template read<double>();
            }

            inline auto readDouble(std::size_t pos) const -> double
            {
                thiz.read_pos = pos;
                return thiz.readDouble();
            }

            inline auto readBoolean() const -> bool
            {
                return thiz.readUint8() == 0x01;
            }

            inline auto readBoolean(std::size_t pos) const -> bool
            {
                thiz.read_pos = pos;
                return thiz.readBoolean();
            }

            inline auto readString(std::size_t size) const -> std::string
            {
                auto str = std::string{};
                for (auto i : Range(size))
                {
                    str += ((char)thiz.readUint8());
                }
                return str;
            }

            inline auto readString(std::size_t size, std::size_t pos) const -> std::string
            {
                thiz.read_pos = pos;
                return thiz.readString(size);
            }

            inline auto readCharByInt16(

            ) const -> char
            {
                auto value = static_cast<char>(
                    thiz.readInt16());
                return value;
            }

            inline auto readCharByInt16(
                std::size_t pos) const -> char
            {
                thiz.read_pos = pos;
                return thiz.readCharByInt16();
            }

            inline auto readStringByUint8() const -> std::string
            {
                return thiz.readString(thiz.readUint8());
            }

            inline auto readStringByUint8(std::size_t pos) const -> std::string
            {
                thiz.read_pos = pos;
                return thiz.readStringByUint8();
            }

            inline auto readStringByUint16() const -> std::string
            {
                return thiz.readString(thiz.readUint16());
            }

            inline auto readStringByUint16(std::size_t pos) const -> std::string
            {
                thiz.read_pos = pos;
                return thiz.readStringByUint16();
            }

            inline auto readStringByUint32() const -> std::string
            {
                return thiz.readString(thiz.readUint32());
            }

            inline auto readStringByUint32(std::size_t pos) const -> std::string
            {
                thiz.read_pos = pos;
                return thiz.readStringByUint32();
            }

            inline auto readStringByInt8() const -> std::string
            {
                return thiz.readString(thiz.readInt8());
            }

            inline auto readStringByInt8(std::size_t pos) const -> std::string
            {
                thiz.read_pos = pos;
                return thiz.readStringByInt8();
            }

            inline auto readStringByInt16() const -> std::string
            {
                return thiz.readString(thiz.readInt16());
            }

            inline auto readStringByInt16(std::size_t pos) const -> std::string
            {
                thiz.read_pos = pos;
                return thiz.readStringByInt16();
            }

            inline auto readStringByInt32() const -> std::string
            {
                return thiz.readString(thiz.readInt32());
            }

            inline auto readStringByInt32(std::size_t pos) const -> std::string
            {
                thiz.read_pos = pos;
                return thiz.readStringByInt32();
            }

            inline auto readStringByVarInt32() const -> std::string
            {
                return thiz.readString(thiz.readVarInt32());
            }

            inline auto readStringByVarInt32(std::size_t pos) const -> std::string
            {
                thiz.read_pos = pos;
                return thiz.readStringByVarInt32();
            }

            inline auto readStringByEmpty() const -> std::string
            {
                auto ss = std::string{};
                auto byte = 0;
                while (true)
                {
                    if ((byte = thiz.readUint8()) == 0)
                    {
                        break;
                    }
                    ss += (char)byte;
                }
                return ss;
            }

            inline auto readStringByEmpty(std::size_t pos) const -> std::string
            {
                thiz.read_pos = pos;
                return thiz.readStringByEmpty();
            }

            inline auto getBytes(
                const size_t &from,
                const size_t &to) const -> std::vector<std::uint8_t>
            {
                auto bytes = std::vector<std::uint8_t>{};
                bytes.reserve(to - from);
                if (use_big_endian)
                {
                    for (auto i = to; i >= from; i++)
                    {
                        bytes.emplace_back(thiz.data.at(i));
                    }
                }
                else
                {
                    for (auto i = from; i <= to; i++)
                    {
                        bytes.emplace_back(thiz.data.at(i));
                    }
                }
                return bytes;
            }

            inline auto readBytes(std::size_t size) const -> std::vector<std::uint8_t>
            {
                std::vector<std::uint8_t> bytes;
                bytes.resize(size);
                if constexpr (use_big_endian)
                {

                    for (auto i = size - 1; i >= 0; i--)
                    {
                        bytes[i] = thiz.data[thiz.read_pos++];
                    }
                }
                else
                {
                    for (auto i : Range(size))
                    {
                        bytes[i] = thiz.data[thiz.read_pos++];
                    }
                }
                return bytes;
            }

            inline auto readBytes(std::size_t size, std::size_t pos) const -> std::vector<std::uint8_t>
            {
                thiz.read_pos = pos;
                return thiz.readBytes(size);
            }

            inline auto readVarInt32() const -> std::int32_t
            {
                auto num = 0;
                auto num_2 = 0;
                auto byte = 0;
                do
                {
                    if (num_2 == 35)
                    {
                        throw Exception("Invalid varint num");
                    }
                    byte = thiz.readUint8();
                    num |= ((byte & 0x7F) << num_2);
                    num_2 += 7;
                } while ((byte & 0x80) != 0);
                return num;
            }

            inline auto readVarInt32(std::size_t pos) const -> std::int32_t
            {
                thiz.read_pos = pos;
                return thiz.readVarInt32();
            }

            inline auto readVarInt64() const -> std::int64_t
            {
                auto num = 0;
                auto num_2 = 0;
                auto byte = 0;
                do
                {
                    if (num_2 == 70)
                    {
                        throw Exception("Invalid varint num");
                    }
                    byte = thiz.readUint8();
                    num |= ((byte & 0x7F) << num_2);
                    num_2 += 7;
                } while ((byte & 0x80) != 0);
                return num;
            }

            inline auto readVarInt64(std::size_t pos) const -> std::int64_t
            {
                thiz.read_pos = pos;
                return thiz.readVarInt64();
            }

            inline auto readVarUint32() const -> std::uint32_t
            {
                return (uint32_t)thiz.readVarInt32();
            }

            inline auto readVarUint32(std::size_t pos) const -> std::uint32_t
            {
                thiz.read_pos = pos;
                return (uint32_t)thiz.readVarInt32();
            }

            inline auto readVarUint64() const -> std::uint64_t
            {
                return (uint32_t)thiz.readVarInt64();
            }

            inline auto readVarUint64(std::size_t pos) const -> std::uint64_t
            {
                thiz.read_pos = pos;
                return (uint64_t)thiz.readVarUint64();
            }

            inline auto readZigZag32() const -> std::int32_t
            {
                auto zigzag_num = thiz.readVarInt32();
                auto decoded = (std::int32_t)((zigzag_num >> 1) ^ -(zigzag_num & 1));
                return decoded;
            }

            inline auto readZigZag32(std::size_t pos) const -> std::int32_t
            {
                thiz.read_pos + pos;
                return thiz.readZigZag32();
            }

            inline auto readZigZag64() const -> std::int64_t
            {
                auto zigzag_num = thiz.readVarInt64();
                auto decoded = (std::int64_t)((zigzag_num >> 1) ^ -(zigzag_num & 1));
                return decoded;
            }

            inline auto readZigZag64(std::size_t pos) const -> std::int64_t
            {
                thiz.read_pos + pos;
                return thiz.readZigZag64();
            }

            template <typename T>
                requires std::is_integral_v<T> || std::is_floating_point_v<T>
            inline auto static reverseEndian(T num) -> T
            {
                auto bytes = std::array<uint8_t, sizeof(T)>{};
                std::memcpy(bytes.data(), &num, sizeof(T));
                std::reverse(bytes.begin(), bytes.end());
                return std::bit_cast<T>(bytes);
            }

            template <typename T>
                requires std::is_integral_v<T> || std::is_floating_point_v<T>
            inline auto read(

            ) const -> T
            {
                if (thiz.read_pos + sizeof(T) > thiz.size())
                {
                    throw Exception(fmt::format("Offset {} is outside bounds of the DataStreamView size", thiz.read_pos, thiz.size()));
                }
                auto value = T{0};
                std::memcpy(&value, thiz.data.data() + thiz.read_pos, sizeof(T));
                thiz.read_pos += sizeof(T);
                return value;
            }

            template <typename T>
                requires std::is_integral_v<T> || std::is_floating_point_v<T>
            inline auto read_has(
                std::size_t size) const -> T
            {
                if (thiz.read_pos + size > thiz.size())
                {
                    throw Exception(fmt::format("Offset {} is outside bounds of the DataStreamView size", thiz.read_pos, thiz.size()));
                }
                T value = 0;
                std::memcpy(&value, thiz.data.data() + thiz.read_pos, size);
                this->read_pos += size;
                return;
            }

            inline auto close() const -> void
            {
                thiz.data.clear();
                thiz.read_pos = 0;
                thiz.write_pos = 0;
                return;
            }
        };
    }
    template <bool T>
    using SenBuffer = Sen::Kernel::Definition::Buffer::Stream<T>;

    using DataStreamView = SenBuffer<false>;

    using DataStreamViewBigEndian = SenBuffer<true>;
}
