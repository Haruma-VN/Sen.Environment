namespace Sen.Script.Support.Texture.Upscaler {
    /**
     * Detail namespace
     */

    export namespace Detail {
        export function sprite_generic(): Array<[bigint, bigint, string]> {
            return [
                [1n, 0n, Kernel.Language.get("popcap.animation.miscellaneous.to_apng.enable_all")], // TODO: add localization.
                [2n, 1n, Kernel.Language.get("popcap.animation.miscellaneous.to_apng.disable_all")],
                [3n, 2n, Kernel.Language.get("popcap.animation.miscellaneous.to_apng.select_to_disable")],
            ];
        }
    }

    export function load_string_by_int(rule: any): string {
        const new_rule: Array<bigint> = [];
        rule.forEach(function make_rule(e: [bigint, string] & any): void {
            if (Shell.is_gui()) {
                Kernel.Console.print(`${e[0]}. ${e[1]}`);
            } else {
                Kernel.Console.print(`    ${e[0]}. ${e[1]}`);
            }
            new_rule.push(e[0]);
        });
        return rule[Number(Sen.Script.Executor.input_integer(new_rule) - 1n)][1];
    }

    export function process(source: string, destination: string) {
        const operating_system = Kernel.OperatingSystem.current();
        assert(operating_system === "Windows" || operating_system === "macOS" || operating_system === "Linux", "unsupported_operating_system"); // TODO
        const real_esrgan_program_third_path = Home.query("~/../thirdapp/real_esrgan"); //
        const real_esrgan_program_file_path = `${real_esrgan_program_third_path}/realesrgan-ncnn-vulkan${operating_system === "Windows" ? ".exe" : ""}`;
        assert(Kernel.FileSystem.is_file(real_esrgan_program_file_path), "cannot_find_third"); // TODO
        assert(Kernel.FileSystem.is_directory(`${real_esrgan_program_third_path}/models`), "cannot_find_real_esrgan_models_folder"); // TODO
        const model_list: Array<string> = Kernel.FileSystem.read_directory_only_file(`${real_esrgan_program_third_path}/models`).filter((e) => Kernel.Path.extname(e).toLowerCase() === ".param");
        assert(model_list.length > 0, "real_esrgan_model_folder_is_empty");
        const model_rule: Array<[bigint, string]> = model_list.map((e, i) => [BigInt(i + 1), Kernel.Path.base_without_extension(e).toLowerCase()]);
        Console.argument("Enter model"); //TODO.
        const model_selected: string = load_string_by_int(model_rule);
        const command = `${real_esrgan_program_file_path} -i "${source}" -o "${destination}" -n "${model_selected}" > nul 2>&1`;
        Kernel.FileSystem.Operation.remove(destination);
        Kernel.Process.execute(command);
        assert(Kernel.FileSystem.is_file(destination), "failed_to_upscale_image"); //TODO
        return;
    }

    export function process_fs(source: string, destination: string) {
        process(source, destination);
        return;
    }
}
