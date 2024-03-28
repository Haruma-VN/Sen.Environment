namespace Sen.Script.Support.PopCap.RTON.DecodeByLooseConstraints {
    export type DataStreamView = Kernel.DataStreamView;

    let writer: Kernel.JsonWriter = undefined!;

    let json_destination: string;

    const json_table = {
        end_bytecode: 0xffn,
        array_byte_start: 0xfdn,
        array_byte_end: 0xfen,
        rtid_0_sc: 0x0n,
        rtid_1_sc: 0x1n,
        rtid_2_sc: 0x2n,
        rtid_3_sc: 0x3n,
        star_s_bytecode: 0x2n,
        varint32_string_bytecode: 0x81n,
        varint32_varint32_string_bytecode: 0x82n,
        rtid_bytecode: 0x83n,
        rtid_0_s_bytecode: 0x84n,
        binary_bytecode: 0x87n,
        varint32_temp_string_bytecode: 0x90n,
        varint32_indexed_string_bytecode: 0x91n,
        varint32_int32_temp_string_bytecode: 0x92n,
        varint32_indexed_string2_bytecode: 0x93n,
        star_s: "*",
        rtid_0_s: "RTID(0)",
    };

    const r0x90_list = [] as Array<string>;

    const r0x92_list = [] as Array<string>;

    let pos = 0n;

    let str_length_1 = 0n;

    let str_length_2 = 0n;

    export function write(): void {
        // when catch error write this json;
        Kernel.FileSystem.write_file(json_destination, writer.toString());
        return;
    }

    export function get_id(stream: DataStreamView): string {
        const uid = stream.readUint32().toString(16);
        return "0".repeat(8 - uid.length) + uid;
    }

    export function read_utf8_character(stream: DataStreamView, size: { value: bigint }): bigint {
        let value = 0n;
        let current = stream.readUint8();
        let extra_size: bigint;
        if (current < 128n) {
            value = current;
            extra_size = 0n;
        } else if (current < 192n) {
            throw new Error(`${Kernel.Language.get("popcap.rton.force_decode.position")}: ${stream.read_position} : ${Kernel.Language.get("popcap.rton.force_decode.invalid_utf8_first_character")}`);
        } else if (current < 224n) {
            value = current & 31n;
            extra_size = 1n;
        } else if (current < 240n) {
            value = current & 15n;
            extra_size = 2n;
        } else if (current < 248n) {
            value = current & 7n;
            extra_size = 3n;
        } else {
            throw new Error(`${Kernel.Language.get("popcap.rton.force_decode.position")}: ${stream.read_position} : ${Kernel.Language.get("popcap.rton.force_decode.invalid_utf8_first_character")}`);
        }
        size.value = 1n + extra_size;
        while (extra_size > 0) {
            --extra_size;
            current = stream.readUint8();
            if ((current & 192n) != 128n) {
                throw new Error(
                    `${Kernel.Language.get("popcap.rton.force_decode.position")}: ${stream.read_position} : ${Kernel.Language.get("popcap.rton.force_decode.invalid_utf8_extra_character")}`,
                );
            }
            value = (value << 6n) | (current & 63n);
        }
        return value;
    }

    export function read_utf8_string(stream: DataStreamView): string {
        let str = "";
        const length = stream.readVarInt32();
        const size = stream.readVarInt32();
        let actual_size = 0n;
        const character_size = { value: 0n };
        for (let i = 0n; i < length; i++) {
            const character = read_utf8_character(stream, character_size);
            str += String.fromCodePoint(Number(character));
            actual_size += character_size.value;
        }
        if (size !== actual_size) {
            Console.warning(
                `${Kernel.Language.get("popcap.rton.force_decode.position")}: ${stream.read_position}: ${Kernel.Language.get("popcap.rton.force_decode.invalid_string_size")}: ${Kernel.Language.get(
                    "got",
                )} ${size} ${Kernel.Language.get("popcap.rton.force_decode.but_actual")} ${actual_size}`,
            );
        }
        return str;
    }

    export function read_rtid(stream: DataStreamView): string {
        const bytecode = stream.readUint8();
        switch (bytecode) {
            case json_table.rtid_0_sc: {
                return json_table.rtid_0_s;
            }
            case json_table.rtid_1_sc: {
                const value_0x1_2 = stream.readVarInt32();
                const value_0x1_1 = stream.readVarInt32();
                const x16_1 = get_id(stream);
                return `RTID(${value_0x1_1}.${value_0x1_2}.${x16_1}@)`;
            }
            case json_table.rtid_2_sc: {
                //  const length = stream.readVarInt32();
                const str = read_utf8_string(stream);
                const value_0x1_2 = stream.readVarInt32();
                const value_0x1_1 = stream.readVarInt32();
                const x16_1 = get_id(stream);
                return `RTID(${value_0x1_1}.${value_0x1_2}.${x16_1}@${str})`;
            }
            case json_table.rtid_3_sc: {
                // const length = stream.readVarInt32();
                const str_2 = read_utf8_string(stream);
                // const length_2 = stream.readVarInt32();
                const str_1 = read_utf8_string(stream);
                return `RTID(${str_1}@${str_2})`;
            }
            default: {
                catch_error(bytecode, stream, "read_rtid");
                return "";
            }
        }
    }

    export function read_binary(stream: DataStreamView): string {
        stream.read_position += 1n;
        return `BINARY(${stream.readStringByVarInt32()}, ${stream.readVarInt32()})`;
    }

    export function read_array(stream: DataStreamView): void {
        writer.writeStartArray();
        const byte_array = stream.readUint8();
        if (byte_array !== json_table.array_byte_start) {
            catch_error(byte_array, stream, "read_array");
        }
        const array_length = stream.readVarInt32();
        for (let i = 0; i < array_length; i++) {
            read_bytecode(stream, stream.readUint8());
        }
        const byte_array_end = stream.readUint8();
        if (byte_array_end !== json_table.array_byte_end) {
            catch_error(byte_array_end, stream, "read_array");
        }
        writer.writeEndArray();
        return;
    }

    export function catch_error(bytecode: bigint, stream: DataStreamView, function_name: string): void {
        write();
        throw new Error(
            `${Kernel.Language.get("popcap.rton.force_decode.invalid_bytecode")}: ${bytecode.toString(16)} | ${Kernel.Language.get("popcap.rton.force_decode.position")}: ${
                stream.read_position
            } ${Kernel.Language.get("in")} ${function_name}`,
        );
    }

    export function read_bytecode(stream: DataStreamView, bytecode: bigint): void {
        switch (bytecode) {
            case 0x0n: {
                writer.writeBoolean(false);
                return;
            }
            case 0x1n: {
                writer.writeBoolean(true);
                return;
            }
            case 0x2n: {
                writer.writeString(json_table.star_s);
                return;
            }
            case 0x8n: {
                writer.writeBigInt(stream.readUint8());
                return;
            }
            case 0x9n:
            case 0xbn:
            case 0x11n:
            case 0x13n:
            case 0x21n:
            case 0x23n:
            case 0x27n:
            case 0x41n:
            case 0x43n:
            case 0x47n: {
                writer.writeBigInt(0n);
                return;
            }
            case 0xan: {
                writer.writeBigInt(stream.readInt8());
                return;
            }
            case 0x10n: {
                writer.writeBigInt(stream.readInt16());
                return;
            }
            case 0x12n: {
                writer.writeBigInt(stream.readUint16());
                return;
            }
            case 0x20n: {
                writer.writeBigInt(stream.readInt32());
                return;
            }
            case 0x22n: {
                writer.writeNumber(stream.readFloat());
                return;
            }
            case 0x24n: {
                writer.writeBigInt(stream.readVarInt32());
                return;
            }
            case 0x25n: {
                writer.writeBigInt(stream.readZigZag32());
                return;
            }
            case 0x26n: {
                writer.writeBigInt(stream.readUint32());
                return;
            }
            case 0x28n: {
                writer.writeBigInt(stream.readVarUint32());
                return;
            }
            case 0x40n: {
                writer.writeBigInt(stream.readInt64());
                return;
            }
            case 0x42n: {
                writer.writeNumber(stream.readDouble());
                return;
            }
            case 0x44n: {
                writer.writeBigInt(stream.readVarInt64());
                return;
            }
            case 0x45n: {
                writer.writeBigInt(stream.readZigZag64());
                return;
            }
            case 0x46n: {
                writer.writeBigInt(stream.readUint64());
                return;
            }
            case 0x48n: {
                writer.writeBigInt(stream.readVarUint64());
                return;
            }
            case 0x82n: {
                // const length = stream.readVarInt32();
                writer.writeString(read_utf8_string(stream));
                return;
            }
            case 0x83n: {
                writer.writeString(read_rtid(stream));
                return;
            }
            case 0x84n: {
                writer.writeString(json_table.rtid_0_s);
                return;
            }
            case 0x85n: {
                read_object(stream);
                return;
            }
            case 0x86n: {
                read_array(stream);
                return;
            }
            case 0x87n: {
                writer.writeString(read_binary(stream));
                return;
            }
            case 0x90n: {
                const temp_string = stream.readStringByVarInt32();
                r0x90_list.push(temp_string);
                writer.writeString(temp_string);
                return;
            }
            case 0x91n: {
                writer.writeString(r0x90_list[Number(stream.readVarInt32())]);
                return;
            }
            case 0x92n: {
                // const length = stream.readVarInt32();
                const temp_string = read_utf8_string(stream);
                r0x92_list.push(temp_string);
                writer.writeString(temp_string);
                return;
            }
            case 0x93n: {
                writer.writeString(r0x92_list[Number(stream.readVarInt32())]);
                return;
            }
            default: {
                catch_error(bytecode, stream, "read_bytecode");
                return;
            }
        }
    }

    export function read_bytecode_property(stream: DataStreamView, bytecode: bigint): string {
        switch (bytecode) {
            case json_table.star_s_bytecode: {
                return json_table.star_s;
            }
            case json_table.varint32_string_bytecode: {
                return stream.readStringByVarInt32();
            }
            case json_table.rtid_bytecode: {
                return read_rtid(stream);
            }
            case json_table.rtid_0_s_bytecode: {
                return json_table.rtid_0_s;
            }
            case json_table.binary_bytecode: {
                return read_binary(stream);
            }
            case json_table.varint32_temp_string_bytecode: {
                const temp_string = stream.readStringByVarInt32();
                r0x90_list.push(temp_string);
                return temp_string;
            }
            case json_table.varint32_indexed_string_bytecode: {
                return r0x90_list[Number(stream.readVarInt32())];
            }
            case json_table.varint32_int32_temp_string_bytecode: {
                //  const length = stream.readVarInt32();
                const temp_string = read_utf8_string(stream);
                r0x92_list.push(temp_string);
                return temp_string;
            }
            case json_table.varint32_indexed_string2_bytecode: {
                return r0x92_list[Number(stream.readVarInt32())];
            }
            default: {
                catch_error(bytecode, stream, "read_bytecode_property");
                return "null";
            }
        }
    }

    export function read_object(stream: DataStreamView): void {
        writer.writeStartObject();
        let bytecode = stream.readUint8();
        while (bytecode !== json_table.end_bytecode) {
            const property_name = read_bytecode_property(stream, bytecode);
            writer.writePropertyName(property_name);
            read_bytecode(stream, stream.readUint8());
            bytecode = stream.readUint8();
        }
        writer.writeEndObject();
        return;
    }

    export function process(stream: DataStreamView, destination: string): void {
        json_destination = destination;
        stream.read_position += 8n; // skip RTON head and version;
        //clear;
        writer.clear();
        writer.write_indent = true;
        r0x90_list.length = 0;
        r0x92_list.length = 0;
        read_object(stream);
        write();
        return;
    }

    export function process_fs(source: string, destination: string): void {
        if (writer === undefined) {
            writer = new Kernel.JsonWriter();
        }
        process(new Kernel.DataStreamView(source), destination);
        return;
    }
}
