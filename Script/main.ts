namespace Sen.Script {
    /**
     * --------------------------------------------------
     * All JS modules need to be initialized
     * @returns
     * --------------------------------------------------
     */

    export function loadLibrary(): void {
        for (const script of mScripts) {
            Sen.Kernel.JavaScript.evaluate_fs(`${Kernel.Home.script_parent}/${script}.js`);
        }
        return;
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
        Sen.Kernel.Console.print(`Sen ~ Kernel: ${Sen.Kernel.version} & Script: ${Sen.Script.version}`);
        const after: number = Date.now();
        Sen.Kernel.Console.print(`Time spent: ${((after - before) / 1000).toPrecision(3)}s`, Console.Color.GREEN);
        return;
    }

    export const mScripts: Array<string> = [...new Set([`utility/definition`])];
}
