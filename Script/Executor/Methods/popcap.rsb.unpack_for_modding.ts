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
        layout: 1n | 2n | "?";
        generic: 1n | 2n | 3n | "?";
    }

    /**
     * Detail namespace
     */

    export namespace Detail {
        /**
         * JS Implement
         * @param layout - Layout to exchange
         * @returns style
         */

        export function exchange_layout(layout: string): Sen.Script.Support.PopCap.ResourceGroup.PathStyle {
            switch (layout) {
                case "string": {
                    return Sen.Script.Support.PopCap.ResourceGroup.PathStyle.WindowStyle;
                }
                case "array": {
                    return Sen.Script.Support.PopCap.ResourceGroup.PathStyle.ArrayStyle;
                }
                default: {
                    throw new Error(format(Kernel.Language.get("popcap.resource_group.convert.cannot_exchange_layout"), layout));
                }
            }
        }
        /**
         *
         * Typical Style
         *
         */

        export function style(): Array<[bigint, string, string]> {
            return [
                [1n, "string", Kernel.Language.get("popcap.rsb.unpack_for_modding.layout.string")],
                [2n, "array", Kernel.Language.get("popcap.rsb.unpack_for_modding.layout.array")],
            ];
        }

        export function generic(): Array<[bigint, string, string]> {
            return [
                [1n, "android", Kernel.Language.get("android")],
                [2n, "android-cn", Kernel.Language.get("pvz2_android_cn")],
                [3n, "ios", Kernel.Language.get("ios")],
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
                //load_bigint(argument, "layout", this.configuration, Detail.style(), Kernel.Language.get("popcap.rsb.unpack_for_modding.layout"));
                load_bigint(argument, "generic", this.configuration, Detail.generic(), Kernel.Language.get("popcap.rsb.unpack_for_modding.generic"));
                clock.start_safe();
                Kernel.Support.Miscellaneous.Modding.unpack_rsb(argument.source, argument.destination!, argument.generic! === "ios");
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
