namespace Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        layout?: string;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
        layout?: string;
    }

    /**
     * Async support
     */

    export interface AsyncArgument extends Sen.Script.Executor.Base {
        parameter: Array<[string, string]>;
        layout: string;
    }

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {
        layout: string;
    }

    export namespace Detail {
        export function exchange_layout(layout: string): Sen.Script.Support.PopCap.ResourceGroup.PathStyle {
            switch (layout) {
                case "string": {
                    return Sen.Script.Support.PopCap.ResourceGroup.PathStyle.WindowStyle;
                }
                case "array": {
                    return Sen.Script.Support.PopCap.ResourceGroup.PathStyle.ArrayStyle;
                }
                default: {
                    throw new Error(format(Kernel.Language.get("popcap.resource_group.convert.cannot_exchange_layout"), layout));
                }
            }
        }
        /**
         *
         * Typical Style
         *
         */

        export function style(): Array<[bigint, string, string]> {
            return [
                [1n, "string", Sen.Kernel.Language.get("popcap.resource_group.convert.layout.string")],
                [2n, "array", Sen.Kernel.Language.get("popcap.resource_group.convert.layout.array")],
            ];
        }
    }

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.Argument,
            Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.Configuration
        >({
            id: "popcap.resource_group.convert",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.resource_group.convert.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${Kernel.Path.dirname(argument.source)}/res.json`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.load_bigint(argument, "layout", this.configuration, Detail.style(), Sen.Kernel.Language.get("popcap.atlas.split.style"));
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.ResourceGroup.convert_fs(argument.source, argument.destination!, Detail.exchange_layout(argument.layout!));
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.BatchArgument): void {
                const files: Array<string> = Sen.Kernel.FileSystem.read_directory(argument.directory).filter((path: string) => Sen.Kernel.FileSystem.is_file(path));
                files.forEach((source: string) => this.direct_forward({ source: source }));
                Sen.Script.Console.finished(Sen.Script.format(Sen.Kernel.Language.get("batch.process.count"), files.length));
                return;
            },
            async_forward(argument: Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.AsyncArgument): void {
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

Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.forward();
