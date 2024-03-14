namespace Sen.Script.Support.PopCap.Animation.Miscellaenous.GenerateAnimation {
    export interface Setting {
        use_image_id: boolean;
        image_reslution: bigint;
        x_position?: bigint;
        y_position?: bigint;
        width?: bigint;
        height?: bigint;
        image_layers_remove_list?: Array<bigint>;
        sprite_layers_remove_list?: Array<bigint>;
        frame_name?: string;
    }

    export type Transform = [number, number, number, number, number, number];

    export type Color = [number, number, number, number];

    export interface FrameStruct {
        resource: bigint | null;
        is_sprite: boolean;
        transform_list?: Transform;
        color_list?: Color;
    }

    export interface AnimationStruct {
        source_image_name: Array<string>;
        source_layer: Array<FrameStruct>;
        sprite_layer: Record<string, Array<Array<FrameStruct>>>;
        action_layer: Record<string, Array<Array<FrameStruct>>>;
    }

    export function read_image(image: Structure.AnimationImage, index: bigint): FrameStruct {
        return {
            resource: index,
            is_sprite: false,
            transform_list: image.transform,
        } as FrameStruct;
    }

    export function to_transform(transform: number[]): Transform {
        if (transform.length !== 6) {
            throw new Error("invalid_transform_length");
        }
        return [transform[0], transform[1], transform[2], transform[3], transform[4], transform[5]] as Transform;
    }

    export function to_color(color: number[]): Color {
        if (color.length !== 4) {
            throw new Error("invalid_color_length");
        }
        return [color[0], color[1], color[2], color[3]] as Color;
    }

    export function read_sprite(frame_node_list: FrameNodeList): Array<Array<FrameStruct>> {
        const frame_keys = Object.keys(frame_node_list);
        const layers_list = [];
        for (let i = frame_keys.length - 1; i > 0; --i) {
            const layer_index = BigInt(frame_keys[i]);
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
                if (FromAnimation.check_base_frame(transform, color, resource)) {
                    frame_struct.resource = resource;
                    frame_struct.transform_list = to_transform(transform);
                    frame_struct.color_list = to_color(color);
                }
                for (let h = 0n; h < frame_node[k].duration; ++h) {
                    frame_list.push(frame_struct);
                }
            }
            layers_list.push(frame_list);
        }
        return layers_list as Array<Array<FrameStruct>>;
    }

    export function create_animation_struct(animation_struct: AnimationStruct, animation: SexyAnimation, setting: Setting): void {
        let image_index = 0n;
        for (let image of animation.image) {
            animation_struct.source_image_name.push(setting.use_image_id ? image.id : image.name);
            animation_struct.source_layer.push(read_image(image, image_index++));
        }
        for (let sprite of animation.sprite) {
            const frame_list = FromAnimation.decode_frame_list(sprite, animation);
            animation_struct.sprite_layer[sprite.name] = read_sprite(frame_list.frame_node_list);
        }
        const frame_list = FromAnimation.decode_frame_list(animation.main_sprite, animation);
        const action_node_list = FromAnimation.write_action(frame_list);
        for (const [key, value] of Object.entries(action_node_list)) {
            animation_struct.action_layer[key] = read_sprite(value);
        }
        return;
    }

    export function write_image() {}

    export function process(animation: SexyAnimation, destination: string, setting: Setting) {
        const animation_struct = { source_image_name: [], source_layer: [], sprite_layer: {}, action_layer: {} } as AnimationStruct;
        create_animation_struct(animation_struct, animation, setting);
        // write_image();
        return;
    }

    export function process_fs(source: string, destination: string, setting: Setting) {
        const animation: SexyAnimation = Kernel.JSON.deserialize_fs<SexyAnimation>(source);
        process(animation, destination, setting);
        return;
    }
}
