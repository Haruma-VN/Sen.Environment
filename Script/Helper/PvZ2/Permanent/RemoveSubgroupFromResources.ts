namespace Sen.Script.Helper.PvZ2.Permanent.RemoveSubgroupFromResources {


    export function rewrite_slot_count(
        resources: any
    ): void {
        let slot_index: bigint = 0n;
        const slot_group: Record<string, bigint> = {};
        for (let element of resources.groups) {
            if (element.resources !== undefined) {
                for (let c of element.resources) {
                    if (!slot_group.hasOwnProperty(c.id)) {
                        c.slot = slot_index;
                        slot_group[c.id] = slot_index++;
                    }
                    else {
                        c.slot = slot_group[c.id];
                    }
                }
            }
        }
        resources.slot_count = slot_index;
        return;
    }

    /**
     *
     * @param source - input
     * @param destination - output
     * @returns void
     */
    export function process(source: string, destination: string, keyword_list: Array<string>): void {
        const resources: any = Kernel.JSON.deserialize_fs(source);
        for (const keyword of keyword_list) {
            const regex: RegExp = new RegExp(`_${keyword}$`, "i");
            let index = 0;
            while (index < resources.groups.length) {
                const resource: any = resources.groups[index];
                if (resource.type === "composite") {
                    let subgroup_index = 0;
                    while (subgroup_index < resource.subgroups.length) {
                        const subgroup = resource.subgroups[subgroup_index];
                        if (regex.test(subgroup.id)) {
                            resources.groups[index].subgroups.splice(subgroup_index, 1);
                        }
                        else {
                            ++subgroup_index;
                        }
                    }
                    ++index;
                }
                else {
                    if (regex.test(resource.id)) {
                        resources.groups.splice(index, 1);
                    }
                    else {
                        ++index;
                    }
                }
            };
        }
        rewrite_slot_count(resources);
        Kernel.JSON.serialize_fs(destination, resources, 1, true);
        return;
    }

    /**
     *
     * @returns Evaluate
     */

    export function execute(): void {
        const keyword_list: Array<string> = ["384", "768"];
        const source: string = Console.path(Kernel.Language.get("script.orgainize_resources.source_file"), "file"); //TODO.
        process(source, source.replace(/((\.json))?$/i, ".trim.json"), keyword_list);
        return;
    }
}

Sen.Script.Helper.PvZ2.Permanent.RemoveSubgroupFromResources.execute();
