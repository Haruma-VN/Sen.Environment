namespace Sen.Script.Helper.Debugger.CheckiOSRSB {
    export interface SubInformation {
        width: bigint;
        height: bigint;
        format: bigint;
        path: string;
    }

    export function is_component_of_2(num: bigint): boolean {
        return (num & -num) === num;
    }

    export function process(manifest: Kernel.Support.PopCap.RSB.Manifest): Record<string, Array<SubInformation>> {
        const destination: Record<string, Array<SubInformation>> = {};
        Object.keys(manifest.group).forEach(function process_group(group: string) {
            if (!manifest.group[group].is_composite) {
                return;
            }
            Object.keys(manifest.group[group].subgroup).forEach(function process_subgroup(subgroup) {
                const current = manifest.group[group].subgroup[subgroup];
                current.packet_info.res.forEach(function process_resource(resource) {
                    if (resource.ptx_info === undefined) {
                        return;
                    }
                    if (destination[subgroup] === undefined) {
                        destination[subgroup] = [];
                    }
                    if (resource.ptx_info.width === resource.ptx_info.height && is_component_of_2(resource.ptx_info.height)) {
                        destination[subgroup].push({
                            path: resource.path,
                            width: resource.ptx_info.width,
                            height: resource.ptx_info.height,
                            format: resource.ptx_info.format,
                        });
                    }
                });
            });
        });
        Object.keys(destination).forEach(function process_array(e) {
            if (destination[e].empty()) {
                delete destination[e];
            }
        });
        return destination;
    }

    export function execute(): void {
        Kernel.JSON.serialize_fs(
            "D:/test/ZombieSkycityZombossGroup_1536.sprite/ipad3_10.8.1_main.rsb.mod_bundle/analyze.json",
            process(Kernel.JSON.deserialize_fs("D:/test/ZombieSkycityZombossGroup_1536.sprite/ipad3_10.8.1_main.rsb.mod_bundle/manifest.json")),
            1,
            false,
        );
        return;
    }
}
Sen.Script.Helper.Debugger.CheckiOSRSB.execute();
