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
                defined_or_default<Executor.Methods.PopCap.Animation.FromFlash.Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.json`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.clock.start_safe();
                Kernel.Support.PopCap.Animation.FromFlash.convert_fs(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.FromFlash.BatchArgument): void {
                return basic_batch(this, argument, true);
            },
            async_forward: undefined,
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /(.+)\.xfl$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Animation.FromFlash.forward();
