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
        const source: string = Console.path(Kernel.Language.get("script.sort_language_file.input_language_file"), "file");
        Sen.Kernel.JSON.serialize_fs(source, process(Kernel.JSON.deserialize_fs<LanguageFile>(source)), 1, false);
        return;
    }
}
Sen.Script.Helper.Debugger.SortLanguageFile.execute();
