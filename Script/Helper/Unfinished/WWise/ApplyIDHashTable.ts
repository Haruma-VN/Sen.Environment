namespace Sen.Script.Helper.WWise.ApplyIDHashTable {
    export function execute() {

        const source: string = Console.path(Kernel.Language.get("script.helper.wwise.apply_id_hash_table.input_soundbank_data_path"), "file");
        const table_source: string = Console.path(Kernel.Language.get("script.helper.wwise.apply_id_hash_table.input_id_table_json_path"), "file");
        const data: any = Kernel.JSON.deserialize_fs(source);
        const table: Record<string, string> = Kernel.JSON.deserialize_fs(table_source);
        if (data["hierarchy"] !== null) {
            for (let i in data["hierarchy"]) {
                const e = data["hierarchy"][i];
                if (table.hasOwnProperty(`${e["id"]}`)) {
                    data["hierarchy"][i] = {
                        "#name": table[`${e["id"]}`],
                        ...e
                    } 
                }
            }
        }
        Kernel.JSON.serialize_fs(`${Kernel.Path.except_extension(source)}.hash.json`, data, 1, true);
        return;
    }
}
Sen.Script.Helper.WWise.ApplyIDHashTable.execute();
