namespace Sen.Script.Executor.Methods.PopCap.PAK.Unpack {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
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

    export interface Configuration extends Sen.Script.Executor.Configuration {}

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.PAK.Unpack.Argument,
            Sen.Script.Executor.Methods.PopCap.PAK.Unpack.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.PAK.Unpack.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.PAK.Unpack.Configuration
        >({
            id: "popcap.pak.unpack",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.pak.unpack.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.PAK.Unpack.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                defined_or_default<Sen.Script.Executor.Methods.PopCap.PAK.Unpack.Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.data_package`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.PAK.unpack_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.PAK.Unpack.BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.*)\.pak$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.PAK.Unpack.forward();
