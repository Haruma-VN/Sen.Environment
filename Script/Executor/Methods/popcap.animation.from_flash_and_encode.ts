namespace Sen.Script.Executor.Methods.PopCap.Animation.FromFlashAndEncode {
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
            Sen.Script.Executor.Methods.PopCap.Animation.FromFlashAndEncode.Argument,
            Sen.Script.Executor.Methods.PopCap.Animation.FromFlashAndEncode.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.FromFlashAndEncode.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.FromFlashAndEncode.Configuration
        >({
            id: "popcap.animation.from_flash_and_encode",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.animation.from_flash_and_encode.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.FromFlashAndEncode.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                defined_or_default<Sen.Script.Executor.Methods.PopCap.Animation.FromFlashAndEncode.Argument, string>(argument, "destination", Kernel.Path.except_extension(argument.source));
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.Animation.Instance.from_flash(argument.source, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.Animation.FromFlashAndEncode.BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["directory", /(.+)\.xfl$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Animation.FromFlashAndEncode.forward();
