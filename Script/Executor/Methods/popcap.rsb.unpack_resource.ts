namespace Sen.Script.Executor.Methods.PopCap.RSB.UnpackResource {
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
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackResource.Argument,
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackResource.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackResource.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackResource.Configuration
        >({
            id: "popcap.rsb.unpack_resource",
            configuration_file: Home.query("~/Executor/Configuration/popcap.rsb.unpack_resource.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${argument.source}.bundle`);
                Console.output(argument.destination!);
                clock.start_safe();
                Kernel.Support.PopCap.RSB.unpack_resource(argument.source, argument.destination!);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.*)\.(rsb|obb)$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.UnpackResource.forward();
