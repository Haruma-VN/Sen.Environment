namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Pack {
    export function calculate_pitch(format: bigint, width: bigint): bigint {
        if (format === 30n) {
            return width / 2n;
        }
        return width * 4n;
    }

    export function remake_manifest(manifest: Kernel.Support.PopCap.RSB.Manifest): void {
        // Expected version always 4
        manifest.version = 4n;
        // re-add pitch
        const groups: Array<string> = Object.keys(manifest.group);
        for (const group of groups) {
            const subgroups: Array<string> = Object.keys(manifest.group[group].subgroup);
            subgroups.forEach(function rewrite_manifest(subgroup: string): void {
                manifest.group[group].subgroup[subgroup].packet_info.res.forEach((e) => {
                    if (e.ptx_info) {
                        e.ptx_info.pitch = calculate_pitch(e.ptx_info.format, e.ptx_info.width);
                    }
                });
            });
        }
        return;
    }

    export function find_manifest_group(manifest: Kernel.Support.PopCap.RSB.Manifest): string {
        let manifest_group: string = undefined!;
        for (const key of Object.keys(manifest.group)) {
            if (/^__MANIFESTGROUP__(.+)?$/i.test(key)) {
                manifest_group = key;
                break;
            }
        }
        return manifest_group;
    }

    export function generate_special_manifest(source: Kernel.Support.PopCap.RSB.Manifest, destination: Kernel.Support.PopCap.RSB.Manifest, resolution: bigint): void {
        Object.keys(source.group).forEach(function process_manifest(e: string): void {
            const is_composite: boolean = source.group[e].is_composite;
            destination.group[e] = {
                is_composite: is_composite,
                subgroup: {},
            };
            if (is_composite) {
                Object.keys(source.group[e].subgroup).forEach(function process_composite(sub: string): void {
                    if (source.group[e].subgroup[sub].category[0] === resolution || source.group[e].subgroup[sub].category[0] === 0n) {
                        destination.group[e].subgroup[sub] = source.group[e].subgroup[sub];
                    }
                });
            } else {
                destination.group[e].subgroup = source.group[e].subgroup;
            }
        });
        return;
    }

    export type Setting = Configuration.Setting;

    export type Callback = (source: string, destination: string) => void;

    export type Cipher = {
        key: string;
        iv: string;
    };

    export function process_rton(source: string, callback: Callback): void {
        const rton_list: Array<string> = Kernel.FileSystem.read_directory(source).filter((e: string) => Kernel.FileSystem.is_file(e) && /((\.rton))$/i.test(e));
        rton_list.forEach(function process(e: string): void {
            try {
                callback(e, e.replace(/((\.rton))$/i, ".json"));
            } catch (error: any) {
                // TODO : Add localization
                Console.error(`error file: ${e}`);
                // rethrow
                throw error;
            }
            return;
        });
        // TODO : Add localization
        Console.finished(`process: ${rton_list.length} JSON to RTON`);
        return;
    }

    export function encode_cipher_rton(source: string, cipher: Cipher): void {
        return process_rton(source, function encode_cipher(source_s: string, destination_s: string): void {
            return Kernel.Support.PopCap.RTON.encode_and_encrypt_fs(source_s, destination_s, cipher.key, cipher.iv);
        });
    }

    export function encode_rton(source: string): void {
        return process_rton(source, Kernel.Support.PopCap.RTON.encode_fs);
    }

    export function encrypt_rton(source: string, cipher: Cipher): void {
        return process_rton(source, function encrypt(source_s: string, destination_s: string): void {
            return Kernel.Support.PopCap.RTON.encrypt_fs(source_s, destination_s, cipher.key, cipher.iv);
        });
    }

    export function convert_resource(
        source: string,
        manifest: Kernel.Support.PopCap.RSB.Manifest,
        category: {
            res_info_convert: boolean;
            encode_newton: boolean;
            encode_rton: boolean;
            merge_res_info: boolean;
            merge_resource_group: boolean;
        },
    ): void {
        let manifest_group: string = undefined!;
        for (const key of Object.keys(manifest.group)) {
            if (/^__MANIFESTGROUP__(.+)?$/i.test(key)) {
                manifest_group = key;
                break;
            }
        }
        if (manifest_group === undefined) {
            // TODO : add localization
            Console.warning("manifest group not found");
            return;
        }
        if (category.merge_res_info) {
            merge_resource(`${source}/res.json.info`, `${source}/res.json`, Kernel.Support.PopCap.ResInfo.merge_fs);
            // TODO : Add localization
            Console.finished("merge res-info done");
        }
        if (category.merge_resource_group) {
            const resource_json: string = `${source}/${manifest.group[manifest_group].subgroup[manifest_group].packet_info.res.at(0)?.path.replace(/((\.rton|.\newton))$/i, ".json")}`;
            merge_resource(`${resource_json}.info`, resource_json, Kernel.Support.PopCap.ResourceGroup.merge_fs);
            // TODO : Add localization
            Console.finished("merge resource-group done");
        }
        if (category.res_info_convert) {
            Kernel.Support.PopCap.ResInfo.convert_fs(
                `${source}/res.json`,
                `${source}/${manifest.group[manifest_group].subgroup[manifest_group].packet_info.res.at(0)?.path.replace(/((\.rton|.\newton))$/i, ".json")}`,
            );
            // TODO : Add localization
            Console.finished("convert res_info to resource done");
        } else {
            Kernel.Support.PopCap.ResourceGroup.convert_fs(
                `${source}/${manifest.group[manifest_group].subgroup[manifest_group].packet_info.res.at(0)?.path.replace(/((\.rton|.\newton))$/i, ".json")}`,
                `${source}/res.json`,
                (function exchange_layout(layout: string): ResourceGroup.PathStyle {
                    switch (layout) {
                        case "string": {
                            return ResourceGroup.PathStyle.WindowStyle;
                        }
                        case "array": {
                            return ResourceGroup.PathStyle.ArrayStyle;
                        }
                        default: {
                            throw new Error(format(Kernel.Language.get("popcap.resource_group.convert.cannot_exchange_layout"), layout));
                        }
                    }
                })(Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.ResInfo.Structure>(`${source}/res.json`).expand_path),
            );
            // TODO : Add localization
            Console.finished("convert resource to res_info done");
        }
        if (category.encode_newton) {
            let ripe_file = `${source}/${manifest.group[manifest_group].subgroup[manifest_group].packet_info.res.at(0)?.path.replace(/((\.rton|.\newton))$/i, ".json")}`;
            Kernel.Support.PopCap.Newton.encode_fs(ripe_file, ripe_file.replace(/((\.json))/i, ".newton"));
            // TODO : Add localization
            Console.finished("encode newton done");
        }
        if (category.encode_rton) {
            let ripe_file = `${source}/${manifest.group[manifest_group].subgroup[manifest_group].packet_info.res.at(0)?.path.replace(/((\.rton|.\newton))$/i, ".json")}`;
            Kernel.Support.PopCap.RTON.encode_fs(ripe_file, ripe_file.replace(/((\.json))/i, ".rton"));
            // TODO : Add localization
            Console.finished("encode rton done");
        }
        return;
    }

    export function merge_resource(source: string, destination: string, callback: Callback): void {
        return callback(source, destination);
    }

    export function process(source: string, destination: string): void {
        const manifest = Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.RSB.Manifest>(`${source}/manifest.json`);
        const configuration = Kernel.JSON.deserialize_fs<Setting>(`${source}/setting.json`);
        if (configuration.res_info.convert === "?") {
            configuration.res_info.convert = Executor.input_boolean();
        }
        if (Kernel.FileSystem.is_directory(`${source}/resource/PROPERTIES`)) {
            convert_resource(source, manifest, {
                res_info_convert: configuration.res_info.convert,
                encode_newton: configuration.resource_group.encode_newton,
                encode_rton: configuration.resource_group.encode_rton,
                merge_res_info: configuration.res_info.automatic_merge_before_encode,
                merge_resource_group: configuration.resource_group.automatic_merge_before_encode,
            });
        } else {
            // TODO : Add localization
            Console.warning("Skip resource convert because PROPERTIES directory not found");
        }
        if (configuration.rton.encrypt_rton && !configuration.rton.encode_rton) {
            encrypt_rton(`${source}/resource/PACKAGES`, { key: configuration.rton.key, iv: configuration.rton.iv });
        }
        if (configuration.rton.encrypt_rton && configuration.rton.encode_rton) {
            encode_cipher_rton(`${source}/resource/PACKAGES`, { key: configuration.rton.key, iv: configuration.rton.iv });
        }
        if (!configuration.rton.encrypt_rton && configuration.rton.encode_rton) {
            encode_rton(`${source}/resource/PACKAGES`);
        }
        remake_manifest(manifest);
        const process_whole = (manifest: Kernel.Support.PopCap.RSB.Manifest, destination: string): void => {
            Kernel.Support.PopCap.RSB.pack(source, destination, manifest);
        };
        Object.keys(configuration.rsb.distribution).forEach(function pack_rsb(resolution: string): void {
            if (!((configuration.rsb.distribution as any)[resolution] as boolean)) {
                return;
            }
            if (resolution === "all") {
                Console.finished(Kernel.Language.get("popcap.rsb.pack_for_modding.process_whole_resolution"));
                return process_whole(manifest, destination);
            } else {
                const new_manifest: Kernel.Support.PopCap.RSB.Manifest = {
                    version: 4n,
                    ptx_info_size: manifest.ptx_info_size,
                    group: {},
                };
                Console.finished(format(Kernel.Language.get("popcap.rsb.pack_for_modding.process_one_resolution"), resolution));
                generate_special_manifest(manifest, new_manifest, BigInt(resolution));
                return process_whole(new_manifest, `${destination}.${resolution}`);
            }
        });
        return;
    }
}
