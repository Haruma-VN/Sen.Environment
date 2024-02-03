namespace Sen.Script.Support.PopCap.Animation.Structure {
    export interface SexyAnimation {
        version: bigint;
        frame_rate: bigint;
        position: AnimationPosition;
        size: AnimationSize;
        image: Record<string, AnimationImage>;
        sprite: Record<string, AnimationSprite>;
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
        name: string;
        size: AnimationSize;
        transform: number[];
    }

    export interface AnimationSprite {
        description: string;
        work_area: AnimationWorkArea;
        frame: AnimationFrame[];
    }

    export interface AnimationWorkArea {
        index: bigint;
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
