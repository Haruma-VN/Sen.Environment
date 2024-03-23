namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Configuration {
    // -----------------------------------------------------

    export type Generic = "android" | "ios" | "android-cn";

    // -----------------------------------------------------

    export type Questionable<T> = "?" | T;

    // -----------------------------------------------------

    export interface Setting extends Record<string, unknown> {
        rton: {
            encode_rton: boolean;
            encrypt_rton: boolean;
            key: string;
            iv: string;
        };
        ptx: {
            generic: Generic;
        };
        rsb: {
            distribution: {
                all: boolean;
                "1536": boolean;
                "768": boolean;
                "384": boolean;
                "640": boolean;
                "1200": boolean;
            };
        };
        resource_group: {
            encode_rton: boolean;
            encode_newton: boolean;
            automatic_merge_before_encode: boolean;
        };
        res_info: {
            convert: Questionable<boolean>;
            automatic_merge_before_encode: boolean;
        };
    }
}
