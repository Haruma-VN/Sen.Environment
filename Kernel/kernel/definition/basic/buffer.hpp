#pragma once

#include "kernel/definition/library.hpp"

namespace Sen::Kernel::Definition
{

    template <typename Type>
    concept CharacterOnView = std::is_same_v<Type, char> || std::is_same_v<Type, unsigned char>;

    namespace Buffer {

        struct Stream {

            private:
                std::vector<std::uint8_t> mutable data;

                std::size_t mutable position;

            public:
                Stream() : position(
                            0) {}

                ~Stream(

                )
                {
                    this->close();
                }

                Stream(
                    const std::vector<std::uint8_t> &data) : position(0)
                {
                    this->data = std::move(data);
                }

                Stream(
                    std::uint8_t *data,
                    std::size_t size) : position(0)
                {
                    this->data.assign(data, data + size);
                }

                Stream(
                    const std::string & filepath
                ) : position(0)
                {
                    auto file = std::ifstream(filepath, std::ios::binary);
                    if (!file)
                    {
                        throw std::runtime_error("Could not open file: " + filepath);
                    }
                    file.seekg(0, std::ios::end);
                    auto size = file.tellg();
                    file.seekg(0, std::ios::beg);
                    this->data.resize(size);
                    file.read(reinterpret_cast<char *>(this->data.data()), size);
                }

                inline auto fill(
                    size_t count,
                    uint8_t value = 0x00
                ) const -> void
                {
                    this->data = std::vector<uint8_t>(count, value);
                    return;
                }

                inline auto reserve(
                    size_t capacity
                ) const -> void
                {
                    this->data.reserve(capacity);
                    return;
                }

                inline auto capacity(
                    size_t range
                ) const -> size_t
                {
                    return this->data.capacity();
                }

                inline auto begin(

                ) -> std::vector<std::uint8_t>::iterator
                {
                    return this->data.begin();
                }

                inline auto get(

                ) const -> const std::vector<std::uint8_t> &
                {
                    return this->data;
                }

                inline auto get_position(

                ) const -> size_t
                {
                    return this->position;
                }

                inline auto writeUint8(
                    std::uint8_t value
                ) const -> void
                {
                    this->writeLE<std::uint8_t>(value);
                    return;
                }

                inline auto static reverseEndian(uint32_t num) -> uint32_t
                {
                    auto bytes = std::bit_cast<std::array<uint8_t, 4>>(num);
                    std::reverse(bytes.begin(), bytes.end());
                    return std::bit_cast<uint32_t>(bytes);
                }

                inline auto change_position(
                    std::vector<std::uint8_t>::size_type pos
                ) const -> void
                {
                    if (pos <= data.capacity())
                    {
                        this->position = pos;
                    }
                    else {
                        throw std::runtime_error("Out of range");
                    }
                }


                inline auto appendPosition(
                    std::size_t pos
                ) const -> void 
                {
                    auto new_pos = this->position + pos;
                    change_position(new_pos);
                    return;
                }

                inline auto writeUint16LE(
                    std::uint16_t value
                ) const -> void
                {
                    this->writeLE<std::uint16_t>(value);
                    return;
                }

                inline auto writeUint32LE(
                    std::uint32_t value
                ) const -> void
                {
                    this->writeLE<std::uint32_t>(value);
                    return;
                }

                inline auto writeUint64LE(
                    std::uint64_t value
                ) const -> void
                {
                    this->writeLE<std::uint64_t>(value);
                    return;
                }

                inline auto out_file(
                    const std::string &path
                ) const -> void
                {
                    auto filePath = std::filesystem::path(path);
                    if (filePath.has_parent_path())
                    {
                        std::filesystem::create_directories(filePath.parent_path());
                    }
                    auto file = std::ofstream(path, std::ios::binary);
                    if (file)
                    {
                        file.write(reinterpret_cast<const char *>(this->data.data()), this->size());
                    }
                    else
                    {
                        throw std::runtime_error(fmt::format("Could not open file: {}", path));
                    }
                    return;
                }

                inline auto writeInt8(
                    std::int8_t value
                ) const -> void
                {
                    this->writeLE<std::int8_t>(value);
                    return;
                }

                inline auto writeInt16LE(
                    std::int16_t value
                ) const -> void
                {
                    this->writeLE<std::int16_t>(value);
                    return;
                }

                inline auto writeInt32LE(
                    std::int32_t value
                ) const -> void
                {
                    this->writeLE<std::int32_t>(value);
                    return;
                }

                inline auto writeInt64LE(
                    std::int64_t value
                ) const -> void
                {
                    this->writeLE<std::int64_t>(value);
                    return;
                }

                inline auto writeUint16BE(
                    std::uint16_t value
                ) const -> void
                {
                    this->writeBE<std::uint16_t>(value);
                    return;
                }

                inline auto writeUint32BE(
                    std::uint32_t value
                ) const -> void
                {
                    this->writeBE<std::uint32_t>(value);
                    return;
                }

                inline auto writeUint64BE(
                    std::uint32_t value
                ) const -> void
                {
                    this->writeBE<std::uint64_t>(value);
                    return;
                }

                inline auto writeInt16BE(
                    std::int16_t value
                ) const -> void
                {
                    this->writeBE<std::int16_t>(value);
                    return;
                }

                inline auto writeInt32BE(
                    std::int32_t value
                ) const -> void
                {
                    this->writeBE<std::int32_t>(value);
                    return;
                }

                inline auto writeInt64BE(
                    std::int64_t value
                ) const -> void
                {
                    this->writeBE<std::int64_t>(value);
                    return;
                }

                inline auto size(

                ) const -> std::size_t
                {
                    return this->data.size();
                }

                inline auto toBytes(

                ) const -> const std::uint8_t *
                {
                    return this->data.data();
                }

                inline auto readUint8(

                ) const -> std::uint8_t
                {
                    return this->readLE<std::uint8_t>();
                }

                inline auto readUint16LE(

                ) const -> std::uint16_t
                {
                    return this->readLE<std::uint16_t>();
                }

                inline auto readUint32LE(

                ) const -> std::uint32_t
                {
                    return this->readLE<std::uint32_t>();
                }

                inline auto readUint64LE(

                ) const -> std::uint64_t
                {
                    return this->readLE<std::uint64_t>();
                }

                inline auto readInt8(

                ) const -> std::int8_t
                {
                    return this->readLE<std::int8_t>();
                }

                inline auto readInt16LE(

                ) const -> std::int16_t
                {
                    return this->readLE<std::int16_t>();
                }

                inline auto readInt32LE(

                ) const -> std::int32_t
                {
                    return this->readLE<std::int32_t>();
                }

                inline auto readInt64LE(

                ) const -> std::int64_t
                {
                    return this->readLE<std::int64_t>();
                }

                inline auto readUint16BE(

                ) const -> std::uint16_t
                {
                    return this->readBE<std::uint16_t>();
                }

                inline auto readUint32BE(

                ) const -> std::uint32_t
                {
                    return this->readBE<std::uint32_t>();
                }

                inline auto readUint64BE(

                ) const -> std::uint64_t
                {
                    return this->readBE<std::uint64_t>();
                }

                inline auto readInt16BE(

                ) const -> std::int16_t
                {
                    return this->readBE<std::int16_t>();
                }

                inline auto readInt32BE(

                ) const -> std::int32_t
                {
                    return this->readBE<std::int32_t>();
                }

                inline auto readInt64BE(

                ) const -> std::int64_t
                {
                    return this->readBE<std::int64_t>();
                }
                
                inline auto readBoolean(

                ) const -> bool
                {
                    return this->readInt8() == 0x01;
                }

                inline auto readString(
                    std::size_t size
                ) const -> std::string
                {
                    auto c = std::string{};
                    for (auto i = 0; i < size; ++i)
                    {
                        c.push_back((char)this->readInt8());
                    }
                    return c;
                }

                inline auto writeString(
                    const std::string & str
                ) const -> void
                {
                    for (auto & c : str)
                    {
                        this->writeInt8((int)c);
                    }
                    return;
                }

                inline auto current_pointer(

                ) -> std::vector<std::uint8_t>::iterator
                {
                    return this->data.begin() + this->position;
                }

                inline auto getBytes(
                    const size_t & from,
                    const size_t & to
                ) const -> uint8_t *
                {
                    auto c = std::vector<uint8_t>{};
                    for (auto i = from; i <= to; i++)
                    {
                        c.push_back(this->data.at(i));
                    }
                    return c.data();
                }

                inline auto get(
                    const size_t & from,
                    const size_t & to

                ) const -> std::vector<uint8_t>
                {
                    if (from < 0 || to > this->data.size())
                    {
                        throw std::runtime_error("Invalid vector size");
                    }
                    return std::vector<unsigned char>(this->data.begin() + from, this->data.begin() + to);
                }

                inline auto cut_buffer(
                    size_t from,
                    size_t to
                ) const -> Stream
                {
                    return Stream{this->get(from, to)};
                }

                inline auto writeBoolean(
                    bool val
                ) const -> void
                {
                    if (val)
                    {
                        this->writeUint8(0x01);
                    }
                    else
                    {
                        this->writeUint8(0x00);
                    }
                    return;
                }

                inline auto writeUint24LE(
                    std::uint32_t value
                ) const -> void
                {
                    this->writeLE_has<std::uint32_t>(value, 3);
                    return;
                }

                inline auto writeInt24LE(
                    std::int32_t value
                ) const -> void
                {
                    this->writeLE_has<std::int32_t>(value, 3);
                    return;
                }

                inline auto writeUint24BE(
                    std::uint32_t value
                ) const -> void
                {
                    this->writeBE_has<std::uint32_t>(value, 3);
                    return;
                }

                inline auto writeInt24BE(
                    std::int32_t value
                ) const -> void
                {
                    this->writeBE_has<std::int32_t>(value, 3);
                    return;
                }

                inline auto readUint24LE(

                ) const -> std::uint32_t
                {
                    return this->readLE_has<std::uint32_t>(3);
                }

                inline auto readInt24LE(

                ) const -> std::int32_t
                {
                    return this->readLE_has<std::int32_t>(3);
                }

                inline auto readUint24BE(

                ) const -> std::uint32_t
                {
                    return this->readBE_has<std::uint32_t>(3);
                }

                inline auto readInt24BE(

                ) const -> std::int32_t
                {
                    return this->readBE_has<std::int32_t>(3);
                }

                inline auto flush(

                ) const -> void
                {
                    this->position = 0;
                    return;
                }

                inline auto close(

                ) const -> void
                {
                    this->data.clear();
                    this->position = 0;
                    return;
                }

                inline auto writeFloatLE(
                    float value
                ) const -> void
                {
                    this->writeBytesLE(
                        reinterpret_cast<std::uint8_t *>(&value),
                        sizeof(float));
                    return;
                }

                inline auto writeDoubleLE(
                    double value
                ) const -> void
                {
                    this->writeBytesLE(
                        reinterpret_cast<std::uint8_t *>(&value),
                        sizeof(double));
                    return;
                }

                inline auto writeFloatBE(
                    float value
                ) const -> void
                {
                    this->writeBytesBE(
                        reinterpret_cast<std::uint8_t *>(&value),
                        sizeof(float));
                    return;
                }

                inline auto writeDoubleBE(
                    double value
                ) const -> void
                {
                    this->writeBytesBE(
                        reinterpret_cast<std::uint8_t *>(&value),
                        sizeof(double));
                    return;
                }

                inline auto readFloatLE(

                ) const -> float
                {
                    return this->readBytesLE<float>(
                        sizeof(float));
                }

                inline auto readDoubleLE(

                ) const -> double
                {
                    return this->readBytesLE<double>(
                        sizeof(double));
                }

                inline auto readFloatBE(

                ) const -> float
                {
                    return this->readBytesBE<float>(
                        sizeof(float));
                }

                inline auto readDoubleBE(

                ) const -> double
                {
                    return this->readBytesBE<double>(
                        sizeof(double));
                }

                inline auto writeChar(
                    char value
                ) const -> void
                {
                    this->data.push_back(
                        static_cast<std::uint8_t>(value));
                    return;
                }

                inline auto writeCharByUint8(
                    char value
                ) const -> void
                {
                    this->writeUint8(static_cast<uint8_t>(value));
                    return;
                }

                inline auto writeCharByUint16LE(
                    char value
                ) const -> void
                {
                    this->writeUint16LE(static_cast<uint16_t>(value));
                    return;
                }

                inline auto writeCharByUint16BE(
                    char value
                ) const -> void
                {
                    this->writeUint16BE(static_cast<uint16_t>(value));
                    return;
                }

                inline auto writeCharByUint32LE(
                    char value
                ) const -> void
                {
                    this->writeUint32LE(static_cast<uint32_t>(value));
                    return;
                }

                inline auto writeCharByUint32BE(
                    char value
                ) const -> void
                {
                    this->writeUint32BE(static_cast<uint32_t>(value));
                    return;
                }

                inline auto writeCharByUint64LE(
                    char value
                ) const -> void
                {
                    this->writeUint64LE(static_cast<uint64_t>(value));
                    return;
                }

                inline auto writeCharByUint64BE(
                    char value
                ) const -> void
                {
                    this->writeUint64BE(static_cast<uint64_t>(value));
                    return;
                }

                inline auto writeCharByInt8(
                    char value
                ) const -> void
                {
                    this->writeInt8(static_cast<int8_t>(value));
                    return;
                }

                inline auto writeCharByInt16LE(
                    char value
                ) const -> void
                {
                    this->writeInt16LE(static_cast<int16_t>(value));
                    return;
                }

                inline auto writeCharByInt16BE(
                    char value
                ) const -> void
                {
                    this->writeInt16BE(static_cast<int16_t>(value));
                    return;
                }

                inline auto writeCharByInt32LE(
                    char value
                ) const -> void
                {
                    this->writeInt32LE(static_cast<int32_t>(value));
                    return;
                }

                inline auto writeCharByInt32BE(
                    char value
                ) const -> void
                {
                    this->writeInt32BE(static_cast<int32_t>(value));
                    return;
                }

                inline auto writeCharByInt64LE(
                    char value
                ) const -> void
                {
                    this->writeInt64LE(static_cast<int64_t>(value));
                    return;
                }

                inline auto writeCharByInt64BE(
                    char value
                ) const -> void
                {
                    this->writeInt64BE(static_cast<int64_t>(value));
                    return;
                }

                inline auto readChar(

                ) const -> char
                {
                    auto value = static_cast<char>(
                        this->data[this->position]);
                    this->position++;
                    return value;
                }

                inline auto readCharByInt16LE(

                ) const -> char
                {
                    auto value = static_cast<char>(
                        this->readInt16LE());
                    return value;
                }

                inline auto readVarInt32(

                ) const -> std::int32_t
                {
                    auto num = 0;
                    auto num_2 = 0;
                    auto byte = 0;
                    do
                    {
                        if (num_2 == 35)
                            throw std::runtime_error("Invaild varint num");
                        byte = this->readUint8();
                        num |= (byte & 0x7F) << num_2;
                        num_2 += 7;
                    } while ((byte & 0x80) != 0);
                    return num;
                }

                inline auto writeVarInt32(
                    std::int32_t value
                ) const -> void
                {
                    auto num = 0;
                    for (num = (uint32_t)value; num >= 128; num >>= 7)
                    {
                        this->writeUint8((uint8_t)(num | 0x80));
                    }
                    this->writeUint8((uint8_t)num);
                    return;
                }

                inline auto writeVarInt64(
                    std::int32_t value
                ) const -> void
                {
                    auto num = 0;
                    for (num = (uint32_t)value; num >= 128; num >>= 7)
                    {
                        this->writeUint8((uint8_t)(num | 0x80));
                    }
                    this->writeUint8((uint8_t)num);
                    return;
                }

                inline auto readVarInt64(

                ) const -> std::int64_t
                {
                    auto num = 0;
                    auto num_2 = 0;
                    auto byte = 0;
                    do
                    {
                        if (num_2 == 70)
                            throw std::runtime_error("Invaild varint num");
                        byte = this->readUint8();
                        num |= ((int64_t)(byte & 0x7F)) << num_2;
                        num_2 += 7;
                    } while ((byte & 0x80) != 0);
                    return num;
                }

                inline auto readVarUInt32(

                ) const -> std::uint32_t
                {
                    return (uint32_t)this->readVarInt32();
                }

                inline auto readVarUInt64(

                ) const -> std::uint64_t
                {
                    return (uint64_t)this->readVarInt64();
                }

                inline auto writeZigZag32(
                    std::int32_t value
                ) const -> void
                {
                    auto zigzagEncoded = (std::uint32_t)((value << 1) ^ (value >> 31));
                    this->writeVarInt32(zigzagEncoded);
                    return;
                }

                inline auto readZigZag32(

                ) const -> std::int32_t
                {
                    auto zigzagEncoded = this->readVarUInt32();
                    auto decoded = (std::int32_t)((zigzagEncoded >> 1) ^ -(zigzagEncoded & 1));
                    return decoded;
                }

                inline auto writeZigZag64(
                    std::int64_t value
                ) const -> void
                {
                    auto zigzagEncoded = (std::uint64_t)((value << 1) ^ (value >> 63));
                    this->writeVarInt64(zigzagEncoded);
                    return;
                }

                inline auto readZigZag64(

                ) const -> std::int64_t
                {
                    auto zigzagEncoded = (std::uint64_t)this->readVarUInt64();
                    auto decoded = (std::int64_t)((zigzagEncoded >> 1) ^ -(zigzagEncoded & 1));
                    return decoded;
                }

                inline auto readStringByUInt8(

                ) const -> std::string
                {
                    return this->readString(this->readUint8());
                }

                inline auto readStringByInt8(

                ) const -> std::string
                {
                    return this->readString(this->readInt8());
                }

                inline auto readStringByUInt16LE(

                ) const -> std::string
                {
                    return this->readString(this->readUint16LE());
                }

                inline auto readStringByInt16LE(

                ) const -> std::string
                {
                    return this->readString(this->readInt16LE());
                }

                inline auto readStringByUInt16BE(

                ) const -> std::string
                {
                    return this->readString(this->readUint16BE());
                }

                inline auto readStringByInt16BE(

                ) const -> std::string
                {
                    return this->readString(this->readInt16BE());
                }

                inline auto readStringByUInt32LE(

                ) const -> std::string
                {
                    return this->readString(this->readUint32LE());
                }

                inline auto readStringByInt32LE(

                ) const -> std::string
                {
                    return this->readString(this->readInt32LE());
                }

                inline auto readStringByUInt32BE(

                ) const -> std::string
                {
                    return this->readString(this->readUint32BE());
                }

                inline auto readStringByInt32BE(

                ) const -> std::string
                {
                    return this->readString(this->readInt32BE());
                }

                inline auto readStringByVarInt32(

                ) const -> std::string
                {
                    return this->readString(this->readVarInt32());
                }

                inline auto readStringByEmpty() -> std::string
                {
                    std::string c = "";
                    auto b = 0;
                    while (true)
                    {
                        if ((b = this->readUint8()) == 0)
                        {
                            break;
                        }
                        c += (char)b;
                    }
                    return c;
                }

                inline auto writeStringByEmpty(
                    const std::string & str
                ) -> void
                {
                    if (str.empty())
                    {
                        this->writeUint8(0);
                        return;
                    }
                    this->writeString(str);
                    this->writeUint8(0);
                    return;
                }

                inline auto writeNull(
                    std::size_t size
                ) const -> void
                {
                    if (size < 0)
                        throw std::runtime_error("Invaild size");
                    if (size == 0)
                        return;
                    uint8_t *null_bytes = new uint8_t(size);
                    this->writeBytesLE(null_bytes, size);
                    return;
                }

                inline auto writeStringFourByte(
                    const std::string & str
                ) const -> void
                {
                    for (auto & c : str)
                    {
                        this->writeUint8((uint8_t)c);
                        this->writeUint24LE(0);
                    }
                    return;
                }

                inline auto writeStringByUInt8(
                    const std::string &str
                ) const -> void
                {
                    if (str.empty())
                    {
                        this->writeUint8(0);
                        return;
                    }
                    this->writeUint8(str.size());
                    this->writeString(str);
                    return;
                }

                inline auto writeStringByInt8(
                    const std::string & str
                ) const -> void
                {
                    if (str.empty())
                    {
                        this->writeInt8(0);
                        return;
                    }
                    this->writeInt8(str.size());
                    this->writeString(str);
                    return;
                }

                inline auto writeStringByUInt16LE(
                    const std::string & str
                ) const -> void
                {
                    if (str.empty())
                    {
                        this->writeUint16LE(0);
                        return;
                    }
                    this->writeUint16LE(str.size());
                    this->writeString(str);
                    return;
                }

                inline auto writeStringByUInt16BE(
                    const std::string & str
                ) const -> void
                {
                    if (str.empty())
                    {
                        this->writeUint16BE(0);
                        return;
                    }
                    this->writeUint16BE(str.size());
                    this->writeString(str);
                    return;
                }

                inline auto writeStringByInt16LE(
                    const std::string & str
                ) const -> void
                {
                    if (str.empty())
                    {
                        this->writeInt16LE(0);
                        return;
                    }
                    this->writeInt16LE(str.size());
                    this->writeString(str);
                    return;
                }

                inline auto writeStringByInt16BE(
                    const std::string & str
                ) const -> void
                {
                    if (str.empty())
                    {
                        this->writeInt16LE(0);
                        return;
                    }
                    this->writeInt16LE(str.size());
                    this->writeString(str);
                    return;
                }

                inline auto writeStringByUInt32LE(
                    const std::string & str
                ) const -> void
                {
                    if (str.empty())
                    {
                        this->writeUint32LE(0);
                        return;
                    }
                    this->writeUint32LE(str.size());
                    this->writeString(str);
                    return;
                }

                inline auto writeStringByUInt32BE(
                    const std::string & str
                ) const -> void
                {
                    if (str.empty())
                    {
                        this->writeUint32BE(0);
                        return;
                    }
                    this->writeUint32BE(str.size());
                    this->writeString(str);
                    return;
                }

                inline auto writeStringByInt32LE(
                    const std::string & str
                ) const -> void
                {
                    if (str.empty())
                    {
                        this->writeInt32LE(0);
                        return;
                    }
                    this->writeInt32LE(str.size());
                    this->writeString(str);
                    return;
                }

                inline auto writeStringByInt32BE(
                    const std::string &str
                ) -> void
                {
                    if (str.empty())
                    {
                        this->writeInt32BE(0);
                        return;
                    }
                    this->writeInt32BE(str.size());
                    this->writeString(str);
                    return;
                }

                inline auto writeStringByVarInt32(
                    const std::string & str
                ) -> void
                {
                    if (str.empty())
                    {
                        this->writeVarInt32(0);
                        return;
                    }
                    this->writeVarInt32(str.size());
                    this->writeString(str);
                    return;
                }

                inline auto peekUInt8(
                    std::size_t offset
                ) const -> std::uint8_t
                {
                    return this->data[offset];
                }

                inline auto peekUInt16(
                    std::size_t offset
                ) const -> std::uint16_t
                {
                    return (this->data[offset] | (this->data[offset + 1] << 8));
                }

                inline auto peekUInt24(
                    std::size_t offset
                ) const -> std::uint32_t
                {
                    return (this->data[offset] | (this->data[offset + 1] << 8) | (this->data[offset + 2] << 16));
                }

                inline auto peekUInt32(
                    std::size_t offset
                ) const -> std::uint32_t
                {
                    return (this->data[offset] | (this->data[offset + 1] << 8) | (this->data[offset + 2] << 16) | (this->data[offset + 3] << 24));
                }

                inline auto peekInt8(
                    std::size_t offset
                ) const -> std::int8_t
                {
                    return static_cast<std::int8_t>(this->data[offset]);
                }

                inline auto peekInt16(
                    std::size_t offset
                ) const -> std::int16_t
                {
                    return static_cast<std::int16_t>(this->data[offset] | (this->data[offset + 1] << 8));
                }

                inline auto peekInt24(
                    std::size_t offset
                ) const -> std::int32_t
                {
                    return static_cast<std::int32_t>(this->data[offset] | (this->data[offset + 1] << 8) | (this->data[offset + 2] << 16));
                }

                inline auto peekInt32(
                    std::size_t offset
                ) const -> std::int32_t
                {
                    return static_cast<std::int32_t>(this->data[offset] | (this->data[offset + 1] << 8) | (this->data[offset + 2] << 16) | (this->data[offset + 3] << 24));
                }

                inline auto peekString(
                    std::size_t offset,
                    std::size_t size
                ) const -> std::string
                {
                    auto str = std::string{};
                    str.reserve(size);
                    for (auto i = offset; i < offset + size; ++i)
                    {
                        str.push_back(static_cast<char>(this->data[i]));
                    }
                    return str;
                }

                inline auto peekChar(
                    std::size_t offset
                ) const -> char
                {
                    return static_cast<char>(this->data[offset]);
                }

                inline auto insertUInt16BE(
                    std::size_t offset,
                    std::uint16_t value
                ) const -> void
                {
                    this->data.insert(this->data.begin() + offset, (value >> 8) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 1, value & 0xFF);
                    return;
                }

                inline auto insertUInt32BE(
                    std::size_t offset,
                    std::uint32_t value
                ) const -> void
                {
                    this->data.insert(this->data.begin() + offset, (value >> 24) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 1, (value >> 16) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 2, (value >> 8) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 3, value & 0xFF);
                    return;
                }

                inline auto insertInt16BE(
                    std::size_t offset,
                    std::int16_t value
                ) const -> void
                {
                    this->data.insert(this->data.begin() + offset, (value >> 8) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 1, value & 0xFF);
                    return;
                }

                inline auto insertInt32BE(
                    std::size_t offset,
                    std::int32_t value
                ) const -> void
                {
                    this->data.insert(this->data.begin() + offset, (value >> 24) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 1, (value >> 16) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 2, (value >> 8) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 3, value & 0xFF);
                    return;
                }

                inline auto insertUInt16LE(
                    std::size_t offset,
                    std::uint16_t value
                ) const -> void
                {
                    this->data.insert(this->data.begin() + offset, value & 0xFF);
                    this->data.insert(this->data.begin() + offset + 1, (value >> 8) & 0xFF);
                    return;
                }

                inline auto insertUInt32LE(
                    std::size_t offset,
                    std::uint32_t value
                ) const -> void
                {
                    this->data.insert(this->data.begin() + offset, value & 0xFF);
                    this->data.insert(this->data.begin() + offset + 1, (value >> 8) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 2, (value >> 16) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 3, (value >> 24) & 0xFF);
                    return;
                }

                inline auto insertInt16LE(
                    std::size_t offset,
                    std::int16_t value
                ) const -> void
                {
                    this->data.insert(this->data.begin() + offset, value & 0xFF);
                    this->data.insert(this->data.begin() + offset + 1, (value >> 8) & 0xFF);
                    return;
                }

                inline auto insertInt32LE(
                    std::size_t offset,
                    std::int32_t value
                ) const -> void
                {
                    this->data.insert(this->data.begin() + offset, value & 0xFF);
                    this->data.insert(this->data.begin() + offset + 1, (value >> 8) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 2, (value >> 16) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 3, (value >> 24) & 0xFF);
                    return;
                }

                inline auto operator[](
                    size_t position
                ) const -> uint8_t &
                {
                    return this->data.at(position);
                }

                inline auto insertString(
                    std::size_t offset,
                    const std::string &str
                ) const -> void
                {
                    this->data.insert(
                        this->data.begin() + offset,
                        str.begin(),
                        str.end());
                    return;
                }

                inline auto insertChar(
                    std::size_t offset,
                    char c
                ) const -> void
                {
                    this->data.insert(
                        this->data.begin() + offset,
                        static_cast<std::uint8_t>(c));
                    return;
                }

                inline auto insertUInt24BE(
                    std::size_t offset,
                    std::uint32_t value
                ) const -> void
                {
                    this->data.insert(this->data.begin() + offset, (value >> 16) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 1, (value >> 8) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 2, value & 0xFF);
                    return;
                }

                inline auto insertInt24BE(
                    std::size_t offset,
                    std::int32_t value
                ) const -> void
                {
                    this->data.insert(this->data.begin() + offset, (value >> 16) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 1, (value >> 8) & 0xFF);
                    this->data.insert(this->data.begin() + offset + 2, value & 0xFF);
                    return;
                }

                inline static auto fromString(
                    const std::string & it
                ) -> Buffer::Stream
                {
                    return Buffer::Stream{std::vector<unsigned char>(it.begin(), it.end())};
                }

                inline auto toString(

                ) const -> std::string
                {
                    auto c = std::string{};
                    for (auto & p : this->data)
                    {
                        c.push_back((char)p);
                    }
                    return c;
                }

                template <typename T>
                    requires CharacterOnView<T>
                inline auto append(
                    const std::vector<T> & m_data
                ) const -> void
                {
                    this->data.insert(this->data.end(), m_data.begin(), m_data.end());
                    return;
                }

                template <typename T, size_t n>
                    requires CharacterOnView<T>
                inline auto append(
                    const std::array<T, n> & m_data
                ) const -> void
                {
                    this->data.insert(this->data.end(), m_data.begin(), m_data.end());
                    return;
                }

                inline auto move(
                    const std::vector<uint8_t> & data
                ) const -> void
                {
                    std::move(data.begin(), data.end(), this->data.end());
                    return;
                }

                template <typename T>
                inline auto writeLE(
                    T value
                ) const -> void
                {
                    auto size = sizeof(T);
                    this->data.reserve(this->data.size() + size);
                    for (auto i = 0; i < size; i++)
                    {
                        this->data.push_back((value >> (i * 8)) & 0xFF);
                    }
                    this->position += size;
                    return;
                }


                template <typename T>
                inline auto writeBE(
                    T value
                ) const -> void
                {
                    auto size = sizeof(T);
                    this->data.reserve(this->data.size() + size);
                    for (auto i = 0; i < size; i++)
                    {
                        this->data.push_back((value >> ((size - 1 - i) * 8)) & 0xFF);
                    }
                    this->position += size;
                    return;
                }

                template <typename T>
                inline auto readLE(

                ) const -> T
                {
                    if (this->position + sizeof(T) > this->size())
                    {
                        throw std::runtime_error(fmt::format("Offset {} is outside bounds of the DataStreamView size", this->position, this->size()));
                    }
                    auto value = T{0};
                    std::memcpy(&value, this->data.data() + this->position, sizeof(T));
                    this->position += sizeof(T);
                    return value;
                }

                template <typename T>
                inline auto readBE(
                    
                ) const -> T
                {
                    if (this->position + sizeof(T) > this->size())
                    {
                        throw std::runtime_error(fmt::format("Offset {} is outside bounds of the DataStreamView size", this->position, this->size()));
                    }
                    T value = 0;
                    std::memcpy(&value, this->data.data() + this->position, sizeof(T));
                    this->position += sizeof(T);
                    return reverseEndian(value);
                }


                template <typename T>
                inline auto writeLE_has(
                    T value,
                    std::size_t size = sizeof(T)) const -> void
                {
                    this->data.reserve(this->data.size() + size);
                    for (auto i = 0; i < size; i++)
                    {
                        this->data.push_back((value >> (i * 8)) & 0xFF);
                    }
                    this->position += size;
                    return;
                }

                template <typename T>
                inline auto writeBE_has(
                    T value,
                    std::size_t size = sizeof(T)) const -> void
                {
                    this->data.reserve(this->data.size() + size);
                    for (auto i = 0; i < size; i++)
                    {
                        this->data.push_back((value >> ((size - 1 - i) * 8)) & 0xFF);
                    }
                    this->position += size;
                    return;
                }

                template <typename T>
                inline auto readLE_has(
                    std::size_t size = sizeof(T)
                ) const -> T
                {
                    if (this->position + size > this->size())
                    {
                        throw std::runtime_error(fmt::format("Offset {} is outside bounds of the DataStreamView size", this->position, this->size()));
                    }
                    T value;
                    std::memcpy(&value, &this->data[this->position], sizeof(T));
                    this->position += sizeof(T);
                    return value;
                }

                template <typename T>
                inline auto readBE_has(
                    std::size_t size = sizeof(T)
                ) const -> T
                {
                    if (this->position + size > this->size())
                    {
                        throw std::runtime_error(fmt::format("Offset {} is outside bounds of the DataStreamView size", this->position, this->size()));
                    }
                    T value = 0;
                    for (auto i = 0; i < size; ++i)
                    {
                        value = (value << 8) | this->data[this->position + i];
                    }
                    this->position += size;
                    return value;
                }


                inline auto writeBytesLE(
                    std::uint8_t *bytes,
                    std::size_t size
                ) const -> void
                {
                    this->data.insert(this->data.end(), bytes, bytes + size);
                    this->position += size;
                    return;
                }

                inline auto writeBytesBE(
                    std::uint8_t *bytes,
                    std::size_t size
                ) const -> void
                {
                    this->data.insert(this->data.end(), std::reverse_iterator(bytes + size), std::reverse_iterator(bytes));
                    this->position += size;
                    return;
                }


                template <typename T>
                inline auto readBytesLE(
                    std::size_t size
                ) const -> T
                {
                    if (this->position + size > this->size())
                    {
                        throw std::runtime_error(fmt::format("Offset {} is outside bounds of the DataStreamView size", this->position, this->size()));
                    }
                    auto value = T{};
                    std::memcpy(&value, &this->data[this->position], size);
                    this->position += size;
                    return value;
                }

                template <typename T>
                inline auto readBytesBE(
                    std::size_t size
                ) const -> T
                {
                    if (this->position + size > this->size())
                    {
                        throw std::runtime_error(fmt::format("Offset {} is outside bounds of the DataStreamView size", this->position, this->size()));
                    }
                    auto value = T{};
                    for (auto i = 0; i < size; i++)
                    {
                        reinterpret_cast<std::uint8_t *>(&value)[size - 1 - i] = this->data[this->position + i];
                    }
                    this->position += size;
                    return value;
                }
        };
    }

    using SenBuffer = Sen::Kernel::Definition::Buffer::Stream;

    using DataStreamView = SenBuffer;
}