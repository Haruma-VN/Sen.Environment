namespace Sen.Script.Executor.Methods.PopCap.RSB.PackForModding {
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
            Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.Argument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.Configuration
        >({
            id: "popcap.rsb.pack_for_modding",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.rsb.pack_for_modding.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.Argument, string>(argument, "destination", Kernel.Path.except_extension(argument.source));
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.clock.start_safe();
                Support.PopCap.ResourceStreamBundle.Project.Pack.process_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward: undefined!,
            async_forward: undefined!,
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /(.*)\.mod_bundle$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.PackForModding.forward();
