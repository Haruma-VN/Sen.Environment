namespace Sen.Script.Helper.WWise.AddMusic {

    /**
     * 
     */

    const music_type = "main_path";

    interface MusicSetting {
        audio_format: Sen.Script.Support.Wwise.Media.Common.Format;
    };

    interface Media {
        target_index: bigint;
        volume: number;
        duration: number;
        loop: number;
    };

    interface MusicChild {
        switches_name: string;
        child: Media[];
    };

    interface MusicInformation {
        audio_name: string;
        type: string;
        music: MusicChild[];
    };

    interface InjectionMusicStruct {
        setting: MusicSetting;
        media: string[];
        music: MusicInformation[];
    };

    export function encode_audio(data: InjectionMusicStruct, source: string) {
        const media_list: bigint[] = [];
        Kernel.FileSystem.create_directory(`${source}/.cache`);
        for (let media_element of data.media) {
            const destination = new Kernel.UInteger32();
            Kernel.Support.WWise.SoundBank.hash(media_element, destination);
            media_list.push(destination.value);
            Sen.Script.Support.Wwise.Media.Encode.process_fs(`${source}/media/${media_element}.wav`, `${source}/.cache/${destination.value}.wem`, data.setting.audio_format);
        }
        return media_list;
    }


    export function execute() {
        const source: string = Console.path(Kernel.Language.get("script.helper.wwise.add_music.input_music_path"), "directory");
        const global_data_source: string = Console.path(Kernel.Language.get("script.helper.wwise.add_music.input_global_data_path"), "file");
        const data: InjectionMusicStruct = Kernel.JSON.deserialize_fs(`${source}/data.json`);
        const media_list = encode_audio(data, source);
        debug(media_list);
        const temporary_path = Home.query("~/../temporary");
        const global_data_destination = `${temporary_path}/${Kernel.Path.basename(global_data_source)}.soundbank`;
        Kernel.Support.WWise.SoundBank.decode_fs(global_data_source, global_data_destination);
        Kernel.Support.WWise.SoundBank.Miscellaneous.add_music(source, global_data_destination, `${source}.cache`, media_list);
        Kernel.Support.WWise.SoundBank.encode_fs(global_data_destination, `${Kernel.Path.except_extension(global_data_source)}_new.bnk`);
        return;
    }


};
Sen.Script.Helper.WWise.AddMusic.execute();