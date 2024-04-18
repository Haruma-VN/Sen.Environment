namespace Sen.Script.Executor.Methods.PopCap.RSB.UnpackByLooseConstraints {
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
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackByLooseConstraints.Argument,
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackByLooseConstraints.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackByLooseConstraints.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.UnpackByLooseConstraints.Configuration
        >({
            id: "popcap.rsb.unpack_by_loose_constraints",
            configuration_file: Home.query("~/Executor/Configuration/popcap.rsb.unpack_by_loose_constraints.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${argument.source}.bundle`);
                Console.output(argument.destination!);
                clock.start_safe();
                Support.PopCap.ResourceStreamBundle.Miscellaneous.UnpackByLooseConstraints.process_fs(argument.source, argument.destination!);
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

Sen.Script.Executor.Methods.PopCap.RSB.UnpackByLooseConstraints.forward();
