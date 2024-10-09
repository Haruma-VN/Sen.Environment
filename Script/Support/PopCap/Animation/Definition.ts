namespace Sen.Script.Support.PopCap.Animation {
    export type Transform = [number, number, number, number, number, number];

    export type Color = [number, number, number, number];

    export interface FrameNode {
        index: bigint;
        duration: bigint;
        resource: number;
        sprite: boolean;
        first_frame?: bigint;
        transform: Transform;
        color: Color;
    }

    export type FrameNodeList = Array<FrameNode>;

    export type FrameNodeStructure = Record<number, FrameNodeList>;

    export enum State {
        state_null,
        state_false,
        state_true
    }

    export interface Model {
        state: State;
        resource: number;
        sprite: boolean;
        transform: Transform;
        color: Color;
        frame_start: bigint;
        frame_duration: bigint;
        index?: bigint;
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

    export interface AnimationDimension {
        width: bigint;
        height: bigint;
    }

    export interface AnimationSize {
        width: number;
        height: number;
    }

    export interface AnimationImage {
        path: string;
        id: string;
        dimension: AnimationDimension;
        transform: number[];
    }

    export interface AnimationSprite {
        name: string;
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
        transform: Array<number>;
        color: Color | null;
        source_rectangle: Array<number> | null;
        sprite_frame_number: bigint;
    }
}
