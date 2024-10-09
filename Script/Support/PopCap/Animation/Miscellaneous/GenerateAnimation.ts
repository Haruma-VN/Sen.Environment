namespace Sen.Script.Support.PopCap.Animation.Miscellaenous.GenerateAnimation {
    export type Canvas = Kernel.Canvas;

    export interface PostionAdditional {
        /**  */
        x: number;
        /**  */
        y: number;
    }

    /**
    * 
    */
    export interface RenderingSize {
        /**  */
        width: bigint;
        /**  */
        height: bigint;
        /**  */
        scale: number;
    }

    export interface AnimateImageSetting {
        /**  */
        make_apng: boolean;
        /**  */
        split_label: boolean;
        /**  */
        frame_rate: bigint;
        /**  */
        loop: bigint;
    }

    export interface Setting {
        /**  */
        image_id: boolean,
        /** */
        frame_name: string;
        /**  */
        sprite_disable: Array<bigint>;
        /**  */
        background_color: [bigint, bigint, bigint, bigint], // r, g, b, a
        /**  */
        rendering_size: RenderingSize;
        /**  */
        position_additional: PostionAdditional;
        /** */
        apng_setting: AnimateImageSetting;
    }

    export type Matrix = [number, number, number, number, number, number];

    export type Color = [number, number, number, number];

    export interface MediaSource {
        size: AnimationSize;
        matrix: Matrix;
        image: Kernel.Dimension.Image;
    }

    export interface VisualLayer {
        resource: number;
        matrix: Matrix;
        color: Color;
        //children: Record<number, VisualLayer>;
        sprite_frame: number | null;
    }

    export interface Rectangle {
        left: number;
        top: number;
        right: number;
        bottom: number;
    }

    export interface LabelInfo {
        frame_start: bigint;
        frame_end: bigint;
    };

    export type DataInfo = Record<string, LabelInfo>;

    export type VisualSpriteFrame = Array<Record<number, VisualLayer>>;

    const k_initial_matrix: Matrix = [1.0, 0.0, 0.0, 1.0, 0.0, 0.0];

    const k_initial_color: Color = [1.0, 1.0, 1.0, 1.0];

    /**
     * Detail namespace
     */

    export namespace Detail {
        export function sprite_generic(): Array<[bigint, bigint, string]> {
            return [
                [1n, 0n, Kernel.Language.get("popcap.animation.miscellaneous.to_apng.enable_all")], // TODO: add localization.
                [2n, 1n, Kernel.Language.get("popcap.animation.miscellaneous.to_apng.disable_all")],
                [3n, 2n, Kernel.Language.get("popcap.animation.miscellaneous.to_apng.select_to_disable")],
            ];
        }
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
        return (rule)[Number(Sen.Script.Executor.input_integer(new_rule) - 1n)][1];
    }

    export function exchange_sprite_disable(animation: SexyAnimation, setting: Setting) {
        const sprite_information_map: Record<string, boolean> = {};
        animation.sprite.map(e => {
            sprite_information_map[e.name] = false;
        });
        const sprite_list = Object.keys(sprite_information_map);
        if (sprite_list.length >= 0) {
            Console.finished("All sprites loaded"); // TODO: add locale
            sprite_list.map((e, i) => {
                Kernel.Console.print(`    ${i + 1}: ${e}`);
            });
            Console.argument(Kernel.Language.get("popcap.animation.miscellaneous.to_apng.disable_sprite")); // TODO: add localization.
            const input_generic = load_bigint(Detail.sprite_generic());
            switch (input_generic) {
                case 0n: {
                    break;
                }
                case 1n: {
                    sprite_list.map((e, i) => {
                        setting.sprite_disable.push(BigInt(i));
                    });
                    break;
                }
                case 2n: {
                    const rule: Array<bigint> = [0n];
                    sprite_list.map((e, i) => {
                        rule.push(BigInt(i + 1));
                    });
                    let sprite_to_disable: string = "";
                    const argument = (rule: Array<bigint>) => {
                        const input: string = Kernel.Console.readline();
                        const check_vaild = (range: Array<bigint>) => {
                            if (range.length > 0) {
                                return;
                            }
                            Console.warning(Sen.Kernel.Language.get("js.invalid_input_value"));
                        }
                        let range: Array<bigint> = [];
                        /*
                        if (input.includes(" ")) {
                            range = input.split(" ").filter( e => !isNaN(Number(e)) && rule.includes(BigInt(e))).map(e => BigInt(e)); 
                            check_vaild(range);
                        }
                        */
                        if (input.includes(",")) {
                            range = input.split(",").filter(e => !isNaN(Number(e)) && rule.includes(BigInt(e))).map(e => BigInt(e));
                            check_vaild(range);
                        }
                        else if (input.includes("-")) {
                            const range_selected = input.split("-").filter(e => !isNaN(Number(e)) && rule.includes(BigInt(e))).map(e => BigInt(e));
                            if (range_selected.length == 2) {
                                for (let i = range_selected[0]; i <= range_selected[1]; ++i) {
                                    range.push(i);
                                }
                            }
                            check_vaild(range);
                        }
                        else {
                            range = [input].filter(e => !isNaN(Number(e)) && rule.includes(BigInt(e))).map(e => BigInt(e));
                            check_vaild(range);
                        }
                        return range;
                    }
                    while (true) {
                        Console.argument(`${Kernel.Language.get("popcap.animation.miscellaneous.to_apng.input_sprite_to_disable")}`); // TODO: add locale
                        const argument_result: Array<bigint> = argument(rule);
                        const selected_list: Array<bigint> = [];
                        for (const index_result of argument_result) {
                            if (index_result === 0n) {
                                continue;
                            }
                            if (sprite_information_map[sprite_list[Number(index_result - 1n)]]) {
                                selected_list.push(index_result);
                                continue;
                            }
                            else {
                                sprite_information_map[sprite_list[Number(index_result - 1n)]] = true;
                            }
                            sprite_to_disable += sprite_to_disable === "" ? `${index_result}` : `, ${index_result}`;
                        }
                        if (selected_list.length > 0) {
                            Console.warning(`${Kernel.Language.get("popcap.animation.miscellaneous.to_apng.sprite_already_selected")}: ${selected_list.join(", ")}`); //TODO: add locale
                        }
                        if (argument_result.includes(0n)) {
                            Console.finished(`${Kernel.Language.get("popcap.animation.miscellaneous.to_apng.disable_sprite")}: ${sprite_to_disable}`); // TODO: add locale
                            break;
                        }
                        Console.send(`${Kernel.Language.get("popcap.animation.miscellaneous.to_apng.disable_sprite")}: ${sprite_to_disable}`); // TODO: add locale
                    }
                    sprite_list.map((e, i) => {
                        if (sprite_information_map[e]) {
                            setting.sprite_disable.push(BigInt(i));
                        }
                    });
                    break;
                }
            }
        }
        else {
            Console.finished("Animation has no sprite"); //TODO: add locale
        }
        return;
    }

    export function variant_to_matrix(transform: number[]): Matrix {
        switch (transform.length) {
            case 2: {
                return [1.0, 0.0, 0.0, 1.0, transform[0], transform[1]];
            }
            case 3: {
                const cos: number = Math.cos(transform[0]);
                const sin: number = Math.sin(transform[0]);
                return [cos, sin, -sin, cos, transform[1], transform[2]];
            }
            case 6: {
                return [...transform] as Matrix;
            }
            default: {
                throw new Error(Kernel.Language.get("popcap.animation.from_animation.invalid_transform")); //TODO: add locale
            }
        }
    }

    export function mix_matrix(source: Matrix, change: Matrix) {
        return [
            change[0] * source[0] + change[2] * source[1],
            change[1] * source[0] + change[3] * source[1],
            change[0] * source[2] + change[2] * source[3],
            change[1] * source[2] + change[3] * source[3],
            change[0] * source[4] + change[2] * source[5] + change[4],
            change[1] * source[4] + change[3] * source[5] + change[5],
        ] as Matrix;
    }

    export function mix_color(source: Color, change: Color) {
        return [change[0] * source[0], change[1] * source[1], change[2] * source[2], change[3] * source[3]] as Color;
    }

    export function load_media_source(animation: SexyAnimation, media_source: string, setting: Setting) {
        const media_source_list: Array<MediaSource> = [];
        const scale_image = (image: Kernel.Dimension.Image) => {
            const percent = setting.rendering_size.scale;
            if (percent !== 1) {
                const new_width = BigInt(Math.round(Number(image.width) * percent));
                const new_height = BigInt(Math.round(Number(image.height) * percent));
                const resized_image_data = new Uint8ClampedArray(Number(new_width * new_height * 4n));
                const source_data = new Uint8ClampedArray(Kernel.Miscellaneous.copyArrayBuffer(image.data));
                for (let j = 0n; j < new_height; j++) {
                    for (let i = 0n; i < new_width; i++) {
                        const old_pixel_height = BigInt(Math.round(Number(j) / percent));
                        const old_pixel_width = BigInt(Math.round(Number(i) / percent));
                        const old_index = Number((old_pixel_height * image.width + old_pixel_width) * 4n);
                        const new_index = Number((j * new_width + i) * 4n);
                        for (let k = 0; k < 4; k++) {
                            resized_image_data[new_index + k] = source_data[old_index + k];
                        }
                    }
                }
                image.width = new_width;
                image.height = new_height;
                image.data = resized_image_data.buffer;
            }
            return image;
        };
        for (const image of animation.image) {
            media_source_list.push({
                size: {
                    width: Number(image.dimension.width),
                    height: Number(image.dimension.height),
                },
                matrix: variant_to_matrix(image.transform),
                image: scale_image(Kernel.Image.open(`${media_source}/${setting.image_id ? image.id : image.path}.png`))
            });
        }
        return media_source_list;
    }

    export function visualize_sprite(sprite: AnimationSprite, setting: Setting) {
        const visual_frame_sprite: VisualSpriteFrame = [];
        const layer_list: Record<number, VisualLayer> = {};
        for (const frame of sprite.frame) {
            for (const remove_index of frame.remove) {
                delete layer_list[Number(remove_index)];
            }
            for (const append of frame.append) {
                if (append.sprite && setting.sprite_disable.includes(append.resource)) {
                    continue;
                }
                const layer: VisualLayer = {
                    resource: Number(append.resource),
                    matrix: k_initial_matrix,
                    color: k_initial_color,
                    sprite_frame: append.sprite ? 0 : null
                };
                layer_list[Number(append.index)] = layer;
            }
            for (const layer_index of Object.keys(layer_list).map(e => Number(e))) {
                const layer = layer_list[layer_index];
                if (layer.sprite_frame !== null) {
                    ++layer.sprite_frame;
                }
            }
            for (const change of frame.change) {
                if (!layer_list.hasOwnProperty(Number(change.index))) {
                    continue;
                }
                const layer = layer_list[Number(change.index)];
                if (layer.sprite_frame !== null) {
                    layer.sprite_frame = 0; // reset.
                }
                layer.matrix = variant_to_matrix(change.transform);
                if (change.color !== null) {
                    layer.color = change.color;
                }
            }
            visual_frame_sprite.push(Kernel.Miscellaneous.make_copy(layer_list));
        }
        return visual_frame_sprite;
    }

    export function draw_image(canvas: Kernel.Canvas, layer: VisualLayer, sprite_list: Array<VisualSpriteFrame>, media_source: Array<MediaSource>, setting: Setting) {
        if (layer.sprite_frame !== null) {
            const sprite = sprite_list[layer.resource];
            let frame_index = layer.sprite_frame;
            while (frame_index >= sprite.length) {
                frame_index -= sprite.length;
            }
            const sprite_frame = sprite[frame_index];
            for (const layer_index of Object.keys(sprite_frame).map(e => Number(e))) {
                const viusal_layer: VisualLayer = {...sprite_frame[layer_index]};
                viusal_layer.matrix = mix_matrix(viusal_layer.matrix, layer.matrix);
                viusal_layer.color = mix_color(viusal_layer.color, layer.color);
                draw_image(canvas, viusal_layer, sprite_list, media_source, setting);
            }
        }
        else {
            const media = media_source[layer.resource];
            const matrix = mix_matrix(media.matrix, layer.matrix);
            const resize_matrix = mix_matrix(matrix, [setting.rendering_size.scale, 0, 0, setting.rendering_size.scale, 0, 0]);
            canvas.set_transform(resize_matrix[0], resize_matrix[1], resize_matrix[2], resize_matrix[3], resize_matrix[4] + setting.position_additional.x, resize_matrix[5] + setting.position_additional.y);
            canvas.set_image_color(layer.color[0], layer.color[1], layer.color[2], layer.color[3]);
            canvas.draw_image(media.image.data, media.image.width, media.image.height, media.image.width * 4n, 0, 0, media.size.width, media.size.height);
        }
    }

    export function color_clamped(color: [bigint, bigint, bigint, bigint] | Color) {
        const clamped = (data: number | bigint) => {
            let c = 0;
            if (typeof data === "bigint") {
                c = Number(Math.max(0, Math.min(255, c))) / 255;
            }
            else { 
                c = data;
            }
            return Math.max(0, Math.min(255, c));
        }
        return [clamped(color[0]), clamped(color[1]), clamped(color[2]), clamped(color[3])];
    }

    export function write_frames(visual_sprite_frame: VisualSpriteFrame, sprite_list: Array<VisualSpriteFrame>, media_source: Array<MediaSource>,  destination: string, setting: Setting) {
        const width = setting.rendering_size.width;
        const height = setting.rendering_size.height;
        Kernel.FileSystem.create_directory(destination);
        for (const frame_index in visual_sprite_frame) {
            const layer_list = visual_sprite_frame[frame_index];
            const canvas = new Kernel.Canvas(width, height);
            const background_color = color_clamped(setting.background_color);
            canvas.set_color(0n, background_color[0], background_color[1], background_color[2], background_color[3]);
            canvas.fill_rectangle(0, 0, Number(width), Number(height));
            for (const layer_index of Object.keys(layer_list).map(e => Number(e))) {
                const layer = layer_list[layer_index];
                draw_image(canvas, layer, sprite_list, media_source, setting);
            }
            const image_path = `${destination}/${setting.frame_name!}_${Number(frame_index) + 1}.png`;
            const image_data = new ArrayBuffer(Number(width * height * 4n));
            canvas.get_image_data(image_data, width, height, width * 4n, 0n, 0n);
            Kernel.ImageView.write_fs(image_path, Kernel.ImageView.instance(width, height, image_data));
        }
        return;
    }

    export function exchange_area(visual_sprite_frame: VisualSpriteFrame, sprite_list: Array<VisualSpriteFrame>, media_source: Array<MediaSource>, setting: Setting) {
        const rectangle: Rectangle = {
            left: Infinity,
            top: Infinity,
            right: -Infinity,
            bottom: -Infinity
        };
        const clamped = (data: number) => {
            return data < 0 ? 0 : data;
        }
        const calculator_dimension = (dimension: AnimationSize, matrix: Matrix) => {
            return {
                width: clamped(matrix[0] * dimension.width + matrix[1] * dimension.height),
                height: clamped(matrix[2] * dimension.width + matrix[3] * dimension.height),
            } as AnimationSize;
        }
        const rect_image = (layer: VisualLayer) => {
            if (layer.sprite_frame !== null) {
                const sprite = sprite_list[layer.resource];
                let frame_index = layer.sprite_frame;
                while (frame_index >= sprite.length) {
                    frame_index -= sprite.length;
                }
                const sprite_frame = sprite[frame_index];
                for (const layer_index of Object.keys(sprite_frame).map(e => Number(e))) {
                    const viusal_layer: VisualLayer = {...sprite_frame[layer_index]};
                    viusal_layer.matrix = mix_matrix(viusal_layer.matrix, layer.matrix);
                    viusal_layer.color = mix_color(viusal_layer.color, layer.color);
                    rect_image(viusal_layer);
                }
            }
            else {
                const media = media_source[layer.resource];
                const matrix = mix_matrix(media.matrix, layer.matrix);
                const resize_matrix = mix_matrix(matrix, [setting.rendering_size.scale, 0, 0, setting.rendering_size.scale, 0, 0]);
                const after_apply_matrix_to_media_dimension = calculator_dimension(media.size, resize_matrix);
                rectangle.left = Math.min(rectangle.left, resize_matrix[4]);
                rectangle.top = Math.min(rectangle.top, resize_matrix[5]);
                rectangle.right = Math.max(rectangle.right, resize_matrix[4] + after_apply_matrix_to_media_dimension.width);
                rectangle.bottom = Math.max(rectangle.bottom, resize_matrix[5] + after_apply_matrix_to_media_dimension.height);
            }
        }
        for (const frame_index in visual_sprite_frame) {
            const layer_list = visual_sprite_frame[frame_index];
            for (const layer_index of Object.keys(layer_list).map(e => Number(e))) {
                const layer = layer_list[layer_index];
                rect_image(layer);
            }
        }
        setting.position_additional.x += -rectangle.left;
        setting.position_additional.y += -rectangle.top;
        setting.rendering_size.width = BigInt(Math.ceil(-rectangle.left + rectangle.right));
        setting.rendering_size.height = BigInt(Math.ceil(-rectangle.top + rectangle.bottom));
        return;
    }

    export function exchange_label(sprite: AnimationSprite) {
        const data_info: DataInfo = {};
        let label_name = "animation";
        for (const frame_index in sprite.frame) {
            if (sprite.frame[frame_index].label !== "") {
                label_name = sprite.frame[frame_index].label;
                data_info[label_name] = {
                    frame_start: BigInt(frame_index) + 1n, 
                    frame_end: BigInt(frame_index) + 1n
                }
            }
            else {
                ++data_info[label_name].frame_end;
            }
        }
        return data_info;
    }

    //public:
    export function process(animation: SexyAnimation, media_source: string, destination: string, setting: Setting) {
        if (setting.rendering_size.scale < 0n) {
            setting.rendering_size.scale = 1;
        }
        const media_source_list = load_media_source(animation, media_source, setting);
        const visual_sprite_list: Array<VisualSpriteFrame> = [];
        for (const sprite of animation.sprite) {
            visual_sprite_list.push(visualize_sprite(sprite, setting));
        }
        const main_visual_sprite = visualize_sprite(animation.main_sprite, setting);
        if (setting.rendering_size.width <= 0n && setting.rendering_size.height <= 0n) {
            exchange_area(main_visual_sprite, visual_sprite_list, media_source_list, setting);
        }
        Console.output(`Animation Width: ${setting.rendering_size.width}`); // TODO
        Console.output(`Animation Height: ${setting.rendering_size.height}`);
        write_frames(main_visual_sprite, visual_sprite_list, media_source_list, `${destination}/frames`, setting);
        const definition: DataInfo = exchange_label(animation.main_sprite);
        Kernel.JSON.serialize_fs(`${destination}/data.json`, definition, 1, true);
        if (setting.apng_setting.make_apng) {
            const frame_rate = setting.apng_setting.frame_rate <= 0n ? animation.frame_rate : setting.apng_setting.frame_rate;
            const write_apng = (path_list: Array<string>, destination: string, frame_list: Array<bigint>) => {
                Kernel.Miscellaneous.to_apng(
                    path_list, destination,
                    new Kernel.APNGMakerSetting(frame_list, new Kernel.UInteger32(setting.apng_setting.loop), new Kernel.UInteger32(setting.rendering_size.width), new Kernel.UInteger32(setting.rendering_size.height), new Kernel.Boolean(false)),
                );
            }
            if (setting.apng_setting.split_label) {
                const main_animation_path_list: Array<string> = [];
                const main_animation_frame_list: Array<bigint> = [];
                for (const [label_name, label_info] of Object.entries(definition)) {
                    const label_path_list: Array<string> = [];
                    const label_frame_list: Array<bigint> = [];
                    for (let i = label_info.frame_start; i <= label_info.frame_end; i++) {
                        label_path_list.push(`${destination}/frames/${setting.frame_name}_${i}.png`);
                        label_frame_list.push(frame_rate);
                    }
                    write_apng(label_path_list, `${destination}/label/${label_name}.apng`, label_frame_list);
                    main_animation_path_list.push(...label_path_list);
                    main_animation_frame_list.push(...label_frame_list);
                }
                write_apng(main_animation_path_list, `${destination}/animation.apng`, main_animation_frame_list);
            }
            else {
                const main_animation_path_list: Array<string> = [];
                const main_animation_frame_list: Array<bigint> = [];
                for (const frame_index in main_visual_sprite) {
                    main_animation_path_list.push(`${destination}/frames/${setting.frame_name}_${Number(frame_index) + 1}.png`);
                    main_animation_frame_list.push(frame_rate);
                }
                write_apng(main_animation_path_list, `${destination}/animation.apng`, main_animation_frame_list);
            }
        }
        return;
    }
}
