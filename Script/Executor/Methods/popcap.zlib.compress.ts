namespace Sen.Script.Executor.Methods.PopCap.Zlib.Compress {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        use_64_bit_variant?: boolean;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
        use_64_bit_variant?: boolean;
    }

    /**
     * Async support
     */

    export interface AsyncArgument extends Sen.Script.Executor.Base {
        parameter: Array<[string, string]>;
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
            Sen.Script.Executor.Methods.PopCap.Zlib.Compress.Argument,
            Sen.Script.Executor.Methods.PopCap.Zlib.Compress.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Zlib.Compress.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Zlib.Compress.Configuration
        >({
            id: "popcap.zlib.compress",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.zlib.compress.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Zlib.Compress.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.Zlib.Compress.Argument, string>(argument, "destination", `${argument.source}.bin`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.load_boolean(argument, "use_64_bit_variant", this.configuration, Sen.Kernel.Language.get("popcap.zlib.compress.use_64_bit_variant"));
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.Zlib.compress_fs(argument.source, argument.destination!, argument.use_64_bit_variant!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.Zlib.Compress.BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)\.(rsb|bin|xml)$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Zlib.Compress.forward();
