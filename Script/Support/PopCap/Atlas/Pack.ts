namespace Sen.Script.Support.PopCap.Atlas.Pack {
    /**
     * JS Detail Definition
     */

    export namespace Detail {
        /**
         * Readable obj content by the tool
         */

        export interface PackableData<T extends number | bigint> {
            id: string;
            path: Array<string>;
            additional_x: T;
            additional_y: T;
            cols?: T;
            rows?: T;
        }

        /**
         * Readable obj content by MaxRectsBin third
         */

        export interface MaxRectsPackableData<T extends number | bigint> extends PackableData<T> {
            width: T;
            height: T;
            source: string;
        }

        /**
         * Readable obj content by MaxRectsBin third
         */

        export interface MaxRectsAfterData<T extends number | bigint> extends MaxRectsPackableData<T> {
            x: T;
            y: T;
            oversized?: boolean;
        }

        /**
         * Dimension interface
         */

        export type Dimension<T extends number | bigint> = Record<"width" | "height", T>;

        /**
         * Size Range interface
         */

        export type SizeRange<T extends number | bigint> = Record<"padding", T> & Dimension<T>;

        /**
         * For reducer trim
         */

        export type MaxDimensionView<T extends bigint | number> = Record<"max_width" | "max_height", T>;

        /**
         * For reducer trim
         */

        export type RectangleView<T extends bigint | number> = Record<"x" | "y" | "width" | "height", T>;

        /**
         * Detail
         */

        export interface Data {
            smart: boolean;
            pot: boolean;
            square: boolean;
            allowRotation: boolean;
            [Symbol.iterator]?: () => Iterator<this>;
        }

        /**
         *
         * @param list - Array of data
         * @returns Trim records
         */

        export function reducer_trim<T extends RectangleView<number>>(list: Array<T>): MaxDimensionView<number> {
            return list.reduce(
                (acc: MaxDimensionView<number>, rect: RectangleView<number>) => ({
                    max_width: Math.max(acc.max_width, rect.x + rect.width),
                    max_height: Math.max(acc.max_height, rect.y + rect.height),
                }),
                {
                    max_width: 0,
                    max_height: 0,
                },
            );
        }

        /**
         *
         * @param list - Array of data
         * @returns Square area records
         */

        export function square_trim<T extends RectangleView<number>>(list: Array<T>): MaxDimensionView<number> {
            const result: MaxDimensionView<number> = reducer_trim(list);
            return {
                max_width: Algorithm.create_2n_square(result.max_width),
                max_height: Algorithm.create_2n_square(result.max_height),
            };
        }
    }

    export namespace Algorithm {
        /**
         *
         * @param num - Provide any number
         * @returns Check if the number is a power of 2
         */

        export function create_2n_square(num: number): number {
            return Math.pow(2, Math.ceil(Math.log2(num)));
        }

        /**
         *
         * @param resource - Atlas Definition
         * @returns
         */

        export function to_packable(resource: Sen.Script.Support.PopCap.Atlas.Structure.Definition): Array<Sen.Script.Support.PopCap.Atlas.Pack.Detail.MaxRectsPackableData<number>> {
            const members: Array<string> = Object.keys(resource.groups) as Array<string>;
            const result: Array<Sen.Script.Support.PopCap.Atlas.Pack.Detail.MaxRectsPackableData<number>> = new Array<Sen.Script.Support.PopCap.Atlas.Pack.Detail.MaxRectsPackableData<number>>();
            for (const member of members) {
                const data: Sen.Script.Support.PopCap.Atlas.Pack.Detail.MaxRectsPackableData<number> = {
                    id: member,
                    path: resource.groups[member].path,
                    additional_x: Number(resource.groups[member].default.x) ?? 0,
                    additional_y: Number(resource.groups[member].default.y) ?? 0,
                    height: undefined!,
                    source: undefined!,
                    width: undefined!,
                };
                if (resource.groups[member].default.cols) {
                    data.cols = Number(resource.groups[member].default.cols);
                }
                if (resource.groups[member].default.rows) {
                    data.rows = Number(resource.groups[member].default.rows);
                }
                result.push(data);
            }
            return result;
        }

        export function assert_oversize<T extends bigint | number>(list: Array<Array<Detail.MaxRectsAfterData<T>>>): void {
            for (const collection of list) {
                collection.forEach((e: Detail.MaxRectsAfterData<T>) => {
                    if ("oversized" in e && e.oversized) {
                        throw new Error(Kernel.Language.get("popcap.atlas.contains_oversized_image"));
                    }
                });
            }
            return;
        }
    }

    export namespace ResourceGroup {
        export function process(
            source: string,
            size: Sen.Script.Support.PopCap.Atlas.Pack.Detail.SizeRange<number>,
            detail: Sen.Script.Support.PopCap.Atlas.Pack.Detail.Data,
            destination: string,
        ): void {
            const json_source: string = Sen.Kernel.Path.resolve(Sen.Kernel.Path.join(source, "atlas.json"));
            const media_path: string = Sen.Kernel.Path.resolve(Sen.Kernel.Path.join(source, "media"));
            const definition: Sen.Script.Support.PopCap.Atlas.Structure.Definition = Sen.Kernel.JSON.deserialize_fs<Sen.Script.Support.PopCap.Atlas.Structure.Definition>(json_source);
            const is_path: boolean = definition.method === "path";
            const prepare: Array<Sen.Script.Support.PopCap.Atlas.Pack.Detail.MaxRectsPackableData<number>> = Sen.Script.Support.PopCap.Atlas.Pack.Algorithm.to_packable(definition);
            const images: Map<string, Kernel.Dimension.Image> = new Map<string, Kernel.Dimension.Image>();
            for (const data of prepare) {
                let source_file: string = undefined!;
                if (is_path) {
                    source_file = Sen.Kernel.Path.resolve(Sen.Kernel.Path.join(media_path, `${data.path[data.path.length - 1]}.png`));
                } else {
                    source_file = Sen.Kernel.Path.resolve(Sen.Kernel.Path.join(media_path, `${data.id}.png`));
                }
                const image: Sen.Kernel.Dimension.Image = Sen.Kernel.Image.open(source_file);
                images.set(source_file, image);
                data.width = Number(image.width);
                data.height = Number(image.height);
                data.source = Sen.Kernel.Path.resolve(source_file);
            }
            const result: Sen.Kernel.Support.PopCap.ResourceGroup.ResourceSubgroup = {
                id: definition.subgroup,
                parent: definition.subgroup.replace(`_${definition.res}`, ""),
                res: definition.res,
                resources: [],
                type: "simple",
            };
            const list_view: Array<Array<Detail.MaxRectsAfterData<number>>> = new Array();
            const packer = new Sen.Script.Third.MaxRectsAlgorithm.MaxRectsPacker(size.width, size.height, size.padding, detail);
            packer.addArray(prepare as any);
            packer.bins.forEach((bin: Third.MaxRectsAlgorithm.Bin<Third.MaxRectsAlgorithm.Rectangle>) => list_view.push(bin.rects as any));
            if (definition.trim) {
                Console.send(Kernel.Language.get("popcap.atlas.pack.use_trim"));
            }
            const is_string_style: boolean = definition.expand_path === "string";
            for (let i = 0; i < list_view.length; ++i) {
                const destination_size: Detail.MaxDimensionView<number> = definition.trim ? Detail.reducer_trim(list_view[i]) : Detail.square_trim(list_view[i]);
                if (destination_size.max_width !== size.width) {
                    Console.send(`Changed width from ${size.width} to ${destination_size.max_width}`);
                }
                if (destination_size.max_height !== size.height) {
                    Console.send(`Changed height from ${size.height} to ${destination_size.max_height}`);
                }
                const parent_name: string = `${result.id}_${i < 10 ? `0${i}` : `${i}`}`;
                result.resources.push({
                    slot: undefined!,
                    id: `ATLASIMAGE_ATLAS_${parent_name.toUpperCase()}`,
                    path: is_string_style ? `atlases\\${parent_name}` : ["atlases", parent_name],
                    type: "Image",
                    atlas: true,
                    width: BigInt(destination_size.max_width),
                    height: BigInt(destination_size.max_height),
                    runtime: true,
                } as Kernel.Support.PopCap.ResourceGroup.ResourceContainsAtlas);
                for (let j = 0; j < list_view[i].length; ++j) {
                    const resource_data: Kernel.Support.PopCap.ResourceGroup.ResourceContainsSprite = {
                        slot: undefined!,
                        id: list_view[i][j].id,
                        path: is_string_style ? list_view[i][j].path.join("\\") : list_view[i][j].path,
                        type: "Image",
                        parent: `ATLASIMAGE_ATLAS_${parent_name.toUpperCase()}`,
                        ax: BigInt(list_view[i][j].x),
                        ay: BigInt(list_view[i][j].y),
                        aw: BigInt(list_view[i][j].width),
                        ah: BigInt(list_view[i][j].height),
                        x: BigInt(list_view[i][j].additional_x),
                        y: BigInt(list_view[i][j].additional_y),
                    };
                    if (list_view[i][j].cols) {
                        resource_data.cols = BigInt(list_view[i][j].cols!);
                    }
                    if (list_view[i][j].rows) {
                        resource_data.rows = BigInt(list_view[i][j].rows!);
                    }
                    result.resources.push(resource_data);
                }
                const image_destination: string = Kernel.Path.join(destination, `${parent_name.toUpperCase()}.png`);
                list_view[i].forEach((e: any) => {
                    e.x = BigInt(e.x);
                    e.y = BigInt(e.y);
                    e.width = BigInt(e.width);
                    e.height = BigInt(e.height);
                });
                Kernel.Image.join_png(
                    image_destination,
                    Kernel.Dimension.instance(BigInt(destination_size.max_width), BigInt(destination_size.max_height)),
                    list_view[i].map((e: Detail.MaxRectsAfterData<number>) => ({ ...images.get(e.source), ...e } as any)),
                );
            }
            Kernel.JSON.serialize_fs(Kernel.Path.join(destination, `${definition.subgroup}.json`), result, 1, false);
            return;
        }

        export function process_fs(
            source: string,
            size: Sen.Script.Support.PopCap.Atlas.Pack.Detail.SizeRange<number>,
            detail: Sen.Script.Support.PopCap.Atlas.Pack.Detail.Data,
            destination: string,
        ): void {
            process(source, size, detail, destination);
            return;
        }
    }
}
