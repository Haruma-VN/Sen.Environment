namespace Sen.Script.Support.PopCap.Animation.Miscellaenous.GenerateFlashFromMedia {
    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {
        resolution: bigint;
    }

    /**
     * Detail
     */

    export namespace Detail {
        /**
         * Input resolution are supported covered here
         */

        export const resolution = (): Array<[bigint, bigint, string]> => {
            return [
                [1n, 1536n, Kernel.Language.get("popcap.animation.to_flash.resolution.1536n")],
                [2n, 768n, Kernel.Language.get("popcap.animation.to_flash.resolution.768n")],
                [3n, 384n, Kernel.Language.get("popcap.animation.to_flash.resolution.384n")],
                [4n, 1200n, Kernel.Language.get("popcap.animation.to_flash.resolution.1200n")],
                [5n, 640n, Kernel.Language.get("popcap.animation.to_flash.resolution.640n")],
            ];
        };

        export const has_label = (): Array<[bigint, bigint, string]> => {
            return [
                [1n, 1n, Kernel.Language.get("input.set_argument_to_true")],
                [2n, 0n, Kernel.Language.get("input.set_argument_to_false")],
            ];
        };
    }

    export function load_bigint(rule: any): bigint {
        const new_rule: Array<bigint> = [];
        rule.forEach(function make_rule(e: [bigint, string] & any): void {
            if (Shell.is_gui) {
                Kernel.Console.print(`${e[0]}. ${e[2]}`);
            } else {
                Kernel.Console.print(`    ${e[0]}. ${e[2]}`);
            }
            new_rule.push(e[0]);
        });
        return rule[Number(Sen.Script.Executor.input_integer(new_rule) - 1n)][1];
    }

    export function read_animation_name(): string {
        let animation_name = Kernel.Console.readline();
        let index = 0;
        while (index < animation_name.length) {
            if (animation_name[index] === " ") {
                Console.warning(Kernel.Language.get("script.helper.pvz2.animation.generate_flash_from_media.animation_name_must_not_cotain_space_char"));
                animation_name = Kernel.Console.readline();
                index = 0;
            } else if (animation_name.charCodeAt(index) > 127) {
                Console.warning(Kernel.Language.get("script.helper.pvz2.animation.generate_flash_from_media.animation_name_must_be_ascii"));
                animation_name = Kernel.Console.readline();
                index = 0;
            } else {
                ++index;
            }
        }
        return animation_name;
    }

    export function execute(): void {
        const source = Console.path(Kernel.Language.get("script.helper.pvz2.animation.generate_flash_from_media.input_media_path"), "directory");
        Console.argument(Kernel.Language.get("script.helper.pvz2.animation.generate_flash_from_media.enter_animation_name"));
        const animation_name = read_animation_name();
        const animation: SexyAnimation = {
            version: 6n,
            frame_rate: 30n,
            position: {
                x: 0,
                y: 0,
            },
            size: {
                width: 390,
                height: 390,
            },
            image: [],
            sprite: [],
            main_sprite: {
                name: "",
                work_area: {
                    start: 0n,
                    duration: 1n,
                },
                frame: [
                    {
                        label: "animation",
                        stop: false,
                        remove: [],
                        append: [],
                        change: [],
                        command: [],
                    },
                ],
            },
        };
        Console.argument(Kernel.Language.get("popcap.animation.to_flash.resolution"));
        const input_generic = load_bigint(Detail.resolution());
        Console.argument(Kernel.Language.get("popcap.animation.extract_label"));
        const has_label = load_bigint(Detail.has_label()) == 1n;
        const image_list = [];
        for (let e of Kernel.FileSystem.read_directory_only_file(source)) {
            if (Kernel.Path.extname(e).toLowerCase() === ".png") {
                image_list.push(e);
            }
        }
        const destination = `${Kernel.Path.dirname(source)}/${animation_name}.pam.xfl`;
        Console.output(destination);
        Console.output(`${Kernel.Language.get("script.helper.pvz2.animation.generate_flash_from_media.total_image_count")}: ${image_list.length}`);
        const k_resolution_ratio = 1200 / Number(input_generic);
        const k_default_transform: number[] = [1.0, 0.0, 0.0, 1.0, 0.0, 0.0];
        Kernel.FileSystem.create_directory(`${destination}/library/media`);
        const image_dulicate: Record<string, bigint> = {};
        for (let e of image_list) {
            const image = Kernel.Image.open(e);
            const image_width = BigInt(Math.round(Number(image.width) * k_resolution_ratio));
            const image_height = BigInt(Math.round(Number(image.height) * k_resolution_ratio));
            let image_dimension = `${image_width}x${image_height}`;
            if (image_dulicate.hasOwnProperty(image_dimension)) {
                image_dimension += `_${++image_dulicate[image_dimension]}`;
            } else {
                image_dulicate[image_dimension] = 1n;
            }
            animation.image.push({
                path: `${animation_name}_${image_dimension}`,
                id: `image_${animation_name}_${animation_name}_${image_dimension}`.toUpperCase(),
                dimension: {
                    width: image_width,
                    height: image_height,
                },
                transform: k_default_transform,
            });
            Kernel.Image.write(`${destination}/library/media/${animation_name}_${image_dimension}.png`, image);
        }
        Kernel.Support.PopCap.Animation.ToFlash.process(animation, destination, input_generic, has_label);
        return;
    }
}

Sen.Script.Support.PopCap.Animation.Miscellaenous.GenerateFlashFromMedia.execute();
