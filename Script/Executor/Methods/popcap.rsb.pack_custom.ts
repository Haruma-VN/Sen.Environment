namespace Sen.Script.Executor.Methods.PopCap.RSB.PackCustom {

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

    export function load_packages(source: string, packages_info_flag: bigint): Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.PackagesSetting {
        const packages_setting: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.PackagesSetting = {
            rton_count: 0n,
            json_count: 0n,
            key: "",
            iv: ""
        }
        if (packages_info_flag !== 0n) {
            const packages_list = Kernel.FileSystem.read_directory(`${source}/packages`);
            if (packages_info_flag % 2n == 0n) {
                const json_file_list: string[] = [];
                for (let element of packages_list) {
                    if (Kernel.Path.extname(element).toLowerCase() == ".json") {
                        json_file_list.push(element.slice(0, element.length - 5));
                        ++packages_setting.json_count;
                    }
                    if (Kernel.Path.extname(element).toLowerCase() == ".rton") {
                        const index = json_file_list.indexOf(element.slice(0, element.length - 5));
                        if (index !== -1) {
                            json_file_list.splice(index, 1);
                        }
                        ++packages_setting.rton_count;
                    }
                }
                packages_setting.rton_count += BigInt(json_file_list.length);
            }
            else {
                for (let element of packages_list) {
                    if (Kernel.Path.extname(element).toLowerCase() == ".rton") {
                        ++packages_setting.rton_count;
                    }
                }
            }
        }
        return packages_setting;
    }

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.RSB.PackCustom.Argument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackCustom.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackCustom.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackCustom.Configuration
        >({
            id: "popcap.rsb.pack_custom",
            configuration_file: Home.query("~/Executor/Configuration/popcap.rsb.pack_custom.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, true);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", Kernel.Path.except_extension(argument.source));
                Console.output(argument.destination!);
                load_bigint(argument, "generic", this.configuration, Script.Executor.Methods.PopCap.RSB.UnpackCustom.Detail.generic(), Kernel.Language.get("popcap.rsb.custom.generic"));
                const packages_info_flag: bigint = BigInt((Kernel.JSON.deserialize_fs(`${argument.source}/data.json`) as any).packages_info_flag);
                const packages_setting: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.PackagesSetting = load_packages(argument.source, packages_info_flag);
                load_boolean(argument, "manifest", this.configuration.compression_setting, Kernel.Language.get("popcap.rsb.pack_custom.manifest_compress"));
                if (packages_info_flag !== 0n) {
                    load_boolean(argument, "packages", this.configuration.compression_setting, Kernel.Language.get("popcap.rsb.pack_custom.packages_compress"));
                    if (packages_setting.json_count !== 0n && packages_info_flag % 2n == 0n) {
                        Console.output(`${Kernel.Language.get("popcap.rsb.pack_custom.total_json_count")}: ${packages_setting.json_count}`);
                        if (packages_setting.json_count !== 0n && packages_info_flag % 4n == 0n) {
                            load_string(argument, "key", this.configuration.packages_setting, Kernel.Language.get("popcap.rsb.pack_custom.key"));
                            load_string(argument, "iv", this.configuration.packages_setting, Kernel.Language.get("popcap.rsb.pack_custom.iv"));
                            packages_setting.key = argument.key!;
                            packages_setting.iv = argument.iv!;
                        }
                    }
                    Console.output(`${Kernel.Language.get("popcap.rsb.pack_custom.total_rton_count")}: ${packages_setting.rton_count}`);
                }
                else {
                    Console.output(Kernel.Language.get("popcap.rsb.pack_custom.packages_does_not_use"));
                }
                const setting: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.Setting = {
                    texture_format_category: argument.generic!,
                    only_high_resolution: false,
                    packages_setting,
                    compression_setting: {
                        manifest: argument.manifest!,
                        packages: argument.packages!
                    }
                };
                clock.start_safe();
                Kernel.Support.Miscellaneous.Custom.ResourceStreamBundle.pack_fs(argument.source, argument.destination!, setting);
                clock.stop_safe();
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

Sen.Script.Executor.Methods.PopCap.RSB.PackCustom.forward();
