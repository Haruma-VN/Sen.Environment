namespace Sen.Script.Support.Miscellaneous.Custom.ResourceStreamBundle.Configuration {
    // -----------------------------------------------------

    export type Generic = 0n | 1n | 2n;

    // -----------------------------------------------------

    export interface PackagesSetting extends Record<string, unknown> {
        rton_count: bigint;
        json_count: bigint;
        key: string;
        iv: string;
    }

    export interface Setting extends Record<string, unknown> {
        texture_format_category: bigint;
        only_high_resolution: boolean;
        packages_setting: PackagesSetting;
    }
}
