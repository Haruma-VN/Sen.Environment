namespace Sen.Script.Executor.Methods.PopCap.RSG.Pack {
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
            Sen.Script.Executor.Methods.PopCap.RSG.Pack.Argument,
            Sen.Script.Executor.Methods.PopCap.RSG.Pack.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSG.Pack.Configuration
        >({
            id: "popcap.rsg.pack",
            configuration_file: Home.query("~/Executor/Configuration/popcap.rsg.pack.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, true);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.rsg`);
                Console.output(argument.destination!);
                clock.start_safe();
                Kernel.Support.PopCap.RSG.pack_fs(argument.source, argument.destination!);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, true);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /(.*)\.packet$/i],
            option: 62n,
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSG.Pack.forward();
