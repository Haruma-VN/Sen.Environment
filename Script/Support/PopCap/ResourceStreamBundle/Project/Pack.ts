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

    export type BasicCommand = Configuration.BasicCommand;

    export class Home {
        private participant: string;

        public constructor(participant: string) {
            this.participant = participant;
        }

        public query(path: string): string {
            return path.replace(/^\.(\/|\\)/gm, this.participant);
        }
    }

    export function process(source: string, destination: string): void {
        const manifest = Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.RSB.Manifest>(`${source}/manifest.json`);
        const configuration = Kernel.JSON.deserialize_fs<Setting>(`${source}/setting.json`);
        const home = new Home(source);
        remake_manifest(manifest);
        configuration.commands.forEach(function process_command(e: BasicCommand): void {
            switch (e.command) {
                case "popcap.rsb.pack": {
                    const process_whole = (manifest: Kernel.Support.PopCap.RSB.Manifest, destination: string): void => {
                        // to do : add localization
                        assert(e.source !== undefined, "source must be provided for packing command");
                        Kernel.Support.PopCap.RSB.pack(home.query(e.source as string), home.query(destination), manifest);
                    };
                    Object.keys(configuration.rsb.distribution).forEach(function process(resolution: string): void {
                        if (!((configuration.rsb.distribution as any)[resolution] as boolean)) {
                            return;
                        }
                        if (resolution === "all") {
                            Console.finished("making rsb for full resolution");
                            return process_whole(manifest, home.query(destination));
                        } else {
                            const new_manifest: Kernel.Support.PopCap.RSB.Manifest = {
                                version: 4n,
                                ptx_info_size: manifest.ptx_info_size,
                                group: {},
                            };
                            Console.finished(`making rsb for ${resolution} resolution`);
                            generate_special_manifest(manifest, new_manifest, BigInt(resolution));
                            return process_whole(new_manifest, `${home.query(destination)}.${resolution}`);
                        }
                    });
                    break;
                }
            }
        });
        return;
    }
}
