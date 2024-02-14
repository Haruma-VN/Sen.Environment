namespace Sen.Script.Support.PopCap.Animation {
    export namespace ToAnimation {
        const initial_transform: number[] = [1.0, 0.0, 0.0, 1.0, 0.0, 0.0];

        const initial_color: number[] = [1.0, 1.0, 1.0, 1.0];

        export interface AnimFramNode {
            frames: Structure.AnimationFrame[];
            layer_length: bigint;
        }

        export interface DocumentSymbol {
            action: string[];
            sprite: string[];
            image: string[];
        }

        export function process(source: string): SexyAnimation {
            // debug("process");
            const dom_document: DOMDocument = Kernel.XML.deserialize_fs(Kernel.Path.join(source, "DomDocument.xml"));
            const dom_symbol_list: DocumentSymbol = read_symbols_include(dom_document);
            const record_info: ExtraInfo = Kernel.JSON.deserialize_fs(Kernel.Path.join(source, "record.json"));
            if (record_info.version > 6n || record_info.version < 1n) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_version"), record_info.version));
            }
            const animation_image_id_list: string[] = dom_symbol_list["image"];
            const animation_image_map: Record<string, Structure.AnimationImage> = {};

            for (let image_id of animation_image_id_list) {
                const image_document: ImageDocument = Kernel.XML.deserialize(Kernel.FileSystem.read_file(Kernel.Path.join(source, "library", "image", `${image_id}.xml`)));
                animation_image_map[image_id] = parse_image(image_document, image_id, record_info);
            }
            const animation_sprite_name_list: string[] = dom_symbol_list["sprite"];
            // debug(animation_sprite_name_list);
            const animation_sprite_map: Record<string, Structure.AnimationSprite> = {};
            for (let sprite_name of animation_sprite_name_list) {
                // debug(sprite_name);
                const sprite_document: SpriteDocument = Kernel.XML.deserialize(Kernel.FileSystem.read_file(Kernel.Path.join(source, "library", "sprite", `${sprite_name}.xml`)));
                // debug(sprite_document);
                const frame_node: AnimFramNode = parse_sprite(sprite_document, sprite_name, false, animation_image_id_list, animation_sprite_name_list);
                animation_sprite_map[sprite_name] = {
                    description: "",
                    work_area: {
                        index: 0n,
                        duration: BigInt(frame_node["frames"].length),
                    },
                    frame: frame_node["frames"],
                };
            }
            const animation_label_list: string[] = dom_symbol_list["action"];
            const animation_action_map: Record<string, AnimFramNode> = {};
            for (let action_label of animation_label_list) {
                const action_document: SpriteDocument = Kernel.XML.deserialize(Kernel.FileSystem.read_file(Kernel.Path.join(source, "library", "action", `${action_label}.xml`)));
                animation_action_map[action_label] = parse_sprite(action_document, action_label, true, animation_image_id_list, animation_sprite_name_list);
            }
            const action_index_list: Record<string, ActionFrameNode> = get_action_index(dom_document);
            const main_frame: Structure.AnimationFrame[] = merge_action(animation_action_map, action_index_list);
            const animation: Structure.SexyAnimation = {
                version: record_info.version,
                frame_rate: 0n,
                position: {
                    x: 0n,
                    y: 0n,
                },
                size: {
                    width: 0n,
                    height: 0n,
                },
                image: animation_image_map,
                sprite: animation_sprite_map,
                main_sprite: {
                    description: "",
                    work_area: {
                        index: 0n,
                        duration: BigInt(main_frame.length),
                    },
                    frame: main_frame,
                },
            };
            parse_dom_document(dom_document, animation, action_index_list, BigInt(main_frame.length - 1));
            return animation;
        }

        export function read_symbols_include(dom_document: DOMDocument): DocumentSymbol {
            const dom_symbol_list: DocumentSymbol = {
                action: [],
                sprite: [],
                image: [],
            };
            if (!dom_document["DOMDocument"].hasOwnProperty("symbols")) {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.document_has_no_symbols"));
            }
            const dom_symbols: AttributesSymbolsItem[] = dom_document["DOMDocument"]["symbols"]["Include"];
            for (let symbol of dom_symbols) {
                const item: string = symbol["@attributes"]["href"];
                if (item.startsWith("action")) {
                    dom_symbol_list["action"].push(item.substring(7, item.length - 4));
                }
                if (item.startsWith("image")) {
                    dom_symbol_list["image"].push(item.substring(6, item.length - 4));
                }
                if (item.startsWith("sprite")) {
                    dom_symbol_list["sprite"].push(item.substring(7, item.length - 4));
                }
            }
            return dom_symbol_list;
        }

        export function sort_list(list: string[]): string[] {
            // debug("sort_list");
            return list.sort((a: string, b: string) => a.localeCompare(b, undefined, { numeric: true }));
        }

        export function parse_dom_document(dom_document: DOMDocument, animation: Structure.SexyAnimation, action_index_list: Record<string, ActionFrameNode>, last_frame: bigint): void {
            // debug("parse_dom_document");
            if (!dom_document["DOMDocument"].hasOwnProperty("folders")) {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.document_has_no_folder"));
            }
            if (!dom_document["DOMDocument"].hasOwnProperty("media")) {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.document_has_no_media"));
            }
            animation["frame_rate"] = BigInt(dom_document["DOMDocument"]["@attributes"]["frameRate"] ?? "24");
            animation["size"]["width"] = BigInt(dom_document["DOMDocument"]["@attributes"]["width"] ?? "390");
            animation["size"]["height"] = BigInt(dom_document["DOMDocument"]["@attributes"]["height"] ?? "390");
            const dom_timeline: DocumentDOMTimeline = dom_document["DOMDocument"]["timelines"]["DOMTimeline"];
            if (dom_timeline["@attributes"]["name"] !== "animation") {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.document_name_must_be_animation"));
            }
            const action_label_list: string[] = Object.keys(action_index_list);
            for (let label of action_label_list) {
                const frame_index: bigint = action_index_list[label]["index"];
                animation["main_sprite"]["frame"][Number(frame_index)]["label"] = label;
            }
            const dom_command: DocumentDomLayer = dom_document["DOMDocument"]["timelines"]["DOMTimeline"]["layers"]["DOMLayer"][1];
            if (dom_command["@attributes"]["name"] !== "command") {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.document_name_must_be_command"));
            }
            const dom_command_frames: DocumentDomFrameProperty = dom_command["frames"]["DOMFrame"];
            const dom_command_frames_list: DocumentDomFrame[] = [];
            if (Array.isArray(dom_command_frames)) {
                dom_command_frames_list.push(...dom_command_frames);
            } else {
                dom_command_frames_list.push(dom_command_frames);
            }
            const all_frames: number = Number(dom_command_frames_list[dom_command_frames_list.length - 1]["@attributes"]["index"]);
            for (let i = 0; i < all_frames - Number(last_frame); i++) {
                animation["main_sprite"]["frame"].push({
                    label: "",
                    stop: false,
                    command: [],
                    remove: [],
                    append: [],
                    change: [],
                });
            }
            for (let frame of dom_command_frames_list) {
                const frame_index: bigint = BigInt(frame["@attributes"]["index"]);
                if (typeof frame["Actionscript"] === "undefined") {
                    continue;
                }
                const action_script: ActionscriptProperty = frame["Actionscript"];
                if (!action_script.hasOwnProperty("script")) {
                    throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_dom_script"), frame["@attributes"].index));
                }
                if (!action_script["script"]["@text"]["is_cdata"]) {
                    throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_script_cdata"), frame["@attributes"].index));
                }
                const script_data: string = action_script["script"]["@text"]["value"].trim();
                const script_list: string[] = script_data.split(";");
                if (script_list.includes("stop()")) {
                    animation["main_sprite"]["frame"][Number(frame_index)]["stop"] = true;
                    continue;
                }
                for (let command of script_list) {
                    const regex_result: RegExpMatchArray | null = command.trim().match(/fscommand\("(.*)", "(.*)"\)/);
                    if (regex_result === null) {
                        throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_command"), command));
                    }
                    animation["main_sprite"]["frame"][Number(frame_index)]["command"].push({
                        command: regex_result[1],
                        parameter: regex_result[2],
                    });
                }
            }
            return;
        }

        export function merge_action(animation_action_map: Record<string, AnimFramNode>, action_index_list: Record<string, ActionFrameNode>): Structure.AnimationFrame[] {
            // debug("merge_action");
            const main_frame: Structure.AnimationFrame[] = [];
            const animation_label_list: string[] = Object.keys(action_index_list);
            let label_remove: bigint = 0n;
            let start_index: bigint = 0n;
            const remove_list: bigint[] = [];
            for (let i = 0; i < animation_label_list.length; i++) {
                const action_name: string = animation_label_list[i];
                const duration: bigint = action_index_list[action_name]["duration"];
                const layer_length: bigint = animation_action_map[action_name]["layer_length"];
                const action_frames: Structure.AnimationFrame[] = animation_action_map[action_name]["frames"];
                const action_length: number = action_frames.length;
                const use_transform: boolean = action_index_list[action_name]["use_transform"];
                const use_color: boolean = action_index_list[action_name]["use_color"];
                if (BigInt(action_length) != duration) {
                    if (duration > BigInt(action_length)) {
                        const append_frames: Structure.AnimationFrame[] = [];
                        for (let k = 0; k < Number(duration) - action_length; k++) {
                            const add_frame: Structure.AnimationFrame = Kernel.Miscellaneous.make_copy<Structure.AnimationFrame>(action_frames[k]);
                            add_frame["append"] = [];
                            append_frames.push(add_frame);
                        }
                        action_frames.push(...append_frames);
                    } else {
                        action_frames.splice(Number(duration), action_length - Number(duration));
                    }
                }
                for (let k = 0; k < action_frames.length; k++) {
                    const frame_remove: bigint[] = action_frames[k]["remove"];
                    for (let h = 0; h < frame_remove.length; h++) {
                        frame_remove[h] += start_index;
                        remove_list.push(frame_remove[h]);
                    }
                    const frame_append: Structure.AnimationAppend[] = action_frames[k]["append"];
                    for (let h = 0; h < frame_append.length; h++) {
                        frame_append[h]["index"] += start_index;
                    }
                    const frame_change: Structure.AnimationMove[] = action_frames[k]["change"];
                    for (let h = 0; h < frame_change.length; h++) {
                        frame_change[h]["index"] += start_index;
                        if (use_transform) {
                            frame_change[h]["transform"] = transform_calculator(mix_transform(frame_change[h]["transform"], action_index_list[action_name]["transform"]!));
                        } else {
                            frame_change[h]["transform"] = transform_calculator(frame_change[h]["transform"]);
                        }
                        if (use_color) {
                            if (frame_change[h]["color"] !== null) {
                                frame_change[h]["color"] = mix_color(frame_change[h]["color"]!, action_index_list[action_name]["color"]!);
                            } else {
                                frame_change[h]["color"] = Kernel.Miscellaneous.make_copy<number[]>(action_index_list[action_name]["color"]!);
                            }
                        }
                    }
                }
                if (i > 0) {
                    const frame_remove = action_frames[0]["remove"];
                    for (let k = label_remove; k < start_index; k++) {
                        if (!frame_remove.includes(k) && !remove_list.includes(k)) {
                            frame_remove.push(k);
                        }
                    }
                    label_remove = start_index;
                }
                start_index += layer_length;
                main_frame.push(...action_frames);
            }
            return main_frame;
        }

        export function mix_transform(source: number[], change: number[]): number[] {
            return [
                change[0] * source[0] + change[2] * source[1],
                change[1] * source[0] + change[3] * source[1],
                change[0] * source[2] + change[2] * source[3],
                change[1] * source[2] + change[3] * source[3],
                change[0] * source[4] + change[2] * source[5] + change[4],
                change[1] * source[4] + change[3] * source[5] + change[5],
            ];
        }

        export function mix_color(source: number[], change: number[]): number[] {
            return [change[0] * source[0], change[1] * source[1], change[2] * source[2], change[3] * source[3]];
        }

        export function get_action_index(dom_document: DOMDocument): Record<string, ActionFrameNode> {
            // debug("get_action_index");
            if (!dom_document.hasOwnProperty("DOMDocument")) {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.document_has_no_DOMDocument"));
            }
            if (!dom_document["DOMDocument"].hasOwnProperty("timelines")) {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.document_has_no_timelines"));
            }
            if (!dom_document["DOMDocument"]["timelines"].hasOwnProperty("DOMTimeline")) {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.document_has_no_DOMTimeline"));
            }
            if (!dom_document["DOMDocument"]["timelines"]["DOMTimeline"].hasOwnProperty("layers")) {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.document_has_no_layers"));
            }
            if (!dom_document["DOMDocument"]["timelines"]["DOMTimeline"]["layers"].hasOwnProperty("DOMLayer")) {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.document_has_no_DOMLayer"));
            }
            const dom_layer: DocumentDomLayer[] = dom_document["DOMDocument"]["timelines"]["DOMTimeline"]["layers"]["DOMLayer"];
            if (dom_layer.length !== 2) {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.invalid_document_DOMLayer_length"));
            }
            const dom_flow: DocumentDomLayer = dom_layer[0];
            if (dom_flow["@attributes"]["name"] !== "flow") {
                throw new Error(Kernel.Language.get("popcap.animation.from_flash.invalid_document_flow"));
            }
            const flow_dom_frames: DocumentDomFrameProperty = dom_flow["frames"]["DOMFrame"];
            const flow_domframes_list: DocumentDomFrame[] = [];
            if (Array.isArray(flow_dom_frames)) {
                flow_domframes_list.push(...flow_dom_frames);
            } else {
                flow_domframes_list.push(flow_dom_frames);
            }
            const action_index_list: Record<string, ActionFrameNode> = {};
            for (let frame of flow_domframes_list) {
                const frame_index: bigint = BigInt(frame["@attributes"]["index"]);
                const duration: bigint = BigInt(frame["@attributes"]["duration"] ?? "1");
                if (frame["@attributes"]["name"] !== undefined) {
                    if (frame["@attributes"]["labelType"] !== "name") {
                        throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_action_label"), frame["@attributes"].name));
                    }
                    if (!frame.hasOwnProperty("elements") && frame["elements"] !== null) {
                        throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.dom_frame_has_no_elements"), frame["@attributes"].name));
                    }
                    const elements: DocumentElements = frame["elements"] as DocumentElements;
                    if (!elements.hasOwnProperty("DOMSymbolInstance")) {
                        throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.dom_frame_has_no_DOMSymbolInstance"), frame["@attributes"].name));
                    }
                    const dom_symbol_instance: DocumentDomSymbolInstance = elements["DOMSymbolInstance"];
                    if (Array.isArray(dom_symbol_instance)) {
                        throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.dom_symbol_cannot_be_array"), frame["@attributes"].name));
                    }
                    const action_label: string = frame["@attributes"]["name"];
                    if (dom_symbol_instance["@attributes"]["libraryItemName"] !== `action/${action_label}`) {
                        throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invaild_action_index"), `action/${action_label}`, frame["@attributes"].name));
                    }
                    action_index_list[action_label] = {
                        index: frame_index,
                        duration: duration,
                        use_transform: false,
                        transform: null,
                        use_color: false,
                        color: null,
                    };
                    if (dom_symbol_instance["matrix"] !== null && dom_symbol_instance["matrix"] !== undefined) {
                        const transform: number[] = parse_transform(dom_symbol_instance["matrix"]!["Matrix"]);
                        const use_transform: boolean = !(
                            transform[0] === initial_transform[0] &&
                            transform[1] === initial_transform[1] &&
                            transform[2] === initial_transform[2] &&
                            transform[3] === initial_transform[3] &&
                            transform[4] === initial_transform[4] &&
                            transform[5] === initial_transform[5]
                        );
                        if (use_transform) {
                            action_index_list[action_label]["use_transform"] = use_transform;
                            action_index_list[action_label]["transform"] = transform;
                        }
                    }
                    if (dom_symbol_instance["color"] !== null && dom_symbol_instance["color"] !== undefined) {
                        const color: number[] = parse_color(dom_symbol_instance["color"]!["Color"]);
                        const use_color: boolean = !(color[0] === initial_color[0] && color[1] === initial_color[1] && color[2] === initial_color[2] && color[3] === initial_color[3]);
                        if (use_color) {
                            action_index_list[action_label]["use_color"] = use_color;
                            action_index_list[action_label]["color"] = color;
                        }
                    }
                }
            }
            return action_index_list;
        }

        export function parse_sprite(sprite_document: SpriteDocument, sprite_name: string, is_action: boolean, animation_image_id_list: string[], animation_sprite_name_list: string[]): AnimFramNode {
            // debug("parse_sprite");
            if (!sprite_document.hasOwnProperty("DOMSymbolItem")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.sprite_has_no_DOMSymbolItem"), sprite_name));
            }
            const dom_sprite_name: string = `${is_action ? "action" : "sprite"}/${sprite_name}`;
            if (sprite_document["DOMSymbolItem"]["@attributes"]["name"] !== dom_sprite_name) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_sprite_name"), sprite_name, sprite_document["DOMSymbolItem"]["@attributes"]["name"], dom_sprite_name));
            }
            if (!sprite_document["DOMSymbolItem"].hasOwnProperty("timeline")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.sprite_has_no_timeline"), sprite_name));
            }
            if (!sprite_document["DOMSymbolItem"]["timeline"].hasOwnProperty("DOMTimeline")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.sprite_has_no_DOMtimeline"), sprite_name));
            }
            const DOMtimeline: SpriteDomTimeline = sprite_document["DOMSymbolItem"]["timeline"]["DOMTimeline"];
            if (DOMtimeline["@attributes"]["name"] !== sprite_name) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_sprite_name"), sprite_name, DOMtimeline["@attributes"]["name"], sprite_name));
            }
            if (!DOMtimeline.hasOwnProperty("layers")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.sprite_has_no_layers"), sprite_name));
            }
            const sprite_layers: SpriteLayers = DOMtimeline["layers"];
            if (!sprite_layers.hasOwnProperty("DOMLayer")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.sprite_has_no_DOMLayer"), sprite_name));
            }
            const dom_layers: SpriteDomLayerProperty = sprite_layers["DOMLayer"];
            const dom_layers_list: SpriteDomLayer[] = [];
            if (Array.isArray(dom_layers)) {
                dom_layers_list.push(...dom_layers);
            } else {
                dom_layers_list.push(dom_layers);
            }
            dom_layers_list.reverse();
            let layer_count: bigint = 0n;
            let model: Model | null = null;
            const frames_result: Structure.AnimationFrame[] = [];

            function get_frame_at_index(index: bigint): Structure.AnimationFrame {
                // debug("get_frame_at_index");
                if (frames_result.length <= Number(index)) {
                    frames_result.concat(frames_result[Number(index) - frames_result.length + 1]);
                }
                if (typeof frames_result[Number(index)] === "undefined") {
                    frames_result[Number(index)] = {
                        label: "",
                        stop: false,
                        command: [],
                        remove: [],
                        append: [],
                        change: [],
                    };
                }
                return frames_result[Number(index)];
            }

            function close_current_model() {
                // debug("close_current_model");
                if (model !== null) {
                    const target_frame: Structure.AnimationFrame = get_frame_at_index(model["frame_start"] + model["frame_duration"]);
                    target_frame["remove"].push(model["index"]!);
                    model = null;
                }
                return;
            }

            function get_resource(is_sprite: boolean, sprite_item_name: string): bigint {
                const index: bigint = is_sprite ? BigInt(animation_sprite_name_list.indexOf(sprite_item_name)) : BigInt(animation_image_id_list.indexOf(sprite_item_name));
                if (index === -1n) {
                    throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.cannot_find_sprite"), sprite_item_name));
                }
                return index;
            }

            let use_empty_last_layer = false;

            dom_layers_list.forEach((layer: SpriteDomLayer, index: number) => {
                if (!layer.hasOwnProperty("frames")) {
                    throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.sprite_has_no_frames"), sprite_name, index + 1));
                }
                const sprite_frames: SpriteFrame = layer["frames"];
                if (!sprite_frames.hasOwnProperty("DOMFrame")) {
                    throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.sprite_has_no_DOMFrame"), sprite_name, index + 1));
                }
                const dom_frames: SpriteDomFrameProperty = sprite_frames["DOMFrame"];
                const dom_frames_list: SpriteDomFrame[] = [];
                if (Array.isArray(dom_frames)) {
                    dom_frames_list.push(...dom_frames);
                } else {
                    dom_frames_list.push(dom_frames);
                }
                dom_frames_list.forEach((dom_frame: SpriteDomFrame) => {
                    const frame_index: bigint = BigInt(dom_frame["@attributes"]["index"]);
                    const frame_duration: bigint = BigInt(dom_frame["@attributes"]["duration"] ?? "1");
                    if (!dom_frame.hasOwnProperty("elements")) {
                        throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.sprite_has_no_elements"), sprite_name, index + 1));
                    }
                    const elements: any = dom_frame["elements"];
                    if (elements === null) {
                        if (index === dom_layers_list.length - 1) {
                            const frame_duration_temp: number = Number(frame_duration) - frames_result.length + 1;
                            if (frame_duration_temp > 0 && dom_frames_list.length === 1) {
                                use_empty_last_layer = true;
                                for (let k = 0; k < frame_duration_temp; k++) {
                                    frames_result.push({
                                        label: "",
                                        stop: false,
                                        command: [],
                                        remove: [],
                                        append: [],
                                        change: [],
                                    });
                                }
                                return;
                            }
                        }
                        close_current_model();
                        return;
                    }
                    const dom_symbol_instance: DOMSymbolInstance = elements["DOMSymbolInstance"];
                    const library_item_name: string = dom_symbol_instance["@attributes"]["libraryItemName"];
                    const name_match: RegExpMatchArray | null = library_item_name.match(/(image|sprite)/);
                    if (name_match === null) {
                        throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_name_match"), sprite_name, index + 1));
                    }
                    if (name_match[0] !== name_match[1]) {
                        throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.mismatch_name_match"), sprite_name, index + 1, name_match[0], name_match[1]));
                    }
                    const sprite_item_name: string = library_item_name.substring(name_match[0].length + 1);
                    const is_sprite: boolean = name_match[0] === "sprite";
                    const current_instance: Instance = {
                        resource: get_resource(is_sprite, sprite_item_name),
                        sprite: is_sprite,
                    };
                    let transform: number[];
                    if (dom_symbol_instance["matrix"] === undefined || dom_symbol_instance["matrix"] === null || Object.keys(dom_symbol_instance["matrix"]).length === 0) {
                        transform = is_action ? [...initial_transform] : [0.0, 0.0];
                    } else if (dom_symbol_instance["matrix"].hasOwnProperty("Matrix")) {
                        transform = is_action ? parse_transform(dom_symbol_instance["matrix"]["Matrix"]) : transform_calculator(parse_transform(dom_symbol_instance["matrix"]["Matrix"]));
                    } else {
                        throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_sprite_matrix"), sprite_name, index + 1));
                    }
                    let color: number[];
                    if (dom_symbol_instance["color"] === undefined || dom_symbol_instance["color"] === null || Object.keys(dom_symbol_instance["color"]).length === 0) {
                        color = [1.0, 1.0, 1.0, 1.0];
                    } else if (dom_symbol_instance["color"].hasOwnProperty("Color")) {
                        color = parse_color(dom_symbol_instance["color"]["Color"]);
                    } else {
                        throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_sprite_color"), sprite_name, index + 1));
                    }
                    const target_frame: Structure.AnimationFrame = get_frame_at_index(frame_index);

                    if (model === null) {
                        model = {
                            state: false,
                            index: layer_count,
                            resource: current_instance["resource"],
                            sprite: current_instance["sprite"],
                            frame_start: frame_index,
                            frame_duration: frame_duration,
                            color: [...initial_color],
                            transform: [...initial_transform],
                        };
                        target_frame["append"].push({
                            index: model["index"]!,
                            name: "",
                            resource: current_instance["resource"],
                            sprite: current_instance["sprite"],
                            additive: false,
                            preload_frame: 0n,
                            time_scale: 1n,
                        });
                        layer_count++;
                    }
                    model["frame_start"] = frame_index;
                    model["frame_duration"] = frame_duration;
                    const color_is_changed: boolean = !(model["color"][0] === color[0] && model["color"][1] === color[1] && model["color"][2] === color[2] && model["color"][3] === color[3]);
                    if (color_is_changed) {
                        model["color"] = color;
                    }
                    target_frame["change"].push({
                        index: model["index"]!,
                        transform: transform,
                        color: color_is_changed ? color : null,
                        source_rectangle: null,
                        sprite_frame_number: 0n,
                    });
                });
                close_current_model();
            });
            for (let i = 0; i < frames_result.length; i++) {
                if (frames_result[i] === undefined) {
                    frames_result[i] = {
                        label: "",
                        stop: false,
                        command: [],
                        remove: [],
                        append: [],
                        change: [],
                    };
                }
            }
            frames_result.pop();
            return {
                frames: frames_result,
                layer_length: use_empty_last_layer ? BigInt(dom_layers_list.length) - 1n : BigInt(dom_layers_list.length),
            };
        }

        export function parse_color(color: Color): number[] {
            if (color["@attributes"]["tintMultiplier"] !== null && color["@attributes"]["tintMultiplier"] !== undefined) {
                return tiny_to_color(color);
                // return [1, 1, 1, 1];
            } else {
                return [
                    parse_color_compute(Number.parseFloat(color["@attributes"]["redMultiplier"] ?? "1"), Number.parseFloat(color["@attributes"]["redOffset"] ?? "0")),
                    parse_color_compute(Number.parseFloat(color["@attributes"]["greenMultiplier"] ?? "1"), Number.parseFloat(color["@attributes"]["greenOffset"] ?? "0")),
                    parse_color_compute(Number.parseFloat(color["@attributes"]["blueMultiplier"] ?? "1"), Number.parseFloat(color["@attributes"]["blueOffset"] ?? "0")),
                    parse_color_compute(Number.parseFloat(color["@attributes"]["alphaMultiplier"] ?? "1"), Number.parseFloat(color["@attributes"]["alphaOffset"] ?? "0")),
                ];
            }
        }

        export function tiny_to_color(color: Color): number[] {
            const tiny_color: number[] = hex_to_rgb(color["@attributes"]["tintColor"]!);
            const tint_multiplier: number = Number.parseFloat(color["@attributes"]["tintMultiplier"]! ?? "1") * 255;
            tiny_color.map((e) => parse_color_compute(e, tint_multiplier));
            tiny_color.push(1);
            return tiny_color;
        }

        export function hex_to_rgb(hex_color: string): number[] {
            const normal = hex_color.match(/^#([0-9a-f]{2})([0-9a-f]{2})([0-9a-f]{2})$/i);
            if (normal) return normal.slice(1).map((e) => Number.parseInt(e, 16) / 255);
            const shorthand = hex_color.match(/^#([0-9a-f])([0-9a-f])([0-9a-f])$/i);
            if (shorthand) return shorthand.slice(1).map((e) => (0x11 * Number.parseInt(e, 16)) / 255);
            return [1, 1, 1];
        }

        export function parse_color_compute(mutil: number, offset: number): number {
            return Math.max(0, Math.min(255, mutil * 255 + offset)) / 255;
        }

        export function transform_calculator(transform: number[]): number[] {
            // debug("transform_calculator");
            if (transform[0] === transform[3] && transform[1] === -transform[2]) {
                if (transform[0] === 1 && transform[1] === 0) {
                    return [transform[4], transform[5]];
                }
                const acos = Math.acos(transform[0]);
                const asin = Math.asin(transform[1]);
                if (Math.abs(acos) - Math.abs(asin) <= 1e-2) {
                    return [Number(asin.toFixed(6)), transform[4], transform[5]];
                }
            }
            return transform;
        }

        export function parse_number(num_str: string): number {
            // debug("parse_number");
            const num = Number(num_str);
            if (Number.isNaN(num)) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.number_is_NAN"), num_str));
            }
            return num;
        }

        export function parse_image(image_document: ImageDocument, image_id: string, record_info: ExtraInfo): Structure.AnimationImage {
            // debug("parse_image");
            if (!image_document.hasOwnProperty("DOMSymbolItem")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.image_has_no_DOMSymbolItem"), image_id));
            }
            if (image_document["DOMSymbolItem"]["@attributes"]["name"] !== `image/${image_id}`) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_image_name"), image_id));
            }
            if (!image_document["DOMSymbolItem"].hasOwnProperty("timeline")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.image_has_no_timeline"), image_id));
            }
            if (!image_document["DOMSymbolItem"]["timeline"].hasOwnProperty("DOMTimeline")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.image_has_no_timeline"), image_id));
            }
            const DOMtimeline: DomTimeline = image_document["DOMSymbolItem"]["timeline"]["DOMTimeline"];
            if (DOMtimeline["@attributes"]["name"] !== image_id) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_image_name"), image_id));
            }
            if (!DOMtimeline.hasOwnProperty("layers")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.image_has_no_layers"), image_id));
            }
            if (!DOMtimeline["layers"].hasOwnProperty("DOMLayer")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.image_has_no_DOMlayer"), image_id));
            }
            if (!DOMtimeline["layers"]["DOMLayer"].hasOwnProperty("frames")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.image_has_no_frames"), image_id));
            }
            if (!DOMtimeline["layers"]["DOMLayer"]["frames"].hasOwnProperty("DOMFrame")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.image_has_no_frames"), image_id));
            }
            const DOMframe: DomFrame = DOMtimeline["layers"]["DOMLayer"]["frames"]["DOMFrame"];
            if (!DOMframe.hasOwnProperty("elements")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.image_has_no_elements"), image_id));
            }
            if (!DOMframe["elements"].hasOwnProperty("DOMBitmapInstance")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.image_has_no_DOMBitmapInstance"), image_id));
            }
            const dom_bitmap_instance: DOMBitmapInstance = DOMframe["elements"]["DOMBitmapInstance"];
            const image_name: string = dom_bitmap_instance["@attributes"]["libraryItemName"].substring(6);
            const animation_image_name: string = record_info.group[image_id]["name"];
            if (!image_name.startsWith("TMP")) {
                if (animation_image_name !== image_name) {
                    throw new Error("invaild_image_name");
                }
            }
            if (!dom_bitmap_instance.hasOwnProperty("matrix")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.image_has_no_matrix"), image_id));
            }
            if (!dom_bitmap_instance["matrix"].hasOwnProperty("Matrix")) {
                throw new Error(format(Kernel.Language.get("popcap.animation.from_flash.invalid_image_matrix"), image_id));
            }
            const image_transform: number[] = parse_transform_for_image(dom_bitmap_instance["matrix"]["Matrix"]);
            const animation_image: Structure.AnimationImage = {
                name: animation_image_name,
                size: record_info.group[image_id]["size"],
                transform: image_transform,
            };
            return animation_image;
        }

        export function parse_transform_for_image(matrix: Matrix): number[] {
            // debug("parse_transform_for_image");
            const resolution_ratio_list: number[] = [0.78125, 1.5625, 1.875, 3.125, 1];
            let a_matrix: number = Number.parseFloat(matrix["@attributes"]["a"] ?? "1");
            if (resolution_ratio_list.includes(a_matrix)) {
                a_matrix = 1;
            }
            let d_matrix: number = Number.parseFloat(matrix["@attributes"]["d"] ?? "1");
            if (resolution_ratio_list.includes(d_matrix)) {
                d_matrix = 1;
            }
            return [
                a_matrix,
                Number.parseFloat(matrix["@attributes"]["b"] ?? "0"),
                Number.parseFloat(matrix["@attributes"]["c"] ?? "0"),
                d_matrix,
                Number.parseFloat(matrix["@attributes"]["tx"] ?? "0"),
                Number.parseFloat(matrix["@attributes"]["ty"] ?? "0"),
            ];
        }

        export function parse_transform(matrix: Matrix): number[] {
            // debug("parse_transform");
            return [
                Number.parseFloat(matrix["@attributes"]["a"] ?? "1"),
                Number.parseFloat(matrix["@attributes"]["b"] ?? "0"),
                Number.parseFloat(matrix["@attributes"]["c"] ?? "0"),
                Number.parseFloat(matrix["@attributes"]["d"] ?? "1"),
                Number.parseFloat(matrix["@attributes"]["tx"] ?? "0"),
                Number.parseFloat(matrix["@attributes"]["ty"] ?? "0"),
            ];
        }

        export function process_fs(source: string, desitnation: string): void {
            // debug("process_fs");
            Kernel.JSON.serialize_fs<SexyAnimation>(desitnation, process(source), 1, false);
            return;
        }
    }
}
