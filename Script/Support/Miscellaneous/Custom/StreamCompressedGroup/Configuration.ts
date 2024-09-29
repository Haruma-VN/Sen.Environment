namespace Sen.Script.Support.Miscellaneous.Custom.StreamCompressedGroup.Configuration {
    // -----------------------------------------------------

    export type Generic = 0n | 1n | 2n;

    // -----------------------------------------------------

    export interface Setting extends Record<string, unknown> {
        decode_method: bigint;
        animation_split_label: boolean;
    }
}
