namespace Sen.Script.Executor.Methods.PopCap.Animation.ToFlash {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        resolution?: bigint;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
        resolution?: bigint;
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

    export interface Configuration extends Sen.Script.Executor.Configuration {
        resolution: bigint;
    }

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.Argument,
            Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.Configuration
        >({
            id: "popcap.animation.to_flash",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.animation.to_flash.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.Argument, string>(
                    argument,
                    "destination",
                    `${Sen.Kernel.Path.except_extension(argument.source)}.xfl`,
                );
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.argument_load(
                    argument,
                    "resolution",
                    this.configuration,
                    [
                        [1n, "1536", Kernel.Language.get("popcap.animation.to_flash.resolution.1536n")],
                        [2n, "768", Kernel.Language.get("popcap.animation.to_flash.resolution.768n")],
                        [3n, "384", Kernel.Language.get("popcap.animation.to_flash.resolution.384n")],
                        [4n, "1200", Kernel.Language.get("popcap.animation.to_flash.resolution.1200n")],
                        [5n, "640", Kernel.Language.get("popcap.animation.to_flash.resolution.640n")],
                    ],
                    Sen.Kernel.Language.get("popcap.animation.to_flash.resolution"),
                );
                Sen.Script.Executor.clock.start_safe();
                Script.Support.PopCap.Animation.FromAnimation.process_fs(argument.source, argument.destination!, BigInt(argument.resolution!));
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.BatchArgument): void {
                Sen.Script.Executor.argument_load(
                    argument,
                    "resolution",
                    this.configuration,
                    [
                        [1n, "1536", Kernel.Language.get("popcap.animation.to_flash.resolution.1536n")],
                        [2n, "768", Kernel.Language.get("popcap.animation.to_flash.resolution.768n")],
                        [3n, "384", Kernel.Language.get("popcap.animation.to_flash.resolution.384n")],
                        [4n, "1200", Kernel.Language.get("popcap.animation.to_flash.resolution.1200n")],
                        [5n, "640", Kernel.Language.get("popcap.animation.to_flash.resolution.640n")],
                    ],
                    Sen.Kernel.Language.get("popcap.animation.to_flash.resolution"),
                );
                const files: Array<string> = Sen.Kernel.FileSystem.read_directory(argument.directory).filter((path: string) => Sen.Kernel.FileSystem.is_file(path) && this.filter[1].test(path));
                files.forEach((source: string) => this.direct_forward({ source: source, resolution: BigInt(argument.resolution!) }));
                Sen.Script.Console.finished(Sen.Script.format(Sen.Kernel.Language.get("batch.process.count"), files.length));
                return;
            },
            async_forward: undefined,
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+).json$/gi],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Animation.ToFlash.forward();
