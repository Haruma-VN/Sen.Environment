namespace Sen.Script.Executor.Methods.PvZ2.Modding.PackPacketContainResource {
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
            Sen.Script.Executor.Methods.PvZ2.Modding.PackPacketContainResource.Argument,
            Sen.Script.Executor.Methods.PvZ2.Modding.PackPacketContainResource.BatchArgument,
            Sen.Script.Executor.Methods.PvZ2.Modding.PackPacketContainResource.AsyncArgument,
            Sen.Script.Executor.Methods.PvZ2.Modding.PackPacketContainResource.Configuration
        >({
            id: "pvz2.modding.pack_packet_contain_resource",
            configuration_file: Home.query("~/Executor/Configuration/pvz2.modding.pack_packet_contain_resource.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${argument.source}.dummy`);
                Console.output(argument.destination!);
                clock.start_safe();
                Kernel.Support.Miscellaneous.Modding.pack_packet_contain_resource(argument.source, argument.destination!);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.*)\.pcr$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.PvZ2.Modding.PackPacketContainResource.forward();
