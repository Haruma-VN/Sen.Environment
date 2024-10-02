namespace Sen.Script.Executor.Methods.PopCap.Animation.ToAPNG {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        media?: string;
        resolution?: bigint;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {}

    /**
     * Async support
     */

    export interface AsyncArgument extends Sen.Script.Executor.Base {}

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
            Sen.Script.Executor.Methods.PopCap.Animation.ToAPNG.Argument,
            Sen.Script.Executor.Methods.PopCap.Animation.ToAPNG.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.ToAPNG.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.ToAPNG.Configuration
        >({
            id: "popcap.animation.to_apng",
            configuration_file: Home.query("~/Executor/Configuration/popcap.animation.to_apng.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                Console.output(argument.source!);
                defined_or_default<Argument, string>(argument, "destination", Kernel.Path.resolve(`${argument.source}.animation`));
                load_bigint(argument, "resolution", this.configuration, Detail.resolution(), Kernel.Language.get("popcap.animation.to_flash.resolution"));
                if (argument.media === undefined) {
                    // TODO : Add localization
                    argument.media = Console.path("input media", "directory");
                }
                clock.start_safe();
                Support.PopCap.Animation.Miscellaenous.GenerateAnimation.process_fs(argument.source!, argument.media!, argument.destination!, {
                    use_image_id: false,
                    image_reslution: argument.resolution!,
                });
                clock.stop_safe();
                return;
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /.*(\.pam\.json)$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Animation.ToAPNG.forward();
