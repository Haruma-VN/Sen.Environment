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
                manifest.group[group].subgroup[subgroup].packet_info.res.forEach(function write_pitch(e) {
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
        const rton_list: Array<string> = Kernel.FileSystem.read_directory(source).filter((e: string) => Kernel.FileSystem.is_file(e) && /((\.json))$/i.test(e));
        rton_list.forEach(function process(e: string): void {
            try {
                callback(e.replace(/((\.rton))$/i, ".json"), e);
            } catch (error: any) {
                Console.error(`${Kernel.Language.get("popcap.rsb.pack_for_modding.error_json_file")}: ${e}`);
                // rethrow
                throw error;
            }
            return;
        });
        Console.finished(format(Kernel.Language.get("popcap.rsb.pack_for_modding.made_n_json_to_rton"), rton_list.length));
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

    export function exchange_layout(layout: string): ResourceGroup.PathStyle {
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
            Console.warning(Kernel.Language.get("popcap.rsb.pack_for_modding.resource_group_not_found"));
            return;
        }
        if (category.merge_res_info) {
            Kernel.Support.PopCap.ResInfo.merge_fs(`${source}/res.json.info`, `${source}/res.json`);
            Console.finished(Kernel.Language.get("popcap.rsb.pack_for_modding.merge_res_info_done"));
        }
        if (category.merge_resource_group) {
            const resource_json: string = `${source}/${manifest.group[manifest_group].subgroup[manifest_group].packet_info.res.at(0)?.path.replace(/((\.rton|\.newton))$/i, ".json")}`;
            Kernel.Support.PopCap.ResourceGroup.merge_fs(`${resource_json}.info`, resource_json);
            Console.finished(Kernel.Language.get("popcap.rsb.pack_for_modding.merge_resource_group_done"));
        }
        if (category.res_info_convert) {
            Kernel.Support.PopCap.ResInfo.convert_fs(
                `${source}/res.json`,
                `${source}/resource/${manifest.group[manifest_group].subgroup[manifest_group].packet_info.res.at(0)?.path.replace(/((\.rton|\.newton))$/i, ".json")}`,
            );
            Console.finished(Kernel.Language.get("popcap.rsb.pack_for_modding.converted_res_info_to_resource_group"));
        } else {
            Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.ResInfo.Structure>(`${source}/res.json`);
            Kernel.Support.PopCap.ResourceGroup.convert_fs(
                `${source}/resource/${manifest.group[manifest_group].subgroup[manifest_group].packet_info.res.at(0)?.path.replace(/((\.rton|\.newton))$/i, ".json")}`,
                `${source}/res.json`,
                exchange_layout(Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.ResInfo.Structure>(`${source}/res.json`).expand_path),
            );
            Console.finished(Kernel.Language.get("popcap.rsb.pack_for_modding.converted_resource_group_to_res_info"));
        }
        if (category.encode_newton) {
            let ripe_file = `${source}/resource/${manifest.group[manifest_group].subgroup[manifest_group].packet_info.res.at(0)?.path.replace(/((\.rton|\.newton))$/i, ".json")}`;
            Kernel.Support.PopCap.Newton.encode_fs(ripe_file, ripe_file.replace(/((\.json))/i, ".newton"));
            Console.finished(Kernel.Language.get("popcap.rsb.pack_for_modding.newton_file_is_encoded"));
        }
        if (category.encode_rton) {
            let ripe_file = `${source}/resource/${manifest.group[manifest_group].subgroup[manifest_group].packet_info.res.at(0)?.path.replace(/((\.rton|\.newton))$/i, ".json")}`;
            Kernel.Support.PopCap.RTON.encode_fs(ripe_file, ripe_file.replace(/((\.json))/i, ".rton"));
            Console.finished(Kernel.Language.get("popcap.rsb.pack_for_modding.rton_file_is_encoded"));
        }
        return;
    }

    export function process(source: string, destination: string): void {
        const manifest = Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.RSB.Manifest>(`${source}/manifest.json`);
        const configuration = Kernel.JSON.deserialize_fs<Setting>(`${source}/setting.json`);
        if (configuration.res_info.convert === "?") {
            Console.argument(Kernel.Language.get("popcap.rsb.pack_for_modding.use_res_info"));
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
            Console.warning(Kernel.Language.get("popcap.rsb.pack_for_modding.properties_directory_not_found"));
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
        Object.keys(configuration.rsb.distribution).forEach(function pack_rsb(resolution: string): void {
            if (!((configuration.rsb.distribution as any)[resolution] as boolean)) {
                return;
            }
            if (resolution === "all") {
                Console.finished(Kernel.Language.get("popcap.rsb.pack_for_modding.process_whole_resolution"));
                return Kernel.Support.PopCap.RSB.pack(source, destination, manifest);
            } else {
                const new_manifest: Kernel.Support.PopCap.RSB.Manifest = {
                    version: 4n,
                    ptx_info_size: manifest.ptx_info_size,
                    group: {},
                };
                Console.finished(format(Kernel.Language.get("popcap.rsb.pack_for_modding.process_one_resolution"), resolution));
                generate_special_manifest(manifest, new_manifest, BigInt(resolution));
                return Kernel.Support.PopCap.RSB.pack(source, `${destination}.${resolution}`, new_manifest);
            }
        });
        return;
    }

    export function process_fs(source: string, destination: string): void {
        process(source, destination);
        return;
    }
}
