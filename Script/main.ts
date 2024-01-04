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

        /**
         * --------------------------------------------------
         * JavaScript send output message
         * @param title - title to send
         * @param message - message to send
         * @param color - color to input
         * @returns The console output
         * --------------------------------------------------
         */

        export function display(title: string, message: any, color: Definition.Console.Color = Definition.Console.Color.DEFAULT): void {
            if (!Sen.Shell.is_gui) {
                Sen.Kernel.Console.print(`● ${title}:`, color);
                Sen.Kernel.Console.print(`    ${message}`);
            } else {
                Sen.Kernel.Console.print(title, color);
                Sen.Kernel.Console.print(message, color);
            }
            return;
        }

        /**
         * --------------------------------------------------
         * JavaScript send error message
         * @param str - string to send
         * @returns The console output
         * --------------------------------------------------
         */

        export function error(str: string | undefined): void {
            if (str !== undefined) {
                Sen.Script.Console.send(str, Sen.Script.Definition.Console.Color.RED);
            }
            return;
        }

        /**
         * --------------------------------------------------
         * JavaScript send argument message
         * @param str - string to send
         * @returns The console output
         * --------------------------------------------------
         */

        export function argument(str: any): void {
            display(Setting.Language.get(`execution_argument`), str, Definition.Console.Color.CYAN);
            return;
        }

        /**
         * --------------------------------------------------
         * JavaScript send finished message
         * @param str - string to send
         * @returns The console output
         * --------------------------------------------------
         */

        export function finished(subtitle: string, message: string): void {
            if (!Sen.Shell.is_gui) {
                Sen.Kernel.Console.print(`● ${Setting.Language.get(`execution_finished`)}: ${subtitle}`, Definition.Console.Color.GREEN);
                Sen.Kernel.Console.print(`    ${message}`);
            } else {
                Sen.Kernel.Console.print(`${Setting.Language.get(`execution_finished`)}: ${subtitle}`, Definition.Console.Color.GREEN);
                Sen.Kernel.Console.print(message);
            }
            return;
        }
    }

    /**
     * JavaScript Home
     */

    export namespace Home {
        /**
         * --------------------------------------------------
         * JS default path
         * --------------------------------------------------
         */

        export let participant: string = undefined!;

        /**
         * --------------------------------------------------
         * JavaScript setup current home
         * @returns Setup home
         * --------------------------------------------------
         */

        export function setup(): void {
            participant = Sen.Kernel.Path.dirname(Sen.Kernel.Home.script);
            return;
        }

        /**
         * --------------------------------------------------
         * JavaScript query current home
         * @param path - Path to query
         * @returns The resolved path
         * --------------------------------------------------
         */

        export function query(path: string): string {
            return Sen.Kernel.Path.resolve(path.replace(/^~(?=([/]|$))/gm, participant));
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

    export function main(): void {
        Sen.Script.Console.error(Sen.Script.launch());
        return;
    }

    /**
     * --------------------------------------------------
     * Main thread
     * @returns - result after execute
     * --------------------------------------------------
     */

    export function launch(): string {
        let result: string = undefined!;
        try {
            Sen.Script.Home.setup();
            Sen.Script.Module.load();
            Sen.Script.Console.send(`Sen ~ Shell: ${Sen.Shell.version} & Kernel: ${Sen.Kernel.version} & Script: ${Sen.Script.version} ~ ${Sen.Kernel.OperatingSystem.current()} & ${Sen.Kernel.OperatingSystem.architecture()}`);
            Sen.Script.Setting.load();
            Sen.Script.Console.finished(Sen.Script.Setting.Language.get(`current_status`), Sen.Script.Setting.Language.get(`script_has_been_loaded`));
            Sen.Script.Executor.run_as_module("data.md5.hash", { directory: `D:/test/g` }, Executor.Forward.BATCH);
        } catch (e: unknown & any) {
            result = `${Sen.Script.Setting.Language.get(`runtime_error`)}: `;
            result += e.message;
            result += "/n";
            result += e.stack;
        }
        return result;
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
                Sen.Kernel.JavaScript.evaluate_fs(Home.query(script));
            }
            return;
        }

        /**
         * Modules in queue await to be execute
         */

        export const script_list: Array<string> = [
            `~/Setting/Language/Format.js`,
            `~/Setting/Setting.js`,
            `~/utility/Definition.js`,
            `~/Support/Texture/Format.js`,
            `~/Support/PopCap/ResourceGroup/Convert.js`,
            `~/Executor/Executor.js`,
            `~/Executor/Methods/data.md5.hash.js`,
        ];
    }
}
