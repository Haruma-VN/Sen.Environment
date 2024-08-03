namespace Sen.Script.Executor.Methods.PopCap.RSB.UnpackForModding {
    // Generic

    export type Generic = Support.PopCap.ResourceStreamBundle.Project.Unpack.Generic;

    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        layout?: string;
        generic?: Generic;
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
        generic: 1n | 2n | 3n | "?";
    }

    /**
     * Detail namespace
     */

    export namespace Detail {
        export function generic(): Array<[bigint, bigint, string]> {
            return [
                [1n, 0n, Kernel.Language.get("android")],
                [2n, 1n, Kernel.Language.get("ios")],
                [3n, 2n, Kernel.Language.get("pvz2_android_cn")],
            ];
        }
    }

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackForModding.Argument,
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackForModding.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackForModding.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackForModding.Configuration
        >({
            id: "popcap.rsb.unpack_for_modding",
            configuration_file: Home.query("~/Executor/Configuration/popcap.rsb.unpack_for_modding.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${argument.source}.mod_bundle`);
                Console.output(argument.destination!);
                load_bigint(argument, "generic", this.configuration, Detail.generic(), Kernel.Language.get("popcap.rsb.unpack_for_modding.generic"));
                clock.start_safe();
                Kernel.Support.Miscellaneous.Modding.unpack_rsb(argument.source, argument.destination!, argument.generic!);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.*)\.(rsb|obb)$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.UnpackForModding.forward();
