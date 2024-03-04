namespace Sen.Script.Helper.PvZ2.Permanent.ErasePlantLevels {
    /**
     * @todo Update this for official
     */
    export interface PlantLevels extends Record<string, unknown> {
        objects: Array<{
            objdata: {
                UsesLeveling?: boolean;
                LevelCoins?: Array<bigint>;
                LevelXP?: Array<bigint>;
                LevelCap?: bigint;
                PlantTier?: Array<bigint>;
            };
        }>;
    }

    /**
     *
     * @param obj Object
     */

    export function erase_of(obj: Record<string, unknown>) {
        const keys = Object.keys(obj);
        keys.forEach((e) => {
            if (is_actual_object(obj[e])) {
                erase_of(obj[e] as Record<string, unknown>);
            }
            if (is_array(obj[e])) {
                (obj[e] as Array<unknown>).length = 1;
                if (is_actual_object((obj[e] as any)[0])) {
                    erase_of((obj[e] as Array<unknown>)[0] as Record<string, unknown>);
                }
            }
        });
    }

    /**
     *
     * @param source - input
     * @param ripe - output
     * @returns Erased
     */
    export function process(source: string, ripe: string): void {
        const json: PlantLevels = Kernel.JSON.deserialize_fs(source);
        for (const obj of json.objects) {
            obj.objdata.LevelCap = 1n;
            obj.objdata.UsesLeveling = false;
            erase_of(obj.objdata);
            obj.objdata.LevelXP = [];
            obj.objdata.LevelCoins = [];
        }
        Kernel.JSON.serialize_fs<PlantLevels>(ripe, json, 1, false);
        return;
    }

    /**
     *
     * @returns Evaluate
     */

    export function execute(): void {
        // todo: add
        const source: string = "D:/test/ZombieSkycityZombossGroup_1536.sprite/ipad3_10.8.1_main.rsb.bundle/resource/PLANTLEVELS.json";
        process(source, source.replace(/((\.json))?$/i, ".patch.json"));
        return;
    }
}

Sen.Script.Helper.PvZ2.Permanent.ErasePlantLevels.execute();
