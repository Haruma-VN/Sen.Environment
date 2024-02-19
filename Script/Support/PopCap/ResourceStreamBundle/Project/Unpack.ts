namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Unpack {
    export interface Category {
        decrypt_rton: boolean;
        decode_rton: boolean;
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
        const keys: Array<string> = Object.keys(manifest);
        let packages: Kernel.Support.PopCap.RSB.ResourceStreamGroupPool = undefined!;
        let manifest_group: Kernel.Support.PopCap.RSB.ResourceStreamGroupPool = undefined!;
        for (const key of keys) {
            if (/^PACKAGES$/gi.test(key)) {
                packages = manifest.group[key];
            }
            if (/^__MANIFESTGROUP__(.+)?$/i.test(key)) {
                manifest_group = manifest.group[key];
            }
        }
        do {
            if (packages === undefined) {
                Console.warning(Kernel.Language.get("popcap.rsb.unpack_for_modding.packages_not_found"));
                break;
            }
            // unpack rsg
            if (category.decrypt_rton) {
            }
            if (category.decode_rton) {
            }
        } while (false);
        do {
            if (manifest_group === undefined) {
                Console.warning(Kernel.Language.get("popcap.rsb.unpack_for_modding.manifest_group_not_found"));
                break;
            }
        } while (false);
        return;
    }

    export function process_fs(source: string, destination: string, category: Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Unpack.Category): void {
        process(source, destination, category);
        return;
    }
}
