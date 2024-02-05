namespace Sen.Script.Executor.Methods.PopCap.Animation.Encode {
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
            Sen.Script.Executor.Methods.PopCap.Animation.Encode.Argument,
            Sen.Script.Executor.Methods.PopCap.Animation.Encode.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.Encode.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.Encode.Configuration
        >({
            id: "popcap.animation.encode",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.animation.encode.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.Encode.Argument): void {
                Sen.Script.Executor.clock.start_safe();
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.Animation.Encode.Argument, string>(
                    argument,
                    "destination",
                    `${Sen.Kernel.Path.except_extension(argument.source)}.pam`,
                );
                Sen.Script.Console.output(argument.destination!);
                Sen.Kernel.Support.PopCap.Animation.encode_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.Encode.BatchArgument): void {
                const files: Array<string> = Sen.Kernel.FileSystem.read_directory(argument.directory).filter((path: string) => Sen.Kernel.FileSystem.is_file(path));
                files.forEach((source: string) => this.direct_forward({ source: source }));
                Sen.Script.Console.finished(Sen.Script.format(Sen.Kernel.Language.get("batch.process.count"), files.length));
                return;
            },
            async_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.Encode.AsyncArgument): void {
                Sen.Script.Executor.clock.start_safe();
                for (let i = 0n; i < BigInt(argument.parameter.length); i += Setting.setting.thread_limit_count) {
                    const current_thread: Array<[string, string]> = [
                        ...argument.parameter.slice(
                            Number(i),
                            i + Setting.setting.thread_limit_count <= BigInt(argument.parameter.length) ? Number(i + Setting.setting.thread_limit_count) : argument.parameter.length,
                        ),
                    ];
                    current_thread.forEach((e: [string, string]) => {
                        Sen.Script.Console.obtained(e[0]);
                        Sen.Script.Console.output(e[1]);
                    });
                    // to do
                }
                Sen.Script.Executor.clock.stop_safe();
                Sen.Script.Console.finished(Sen.Script.format(Sen.Kernel.Language.get("batch.process.count"), argument.parameter.length));
                return;
            },
            is_enabled: false,
            configuration: undefined!,
            filter: ["file", /(.+).json$/gi],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Animation.Encode.forward();
