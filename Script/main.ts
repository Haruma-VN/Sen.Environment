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

        export function send(str: string, color?: Color): void {
            if (color) {
                Sen.Kernel.Console.print(`● ${str}`, color);
            } else {
                Sen.Kernel.Console.print(`● ${str}`);
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

    export function main(): void {
        const before: number = Date.now();
        Sen.Script.loadLibrary();
        Sen.Script.Console.send(`Sen ~ Shell: ${Sen.Shell.version} & Kernel: ${Sen.Kernel.version} & Script: ${Sen.Script.version}`);
        Sen.Script.Console.send(`Modules have been loaded`, Console.Color.GREEN);
        const after: number = Date.now();
        Sen.Script.Console.send(`Time spent: ${((after - before) / 1000).toFixed(3)}s`, Console.Color.GREEN);
        return;
    }

    export const script_list: Array<string> = [...new Set([`utility/definition`])];
}
