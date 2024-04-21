namespace Sen.Script.Support.PopCap.Animation.Miscellaenous.GenerateAnimation {
    export type Canvas = Kernel.Canvas;
    export interface Setting {
        use_image_id: boolean;
        image_reslution: bigint;
        x_position?: number;
        y_position?: number;
        width?: bigint;
        height?: bigint;
        image_layers_remove_list?: Array<bigint>;
        sprite_layers_remove_list?: Array<bigint>;
        frame_name?: string;
        image_ratio?: number;
        apng_delay_frame?: bigint;
        apng_loop_count?: bigint;
    }

    export type Transform = [number, number, number, number, number, number];

    export type Color = [number, number, number, number];

    export interface AreaList {
        width: Array<number>;
        height: Array<number>;
        x_pos: Array<number>;
        y_pos: Array<number>;
    }

    export interface FrameList {
        frame_node_list: FrameNodeList;
        action_list: ActionList;
    }

    export interface FrameStruct {
        resource: bigint | null;
        is_sprite: boolean;
        transform_list?: Transform;
        color_list?: Color;
    }

    export interface Layer {
        name: string;
        frame: Record<string, Array<FrameStruct>>;
    }

    export interface AnimationStruct {
        source_image_name: Array<string>;
        source_layer: Array<FrameStruct>;
        sprite_layer: Array<Layer>;
        action_layer: Record<string, Record<string, Array<FrameStruct>>>;
        action_length: Record<string, bigint>;
    }

    export type SourceLayerData = Array<Kernel.Dimension.Image>;

    const initial_transform: number[] = [1.0, 0.0, 0.0, 1.0, 0.0, 0.0];

        const initial_color: number[] = [1.0, 1.0, 1.0, 1.0];

    export function read_image(image: AnimationImage, index: bigint): FrameStruct {
        return {
            resource: index,
            is_sprite: false,
            transform_list: [image.transform[0], image.transform[1], image.transform[2], image.transform[3], image.transform[4], image.transform[5]],
        } as FrameStruct;
    }

    export function to_transform(transform: number[]): Transform {
        if (transform.length !== 6) {
            throw new Error(format(Kernel.Language.get("popcap.animation.miscellaneous.to_apng.invalid_transform_length"), transform.length));
        }
        return [transform[0], transform[1], transform[2], transform[3], transform[4], transform[5]] as Transform;
    }

    export function to_color(color: number[]): Color {
        if (color.length !== 4) {
            throw new Error(format(Kernel.Language.get("popcap.animation.miscellaneous.to_apng.invalid_color_length"), color.length));
        }
        return [color[0], color[1], color[2], color[3]] as Color;
    }

    export function calculation_area(frame_struct: FrameStruct, transform_list: Transform, image_max_area: AreaList, image_source_list: SourceLayerData, animation_struct: AnimationStruct): void {
        const resource = Number(frame_struct.resource);
        if (frame_struct.is_sprite) {
            const frame = animation_struct.sprite_layer[resource].frame["1"][0];
            const transform = mix_transform(transform_list, frame.transform_list!);
            calculation_area(frame, transform, image_max_area, image_source_list, animation_struct);
        } else {
            const transform = mix_transform(transform_list, animation_struct.source_layer[resource].transform_list!);
            const image_width = transform[4] + Number(image_source_list[resource].width);
            const image_height = transform[5] + Number(image_source_list[resource].height);
            image_max_area.width.push(image_width);
            image_max_area.height.push(image_height);
            image_max_area.x_pos.push(transform[4]);
            image_max_area.y_pos.push(transform[5]);
        }
        return;
    }

    export function check_base_frame(transform: number[], color: number[], resource: bigint | null): boolean {
        for (let i = 0; i < transform.length; ++i) {
            if (transform[i] !== initial_transform[i]) {
                return true;
            }
        }
        for (let i = 0; i < color.length; ++i) {
            if (color[i] !== initial_color[i]) {
                return true;
            }
        }
        if (resource !== null) {
            return true;
        }
        return false;
    }

    export function variant_to_standard(transform: number[]): number[] {
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
                return [...transform];
            }
            default: {
                throw new Error(Kernel.Language.get("popcap.animation.from_animation.invalid_transform"));
            }
        }
    }

    export function decode_frame_list(animation_sprite: AnimationSprite, animation: SexyAnimation): FrameList {
        const sprite_model: SpriteModel = {};
        const frame_node_list: FrameNodeList = {
            "0": [
                {
                    index: 0n,
                    duration: BigInt(animation_sprite["frame"].length),
                    resource: null,
                    sprite: false,
                    transform: [...initial_transform],
                    color: [...initial_color],
                },
            ],
        };
        const frame_length: number = animation_sprite["frame"].length;
        let main_label: string = "";
        const action_list: ActionList = {};
        for (let i = 0; i < frame_length; ++i) {
            const label: string = animation_sprite["frame"][i]["label"];
            if (label !== null && label !== "") {
                if (main_label !== "") {
                    action_list[main_label]["duration"] = BigInt(i);
                }
                main_label = label;
            }
            if (main_label !== "") {
                if (!action_list.hasOwnProperty(main_label)) {
                    action_list[main_label] = {
                        start_index: BigInt(i),
                        duration: 0n,
                        frame_index: [],
                    };
                }
                if (i === frame_length - 1) {
                    action_list[main_label]["duration"] = BigInt(i + 1);
                }
            }
            const frame_removes: bigint[] = animation_sprite["frame"][i]["remove"];
            for (let k = 0; k < frame_removes.length; ++k) {
                sprite_model[`${frame_removes[k]}`]["state"] = false;
            }
            const frame_additives: AnimationAppend[] = animation_sprite["frame"][i]["append"];
            for (let k = 0; k < frame_additives.length; ++k) {
                sprite_model[`${frame_additives[k]["index"]}`] = {
                    state: null,
                    resource: frame_additives[k]["resource"],
                    sprite: frame_additives[k]["sprite"],
                    transform: [...initial_transform],
                    color: [...initial_color],
                    frame_start: BigInt(i),
                    frame_duration: BigInt(i),
                };
                frame_node_list[`${frame_additives[k]["index"] + 1n}`] = [];
                if (i > 0) {
                    frame_node_list[`${frame_additives[k]["index"] + 1n}`].push({
                        index: 0n,
                        duration: BigInt(i),
                        resource: null,
                        sprite: false,
                        transform: [...initial_transform],
                        color: [...initial_color],
                    });
                }
            }
            const frame_moves: AnimationMove[] = animation_sprite["frame"][i]["change"];
            for (let k = 0; k < frame_moves.length; ++k) {
                const frame_change: AnimationMove = frame_moves[k];
                const layers: Model = sprite_model[`${frame_change["index"]}`];
                layers["state"] = true;
                const static_transform: number[] = variant_to_standard(frame_change["transform"]);
                layers["transform"] = static_transform;
                if (
                    frame_change["color"] !== null &&
                    (frame_change["color"][0] !== initial_color[0] ||
                        frame_change["color"][1] !== initial_color[1] ||
                        frame_change["color"][2] !== initial_color[2] ||
                        frame_change["color"][3] !== initial_color[3])
                ) {
                    layers["color"] = frame_change["color"];
                }
            }
            const model_keys: string[] = Object.keys(sprite_model);
            for (let k = 0; k < model_keys.length; ++k) {
                const layers_index: bigint = BigInt(model_keys[k]);
                if (main_label !== "") {
                    if (!action_list[main_label]["frame_index"].includes(layers_index + 1n)) {
                        action_list[main_label]["frame_index"].push(layers_index + 1n);
                    }
                }
                const layers: Model = sprite_model[`${layers_index}`];
                const frame_node: FrameNode[] = frame_node_list[`${layers_index + 1n}`];
                if (layers["state"] !== null) {
                    if (frame_node.length > 0) {
                        frame_node[`${frame_node.length - 1}`]["duration"] = layers["frame_duration"];
                    }
                }
                if (layers["state"] === true) {
                    let first_frame: bigint = layers["sprite"] ? (BigInt(i) - layers["frame_start"]) % BigInt(animation.sprite[Number(layers["resource"])]["frame"].length) : 0n;
                    frame_node.push({
                        index: BigInt(i),
                        duration: 0n,
                        resource: layers["resource"],
                        sprite: layers["sprite"],
                        first_frame: first_frame,
                        transform: layers["transform"],
                        color: layers["color"],
                    });
                    layers["state"] = null;
                    layers["frame_duration"] = 0n;
                }
                if (layers["state"] === false) {
                    delete sprite_model[`${layers_index}`];
                }
                ++layers["frame_duration"];
            }
        }
        const model_keys: string[] = Object.keys(sprite_model);
        for (let i = 0; i < model_keys.length; ++i) {
            const layers_index: bigint = BigInt(model_keys[i]);
            const layer: Model = sprite_model[`${layers_index}`];
            const frame_node: FrameNode[] = frame_node_list[`${layers_index + 1n}`];
            frame_node[`${frame_node.length - 1}`]["duration"] = layer["frame_duration"];
            delete sprite_model[`${layers_index}`];
        }
        return {
            frame_node_list: frame_node_list,
            action_list: action_list,
        };
    }

    export function write_action(frame_list: FrameList): Record<string, Record<string, FrameNode[]>> {
        const action_list: ActionList = frame_list["action_list"];
        const action_keys: string[] = Object.keys(action_list);
        const action_node_list: Record<string, Record<string, FrameNode[]>> = {};
        const frame_node_list: FrameNodeList = frame_list["frame_node_list"];
        const frame_keys: string[] = Object.keys(frame_node_list);
        for (let label of action_keys) {
            const action_node: Action = action_list[label];
            const start_index: bigint = action_node["start_index"];
            const action_duration: bigint = action_node["duration"];
            action_node_list[label] = { "0": [] };
            const last_frames_list: bigint[] = [];
            let last_index: number = 0;
            for (let i = 0; i < frame_keys.length; ++i) {
                if (!action_node["frame_index"].includes(BigInt(i))) continue;
                const action_frame_node: FrameNode[] = [];
                const frame_node: FrameNode[] = frame_node_list[i];
                for (let k = 0; k < frame_node.length; ++k) {
                    const index: bigint = frame_node[k]["index"];
                    const duration: bigint = frame_node[k]["duration"];
                    if (index + duration <= start_index) continue;
                    if (index >= action_duration) break;
                    const frame_node_template = Kernel.Miscellaneous.make_copy<FrameNode>(frame_node[k]);
                    if (index + duration > action_duration) {
                        if (index < action_duration) {
                            if (index < start_index) {
                                frame_node_template["index"] = start_index;
                                frame_node_template["duration"] -= duration - action_duration - index + start_index;
                            } else {
                                frame_node_template["duration"] -= duration + index - action_duration;
                            }
                        }
                        frame_node_template["index"] -= start_index;
                        action_frame_node.push(frame_node_template);
                    } else {
                        if (index < start_index) {
                            frame_node_template["index"] = start_index;
                            frame_node_template["duration"] -= start_index - index;
                        }
                        frame_node_template["index"] -= start_index;
                        action_frame_node.push(frame_node_template);
                    }
                    last_frames_list.push(frame_node_template["index"] + frame_node_template["duration"]);
                }
                action_node_list[label][`${i}`] = action_frame_node;
                last_index = i;
            }
            const last_frame: bigint = BigIntMax(...last_frames_list);
            if (last_frame < action_duration - start_index) {
                action_node_list[label][`${last_index + 1}`] = [
                    {
                        index: 0n,
                        duration: action_duration - start_index,
                        resource: null,
                        sprite: false,
                        transform: [1.0, 0.0, 0.0, 1.0, 0.0, 0.0],
                        color: [1.0, 1.0, 1.0, 1.0],
                    },
                ];
            }
        }
        return action_node_list;
    }

    function BigIntMax(...args: bigint[]): bigint {
        return args.reduce((m, e) => (e > m ? e : m));
    }

    export function read_sprite_for_action(
        frame_node_list: FrameNodeList,
        image_max_area: AreaList,
        image_source_list: SourceLayerData,
        animation_struct: AnimationStruct,
    ): Record<string, Array<FrameStruct>> {
        const frame_keys = Object.keys(frame_node_list);
        const layers_list = {} as Record<string, Array<FrameStruct>>;
        for (let i = frame_keys.length - 1; i > 0; --i) {
            // const layer_index = frame_keys[i];
            const frame_node = frame_node_list[frame_keys[i]];
            const frame_list = [];
            for (let k = 0; k < frame_node.length; ++k) {
                const frame_struct = {
                    resource: null,
                    is_sprite: frame_node[k].sprite,
                } as FrameStruct;
                const transform = frame_node[k].transform;
                const color = frame_node[k].color;
                const resource = frame_node[k].resource;
                if (check_base_frame(transform, color, resource)) {
                    frame_struct.resource = resource;
                    frame_struct.transform_list = to_transform(transform);
                    frame_struct.color_list = to_color(color);
                    calculation_area(frame_struct, frame_struct.transform_list, image_max_area, image_source_list, animation_struct);
                }
                for (let h = 0n; h < frame_node[k].duration; ++h) {
                    frame_list.push(frame_struct);
                }
            }
            if (frame_list.length > 0) layers_list[`${i}`] = frame_list;
        }
        return layers_list;
    }

    export function read_sprite(frame_node_list: FrameNodeList): Record<string, Array<FrameStruct>> {
        const frame_keys = Object.keys(frame_node_list);
        const layers_list = {} as Record<string, Array<FrameStruct>>;
        for (let i = frame_keys.length - 1; i > 0; --i) {
            // const layer_index = frame_keys[i];
            const frame_node = frame_node_list[frame_keys[i]];
            const frame_list = [];
            for (let k = 0; k < frame_node.length; ++k) {
                const frame_struct = {
                    resource: null,
                    is_sprite: frame_node[k].sprite,
                } as FrameStruct;
                const transform = frame_node[k].transform;
                const color = frame_node[k].color;
                const resource = frame_node[k].resource;
                if (check_base_frame(transform, color, resource)) {
                    frame_struct.resource = resource;
                    frame_struct.transform_list = to_transform(transform);
                    frame_struct.color_list = to_color(color);
                }
                for (let h = 0n; h < frame_node[k].duration; ++h) {
                    frame_list.push(frame_struct);
                }
            }
            if (frame_list.length > 0) layers_list[`${i}`] = frame_list;
        }
        return layers_list;
    }

    export function mix_transform(source: Transform, change: Transform): Transform {
        return [
            change[0] * source[0] + change[2] * source[1],
            change[1] * source[0] + change[3] * source[1],
            change[0] * source[2] + change[2] * source[3],
            change[1] * source[2] + change[3] * source[3],
            change[0] * source[4] + change[2] * source[5] + change[4],
            change[1] * source[4] + change[3] * source[5] + change[5],
        ] as Transform;
    }

    export function mix_color(source: Color, change: Color): Color {
        return [change[0] * source[0], change[1] * source[1], change[2] * source[2], change[3] * source[3]] as Color;
    }

    export function composite_image(
        frame_index: number,
        canvas: Kernel.Canvas,
        frame: FrameStruct,
        transform_list: Transform,
        color_list: Color,
        animation_struct: AnimationStruct,
        source_layer_list: SourceLayerData,
        setting: Setting,
    ): void {
        const resource = Number(frame.resource);
        if (frame.is_sprite) {
            if (setting.sprite_layers_remove_list!.includes(frame.resource!)) return;
            const sprite = animation_struct.sprite_layer[resource].frame;
            for (const [layer_index, frame] of Object.entries(sprite)) {
                const last_frame = frame_index >= frame.length ? frame.length - 1 : frame_index;
                const transform = mix_transform(transform_list, frame[last_frame].transform_list!);
                const color = mix_color(color_list, frame[last_frame].color_list!);
                composite_image(frame_index, canvas, frame[last_frame], transform, color, animation_struct, source_layer_list, setting);
            }
            return;
        }
        if (setting.image_layers_remove_list!.includes(frame.resource!)) {
            return;
        }
        const transform = mix_transform(transform_list!, animation_struct.source_layer[resource].transform_list!);
        canvas.set_transform(transform[0], transform[1], transform[2], transform[3], transform[4] + setting.x_position!, transform[5] + setting.y_position!);
        const image = source_layer_list[Number(frame.resource)];
        let image_data = image.data;
        if (color_list[0] !== 1.0 || color_list[1] !== 1.0 || color_list[2] !== 1.0 || color_list[3] !== 1.0) {
            const new_image_data = new Uint8ClampedArray(Kernel.Miscellaneous.copyArrayBuffer(image_data));
            for (let i = 0; i < image_data.byteLength / 4; ++i) {
                if (color_list[0] !== 1.0) {
                    new_image_data[i * 4] = color_list[0];
                }
                if (color_list[1] !== 1.0) {
                    new_image_data[i * 4 + 1] *= color_list[1];
                }
                if (color_list[2] !== 1.0) {
                    new_image_data[i * 4 + 2] *= color_list[2];
                }
                if (color_list[3] !== 1.0) {
                    new_image_data[i * 4 + 3] *= color_list[3];
                }
            }
            image_data = new_image_data.buffer;
        }
        canvas.draw_image(image_data, image.width, image.height, image.width * 4n, 0, 0, Number(image.width) * setting.image_ratio!, Number(image.height) * setting.image_ratio!);
        return;
    }

    export function write_image(animation_struct: AnimationStruct, destination: string, source_layer_list: SourceLayerData, setting: Setting): void {
        const width = setting.width!;
        const height = setting.width!;
        const main_image_path_list = [] as Array<string>;
        const main_frame_list = [] as Array<bigint>;
        for (const [key, value] of Object.entries(animation_struct.action_layer)) {
            const image_path_list = [] as Array<string>;
            const frame_list = [] as Array<bigint>;
            Kernel.FileSystem.create_directory(`${destination}/${key}`);
            for (let i = 0; i < animation_struct.action_length[key]; i++) {
                const canvas = new Kernel.Canvas(width, height);
                for (const [layer_index, frame] of Object.entries(value)) {
                    if (i >= frame.length || frame[i].resource === null) continue;
                    composite_image(i, canvas, frame[i], frame[i].transform_list!, frame[i].color_list!, animation_struct, source_layer_list, setting);
                }
                const image_data = new ArrayBuffer(Number(width * height * 4n));
                canvas.get_image_data(image_data, width, height, width * 4n, 0n, 0n);
                const png_path = `${destination}/${key}/${setting.frame_name}_${i}.png`;
                Kernel.ImageView.write_fs(png_path, Kernel.ImageView.instance(width, height, image_data));
                image_path_list.push(png_path);
                frame_list.push(setting.apng_delay_frame!);
            }
            main_image_path_list.push(...image_path_list);
            main_frame_list.push(...frame_list);
            //make apng
            Kernel.Miscellaneous.to_apng(
                image_path_list,
                `${destination}/${key}.apng`,
                new Kernel.APNGMakerSetting(frame_list, new Kernel.UInteger32(setting.apng_loop_count), new Kernel.UInteger32(width), new Kernel.UInteger32(height), new Kernel.Boolean(false)),
            );
        }
        Kernel.Miscellaneous.to_apng(
            main_image_path_list,
            `${destination}/main.apng`,
            new Kernel.APNGMakerSetting(main_frame_list, new Kernel.UInteger32(setting.apng_loop_count), new Kernel.UInteger32(width), new Kernel.UInteger32(height), new Kernel.Boolean(false)),
        );
        return;
    }

    export function apply_setting(setting: Setting, image_max_area: Array<number>): void {
        setting.image_ratio = 1200 / Number(setting.image_reslution);
        if (setting.frame_name === undefined) {
            setting.frame_name = "frame";
        }
        if (setting.width === undefined) {
            setting.width = BigInt(Math.ceil(image_max_area[0]));
        }
        if (setting.height === undefined) {
            setting.height = BigInt(Math.ceil(image_max_area[1]));
        }
        if (setting.x_position === undefined) {
            setting.x_position = 0;
        }
        setting.x_position += Math.abs(image_max_area[2]);
        if (setting.y_position === undefined) {
            setting.y_position = 0;
        }
        setting.y_position += Math.abs(image_max_area[3]);
        if (setting.apng_delay_frame === undefined) {
            setting.apng_delay_frame = 30n;
        }
        if (setting.apng_loop_count === undefined) {
            setting.apng_loop_count = 0n;
        }
        if (setting.image_layers_remove_list === undefined) {
            setting.image_layers_remove_list = [];
        }
        if (setting.sprite_layers_remove_list === undefined) {
            setting.sprite_layers_remove_list = [];
        }
        return;
    }

    export type BaseWrapper = {
        image_source_list: SourceLayerData;
        image_max_area: Array<number>;
    };

    export function create_animation_struct(animation_struct: AnimationStruct, animation: SexyAnimation, media_source: string, setting: Setting): BaseWrapper {
        let image_index = 0n;
        const image_source_list = [] as SourceLayerData;
        const image_max_area = { width: [], height: [], x_pos: [], y_pos: [] } as AreaList;
        for (let image of animation.image) {
            const image_name = setting.use_image_id ? image.id : image.name;
            image_source_list.push(Kernel.Image.open(`${media_source}/${image_name}.png`));
            animation_struct.source_image_name.push();
            animation_struct.source_layer.push(read_image(image, image_index++));
        }
        for (let sprite of animation.sprite) {
            const frame_list = decode_frame_list(sprite, animation);
            animation_struct.sprite_layer.push({
                name: sprite.name,
                frame: read_sprite(frame_list.frame_node_list),
            });
        }
        const frame_list = decode_frame_list(animation.main_sprite, animation);
        const action_node_list = write_action(frame_list);
        for (const [key, value] of Object.entries(action_node_list)) {
            const action = read_sprite_for_action(value, image_max_area, image_source_list, animation_struct);
            animation_struct.action_layer[key] = action;
            animation_struct.action_length[key] = frame_list.action_list[key].duration - frame_list.action_list[key].start_index;
        }
        const max_width = Math.max(...image_max_area.width);
        const max_height = Math.max(...image_max_area.height);
        const min_x = Math.min(...image_max_area.x_pos);
        const min_y = Math.min(...image_max_area.y_pos);
        return {
            image_source_list,
            image_max_area: [max_width, max_height, min_x, min_y] as Array<number>,
        };
    }

    export function add_sprite_to_disable(animation: SexyAnimation, setting: Setting): void {
        Console.finished("All Sprites loaded");
        const sprite_length = animation.sprite.length;
        for (let i = 0; i < sprite_length; ++i) {
            Console.send(`     ${i + 1}: ${animation.sprite[i].name}`);
        }
        Console.argument(Kernel.Language.get("popcap.animation.miscellaneous.to_apng.input_sprite_to_disable"));
        let sprite_to_disable: string = "";
        const add_sprite = () => {
            const result = Executor.input_integer([0n, ...setting.sprite_layers_remove_list!]);
            const sprite_index = BigInt(result) - 1n;
            if (BigInt(result) === 0n) {
                return;
            }
            if (setting.sprite_layers_remove_list!.includes(sprite_index)) {
                Console.error(Kernel.Language.get("popcap.animation.miscellaneous.to_apng.sprite_already_selected"));
                add_sprite();
            } else {
                setting.sprite_layers_remove_list!.push(sprite_index);
                sprite_to_disable += `${result} `;
                add_sprite();
            }
        };
        add_sprite();
        Console.argument(`${Kernel.Language.get("popcap.animation.miscellaneous.to_apng.disable_sprite")}: ${sprite_to_disable}`);
        return;
    }

    export function before_start(animation: SexyAnimation, setting: Setting): void {
        Console.argument(Kernel.Language.get("popcap.animation.miscellaneous.to_apng.disable_sprite"));
        Console.send(`      1. ${Kernel.Language.get("popcap.animation.miscellaneous.to_apng.enable_all")}`);
        Console.send(`      2. ${Kernel.Language.get("popcap.animation.miscellaneous.to_apng.disable_all")}`);
        Console.send(`      3. ${Kernel.Language.get("popcap.animation.miscellaneous.to_apng.select_to_disable")}`);
        const result = Executor.input_integer([1n, 2n, 3n]);
        setting.sprite_layers_remove_list = [];
        switch (result) {
            case 1n: {
                break;
            }
            case 2n: {
                for (let i = 0; i < animation.sprite.length; ++i) {
                    setting.sprite_layers_remove_list!.push(BigInt(i));
                }
                break;
            }
            case 3n: {
                add_sprite_to_disable(animation, setting);
                break;
            }
        }
        return;
    }

    export function process(animation: SexyAnimation, media_source: string, destination: string, setting: Setting): void {
        before_start(animation, setting); //TODO rewrite this funciton;
        Kernel.FileSystem.create_directory(destination);
        const animation_struct = { source_image_name: [], source_layer: [], sprite_layer: [], action_layer: {}, action_length: {} } as AnimationStruct;
        const helper = create_animation_struct(animation_struct, animation, media_source, setting);
        // Kernel.JSON.serialize_fs(`${destination}/struct.json`, animation_struct, 1, true);
        apply_setting(setting, helper.image_max_area);
        write_image(animation_struct, destination, helper.image_source_list, setting);
        return;
    }

    export function process_fs(source: string, media_source: string, destination: string, setting: Setting): void {
        const animation: SexyAnimation = Kernel.JSON.deserialize_fs<SexyAnimation>(source);
        process(animation, media_source, destination, setting);
        return;
    }
}
