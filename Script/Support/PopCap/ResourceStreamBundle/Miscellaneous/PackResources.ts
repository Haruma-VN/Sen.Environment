namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Miscellaneous.PackResources {
    export type Manifest = Kernel.Support.PopCap.RSB.Manifest;

    export type PacketInfo = Kernel.Support.PopCap.RSG.Definition;

    export type DataStreamView = Kernel.DataStreamView;

    export function pack_rsg(source: string, manifest: Manifest): void {
        const version = manifest.version as Kernel.Support.PopCap.RSG.Version;
        const groups = Object.keys(manifest.group);
        Kernel.FileSystem.create_directory(`${source}/packet`);
        for (const group of groups) {
            const subgroups = Object.keys(manifest.group[group].subgroup);
            for (const subgroup of subgroups) {
                const packet_info: PacketInfo = {
                    version,
                    ...manifest.group[group].subgroup[subgroup].packet_info,
                };
                Kernel.Support.PopCap.RSG.pack(`${source}/resource`, `${source}/packet/${subgroup}.rsg`, packet_info);
            }
        }
        return;
    }

    export function process(source: string, destination: string, manifest: Manifest): void {
        pack_rsg(source, manifest);
        Kernel.Support.PopCap.RSB.pack(source, destination, manifest);
        return;
    }

    export function process_fs(source: string, destination: string): void {
        const manifest = Kernel.JSON.deserialize_fs(`${source}/manifest.json`) as Manifest;
        process(source, destination, manifest);
        return;
    }
}
