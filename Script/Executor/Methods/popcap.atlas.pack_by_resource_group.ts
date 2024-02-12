namespace Sen.Script.Executor.Methods.PopCap.Atlas.PackByResourceGroup {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        size: Sen.Script.Support.PopCap.Atlas.Pack.Detail.SizeRange<number>;
        detail: Sen.Script.Support.PopCap.Atlas.Pack.Detail.Data;
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

    export interface Configuration extends Sen.Script.Executor.Configuration {
        size: Sen.Script.Support.PopCap.Atlas.Pack.Detail.SizeRange<number>;
        detail: Sen.Script.Support.PopCap.Atlas.Pack.Detail.Data;
    }

    export namespace Detail {
        /**
         * Only size allowance in this list
         */

        export const dimension: Array<bigint> = [128n, 256n, 512n, 1024n, 2048n, 4096n, 8192n, 16384n];

        /**
         * Padding allowance in this list
         */

        export const padding: Array<bigint> = [1n, 2n, 3n, 4n, 5n, 6n, 7n, 8n, 9n, 10n];
    }

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {}
    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.Atlas.PackByResourceGroup.Argument,
            Sen.Script.Executor.Methods.PopCap.Atlas.PackByResourceGroup.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Atlas.PackByResourceGroup.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Atlas.PackByResourceGroup.Configuration
        >({
            id: "popcap.atlas.pack_by_resource_group",
            configuration_file: Home.query("~/Executor/Configuration/popcap.atlas.pack_by_resource_group.json"),
            configuration: undefined!,
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Atlas.PackByResourceGroup.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default(argument, "destination", `${Sen.Kernel.Path.dirname(argument.source)}`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.argument_load(argument.size as any, "width", this.configuration.size, Detail.dimension, Kernel.Language.get("popcap.atlas.pack.width"));
                Sen.Script.Executor.argument_load(argument.size as any, "height", this.configuration.size, Detail.dimension, Sen.Kernel.Language.get("popcap.atlas.pack.height"));
                Sen.Script.Executor.argument_load(argument.size as any, "padding", this.configuration.size, Detail.padding, Sen.Kernel.Language.get("popcap.atlas.pack.padding"));
                Sen.Script.Executor.clock.start_safe();
                Sen.Script.Support.PopCap.Atlas.Pack.ResourceGroup.process_fs(argument.source, argument.size, argument.detail, argument.destination!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            async_forward: undefined!,
            batch_forward: undefined!,
            is_enabled: true,
            filter: ["directory", /(.+)\.sprite$/gi],
        });
        return;
    }
}
Sen.Script.Executor.Methods.PopCap.Atlas.PackByResourceGroup.forward();
