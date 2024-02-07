namespace Sen.Script.Support.PopCap.Animation {
    export namespace ToAnimation {
        const initial_transform: number[] = [1.0, 0.0, 0.0, 1.0, 0.0, 0.0];

        const initial_color: number[] = [1.0, 1.0, 1.0, 1.0];

        export interface FrameNode {
            frames: Structure.AnimationFrame[];
            layer_length: bigint;
        }

        export function process(source: string): SexyAnimation {
            debug("process");
            const record_info: ExtraInfo = Kernel.JSON.deserialize_fs(Kernel.Path.join(source, "record.json"));
            const animation_image_id_list: string[] = Kernel.FileSystem.read_directory_only_file(Kernel.Path.join(source, "library", "image"))
                .filter((e) => /(.*)\.xml$/gi.test(e))
                .map((e) => Kernel.Path.base_without_extension(e));
            const animation_image_map: Record<string, Structure.AnimationImage> = {};
            for (let image_id of animation_image_id_list) {
                const image_document: ImageDocument = Kernel.XML.deserialize_fs(Kernel.Path.join(source, "library", "image", `${image_id}.xml`));
                debug(image_id);
                animation_image_map[image_id] = parse_image(image_document, image_id, record_info);
            }
            const animation_sprite_name_list: string[] = Kernel.FileSystem.read_directory_only_file(Kernel.Path.join(source, "library", "sprite"))
                .filter((e) => /(.*)\.xml$/gi.test(e))
                .map((e) => Kernel.Path.base_without_extension(e));
            const animation_sprite_map: Record<string, Structure.AnimationSprite> = {};
            for (let sprite_name of animation_sprite_name_list) {
                const sprite_document: SpriteDocument = Kernel.XML.deserialize_fs(Kernel.Path.join(source, "library", "sprite", `${sprite_name}.xml`));
                const frame_node: FrameNode = parse_sprite(sprite_document, sprite_name, false, animation_image_id_list, animation_sprite_name_list);
                animation_sprite_map[sprite_name] = {
                    description: "",
                    work_area: {
                        index: 0n,
                        duration: BigInt(frame_node["frames"].length),
                    },
                    frame: frame_node["frames"],
                };
            }
            const animation_label_list: string[] = Kernel.FileSystem.read_directory_only_file(Kernel.Path.join(source, "library", "action"))
                .filter((e) => /(.*)\.xml$/gi.test(e))
                .map((e) => Kernel.Path.base_without_extension(e));
            const animation_action_map: Record<string, FrameNode> = {};
            for (let action_label of animation_label_list) {
                const action_document: SpriteDocument = Kernel.XML.deserialize_fs(Kernel.Path.join(source, "library", "action", `${action_label}.xml`));
                animation_action_map[action_label] = parse_sprite(action_document, action_label, true, animation_image_id_list, animation_sprite_name_list);
            }
            const dom_document: DOMDocument = Kernel.XML.deserialize_fs(Kernel.Path.join(source, "DomDocument.xml"));
            const action_index_list: Record<string, bigint> = get_action_index(dom_document);
            const main_frame: Structure.AnimationFrame[] = merge_action(animation_action_map, action_index_list);
            const animation: Structure.SexyAnimation = {
                version: 6n,
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

        export function sort_list(list: string[]): string[] {
            debug("sort_list");
            return list.sort((a: string, b: string) => a.localeCompare(b, undefined, { numeric: true }));
        }

        export function parse_dom_document(dom_document: DOMDocument, animation: Structure.SexyAnimation, action_index_list: Record<string, bigint>, last_frame: bigint): void {
            debug("parse_dom_document");
            if (!dom_document["DOMDocument"].hasOwnProperty("folder")) {
                throw new Error("document_has_no_folder");
            }
            if (!dom_document["DOMDocument"].hasOwnProperty("media")) {
                throw new Error("document_has_no_media");
            }
            if (!dom_document["DOMDocument"].hasOwnProperty("symbols")) {
                throw new Error("document_has_no_symbols");
            }
            animation["frame_rate"] = BigInt(dom_document["DOMDocument"]["@attributes"]["frameRate"] ?? "30");
            animation["size"]["width"] = BigInt(dom_document["DOMDocument"]["@attributes"]["width"] ?? "390");
            animation["size"]["height"] = BigInt(dom_document["DOMDocument"]["@attributes"]["height"] ?? "390");
            const dom_timeline: DocumentDOMTimeline = dom_document["DOMDocument"]["timelines"]["DOMTimeline"];
            if (dom_timeline["@attributes"]["name"] !== "animation") {
                throw new Error("invaild_dom_document_name");
            }
            const action_label_list: string[] = Object.keys(action_index_list);
            for (let label of action_label_list) {
                const frame_index: bigint = action_index_list[label];
                animation["main_sprite"]["frame"][Number(frame_index)]["label"] = label;
            }
            const dom_command: DocumentDomLayer = dom_document["DOMDocument"]["timelines"]["DOMTimeline"]["layers"]["DOMLayer"][1];
            if (dom_command["@attributes"]["name"] !== "command") {
                throw new Error("invaild_document_commamd");
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
                    throw new Error("invaild_dom_script");
                }
                if (!action_script["script"]["@text"]["is_cdata"]) {
                    throw new Error("invaild_script_cdata");
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
                        throw new Error("invaild_command");
                    }
                    animation["main_sprite"]["frame"][Number(frame_index)]["command"].push({
                        command: regex_result[1],
                        parameter: regex_result[2],
                    });
                }
            }
            return;
        }

        export function merge_action(animation_action_map: Record<string, FrameNode>, action_index_list: Record<string, bigint>): Structure.AnimationFrame[] {
            debug("merge_action");
            const main_frame: Structure.AnimationFrame[] = [];
            const animation_label_list: string[] = Object.keys(action_index_list);
            let label_remove: bigint = 0n;
            let start_index: bigint = 0n;
            const remove_list: bigint[] = [];
            for (let i = 0; i < animation_label_list.length; i++) {
                const action_name: string = animation_label_list[i];
                const layer_length: bigint = animation_action_map[action_name]["layer_length"];
                const action_frames: Structure.AnimationFrame[] = animation_action_map[action_name]["frames"];
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

        export function get_action_index(dom_document: DOMDocument): Record<string, bigint> {
            debug("get_action_index");
            if (!dom_document.hasOwnProperty("DOMDocument")) {
                throw new Error("document_has_no_DOMDocument");
            }
            if (!dom_document["DOMDocument"].hasOwnProperty("timelines")) {
                throw new Error("document_has_no_timelines");
            }
            if (!dom_document["DOMDocument"]["timelines"].hasOwnProperty("DOMTimeline")) {
                throw new Error("document_has_no_DOMTimeline");
            }
            if (!dom_document["DOMDocument"]["timelines"]["DOMTimeline"].hasOwnProperty("layers")) {
                throw new Error("document_has_no_layers");
            }
            if (!dom_document["DOMDocument"]["timelines"]["DOMTimeline"]["layers"].hasOwnProperty("DOMLayer")) {
                throw new Error("document_has_no_DOMLayer");
            }
            const dom_layer: DocumentDomLayer[] = dom_document["DOMDocument"]["timelines"]["DOMTimeline"]["layers"]["DOMLayer"];
            if (dom_layer.length !== 2) {
                throw new Error("invaild_document_DOMLayer_length");
            }
            const dom_flow: DocumentDomLayer = dom_layer[0];
            if (dom_flow["@attributes"]["name"] !== "flow") {
                throw new Error("invaild_document_flow");
            }
            const flow_dom_frames: DocumentDomFrameProperty = dom_flow["frames"]["DOMFrame"];
            const flow_domframes_list: DocumentDomFrame[] = [];
            if (Array.isArray(flow_dom_frames)) {
                flow_domframes_list.push(...flow_dom_frames);
            } else {
                flow_domframes_list.push(flow_dom_frames);
            }
            const action_index_list: Record<string, bigint> = {};
            for (let frame of flow_domframes_list) {
                const frame_index: bigint = BigInt(frame["@attributes"]["index"]);
                if (frame["@attributes"]["name"] !== undefined) {
                    if (frame["@attributes"]["labelType"] !== "name") {
                        throw new Error("invaild_action_label");
                    }
                    action_index_list[frame["@attributes"]["name"]] = frame_index;
                }
            }
            return action_index_list;
        }

        export function parse_sprite(sprite_document: SpriteDocument, sprite_name: string, is_action: boolean, animation_image_id_list: string[], animation_sprite_name_list: string[]): FrameNode {
            debug("parse_sprite");
            if (!sprite_document.hasOwnProperty("DOMSymbolItem")) {
                throw new Error("sprite_has_no_DOMSymbolItem");
            }
            const dom_sprite_name: string = `${is_action ? "action" : "sprite"}/${sprite_name}`;
            if (sprite_document["DOMSymbolItem"]["@attributes"]["name"] !== dom_sprite_name) {
                throw new Error("invaild_sprite_name");
            }
            if (!sprite_document["DOMSymbolItem"].hasOwnProperty("timeline")) {
                throw new Error("sprite_has_no_timeline");
            }
            if (!sprite_document["DOMSymbolItem"]["timeline"].hasOwnProperty("DOMTimeline")) {
                throw new Error("sprite_has_no_DOMtimeline");
            }
            const DOMtimeline: SpriteDomTimeline = sprite_document["DOMSymbolItem"]["timeline"]["DOMTimeline"];
            if (DOMtimeline["@attributes"]["name"] !== sprite_name) {
                throw new Error("invaild_sprite_name");
            }
            if (!DOMtimeline.hasOwnProperty("layers")) {
                throw new Error("sprite_has_no_layers");
            }
            const sprite_layers: SpriteLayersProperty = DOMtimeline["layers"];
            const sprite_layers_list: SpriteLayers[] = [];
            if (Array.isArray(sprite_layers)) {
                sprite_layers_list.push(...sprite_layers);
            } else {
                sprite_layers_list.push(sprite_layers);
            }
            sprite_layers_list.reverse();
            let layer_count: bigint = 0n;
            let model: Model | null = null;
            const frames_result: Structure.AnimationFrame[] = [];

            function get_frame_at_index(index: bigint): Structure.AnimationFrame {
                debug("get_frame_at_index");
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
                debug("close_current_model");
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
                    throw new Error("cannot_find_sprite");
                }
                return index;
            }

            sprite_layers_list.forEach((layer) => {
                if (!layer.hasOwnProperty("DOMLayer")) {
                    throw new Error("sprite_has_no_DOMLayer");
                }
                if (!layer["DOMLayer"].hasOwnProperty("frames")) {
                    throw new Error("sprite_has_no_frames");
                }
                const sprite_frames: SpriteFrameProperty = layer["DOMLayer"]["frames"];
                const sprite_frames_list: SpriteFrame[] = [];
                if (Array.isArray(sprite_frames)) {
                    sprite_frames_list.push(...sprite_frames);
                } else {
                    sprite_frames_list.push(sprite_frames);
                }
                sprite_frames_list.forEach((frame) => {
                    if (!frame.hasOwnProperty("DOMFrame")) {
                        throw new Error("sprite_has_no_DOMFrame");
                    }
                    const dom_frame: SpriteDomFrame = frame["DOMFrame"];
                    const frame_index: bigint = BigInt(dom_frame["@attributes"]["index"]);
                    const frame_duration: bigint = BigInt(dom_frame["@attributes"]["duration"]);
                    if (!frame["DOMFrame"].hasOwnProperty("elements")) {
                        throw new Error("sprite_has_no_elements");
                    }
                    const elements: any = frame["DOMFrame"]["elements"];
                    if (elements === null) {
                        close_current_model();
                        return;
                    }
                    const dom_symbol_instance: DOMSymbolInstance = elements["DOMSymbolInstance"];
                    const library_item_name: string = dom_symbol_instance["@attributes"]["libraryItemName"];
                    const name_match: RegExpMatchArray | null = library_item_name.match(/(image|sprite)/);
                    if (name_match === null) {
                        throw new Error("invaild_name_match");
                    }
                    if (name_match[0] !== name_match[1]) {
                        throw new Error("invaild_name_match");
                    }
                    const sprite_item_name: string = library_item_name.substring(name_match[0].length + 1);
                    const is_sprite: boolean = name_match[0] === "sprite";
                    const current_instance: Instance = {
                        resource: get_resource(is_sprite, sprite_item_name),
                        sprite: is_sprite,
                    };
                    let transform: number[];
                    if (dom_symbol_instance["matrix"] === undefined || dom_symbol_instance["matrix"] === null || Object.keys(dom_symbol_instance["matrix"]).length === 0) {
                        transform = [0.0, 0.0];
                    } else if (dom_symbol_instance["matrix"].hasOwnProperty("Matrix")) {
                        transform = transform_calculator(parse_transform(dom_symbol_instance["matrix"]["Matrix"]));
                    } else {
                        throw new Error("invaild_sprite_matrix");
                    }
                    let color: number[];
                    if (dom_symbol_instance["color"] === undefined || dom_symbol_instance["color"] === null || Object.keys(dom_symbol_instance["color"]).length === 0) {
                        color = [1.0, 1.0, 1.0, 1.0];
                    } else if (dom_symbol_instance["color"].hasOwnProperty("Color")) {
                        color = parse_color(dom_symbol_instance["color"]["Color"]);
                    } else {
                        throw new Error("invaild_sprite_color");
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
                layer_length: BigInt(sprite_layers_list.length),
            };
        }

        export function parse_color(color: Color): number[] {
            debug("parse_color");
            return [
                parse_color_compute(parseFloat(color["@attributes"]["redMultiplier"] ?? "1"), parseFloat(color["@attributes"]["redOffset"] ?? "0")),
                parse_color_compute(parseFloat(color["@attributes"]["greenMultiplier"] ?? "1"), parseFloat(color["@attributes"]["greenOffset"] ?? "0")),
                parse_color_compute(parseFloat(color["@attributes"]["blueMultiplier"] ?? "1"), parseFloat(color["@attributes"]["blueOffset"] ?? "0")),
                parse_color_compute(parseFloat(color["@attributes"]["alphaMultiplier"] ?? "1"), parseFloat(color["@attributes"]["alphaOffset"] ?? "0")),
            ];
        }

        export function parse_color_compute(mutil: number, offset: number): number {
            debug("parse_color_compute");
            return Math.max(0, Math.min(255, mutil * 255 + offset)) / 255;
        }

        export function transform_calculator(transform: number[]): number[] {
            debug("transform_calculator");
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
            debug("parse_number");
            const num = Number(num_str);
            if (Number.isNaN(num)) {
                throw new Error("number_is_NAN");
            }
            return num;
        }

        export function parse_image(image_document: ImageDocument, image_id: string, record_info: ExtraInfo): Structure.AnimationImage {
            debug("parse_image");
            if (!image_document.hasOwnProperty("DOMSymbolItem")) {
                throw new Error("image_has_no_DOMSymbolItem");
            }
            if (image_document["DOMSymbolItem"]["@attributes"]["name"] !== `image/${image_id}`) {
                throw new Error(`invaild_image_name: ${image_id}`);
            }
            if (!image_document["DOMSymbolItem"].hasOwnProperty("timeline")) {
                throw new Error("image_has_no_timeline");
            }
            if (!image_document["DOMSymbolItem"]["timeline"].hasOwnProperty("DOMTimeline")) {
                throw new Error("image_has_no_DOMtimeline");
            }
            const DOMtimeline: DomTimeline = image_document["DOMSymbolItem"]["timeline"]["DOMTimeline"];
            if (DOMtimeline["@attributes"]["name"] !== image_id) {
                throw new Error("invaild_image_id");
            }
            if (!DOMtimeline.hasOwnProperty("layers")) {
                throw new Error("image_has_no_layers");
            }
            if (!DOMtimeline["layers"].hasOwnProperty("DOMLayer")) {
                throw new Error("image_has_no_DOMlayer");
            }
            if (!DOMtimeline["layers"]["DOMLayer"].hasOwnProperty("frames")) {
                throw new Error("image_has_no_frames");
            }
            if (!DOMtimeline["layers"]["DOMLayer"]["frames"].hasOwnProperty("DOMFrame")) {
                throw new Error("image_has_no_DOMframe");
            }
            const DOMframe: DomFrame = DOMtimeline["layers"]["DOMLayer"]["frames"]["DOMFrame"];
            if (!DOMframe.hasOwnProperty("elements")) {
                throw new Error("image_has_no_elements");
            }
            if (!DOMframe["elements"].hasOwnProperty("DOMBitmapInstance")) {
                throw new Error("image_has_no_DOMBitmapInstance");
            }
            const dom_bitmap_instance: DOMBitmapInstance = DOMframe["elements"]["DOMBitmapInstance"];
            const image_name: string = dom_bitmap_instance["@attributes"]["libraryItemName"].substring(6);
            if (!dom_bitmap_instance.hasOwnProperty("matrix")) {
                throw new Error("image_has_no_matrix");
            }
            if (!dom_bitmap_instance["matrix"].hasOwnProperty("Matrix")) {
                throw new Error("invaild_image_matrix");
            }
            const image_transform: number[] = parse_transform_for_image(dom_bitmap_instance["matrix"]["Matrix"]);
            const animation_image: Structure.AnimationImage = {
                name: image_name,
                size: record_info[image_name],
                transform: image_transform,
            };
            return animation_image;
        }

        export function parse_transform_for_image(matrix: Matrix): number[] {
            debug("parse_transform_for_image");
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
            debug("parse_transform");
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
            debug("process_fs");
            Kernel.JSON.serialize_fs<SexyAnimation>(desitnation, process(source), 1, false);
            return;
        }
    }
}
