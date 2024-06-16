#pragma once

#include "kernel/definition/utility.hpp"
#include "kernel/support/popcap/reflection_object_notation/decode.hpp"
#include "kernel/support/popcap/reflection_object_notation/encode.hpp"

namespace Sen::Kernel::Support::PopCap::ReflectionObjectNotation
{
    using namespace Definition;

    struct Instance
    {
    private:
            using Rijndael = Sen::Kernel::Definition::Encryption::Rijndael;
    public:
        inline static auto decrypt(
            DataStreamView & source,
            DataStreamView & destination,
            std::string_view key,
            std::string_view iv
        ) -> void
        {
            destination.writeBytes(
                Rijndael::decrypt<std::uint64_t, Rijndael::Mode::CBC>(
                    reinterpret_cast<char*>(source.getBytes(2, source.size()).data()),
                    key, 
                    iv, 
                    source.size() - 2_size
                )
            );
            return;
        }

        inline static auto decrypt_fs(
            std::string_view source,
            std::string_view destination,
            std::string_view key,
            std::string_view iv) -> void
        {
            auto stream = DataStreamView{source};
            auto result = DataStreamView{};
            decrypt(stream, result, key, iv);
            result.out_file(destination);
            return;
        }

        inline static auto decrypt_and_decode_fs(
            std::string_view source,
            std::string_view destination,
            std::string_view key,
            std::string_view iv) -> void
        {
            auto stream = DataStreamView{source};
            auto dest = DataStreamView{};
            decrypt(stream, dest, key, iv);
            auto json = JsonWriter{};
            Decode::process_whole(dest, json);
            FileSystem::write_file(destination, json.ToString());
            return;
        }

        inline static auto decode_fs_as_multiple_threads(
            const std::vector<std::vector<std::string>> &paths) -> void
        {
            auto threads = std::vector<std::thread>{};
            auto file_mutexes = std::map<std::string, std::mutex>{};
            for (const auto & data : paths) {
                threads.emplace_back([=, &file_mutexes]() { 
                auto lock_source = std::lock_guard<std::mutex>(file_mutexes[data[0]]);
                auto lock_destination = std::lock_guard<std::mutex>(file_mutexes[data[1]]);
                    Decode::process_fs(data[0], data[1]); 
                });
            }
            for (auto & thread : threads) {
                thread.join();
            }
            return;
        }

        // ---------------------------------------------

        inline static auto encrypt(
            DataStreamView &source_buffer,
            DataStreamView &destination,
            std::string_view key,
            std::string_view iv) -> void
        {
            fill_rijndael_block(source_buffer, iv);
            destination.writeUint8(0x10_byte);
            destination.writeUint8(0x00_byte);
            destination.writeBytes(
                Rijndael::encrypt<std::size_t, Rijndael::Mode::CBC>(
                    reinterpret_cast<char *>(source_buffer.getBytes(0, source_buffer.size()).data()),
                    key,
                    iv,
                    source_buffer.size()));
            return;
        }

        inline static auto encrypt_fs(
            std::string_view source,
            std::string_view destination,
            std::string_view key,
            std::string_view iv) -> void
        {
            auto source_buffer = DataStreamView{source};
            auto encrypted_data = DataStreamView{};
            encrypt(source_buffer, encrypted_data, key, iv);
            encrypted_data.out_file(destination);
            return;
        }

        inline static auto encode_and_encrypt_fs(
            std::string_view source,
            std::string_view destination,
            std::string_view key,
            std::string_view iv) -> void
        {
            auto result = DataStreamView{};
            Encode::process_whole(result, FileSystem::read_file(source));
            auto view = DataStreamView{};
            encrypt(result, view, key, iv);
            view.out_file(destination);
            return;
        }

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
                    Encode::process_fs(data[0], data[1]); });
            }
            for (auto &thread : threads)
            {
                thread.join();
            }
            return;
        }
    };

}