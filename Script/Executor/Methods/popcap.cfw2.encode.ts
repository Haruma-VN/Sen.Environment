namespace Sen.Script.Executor.Methods.PopCap.CFW2.Encode {
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
            Sen.Script.Executor.Methods.PopCap.CFW2.Encode.Argument,
            Sen.Script.Executor.Methods.PopCap.CFW2.Encode.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.CFW2.Encode.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.CFW2.Encode.Configuration
        >({
            id: "popcap.cfw2.encode",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.cfw2.encode.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.CFW2.Encode.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                defined_or_default<Sen.Script.Executor.Methods.PopCap.CFW2.Encode.Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.cfw2`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.CharacterFontWidget2.encode_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.CFW2.Encode.BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /.*\.cfw2\.json$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.CFW2.Encode.forward();
