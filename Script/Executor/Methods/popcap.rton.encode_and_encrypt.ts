namespace Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        key?: string;
        iv?: string;
        use_64_bit_variant?: boolean;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
        key?: string;
        iv?: string;
        use_64_bit_variant?: boolean;
    }

    /**
     * Async support
     */

    export interface AsyncArgument extends Sen.Script.Executor.Base {
        parameter: Array<[string, string]>;
        key?: string;
        iv?: string;
        use_64_bit_variant?: boolean;
    }

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {
        key?: string;
        iv?: string;
        use_64_bit_variant?: boolean;
    }

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.Argument,
            Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.Configuration
        >({
            id: "popcap.rton.encode_and_encrypt",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.rton.encode_and_encrypt.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                defined_or_default<Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.rton`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.load_string(argument, "key", this.configuration, Sen.Kernel.Language.get("popcap.rton.decrypt.key"));
                Sen.Script.Executor.load_string(argument, "iv", this.configuration, Sen.Kernel.Language.get("popcap.rton.decrypt.iv"));
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.RTON.encode_and_encrypt_fs(argument.source, argument.destination!, argument.key!, argument.iv!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.BatchArgument): void {
                const files: Array<string> = Sen.Kernel.FileSystem.read_directory(argument.directory).filter((path: string) => Sen.Kernel.FileSystem.is_file(path));
                files.forEach((source: string) => this.direct_forward({ source: source }));
                Sen.Script.Console.finished(Sen.Script.format(Sen.Kernel.Language.get("batch.process.count"), files.length));
                return;
            },
            async_forward(argument: Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.AsyncArgument): void {
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
                }
                Sen.Script.Executor.clock.stop_safe();
                Sen.Script.Console.finished(Sen.Script.format(Sen.Kernel.Language.get("batch.process.count"), argument.parameter.length));
                return;
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)\.json$/gi],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.forward();
