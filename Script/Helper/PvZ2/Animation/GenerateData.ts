namespace Sen.Script.Support.PopCap.Animation.Miscellaenous.GenerateData {
    export interface Data {
        version: bigint;
        resolution: bigint;
        position: Position;
        image: Image;
        sprite: Sprite;
    }

    export interface Position {
        x: bigint;
        y: bigint;
    }

    export type Image = Record<string, ImageInfo>;

    export interface ImageInfo {
        id: string;
        size: Size;
    }

    export interface Size {
        width: bigint;
        height: bigint;
    }

    export type Sprite = Record<string, Record<string, string>>;

    export function process(resolution: bigint, source: string, image_posix: string): Data {
        const document = {} as Kernel.Support.PopCap.Animation.Miscellaneous.Document;
        Kernel.Support.PopCap.Animation.Miscellaneous.dump_document(source, document);
        const result: Data = {
            version: 6n,
            position: {
                x: 0n,
                y: 0n,
            },
            resolution,
            image: {},
            sprite: {},
        };
        document.image.forEach((e) => {
            const image_name = e.replace(/image\//, "").replace(/\.xml/, "");
            const image = Kernel.Image.open(Kernel.Path.normalize(`${source}/library/media/${image_name}.png`));
            const distance: number = 1200 / Number(resolution);
            result.image[image_name] = {
                id: `${image_posix}_${image_name.toUpperCase()}`,
                size: {
                    width: BigInt(Math.round(Number(image.width) * distance)),
                    height: BigInt(Math.round(Number(image.height) * distance)),
                },
            };
        });
        return result;
    }

    export function process_fs(source: string, destination: string, resolution: bigint, image_posix: string): void {
        const data = process(resolution, source, image_posix);
        Kernel.JSON.serialize_fs(destination, data, 1, false);
    }

    export function execute(): void {
        let resolution = 1536n;
        const source = Console.path("input source", "directory");
        Console.argument("input image posix for id");
        const image_posix = Kernel.Console.readline();
        process_fs(source, `${source}/data.generated.json`, resolution, image_posix);
    }
}
Sen.Script.Support.PopCap.Animation.Miscellaenous.GenerateData.execute();
