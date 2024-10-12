namespace Sen.Script.Executor.Methods.PopCap.RSB.Watch {
    // Generic

    export type Generic = Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.Generic;

    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        generic?: Generic;
        key?: string;
        iv?: string;
        manifest?: boolean;
        packages?: boolean;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
    }

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {
        packages_setting: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.PackagesSetting;
        compression_setting: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.CompressionSetting;
    }

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export interface PackagesFileList {
        rton_file: string[];
        json_file: string[];
    }

    export function current_date() {
        const date = new Date();
        const month = (date.getMonth() + 1).toString().padStart(2, "0");
        const day = date.getDate().toString().padStart(2, "0");
        const year = date.getFullYear();
        let hours = date.getHours();
        const minutes = date.getMinutes().toString().padStart(2, "0");
        const ampm = hours >= 12 ? "PM" : "AM";
        hours = hours % 12 || 12;
        const formattedHours = hours.toString().padStart(2, "0");
        return `${month}-${day}-${year}-${formattedHours}-${minutes}-${ampm}`;
    }

    export function backup_content(source: string, destination: string) {
        Kernel.FileSystem.Operation.copy(Kernel.Path.normalize(source), Kernel.Path.normalize(destination));
    }

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.RSB.PackCustom.Argument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackCustom.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackCustom.Configuration
        >({
            id: "popcap.rsb.watch",
            configuration_file: Home.query("~/Executor/Configuration/popcap.rsb.watch.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, true);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", Kernel.Path.except_extension(argument.source));
                Console.output(argument.destination!);
                load_bigint(argument, "generic", this.configuration, Script.Executor.Methods.PopCap.RSB.UnpackCustom.Detail.generic(), Kernel.Language.get("popcap.rsb.custom.generic"));
                const packages_info: PackCustom.PackagesInfo | null = (Kernel.JSON.deserialize_fs(`${argument.source}/data.json`) as any).packages_info;
                const count_of = () => {
                    const result: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.PackagesSetting = PackCustom.load_packages(argument.source, packages_info);
                    if (packages_info !== null) {
                        if (result.json_count !== 0n && packages_info.encode) {
                            Console.output(`${Kernel.Language.get("popcap.rsb.pack_custom.total_json_count")}: ${result.json_count}`);
                            if (result.json_count !== 0n && packages_info.chinese) {
                                load_string(argument, "key", this.configuration.packages_setting, Kernel.Language.get("popcap.rsb.pack_custom.key"));
                                load_string(argument, "iv", this.configuration.packages_setting, Kernel.Language.get("popcap.rsb.pack_custom.iv"));
                                result.key = argument.key!;
                                result.iv = argument.iv!;
                            }
                        }
                        Console.output(`${Kernel.Language.get("popcap.rsb.pack_custom.total_rton_count")}: ${result.rton_count}`);
                    } else {
                        Console.output(Kernel.Language.get("popcap.rsb.pack_custom.packages_does_not_use"));
                    }
                    return result;
                };
                const packages_setting = count_of();
                const setting: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.Setting = {
                    texture_format_category: argument.generic!,
                    only_high_resolution: false,
                    packages_setting,
                    compression_setting: {
                        manifest: argument.manifest!,
                        packages: argument.packages!,
                    },
                };
                Console.warning(Kernel.Language.get("popcap.rsb.watch.copying_root"));
                Kernel.FileSystem.create_directory(`${argument.source}.repo`);
                const repo = {
                    root: `${argument.source}.repo/root`,
                    directory: `${argument.source}.repo`,
                };
                Kernel.FileSystem.Operation.copy_directory(argument.source, repo.root);
                const watcher = new Kernel.FileWatcher(argument.source);
                watcher.on("delete", (e) => {
                    const timestamp = current_date();
                    Console.obtained(`${timestamp}: ${Kernel.Language.get("popcap.rsb.watch.delete")} ${e}`);
                    clock.start_safe();
                    setting.packages_setting = count_of();
                    const home = `${repo.directory}/${timestamp}`;
                    Kernel.FileSystem.create_directory(home);
                    Kernel.FileSystem.create_directory(`${home}/${Kernel.Path.dirname(e)}`);
                    backup_content(`${repo.root}/${e}`, `${home}/${e}`);
                    Kernel.FileSystem.write_file(`${repo.directory}/${timestamp}/change.txt`, `Delete ${e}`);
                    Kernel.FileSystem.Operation.remove(Kernel.Path.normalize(`${repo.root}/${e}`));
                    if (/(.*)\.scg$/i.test(`${argument.source}/${e}`) || /(.*)\.rton$/i.test(e)) {
                        Console.output(argument.destination!);
                        Kernel.Support.Miscellaneous.Custom.ResourceStreamBundle.pack_fs(argument.source, argument.destination!, setting);
                    }
                    clock.stop_safe();
                    Console.send(`${Kernel.Language.get("elapsed_time")}: ${clock.duration.toFixed(3)}s`, Definition.Console.Color.GREEN);
                });
                watcher.on("update", (e) => {
                    const timestamp = current_date();
                    Console.obtained(`${timestamp.split("-")}: ${Kernel.Language.get("popcap.rsb.watch.update")} ${e}`);
                    clock.start_safe();
                    const home = `${repo.directory}/${timestamp}`;
                    Kernel.FileSystem.create_directory(home);
                    backup_content(`${repo.root}/${e}`, `${repo.directory}/${timestamp}/${e}`);
                    backup_content(`${argument.source}/${e}`, `${repo.root}/${e}`);
                    Kernel.FileSystem.write_file(`${repo.directory}/${timestamp}/change.txt`, `Update ${e}`);
                    if (/(.*)\.scg$/i.test(`${argument.source}/${e}`) || /(.*)\.rton$/i.test(e)) {
                        Console.output(argument.destination!);
                        Kernel.Support.Miscellaneous.Custom.ResourceStreamBundle.pack_fs(argument.source, argument.destination!, setting);
                    }
                    clock.stop_safe();
                    Console.send(`${Kernel.Language.get("elapsed_time")}: ${clock.duration.toFixed(3)}s`, Definition.Console.Color.GREEN);
                });
                Console.argument(format(Kernel.Language.get("popcap.rsb.is_watching"), argument.source));
                watcher.start();
                return;
            },
            batch_forward: undefined!,
            is_enabled: false,
            configuration: undefined!,
            filter: ["directory", /(.*)\.bundle$/i],
            option: 61n,
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.Watch.forward();
