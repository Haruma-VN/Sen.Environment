namespace Sen.Script.Support.PopCap.Atlas.Split {
    // using the definition

    export type ImageRes = Sen.Kernel.Support.PopCap.ResourceGroup.ResourceContainsSprite;

    // using resource

    export type ResourceSubgroup = Sen.Kernel.Support.PopCap.ResourceGroup.ResourceSubgroup;

    /**
     *
     * @param resource - Resource file
     * @returns
     */

    export function is_sprite_container(resource: Sen.Kernel.Support.PopCap.ResourceGroup.ResourceContainsImage): resource is Sen.Kernel.Support.PopCap.ResourceGroup.ResourceContainsSprite {
        if ((resource as ImageRes).ax && (resource as ImageRes).ay && (resource as ImageRes).ah && (resource as ImageRes).aw && (resource as ImageRes).path) {
            return true;
        }
        return false;
    }

    /**
     * Make destination
     * @param destination - atlas directory
     * @param resource - resource
     * @param is_path - is path
     * @returns
     */

    export function destination(destination: string, resource: Sen.Kernel.Support.PopCap.ResourceGroup.ResourceContainsSprite, is_path: boolean): string {
        if (is_path) {
            return `${Sen.Kernel.Path.join(destination, (resource.path as string).split("/").at(-1)!)}.png`;
        }
        return `${Sen.Kernel.Path.join(destination, resource.id)}.png`;
    }

    /**
     * Support PvZ2 Resource-Group
     */

    export namespace ResourceGroup {
        /**
         *
         * @param resource - Resource to convert
         * @param method - Method
         * @param style - Style
         * @returns atlas
         */

        export function make_definition(
            resource: Sen.Kernel.Support.PopCap.ResourceGroup.ResourceSubgroup,
            method: Sen.Script.Support.PopCap.Atlas.Structure.TypicalMethod,
            style: Sen.Script.Support.PopCap.Atlas.Structure.PathStyle,
        ): Sen.Script.Support.PopCap.Atlas.Structure.Definition {
            const json: Sen.Script.Support.PopCap.Atlas.Structure.Definition = {
                method: method,
                expand_path: style,
                subgroup: resource.id,
                trim: false,
                res: resource.res,
                groups: {},
            };
            for (const subgroup of resource.resources) {
                if (is_sprite_container(subgroup)) {
                    const wrapper: Sen.Script.Support.PopCap.Atlas.Structure.BasicGroup = {
                        path: subgroup.path as string,
                        default: {
                            x: (subgroup.x ??= 0n),
                            y: (subgroup.y ??= 0n),
                        },
                    };
                    if (subgroup.cols && subgroup.cols !== 1n) {
                        wrapper.default.cols = subgroup.cols;
                    }
                    if (subgroup.rows && subgroup.rows !== 1n) {
                        wrapper.default.rows = subgroup.rows;
                    }
                    json.groups[subgroup.id] = wrapper;
                }
            }
            return json;
        }

        /**
         *
         * @param resource - Resource after deserialize
         * @param images - Images path
         * @param method - Method to split
         * @param style - Path style
         * @param destination - Where to split
         * @returns
         */

        export function process(
            resource: Sen.Kernel.Support.PopCap.ResourceGroup.ResourceSubgroup,
            images: Array<string>,
            method: Sen.Script.Support.PopCap.Atlas.Structure.TypicalMethod,
            style: Sen.Script.Support.PopCap.Atlas.Structure.PathStyle,
            destination: string,
        ): void {
            const resources_used: ResourceSubgroup = {
                ...resource,
                resources: [],
            };
            const sprite_destination: string = Sen.Kernel.Path.join(destination, "media");
            const by_path: boolean = method === "path";
            const style_use_string: boolean = style === "string";
            Sen.Kernel.FileSystem.create_directory(sprite_destination);
            const image_wrapper: Map<string, Array<Sen.Kernel.Image.RectangleFileIO>> = new Map<string, Array<Sen.Kernel.Image.RectangleFileIO>>();
            images.forEach((e: string) => image_wrapper.set(e, []));
            for (const current_resource of resource.resources) {
                if (is_sprite_container(current_resource)) {
                    for (const png of images) {
                        const current_parent: string = png.replaceAll(/\.png/gi, "").toUpperCase();
                        if (current_resource.parent && current_parent.endsWith(current_resource.parent.replaceAll("ATLASIMAGE_ATLAS_", ""))) {
                            if (style_use_string) {
                                current_resource.path = (current_resource.path as string).replaceAll("\\", "/");
                            } else {
                                current_resource.path = (current_resource.path as Array<string>).join("/");
                            }
                            image_wrapper.get(png)!.push({
                                x: Number(current_resource.ax),
                                y: Number(current_resource.ay),
                                width: Number(current_resource.aw),
                                height: Number(current_resource.ah),
                                destination: Sen.Script.Support.PopCap.Atlas.Split.destination(sprite_destination, current_resource, by_path),
                            });
                            resources_used.resources.push(current_resource);
                            break;
                        }
                    }
                }
            }
            if (by_path) {
                image_wrapper.forEach((data: Array<Sen.Kernel.Image.RectangleFileIO>, source: string) => Sen.Kernel.Image.composite_multiple_fs(source, data));
            } else {
                image_wrapper.forEach((data: Array<Sen.Kernel.Image.RectangleFileIO>, source: string) => Sen.Kernel.Image.composite_multiple_fs_asynchronous(source, data));
            }
            Sen.Kernel.JSON.serialize_fs<Structure.Definition>(Sen.Kernel.Path.join(destination, "atlas.json"), make_definition(resources_used, method, style), 1, false);
            return;
        }

        /**
         * source[0] - json file
         * source[n] - images
         */

        export type Source = [string, ...params: Array<string>];

        /**
         *
         * @param param0 - json file
         * @param destination - output
         * @param method - method to split
         * @param style - path style
         * @returns
         */

        export function process_fs(
            [json, ...images]: Sen.Script.Support.PopCap.Atlas.Split.ResourceGroup.Source,
            destination: string,
            method: Sen.Script.Support.PopCap.Atlas.Structure.TypicalMethod,
            style: Sen.Script.Support.PopCap.Atlas.Structure.PathStyle,
        ): void {
            Sen.Script.Support.PopCap.Atlas.Split.ResourceGroup.process(
                Sen.Kernel.JSON.deserialize_fs<Sen.Kernel.Support.PopCap.ResourceGroup.ResourceSubgroup>(json),
                images,
                method,
                style,
                destination,
            );
            return;
        }
    }

    export namespace ResInfo {}
}
