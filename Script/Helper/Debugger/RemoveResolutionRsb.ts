namespace Sen.Script.Helper.Debugger.RemoveResolutionRSB {
    export function execute() {
        const source: string = Console.path("input_folder", "directory");
        const destination = `${source}.bin`;
        const bundle: any = Kernel.JSON.deserialize_fs(`${source}/data.json`);
        for (let [g_id, g_value] of Object.entries(bundle.group) as any) {
            if (g_value.composite) {
                for (let [s_id, s_value] of Object.entries(g_value.subgroup) as any) {
                    if (s_id.endsWith("_768")) {
                       delete bundle.group[g_id].subgroup[s_id];
                    }
                    if (s_id.endsWith("_384")) {
                        delete bundle.group[g_id].subgroup[s_id];
                     }
                }
            }
        }
        Kernel.JSON.serialize_fs(`${source}/data.json`, bundle, 1, true);
        const mainfest = `${source}/packet/__MANIFESTGROUP__.rsg`;
        Kernel.Support.PopCap.RSG.unpack_fs(mainfest, mainfest + ".temp");
        Kernel.Support.PopCap.RTON.decode_fs(`${mainfest}.temp/resource/PROPERTIES/RESOURCES.RTON`, `${mainfest}.temp/resource/PROPERTIES/RESOURCES.JSON`);
        Kernel.Support.PopCap.ResourceGroup.split_fs(`${mainfest}.temp/resource/PROPERTIES/RESOURCES.JSON`, `${mainfest}.temp/resource/PROPERTIES/RESOURCES.split`);
        const data: any = Kernel.JSON.deserialize_fs(`${mainfest}.temp/resource/PROPERTIES/RESOURCES.split/data.json`);
        for (let g_id of Object.keys(data) as any) {
            if (data[g_id].is_composite) {
                for (let [s_id, s_value] of Object.entries(data[g_id].subgroups) as any) {
                    if (s_id.endsWith("_768")) {
                       delete data[g_id].subgroups[s_id];
                    }
                    if (s_id.endsWith("_384")) {
                        delete data[g_id].subgroups[s_id];
                     }
                }
            }
        }
        Kernel.JSON.serialize_fs(`${mainfest}.temp/resource/PROPERTIES/RESOURCES.split/data.json`, data, 1, true);
        Kernel.Support.PopCap.ResourceGroup.merge_fs(`${mainfest}.temp/resource/PROPERTIES/RESOURCES.split`, `${mainfest}.temp/resource/PROPERTIES/RESOURCES.JSON`);
        Kernel.Support.PopCap.RTON.encode_fs(`${mainfest}.temp/resource/PROPERTIES/RESOURCES.JSON`, `${mainfest}.temp/resource/PROPERTIES/RESOURCES.RTON`);
        Kernel.Support.PopCap.RSG.pack_fs(mainfest + ".temp", mainfest  );
        return;
    }
}
Sen.Script.Helper.Debugger.RemoveResolutionRSB.execute();
