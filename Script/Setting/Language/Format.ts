namespace Sen.Script.Setting.Language {
    /**
     * Language Map Declaration
     */

    export type LanguageMap = Record<string, string>;

    /**
     * Language loaded by Kernel
     */

    export let language: LanguageMap = undefined!;

    /**
     * JavaScript custom formatter
     * @param str - string to format
     * @param args - arguments
     * @returns formatted string
     *
     */

    export function format(str: string, ...args: Array<any>): string {
        for (const arg of args) {
            const format_index: number = str.indexOf("{}");
            if (format_index !== -1) {
                str = str.replace("{}", arg.toString());
            }
        }
        return str;
    }

    /**
     * JavaScript Language Support
     * @param source - Source file to read
     * @returns Map
     */

    export function read_language(source: string): LanguageMap {
        return Sen.Kernel.JSON.deserialize_fs<LanguageMap>(source);
    }

    /**
     * JavaScript Language Support
     * @param property - Property to read
     * @returns if property find, a string will be returned. Else the property itself.
     */

    export function get(property: string): string {
        return language[property] || property;
    }
}
