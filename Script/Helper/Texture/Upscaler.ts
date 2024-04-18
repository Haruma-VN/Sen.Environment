namespace Sen.Script.Helper.Texture.Upscaler {
    export const modelX: Array<string> = ["realesr-animevideov3", "realesrgan-x4plus", "realesrgan-x4plus-anime", "realesrnet-x4plus"];

    export type Model = (typeof modelX)[number];

    export const scaleX: Array<bigint> = [2n, 3n, 4n];

    export interface Switchable {
        model: (typeof modelX)[number];
        scale: (typeof scaleX)[number];
    }

    export type Scale = (typeof scaleX)[number];

    export function search_path(name: string) {
        let result: null | string = null;
        const item_delimiter = Kernel.OperatingSystem.current() === "Windows" ? ";" : ":";
        assert(Kernel.Process.is_exists_in_path_environment("PATH"), Kernel.Language.get("environment_path_not_found"));
        const path_environment_list = Kernel.Process.get_path_environment("PATH").split(item_delimiter);
        const path_extension_list = [""];
        if (Kernel.OperatingSystem.current() === "Windows") {
            assert(Kernel.Process.is_exists_in_path_environment("PATHEXT"), Kernel.Language.get("environment_pathext_not_found"));
            const path_extension_environment = Kernel.Process.get_path_environment("PATHEXT");
            path_extension_list.push(...path_extension_environment.split(item_delimiter).map((value) => value.toLowerCase()));
        }
        for (let path of path_environment_list) {
            let path_base = `${path}/${name}`;
            let path_extension = path_extension_list.find((value) => Kernel.FileSystem.is_file(`${path_base}${value}`));
            if (path_extension !== undefined) {
                result = `${path_base}${path_extension}`;
                break;
            }
        }
        return result;
    }

    export function process(source: string, destination: string, model: Model, scale: Scale, realesrgan_path: string | null): void {
        const operating_system = Kernel.OperatingSystem.current();
        assert(
            operating_system === "Windows" || operating_system === "macOS" || operating_system === "Linux",
            Kernel.Language.get("script.helper.texture.this_function_only_support_windows_linux_macos"),
        );
        if (realesrgan_path!.length === 0) {
            realesrgan_path = search_path("realesrgan-ncnn-vulkan");
        }
        assert(realesrgan_path !== null, Kernel.Language.get("script.helper.texture.cannot_find_real_esrgan"));
        if (model !== "realesr-animevideov3" && scale !== 4n) {
            scale = 4n;
            Console.warning(Kernel.Language.get("script.helper.texture.upscaler.debug"));
        }
        const out_path = Kernel.Path.join(Kernel.Path.dirname(destination), `${Kernel.Path.base_without_extension(destination)}_x${scale}${Kernel.Path.extname(destination)}`);
        if (Kernel.FileSystem.is_file(out_path)) {
            Kernel.FileSystem.Operation.remove(out_path);
        }
        const command = `${realesrgan_path} -i "${source}" -o "${out_path}" -n ${model} -s ${scale}`;
        Kernel.Process.run(command); //TODO remove execute print on console.
        assert(Kernel.FileSystem.is_file(out_path), Kernel.Language.get("script.helper.texture.upscaler.failed_to_upscale"));
        return;
    }

    export function as_switch(num: bigint): Switchable {
        const destination: Switchable = {
            model: modelX[0],
            scale: 4n,
        };
        switch (num) {
            case 1n: {
                destination.scale = 3n;
                break;
            }
            case 2n: {
                destination.model = modelX[1];
                break;
            }
            case 3n: {
                destination.model = modelX[2];
                break;
            }
            case 4n: {
                destination.model = modelX[3];
                break;
            }
        }
        return destination;
    }

    export function execute() {
        const source = Console.path(Kernel.Language.get("script.helper.texture.upscaler.input_source"), "any");
        modelX.forEach((e, i) => Console.argument(`${i + 1}. ${e}`));
        const data = as_switch(Executor.input_integer([1n, 2n, 3n, 4n]));
        // Edit this if you want to provide your own path for default
        const realesrgan_path: string = Console.path(Kernel.Language.get("script.helper.texture.upscaler.input_realesrgan_path"), "any");
        process(source, source, data.model, data.scale, realesrgan_path);
        return;
    }
}
Sen.Script.Helper.Texture.Upscaler.execute();
