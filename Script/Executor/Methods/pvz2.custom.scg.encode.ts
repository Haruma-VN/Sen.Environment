namespace Sen.Script.Executor.Methods.PvZ2.Custom.SCG.Encode {
    /**
     * Argument for the current method
     */
    export type Generic = Sen.Script.Support.Miscellaneous.Custom.StreamCompressedGroup.Configuration.Generic;

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        generic?: Generic;
        animation_split_label?: boolean;
        enable_debug?: boolean;
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
                [1n, 0n, Kernel.Language.get("simple")],
                [2n, 1n, Kernel.Language.get("advanced")],
                [3n, 2n, Kernel.Language.get("debug")]
            ];
        }
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

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PvZ2.Custom.SCG.Encode.Argument,
            Sen.Script.Executor.Methods.PvZ2.Custom.SCG.Encode.BatchArgument,
            Sen.Script.Executor.Methods.PvZ2.Custom.SCG.Encode.AsyncArgument,
            Sen.Script.Executor.Methods.PvZ2.Custom.SCG.Encode.Configuration
        >({
            id: "pvz2.custom.scg.encode",
            configuration_file: Home.query("~/Executor/Configuration/pvz2.custom.scg.encode.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, true);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.scg`);
                Console.output(argument.destination!);
                load_boolean(argument, "enable_debug", this.configuration, Kernel.Language.get("pvz2.custom.scg.enable_debug"));
                const generic = Detail.generic();
                if (!argument.enable_debug) {
                    generic.pop();
                }
                load_bigint(argument, "generic", this.configuration, generic, Kernel.Language.get("pvz2.custom.scg.encode.generic"));
                if (argument.generic! == 1n) {
                    load_boolean(argument, "animation_split_label", this.configuration, Kernel.Language.get("pvz2.custom.scg.animation_split_label"));
                }
                const setting: Script.Support.Miscellaneous.Custom.StreamCompressedGroup.Configuration.Setting = {
                    decode_method: argument.generic!,
                    animation_split_label: argument.animation_split_label! ?? false
                };
                clock.start_safe();
                Kernel.Support.Miscellaneous.Custom.StreamCompressedGroup.encode_fs(argument.source, argument.destination!, setting);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /(.*)\.package$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PvZ2.Custom.SCG.Encode.forward();
