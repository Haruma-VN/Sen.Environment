namespace Sen.Script.Executor.Methods.PopCap.CompiledText.Encode {
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
            Sen.Script.Executor.Methods.PopCap.CompiledText.Encode.Argument,
            Sen.Script.Executor.Methods.PopCap.CompiledText.Encode.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.CompiledText.Encode.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.CompiledText.Encode.Configuration
        >({
            id: "popcap.compiled_text.encode",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.compiled_text.encode.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.CompiledText.Encode.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.CompiledText.Encode.Argument, string>(argument, "destination", `${argument.source}.bin`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.load_string(argument, "key", this.configuration, Sen.Kernel.Language.get("popcap.compiled_text.encode.key"));
                Sen.Script.Executor.load_string(argument, "iv", this.configuration, Sen.Kernel.Language.get("popcap.compiled_text.encode.iv"));
                Sen.Script.Executor.load_boolean(argument, "use_64_bit_variant", this.configuration, Sen.Kernel.Language.get("popcap.compiled_text.encode.use_64_bit_variant"));
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.CompiledText.encode_fs(argument.source, argument.destination!, argument.key!, argument.iv!, argument.use_64_bit_variant!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.CompiledText.Encode.BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)\.bin$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.CompiledText.Encode.forward();
