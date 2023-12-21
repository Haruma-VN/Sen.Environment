namespace Sen.Script {
    const home: string = "D:/Code/Sen.Environment/Script/build";

    export function loadLibrary(): void {
        for (const script of mScripts) {
            Sen.Kernel.JavaScript.evaluate_fs(`${script}.js`);
        }
        return;
    }

    export function main(): void {
        Sen.Script.loadLibrary();
        Sen.Kernel.Console.print("", `${Sen.Kernel.Encryption.MD5.hash("com_popcap_pvz2_magento_product_2013_05_05")}`, Sen.Script.Color.RED);
        return;
    }

    export const mScripts = [...new Set([`${home}/utility/color`])] as const;
}
Sen.Script.main();
