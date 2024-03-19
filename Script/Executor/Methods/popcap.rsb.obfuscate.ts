namespace Sen.Script.Executor.Methods.PopCap.RSB.Obfuscate {
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
            Sen.Script.Executor.Methods.PopCap.RSB.Obfuscate.Argument,
            Sen.Script.Executor.Methods.PopCap.RSB.Obfuscate.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.Obfuscate.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RSB.Obfuscate.Configuration
        >({
            id: "popcap.rsb.obfuscate",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.rsb.obfuscate.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.RSB.Obfuscate.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.RSB.Obfuscate.Argument, string>(argument, "destination", `${argument.source}.bin`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.clock.start_safe();
                Sen.Script.Support.PopCap.ResourceStreamBundle.Miscellaneous.Obfuscate.process_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.RSB.Obfuscate.BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            async_forward: undefined!,
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.*)\.(rsb|obb)$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSB.Obfuscate.forward();
