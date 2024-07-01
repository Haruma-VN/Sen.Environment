namespace Sen.Script.Executor.Methods.PopCap.Animation.DecodeAndToFlash {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        resolution?: bigint;
        has_label?: boolean;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
    }

    /**
     * Detail
     */

    export namespace Detail {
        /**
         * Input resolution are supported covered here
         */

        export const resolution = (): Array<[bigint, bigint, string]> => {
            return [
                [1n, 1536n, Kernel.Language.get("popcap.animation.to_flash.resolution.1536n")],
                [2n, 768n, Kernel.Language.get("popcap.animation.to_flash.resolution.768n")],
                [3n, 384n, Kernel.Language.get("popcap.animation.to_flash.resolution.384n")],
                [4n, 1200n, Kernel.Language.get("popcap.animation.to_flash.resolution.1200n")],
                [5n, 640n, Kernel.Language.get("popcap.animation.to_flash.resolution.640n")],
            ];
        };
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
        resolution: bigint;
        has_label: boolean;
    }

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.Animation.DecodeAndToFlash.Argument,
            Sen.Script.Executor.Methods.PopCap.Animation.DecodeAndToFlash.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.DecodeAndToFlash.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.DecodeAndToFlash.Configuration
        >({
            id: "popcap.animation.decode_and_to_flash",
            configuration_file: Home.query("~/Executor/Configuration/popcap.animation.decode_and_to_flash.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${argument.source}.xfl`);
                Console.output(argument.destination!);
                load_bigint(argument, "resolution", this.configuration, Detail.resolution(), Kernel.Language.get("popcap.animation.to_flash.resolution"));
                load_boolean(argument, "has_label", this.configuration, Kernel.Language.get("popcap.animation.extract_label"));
                clock.start_safe();
                Kernel.Support.PopCap.Animation.Instance.to_flash(argument.source, argument.destination!, argument.resolution!, argument.has_label!);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)\.pam$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Animation.DecodeAndToFlash.forward();
