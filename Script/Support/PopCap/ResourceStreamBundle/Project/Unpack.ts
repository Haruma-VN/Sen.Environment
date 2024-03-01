namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Unpack {
    export interface Category {
        decrypt_rton: boolean;
        decode_rton: boolean;
        layout: ResourceGroup.PathStyle;
    }

    export function make_setting(category: {
        ptx: Configuration.Generic;
        rsb: {
            has_newton: boolean;
            has_rton: boolean;
        };
        rton: {
            encryption_key?: string;
        };
    }): Configuration.Setting {
        return {
            rton: {
                encode_rton: true,
                encrypt_rton: false,
                key: category.rton.encryption_key ?? "",
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
                    Kernel.Support.PopCap.RTON.decode_fs(`${resource_destination}/${e.path}`, `${resource_destination}/${e.path.replace(/\.RTON$/gi, ".json")}`);
                });
            }
        } while (false);
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
                    Console.send("Catch newton first, decode newton file");
                    break;
                }
                if (/\.rton$/gi.test(e.path)) {
                    ripe_json = `${resource_destination}/${e.path.replace(/\.rton$/gi, ".json")}`;
                    Kernel.Support.PopCap.RTON.decode_fs(`${resource_destination}/${e.path}`, ripe_json);
                    Console.send("Catch rton first, decode rton file");
                    break;
                }
            }
            Kernel.Support.PopCap.ResourceGroup.convert_fs(ripe_json, `${destination}/res.json`, category.layout);
        } while (false);
        Console.send("Setting were made, edit setting.json for your own purpose");
        return;
    }

    export function process_fs(source: string, destination: string, category: Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Unpack.Category): void {
        process(source, destination, category);
        return;
    }
}
