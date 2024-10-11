namespace Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: [string, ...Array<string>];
        destination?: string;
        split_method?: Sen.Script.Support.PopCap.Atlas.Structure.TypicalMethod;
        style?: Sen.Script.Support.PopCap.Atlas.Structure.PathStyle;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {}

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {
        split_method: "?" | "path" | "id";
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
                [1n, "id", Kernel.Language.get("popcap.atlas.split.method.id")],
                [2n, "path", Kernel.Language.get("popcap.atlas.split.method.path")],
            ];
        }

        /**
         *
         * Typical Style
         *
         */

        export function style(): Array<[bigint, string, string]> {
            return [
                [1n, "string", Kernel.Language.get("popcap.atlas.split.style.string")],
                [2n, "array", Kernel.Language.get("popcap.atlas.split.style.array")],
            ];
        }
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
            Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup.Argument,
            Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup.Configuration
        >({
            id: "popcap.atlas.split_by_resource_group",
            configuration_file: Home.query("~/Executor/Configuration/popcap.atlas.split_by_resource_group.json"),
            configuration: undefined!,
            direct_forward(argument: Argument): void {
                const jsons: Array<string> = argument.source.filter((e) => /\.json$/gi.test(e));
                if (jsons.length === 0) {
                    throw new Error(Kernel.Language.get("popcap.atlas.split_by_resource_group.source_file_must_be_json"));
                }
                const pngs: Array<string> = argument.source.filter((e) => /\.png$/gi.test(e));
                jsons.forEach((json: string) => {
                    const category: [string, ...Array<string>] = [json, ...pngs];
                    category.forEach((e: string) => Console.obtained(e));
                    defined_or_default(argument, "destination", `${Kernel.Path.except_extension(json)}.sprite`);
                    Console.output(argument.destination!);
                    load_bigint(argument, "split_method", this.configuration, Detail.method(), Kernel.Language.get("popcap.atlas.split.method"));
                    load_bigint(argument, "style", this.configuration, Detail.style(), Kernel.Language.get("popcap.atlas.split.style"));
                    clock.start_safe();
                    Support.PopCap.Atlas.Split.ResourceGroup.process_fs(category, argument.destination!, argument.split_method!, argument.style!);
                    clock.stop_safe();
                });
                return;
            },
            batch_forward: undefined!,
            is_enabled: true,
            filter: ["files", /(.+)\.json$/gi, /(.+)\.png$/gi],
            option: undefined!,
        });
        return;
    }
}
Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResourceGroup.forward();
