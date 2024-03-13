namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Configuration {
    // -----------------------------------------------------

    export type Generic = "android" | "ios" | "android-cn";

    // -----------------------------------------------------

    export type Questionable<T> = "?" | T;

    // -----------------------------------------------------

    export type Forward = "async" | "direct" | "batch";

    // -----------------------------------------------------

    export interface ZCommand extends Record<string, unknown> {
        command: string;
        forward: Forward;
    }

    export interface Setting extends Record<string, unknown> {
        rton: {
            key: string;
            iv: string;
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
        commands: Array<ZCommand>;
    }
}
