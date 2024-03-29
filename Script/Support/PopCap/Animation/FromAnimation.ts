namespace Sen.Script.Support.PopCap.Animation {
    // definition
    export type SexyAnimation = Structure.SexyAnimation;

    export namespace FromAnimation {
        const xmlns_xsi: xmlns_xsi = "http://www.w3.org/2001/XMLSchema-instance";

        const xmlns: xmlns = "http://ns.adobe.com/xfl/2008/";

        const initial_transform: number[] = [1.0, 0.0, 0.0, 1.0, 0.0, 0.0];

        const initial_color: number[] = [1.0, 1.0, 1.0, 1.0];

        export interface FrameList {
            frame_node_list: FrameNodeList;
            action_list: ActionList;
        }

        export function process(animation: SexyAnimation, destination: string, resolution: BigInt): void {
            const image_scale_ratio: number = 1200 / Number(resolution);
            const record_info: RecordInfo = {
                version: animation.version,
                group: {},
            };
            for (let animation_image of animation["image"]) {
                record_info.group[animation_image.id] = {
                    name: animation_image.name,
                    size: animation_image.size,
                };
                const image_document: string = write_image(animation_image, image_scale_ratio);
                Sen.Kernel.FileSystem.write_file(Sen.Kernel.Path.join(destination, "library", "image", `${animation_image.id}.xml`), image_document);
            }
            for (let animation_sprite of animation["sprite"]) {
                const frame_list: FrameList = decode_frame_list(animation_sprite, animation);
                const sprite_document: string = write_sprite(animation_sprite.name, frame_list.frame_node_list, animation, false);
                Sen.Kernel.FileSystem.write_file(Sen.Kernel.Path.join(destination, "library", "sprite", `${animation_sprite.name}.xml`), sprite_document);
            }
            const frame_list: FrameList = decode_frame_list(animation.main_sprite, animation);
            const action_node_list: Record<string, Record<string, FrameNode[]>> = write_action(frame_list);
            const action_keys_templates: string[] = Object.keys(action_node_list);
            for (let action_label of action_keys_templates) {
                const action_document: string = write_sprite(action_label, action_node_list[action_label], animation, true);
                Sen.Kernel.FileSystem.write_file(Sen.Kernel.Path.join(destination, "library", "action", `${action_label}.xml`), action_document);
            }
            const dom_document: string = write_document(animation, frame_list["action_list"]);
            Sen.Kernel.FileSystem.write_file(Sen.Kernel.Path.join(destination, "DomDocument.xml"), dom_document);
            Sen.Kernel.FileSystem.write_file(Sen.Kernel.Path.join(destination, "main.xfl"), "PROXY-CS5");
            Sen.Kernel.JSON.serialize_fs(Sen.Kernel.Path.join(destination, "record.json"), record_info, 1, false);
            Sen.Kernel.FileSystem.create_directory(Sen.Kernel.Path.join(destination, "library", "media"));
            return;
        }

        export function write_document(animation: SexyAnimation, action_list: ActionList): string {
            const dombitmap_items: DomBitmapItem = {
                DOMBitmapItem: [],
            };
            const symbols_include_list: AttributesSymbolsItem[] = [];
            for (let animation_image of animation.image) {
                dombitmap_items["DOMBitmapItem"].push({
                    "@attributes": {
                        name: `media/${animation_image.name}`,
                        href: `media/${animation_image.name}.png`,
                    },
                });
                symbols_include_list.push({
                    "@attributes": {
                        href: `image/${animation_image.id}.xml`,
                    },
                });
            }
            for (let animation_sprite of animation.sprite) {
                symbols_include_list.push({
                    "@attributes": {
                        href: `sprite/${animation_sprite.name}.xml`,
                    },
                });
            }
            const action_keys: string[] = Object.keys(action_list);
            const flow_dom_frames: DocumentDomFrame[] = [];
            for (let action_name of action_keys) {
                symbols_include_list.push({
                    "@attributes": {
                        href: `action/${action_name}.xml`,
                    },
                });
                const start_index: bigint = action_list[action_name]["start_index"];
                const action_duration: bigint = action_list[action_name]["duration"];
                flow_dom_frames.push({
                    "@attributes": {
                        index: `${start_index}`,
                        duration: `${action_duration - start_index}`,
                        name: action_name,
                        labelType: "name",
                    },
                    elements: {
                        DOMSymbolInstance: {
                            "@attributes": {
                                libraryItemName: `action/${action_name}`,
                                symbolType: "graphic",
                                loop: "loop",
                            },
                        },
                    },
                });
            }
            const main_frame: Structure.AnimationFrame[] = animation["main_sprite"]["frame"];
            let prev_end_index: bigint = 0n;
            let write_command: boolean = false;
            let write_command_pos: bigint = 0n;
            const command_dom_frames: DocumentDomFrame[] = [];
            for (let i = 0; i < main_frame.length; ++i) {
                let command_script: string = "";
                if (main_frame[i]["stop"]) {
                    command_script += "stop();";
                    write_command = true;
                }
                for (let k = 0; k < main_frame[i]["command"].length; ++k) {
                    const command_list: Structure.AnimationCommand = main_frame[i]["command"][k];
                    const command: string = `fscommand("${command_list["parameter"]}", "${command_list["command"]}")`;
                    command_script += command;
                    write_command = true;
                }
                if (write_command) {
                    if (BigInt(i) - prev_end_index > 0) {
                        command_dom_frames.push({
                            "@attributes": {
                                index: `${prev_end_index}`,
                                duration: `${BigInt(i) - prev_end_index}`,
                            },
                            elements: {
                                "@text": {
                                    is_cdata: false,
                                    value: "",
                                },
                            },
                        });
                    }
                    prev_end_index = BigInt(i);
                    command_dom_frames.push({
                        "@attributes": {
                            index: `${prev_end_index}`,
                        },
                        Actionscript: {
                            script: {
                                "@text": {
                                    is_cdata: true,
                                    value: command_script,
                                },
                            },
                        },
                        elements: {
                            "@text": {
                                is_cdata: false,
                                value: "",
                            },
                        },
                    });
                    ++prev_end_index;
                    write_command_pos = prev_end_index;
                    write_command = false;
                }
                if (i === main_frame.length - 1) {
                    if (write_command_pos != BigInt(main_frame.length)) {
                        command_dom_frames.push({
                            "@attributes": {
                                index: `${prev_end_index}`,
                                duration: `${main_frame.length - Number(prev_end_index)}`,
                            },
                            elements: {
                                "@text": {
                                    is_cdata: false,
                                    value: "",
                                },
                            },
                        });
                    }
                }
            }
            const dom_layers: DocumentDomLayer[] = [
                {
                    "@attributes": {
                        name: "flow",
                    },
                    frames: {
                        DOMFrame: flow_dom_frames,
                    },
                },
                {
                    "@attributes": {
                        name: "command",
                    },
                    frames: {
                        DOMFrame: command_dom_frames,
                    },
                },
            ];
            const dom_document: DOMDocument = {
                DOMDocument: {
                    "@attributes": {
                        "xmlns:xsi": xmlns_xsi,
                        xmlns: xmlns,
                        frameRate: `${animation["frame_rate"]}`,
                        width: `${animation["size"]["width"]}`,
                        height: `${animation["size"]["height"]}`,
                        xflVersion: "2.971",
                    },
                    folders: {
                        DOMFolderItem: [
                            {
                                "@attributes": {
                                    name: "media",
                                    isExpanded: "true",
                                },
                            },
                            {
                                "@attributes": {
                                    name: "action",
                                    isExpanded: "true",
                                },
                            },
                            {
                                "@attributes": {
                                    name: "image",
                                    isExpanded: "true",
                                },
                            },
                            {
                                "@attributes": {
                                    name: "sprite",
                                    isExpanded: "true",
                                },
                            },
                        ],
                    },
                    media: dombitmap_items,
                    symbols: {
                        Include: symbols_include_list,
                    },
                    timelines: {
                        DOMTimeline: {
                            "@attributes": {
                                name: "animation",
                            },
                            layers: {
                                DOMLayer: dom_layers,
                            },
                        },
                    },
                },
            };
            return Sen.Kernel.XML.serialize(dom_document);
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
        export function write_sprite(name: string, frame_node_list: FrameNodeList, animation: SexyAnimation, is_action: boolean): string {
            const layer_keys: string[] = Object.keys(frame_node_list);
            let dom_sprite_layers: SpriteDomLayer[] = [];
            let layers_name_index = 0;
            for (let i = 1; i < layer_keys.length; ++i) {
                const layers_index: bigint = BigInt(layer_keys[i]);
                const frame_node: FrameNode[] = frame_node_list[`${layers_index}`];
                const dom_layer_frames: SpriteDomFrame[] = [];
                for (let k = 0; k < frame_node.length; k++) {
                    const transform: number[] = frame_node[k]["transform"];
                    const base_color: number[] = frame_node[k]["color"];
                    const resource: bigint | null = frame_node[k]["resource"];
                    const sprite_dom_frame: SpriteDomFrame = {
                        "@attributes": {
                            index: `${frame_node[k]["index"]}`,
                            duration: `${frame_node[k]["duration"]}`,
                        },
                        elements: {
                            "@text": {
                                is_cdata: false,
                                value: "",
                            },
                        },
                    };
                    if (check_base_frame(transform, base_color, resource)) {
                        const symbol_instance_attributes: DOMSymbolInstanceAttributes = frame_node[k]["sprite"]
                            ? {
                                  libraryItemName: `sprite/${animation.sprite[Number(resource)].name}`,
                                  firstFrame: `${frame_node[k]["first_frame"]}`,
                                  symbolType: "graphic",
                                  loop: "loop",
                              }
                            : {
                                  libraryItemName: `image/${animation.image[Number(resource)].id}`,
                                  symbolType: "graphic",
                                  loop: "loop",
                              };
                        if (frame_node[k]["sprite"]) {
                            symbol_instance_attributes["firstFrame"] = `${frame_node[k]["first_frame"]}`;
                        }
                        const dom_symbol_instance: DOMSymbolInstance = {
                            "@attributes": symbol_instance_attributes,
                            matrix: {
                                Matrix: {
                                    "@attributes": {
                                        a: transform[0].toFixed(6),
                                        b: transform[1].toFixed(6),
                                        c: transform[2].toFixed(6),
                                        d: transform[3].toFixed(6),
                                        tx: transform[4].toFixed(6),
                                        ty: transform[5].toFixed(6),
                                    },
                                },
                            },
                            color: {
                                Color: {
                                    "@attributes": {
                                        redMultiplier: base_color[0].toFixed(6),
                                        greenMultiplier: base_color[1].toFixed(6),
                                        blueMultiplier: base_color[2].toFixed(6),
                                        alphaMultiplier: base_color[3].toFixed(6),
                                    },
                                },
                            },
                        };
                        sprite_dom_frame["elements"] = {
                            DOMSymbolInstance: dom_symbol_instance,
                        };
                    }
                    dom_layer_frames.push(sprite_dom_frame);
                }
                if (dom_layer_frames.length === 0) continue;
                dom_sprite_layers.unshift({
                    "@attributes": {
                        name: `${layers_name_index}`,
                    },
                    frames: {
                        DOMFrame: dom_layer_frames,
                    },
                });
                ++layers_name_index;
            }
            const sprite_attributes: Attributes = {
                "xmlns:xsi": xmlns_xsi,
                xmlns: xmlns,
                name: is_action ? `action/${name}` : `sprite/${name}`,
                symbolType: "graphic",
            };
            const sprite_document: SpriteDocument = {
                DOMSymbolItem: {
                    "@attributes": sprite_attributes,
                    timeline: {
                        DOMTimeline: {
                            "@attributes": {
                                name: name,
                            },
                            layers: {
                                DOMLayer: dom_sprite_layers,
                            },
                        },
                    },
                },
            };
            return Sen.Kernel.XML.serialize(sprite_document);
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

        export function decode_frame_list(animation_sprite: Structure.AnimationSprite, animation: SexyAnimation): FrameList {
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
                const frame_additives: Structure.AnimationAppend[] = animation_sprite["frame"][i]["append"];
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
                const frame_moves: Structure.AnimationMove[] = animation_sprite["frame"][i]["change"];
                for (let k = 0; k < frame_moves.length; ++k) {
                    const frame_change: Structure.AnimationMove = frame_moves[k];
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

        export function write_image(animation_image: Structure.AnimationImage, image_scale_ratio: number): string {
            const image_transform: number[] = animation_image["transform"];
            const image_attributes: Attributes = {
                "xmlns:xsi": xmlns_xsi,
                xmlns: xmlns,
                name: `image/${animation_image.id}`,
                symbolType: "graphic",
            };
            const image_domtimeline: DomTimeline = {
                "@attributes": {
                    name: animation_image.id,
                },
                layers: {
                    DOMLayer: {
                        frames: {
                            DOMFrame: {
                                "@attributes": {
                                    index: "0",
                                },
                                elements: {
                                    DOMBitmapInstance: {
                                        "@attributes": {
                                            libraryItemName: `media/${animation_image.name}`,
                                        },
                                        matrix: {
                                            Matrix: {
                                                "@attributes": {
                                                    a: (image_transform[0] * image_scale_ratio).toFixed(6),
                                                    b: image_transform[1].toFixed(6),
                                                    c: image_transform[2].toFixed(6),
                                                    d: (image_transform[3] * image_scale_ratio).toFixed(6),
                                                    tx: image_transform[4].toFixed(6),
                                                    ty: image_transform[5].toFixed(6),
                                                },
                                            },
                                        },
                                    },
                                },
                            },
                        },
                    },
                },
            };
            const image_document: ImageDocument = {
                DOMSymbolItem: {
                    "@attributes": image_attributes,
                    timeline: {
                        DOMTimeline: image_domtimeline,
                    },
                },
            };
            return Sen.Kernel.XML.serialize(image_document);
        }

        export function process_fs(source: string, destination: string, resolution: bigint): void {
            const animation: SexyAnimation = Kernel.JSON.deserialize_fs<SexyAnimation>(source);
            FromAnimation.process(animation, destination, resolution);
            return;
        }
    }
}
