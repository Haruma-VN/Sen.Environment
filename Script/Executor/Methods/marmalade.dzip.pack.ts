namespace Sen.Script.Executor.Methods.Marmalade.DZip.Pack {
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
            Sen.Script.Executor.Methods.Marmalade.DZip.Pack.Argument,
            Sen.Script.Executor.Methods.Marmalade.DZip.Pack.BatchArgument,
            Sen.Script.Executor.Methods.Marmalade.DZip.Pack.Configuration
        >({
            id: "marmalade.dzip.pack",
            configuration_file: Home.query("~/Executor/Configuration/marmalade.dzip.pack.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, true);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.dz`);
                Console.output(argument.destination!);
                clock.start_safe();
                Kernel.Support.Marmalade.DZip.pack_fs(argument.source, argument.destination!);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, true);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /(.*)\.data_package$/i],
            option: 6n,
        });
        return;
    }
}

Sen.Script.Executor.Methods.Marmalade.DZip.Pack.forward();
