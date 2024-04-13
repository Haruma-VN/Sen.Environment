namespace Sen.Script.Executor.Methods.PopCap.Reanim.Decode {
    // Using platform

    export type Platform = Kernel.Support.PopCap.Reanim.Platform;

    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        platform?: Platform;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
        platform?: Platform;
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

    export interface Configuration extends Sen.Script.Executor.Configuration {}

    /**
     * Detail namespace
     */

    export namespace Detail {
        /**
         * Platform supported
         */

        export const _platform: Array<Kernel.Support.PopCap.Reanim.Platform> = ["pc", "game-console", "phone-32", "phone-64", "raw-xml", "tv"];
        /**
         *
         * Typical Style
         *
         */

        export function platform(): Array<[bigint, string, string]> {
            return _platform.map((e, i) => [BigInt(i + 1), e as string, Kernel.Language.get(`popcap.reanim.platform.${e}`)]);
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
            Sen.Script.Executor.Methods.PopCap.Reanim.Decode.Argument,
            Sen.Script.Executor.Methods.PopCap.Reanim.Decode.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Reanim.Decode.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Reanim.Decode.Configuration
        >({
            id: "popcap.reanim.decode",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.reanim.decode.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Reanim.Decode.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                defined_or_default<Sen.Script.Executor.Methods.PopCap.Reanim.Decode.Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.json`);
                Sen.Script.Console.output(argument.destination!);
                configurate_or_input(argument, "platform", Detail.platform());
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.Reanim.decode_fs(argument.source, argument.destination!, argument.platform!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.Reanim.Decode.BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)(\.reanim)$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Reanim.Decode.forward();
