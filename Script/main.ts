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
                Sen.Kernel.Console.print(`● ${str}`, ``, color);
            } else {
                Sen.Kernel.Console.print(str, ``, color);
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
                Sen.Kernel.Console.print(`● ${title}:`, ``, color);
                Sen.Kernel.Console.print(`    ${message}`);
            } else {
                Sen.Kernel.Console.print(title, ``, color);
                Sen.Kernel.Console.print(message, ``, color);
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
                Sen.Script.Console.send(`${Sen.Script.Setting.Language.get(`runtime_error`)}: ${str}`, Sen.Script.Definition.Console.Color.RED);
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

        export function finished(subtitle: string, message?: string): void {
            if (!Sen.Shell.is_gui) {
                Sen.Kernel.Console.print(`● ${Setting.Language.get(`execution_finished`)}: ${subtitle}`, ``, Definition.Console.Color.GREEN);
                if (message) {
                    Sen.Kernel.Console.print(`    ${message}`);
                }
            } else {
                Sen.Kernel.Console.print(`${Setting.Language.get(`execution_finished`)}: ${subtitle}`, ``, Definition.Console.Color.GREEN);
                if (message) {
                    Sen.Kernel.Console.print(message);
                }
            }
            return;
        }

        /**
         * --------------------------------------------------
         * JavaScript notify
         * @param source - source to send
         * @returns The console output
         * --------------------------------------------------
         */

        export function obtained(source: string): void {
            Sen.Script.Console.display(Setting.Language.get(`input_argument`), source, Definition.Console.Color.CYAN);
        }

        /**
         * --------------------------------------------------
         * JavaScript notify
         * @param source - source to send
         * @returns The console output
         * --------------------------------------------------
         */

        export function output(source: string): void {
            Sen.Script.Console.display(Setting.Language.get(`output_argument`), source, Definition.Console.Color.GREEN);
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
        let result = Sen.Script.launch();
        Console.error(result);
        Sen.Script.Console.finished(Sen.Script.Setting.Language.get(`method_are_succeeded`));
        return;
    }

    /**
     * --------------------------------------------------
     * Main thread
     * @returns - result after execute
     * --------------------------------------------------
     */

    //  "AS23DSREPLKL335KO4439032N8345NF"

    export function launch(): string {
        let result: string = undefined!;
        try {
            Sen.Script.Home.setup();
            Sen.Script.Module.load();
            Sen.Script.Console.send(
                `Sen ~ Shell: ${Shell.version} & Kernel: ${Kernel.version} & Script: ${Script.version} ~ ${Kernel.OperatingSystem.current()} & ${Kernel.OperatingSystem.architecture()}`,
            );
            Sen.Script.Setting.load();
            Sen.Script.Console.finished(Sen.Script.Setting.Language.get(`current_status`), Sen.Script.Setting.Language.get(`script_has_been_loaded`));
            const before = Date.now();
            Sen.Kernel.FileSystem.read_file("D:/test/RESOURCES.json");
            const after = Date.now();
            Script.Console.send(`Duration: ${(after - before) / 1000}s`);
        } catch (e: unknown & any) {
            result = e.message;
            result += `\n`;
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
            `~/utility/Clock.js`,
            `~/Support/Texture/Format.js`,
            `~/Support/PopCap/ResourceGroup/Convert.js`,
            `~/Executor/Executor.js`,
            `~/Executor/Methods/data.md5.hash.js`,
            `~/Executor/Methods/data.base64.encode.js`,
            `~/Executor/Methods/data.base64.decode.js`,
        ];
    }
}
