namespace Sen.Script.Support.PopCap.Animation.Miscellaenous.AnimationFromFrames {

    export type DataInfo = Sen.Script.Support.PopCap.Animation.Miscellaenous.GenerateAnimation.DataInfo;

    export type Rectangle = Sen.Script.Support.PopCap.Animation.Miscellaenous.GenerateAnimation.Rectangle;

    export type PostionAdditional = Sen.Script.Support.PopCap.Animation.Miscellaenous.GenerateAnimation.PostionAdditional;

    /*
    export interface Rectangle {
        x: number;
        y: number;
        width: number;
        height: number;
    }
        */

    export interface Setting {   // TODO: add configuration
        animation_name: string;     // argument
        atlas_dimension: AnimationDimension;
        position_additional: PostionAdditional;
    }

    export interface FrameHelper {
        layer_index: bigint;
        remove_list: Array<bigint>;
    };

    export interface ImageInfo {
        size: AnimationSize,
        data: ArrayBuffer
    } 

    const k_initial_matrix = [1.0, 0.0, 0.0, 1.0, 0.0, 0.0];

    const k_default_atlas_size = 4096n;

    export function find_non_transparent_bounds(source_data: DataView, width: number, height: number) {
        const rectangle: Rectangle = {
            left: width,
            top: height,
            right: 0,
            bottom: 0
        };
        for (let j = 0; j < height; j++) {
            for (let i = 0; i < width; i++) {
                const index = (j * width + i) * 4;
                if (source_data.getUint8(index + 3) !== 0) {
                    rectangle.left = Math.min(rectangle.left, i);
                    rectangle.top = Math.min(rectangle.top, j);
                    rectangle.right = Math.max(rectangle.right, i);
                    rectangle.bottom = Math.max(rectangle.bottom, j);
                }
            }
        }
        return rectangle;
    }

    /*

    const k_initial_width = 64;

    const k_initial_height = 64;

    export function divide_mod(size: number) {
        return k_initial_width - (size % k_initial_width) + size;
    }

    */

    export function exchange_frame(image: Kernel.Dimension.Image, frame: AnimationFrame, frame_helper: FrameHelper, source_list: Array<ImageInfo>, setting: Setting) {
        const source_data = new DataView(image.data);
        const rectangle = find_non_transparent_bounds(source_data, Number(image.width), Number(image.height));
        const dimension: AnimationSize = {
            width: rectangle.right - rectangle.left,
            height: rectangle.bottom - rectangle.top
        }
        frame_helper.remove_list = [];
        let pixel_index = 0;
        const image_source_data = new DataView(new ArrayBuffer(dimension.width * dimension.height * 4));
        const rectangle_y = rectangle.top;
        const rectangle_x = rectangle.left;
        for (let j = rectangle_y; j < rectangle_y + dimension.height; j++) {
            for (let i = rectangle_x; i < rectangle_x + dimension.width; i++) {
                const index = (j * Number(image.width) + i) * 4;
                if (index > source_data.byteLength - 4) {
                    break;
                }
                image_source_data.setUint32(pixel_index, source_data.getUint32(index));
                pixel_index += 4;
            }
        }
        let index = source_list.length;
        for (let i = 0; i < source_list.length; i++) {
            if (Kernel.Miscellaneous.compareArrayBuffer(image_source_data.buffer, source_list[i].data)) {
                index = i;
                break;
            }
        }
        if (index === source_list.length) {
            source_list.push({
                size: dimension,
                data: image_source_data.buffer
            });
        }
        frame.append.push({
            index: frame_helper.layer_index,
            name: "",
            resource: BigInt(index),
            sprite: false,
            additive: false,
            preload_frame: 0n,
            time_scale: 1.0
        });
        frame.change.push({
            index: frame_helper.layer_index,
            transform: [
                rectangle_x + setting.position_additional.x,
                rectangle_y + setting.position_additional.y
            ],
            color: null,
            sprite_frame_number: 0n,
            source_rectangle: null
        });
        frame_helper.remove_list.push(frame_helper.layer_index);
        ++frame_helper.layer_index;
        return;
    }

    export function process(source: string, destination: string, setting: Setting) { 
        const definition: DataInfo = Kernel.JSON.deserialize_fs<DataInfo>(`${source}/data.json`);
        const animation: SexyAnimation = {
            version: 6n, // always 6
            frame_rate: definition.frame_rate,
            position: { x: 0, y: 0 },
            size: { width: 390, height: 390 },
            image: [],
            sprite: [],
            main_sprite: {
                name: "",
                work_area: { start: 0n, duration: 0n },
                frame: []
            }
        };
        const source_list: Array<ImageInfo> = [];
        const frame_helper: FrameHelper = {
            layer_index: 0n,
            remove_list: []
        }
        for (const [label_name, label_info] of Object.entries(definition.label)) {
            assert(label_info.frame_end > label_info.frame_start, "frame_end_must_be_greater_than_frame_start"); // TODO 
            for (let i = label_info.frame_start; i <= label_info.frame_end; ++i) {
                const image_path = `${source}/frames/${definition.frame_name}_${i}.png`;
                const image = Kernel.Image.open(image_path);
                assert(image.width === definition.dimension.width, "mismatch_width_at: " + `${image_path}`);
                assert(image.height === definition.dimension.height, "mismatch_height_at: " + `${image_path}`); // TODO 
                const frame: AnimationFrame = {
                    label: i === label_info.frame_start ? label_name : "",
                    stop: i === label_info.frame_end,
                    command: [],
                    remove: frame_helper.remove_list,
                    append: [],
                    change: []
                };
                exchange_frame(image, frame, frame_helper, source_list, setting);
                animation.main_sprite.frame.push(frame);
            }
        }
        const animation_name = setting.animation_name;
        const atlases: Script.Support.PopCap.Atlas.Structure.Definition = {
            method: "path",
            expand_path: "array",
            subgroup: `${animation_name}`,
            trim: false,
            res: "1200",
            groups: {}
        }
        Kernel.FileSystem.create_directory(`${destination}/${animation_name}.sprite/media`);
        for (let i = 0; i < source_list.length; i++) {
            const source = source_list[i];
            const image = {
                path: `${animation_name}_${i + 1}`,
                id: `IMAGE_${animation_name}_${animation_name}_${i + 1}`.toUpperCase(),
                dimension: {
                    width: BigInt(source.size.width),
                    height: BigInt(source.size.height)
                },
                transform: k_initial_matrix
            };
            atlases.groups[image.id] = {
                default: {
                    x: 0n, y: 0n
                },
                path: `image/full/${animation_name}/${image.path}`
            };
            animation.image.push(image);
            Kernel.ImageView.write_fs(`${destination}/${animation_name}.sprite/media/${animation_name}_${i + 1}.png`, Kernel.ImageView.instance(BigInt(source.size.width), BigInt(source.size.height), source.data));
        }
        animation.main_sprite.work_area.duration = BigInt(animation.main_sprite.frame.length);
        Kernel.JSON.serialize_fs(`${destination}/${animation_name}.pam.json`, animation, 1, true);
        Kernel.JSON.serialize_fs(`${destination}/${animation_name}.sprite/atlas.json`, atlases, 1, true);
        const atlas_size: Sen.Script.Support.PopCap.Atlas.Pack.Detail.SizeRange<number> = {
            width: setting.atlas_dimension.width <= 0n ? Number(k_default_atlas_size) : Number(setting.atlas_dimension.width),
            height: setting.atlas_dimension.height <= 0n ? Number(k_default_atlas_size) : Number(setting.atlas_dimension.height),
            padding: 1
        };
        const detail: Sen.Script.Support.PopCap.Atlas.Pack.Detail.Data = {
            smart: true,
            pot: false,
            allowRotation: false,
            square: false
        }; // TODO: add configuration
        Support.PopCap.Atlas.Pack.ResInfo.process_fs(`${destination}/${animation_name}.sprite`, atlas_size, detail, destination)
        return;
    }
}