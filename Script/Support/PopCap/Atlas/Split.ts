namespace Sen.Script.Support.PopCap.Atlas.Split {
    // using the definition

    export type ImageRes = Sen.Kernel.Support.PopCap.ResourceGroup.ResourceContainsSprite;

    // using resource

    export type ResourceSubgroup = Sen.Kernel.Support.PopCap.ResourceGroup.ResourceSubgroup;

    /**
     * source[0] - json file
     * source[n] - images
     */

    export type Source = [string, ...params: Array<string>];

    /**
     * Support PvZ2 Resource-Group
     */

    export namespace ResourceGroup {
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
         *
         * @param resource - Resource file
         * @returns
         */

        export function is_sprite_container(resource: Sen.Kernel.Support.PopCap.ResourceGroup.ResourceContainsImage): resource is Sen.Kernel.Support.PopCap.ResourceGroup.ResourceContainsSprite {
            if ((resource as ImageRes).ax !== undefined && (resource as ImageRes).ay !== undefined && (resource as ImageRes).ah !== undefined && (resource as ImageRes).aw !== undefined) {
                return true;
            }
            return false;
        }

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
            const definition: Sen.Script.Support.PopCap.Atlas.Structure.Definition = {
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
                            x: subgroup.x ?? 0n,
                            y: subgroup.y ?? 0n,
                        },
                    };
                    if (subgroup.cols && subgroup.cols !== 1n) {
                        wrapper.default.cols = subgroup.cols;
                    }
                    if (subgroup.rows && subgroup.rows !== 1n) {
                        wrapper.default.rows = subgroup.rows;
                    }
                    definition.groups[subgroup.id] = wrapper;
                }
            }
            return definition;
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
        ): Structure.Definition {
            const resources_used: ResourceSubgroup = {
                ...resource,
                resources: [],
            };
            const sprite_destination: string = Sen.Kernel.Path.join(destination, "media");
            const by_path: boolean = method === "path";
            const style_use_string: boolean = style === "string";
            Sen.Kernel.FileSystem.create_directory(sprite_destination);
            const image_wrapper: Map<string, Array<Sen.Kernel.Image.RectangleFileIO>> = new Map<string, Array<Sen.Kernel.Image.RectangleFileIO>>();
            images.forEach((e: string) => image_wrapper.set(e.toUpperCase(), []));
            for (const current_resource of resource.resources) {
                if (is_sprite_container(current_resource)) {
                    for (const png of images) {
                        const current_parent: string = png.replaceAll(/\.png/gi, "").toUpperCase();
                        if (current_resource.parent && current_parent.endsWith(current_resource.parent.replace("ATLASIMAGE_ATLAS_", ""))) {
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
                                destination: ResourceGroup.destination(sprite_destination, current_resource, by_path),
                            });
                            resources_used.resources.push(current_resource);
                            break;
                        }
                    }
                }
            }
            if (by_path) {
                image_wrapper.forEach((data: Array<Sen.Kernel.Image.RectangleFileIO>, source: string) => Sen.Kernel.Image.cut_multiple_fs(source, data));
            } else {
                image_wrapper.forEach((data: Array<Sen.Kernel.Image.RectangleFileIO>, source: string) => Sen.Kernel.Image.cut_multiple_fs_asynchronous(source, data));
            }
            return make_definition(resources_used, method, style);
        }

        /**
         *
         * @param param0 - json file
         * @param destination - output
         * @param method - method to split
         * @param style - path style
         * @returns
         */

        export function process_fs(
            [json, ...images]: Sen.Script.Support.PopCap.Atlas.Split.Source,
            destination: string,
            method: Sen.Script.Support.PopCap.Atlas.Structure.TypicalMethod,
            style: Sen.Script.Support.PopCap.Atlas.Structure.PathStyle,
        ): void {
            const definition = process(Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.ResourceGroup.ResourceSubgroup>(json), images, method, style, destination);
            Kernel.JSON.serialize_fs<Structure.Definition>(Kernel.Path.join(destination, "atlas.json"), definition, 1, false);
            return;
        }
    }

    export namespace ResInfo {
        /**
         *
         * @param resource - Resource file
         * @returns
         */

        export function is_sprite_container(resource: unknown): resource is Kernel.Support.PopCap.ResInfo.Default {
            if ((resource as ImageRes).ax !== undefined && (resource as ImageRes).ay !== undefined && (resource as ImageRes).ah !== undefined && (resource as ImageRes).aw !== undefined) {
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

        export function destination(destination: string, resource: Kernel.Support.PopCap.ResInfo.Sprite, id: string | undefined): string {
            if (id === undefined) {
                return `${Sen.Kernel.Path.join(destination, (resource.path as string).split("/").at(-1)!)}.png`;
            }
            return `${Sen.Kernel.Path.join(destination, id)}.png`;
        }

        export function make_definition(
            resource: Sen.Kernel.Support.PopCap.ResInfo.Atlas,
            method: Sen.Script.Support.PopCap.Atlas.Structure.TypicalMethod,
            id: string,
            style?: Sen.Script.Support.PopCap.Atlas.Structure.PathStyle,
        ): Sen.Script.Support.PopCap.Atlas.Structure.Definition {
            const definition: Sen.Script.Support.PopCap.Atlas.Structure.Definition = {
                method: method,
                expand_path: style ?? "array",
                subgroup: id,
                trim: false,
                res: resource.type,
                groups: {},
            };
            const parents: Array<string> = Object.keys(resource.packet);
            for (const parent of parents) {
                const sub_keys: Array<string> = Object.keys(resource.packet[parent].data);
                for (const key of sub_keys) {
                    const group: Structure.BasicGroup = {
                        default: {
                            x: resource.packet[parent].data[key].default.x ?? 0n,
                            y: resource.packet[parent].data[key].default.y ?? 0n,
                        },
                        path: resource.packet[parent].data[key].path,
                    };
                    if (resource.packet[parent].data[key].default.cols && resource.packet[parent].data[key].default.cols !== 1n) {
                        group.default.cols = resource.packet[parent].data[key].default.cols;
                    }
                    if (resource.packet[parent].data[key].default.rows && resource.packet[parent].data[key].default.rows !== 1n) {
                        group.default.rows = resource.packet[parent].data[key].default.rows;
                    }
                    definition.groups[key] = group;
                }
            }
            return definition;
        }

        export function process(
            resource: Sen.Kernel.Support.PopCap.ResInfo.Atlas,
            images: Array<string>,
            id: string,
            method: Sen.Script.Support.PopCap.Atlas.Structure.TypicalMethod,
            destination: string,
            style?: Sen.Script.Support.PopCap.Atlas.Structure.PathStyle,
        ): Sen.Script.Support.PopCap.Atlas.Structure.Definition {
            const resources_used: Sen.Kernel.Support.PopCap.ResInfo.Atlas = {
                ...resource,
                packet: {},
            };
            const parents: Array<string> = Object.keys(resource.packet);
            Kernel.FileSystem.create_directory(destination);
            const sprite_destination: string = Kernel.Path.join(destination, "media");
            Kernel.FileSystem.create_directory(sprite_destination);
            const image_wrapper: Map<string, Array<Sen.Kernel.Image.RectangleFileIO>> = new Map<string, Array<Sen.Kernel.Image.RectangleFileIO>>();
            images.forEach((e: string) => image_wrapper.set(e, []));
            const by_path: boolean = method === "path";
            for (const parent of parents) {
                const ids: Array<string> = Object.keys(resource.packet[parent].data);
                resources_used.packet[parent] = {
                    ...resource.packet[parent],
                    data: {},
                };
                for (const id of ids) {
                    const group: Kernel.Support.PopCap.ResInfo.Sprite = resource.packet[parent].data[id];
                    const default_subinfo = resource.packet[parent].data[id].default;
                    if (is_sprite_container(default_subinfo)) {
                        for (const png of images) {
                            const current_parent = png.replaceAll(/\.png$/gi, "").toUpperCase();
                            if (current_parent.endsWith(parent.replace("ATLASIMAGE_ATLAS_", ""))) {
                                image_wrapper.get(png)!.push({
                                    x: Number(default_subinfo.ax),
                                    y: Number(default_subinfo.ay),
                                    width: Number(default_subinfo.aw),
                                    height: Number(default_subinfo.ah),
                                    destination: ResInfo.destination(sprite_destination, group, by_path ? undefined : id),
                                });
                                resources_used.packet[parent].data[id] = {
                                    default: {
                                        ...default_subinfo,
                                    },
                                    path: group.path,
                                    type: group.type,
                                };
                            }
                        }
                    }
                }
            }
            if (by_path) {
                image_wrapper.forEach((data: Array<Sen.Kernel.Image.RectangleFileIO>, source: string) => Sen.Kernel.Image.cut_multiple_fs(source, data));
            } else {
                image_wrapper.forEach((data: Array<Sen.Kernel.Image.RectangleFileIO>, source: string) => Sen.Kernel.Image.cut_multiple_fs_asynchronous(source, data));
            }
            return make_definition(resources_used, method, id, style);
        }

        /**
         *
         * @param param0 - json file
         * @param destination - output
         * @param method - method to split
         * @param style - path style
         * @returns
         */

        export function process_fs(
            [json, ...images]: Sen.Script.Support.PopCap.Atlas.Split.Source,
            destination: string,
            method: Sen.Script.Support.PopCap.Atlas.Structure.TypicalMethod,
            style?: Sen.Script.Support.PopCap.Atlas.Structure.PathStyle,
        ): void {
            const definition = process(Kernel.JSON.deserialize_fs<Sen.Kernel.Support.PopCap.ResInfo.Atlas>(json), images, Kernel.Path.base_without_extension(json), method, destination, style);
            Kernel.JSON.serialize_fs<Structure.Definition>(Sen.Kernel.Path.join(destination, "atlas.json"), definition, 1, false);
            return;
        }
    }
}
