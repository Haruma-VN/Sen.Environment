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
     * Async support
     */

    export interface AsyncArgument extends Sen.Script.Executor.Base {
        parameter: Array<[string, string]>;
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
            Sen.Script.Executor.Methods.PopCap.RSB.PackCustom.AsyncArgument,
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
                const packages_info_flag: bigint = BigInt((Kernel.JSON.deserialize_fs(`${argument.source}/data.json`) as any).packages_info_flag);
                const count_of = () => {
                    const result: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.PackagesSetting = PackCustom.load_packages(argument.source, packages_info_flag);
                    load_boolean(argument, "manifest", this.configuration.compression_setting, Kernel.Language.get("popcap.rsb.pack_custom.manifest_compress"));
                    if (packages_info_flag !== 0n) {
                        load_boolean(argument, "packages", this.configuration.compression_setting, Kernel.Language.get("popcap.rsb.pack_custom.packages_compress"));
                        if (result.json_count !== 0n && packages_info_flag % 2n == 0n) {
                            Console.output(`${Kernel.Language.get("popcap.rsb.pack_custom.total_json_count")}: ${result.json_count}`);
                            if (result.json_count !== 0n && packages_info_flag % 4n == 0n) {
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
                Console.warning("copying root directory, please do not delete current directory...");
                Kernel.FileSystem.create_directory(`${argument.source}.repo`);
                const repo = {
                    root: `${argument.source}.repo/root`,
                    directory: `${argument.source}.repo`,
                };
                Kernel.FileSystem.Operation.copy_directory(argument.source, repo.root);
                // TODO : Add localization
                const watcher = new Kernel.FileWatcher(argument.source);
                watcher.on("delete", (e) => {
                    const timestamp = current_date();
                    Console.obtained(`${timestamp}: Deleted ${e}`);
                    clock.start_safe();
                    setting.packages_setting = count_of();
                    const home = `${repo.directory}/${timestamp}`;
                    Kernel.FileSystem.create_directory(home);
                    Kernel.FileSystem.create_directory(`${home}/${Kernel.Path.dirname(e)}`);
                    backup_content(`${repo.root}/${e}`, `${home}/${e}`);
                    Kernel.FileSystem.write_file(`${repo.directory}/${timestamp}/change.txt`, `Delete ${e}`);
                    Kernel.FileSystem.Operation.remove(Kernel.Path.normalize(`${repo.root}/${e}`));
                    if (/(.*)\.scg$/i.test(`${argument.source}/${e}`) || /(.*)\.rton$/i.test(e)) {
                        Console.output(`Repacking: ${argument.destination}`);
                        Kernel.Support.Miscellaneous.Custom.ResourceStreamBundle.pack_fs(argument.source, argument.destination!, setting);
                    }
                    clock.stop_safe();
                    Console.finished(`Elapsed time: ${clock.duration.toFixed(3)}s`);
                });
                watcher.on("update", (e) => {
                    const timestamp = current_date();
                    Console.obtained(`${timestamp.split("-")}: Updated ${e}`);
                    clock.start_safe();
                    const home = `${repo.directory}/${timestamp}`;
                    Kernel.FileSystem.create_directory(home);
                    backup_content(`${repo.root}/${e}`, `${repo.directory}/${timestamp}/${e}`);
                    backup_content(`${argument.source}/${e}`, `${repo.root}/${e}`);
                    Kernel.FileSystem.write_file(`${repo.directory}/${timestamp}/change.txt`, `Update ${e}`);
                    if (/(.*)\.scg$/i.test(`${argument.source}/${e}`) || /(.*)\.rton$/i.test(e)) {
                        Console.output(`Repacking: ${argument.destination}`);
                        Kernel.Support.Miscellaneous.Custom.ResourceStreamBundle.pack_fs(argument.source, argument.destination!, setting);
                    }
                    clock.stop_safe();
                    Console.finished(`Elapsed time: ${clock.duration.toFixed(3)}s`);
                });
                Console.argument(`${argument.source} is being watch`);
                watcher.start();
                return;
            },
            batch_forward: undefined!,
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /(.*)\.bundle$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.Watch.forward();
