namespace Sen.Script.Executor.Methods.Data.MD5.Hash {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
    }

    export interface AsyncArgument extends Sen.Script.Executor.Base {}

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
            Sen.Script.Executor.Methods.Data.MD5.Hash.Argument,
            Sen.Script.Executor.Methods.Data.MD5.Hash.BatchArgument,
            Sen.Script.Executor.Methods.Data.MD5.Hash.AsyncArgument,
            Sen.Script.Executor.Methods.Data.MD5.Hash.Configuration
        >({
            id: "data.md5.hash",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/data.md5.hash.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.Data.MD5.Hash.Argument): void {
                Sen.Script.Executor.clock.start_safe();
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Console.output(Sen.Kernel.Encryption.MD5.hash_fs(argument.source));
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.Data.MD5.Hash.BatchArgument): void {
                const files: Array<string> = Sen.Kernel.FileSystem.read_directory(argument.directory).filter((path: string) => Sen.Kernel.FileSystem.is_file(path));
                files.forEach((source: string) => this.direct_forward({ source: source }));
                Sen.Script.Console.finished(Sen.Script.Setting.format(Sen.Kernel.Language.get("batch.process.count"), files.length));
                return;
            },
            async_forward: undefined,
            is_enabled: false,
            configuration: undefined!,
        });
        return;
    }
}

Sen.Script.Executor.Methods.Data.MD5.Hash.forward();
