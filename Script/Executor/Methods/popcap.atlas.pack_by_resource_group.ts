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
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

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
            direct_forward(argument: Argument): void {
                is_valid_source(argument, true);
                Console.obtained(argument.source);
                defined_or_default(argument, "destination", `${Kernel.Path.dirname(argument.source)}`);
                Console.output(argument.destination!);
                argument.size = {} as any;
                argument.detail = {} as any;
                input_range(argument.size as any, "width", this.configuration.size, [64n, 16384n], Kernel.Language.get("popcap.atlas.pack.width"));
                input_range(argument.size as any, "height", this.configuration.size, [64n, 16384n], Kernel.Language.get("popcap.atlas.pack.height"));
                input_range(argument.size as any, "padding", this.configuration.size, [1n, 10n], Kernel.Language.get("popcap.atlas.pack.padding"));
                load_boolean(argument.detail as any, "smart", this.configuration.detail as any, Kernel.Language.get("popcap.atlas.pack.smart"));
                load_boolean(argument.detail as any, "pot", this.configuration.detail as any, Kernel.Language.get("popcap.atlas.pack.pot"));
                load_boolean(argument.detail as any, "allowRotation", this.configuration.detail as any, Kernel.Language.get("popcap.atlas.pack.allow_rotation"));
                load_boolean(argument.detail as any, "square", this.configuration.detail as any, Kernel.Language.get("popcap.atlas.pack.square"));
                argument.size.width = Number(argument.size.width);
                argument.size.height = Number(argument.size.height);
                argument.size.padding = Number(argument.size.padding);
                clock.start_safe();
                Support.PopCap.Atlas.Pack.ResourceGroup.process_fs(argument.source, argument.size, argument.detail, argument.destination!);
                clock.stop_safe();
                return;
            },
            batch_forward: undefined!,
            is_enabled: true,
            filter: ["directory", /(.+)\.sprite$/i],
        });
        return;
    }
}
Sen.Script.Executor.Methods.PopCap.Atlas.PackByResourceGroup.forward();
