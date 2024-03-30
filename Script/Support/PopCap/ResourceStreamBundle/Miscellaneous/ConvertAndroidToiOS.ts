namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Miscellaneous.ConvertAndroidToIOS {
    export type Manifest = Kernel.Support.PopCap.RSB.Manifest;

    export type PacketInfo = Kernel.Support.PopCap.RSG.Definition;

    export type DataStreamView = Kernel.DataStreamView;

    export interface ResourceSubInfo {
        type: string;
        id: string;
        path: string;
    }

    export interface ResourceInfo {
        type: string;
        id: string;
        resources: Array<ResourceSubInfo>;
    }

    const RSBResolutionX = [
        "high_quality_resolution", // 1536
        "low_quality_resolution", // 768
    ];

    export type RSBResolution = (typeof RSBResolutionX)[number];

    export function is_power_of_2<T extends number | bigint>(num: T): boolean {
        return (num & -num) === num;
    }

    export const format_148 = ["UI_JOUST_ICICLES_1536", "UI_LEAGUES_1536", "UI_PENNYPURSUITS_HOWTOPLAY_1536"];

    export function convert_image(source: string, group: string, manifest: Manifest, version: Kernel.Support.PopCap.RSG.Version, resolution: RSBResolution) {
        const subgroups = Object.keys(manifest.group[group].subgroup);
        for (const subgroup of subgroups) {
            if (subgroup.toLowerCase().endsWith("_common")) continue;
            if (!subgroup.endsWith(resolution === "high_quality_resolution" ? "_1536" : "_768")) {
                delete manifest.group[group].subgroup[subgroup];
                continue;
            }
            Kernel.Support.PopCap.RSG.unpack_modding(`${source}/packet/${subgroup}.rsg`, `${source}/resource`);
            const packet_info = manifest.group[group].subgroup[subgroup].packet_info;
            for (const e of packet_info.res) {
                const ptx_info = e.ptx_info!;
                switch (ptx_info!.format) {
                    case 0n: {
                        Kernel.Support.Texture.decode_fs(`${source}/resource/${e.path}`, `${source}/resource/${e.path}`, ptx_info.width, ptx_info.height, Texture.Format.RGBA_8888);
                        break;
                    }
                    case 147n: {
                        Kernel.Support.Texture.decode_fs(`${source}/resource/${e.path}`, `${source}/resource/${e.path}`, ptx_info.width, ptx_info.height, Texture.Format.RGB_ETC1_A_8);
                        break;
                    }
                    default: {
                        throw new Error(Kernel.Language.get("popcap.ptx.invalid_image_format"));
                    }
                }
                const convert_whole = (info_format: bigint, format: Texture.Format) => {
                    ptx_info.format = info_format;
                    Kernel.Support.Texture.encode_fs(`${source}/resource/${e.path}`, `${source}/resource/${e.path}`, format);
                };
                /*
                if (format_148.includes(subgroup.toUpperCase())) {
                    convert_whole(148n, Texture.Format.RGB_PVRTC_4BPP_A_8);
                    continue;
                }
                if (ptx_info.width === ptx_info.height && is_power_of_2(ptx_info.width)) {
                    convert_whole(30n, Texture.Format.RGBA_PVRTC_4BPP);
                    ptx_info.pitch = ptx_info.width / 2n;
                    continue;
                }
                */
                convert_whole(0n, Texture.Format.ARGB_8888);
            }
            Kernel.Support.PopCap.RSG.pack(`${source}/resource`, `${source}/packet/${subgroup}.rsg`, { version, ...packet_info });
        }
        return;
    }

    export function convert_streaming_wave(source: string, group: string, manifest: Manifest, version: Kernel.Support.PopCap.RSG.Version) {
        const subgroups = Object.keys(manifest.group[group].subgroup);
        if (subgroups[0].toLowerCase() !== "streamingwave") {
            throw new Error("invaild_streaming_wave");
        }
        Kernel.Support.PopCap.RSG.unpack_modding(`${source}/packet/${subgroups[0]}.rsg`, `${source}/resource`);
        const packet_info = manifest.group[group].subgroup[subgroups[0]].packet_info;
        //static_path
        Kernel.FileSystem.create_directory(`${source}/resource/STREAMINGWAVES/GLOBAL_DATA`);
        for (let i = 0; i < packet_info.res.length; i++) {
            const new_path = packet_info.res[i].path.replace(`STREAMINGWAVES`, `STREAMINGWAVES/GLOBAL_DATA`);
            Kernel.FileSystem.Operation.rename(`${source}/resource/${packet_info.res[i].path}`, `${source}/resource/${new_path}`);
            packet_info.res[i].path = new_path;
        }
        Kernel.Support.PopCap.RSG.pack(`${source}/resource`, `${source}/packet/GLOBAL_DATA.rsg`, { version, ...packet_info });
        manifest.group["Global_Data"] = {
            is_composite: false,
            subgroup: {
                Global_Data: manifest.group[group].subgroup[subgroups[0]],
            },
        };
        delete manifest.group[group];
        return;
    }

    export function convert_mainfest(source: string, group: string, manifest: Manifest, version: Kernel.Support.PopCap.RSG.Version, resolution: RSBResolution) {
        const subgroups = Object.keys(manifest.group[group].subgroup);
        if (subgroups[0].includes("manifestgroup")) {
            throw new Error(Kernel.Language.get("popcap.ptx.invalid_manifest_group"));
        }
        Kernel.Support.PopCap.RSG.unpack_modding(`${source}/packet/${subgroups[0]}.rsg`, `${source}/resource`);
        const manifest_resource_dir = Kernel.FileSystem.read_directory_only_file(`${source}/resource/PROPERTIES`);
        const resource_contains_newton = manifest_resource_dir.find((e) => e.toLowerCase().endsWith(".newton"));
        const resource_path = `${source}/resource/PROPERTIES/RESOURCES${resource_contains_newton ? ".newton" : ".json"}`;
        if (resource_contains_newton) {
            Kernel.Support.PopCap.Newton.decode_fs(resource_path, resource_path);
        } else {
            Kernel.Support.PopCap.RTON.decode_fs(resource_path, resource_path);
        }
        Kernel.Support.PopCap.ResourceGroup.split_fs(resource_path, `${resource_path}.info`);
        const content = Kernel.JSON.deserialize_fs(`${resource_path}.info/content.json`) as Record<string, any>;
        const groups = Object.keys(content);
        // rewrite_content
        for (const group of groups) {
            if (!content[group].is_composite) continue;
            const subgroups = Object.keys(content[group].subgroups);
            for (const subgroup of subgroups) {
                if (subgroup.toLowerCase().endsWith("_common")) continue;
                if (!subgroup.endsWith(resolution === "high_quality_resolution" ? "_1536" : "_768")) {
                    delete content[group].subgroups[subgroup];
                    continue;
                }
            }
        }
        const streamingwave_name = groups.find(function (e) {
            if (e.toLowerCase() === "streamingwave") return e;
        });
        if (streamingwave_name === undefined) throw Error("cannot_find_streaming_wave");
        const info = Kernel.JSON.deserialize_fs(`${resource_path}.info/subgroup/${streamingwave_name}.json`) as ResourceInfo;
        info.id = "Global_Data";
        for (let i = 0; i < info.resources.length; i++) {
            info.resources[i].id = info.resources[i].id.replace("RESFILE_STREAMINGWAVES", "RESFILE_STREAMINGWAVES_GLOBAL_DATA");
            info.resources[i].path = info.resources[i].path.replace("streamingwaves", "streamingwaves\\Global_Data");
        }
        Kernel.JSON.serialize_fs(`${resource_path}.info/subgroup/Global_Data.json`, info, 1, true);
        content["Global_Data"] = {
            is_composite: false,
            subgroups: {
                Global_Data: {
                    type: null,
                },
            },
        };
        delete content[streamingwave_name];
        Kernel.JSON.serialize_fs(`${resource_path}.info/content.json`, content, 1, true);
        Kernel.Support.PopCap.ResourceGroup.merge_fs(`${resource_path}.info`, resource_path);
        if (resource_contains_newton) {
            Kernel.Support.PopCap.Newton.encode_fs(resource_path, resource_path);
        } else {
            Kernel.Support.PopCap.RTON.encode_fs(resource_path, resource_path);
        }
        const packet_info = manifest.group[group].subgroup[subgroups[0]].packet_info;
        Kernel.Support.PopCap.RSG.pack(`${source}/resource`, `${source}/packet/${group}.rsg`, { version, ...packet_info });
        return;
    }

    export function process(source: string, destination: string, resolution: RSBResolution) {
        if (resolution !== "high_quality_resolution" && resolution !== "low_quality_resolution") {
            throw new Error(Kernel.Language.get("popcap.ptx.invalid_rsb_resolution"));
        }
        const folder_destination = `${destination}.bundle`;
        Kernel.Support.PopCap.RSB.unpack_fs(source, folder_destination);
        const manifest = Kernel.JSON.deserialize_fs(`${folder_destination}/manifest.json`) as Manifest;
        Kernel.FileSystem.create_directory(`${folder_destination}/resource`);
        const version = manifest.version as Kernel.Support.PopCap.RSG.Version;
        const groups = Object.keys(manifest.group);
        for (const group of groups) {
            Console.send(group);
            // TODO: Add progress bar
            // TODO add localization;
            if (group.toLowerCase().includes("manifestgroup")) {
                convert_mainfest(folder_destination, group, manifest, version, resolution);
                continue;
            }
            if (group.toLowerCase() === "streamingwave") {
                convert_streaming_wave(folder_destination, group, manifest, version);
                continue;
            }
            if (manifest.group[group].is_composite) {
                convert_image(folder_destination, group, manifest, version, resolution);
                continue;
            }
        }
        Kernel.Support.PopCap.RSB.pack(folder_destination, destination, manifest);
        Kernel.FileSystem.Operation.remove_all(folder_destination);
        return;
    }

    export function process_fs(source: string, destination: string, resolution: RSBResolution) {
        process(source, destination, resolution);
        return;
    }
}
