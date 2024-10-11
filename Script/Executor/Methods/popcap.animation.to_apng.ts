namespace Sen.Script.Executor.Methods.PopCap.Animation.ToAPNG {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
        destination?: string;
        media?: string;
    }
    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {}

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {
        resolution: bigint;
    }

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.PopCap.Animation.ToAPNG.Argument,
            Sen.Script.Executor.Methods.PopCap.Animation.ToAPNG.BatchArgument,
            Sen.Script.Executor.Methods.PopCap.Animation.ToAPNG.Configuration
        >({
            id: "popcap.animation.to_apng",
            configuration_file: Home.query("~/Executor/Configuration/popcap.animation.to_apng.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                Console.output(argument.source!);
                defined_or_default<Argument, string>(argument, "destination", Kernel.Path.resolve(`${argument.source}.animation`)); // TODO: change dest folder name
                if (argument.media === undefined) {
                    argument.media = Console.path(Kernel.Language.get("popcap.animation.to_apng.input_media"), "directory");
                }
                const setting: Support.PopCap.Animation.Miscellaenous.GenerateAnimation.Setting = {
                    image_id: false,
                    frame_name: "frame",
                    sprite_disable: [], // nothing to do
                    background_color: [0n, 0n, 0n, 0n], //black with fully transparent,
                    rendering_size: {
                        width: 0n, // if width, height = 0 use pre-calculated area
                        height: 0n,
                        scale: 1, // scaling of the animation, it cannot <= 0
                    },
                    position_additional: {
                        x: 0,
                        y: 0,
                    },
                    apng_setting: {
                        make_apng: true,
                        split_label: true,
                        frame_rate: 0n,
                        loop: 0n, // 0 - infinity
                    },
                }; // TODO: make setting json
                const animation: Sen.Script.Support.PopCap.Animation.SexyAnimation = Kernel.JSON.deserialize_fs<Sen.Script.Support.PopCap.Animation.SexyAnimation>(argument.source!);
                Support.PopCap.Animation.Miscellaenous.GenerateAnimation.exchange_sprite_disable(animation, setting);
                Console.output(`Total animation frames: ${animation.main_sprite.frame.length}`); // TODO
                clock.start_safe();
                Support.PopCap.Animation.Miscellaenous.GenerateAnimation.process(animation, argument.media!, argument.destination!, setting);
                clock.stop_safe();
                return;
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /.*(\.pam\.json)$/i],
            option: 13n,
        });
        return;
    }
}

Sen.Script.Executor.Methods.PopCap.Animation.ToAPNG.forward();
