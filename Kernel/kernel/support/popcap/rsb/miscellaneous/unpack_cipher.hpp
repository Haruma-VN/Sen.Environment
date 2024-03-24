#pragma once
#include "kernel/support/popcap/rsb/unpack.hpp"

namespace Sen::Kernel::Support::PopCap::RSB::Miscellaneous {

	class UnpackCipher {

	private:
		template <typename T> requires std::is_same<T, char>::value or std::is_same<T, unsigned char>::value
		using List = std::vector<T>;

		using Byte = std::uint8_t;

		template <typename T> requires std::is_same<T, DataStreamView>::value
		using Pointer = std::unique_ptr<T>;

		using RSBHeadInfo = RSB_HeadInfo<std::uint32_t>;

		using Zlib = Definition::Compression::Zlib;

		using Position = std::size_t;

		using Size = std::size_t;

		using CompiledMap = std::map<int, std::string>;

		template <typename T, auto size>
		using Array = std::array<T, size>;

	private:

		List<Byte> keys;

		Pointer<DataStreamView> rsb_data;

		Pointer<DataStreamView> raw_data;

		Position atlas_position;

	protected:

		auto repair_zlib_data(
			DataStreamView* zlib_data
		) -> void
		{
			auto key_data = List<Byte>{};
			key_data.assign(keys.begin(), keys.end());
			std::for_each(key_data.begin(), key_data.end(), [](auto& e) {
				e -= 2;
			});
			zlib_data->write_pos = 0;
			auto key_index = 8;
			zlib_data->writeUint8(static_cast<std::uint8_t>(0x78));
			zlib_data->writeUint8(static_cast<std::uint8_t>(0xDA));
			for (auto index = 2; index != 4096; ++index) {
				zlib_data->writeUint8(zlib_data->readUint8(static_cast<std::uint64_t>(index))
					^ key_data[key_index], static_cast<std::uint64_t>(index));
				key_data[key_index] ^= index;
				key_index = (key_index + 5) % 48;
			}
			return;
		}

		auto read_head(
			RSBHeadInfo* rsb_head
		) -> void
		{
			rsb_data->read_pos = 16;
			rsb_head->file_list_length = rsb_data->readUint32();
			rsb_head->file_list_begin = rsb_data->readUint32();
			rsb_data->read_pos += 8;
			rsb_head->rsg_list_length = rsb_data->readUint32();
			rsb_head->rsg_list_begin = rsb_data->readUint32();
			rsb_head->rsg_number = rsb_data->readUint32();
			rsb_head->rsg_info_begin = rsb_data->readUint32();
			rsb_head->rsg_info_each_length = rsb_data->readUint32();
			rsb_head->composite_number = rsb_data->readUint32();
			rsb_head->composite_info_begin = rsb_data->readUint32();
			rsb_head->composite_info_each_length = rsb_data->readUint32();
			rsb_head->composite_list_length = rsb_data->readUint32();
			rsb_head->composite_list_begin = rsb_data->readUint32();
			rsb_head->autopool_number = rsb_data->readUint32();
			rsb_head->autopool_info_begin = rsb_data->readUint32();
			rsb_head->autopool_info_each_length = rsb_data->readUint32();
			rsb_head->ptx_number = rsb_data->readUint32();
			rsb_head->ptx_info_begin = rsb_data->readUint32();
			rsb_head->ptx_info_each_length = rsb_data->readUint32();
			rsb_data->read_pos += 12;
			rsb_head->file_offset = rsb_data->readUint32();
			return;
		}

		auto read_rsg_info(
			std::uint32_t rsg_info_pos, 
			std::string_view rsg_name,
			std::string_view destination
		) -> void
		{
			rsb_data->read_pos = static_cast<std::size_t>(rsg_info_pos) + 0x80;
			auto packet_pos = rsb_data->readUint32();
			rsb_data->read_pos = static_cast<std::size_t>(rsg_info_pos) + 140;
			auto packet_bytes = rsb_data->readBytes(32);
			auto packet_size = rsb_data->readUint32(static_cast<std::uint64_t>(rsg_info_pos + 148)) +
				rsb_data->readUint32(static_cast<std::uint64_t>(rsg_info_pos + 152)) + 
				rsb_data->readUint32(static_cast<std::uint64_t>(rsg_info_pos + 168));
			rsb_data->read_pos = packet_pos;
			fix_rsg(packet_size, packet_bytes, rsg_name, destination);
			return;
		}

		auto fix_rsg(
			Size packet_size, 
			List<Byte> & packet_bytes, 
			std::string_view rsg_name,
			std::string_view destination
		) -> void 
		{
			auto rsg_data = DataStreamView{ rsb_data->readBytes(packet_size) };
			rsg_data.write_pos = 0;
			rsg_data.writeString("pgsr"_sv);
			rsg_data.writeUint32(4);
			rsg_data.writeNull(8);
			rsg_data.writeBytes(packet_bytes);
			rsg_data.writeNull(16);
			uncompress_zlib(&rsg_data);
			rsg_data.read_pos = 72;
			auto file_list_length = rsg_data.readUint32();
			auto file_list_begin = rsg_data.readUint32();
			rsg_data.read_pos = file_list_begin;
			auto offset_limit = file_list_begin + file_list_length;
			auto parent_string = CompiledMap{};
			while (rsg_data.read_pos < offset_limit) {
				auto key = std::string{};
				auto positions = Array<std::uint64_t, 3>{
					(rsg_data.read_pos - file_list_begin) / 4,
					(rsg_data.read_pos - file_list_begin) / 4 - 1,
					(rsg_data.read_pos - file_list_begin) / 4 + 1,
				};
				std::for_each(positions.begin(), positions.end(), [&](auto position) {
					if (parent_string.find(position) != parent_string.end()) {
						key += parent_string[position];
						parent_string.erase(position);
					}
					return;
				});
				while (true) {
					auto composite_value = rsg_data.readUint32();
					auto child_string_offset = (composite_value & 0xffffff00) >> 8;
					auto current_character = (composite_value & 0x000000ff) >> 0;
					child_string_offset *= 2;
					current_character -= 2;
					current_character /= 2;
					if (child_string_offset != 0) {
						parent_string.insert(std::pair<int, std::string>(child_string_offset, key));
					}
					if (current_character == 0) {
						break;
					}
					if (current_character == 0x5c) {
						current_character = 0x2f;
					}
					key += static_cast<char>(current_character);
				}
				if (key.size() == 0) {
					break;
				}
				auto res = nlohmann::ordered_json{ 
					{"path", key }
				};
				thiz.write_rsg(fmt::format("{}/{}", destination, key), &rsg_data, res);
			}
			return;
		}

		auto write_rsg(
			std::string_view destination,
			DataStreamView* rsg_data,
			nlohmann::ordered_json &res
		) -> void
		{
			auto is_atlas = rsg_data->readUint32() == 1;
			auto pos = rsg_data->readUint32() + is_atlas ? thiz.atlas_position : 0;
			auto size = rsg_data->readUint32();
			if (is_atlas) {
				auto id = rsg_data->readUint32();
				rsg_data->read_pos += 8;
				res["ptx_info"] = {
					{"id", id},
					{"width", rsg_data->readUint32()},
					{"height", rsg_data->readUint32()}
				};
			}
			raw_data->read_pos = pos;
			{
				#if WINDOWS
				auto path = std::filesystem::path(String::utf8_to_utf16(destination.data()));
				#else
				auto path = std::filesystem::path(destination);
				#endif
				if (path.has_parent_path())
				{
					std::filesystem::create_directories(path.parent_path());
				}
			}
			FileSystem::write_binary<Byte>(destination, raw_data->readBytes(size));
			return;
		}

		inline auto uncompress_zlib(
			DataStreamView* rsg_data
		) -> void
		{
			rsg_data->read_pos = 16;
			auto flags = rsg_data->readUint32();
			rsg_data->read_pos += 4;
			auto part0_pos = rsg_data->readUint32();
			auto part0_size = rsg_data->readUint32();
			auto part0_zlib = rsg_data->readUint32();
			rsg_data->read_pos += 4;
			auto part1_pos = rsg_data->readUint32();
			auto part1_size = rsg_data->readUint32();
			auto part1_zlib = rsg_data->readUint32();
			auto part0_length = part0_pos + part0_size;
			auto part0_zlib_length = part0_pos + part0_zlib;
			if (part0_length > 0 || part0_zlib_length > 0) {
				rsg_data->read_pos = part0_pos;
				if (flags == 0 || flags == 1 || part0_zlib == 0) {
					raw_data->writeBytes(rsg_data->readBytes(part0_size));
				}
				else {
					auto data = DataStreamView{ rsg_data->readBytes(part0_zlib) };
					repair_zlib_data(&data);
					raw_data->writeBytes(Zlib::uncompress(data.getBytes(0, data.size())));
				}
			}
			auto use_altas = part1_pos != 0 && part1_size != 0 && part1_zlib != 0;
			if (use_altas) {
				atlas_position = raw_data->write_pos;
				rsg_data->read_pos = part1_pos;
				if (flags == 0 || flags == 2 || part1_zlib == 0) {
					raw_data->writeBytes(rsg_data->readBytes(part1_size));
				}
				else {
					auto data = DataStreamView{ rsg_data->readBytes(part1_zlib) };
					repair_zlib_data(&data);
					raw_data->writeBytes(Zlib::uncompress(data.getBytes(0, data.size())));
				}
			}
			return;
		}


	public:

		auto process(
			std::string_view destination
		) -> void
		{
			auto rsb_head = RSBHeadInfo{};
			read_head(&rsb_head);
			auto composite_number = rsb_head.composite_number;
			auto composite_info_begin = rsb_head.composite_info_begin;
			auto composite_info_each_length = rsb_head.composite_info_each_length;
			for (auto i : Range<std::uint32_t>(composite_number)) {
				auto c_pool = i * composite_info_each_length + composite_info_begin;
				rsb_data->read_pos = c_pool;
				auto composite_name = rsb_data->readString(128);
				auto is_composite = std::equal(composite_name.rbegin(), composite_name.rend(), std::string{"_CompositeShell"}.rbegin());
				if (is_composite) {
					composite_name = composite_name.substr(0, composite_name.size() - 15);
				}
				auto c_info_pos = c_pool + 0x80;
				auto rsg_number = rsb_data->readUint32(static_cast<std::uint64_t>(c_pool + 0x480));
				for (auto k : Range<std::uint32_t>(rsg_number)) {
					rsb_data->read_pos = static_cast<size_t>(k * 0x10 + c_info_pos);
					auto rsg_index = rsb_data->readUint32();
					auto rsg_res = rsb_data->readUint32();
					auto rsg_info_pos = rsg_index * rsb_head.rsg_info_each_length + rsb_head.rsg_info_begin;
					auto rsg_name = fmt::format("{}{}", composite_name, rsg_res != 0 ? fmt::format("_{}", rsg_res) : rsg_number > 1 ? "_Common" : "");
					read_rsg_info(rsg_info_pos, rsg_name, destination);
				}
			}
			return;
		}

		UnpackCipher(
			UnpackCipher&& that
		) = delete;

		UnpackCipher(

		) = default;

		UnpackCipher(
			std::string_view source,
			const List<Byte> &keys
		) : rsb_data(std::make_unique<DataStreamView>(source)), raw_data(std::make_unique<DataStreamView>()), atlas_position(0), 
			keys(std::move(keys))
		{

		}

		~UnpackCipher(

		) = default;

		auto operator =(
			UnpackCipher&& that
		)->UnpackCipher & = delete;

		static auto process_fs(
			std::string_view source,
			std::string_view destination,
			const List<Byte>& keys
		) -> void
		{
			auto unpacker = UnpackCipher{ source, keys };
			unpacker.process(destination);
			return;
		}
	};

}