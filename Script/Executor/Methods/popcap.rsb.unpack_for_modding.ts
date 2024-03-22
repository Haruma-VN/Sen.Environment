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
                [1n, "string", Sen.Kernel.Language.get("popcap.rsb.unpack_for_modding.layout.string")],
                [2n, "array", Sen.Kernel.Language.get("popcap.rsb.unpack_for_modding.layout.array")],
            ];
        }

        export function generic(): Array<[bigint, string, string]> {
            return [
                [1n, "android", Sen.Kernel.Language.get("android")],
                [2n, "android-cn", Sen.Kernel.Language.get("pvz2_android_cn")],
                [3n, "ios", Sen.Kernel.Language.get("ios")],
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
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.rsb.unpack_for_modding.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.RSB.UnpackForModding.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.RSB.UnpackForModding.Argument, string>(argument, "destination", `${argument.source}.mod_bundle`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.clock.start_safe();
                Sen.Script.Executor.load_bigint(argument, "layout", this.configuration, Detail.style(), Sen.Kernel.Language.get("popcap.rsb.unpack_for_modding.layout"));
                Sen.Script.Executor.load_bigint(argument, "generic", this.configuration, Detail.generic(), Sen.Kernel.Language.get("popcap.rsb.unpack_for_modding.generic"));
                Support.PopCap.ResourceStreamBundle.Project.Unpack.process_fs(argument.source, argument.destination!, {
                    decode_rton: false,
                    decrypt_rton: false,
                    layout: Detail.exchange_layout(argument.layout as string),
                    generic: argument.generic!,
                });
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.RSB.UnpackForModding.BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            async_forward(argument: Sen.Script.Executor.Methods.PopCap.RSB.UnpackForModding.AsyncArgument): void {
                Sen.Script.Executor.clock.start_safe();
                for (let i = 0n; i < BigInt(argument.parameter.length); i += Setting.setting.thread_limit_count) {
                    const current_thread: Array<[string, string]> = [
                        ...argument.parameter.slice(
                            Number(i),
                            i + Setting.setting.thread_limit_count <= BigInt(argument.parameter.length) ? Number(i + Setting.setting.thread_limit_count) : argument.parameter.length,
                        ),
                    ];
                    current_thread.forEach((e: [string, string]) => {
                        Sen.Script.Console.obtained(e[0]);
                        Sen.Script.Console.output(e[1]);
                    });
                    // to do
                }
                Sen.Script.Executor.clock.stop_safe();
                Sen.Script.Console.finished(Sen.Script.format(Sen.Kernel.Language.get("batch.process.count"), argument.parameter.length));
                return;
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.*)\.(rsb|obb)$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.UnpackForModding.forward();
