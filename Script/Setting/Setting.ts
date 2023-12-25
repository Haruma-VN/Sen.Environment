namespace Sen.Script.Setting {
    /**
     * Configuration file
     */

    export const settingFile: string = `${Sen.Kernel.Home.script_parent}/Setting/Setting.json`;

    export interface Entry {
        language: string;
    }

    export let setting: Entry = undefined!;

    /**
     * Load method
     */

    export function load(): void {
        setting = Sen.Kernel.JSON.deserialize_fs<Entry>(settingFile);
        Sen.Script.Setting.Language.language = Sen.Script.Setting.Language.read_language(`${Sen.Kernel.Home.script_parent}/Setting/Language/${setting.language}.json`);
        return;
    }
}
