namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Miscellaneous.UnpackByLooseConstraints {
    export interface PTXInfo {
        id: bigint;
        width: bigint;
        height: bigint;
        pitch: bigint;
        format: bigint;
        alpha_size?: bigint;
        alpha_format?: bigint;
    }

    export interface ResInfo {
        path: string;
        ptx_info?: PTXInfo;
    }

    export type CompressionFlags = 1n | 2n | 3n | 4n;

    export interface PacketInfo {
        compression_flags: bigint;
        res: Array<ResInfo>;
    }

    export type Category = [bigint, string | null];

    export interface ManifestSubGroup {
        category: Category;
        packet_info: PacketInfo;
    }

    export interface ManifestGroup {
        is_composite: boolean;
        subgroup: Record<string, ManifestSubGroup>;
    }

    export interface Manifest {
        version: bigint;
        ptx_info_size: bigint;
        group: Record<string, ManifestGroup>;
    }

    export interface CompositeRSGInfo {
        name: string;
        res: bigint;
        pos: bigint;
        size: bigint;
        part0_pos: bigint;
        part0_size: bigint;
        part0_zlib: bigint;
        part1_pos: bigint;
        part1_size: bigint;
        part1_zlib: bigint;
        ptx_number: bigint;
        ptx_before_number: bigint;
    }

    export type RSBHead = Obfuscate.RSBHead;

    export interface RSGValue {
        path: string;
        pos: bigint;
        size: bigint;
        is_atlas: boolean;
        ptx_info?: {
            id: bigint;
            width: bigint;
            height: bigint;
        };
    }

    // -----------------------------------------------------------------

    export function decode_compiled_map_for_rsg(
        sen: Kernel.DataStreamView,
        rsb_head_info: RSBHead,
        rsg_info: CompositeRSGInfo,
        destination: string,
        raw_data: Kernel.DataStreamView,
        zlib_info: [bigint, bigint],
    ): PacketInfo {
        const file_list_length: bigint = sen.readUint32(rsg_info.pos + 0x48n) + rsg_info.pos;
        const file_list_begin: bigint = sen.readUint32() + rsg_info.pos;
        sen.read_position = file_list_begin;
        const file_list_length_limit: bigint = file_list_begin + file_list_length;
        const parent_string: Record<number, string> = {};
        const res_list: Array<ResInfo> = new Array<ResInfo>();
        while (sen.read_position < file_list_length_limit) {
            let key: string = "";
            const position: number = Number((sen.read_position - file_list_begin) / 4n);
            if (parent_string[position] !== undefined) {
                key += parent_string[position];
                delete parent_string[position];
            }
            while (true) {
                const composite_value: bigint = sen.readUint32();
                const child_string_offset: number = Number((composite_value & 0xffffff00n) >> 8n);
                let current_character: number = Number((composite_value & 0x000000ffn) >> 0n);
                if (child_string_offset !== 0) {
                    parent_string[child_string_offset] = key;
                }
                if (current_character === 0) {
                    break;
                }
                if (current_character === 0x5c) {
                    current_character = 0x2f;
                }
                key += String.fromCharCode(current_character);
            }
            if (key.length === 0) {
                break;
            }
            const is_atlas: boolean = sen.readUint32() === 1n;
            const pos: bigint = sen.readUint32() + (is_atlas ? zlib_info[0] : 0n);
            const size: bigint = sen.readUint32();
            Kernel.FileSystem.create_directory(Kernel.Path.dirname(`${destination}/resource/${key}`));
            Kernel.ArrayBuffer.out(`${destination}/resource/${key}`, raw_data.getArrayBuffer(pos, pos + size));
            const res: ResInfo = { path: key };
            if (is_atlas) {
                const id: bigint = sen.readUint32();
                sen.read_position += 8n;
                const pitch_pos = rsb_head_info.ptx_info_begin + (rsg_info.ptx_before_number - id) * rsb_head_info.ptx_info_each_length + 0x08n;
                const ptx_info: PTXInfo = {
                    id,
                    width: sen.readUint32(),
                    height: sen.readUint32(),
                    pitch: 0n,
                    format: 0n,
                };
                const backup_pos: bigint = sen.read_position;
                ptx_info.pitch = sen.readUint32(pitch_pos);
                ptx_info.format = sen.readUint32();
                if (rsb_head_info.ptx_info_each_length >= 0x14n) {
                    ptx_info.alpha_size = sen.readUint32();
                    ptx_info.alpha_format = rsb_head_info.ptx_info_each_length === 0x18n ? sen.readUint32() : ptx_info.alpha_size === 0n ? 0n : 0x64n;
                }
                sen.read_position = backup_pos;
                res.ptx_info = ptx_info;
            }
            res_list.push(res);
        }
        return {
            compression_flags: zlib_info[1],
            res: res_list,
        };
    }

    // -----------------------------------------------------------------

    const zlib_head_level_list = [
        [0x78n, 0x01n],
        [0x78n, 0x5en],
        [0x78n, 0x9cn],
        [0x78n, 0xdan],
    ];

    export function check_no_zlib_head(sen: Kernel.DataStreamView, pos: bigint): boolean {
        const backup_pos: bigint = sen.read_position;
        const head_1: bigint = sen.readUint8(pos);
        const head_2: bigint = sen.readUint8();
        sen.read_position = backup_pos;
        for (let i = 0; i < 4; ++i) {
            if (head_1 === zlib_head_level_list[i][0] && head_2 === zlib_head_level_list[i][1]) {
                return false;
            }
        }
        return true;
    }

    // -----------------------------------------------------------------

    export function uncompress_zlib(sen: Kernel.DataStreamView, raw_data: Kernel.DataStreamView, rsg_info: CompositeRSGInfo): [bigint, bigint] {
        const pos: bigint = rsg_info.pos;
        const part0_length: bigint = rsg_info.part0_pos + rsg_info.part0_size;
        const part0_zlib_length: bigint = rsg_info.part0_pos + rsg_info.part0_zlib;
        let compression_flags = 3n;
        const rsg_pos: bigint = pos + rsg_info.size;
        if (part0_length > 0n || part0_zlib_length > 0n) {
            if (check_no_zlib_head(sen, rsg_info.part0_pos + pos)) {
                compression_flags = 1n;
                raw_data.writeArrayBuffer(sen.getArrayBuffer(rsg_info.part0_pos + pos, part0_length + pos));
            } else {
                const end_pos: bigint = part0_zlib_length + pos;
                let uncompress = Kernel.Compression.Zlib.uncompress(new Kernel.BinaryView(sen.getArrayBuffer(rsg_info.part0_pos + pos, end_pos > rsg_pos ? rsg_pos : end_pos)));
                raw_data.writeArrayBuffer(uncompress.value);
            }
        }
        //const use_atlas: boolean = rsg_info.part1_pos !== 0n && rsg_info.part1_size !== 0n && rsg_info.part1_zlib !== 0n;
        let atlas_pos = 0n;
        if (rsg_info.ptx_number !== 0n) {
            atlas_pos = raw_data.write_position;
            if (check_no_zlib_head(sen, rsg_info.part1_pos + pos)) {
                compression_flags = 0n;
                raw_data.writeArrayBuffer(sen.getArrayBuffer(rsg_info.part1_pos + pos, rsg_info.part1_pos + rsg_info.part1_size + pos));
            } else {
                const end_pos: bigint = rsg_info.part1_pos + rsg_info.part1_zlib + pos;
                let uncompress = Kernel.Compression.Zlib.uncompress(new Kernel.BinaryView(sen.getArrayBuffer(rsg_info.part1_pos + pos, end_pos > rsg_pos ? rsg_pos : end_pos)));
                raw_data.writeArrayBuffer(uncompress.value);
            }
        }
        return [atlas_pos, compression_flags];
    }

    // -----------------------------------------------------------------

    export function unpack_rsg(sen: Kernel.DataStreamView, rsb_head_info: RSBHead, rsg_info: CompositeRSGInfo, destination: string): PacketInfo {
        const raw_data = new Kernel.DataStreamView();
        const zlib_info: [bigint, bigint] = uncompress_zlib(sen, raw_data, rsg_info);
        return decode_compiled_map_for_rsg(sen, rsb_head_info, rsg_info, destination, raw_data, zlib_info);
    }

    // -----------------------------------------------------------------

    export function decode_compiled_map(sen: Kernel.DataStreamView, rsb_head_info: RSBHead): Record<number, string> {
        const rsg_begin_pos: bigint = rsb_head_info.rsg_list_begin;
        sen.read_position = rsg_begin_pos;
        const file_list_length: bigint = rsg_begin_pos + rsb_head_info.rsg_list_length;
        const parent_string: Record<number, string> = {};
        const map_string: Record<number, string> = {};
        while (sen.read_position < file_list_length) {
            let key = "";
            const position: number = Number((sen.read_position - rsg_begin_pos) / 4n);
            if (parent_string[position] !== undefined) {
                key += parent_string[position];
                delete parent_string[position];
            }
            while (true) {
                const composite_value: bigint = sen.readUint32();
                const child_string_offset: number = Number((composite_value & 0xffffff00n) >> 8n);
                const current_character: number = Number((composite_value & 0x000000ffn) >> 0n);
                if (child_string_offset !== 0) {
                    parent_string[child_string_offset] = key;
                }
                if (current_character === 0) {
                    break;
                }
                key += String.fromCharCode(current_character);
            }
            if (key.length === 0) {
                break;
            }
            const pool_index: number = Number(sen.readUint32());
            map_string[pool_index] = key;
        }
        return map_string;
    }

    // -----------------------------------------------------------------

    export function process(sen: Kernel.DataStreamView, destination: string): Manifest {
        const rsb_head_info: RSBHead = Obfuscate.read_head(sen);
        // phase_1_get_info
        const map_string: Record<number, string> = decode_compiled_map(sen, rsb_head_info);
        // phase_2_unpack
        const group: Record<string, ManifestGroup> = {};
        for (let i = 0n; i < rsb_head_info.composite_number; ++i) {
            const composite_start_pos: bigint = i * rsb_head_info.composite_info_each_length + rsb_head_info.composite_info_begin;
            let composite_name: string = sen.readStringByEmpty(composite_start_pos);
            const composite_info_pos: bigint = composite_start_pos + 0x80n;
            const is_composite: boolean = composite_name.endsWith("_CompositeShell");
            if (is_composite) {
                composite_name = composite_name.substring(0, composite_name.length - 15);
            }
            const rsg_number: bigint = sen.readUint32(composite_start_pos + 0x480n);
            const subgroup: Record<string, ManifestSubGroup> = {};
            for (let k = 0n; k < rsg_number; ++k) {
                const rsg_index: bigint = sen.readUint32(k * 0x10n + composite_info_pos);
                const res_1: bigint = sen.readUint32();
                const res_2: bigint = sen.readUint32();
                const category: Category = [res_1, null];
                if (res_2 !== 0n) {
                    sen.read_position -= 4n;
                    category[1] = sen.readString(4n);
                }
                const rsg_name: string = map_string[Number(rsg_index)];
                const rsg_info_pos: bigint = rsg_index * rsb_head_info.rsg_info_each_length + rsb_head_info.rsg_info_begin;
                const rsg_pos: bigint = sen.readUint32(rsg_info_pos + 0x80n);
                const part0_pos: bigint = sen.readUint32(rsg_info_pos + 0x94n);
                const part0_size: bigint = sen.readUint32();
                const part0_zlib: bigint = sen.readUint32();
                sen.read_position += 4n;
                const part1_pos: bigint = sen.readUint32();
                const part1_size: bigint = sen.readUint32();
                const part1_zlib: bigint = sen.readUint32();
                let rsg_size: bigint = sen.readUint32(rsg_info_pos + 148n) + sen.readUint32(rsg_info_pos + 152n) + sen.readUint32(rsg_info_pos + 168n);
                if (rsg_size <= 1024n) {
                    rsg_size = 4096n;
                }
                const ptx_number: bigint = sen.readUint32(rsg_info_pos + rsb_head_info.rsg_info_each_length - 8n);
                const ptx_before_number: bigint = sen.readUint32();
                const composite_rsg_info: CompositeRSGInfo = {
                    name: rsg_name,
                    res: res_1,
                    pos: rsg_pos,
                    size: rsg_size,
                    part0_pos,
                    part0_size,
                    part0_zlib,
                    part1_pos,
                    part1_size,
                    part1_zlib,
                    ptx_number,
                    ptx_before_number,
                };
                const rsg_file_list_pos: bigint = sen.readUint32(rsg_pos + 0x4cn);
                if (rsg_file_list_pos !== 0x5cn && rsg_file_list_pos !== 0x1000n && rsg_file_list_pos !== 0xecn) {
                    Console.send(`${Kernel.Language.get("popcap.rsb.unpack_by_loose_constraints.skip_category")}: ${rsg_name}`, Definition.Console.Color.RED);
                    continue;
                }
                const packet_info: PacketInfo = unpack_rsg(sen, rsb_head_info, composite_rsg_info, destination);
                subgroup[rsg_name] = {
                    category,
                    packet_info,
                };
            }
            group[composite_name.toUpperCase()] = {
                is_composite,
                subgroup,
            };
        }
        return {
            version: 4n,
            ptx_info_size: rsb_head_info.ptx_info_each_length,
            group,
        };
    }

    // -----------------------------------------------------------------

    export function process_fs(source: string, destination: string): void {
        const sen = new Kernel.DataStreamView(source);
        const manifest: Manifest = process(sen, destination);
        Kernel.JSON.serialize_fs(`${destination}/manifest.json`, manifest, 1, true);
        return;
    }
}
