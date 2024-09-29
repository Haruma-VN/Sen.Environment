namespace Sen.Script.Support.Wwise.Media.Encode {
    // base on Wwise 2021.1
    export const k_sample_conversion_settings: string = `<?xml version="1.0" encoding="utf-8"?>
    <WwiseDocument Type="WorkUnit" ID="{E271866C-F671-47E6-BC51-1593F78D3B68}" SchemaVersion="103">
        <Conversions>
            <WorkUnit Name="Sample Conversion Settings" ID="{E271866C-F671-47E6-BC51-1593F78D3B68}" PersistMode="Standalone">
                <ChildrenList>
                    <Conversion Name="_pcm" ID="{C1C231EE-7674-41C8-A204-832F63710222}">
                        <PropertyList>
                            <Property Name="Channels" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">4</Value>
                                    <Value Platform="Android">4</Value>
                                </ValueList>
                            </Property>
                            <Property Name="LRMix" Type="Real64">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MaxSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MinSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="SRConversionQuality" Type="int32">
                                <ValueList>
                                    <Value>1</Value>
                                </ValueList>
                            </Property>
                            <Property Name="SampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                        </PropertyList>
                        <ConversionPluginInfoList>
                            <ConversionPluginInfo Platform="iOS">
                                <ConversionPlugin Name="" ID="{E4CEA53E-E20A-474D-9269-A44C574513A0}" PluginName="PCM" CompanyID="0" PluginID="1"/>
                            </ConversionPluginInfo>
                            <ConversionPluginInfo Platform="Android">
                                <ConversionPlugin Name="" ID="{E73F4FCE-2AEE-4932-9B10-81CD50CA2B54}" PluginName="PCM" CompanyID="0" PluginID="1"/>
                            </ConversionPluginInfo>
                        </ConversionPluginInfoList>
                    </Conversion>
                    <Conversion Name="_adpcm" ID="{F32D4E21-F791-42DF-8492-EBD7DDD7A2A7}">
                        <PropertyList>
                            <Property Name="Channels" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">4</Value>
                                    <Value Platform="Android">4</Value>
                                </ValueList>
                            </Property>
                            <Property Name="LRMix" Type="Real64">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MaxSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MinSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="SampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                        </PropertyList>
                        <ConversionPluginInfoList>
                            <ConversionPluginInfo Platform="iOS">
                                <ConversionPlugin Name="" ID="{380EB198-D923-4C0C-8DFF-54F72D7A8CA2}" PluginName="ADPCM" CompanyID="0" PluginID="2"/>
                            </ConversionPluginInfo>
                            <ConversionPluginInfo Platform="Android">
                                <ConversionPlugin Name="" ID="{99CCAA5F-FC6B-48DE-93A4-0ADE935AF45B}" PluginName="ADPCM" CompanyID="0" PluginID="2"/>
                            </ConversionPluginInfo>
                        </ConversionPluginInfoList>
                    </Conversion>
                    <Conversion Name="_vorbis" ID="{189A2F8B-DC2F-44AB-B5B7-0A4C0B2BAD78}">
                        <PropertyList>
                            <Property Name="Channels" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">4</Value>
                                    <Value Platform="Android">4</Value>
                                </ValueList>
                            </Property>
                            <Property Name="LRMix" Type="Real64">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MaxSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MinSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="SampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                        </PropertyList>
                        <ConversionPluginInfoList>
                            <ConversionPluginInfo Platform="iOS">
                                <ConversionPlugin Name="" ID="{6884A0F7-A19C-41CD-A705-BD54D54FACB2}" PluginName="Vorbis" CompanyID="0" PluginID="4"/>
                            </ConversionPluginInfo>
                            <ConversionPluginInfo Platform="Android">
                                <ConversionPlugin Name="" ID="{27A4E896-5237-4397-9B4C-EC846BD756F9}" PluginName="Vorbis" CompanyID="0" PluginID="4"/>
                            </ConversionPluginInfo>
                        </ConversionPluginInfoList>
                    </Conversion>
                    <Conversion Name="_aac" ID="{6817D8E5-C538-4AE8-845F-BC95ED28CE27}">
                        <PropertyList>
                            <Property Name="Channels" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">4</Value>
                                    <Value Platform="Android">4</Value>
                                </ValueList>
                            </Property>
                            <Property Name="LRMix" Type="Real64">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MaxSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MinSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="SampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                        </PropertyList>
                        <ConversionPluginInfoList>
                            <ConversionPluginInfo Platform="iOS">
                                <ConversionPlugin Name="" ID="{5CA1BE1E-77BF-4834-B70C-47727954310A}" PluginName="AAC" CompanyID="0" PluginID="10"/>
                            </ConversionPluginInfo>
                            <ConversionPluginInfo Platform="Android">
                                <ConversionPlugin Name="" ID="{19AD82DD-C013-490D-B802-97B1402B79E1}" PluginName="PCM" CompanyID="0" PluginID="1"/>
                            </ConversionPluginInfo>
                        </ConversionPluginInfoList>
                    </Conversion>
                    <Conversion Name="_opus" ID="{2D38EA40-C5FB-4F85-8CEF-06434DEFB575}">
                        <PropertyList>
                            <Property Name="Channels" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">4</Value>
                                    <Value Platform="Android">4</Value>
                                </ValueList>
                            </Property>
                            <Property Name="LRMix" Type="Real64">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MaxSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MinSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="SampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                        </PropertyList>
                        <ConversionPluginInfoList>
                            <ConversionPluginInfo Platform="iOS">
                                <ConversionPlugin Name="" ID="{B81804D1-D426-41F9-A40C-1C48125FA573}" PluginName="WEM Opus" CompanyID="0" PluginID="20"/>
                            </ConversionPluginInfo>
                            <ConversionPluginInfo Platform="Android">
                                <ConversionPlugin Name="" ID="{4D35DF5C-F37D-4253-B55B-5F7DB2745E96}" PluginName="WEM Opus" CompanyID="0" PluginID="20"/>
                            </ConversionPluginInfo>
                        </ConversionPluginInfoList>
                    </Conversion>
                    <Conversion Name="_wemopus" ID="{8D55F635-8222-4BEA-85A1-AD019F0EDF0F}">
                        <PropertyList>
                            <Property Name="Channels" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">4</Value>
                                    <Value Platform="Android">4</Value>
                                </ValueList>
                            </Property>
                            <Property Name="LRMix" Type="Real64">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MaxSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="MinSampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                            <Property Name="SampleRate" Type="int32">
                                <ValueList>
                                    <Value Platform="iOS">0</Value>
                                    <Value Platform="Android">0</Value>
                                </ValueList>
                            </Property>
                        </PropertyList>
                        <ConversionPluginInfoList>
                            <ConversionPluginInfo Platform="iOS">
                                <ConversionPlugin Name="" ID="{4287F781-5A09-4E1B-8BB0-8CF6E1B4F5AC}" PluginName="WEM Opus" CompanyID="0" PluginID="20"/>
                            </ConversionPluginInfo>
                            <ConversionPluginInfo Platform="Android">
                                <ConversionPlugin Name="" ID="{534034E0-D383-42C5-A23B-3BC769B20661}" PluginName="WEM Opus" CompanyID="0" PluginID="20"/>
                            </ConversionPluginInfo>
                        </ConversionPluginInfoList>
                    </Conversion>
                </ChildrenList>
            </WorkUnit>
        </Conversions>
    </WwiseDocument>
    `;

    export function cast_wwise_internal_path(raw: string): string | undefined {
        const operating_system = Kernel.OperatingSystem.current();
        if (operating_system === "Windows") {
            return raw;
        }
        if (operating_system === "macOS") {
            assert(raw.startsWith("/"), Kernel.Language.get("wwise.media.encode.path_must_starts_with_seperator"));
            return `Z:${raw}`;
        }
        assert(false, Kernel.Language.get("wwise.media.encode.unsupported_operating_system")) as never;
    }

    export function process(source: string, destination: string, format: Common.Format): void {
        const operating_system = Kernel.OperatingSystem.current();
        assert(operating_system === "Windows" || operating_system === "macOS", Kernel.Language.get("wwise.media.encode.unsupported_operating_system"));
        const wwise_program_file_path = Script.Support.Wwise.Media.Common.search_path("WwiseConsole");
        assert(wwise_program_file_path !== null, Kernel.Language.get("wwise.media.encode.could_not_find_wwise_console"));
        const temporary_path = Home.query("~/../temporary");
        const wwise_project_dir = `${temporary_path}/Sample`;
        const wwise_wproj_file = `${wwise_project_dir}/Sample.wproj`;
        const wwise_create_new_project_command = `WwiseConsole create-new-project "${wwise_wproj_file}" --platform "Android" "iOS"`;
        while (true) {
            Kernel.Process.execute(wwise_create_new_project_command);
            if (Kernel.FileSystem.is_file(wwise_wproj_file)) {
                break;
            }
        }
        const wwise_wsources_file = `${wwise_project_dir}/Sample.wsources`;
        const wwise_wsources = {
            ExternalSourcesList: {
                "@attributes": {
                    SchemaVersion: "1",
                    Root: cast_wwise_internal_path(wwise_project_dir),
                },
                Source: {
                    "@attributes": {
                        Path: "Sample.wav",
                        Destination: "Sample.wem",
                        Conversion: `_${format}`,
                    },
                },
            },
        };
        Kernel.FileSystem.write_file(wwise_wsources_file, Kernel.XML.serialize(wwise_wsources));
        Kernel.FileSystem.write_file(`${wwise_project_dir}/Conversion Settings/Sample Conversion Settings.wwu`, k_sample_conversion_settings);
        if (Kernel.FileSystem.is_file(`${wwise_project_dir}/Sample.wav`)) Kernel.FileSystem.Operation.remove(`${wwise_project_dir}/Sample.wav`);
        Kernel.FileSystem.Operation.copy(source, `${wwise_project_dir}/Sample.wav`);
        const platform = {
            pcm: "Android",
            adpcm: "Android",
            vorbis: "Android",
            aac: "iOS",
            opus: "Android",
            wemopus: "Android",
        }[format];
        const wwise_command = `WwiseConsole convert-external-source "${wwise_wproj_file}" --platform "${platform}" --source-file "${wwise_wsources_file}"`;
        const process_result = Kernel.Process.execute(wwise_command);
        const result_string_list = process_result.split("\n");
        assert(result_string_list[result_string_list.length - 2] === "Process completed successfully.", Kernel.Language.get("wwise.media.encode.failed"))
        Kernel.FileSystem.Operation.remove(destination);
        Kernel.FileSystem.Operation.copy(`${wwise_project_dir}/GeneratedSoundBanks/${platform}/Sample.wem`, destination);
        Kernel.FileSystem.Operation.remove_all(wwise_project_dir);
        return;
    }

    export function process_fs(source: string, destination: string, format: Common.Format): void {
        process(source, destination, format);
        return;
    }
}
