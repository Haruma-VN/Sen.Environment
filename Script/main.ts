namespace Sen.Script {
    export function loadLibrary(): void {
        for (const script of mScripts) {
            Sen.Kernel.JavaScript.evaluate_fs(`${script}.js`);
        }
        return;
    }

    export function main(): void {
        Sen.Script.loadLibrary();
        Sen.Kernel.Console.print("", `${Kernel.Home.script_parent}`, Sen.Script.Color.RED);
        return;
    }

    export const mScripts: Array<string> = [...new Set([`${Kernel.Home.script_parent}/utility/color`])];
}
Sen.Script.main();
