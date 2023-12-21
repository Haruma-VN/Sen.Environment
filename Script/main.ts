namespace Sen.Script {
    export function main(): void {
        Sen.Kernel.JavaScript.evaluate_fs("D:/Code/Sen.Environment/Script/build/utility/color.js");
        Sen.Kernel.Console.print("", `${Sen.Script.Color.CYAN}`, Sen.Script.Color.CYAN);
        return;
    }
}
Sen.Script.main();
