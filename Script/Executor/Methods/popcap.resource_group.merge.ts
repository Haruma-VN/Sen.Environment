namespace Sen.Script.Executor.Methods.PopCap.ResourceGroup.Merge {
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
            Sen.Script.Executor.Methods.PopCap.ResourceGroup.Merge.Argument,
            Sen.Script.Executor.Methods.PopCap.ResourceGroup.Merge.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.ResourceGroup.Merge.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.ResourceGroup.Merge.Configuration
        >({
            id: "popcap.resource_group.merge",
            configuration_file: Home.query("~/Executor/Configuration/popcap.resource_group.merge.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, true);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", Kernel.Path.except_extension(argument.source));
                Console.output(argument.destination!);
                clock.start_safe();
                Kernel.Support.PopCap.ResourceGroup.merge_fs(argument.source, argument.destination!);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, true);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /.*\.info$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.ResourceGroup.Merge.forward();
