namespace Sen.Script.Executor.Methods.PopCap.Animation.ToFlash {
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
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.Argument,
            Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.Configuration
        >({
            id: "popcap.animation.to_flash",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.animation.to_flash.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Methods.PopCap.Animation.ToFlash.Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.xfl`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.load_bigint(argument, "resolution", this.configuration, Detail.resolution(), Sen.Kernel.Language.get("popcap.animation.to_flash.resolution"));
                Sen.Script.Executor.clock.start_safe();
                Kernel.Support.PopCap.Animation.ToFlash.convert_fs(argument.source, argument.destination!, BigInt(argument.resolution!));
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.BatchArgument): void {
                load_bigint(argument, "resolution", this.configuration, Detail.resolution(), Kernel.Language.get("popcap.animation.to_flash.resolution"));
                return basic_batch(this, argument, false, { resolution: BigInt(argument.resolution!) });
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /.*\.pam\.json$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.forward();
