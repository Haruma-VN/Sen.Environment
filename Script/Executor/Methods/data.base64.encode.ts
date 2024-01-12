namespace Sen.Script.Executor.Methods.Data.Base64.Encode {
    /**
     * Argument for the current method
     */

    export interface Argument extends Base {
        source: string;
        destination?: string;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Base {
        directory: string;
    }

    /**
     * Async support
     */

    export interface AsyncArgument<source extends string, destination extends string> extends Base {
        worker: Array<[source, destination]>;
    }

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Base {}

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.Data.Base64.Encode.Argument,
            Sen.Script.Executor.Methods.Data.Base64.Encode.BatchArgument,
            Sen.Script.Executor.Methods.Data.Base64.Encode.AsyncArgument<string, string>
        >({
            id: `data.base64.encode`,
            configuration_file: Sen.Script.Home.query(`~/Executor/Configuration/data.base64.encode.json`),
            direct_forward(argument: Sen.Script.Executor.Methods.Data.Base64.Encode.Argument) {
                Sen.Script.Executor.clock.start_safe();
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.Data.Base64.Encode.Argument, string>(argument, `destination`, Sen.Kernel.Path.resolve(`${argument.source}.bin`));
                Sen.Script.Console.output(argument.destination!);
                Sen.Kernel.Encryption.Base64.encode_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.Data.Base64.Encode.BatchArgument) {
                Sen.Kernel.FileSystem.read_directory(argument.directory)
                    .filter((path: string) => Sen.Kernel.FileSystem.is_file(path))
                    .forEach((source: string) => this.direct_forward({ source }));
                return;
            },
            async_forward(argument: Sen.Script.Executor.Methods.Data.Base64.Encode.AsyncArgument<string, string>) {
                Sen.Kernel.Encryption.Base64.encode_fs_as_multiple_threads<string, string>(argument.worker);
                return;
            },
        });
        return;
    }
}

Sen.Script.Executor.Methods.Data.Base64.Encode.forward();
