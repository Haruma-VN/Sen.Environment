namespace Sen.Script {
    export function loadLibrary(): void {
        for (const script of mScripts) {
            Sen.Kernel.JavaScript.evaluate_fs(`${script}.js`);
        }
        return;
    }

    export function main(): void {
        let before = Date.now();
        Sen.Script.loadLibrary();
        Sen.Kernel.Compression.Zlib.compress_fs("D:/test/res.json", "D:/test/res.json.bin", Zlib.Level.LEVEL_9);
        let after = Date.now();
        Sen.Kernel.Console.print(`Time spent: ${((after - before) / 1000).toPrecision(3)}s`);
        return;
    }

    export const mScripts: Array<string> = [...new Set([`${Kernel.Home.script_parent}/utility/definition`])];
}
