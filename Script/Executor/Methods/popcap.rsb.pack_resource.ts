namespace Sen.Script.Executor.Methods.PopCap.RSB.PackResource {
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
            Sen.Script.Executor.Methods.PopCap.RSB.PackResource.Argument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackResource.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackResource.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackResource.Configuration
        >({
            id: "popcap.rsb.pack_resource",
            configuration_file: Home.query("~/Executor/Configuration/popcap.rsb.pack_resource.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, true);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", Kernel.Path.except_extension(argument.source));
                Console.output(argument.destination!);
                clock.start_safe();
                Kernel.Support.PopCap.RSB.pack_resource(argument.source, argument.destination!);
                clock.stop_safe();
                return;
            },
            batch_forward: undefined!,
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /(.*)\.bundle$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.PackResource.forward();
