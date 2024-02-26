namespace Sen.Script.Executor.Methods.PopCap.PTX.Decode {
    /**
     * Dimension
     */

    export interface Dimension extends Record<string, unknown> {
        width: bigint;
        height: bigint;
    }

    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        size?: Dimension;
        format?: string;
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
        size: Dimension | "?";
        format: Support.Texture.Format | "?";
    }

    export namespace Detail {
        /**
         *
         * Typical Style
         *
         */

        export function format(): Array<[bigint, string, string]> {
            return [
                [1n, "argb_8888", "argb_8888 (0, iOS)"],
                [2n, "rgba_8888", "rgba_8888 (0, Android)"],
                [3n, "rgba_4444", "rgba_4444 (1, Android & iOS)"],
                [4n, "rgb_565", "rgb_565 (2, Android)"],
                [5n, "rgba_5551", "(3, Android & iOS)"],
                [6n, "rgba_4444_tiled", "(21, Android & iOS)"],
                [7n, "rgb_565_tiled", "(22, Android)"],
                [8n, "rgba_5551_tiled", "(23, Android & iOS)"],
                [9n, "rgba_pvrtc4", "(30, iOS)"],
                [10n, "rgb_etc1_a_8", "(147, Android)"],
                [11n, "rgb_etc1_a_palette", `(147, ${Kernel.Language.get("pvz2_android_cn")})`],
                [12n, "rgb_pvrtc4_a_8", "(148, iOS)"],
            ];
        }

        /**
         * JS Implement
         * @param layout - Layout to exchange
         * @returns style
         */

        export function exchange_format(m_format: string): Support.Texture.Format {
            switch (m_format) {
                case "argb_8888": {
                    return Support.Texture.Format.ARGB_8888;
                }
                case "rgba_8888": {
                    return Support.Texture.Format.RGBA_8888;
                }
                case "rgba_4444": {
                    return Support.Texture.Format.RGBA_4444;
                }
                case "rgb_565": {
                    return Support.Texture.Format.RGB_565;
                }
                case "rgba_5551": {
                    return Support.Texture.Format.RGBA_5551;
                }
                case "rgba_4444_tiled": {
                    return Support.Texture.Format.RGBA_4444_TILED;
                }
                case "rgb_565_tiled": {
                    return Support.Texture.Format.RGB_565_TILED;
                }
                case "rgba_5551_tiled": {
                    return Support.Texture.Format.RGBA_5551_TILED;
                }
                case "rgba_pvrtc4": {
                    return Support.Texture.Format.RGBA_PVRTC_4BPP;
                }
                case "rgb_etc1_a_8": {
                    return Support.Texture.Format.RGB_ETC1_A_8;
                }
                case "rgb_etc1_a_palette": {
                    return Support.Texture.Format.RGB_ETC1_A_PALETTE;
                }
                case "rgb_pvrtc4_a_8": {
                    return Support.Texture.Format.RGB_PVRTC_4BPP_A_8;
                }
                default: {
                    throw new Error(Script.format(Kernel.Language.get("popcap.ptx.unsupported_format"), format));
                }
            }
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
            Sen.Script.Executor.Methods.PopCap.PTX.Decode.Argument,
            Sen.Script.Executor.Methods.PopCap.PTX.Decode.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.PTX.Decode.AsyncArgument,
            Sen.Script.Executor.Methods.PopCap.PTX.Decode.Configuration
        >({
            id: "popcap.ptx.decode",
            configuration_file: Home.query("~/Executor/Configuration/popcap.ptx.decode.json"),
            configuration: undefined!,
            direct_forward(argument: Sen.Script.Executor.Methods.PopCap.PTX.Decode.Argument): void {
                Sen.Script.Console.obtained(argument.source);
                Sen.Script.Executor.defined_or_default(argument, "destination", `${Sen.Kernel.Path.except_extension(argument.source)}.png`);
                Sen.Script.Console.output(argument.destination!);
                Sen.Script.Executor.load_bigint(argument, "format", this.configuration, Detail.format(), Sen.Kernel.Language.get("popcap.ptx.decode.format"));
                argument.size = {} as any;
                Sen.Script.Executor.input_range(argument.size as any, "width", this.configuration.size as Dimension, [1n, 16384n], Kernel.Language.get("popcap.ptx.decode.width"));
                if (argument.format === "rgb_pvrtc4_a_8") {
                    (this.configuration.size as Dimension).height = argument.size!.width;
                }
                Sen.Script.Executor.input_range(argument.size as any, "height", this.configuration.size as Dimension, [1n, 16384n], Sen.Kernel.Language.get("popcap.ptx.decode.height"));
                Sen.Script.Executor.clock.start_safe();
                Sen.Kernel.Support.Texture.decode_fs(argument.source, argument.destination!, argument!.size!.width, argument!.size!.height, Detail.exchange_format(argument.format!));
                Sen.Script.Executor.clock.stop_safe();
                return;
            },
            async_forward: undefined!,
            batch_forward: undefined!,
            is_enabled: true,
            filter: ["file", /(.+)\.ptx$/gi],
        });
        return;
    }
}
Sen.Script.Executor.Methods.PopCap.PTX.Decode.forward();
