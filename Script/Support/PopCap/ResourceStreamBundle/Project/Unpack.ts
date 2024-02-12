namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Unpack {
    export interface Category {
        decrypt_rton: boolean;
        decode_rton: boolean;
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
        } while (false);
        return;
    }

    export function process_fs(source: string, destination: string, category: Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Unpack.Category): void {
        process(source, destination, category);
        return;
    }
}
