namespace Sen.Script.Support.PopCap.ResourceStreamBundle.Project.Configuration {
    // -----------------------------------------------------

    export type Generic = "android" | "ios" | "android-cn";

    // -----------------------------------------------------

    export type Questionable<T> = "?" | T;

    // -----------------------------------------------------

    export type Forward = "direct" | "batch";

    // -----------------------------------------------------

    export interface BasicCommand extends Record<string, unknown> {
        command: string;
        forward: Forward;
    }

    // -----------------------------------------------------

    export interface Setting extends Record<string, unknown> {
        rton: {
            key: string;
            iv: string;
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
        commands: Array<BasicCommand>;
    }
}
