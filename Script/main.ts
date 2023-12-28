namespace Sen.Script {
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
        const before: number = Sen.Kernel.Thread.now();
        Sen.Script.Module.load();
        Sen.Script.Setting.load();
        Sen.Script.Console.send(`Sen ~ Shell: ${Sen.Shell.version} & Kernel: ${Sen.Kernel.version} & Script: ${Sen.Script.version} ~ ${Sen.Kernel.OperatingSystem.current()} & ${Sen.Kernel.OperatingSystem.architecture()}`);
        Sen.Script.Console.send(Sen.Script.Setting.Language.get("js_has_been_loaded"), Sen.Script.Definition.Console.Color.GREEN);
        const after: number = Sen.Kernel.Thread.now();
        Sen.Script.Console.send(`${Sen.Script.Setting.Language.get("execution_time")}: ${(after - before).toFixed(3)}s`, Sen.Script.Definition.Console.Color.GREEN);
        return;
    }

    /**
     * JavaScript Modules
     */

    export namespace Module {
        /**
         * --------------------------------------------------
         * All JS modules need to be initialized
         * @returns
         * --------------------------------------------------
         */

        export function load(): void {
            for (const script of script_list) {
                Sen.Kernel.JavaScript.evaluate_fs(`${Sen.Kernel.Home.script_parent}/${script}.js`);
            }
            return;
        }

        /**
         * Modules in queue await to be execute
         */

        export const script_list: Array<string> = [`Setting/Language/format`, `Setting/Setting`, `utility/definition`, `Support/Texture/Format`, `Support/PopCap/ResourceGroup/Convert`];
    }
}
