namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Configuration {
    // -----------------------------------------------------

    export type Generic = "android" | "ios" | "android-cn";

    // -----------------------------------------------------

    export type Questionable<T> = "?" | T;

    // -----------------------------------------------------

    export interface Setting extends Record<string, unknown> {
        rton: {
            encode_rton: Questionable<boolean>;
            encrypt_rton: Questionable<boolean>;
            key: Questionable<string>;
            iv: Questionable<string>;
        };
        ptx: {
            generic: Generic;
        };
        rsg: {
            rewrite: Questionable<boolean>;
            pack_everything_still_exists: Questionable<boolean>;
        };
        rsb: {
            specialization: {
                all: Questionable<boolean>;
                "1536": Questionable<boolean>;
                "768": Questionable<boolean>;
                "384": Questionable<boolean>;
                "640": Questionable<boolean>;
                "1200": Questionable<boolean>;
            };
        };
        resource_group: {
            encode_rton: Questionable<boolean>;
            encode_newton: Questionable<boolean>;
            automatic_merge_before_encode: Questionable<boolean>;
        };
        res_info: {
            convert: Questionable<boolean>;
            automatic_merge_before_encode: Questionable<boolean>;
        };
    }
}
