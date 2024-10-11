namespace Sen.Script.Executor.Methods.PopCap.CompiledText.Decode {
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
            Sen.Script.Executor.Methods.PopCap.CompiledText.Decode.Argument,
            Sen.Script.Executor.Methods.PopCap.CompiledText.Decode.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.CompiledText.Decode.Configuration
        >({
            id: "popcap.compiled_text.decode",
            configuration_file: Home.query("~/Executor/Configuration/popcap.compiled_text.decode.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${argument.source}.bin`);
                Console.output(argument.destination!);
                load_string(argument, "key", this.configuration, Kernel.Language.get("popcap.compiled_text.decode.key"));
                load_string(argument, "iv", this.configuration, Kernel.Language.get("popcap.compiled_text.decode.iv"));
                load_boolean(argument, "use_64_bit_variant", this.configuration, Kernel.Language.get("popcap.compiled_text.decode.key"));
                clock.start_safe();
                Kernel.Support.PopCap.CompiledText.decode_fs(argument.source, argument.destination!, argument.key!, argument.iv!, argument.use_64_bit_variant!);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)\.txt$/i],
            option: 19n,
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.CompiledText.Decode.forward();
