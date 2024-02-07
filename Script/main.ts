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
                Sen.Kernel.Console.print(`● ${str}`, "", color);
            } else {
                Sen.Kernel.Console.print(str, "", color);
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
                Sen.Kernel.Console.print(`● ${title}:`, "", color);
                Sen.Kernel.Console.print(`    ${message}`);
            } else {
                Sen.Kernel.Console.print(title, "", color);
                Sen.Kernel.Console.print(message);
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
                Sen.Script.Console.send(`${Sen.Kernel.Language.get("runtime_error")}: ${str}`, Sen.Script.Definition.Console.Color.RED);
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
            send(`${Sen.Kernel.Language.get("execution_argument")}: ${str}`, Definition.Console.Color.CYAN);
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
                Sen.Kernel.Console.print(`● ${Sen.Kernel.Language.get(`execution_finished`)}: ${subtitle}`, "", Definition.Console.Color.GREEN);
                if (message) {
                    Sen.Kernel.Console.print(`    ${message}`);
                }
            } else {
                Sen.Kernel.Console.print(`${Sen.Kernel.Language.get(`execution_finished`)}: ${subtitle}`, "", Definition.Console.Color.GREEN);
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
            Sen.Script.Console.display(Sen.Kernel.Language.get("input_argument"), source, Definition.Console.Color.CYAN);
        }

        /**
         * --------------------------------------------------
         * JavaScript notify
         * @param source - source to send
         * @returns The console output
         * --------------------------------------------------
         */

        export function output(source: string): void {
            Sen.Script.Console.display(Sen.Kernel.Language.get("output_argument"), source, Definition.Console.Color.GREEN);
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
     * JS exception
     */

    export namespace Exception {
        /**
         * --------------------------------------------------
         * JS make stack, it can catch c++ code too
         * @param stack - current stack
         * @returns newly stack
         * --------------------------------------------------
         */

        export function make_stack(stack: string): string {
            return stack
                .replace(/(\s)at(\s)/g, ` ${Kernel.Language.get("at")} `)
                .replace(/\(native\)/gm, "(<native>:?)")
                .replace(/(?<=\()(.*)(?=(Kernel|Script))/gm, "")
                .replaceAll("\\", "/")
                .split("\n")
                .filter((e: string) => !/(\s)<eval>(\s)/g.test(e))
                .join("\n");
        }

        export function make_exception(e: Error): string {
            let result: string = e.message;
            result += "\n";
            result += Exception.make_stack(e.stack!);
            result = result.replace(/\n$/, "");
            return result;
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
        const result: string = Sen.Script.launch();
        Console.error(result);
        Sen.Script.Console.finished(Sen.Kernel.Language.get("method_are_succeeded"));
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
            Kernel.arguments.splice(0, 3);
            Sen.Script.Home.setup();
            Sen.Script.Module.load();
            Console.send(`Sen ~ Shell: ${Shell.version} & Kernel: ${Kernel.version} & Script: ${Script.version} ~ ${Kernel.OperatingSystem.current()} & ${Kernel.OperatingSystem.architecture()}`);
            Sen.Script.Setting.load();
            Sen.Script.Console.finished(Sen.Kernel.Language.get("current_status"), format(Sen.Kernel.Language.get("js.environment_has_been_loaded"), 1n, 1n, Module.script_list.length + 1));
            // Executor.execute<Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.BatchArgument>(
            //     { source: undefined, directory: "D:/test/test/New Folder/test" },
            //     "popcap.animation.to_flash",
            //     Executor.Forward.BATCH,
            // );
            // Kernel.FileSystem.write_file("C:/Users/HarumaVN/Downloads/LAWNSTRINGS1.TXT", utf16);
            Kernel.arguments.forEach((source: string) => {
                Sen.Script.Executor.load_module({ source });
            });
            // Kernel.Support.WWise.SoundBank.decode_fs(
            //     "D:/test/ipad3_10.8.1_main.rsb.bundle/packet/WiseAlwaysLoaded.packet/res/SOUNDBANKS/GLOBAL_DATA.BNK",
            //     "D:/test/ipad3_10.8.1_main.rsb.bundle/packet/WiseAlwaysLoaded.packet/res/SOUNDBANKS/GLOBAL_DATA.soundbank",
            // );
            // Support.PopCap.Animation.FromAnimation.process_fs(
            //     "D:/test/ipad3_10.8.1_main.rsb.bundle/packet/PlantPrimalSunflower_Common.packet/res/IMAGES/1536/FULL/PLANT/PRIMAL_SUNFLOWER.json",
            //     "D:/test/ipad3_10.8.1_main.rsb.bundle/packet/PlantPrimalSunflower_Common.packet/res/IMAGES/1536/FULL/PLANT/PRIMAL_SUNFLOWER.xfl",
            //     1536n,
            // );
            // Sen.Script.Support.PopCap.Atlas.Pack.ResourceGroup.process_fs(
            //     "D:/test/ZombieSkycityZombossGroup_1536.sprite",
            //     {
            //         height: 4096,
            //         width: 4096,
            //         padding: 1,
            //     },
            //     {
            //         allowRotation: false,
            //         pot: false,
            //         smart: true,
            //         square: false,
            //     },
            //     "D:/test/test",
            // );
            // Sen.Script.Executor.run_as_module<Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup.Argument>(
            //     "popcap.atlas.split_by_resource_group",
            //     {
            //         source: ["D:/test/ZombieSkycityZombossGroup_1536.json", "D:/test/ZOMBIESKYCITYZOMBOSSGROUP_1536_00.png", "D:/test/ZOMBIESKYCITYZOMBOSSGROUP_1536_01.png"],
            //         destination: "D:/test/ZombieSkycityZombossGroup_1536.sprite",
            //         method: "id",
            //         style: "array",
            //     },
            //     Executor.Forward.DIRECT,
            // );
        } catch (e: unknown & any) {
            result = Exception.make_exception(e);
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
            "~/Third/maxrects-packer/maxrects-packer.js",
            "~/utility/Miscellaneous.js",
            "~/Setting/Setting.js",
            "~/utility/Definition.js",
            "~/utility/Clock.js",
            "~/Support/Texture/Format.js",
            "~/Support/PopCap/ResourceGroup/Convert.js",
            "~/Support/PopCap/Atlas/Structure.js",
            "~/Support/PopCap/Atlas/Split.js",
            "~/Support/PopCap/Atlas/Pack.js",
            "~/Support/PopCap/Animation/Definition.js",
            "~/Support/PopCap/Animation/Structure.js",
            "~/Support/PopCap/Animation/FromAnimation.js",
            "~/Support/PopCap/Animation/ToAnimation.js",
            "~/Support/PopCap/LawnStrings/Convert.js",
            "~/Executor/Executor.js",
            "~/Executor/Methods/js.evaluate.js",
            "~/Executor/Methods/data.md5.hash.js",
            "~/Executor/Methods/data.base64.encode.js",
            "~/Executor/Methods/data.base64.decode.js",
            "~/Executor/Methods/popcap.rton.decode.js",
            "~/Executor/Methods/popcap.rton.encode.js",
            "~/Executor/Methods/popcap.newton.decode.js",
            "~/Executor/Methods/popcap.newton.encode.js",
            "~/Executor/Methods/popcap.animation.decode.js",
            "~/Executor/Methods/popcap.animation.encode.js",
            "~/Executor/Methods/popcap.animation.to_flash.js",
            "~/Executor/Methods/popcap.animation.from_flash.js",
            "~/Executor/Methods/wwise.soundbank.decode.js",
            "~/Executor/Methods/wwise.soundbank.encode.js",
            "~/Executor/Methods/popcap.atlas.split_by_resource_group.js",
        ];
    }
}
