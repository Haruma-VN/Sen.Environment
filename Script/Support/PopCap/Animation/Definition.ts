namespace Sen.Script.Support.PopCap.Animation {
    // extra_info
    export type ExtraInfo = {
        version: bigint;
        group: Record<string, ImageInfo>;
    };

    export type ImageInfo = {
        name: string,
        size: Sen.Script.Support.PopCap.Animation.AnimationSize
    }

    export interface Model {
        index?: bigint;
        state: boolean | null;
        resource: bigint;
        sprite: boolean;
        transform: number[];
        color: number[];
        frame_start: bigint;
        frame_duration: bigint;
    }

    export type SpriteModel = Record<string, Model>;

    export interface FrameNode {
        index: bigint;
        duration: bigint;
        resource: bigint | null;
        sprite: boolean;
        first_frame?: bigint;
        transform: number[];
        color: number[];
    }

    export interface ActionFrameNode {
        index: bigint;
        duration: bigint;
        use_transform: boolean;
        transform?: number[] | null;
        use_color: boolean;
        color?: number[] | null;
    }

    export type FrameNodeList = Record<string, FrameNode[]>;

    export interface Action {
        start_index: bigint;
        duration: bigint;
        frame_index: bigint[];
    }

    export type ActionList = Record<string, Action>;

    export interface Instance {
        resource: bigint;
        sprite: boolean;
    }

    // animation
    export interface SexyAnimation {
        version: bigint;
        frame_rate: bigint;
        position: AnimationPosition;
        size: AnimationSize;
        image: Array<AnimationImage>;
        sprite: Array<AnimationSprite>;
        main_sprite: AnimationSprite;
    }

    export interface AnimationPosition {
        x: bigint;
        y: bigint;
    }

    export interface AnimationSize {
        width: bigint;
        height: bigint;
    }

    export interface AnimationImage {
        path: string;
        id: string;
        size: AnimationSize;
        transform: number[];
    }

    export interface AnimationSprite {
        name: string,
        work_area: AnimationWorkArea;
        frame: AnimationFrame[];
    }

    export interface AnimationWorkArea {
        start: bigint;
        duration: bigint;
    }

    export interface AnimationFrame {
        label: string;
        stop: boolean;
        command: AnimationCommand[];
        remove: bigint[];
        append: AnimationAppend[];
        change: AnimationMove[];
    }

    export interface AnimationCommand {
        command: string;
        parameter: string;
    }

    export interface AnimationAppend {
        index: bigint;
        name: string;
        resource: bigint;
        sprite: boolean;
        additive: boolean;
        preload_frame: bigint;
        time_scale: bigint;
    }

    export interface AnimationMove {
        index: bigint;
        transform: number[];
        color: number[] | null;
        source_rectangle: number[] | null;
        sprite_frame_number: bigint;
    }
}
