namespace Sen.Script.Executor.Methods.PopCap.RTON.Decrypt {
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
            Sen.Script.Executor.Methods.PopCap.RTON.Decrypt.Argument,
            Sen.Script.Executor.Methods.PopCap.RTON.Decrypt.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RTON.Decrypt.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RTON.Decrypt.Configuration
        >({
            id: "popcap.rton.decrypt",
            configuration_file: Sen.Script.Home.query("~/Executor/Configuration/popcap.rton.decrypt.json"),
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.RTON.Decrypt.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default<Sen.Script.Executor.Methods.PopCap.RTON.Decrypt.Argument, string>(argument, "destination", `${argument.source}.bin`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.load_string(argument, "key", this.configuration, Sen.Kernel.Language.get("popcap.rton.decrypt.key"));
                Sen.Script.Executor.load_string(argument, "iv", this.configuration, Sen.Kernel.Language.get("popcap.rton.decrypt.iv"));
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.PopCap.RTON.decrypt_fs(argument.source, argument.destination!, argument.key!, argument.iv!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            batch_forward(argument: Sen.Script.Executor.Methods.PopCap.RTON.Decrypt.BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)(\.rton|pp\.dat)$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RTON.Decrypt.forward();
