namespace Sen.Script.Executor.Methods.PopCap.Animation.Decode {
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
            Sen.Script.Executor.Methods.PopCap.Animation.Decode.Argument,
            Sen.Script.Executor.Methods.PopCap.Animation.Decode.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.Decode.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.Decode.Configuration
        >({
            id: "popcap.animation.decode",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.animation.decode.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.Decode.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.Animation.Decode.Argument, string>(argument, "destination", `${argument.source}.json`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.Animation.decode_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.Decode.BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            async_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.Decode.AsyncArgument): void {
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
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)\.pam$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Animation.Decode.forward();
