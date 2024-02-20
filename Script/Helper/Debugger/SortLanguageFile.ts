namespace Sen.Script.Helper.Debugger.SortLanguageFile {
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

    export function process(source: LanguageFile): LanguageFile {
        const destination: LanguageFile = {};
        const keys = Object.keys(source);
        keys.sort();
        keys.forEach((e) => (destination[e] = source[e]));
        return destination;
    }

    /**
     * Execute method
     */

    export function execute(): void {
        const source: string = "D:/Code/Sen.Environment/Script/Setting/Language/en-us.json";
        Sen.Kernel.JSON.serialize_fs(source, process(Kernel.JSON.deserialize_fs<LanguageFile>(source)), 1, false);
        return;
    }
}
Sen.Script.Helper.Debugger.SortLanguageFile.execute();
