namespace Sen.Script.Executor.Methods.WWise.Media.Encode {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        format?: string;
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
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {
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
                [1n, "pcm", "16-bit Little Endian PCM (pcm)"],
                [2n, "adpcm", "Platinum 4-bit ADPCM (adpcm)"],
                [3n, "vorbis", "Custom Vorbis (vorbis)"],
                [4n, "wemopus", "libopus Opus (wemopus)"],
               // [5n, "acc", "AAC (Advanced Audio Coding) (acc)"]
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
            Sen.Script.Executor.Methods.WWise.Media.Encode.Argument,
            Sen.Script.Executor.Methods.WWise.Media.Encode.BatchArgument,
            Sen.Script.Executor.Methods.WWise.Media.Encode.AsyncArgument,
            Sen.Script.Executor.Methods.WWise.Media.Encode.Configuration
        >({
            id: "wwise.media.encode",
            configuration_file: Home.query("~/Executor/Configuration/wwise.media.encode.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                defined_or_default<Argument, string>(argument, "destination", `${Kernel.Path.except_extension(argument.source)}.wem`);
                Console.output(argument.destination!);
                load_bigint(argument, "format", this.configuration, Detail.format(), Kernel.Language.get("wwise.media.audio_format"));
                clock.start_safe();
                Sen.Script.Support.Wwise.Media.Encode.process_fs(argument.source, argument.destination!, argument.format! as Sen.Script.Support.Wwise.Media.Common.Format);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)\.wav$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.WWise.Media.Encode.forward();
