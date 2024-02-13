namespace Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: [string, ...Array<string>];
        destination?: string;
        method?: Sen.Script.Support.PopCap.Atlas.Structure.TypicalMethod;
        style?: Sen.Script.Support.PopCap.Atlas.Structure.PathStyle;
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
        method: "?" | "path" | "id";
        style: "?" | "string" | "array";
    }

    /**
     * Detail
     */

    export namespace Detail {
        /**
         *
         * Typical Method
         *
         */

        export function method(): Array<[bigint, string, string]> {
            return [
                [1n, "id", Sen.Kernel.Language.get("popcap.atlas.split.method.id")],
                [2n, "path", Sen.Kernel.Language.get("popcap.atlas.split.method.path")],
            ];
        }

        /**
         *
         * Typical Style
         *
         */

        export function style(): Array<[bigint, string, string]> {
            return [
                [1n, "string", Sen.Kernel.Language.get("popcap.atlas.split.style.string")],
                [2n, "array", Sen.Kernel.Language.get("popcap.atlas.split.style.array")],
            ];
        }
    }

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {}
    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup.Argument,
            Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup.Configuration
        >({
            id: "popcap.atlas.split_by_resource_group",
            configuration_file: Home.query("~/Executor/Configuration/popcap.atlas.split_by_resource_group.json"),
            configuration: undefined!,
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup.Argument): void {
                if (!/\.json$/gi.test(argument.source[0])) {
                    throw new Error(format(Kernel.Language.get("popcap.atlas.split_by_resource_group.source_file_must_be_json"), argument.source[0]));
                }
                argument.source.forEach((e: string) => Sen.Script.Console.obtained(e));
                Sen.Script.Executor.defined_or_default(argument, "destination", `${Sen.Kernel.Path.except_extension(argument.source[0])}.sprite`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.argument_load(argument, "method", this.configuration, Detail.method(), Kernel.Language.get("popcap.atlas.split.method"));
                Sen.Script.Executor.argument_load(argument, "style", this.configuration, Detail.style(), Sen.Kernel.Language.get("popcap.atlas.split.style"));
                Sen.Script.Executor.clock.start_safe();
                Sen.Script.Support.PopCap.Atlas.Split.ResourceGroup.process_fs(argument.source, argument.destination!, argument.method!, argument.style!);
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            async_forward: undefined!,
            batch_forward: undefined!,
            is_enabled: true,
            filter: ["files", /(.+)\.json$/gi, /(.+)\.png$/gi],
        });
        return;
    }
}
Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup.forward();
