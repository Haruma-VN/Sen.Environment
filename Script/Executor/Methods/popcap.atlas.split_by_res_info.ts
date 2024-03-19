namespace Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResInfo {
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

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResInfo.Argument,
            Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResInfo.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResInfo.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResInfo.Configuration
        >({
            id: "popcap.atlas.split_by_res_info",
            configuration_file: Home.query("~/Executor/Configuration/popcap.atlas.split_by_res_info.json"),
            configuration: undefined!,
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResInfo.Argument): void {
                const jsons: Array<string> = argument.source.filter((e) => /\.json$/gi.test(e));
                if (jsons.length === 0) {
                    throw new Error(Kernel.Language.get("popcap.atlas.split_by_res_info.source_file_must_be_json"));
                }
                const pngs: Array<string> = argument.source.filter((e) => /\.png$/gi.test(e));
                jsons.forEach((json: string) => {
                    const category: [string, ...Array<string>] = [json, ...pngs];
                    category.forEach((e: string) => Sen.Script.Console.obtained(e));
                    Sen.Script.Executor.defined_or_default(argument, "destination", `${Sen.Kernel.Path.except_extension(json)}.sprite`);
                    Sen.Script.Console.output(argument.destination!);
                    Sen.Script.Executor.load_bigint(argument, "method", this.configuration, Detail.method(), Kernel.Language.get("popcap.atlas.split.method"));
                    Sen.Script.Executor.clock.start_safe();
                    Sen.Script.Support.PopCap.Atlas.Split.ResInfo.process_fs(category, argument.destination!, argument.method!);
                    Sen.Script.Executor.clock.stop_safe();
                });
                return;
            },
            async_forward: undefined!,
            batch_forward: undefined!,
            is_enabled: true,
            filter: ["files", /(.+)\.json$/i, /(.+)\.png$/i],
        });
        return;
    }
}
Sen.Script.Executor.Methods.PopCap.Atlas.SplitByResInfo.forward();
