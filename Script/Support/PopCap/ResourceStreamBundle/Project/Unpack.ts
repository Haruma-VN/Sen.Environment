namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Unpack {
    export type Generic = Configuration.Generic;

    export type Setting = Configuration.Setting;

    export interface Category {
        decrypt_rton: boolean;
        decode_rton: boolean;
        layout: ResourceGroup.PathStyle;
        generic: Generic;
    }

    export function make_setting(category: {
        ptx: Generic;
        rsb: {
            has_newton: boolean;
            has_rton: boolean;
        };
        rton: {
            encryption_key?: string;
            iv?: string;
        };
    }): Setting {
        return {
            rton: {
                encode_rton: true,
                encrypt_rton: false,
                key: category.rton.encryption_key ?? "",
                iv: category.rton.iv ?? "",
            },
            ptx: {
                generic: category.ptx,
            },
            rsg: {
                rewrite: false,
                pack_everything_still_exists: false,
            },
            rsb: {
                specialization: {
                    all: true,
                    "1536": false,
                    "768": false,
                    "384": false,
                    "1200": false,
                    "640": false,
                },
            },
            resource_group: {
                encode_newton: category.rsb.has_newton,
                encode_rton: category.rsb.has_rton,
                automatic_merge_before_encode: false,
            },
            res_info: {
                convert: "?",
                automatic_merge_before_encode: false,
            },
        };
    }

    export function remake_manifest(manifest: Kernel.Support.PopCap.RSB.Manifest): void {
        // default is version 4
        delete (manifest as any).version;
        // remove pitch
        const groups: Array<string> = Object.keys(manifest.group);
        for (const group of groups) {
            const subgroups: Array<string> = Object.keys(manifest.group[group].subgroup);
            subgroups.forEach((subgroup: string) =>
                manifest.group[group].subgroup[subgroup].packet_info.res.forEach((e) => {
                    if (e.ptx_info) {
                        delete (e as any).ptx_info.pitch;
                    }
                }),
            );
        }
        return;
    }

    export function process(source: string, destination: string, category: Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Unpack.Category): void {
        const manifest: Kernel.Support.PopCap.RSB.Manifest = Kernel.Support.PopCap.RSB.unpack_for_modding_fs(source, destination);
        const keys: Array<string> = Object.keys(manifest.group);
        let packages: string = undefined!;
        let manifest_group: string = undefined!;
        for (const key of keys) {
            if (/^PACKAGES$/gi.test(key)) {
                packages = key;
                break;
            }
        }
        for (const key of keys) {
            if (/^__MANIFESTGROUP__(.+)?$/i.test(key)) {
                manifest_group = key;
                break;
            }
        }
        const resource_destination: string = `${destination}/resource`;
        do {
            if (packages === undefined) {
                Console.warning(Kernel.Language.get("popcap.rsb.unpack_for_modding.packages_not_found"));
                break;
            }
            const definition: Kernel.Support.PopCap.RSG.Definition = Kernel.Support.PopCap.RSG.unpack_modding(`${destination}/packet/${packages}.rsg`, resource_destination);
            if (category.decode_rton) {
                definition.res.forEach((e) => {
                    Kernel.Support.PopCap.RTON.decode_fs(`${resource_destination}/${e.path}`, `${resource_destination}/${e.path.replace(/\.rton$/gi, ".json")}`);
                });
            }
        } while (false);
        const config = {
            has_rton: true,
            has_newton: false,
        };
        do {
            if (manifest_group === undefined) {
                Console.warning(Kernel.Language.get("popcap.rsb.unpack_for_modding.manifest_group_not_found"));
                break;
            }
            const definition: Kernel.Support.PopCap.RSG.Definition = Kernel.Support.PopCap.RSG.unpack_modding(`${destination}/packet/${manifest_group}.rsg`, resource_destination);
            let ripe_json: string = undefined!;
            for (const e of definition.res) {
                if (/\.newton$/gi.test(e.path)) {
                    ripe_json = `${resource_destination}/${e.path.replace(/\.newton$/gi, ".json")}`;
                    Kernel.Support.PopCap.Newton.decode_fs(`${resource_destination}/${e.path}`, ripe_json);
                    Console.finished(Kernel.Language.get("popcap.rsb.unpack_for_modding.decode_newton_file"));
                    config.has_newton = true;
                    break;
                }
                if (/\.rton$/gi.test(e.path)) {
                    ripe_json = `${resource_destination}/${e.path.replace(/\.rton$/gi, ".json")}`;
                    Kernel.Support.PopCap.RTON.decode_fs(`${resource_destination}/${e.path}`, ripe_json);
                    Console.finished(Kernel.Language.get("popcap.rsb.unpack_for_modding.decode_rton_file"));
                    break;
                }
            }
            Kernel.Support.PopCap.ResourceGroup.convert_fs(ripe_json, `${destination}/res.json`, category.layout);
        } while (false);
        Kernel.JSON.serialize_fs<Setting>(
            `${destination}/setting.json`,
            make_setting({
                ptx: category.generic,
                rsb: { has_newton: config.has_newton, has_rton: config.has_rton },
                rton: { encryption_key: "65bd1b2305f46eb2806b935aab7630bb", iv: "1b2305f46eb2806b935aab76" },
            }),
            1,
            false,
        );
        remake_manifest(manifest);
        Console.output(Kernel.Language.get("popcap.rsb.unpack_for_modding.make_setting"));
        Kernel.JSON.serialize_fs<Kernel.Support.PopCap.RSB.Manifest>(`${destination}/manifest.json`, manifest, 1, false);
        return;
    }

    export function process_fs(source: string, destination: string, category: Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Unpack.Category): void {
        process(source, destination, category);
        return;
    }
}
