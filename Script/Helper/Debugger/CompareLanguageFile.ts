namespace Sen.Script.Helper.Debugger.CompareLanguageFile {
    /**
     * Type of Language file
     */

    export type LanguageFile = Record<string, string>;

    /**
     *
     * @param source - Original language
     * @param destination - Modified language
     * @returns
     */

    export function process(source: LanguageFile, destination: LanguageFile): LanguageFile {
        const missing_key: LanguageFile = {};
        const source_keys: Array<string> = Object.keys(source);
        const destination_keys: Array<string> = Object.keys(destination);
        for (const e of source_keys) {
            if (destination_keys.includes(e)) {
                continue;
            }
            missing_key[e] = source[e];
        }
        return missing_key;
    }

    /**
     * Execute method
     */

    export function execute(): void {
        const source: string = "D:/Code/Sen.Environment/Script/Setting/Language/en-us.json";
        const destination: string = "D:/Code/Sen.Environment/Script/Setting/Language/vi-vn.json";
        const missing_key = process(Kernel.JSON.deserialize_fs<LanguageFile>(source), Kernel.JSON.deserialize_fs<LanguageFile>(destination));
        debug(missing_key);
        return;
    }
}
Sen.Script.Helper.Debugger.CompareLanguageFile.execute();
