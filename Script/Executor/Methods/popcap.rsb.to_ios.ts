namespace Sen.Script.Executor.Methods.PopCap.RSB.ToIOS {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        resolution?: Support.PopCap.ResourceStreamBundle.Miscellaneous.ConvertAndroidToIOS.RSBResolution;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
    }

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {}

    /**
     * Detail
     */

    export namespace Detail {
        /**
         * Input resolution are supported covered here
         */

        export const resolution = (): Array<[bigint, string, string]> => {
            return [
                [1n, Support.PopCap.ResourceStreamBundle.Miscellaneous.ConvertAndroidToIOS.RSBResolutionX[0], Kernel.Language.get("popcap.animation.to_flash.resolution.1536n")],
                [2n, Support.PopCap.ResourceStreamBundle.Miscellaneous.ConvertAndroidToIOS.RSBResolutionX[1], Kernel.Language.get("popcap.animation.to_flash.resolution.768n")],
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
            Sen.Script.Executor.Methods.PopCap.RSB.ToIOS.Argument,
            Sen.Script.Executor.Methods.PopCap.RSB.ToIOS.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.ToIOS.Configuration
        >({
            id: "popcap.rsb.to_ios",
            configuration_file: Home.query("~/Executor/Configuration/popcap.rsb.to_ios.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${argument.source}.bundle`);
                Console.output(argument.destination!);
                load_bigint(argument, "resolution", this.configuration, Detail.resolution(), Kernel.Language.get("popcap.animation.to_flash.resolution"));
                clock.start_safe();
                Support.PopCap.ResourceStreamBundle.Miscellaneous.ConvertAndroidToIOS.process_fs(argument.source, argument.destination!, argument.resolution!);
                clock.stop_safe();
                return;
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.*)\.(rsb|obb)$/i],
            option: 80n,
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.ToIOS.forward();
