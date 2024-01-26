namespace Sen.Script.Support.PopCap.Atlas.Structure {
    /**
     * Method to split
     */

    export type TypicalMethod = `path` | `id`;

    /**
     * Path style
     */

    export type PathStyle = `string` | `array`;

    /**
     * Definition file: atlas.json
     */

    export interface Definition {
        method: TypicalMethod;
        expand_path: PathStyle;
        subgroup: string;
        trim: boolean;
        res: string;
        groups: Record<string, BasicGroup>;
    }

    /**
     * Basic group
     */

    export interface BasicGroup {
        default: {
            x: bigint;
            y: bigint;
            cols?: bigint;
            rows?: bigint;
        };
        path: string[];
    }
}
