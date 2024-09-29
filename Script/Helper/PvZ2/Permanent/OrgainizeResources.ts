namespace Sen.Script.Helper.PvZ2.Permanent.OrgainizeResources {
     /**
     * Structure
     */
     export interface OrganizeOrder {
        slot: bigint;
        id: bigint;
        path: bigint;
        type: bigint;
        atlas: bigint;
        width: bigint;
        height: bigint;
        parent: bigint;
        ah: bigint;
        aw: bigint;
        ax: bigint;
        ay: bigint;
        cols: bigint;
        x: bigint;
        y: bigint;
        srcpath: bigint;
        runtime: bigint;
        forceOriginalVectorSymbolSize: bigint;
    }

    /**
     * Edit this to change order
     */

    export const Order: OrganizeOrder = {
        slot: 1n,
        id: 2n,
        path: 3n,
        type: 4n,
        atlas: 5n,
        width: 6n,
        height: 7n,
        parent: 8n,
        ah: 9n,
        aw: 10n,
        ax: 11n,
        ay: 12n,
        cols: 13n,
        x: 14n,
        y: 15n,
        srcpath: 16n,
        runtime: 17n,
        forceOriginalVectorSymbolSize: 18n,
    };


    export interface Resource_Structure_Template {
        id: string;
        parent: string;
        res?: string;
        resources: Array<{
            slot: number;
            id: string;
            path: Array<string> | string;
            type?: string;
            atlas?: boolean;
            width?: number;
            height?: number;
            runtime?: boolean;
            parent?: string;
            ah?: number;
            aw?: number;
            ax?: number;
            ay?: number;
            x?: number;
            y?: number;
            forceOriginalVectorSymbolSize?: boolean;
            cols?: number;
            srcpath?: string | Array<string>;
        }>;
        type: string;
    };

    /**
         * Structure
         */

    export interface Resources_Group_Structure_Template {
        groups: Array<any & Resource_Structure_Template>;
        slot_count: number;
    };


    export function orgainize_resources_content(res: Resource_Structure_Template): void {
        const organize_property = Object.keys(Order).sort((a: string, b: string) =>
            Number((Order as any)[a] - (Order as any)[b])
        );
        res.resources.forEach((e, i) => {
            const reorganizedObject: any = {};
            for (const prop of organize_property) {
                if (e.hasOwnProperty(prop)) {
                    reorganizedObject[prop] = (e as any)[prop];
                }
            }
            res.resources[i] = reorganizedObject;
        });
        res.resources.sort((a: any, b: any) => a.id.localeCompare(b.id));
        return;
    }

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
    export function process(source: string, destination: string): void {
        const resources: Resources_Group_Structure_Template = Kernel.JSON.deserialize_fs(source);
        resources.groups.sort((a: Resource_Structure_Template, b: Resource_Structure_Template) => a.id.localeCompare(b.id));
        resources.groups.forEach((res: any) => {
            if (`resources` in res) {
                orgainize_resources_content(res);
            }
        });
        rewrite_slot_count(resources);
        Kernel.JSON.serialize_fs(destination, resources, 1, true);
        return;
    }

    /**
     *
     * @returns Evaluate
     */

    export function execute(): void {
        const source: string = Console.path(Kernel.Language.get("script.orgainize_resources.source_file"), "file"); //TODO.
        process(source, source.replace(/((\.json))?$/i, ".orgainize.json"));
        return;
    }
}

Sen.Script.Helper.PvZ2.Permanent.OrgainizeResources.execute();
