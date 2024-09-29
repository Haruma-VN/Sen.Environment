namespace Sen.Script.Executor.Methods.Animation.Flash.Resize {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        resolution?: bigint;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
        resolution?: bigint;
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
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.Animation.Flash.Resize.Argument,
            Sen.Script.Executor.Methods.Animation.Flash.Resize.BatchArgument,
            Sen.Script.Executor.Methods.Animation.Flash.Resize.AsyncArgument,
            Sen.Script.Executor.Methods.Animation.Flash.Resize.Configuration
        >({
            id: "animation.flash.resize",
            configuration_file: Home.query("~/Executor/Configuration/animation.flash.resize.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, true);
                Console.obtained(argument.source);
                Console.output(argument.source!);
                load_bigint(argument, "resolution", this.configuration, Detail.resolution(), Kernel.Language.get("popcap.animation.to_flash.resolution"));
                clock.start_safe();
                Kernel.Support.PopCap.Animation.Miscellaneous.resize_fs(argument.source, BigInt(argument.resolution!));
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                load_bigint(argument, "resolution", this.configuration, Detail.resolution(), Kernel.Language.get("popcap.animation.to_flash.resolution"));
                return basic_batch(this, argument, false, { resolution: BigInt(argument.resolution!) });
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /.*(\.xfl|\.pam\.xfl)$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.Animation.Flash.Resize.forward();
