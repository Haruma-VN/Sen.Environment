namespace Sen.Script.Support.PopCap.Atlas.MultiResolution {
    // Definition

    export type Definition = Structure.Definition;

    // Resolution

    export type Resolution = "1536" | "768" | "384" | "1200" | "640";

    // Category

    export interface Category extends Record<string, unknown> {
        before: Resolution;
        after: Resolution;
        source: string;
        destination: string;
    }

    /**
     * Make new definition
     * @param definition - Definition file
     * @param rule - Rule to check
     * @param value - Value to override
     * @returns - New definition
     */

    export function generator(definition: Definition, rule: RegExp, value: Resolution): Definition {
        const new_definition: Definition = { ...definition, res: value, groups: {} };
        const keys: Array<string> = Object.keys(definition.groups);
        keys.forEach((key) => {
            new_definition.groups[key] = {
                default: {
                    ...definition.groups[key].default,
                },
                path: definition.groups[key].path.replace(rule, value),
            };
        });
        return new_definition;
    }

    // Callback

    export type Callback = (source: string, destination: string, percentage: number) => void;

    /**
     * Make image
     * @param images - Images
     * @param destination - Destination directory
     * @param callback - Callback method
     * @returns
     */

    export function make_image(images: Array<string>, destination: string, percentage: number, callback: Callback): void {
        images.forEach((e) => callback(e, `${destination}/${Kernel.Path.basename(e)}`, percentage));
        return;
    }

    /**
     * Downscale image
     * @param images - Images
     * @param destination - Destination directory
     * @param percentage - Percentage
     * @returns
     */

    export function resize_fs(images: Array<string>, destination: string, percentage: number): void {
        make_image(images, destination, percentage, Kernel.Image.resize_fs);
        return;
    }

    /**
     * Upscale image
     * @param images - Images
     * @param destination - Destination directory
     * @param percentage - Percentage
     * @returns
     */

    export function scale_fs(images: Array<string>, destination: string, percentage: number): void {
        make_image(images, destination, percentage, Kernel.Image.scale_fs);
        return;
    }

    // Z-Callback

    export type ZCall = (images: Array<string>, destination: string, percentage: number) => void;

    /**
     * Make image
     * @param source - Source directory
     * @param destination - Destination directory
     * @param original - Original resolution
     * @param resolution - After resolution
     * @returns
     */

    export function generalization(source: string, destination: string, before: bigint, after: bigint): void {
        let call: ZCall = undefined!;
        if (after < before) {
            call = resize_fs;
        } else {
            call = scale_fs;
        }
        Kernel.FileSystem.create_directory(destination);
        const media: string = `${destination}/media`;
        Kernel.FileSystem.create_directory(media);
        const definition: Definition = generator(Kernel.JSON.deserialize_fs<Definition>(`${source}/atlas.json`), new RegExp(`${before}`, "i"), `${after}` as Resolution);
        Kernel.JSON.serialize_fs<Definition>(`${destination}/atlas.json`, definition, 1, false);
        call(
            Kernel.FileSystem.read_directory_only_file(`${source}/media`).filter((e) => /((\.png))$/i.test(e)),
            media,
            Number(after) / Number(before),
        );
        return;
    }

    // Callback

    export type XCallback = (
        source: string,
        size: Sen.Script.Support.PopCap.Atlas.Pack.Detail.SizeRange<number>,
        detail: Sen.Script.Support.PopCap.Atlas.Pack.Detail.Data,
        destination: string,
    ) => void;

    /**
     * Process method
     * @param view - List of process
     * @returns
     */
    export function process(
        category: Array<Sen.Script.Support.PopCap.Atlas.MultiResolution.Category>,
        size: Sen.Script.Support.PopCap.Atlas.Pack.Detail.SizeRange<number>,
        detail: Sen.Script.Support.PopCap.Atlas.Pack.Detail.Data,
        callback: XCallback,
    ): void {
        for (const e of category) {
            if (e.before === e.after) {
                callback(e.source, size, detail, e.destination);
                continue;
            }
            generalization(e.source, e.destination, BigInt(e.before), BigInt(e.after));
            const distance: number = Number(e.after) / Number(e.before);
            callback(e.source, { padding: size.padding, width: size.width * distance, height: size.height * distance }, { ...detail }, e.destination);
        }
        return;
    }
    /**
     * Process method
     * @param category - List of process
     * @returns
     */
    export function process_fs(
        category: Array<Sen.Script.Support.PopCap.Atlas.MultiResolution.Category>,
        size: Sen.Script.Support.PopCap.Atlas.Pack.Detail.SizeRange<number>,
        detail: Sen.Script.Support.PopCap.Atlas.Pack.Detail.Data,
        use_res_info: boolean,
    ): void {
        let callback: XCallback = undefined!;
        if (use_res_info) {
            callback = Pack.ResInfo.process_fs;
        } else {
            callback = Pack.ResourceGroup.process_fs;
        }
        process(category, size, detail, callback);
        return;
    }
}
