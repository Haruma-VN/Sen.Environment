namespace Sen.Script.Executor.Methods.PopCap.Animation.FromFlashAndEncode {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        has_label?: boolean;
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
            configuration_file: Home.query("~/Executor/Configuration/popcap.animation.from_flash_and_encode.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, true);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", Kernel.Path.except_extension(argument.source));
                Console.output(argument.destination!);
                load_boolean(argument, "has_label", this.configuration, Kernel.Language.get("popcap.animation.extract_label"));
                clock.start_safe();
                Kernel.Support.PopCap.Animation.Instance.from_flash(argument.source, argument.destination!, argument.has_label!);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
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
