namespace Sen.Script {
    /**
     * --------------------------------------------------
     * All JS modules need to be initialized
     * @returns
     * --------------------------------------------------
     */

    export function loadLibrary(): void {
        for (const script of script_list) {
            Sen.Kernel.JavaScript.evaluate_fs(`${Kernel.Home.script_parent}/${script}.js`);
        }
        return;
    }

    /**
     * Console namespace of Script
     */

    export namespace Console {
        /**
         * --------------------------------------------------
         * JavaScript send output message
         * @param str - string to send
         * @param color - color to input
         * @returns The console output
         * --------------------------------------------------
         */

        export function send(str: any, color: Definition.Console.Color = Definition.Console.Color.DEFAULT): void {
            if (!Sen.Shell.is_gui) {
                Sen.Kernel.Console.print(`● ${str}`, color);
            } else {
                Sen.Kernel.Console.print(str, color);
            }
            return;
        }
    }

    /**
     * --------------------------------------------------
     * Script version
     * --------------------------------------------------
     */

    export const version = 1 as const;

    /**
     * --------------------------------------------------
     * Main function
     * @returns
     * --------------------------------------------------
     */

    export async function main(): Promise<void> {
        const before: number = Date.now();
        Sen.Script.loadLibrary();
        Sen.Script.Setting.load();
        Sen.Script.Console.send(
            `Sen ~ Shell: ${Sen.Shell.version} & Kernel: ${Sen.Kernel.version} & Script: ${Sen.Script.version}`,
        );
        Sen.Script.Console.send(Setting.Language.get("js_has_been_loaded"), Definition.Console.Color.GREEN);
        const after: number = Date.now();
        Sen.Script.Console.send(
            `${Setting.Language.get("execution_time")}: ${((after - before) / 1000).toFixed(3)}s`,
            Definition.Console.Color.GREEN,
        );
        return;
    }

    /**
     * Modules in queue await to be execute
     */

    export const script_list: Array<string> = [
        ...new Set([
            `Setting/Language/format`,
            `Setting/Setting`,
            `utility/definition`,
            `Support/Texture/Format`,
            `Support/PopCap/ResourceGroup/Convert`,
        ]),
    ];
}
