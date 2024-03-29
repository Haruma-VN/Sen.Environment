namespace Sen.Script.Setting {
    /**
     * --------------------------------------------------
     * JavaScript current configuration file location
     * @abstract: Configuration file location
     * --------------------------------------------------
     */

    export const setting_file: string = Sen.Script.Home.query(`~/Setting/Setting.json`);

    /**
     * --------------------------------------------------
     * JavaScript Configuration file Entry
     * --------------------------------------------------
     */

    export interface Entry {
        language: string;
        thread_limit_count: bigint;
    }

    /**
     * --------------------------------------------------
     * JavaScript current configuration file
     * @abstract: Configuration file
     * --------------------------------------------------
     */

    export let setting: Entry = undefined!;

    /**
     * --------------------------------------------------
     * JavaScript load configuration file method
     * @returns: Load method
     * --------------------------------------------------
     */

    export function load(): void {
        setting = Sen.Kernel.JSON.deserialize_fs<Entry>(setting_file);
        Sen.Kernel.Language.load_language(Sen.Script.Home.query(`~/Setting/Language/${setting.language}.json`));
        return;
    }
}
