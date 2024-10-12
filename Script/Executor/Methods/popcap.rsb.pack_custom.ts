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
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {
        layout: string | "?";
        generic: string | "?";
        packages_setting: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.PackagesSetting;
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

    export interface PackagesInfo {
        compression: 1n | 2n | 3n | 4n;
        chinese: boolean;
        encode: boolean;
    }

    export function load_packages(source: string, packages_info: null | PackagesInfo): Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.PackagesSetting {
        const packages_setting: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.PackagesSetting = {
            rton_count: 0n,
            json_count: 0n,
            key: "",
            iv: "",
        };
        if (packages_info !== null) {
            const packages_list = Kernel.FileSystem.read_directory(`${source}/packages`);
            if (packages_info.encode) {
                const json_file_list = new Set<string>();
                for (let element of packages_list) {
                    const currentElement = element.slice(0, element.length - 5);
                    if (Kernel.Path.extname(element).toLowerCase() === ".json") {
                        json_file_list.add(currentElement);
                        ++packages_setting.json_count;
                    }
                    if (Kernel.Path.extname(element).toLowerCase() === ".rton") {
                        const hasValue: boolean = json_file_list.has(currentElement);
                        if (hasValue) {
                            json_file_list.delete(currentElement);
                        }
                        ++packages_setting.rton_count;
                    }
                }
                packages_setting.rton_count += BigInt(json_file_list.size);
            } else {
                for (let element of packages_list) {
                    if (Kernel.Path.extname(element).toLowerCase() === ".rton") {
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
                const packages_info: PackagesInfo | null = (Kernel.JSON.deserialize_fs(`${argument.source}/data.json`) as any).packages_info;
                const packages_setting: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.PackagesSetting = load_packages(argument.source, packages_info);
                if (packages_info !== null) {
                    if (packages_setting.json_count !== 0n && packages_info.encode) {
                        Console.output(`${Kernel.Language.get("popcap.rsb.pack_custom.total_json_count")}: ${packages_setting.json_count}`);
                        if (packages_setting.json_count !== 0n && packages_info.chinese) {
                            load_string(argument, "key", this.configuration.packages_setting, Kernel.Language.get("popcap.rsb.pack_custom.key"));
                            load_string(argument, "iv", this.configuration.packages_setting, Kernel.Language.get("popcap.rsb.pack_custom.iv"));
                            packages_setting.key = argument.key!;
                            packages_setting.iv = argument.iv!;
                        }
                    }
                    Console.output(`${Kernel.Language.get("popcap.rsb.pack_custom.total_rton_count")}: ${packages_setting.rton_count}`);
                } else {
                    Console.output(Kernel.Language.get("popcap.rsb.pack_custom.packages_does_not_use"));
                }
                const setting: Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration.Setting = {
                    texture_format_category: argument.generic!,
                    only_high_resolution: false,
                    packages_setting,
                    compression_setting: {
                        manifest: argument.manifest!,
                        packages: argument.packages!,
                    },
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
            option: 55n,
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.PackCustom.forward();
