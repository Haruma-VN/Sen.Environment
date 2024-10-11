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
            Sen.Script.Executor.Methods.PopCap.PAK.Unpack.Configuration
        >({
            id: "popcap.pak.unpack",
            configuration_file: Home.query("~/Executor/Configuration/popcap.pak.unpack.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.data_package`);
                Console.output(argument.destination!);
                clock.start_safe();
                Kernel.Support.PopCap.PAK.unpack_fs(argument.source, argument.destination!);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.*)\.pak$/i],
            option: 25n,
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.PAK.Unpack.forward();
