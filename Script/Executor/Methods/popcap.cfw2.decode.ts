namespace Sen.Script.Executor.Methods.PopCap.CFW2.Decode {
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
            Sen.Script.Executor.Methods.PopCap.CFW2.Decode.Argument,
            Sen.Script.Executor.Methods.PopCap.CFW2.Decode.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.CFW2.Decode.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.CFW2.Decode.Configuration
        >({
            id: "popcap.cfw2.decode",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.cfw2.decode.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.CFW2.Decode.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.CFW2.Decode.Argument, string>(argument, "destination", `${argument.source}.json`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.CharacterFontWidget2.decode_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.CFW2.Decode.BatchArgument): void {
                const files: Array<string> = Sen.Kernel.FileSystem.read_directory(argument.directory).filter((path: string) => Sen.Kernel.FileSystem.is_file(path));
                files.forEach((source: string) => this.direct_forward({ source: source }));
                Sen.Script.Console.finished(Sen.Script.format(Sen.Kernel.Language.get("batch.process.count"), files.length));
                return;
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)\.cfw2$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.CFW2.Decode.forward();
