namespace Sen.Script.Executor.Methods.PopCap.Animation.FromFlash {
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
            Sen.Script.Executor.Methods.PopCap.Animation.FromFlash.Argument,
            Sen.Script.Executor.Methods.PopCap.Animation.FromFlash.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.FromFlash.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.FromFlash.Configuration
        >({
            id: "popcap.animation.from_flash",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.animation.from_flash.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.FromFlash.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.Animation.FromFlash.Argument, string>(
                    argument,
                    "destination",
                    `${Sen.Kernel.Path.except_extension(argument.source)}.pam.json`,
                );
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.clock.start_safe();
                Script.Support.PopCap.Animation.ToAnimation.process_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.FromFlash.BatchArgument): void {
                const files: Array<string> = Sen.Kernel.FileSystem.read_directory_only_directory(argument.directory).filter((path: string) => this.filter[1].test(path));
                files.forEach((source: string) => this.direct_forward({ source: source }));
                Sen.Script.Console.finished(Sen.Script.format(Sen.Kernel.Language.get("batch.process.count"), files.length));
                return;
            },
            async_forward: undefined,
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /(.+).xfl$/gi],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Animation.FromFlash.forward();
