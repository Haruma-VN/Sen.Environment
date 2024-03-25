namespace Sen.Script.Support.Wwise.Media.Common {

    const FormatX = [
		'pcm',
		'adpcm',
		'vorbis',
		'aac',
		'opus',
		'wemopus',
	] as const;

	export type Format = typeof FormatX[number];

    export function search_path(name: string) {
        let result: null | string = null;
        const item_delimiter = Kernel.OperatingSystem.current() === "Windows" ? ";" : ":";
        // TODO add localization;
        assert(Kernel.Process.is_exists_in_path_environment("PATH"), `enviroment PATH not found`);
        const path_environment_list = Kernel.Process.get_path_environment("PATH").split(item_delimiter);
        const path_extension_list = [""];
        if (Kernel.OperatingSystem.current() === "Windows") {
            // TODO add localization;
			assert(Kernel.Process.is_exists_in_path_environment("PATHEXT"), `environment PATHEXT not found`);
			const path_extension_environment = Kernel.Process.get_path_environment('PATHEXT');
			path_extension_list.push(...path_extension_environment.split(item_delimiter).map((value) => (value.toLowerCase())));
		}
		for (let path of path_environment_list) {
			let path_base = `${path}/${name}`;
			let path_extension = path_extension_list.find((value) => (Kernel.FileSystem.is_file(`${path_base}${value}`)));
			if (path_extension !== undefined) {
				result = `${path_base}${path_extension}`;
				break;
			}
		}
        return result;
    }
}