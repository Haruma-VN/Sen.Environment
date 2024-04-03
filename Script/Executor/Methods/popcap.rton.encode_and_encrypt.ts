namespace Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt {
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
            Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.Argument,
            Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.Configuration
        >({
            id: "popcap.rton.encode_and_encrypt",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.rton.encode_and_encrypt.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                defined_or_default<Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.rton`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.load_string(argument, "key", this.configuration, Sen.Kernel.Language.get("popcap.rton.decrypt.key"));
                Sen.Script.Executor.load_string(argument, "iv", this.configuration, Sen.Kernel.Language.get("popcap.rton.decrypt.iv"));
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.RTON.encode_and_encrypt_fs(argument.source, argument.destination!, argument.key!, argument.iv!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)\.json$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RTON.EncodeAndEncrypt.forward();
