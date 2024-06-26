namespace Sen.Script.Executor.Methods.PopCap.RSBPatch.Decode {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        patch_file?: string;
        destination?: string;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {}

    /**
     * Async support
     */

    export interface AsyncArgument extends Sen.Script.Executor.Base {}

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
            Sen.Script.Executor.Methods.PopCap.RSBPatch.Decode.Argument,
            Sen.Script.Executor.Methods.PopCap.RSBPatch.Decode.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.RSBPatch.Decode.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.RSBPatch.Decode.Configuration
        >({
            id: "popcap.rsb_patch.decode",
            configuration_file: Home.query("~/Executor/Configuration/popcap.rsb_patch.decode.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                argument.patch_file = Console.path(Kernel.Language.get("popcap.rsb_patch.encode.after_file"), "file");
                defined_or_default<Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.rsb`);
                Console.output(argument.destination!);
                clock.start_safe();
                Kernel.Support.PopCap.RSBPatch.decode_fs(argument.source, argument.patch_file!, argument.destination!);
                clock.stop_safe();
                return;
            },
            batch_forward: undefined!,
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)(\.rsb|\.obb)$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.RSBPatch.Decode.forward();
