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

    export function convert_image(source: string, group: string, manifest: Manifest, version: Kernel.Support.PopCap.RSG.Version): void {
        const subgroups = Object.keys(manifest.group[group].subgroup);
        for (const subgroup of subgroups) {
            const packet_info = manifest.group[group].subgroup[subgroup].packet_info;
            if (packet_info.res[0].ptx_info === undefined) continue;
            Kernel.Support.PopCap.RSG.unpack_modding(`${source}/packet/${subgroup}.rsg`, `${source}/resource`);
            for (let i = 0; i < packet_info.res.length; i++) {
                const ptx_info = packet_info.res[i].ptx_info;
                switch (ptx_info!.format) {
                    case 0n: {
                        Kernel.Support.Texture.decode_fs(
                            `${source}/resource/${packet_info.res[i].path}`,
                            `${source}/resource/${packet_info.res[i].path}`,
                            ptx_info!.width,
                            ptx_info!.height,
                            Texture.Format.RGBA_8888,
                        );
                        break;
                    }
                    case 147n: {
                        Kernel.Support.Texture.decode_fs(
                            `${source}/resource/${packet_info.res[i].path}`,
                            `${source}/resource/${packet_info.res[i].path}`,
                            ptx_info!.width,
                            ptx_info!.height,
                            Texture.Format.RGB_ETC1_A_8,
                        );
                        break;
                    }
                    default: {
                        // TODO : add localization
                        throw new Error("invaild_image_format");
                    }
                }
                ptx_info!.format = 0n;
                Kernel.Support.Texture.encode_fs(`${source}/resource/${packet_info.res[i].path}`, `${source}/resource/${packet_info.res[i].path}`, Texture.Format.ARGB_8888);
            }
            Kernel.Support.PopCap.RSG.pack(`${source}/resource`, `${source}/packet/${subgroup}.rsg`, { version, ...packet_info });
        }
        return;
    }

    export function convert_streaming_wave(source: string, group: string, manifest: Manifest, version: Kernel.Support.PopCap.RSG.Version): void {
        const subgroups = Object.keys(manifest.group[group].subgroup);
        if (subgroups[0].toLowerCase() !== "streamingwave") {
            // TODO : add localization
            throw new Error("invalid_streaming_wave");
        }
        Kernel.Support.PopCap.RSG.unpack_modding(`${source}/packet/${subgroups[0]}.rsg`, `${source}/resource`);
        const packet_info = manifest.group[group].subgroup[subgroups[0]].packet_info;
        //static_path
        Kernel.FileSystem.create_directory(`${source}/resource/STREAMINGWAVES/GLOBAL_DATA`);
        for (let i in packet_info.res) {
            const new_path = packet_info.res[i].path.replace(`STREAMINGWAVES`, `STREAMINGWAVES/GLOBAL_DATA`);
            Kernel.FileSystem.Operation.rename(`${source}/resource/${packet_info.res[i].path}`, `${source}/resource/${new_path}`);
            packet_info.res[i].path = new_path;
        }
        Kernel.Support.PopCap.RSG.pack(`${source}/resource`, `${source}/packet/GLOBAL_DATA.rsg`, { version, ...packet_info });
        manifest.group["GLOBAL_DATA"] = {
            is_composite: false,
            subgroup: {
                GOLBAL_DATA: manifest.group[group].subgroup[subgroups[0]],
            },
        };
        delete manifest.group[group];
        return;
    }

    export function convert_mainfest(source: string, group: string, manifest: Manifest, version: Kernel.Support.PopCap.RSG.Version): void {
        const subgroups = Object.keys(manifest.group[group].subgroup);
        if (subgroups[0].includes("manifestgroup")) {
            // TODO : add localization
            throw new Error("invaild_manifestgroup");
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
        const streamingwave_name = groups.find(function (e) {
            if (e.toLowerCase() === "streamingwave") return e;
        });
        if (streamingwave_name === undefined) {
            // TODO : add localization
            throw new Error("cannot_find_streaming_wave");
        }
        const info = Kernel.JSON.deserialize_fs(`${resource_path}.info/subgroup/${streamingwave_name}.json`) as ResourceInfo;
        info.id = "Global_Data";
        for (const e of info.resources) {
            e.id.replace(`RESFILE_STREAMINGWAVES`, `RESFILE_STREAMINGWAVES_GLOBAL_DATA`);
            e.path.replace(`streamingwaves`, `streamingwaves\\Global_Data`);
        }
        Kernel.JSON.serialize_fs(`${resource_path}.info/subgroup/Global_Data.json`, info, 1, true);
        content["Global_Data"] = {
            is_composite: false,
            subgroups: {
                StreamingWave: {
                    type: null,
                },
            },
        };
        delete content[streamingwave_name];
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

    export function process(source: string, destination: string): void {
        const rsb_destination = `${destination}.bundle`;
        Kernel.Support.PopCap.RSB.unpack_fs(source, rsb_destination);
        const manifest = Kernel.JSON.deserialize_fs(`${rsb_destination}/manifest.json`) as Manifest;
        Kernel.FileSystem.create_directory(`${rsb_destination}/resource`);
        const version = manifest.version as Kernel.Support.PopCap.RSG.Version;
        const groups = Object.keys(manifest.group);
        for (const group of groups) {
            if (group.toLowerCase().includes("manifestgroup")) {
                convert_mainfest(rsb_destination, group, manifest, version);
            } else if (group.toLowerCase() === "streamingwave") {
                convert_streaming_wave(rsb_destination, group, manifest, version);
            } else if (manifest.group[group].is_composite) {
                convert_image(rsb_destination, group, manifest, version);
            }
        }
        Kernel.Support.PopCap.RSB.pack(rsb_destination, destination, manifest);
        Kernel.FileSystem.Operation.remove(rsb_destination);
        return;
    }

    export function process_fs(source: string, destination: string): void {
        process(source, destination);
        return;
    }
}
