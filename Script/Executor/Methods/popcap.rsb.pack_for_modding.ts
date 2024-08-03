namespace Sen.Script.Executor.Methods.PopCap.RSB.PackForModding {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
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

    export interface Configuration extends Sen.Script.Executor.Configuration {}

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

    export function read_packages(source: string): PackagesFileList {
        const packages_list: PackagesFileList = {
            rton_file: [],
            json_file: [],
        };
        const packages_folder = `${source}/packages`;
        if (Kernel.FileSystem.is_directory(packages_folder)) {
            Console.output(`resource_bundle_doesn't_have_packages`); // TODO: add to localization.
        } else {
            const file_list = Kernel.FileSystem.read_directory(packages_folder);
            for (let element of file_list) {
                if (Kernel.Path.extname(element).toLowerCase() == ".json") {
                    packages_list.json_file.push(element);
                }
                if (Kernel.Path.extname(element).toLowerCase() == ".rton") {
                    packages_list.rton_file.push(element);
                }
            }
            Console.output(`packages has ${packages_list.json_file.size()} json file to convert`); // TODO: add to localization.
            Console.output(`num of rton files: ${packages_list.rton_file.size()}`); // TODO: add to localization.
        }
        return packages_list;
    }

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.Argument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.Configuration
        >({
            id: "popcap.rsb.pack_for_modding",
            configuration_file: Home.query("~/Executor/Configuration/popcap.rsb.pack_for_modding.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, true);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", Kernel.Path.except_extension(argument.source));
                Console.output(argument.destination!);
                clock.start_safe();
                const packages_list = read_packages(argument.source);
                Kernel.Support.Miscellaneous.Modding.pack_rsb(argument.source, argument.destination!, packages_list.rton_file, packages_list.json_file);
                clock.stop_safe();
                return;
            },
            batch_forward: undefined!,
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /(.*)\.mod_bundle$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.forward();
