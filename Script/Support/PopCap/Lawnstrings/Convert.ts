namespace Sen.Script.Support.PopCap.LawnStrings.Convert {
    /**
     * Structure
     */
    export interface JsonMap {
        objects: [
            {
                aliases: ["LawnStringsData"];
                objclass: "LawnStringsData";
                objdata: {
                    LocStringValues: Record<string, string>;
                };
            },
        ];
        version: 1n;
    }
    /**
     * Structure
     */
    export interface JsonArray {
        objects: [
            {
                aliases: ["LawnStringsData"];
                objclass: "LawnStringsData";
                objdata: {
                    LocStringValues: Array<string>;
                };
            },
        ];
        version: 1n;
    }

    /**
     *
     * @param text - from lawnstrings text
     * @returns list
     */

    export function make_list(text: string): Array<string> {
        const text_list: Array<string> = text.split("\n");
        const arr: Array<string> = new Array<string>();
        for (let i: number = 0; i < text_list.length; ++i) {
            if (/^\[.*\]\s*$/.test(text_list[i])) {
                arr.push(text_list[i].slice(1, -1));
                let destination: number = -1;
                find_next: for (let j: number = i + 1; j < text_list.length; ++j) {
                    if (/^\[.*\]\s*$/.test(text_list[j])) {
                        destination = j;
                        break find_next;
                    }
                }
                let text_ripe: string = "";
                if (destination !== -1) {
                    for (let k: number = i + 1; k < destination - 1; ++k) {
                        test_case: switch (text_list[k]) {
                            case "": {
                                text_ripe = text_ripe.concat("\r\n");
                                break test_case;
                            }
                            default: {
                                if (text_ripe !== "") {
                                    text_ripe = text_ripe.concat("\r\n");
                                }
                                text_ripe = text_ripe.concat(text_list[k]);
                                break test_case;
                            }
                        }
                        ++i;
                    }
                } else {
                    find_last: for (let j: number = text_list.length - 1; j > i; --j) {
                        if (text_list[j] === "") {
                            destination = j;
                            break find_last;
                        }
                    }
                    for (let k: number = i + 1; k < destination - 1; ++k) {
                        switch (text_list[k]) {
                            case "": {
                                text_ripe = text_ripe.concat("\r\n");
                                break;
                            }
                            default: {
                                text_ripe = text_ripe.concat(text_list[k]);
                            }
                        }
                        ++i;
                    }
                }
                arr.push(text_ripe);
            }
        }
        return arr;
    }

    /**
     *
     * @param text - JS String
     * @returns JS Object
     */

    export function text_to_map(text: string): JsonMap {
        const json_map: JsonMap = {
            objects: [
                {
                    aliases: ["LawnStringsData"],
                    objclass: "LawnStringsData",
                    objdata: {
                        LocStringValues: {},
                    },
                },
            ],
            version: 1n,
        };
        const list_text: Array<string> = make_list(text);
        if (list_text.length < 1) {
            return json_map;
        }
        for (let i: number = 0; i < list_text.length; ++i) {
            json_map.objects[0].objdata.LocStringValues[list_text[i]] = list_text[i + 1];
            ++i;
        }
        return json_map;
    }
    /**
     *
     * @param text - Pass text
     * @returns JSON Text
     */

    export function text_to_array(text: string): JsonArray {
        return {
            objects: [
                {
                    aliases: ["LawnStringsData"],
                    objclass: "LawnStringsData",
                    objdata: {
                        LocStringValues: make_list(text),
                    },
                },
            ],
            version: 1n,
        };
    }
    /**
     *
     * @param information - Pass JSON Map Deserialize
     * @returns Text
     */

    export function map_to_text(source: JsonMap): string {
        let text: string = "";
        const keys: Array<string> = Object.keys(source.objects[0].objdata.LocStringValues);
        keys.forEach((key: string) => {
            text = text.concat(`[${key}]`);
            text = text.concat("\n");
            text = text.concat(`${source.objects[0].objdata.LocStringValues[key].replace(/\r/g, ``)}`);
            text = text.concat("\n");
            text = text.concat("\n");
        });
        return text;
    }

    /**
     *
     * @param information - Pass Json Text
     * @returns - Text
     */

    export function array_to_text(information: JsonArray): string {
        let text: string = "";
        for (let i: number = 0; i < information.objects[0].objdata.LocStringValues.length; ++i) {
            text = text.concat(`[${information.objects[0].objdata.LocStringValues[i]}]`);
            text = text.concat("\n");
            text = text.concat(`${information.objects[0].objdata.LocStringValues[i + 1].replace(/\r/g, ``)}`);
            text = text.concat("\n");
            text = text.concat("\n");
            ++i;
        }
        return text;
    }

    /**
     *
     * @param information - JSON Array
     * @returns JSON Map
     */

    export function array_to_map(information: JsonArray): JsonMap {
        const json_map: JsonMap = {
            version: 1n,
            objects: [
                {
                    aliases: ["LawnStringsData"],
                    objclass: "LawnStringsData",
                    objdata: {
                        LocStringValues: {},
                    },
                },
            ],
        };
        for (let i: number = 0; i < information.objects[0].objdata.LocStringValues.length; ++i) {
            json_map.objects[0].objdata.LocStringValues[information.objects[0].objdata.LocStringValues[i]] = information.objects[0].objdata.LocStringValues[i + 1];
            ++i;
        }
        return json_map;
    }

    /**
     *
     * @param information - JSON Map
     * @returns JSON Array
     */

    export function map_to_array(information: JsonMap): JsonArray {
        const json_array: JsonArray = {
            version: 1n,
            objects: [
                {
                    aliases: ["LawnStringsData"],
                    objclass: "LawnStringsData",
                    objdata: {
                        LocStringValues: [],
                    },
                },
            ],
        };
        const keys: Array<string> = Object.keys(information.objects[0].objdata.LocStringValues);
        for (let key of keys) {
            json_array.objects[0].objdata.LocStringValues.push(key);
            json_array.objects[0].objdata.LocStringValues.push(information.objects[0].objdata.LocStringValues[key]);
        }
        return json_array;
    }

    /**
     * Support type
     */

    export type Conversion = "text" | "array" | "map" | "cn-text";

    /**
     * -------------------------------------------
     * Process method
     * @param source - Source data
     * @param source_type - Typeof source
     * @param destination_type - Destination type
     * @returns Data
     * -------------------------------------------
     */

    export function process<T extends string | JsonArray | JsonMap, Value extends string | JsonArray | JsonMap>(source: T, source_type: Conversion, destination_type: Conversion): Value {
        if (source_type === destination_type) {
            throw new Error(Kernel.Language.get("popcap.lawnstrings.convert.invalid_conversion_type"));
        }
        switch (source_type) {
            case "cn-text":
            case "text": {
                if (destination_type === "array") {
                    return text_to_array(source as string) as Value;
                } else {
                    return text_to_map(source as string) as Value;
                }
            }
            case "array": {
                if (destination_type === "text") {
                    return array_to_text(source as JsonArray) as Value;
                } else {
                    return array_to_map(source as JsonArray) as Value;
                }
            }
            case "map": {
                if (destination_type === "text") {
                    return map_to_text(source as JsonMap) as Value;
                } else {
                    return map_to_array(source as JsonMap) as Value;
                }
            }
        }
    }

    /**
     * -------------------------------------------
     * Process file
     * @param source - Source
     * @param destination - Destination
     * @param source_type - Typeof source
     * @param destination_type - Destination type
     * @returns Data
     * -------------------------------------------
     */

    export function process_fs(source: string, destination: string, source_type: Conversion, destination_type: Conversion): void {
        let source_data: string | JsonArray | JsonMap = undefined!;
        switch (source_type) {
            case "text": {
                source_data = Kernel.FileSystem.read_file_encode_with_utf16le(source);
                break;
            }
            case "cn-text": {
                source_data = Kernel.FileSystem.read_file(source);
                break;
            }
            case "array":
            case "map": {
                source_data = Kernel.JSON.deserialize_fs(source);
                break;
            }
            default: {
                throw new Error(format(Kernel.Language.get("popcap.lawnstrings.convert.unsupported_type"), source_type));
            }
        }
        const destination_data: string | JsonMap | JsonArray = process(source_data, source_type, destination_type);
        switch (destination_type) {
            case "text": {
                Kernel.FileSystem.write_file_encode_with_utf16le(destination, destination_data as string);
                break;
            }
            case "cn-text": {
                Kernel.FileSystem.write_file(destination, destination_data as string);
                break;
            }
            case "map":
            case "array": {
                Kernel.JSON.serialize_fs(destination, destination_data, 1, false);
                break;
            }
            default: {
                throw new Error(format(Kernel.Language.get("popcap.lawnstrings.convert.unsupported_type"), source_type));
            }
        }
        return;
    }
}
