namespace Sen.Script.Support.Wwise.Media.Decode {
    export function process(source: string, destination: string) {
        const operating_system = Kernel.OperatingSystem.current();
        assert(operating_system === "Windows" || operating_system === "macOS" || operating_system === "Linux", Kernel.Language.get("wwise.media.decode.unsupported_operating_system"));
        const vgmstream_program_file_path = Script.Support.Wwise.Media.Common.search_path("vgmstream-cli");
        assert(vgmstream_program_file_path !== null, Kernel.Language.get("wwise.media.decode.cannot_find_third"));
        //  const fallback_temporary_path = `${Kernel.Process.get_path_environment("TEMP")}/wem`;
        //  Kernel.FileSystem.create_directory(fallback_temporary_path);
        const command = `${vgmstream_program_file_path} -o "${destination}" "${source}"`;
        const program_result = Kernel.Process.execute(command);
        let regex_result = /^encoding: (.+)$/m.exec(program_result);
        assert(regex_result !== null);
        let encoding = regex_result![1];
        let format: null | Common.Format = null;
        switch (encoding) {
            case "Little Endian 16-bit PCM": {
                format = "pcm";
            }
            case "Platinum 4-bit ADPCM": {
                format = "adpcm";
            }
            case "Audiokinetic Wwise 4-bit IMA ADPCM": {
                format = "adpcm";
            }
            case "Custom Vorbis": {
                format = "vorbis";
            }
            case "AAC (Advanced Audio Coding)": {
                format = "aac";
            }
            case "libopus Opus": {
                format = "wemopus";
            }
        }
        assert(format !== null, Script.format(Kernel.Language.get("wwise.media.decode.unknown_wem_format"), format));
        return;
    }

    export function process_fs(source: string, destination: string) {
        process(source, destination);
        return;
    }
}
