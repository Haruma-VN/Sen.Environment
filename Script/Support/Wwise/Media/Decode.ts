namespace Sen.Script.Support.Wwise.Media.Decode {
    export function process(source: string, destination: string) {
        const operating_system = Kernel.OperatingSystem.current();
        assert(operating_system === "Windows" || operating_system === "macOS" || operating_system === "Linux", Kernel.Language.get("wwise.media.decode.unsupported_operating_system"));
        // const vgmstream_program_file_path = Script.Support.Wwise.Media.Common.search_path("vgmstream-cli");
        const vgmstream_program_third_path = Home.query("~/../thirdapp/vgmstream");
        const vgmstream_program_file_path = `${vgmstream_program_third_path}/vgmstream-cli${(operating_system === "Windows" ? ".exe" : "")}`;
        assert(Kernel.FileSystem.is_file(vgmstream_program_file_path), Kernel.Language.get("wwise.media.decode.cannot_find_third"));
        //  const fallback_temporary_path = `${Kernel.Process.get_path_environment("TEMP")}/wem`;
        //  Kernel.FileSystem.create_directory(fallback_temporary_path);
        const command = `${vgmstream_program_file_path} -o "${destination}" "${source}"`;
        const program_result = Kernel.Process.execute(command);
        let regex_result = /^encoding: (.+)$/m.exec(program_result);
        assert(regex_result !== null);
        let encoding = regex_result![1];
        let format: null | Common.Format = null;
        switch (encoding) {
            case "16-bit Little Endian PCM":
            case "16-bit Big Endian PCM":
            case "16-bit PCM (block)":
            case "8-bit signed PCM":
            case "8-bit signed PCM (block)":
            case "8-bit sign bit PCM":
            case "4-bit signed PCM":
            case "4-bit unsigned PCM":
            case "32-bit float PCM":
            case "24-bit Little Endian PCM":
            case "24-bit Big Endian PCM":
            case "32-bit Little Endian PCM": {
                format = "pcm";
                break;
            }
            case "CRI ADX 4-bit ADPCM":
            case "CRI ADX 4-bit ADPCM (fixed coefficients)":
            case "CRI ADX 4-bit ADPCM (exponential scale)":
            case "CRI ADX 4-bit ADPCM (type 8 encryption)":
            case "CRI ADX 4-bit ADPCM (type 9 encryption)":
            case "Nintendo DSP 4-bit ADPCM":
            case "Nintendo DSP 4-bit ADPCM (subinterleave)":
            case "Nintendo DTK 4-bit ADPCM":
            case "Nintendo AFC 4-bit ADPCM":
            case "Silicon Graphics VADPCM 4-bit ADPCM":
            case "CCITT G.721 4-bit ADPCM":
            case "CD-ROM XA 4-bit ADPCM":
            case "CD-ROM XA 8-bit ADPCM":
            case "Electronic Arts XA 4-bit ADPCM":
            case "Playstation 4-bit ADPCM":
            case "Playstation 4-bit ADPCM (bad flags)":
            case "Playstation 4-bit ADPCM (configurable)":
            case "Playstation 4-bit ADPCM (Pivotal)":
            case "Sony HEVAG 4-bit ADPCM":
            case "Electronic Arts EA-XA 4-bit ADPCM v1":
            case "Electronic Arts EA-XA 4-bit ADPCM v1 (mono/interleave)":
            case "Electronic Arts EA-XA 4-bit ADPCM v2":
            case "Maxis EA-XA 4-bit ADPCM":
            case "Electronic Arts EA-XAS 4-bit ADPCM v0":
            case "Electronic Arts EA-XAS 4-bit ADPCM v1":
            case "IMA 4-bit ADPCM":
            case "IMA 4-bit ADPCM (mono/interleave)":
            case "Intel DVI 4-bit IMA ADPCM":
            case "Intel DVI 4-bit IMA ADPCM (mono/interleave)":
            case "NintendoWare IMA 4-bit ADPCM":
            case "Heavy Iron .snds 4-bit IMA ADPCM":
            case "Quantic Dream 4-bit IMA ADPCM":
            case "Gorilla Systems WV6 4-bit IMA ADPCM":
            case "High Voltage 4-bit IMA ADPCM":
            case "Final Fantasy Tactics A2 4-bit IMA ADPCM":
            case "Blitz Games 4-bit IMA ADPCM":
            case "MT Framework 4-bit IMA ADPCM":
            case "Microsoft 4-bit IMA ADPCM":
            case "Microsoft 4-bit IMA ADPCM (mono)":
            case "XBOX 4-bit IMA ADPCM":
            case "XBOX 4-bit IMA ADPCM (multichannel)":
            case "XBOX 4-bit IMA ADPCM (mono)":
            case "NDS-style 4-bit IMA ADPCM":
            case "Eurocom DAT4 4-bit IMA ADPCM":
            case "Radical 4-bit IMA ADPCM":
            case "Radical 4-bit IMA ADPCM (mono)":
            case "Apple Quicktime 4-bit IMA ADPCM":
            case "FSB 4-bit IMA ADPCM":
            case "Audiokinetic Wwise 4-bit IMA ADPCM":
            case "Reflections 4-bit IMA ADPCM":
            case "Rockstar AWC 4-bit IMA ADPCM":
            case "Ubisoft 4-bit IMA ADPCM":
            case "Ubisoft 4-bit SCE IMA ADPCM":
            case "Hudson HVQM4 4-bit IMA ADPCM":
            case "Crystal Dynamics 4-bit IMA ADPCM":
            case "CrankcaseAudio REV 4-bit IMA ADPCM":
            case "Microsoft 4-bit ADPCM":
            case "Microsoft 4-bit ADPCM (mono)":
            case "Microsoft 4-bit ADPCM (Cricket Audio)":
            case "Westwood Studios VBR ADPCM":
            case "Yamaha AICA 4-bit ADPCM":
            case "Yamaha AICA 4-bit ADPCM (mono/interleave)":
            case "Capcom Yamaha 4-bit ADPCM":
            case "tri-Ace Aska 4-bit ADPCM":
            case "Nex NXAP 4-bit ADPCM":
            case "Tiger Game.com 4-bit ADPCM":
            case "Procyon Studio Digital Sound Elements NDS 4-bit APDCM":
            case "Level-5 4-bit ADPCM":
            case "Gizmondo Studios Helsingborg LSF 4-bit ADPCM":
            case "Konami MTAF 4-bit ADPCM":
            case "Konami MTA2 4-bit ADPCM":
            case "Paradigm MC3 3-bit ADPCM":
            case "FMOD FADPCM 4-bit ADPCM":
            case "Argonaut ASF 4-bit ADPCM":
            case "Tantalus 4-bit ADPCM":
            case "Ocean DSA 4-bit ADPCM":
            case "Konami XMD 4-bit ADPCM":
            case "PC-FX 4-bit ADPCM":
            case "OKI 4-bit ADPCM (16-bit output)":
            case "OKI 4-bit ADPCM (4-shift)":
            case "Platinum 4-bit ADPCM":
            case "LucasArts iMUSE VIMA ADPCM":
            case "CompressWave Huffman ADPCM":
            case "Circus 8-bit ADPCM":
            case "Ubisoft 4/6-bit ADPCM":
            case "Ongakukan 4-bit ADPCM":
            case "CRI ADPCM_WII header":
            case "Headerless PS-ADPCM raw header":
            case "NAOMI/NAOMI2 Arcade games ADPCM header":
            case "assumed The Warriors Sony ADPCM by .wmus extension":
            case "Bethesda .mcadpcm header":
            case "Capcom .ADPCM header":
            case "Exient WIIADPCM header": {
                format = "adpcm";
                break;
            }
            case "Ogg Vorbis":
            case "Custom Vorbis": {
                format = "vorbis";
                break;
            }
            case "tri-Ace AAC header":
            case "Namco NAAC header":
            case "MP4/AAC header":
            case "MPEG-4 AAC":
            case "AAC (Advanced Audio Coding)": {
                format = "aac";
                break;
            }
            case "libopus Opus":
            case "Epic Games UE4OPUS header":
            case "Exient XOPUS header":
            case "Ogg Opus header":
            case "Nintendo Switch OPUS header": {
                format = "wemopus";
                break;
            }
        }
        assert(format !== null, Script.format(`${Kernel.Language.get("wwise.media.decode.unknown_audio_format")}`, encoding));
        Script.Console.display(`${Kernel.Language.get("wwise.media.audio_format")}:`, `${encoding} (${format})`, Definition.Console.Color.GREEN);
        return;
    }

    export function process_fs(source: string, destination: string) {
        process(source, destination);
        return;
    }
}
