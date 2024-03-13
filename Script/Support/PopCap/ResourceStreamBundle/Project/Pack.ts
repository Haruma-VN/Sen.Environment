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
            subgroups.forEach((subgroup: string) =>
                manifest.group[group].subgroup[subgroup].packet_info.res.forEach((e) => {
                    if (e.ptx_info) {
                        e.ptx_info.pitch = calculate_pitch(e.ptx_info.format, e.ptx_info.width);
                    }
                }),
            );
        }
        return;
    }

    export function resource_convert(source: string, destination: string, is_resinfo: boolean): void {
        if (is_resinfo) {
            Kernel.Support.PopCap.ResInfo.convert_fs(source, destination);
            return;
        }
        Kernel.Support.PopCap.ResourceGroup.convert_fs(source, destination, Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.ResInfo.Structure>(destination).expand_path);
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

    export function generate_special_manifest(source: Kernel.Support.PopCap.RSB.Manifest, destination: Kernel.Support.PopCap.RSB.Manifest): void {
        Object.keys(source).forEach((e) => {});
        return;
    }

    export function process(source: string, destination: string): void {
        const manifest = Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.RSB.Manifest>(`${source}/manifest.json`);
        const configuration = Kernel.JSON.deserialize_fs<Configuration.Setting>(`${source}/setting.json`);
        remake_manifest(manifest);
        // to do : handle configuration
        Kernel.Support.PopCap.RSB.pack(source, destination, manifest);
        return;
    }
}
