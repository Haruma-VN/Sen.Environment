namespace Sen.Script.Helper.PvZ2.WorldMapData.Merge {
    /**
     *
     * @param source - source directory
     * @returns
     */

    export function process(source: string): Split.WorldMapData {
        const result: WorldMapData.Split.WorldMapData = {
            version: 1n,
            objects: [],
        };
        const definition: Split.Definition = Kernel.JSON.deserialize_fs<Split.Definition>(`${source}/map_list.json`);
        for (const map_name of Object.keys(definition.objects)) {
            const data = Kernel.JSON.deserialize_fs<Split.Object>(`${source}/worldmap/${map_name}.json`);
            data.objdata.m_worldName = map_name;
            data.objdata.m_resGroupID = definition.objects[map_name].m_resGroupID;
            data.objdata.m_worldId = definition.objects[map_name].m_worldId;
            result.objects.push(data);
        }
        return result;
    }

    /**
     * Evaluate method
     * @returns
     */

    export function execute(): void {
        // todo
        const source: string = "";
        Kernel.JSON.serialize_fs<Split.WorldMapData>(`${source}/WorldMapData.json`, process(source), 1, false);
        return;
    }
}
Sen.Script.Helper.PvZ2.WorldMapData.Merge.execute();
