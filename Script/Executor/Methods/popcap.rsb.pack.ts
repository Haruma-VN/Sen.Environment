namespace Sen.Script.Executor.Methods.PopCap.RSB.Pack {
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
            Sen.Script.Executor.Methods.PopCap.RSB.Pack.Argument,
            Sen.Script.Executor.Methods.PopCap.RSB.Pack.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.Pack.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.Pack.Configuration
        >({
            id: "popcap.rsb.pack",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.rsb.pack.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.RSB.Pack.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.RSB.Pack.Argument, string>(argument, "destination", Kernel.Path.except_extension(argument.source));
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.RSB.pack_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward: undefined!,
            async_forward: undefined,
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /(.*)\.bundle$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.Pack.forward();
