namespace Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        layout?: string;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
        layout?: string;
    }

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {
        layout: string;
    }

    /**
     * Detail namespace
     */

    export namespace Detail {
        /**
         * JS Implement
         * @param layout - Layout to exchange
         * @returns style
         */

        export function exchange_layout(layout: string): Support.PopCap.ResourceGroup.PathStyle {
            switch (layout) {
                case "string": {
                    return Support.PopCap.ResourceGroup.PathStyle.WindowStyle;
                }
                case "array": {
                    return Support.PopCap.ResourceGroup.PathStyle.ArrayStyle;
                }
                default: {
                    throw new Error(format(Kernel.Language.get("popcap.resource_group.convert.cannot_exchange_layout"), layout));
                }
            }
        }
        /**
         *
         * Typical Style
         *
         */

        export function style(): Array<[bigint, string, string]> {
            return [
                [1n, "string", Kernel.Language.get("popcap.resource_group.convert.layout.string")],
                [2n, "array", Kernel.Language.get("popcap.resource_group.convert.layout.array")],
            ];
        }
    }

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.Argument,
            Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.Configuration
        >({
            id: "popcap.resource_group.convert",
            configuration_file: Home.query("~/Executor/Configuration/popcap.resource_group.convert.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${Kernel.Path.dirname(argument.source)}/res.json`);
                Console.output(argument.destination!);
                load_bigint(argument, "layout", this.configuration, Detail.style(), Kernel.Language.get("popcap.atlas.split.style"));
                clock.start_safe();
                Kernel.Support.PopCap.ResourceGroup.convert_fs(argument.source, argument.destination!, Detail.exchange_layout(argument.layout!));
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)\.json$/i],
            option: 49n,
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.ResourceGroup.Convert.forward();
