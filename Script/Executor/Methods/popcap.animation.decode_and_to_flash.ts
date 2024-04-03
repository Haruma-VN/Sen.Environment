namespace Sen.Script.Executor.Methods.PopCap.Animation.DecodeAndToFlash {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        resolution?: bigint;
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

        export const resolution = (): Array<[bigint, string, string]> => {
            return [
                [1n, "1536", Kernel.Language.get("popcap.animation.to_flash.resolution.1536n")],
                [2n, "768", Kernel.Language.get("popcap.animation.to_flash.resolution.768n")],
                [3n, "384", Kernel.Language.get("popcap.animation.to_flash.resolution.384n")],
                [4n, "1200", Kernel.Language.get("popcap.animation.to_flash.resolution.1200n")],
                [5n, "640", Kernel.Language.get("popcap.animation.to_flash.resolution.640n")],
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
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.animation.decode_and_to_flash.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.DecodeAndToFlash.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.Animation.DecodeAndToFlash.Argument, string>(argument, "destination", `${argument.source}.xfl`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.load_bigint(argument, "resolution", this.configuration, Detail.resolution(), Sen.Kernel.Language.get("popcap.animation.to_flash.resolution"));
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.Animation.Instance.to_flash(argument.source, argument.destination!, argument.resolution!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.DecodeAndToFlash.BatchArgument): void {
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
