namespace Sen.Script.Support.PopCap.Animation.Miscellaenous.AddLibrary {
    export type Data = GenerateData.Data;

    export type Sprite = GenerateData.Sprite;

    export type Additional = {
        media: string;
        image: string;
        sprite?: string;
        original_path: string;
    };

    export function check_duplicate() {}

    export function process(source: string, image_posix: string, additional: Array<Additional>): void {
        const document = {} as Kernel.Support.PopCap.Animation.Miscellaneous.Document;
        const data_path: string = `${source}/data.json`;
        Kernel.Support.PopCap.Animation.Miscellaneous.dump_document(source, document);
        const data: Data = Kernel.JSON.deserialize_fs(data_path);
        let base_transform: Kernel.Support.PopCap.Animation.Miscellaneous.Matrix;
        const additional_data: Kernel.Support.PopCap.Animation.Miscellaneous.Document = {
            image: [],
            action: [],
            media: [],
            sprite: [],
        };
        const resolution = data.resolution;
        switch (resolution) {
            case 768n:
                base_transform = [1.28125, 0.0, 0.0, 1.28125, -2.75, -2.75];
                break;
            case 1536n:
                base_transform = [0.78125, 0.0, 0.0, 0.78125, -2.75, -2.75];
                break;
            default:
                throw new Error(`resolution ${resolution} not found`);
        }
        const multipler = 1200 / Number(data.resolution);
        for (const { media, image, sprite, original_path } of additional) {
            Kernel.FileSystem.Operation.copy(original_path, `${source}/library/media/${media}.png`);
            Kernel.Support.PopCap.Animation.Miscellaneous.generate_image(`${source}/library/image/${image}.xml`, new Kernel.Support.PopCap.Animation.Miscellaneous.Image(media, media, base_transform));
            additional_data.media.push(media);
            additional_data.image.push(image);
            const img = Kernel.Image.open(original_path);
            data.image[image] = {
                id: `${image_posix}_${image}`.toUpperCase(),
                dimension: {
                    width: BigInt(Math.ceil(multipler * Number(img.width))),
                    height: BigInt(Math.ceil(multipler * Number(img.height))),
                },
            };
            if (sprite !== undefined) {
                Kernel.Support.PopCap.Animation.Miscellaneous.generate_sprite(
                    `${source}/library/sprite/${sprite}.xml`,
                    new Kernel.Support.PopCap.Animation.Miscellaneous.Sprite(sprite, `image/${image}`, base_transform, [1.0, 1.0, 1.0, 1.0]),
                );
                additional_data.sprite.push(sprite);
            }
        }
        Kernel.JSON.serialize_fs(data_path, data, 1, false);
        Kernel.Support.PopCap.Animation.Miscellaneous.generate_document(`${source}/DOMDocument.xml`, additional_data);
        return;
    }

    export function process_fs(source: string, image_posix: string, additional: Array<Additional>): void {
        return process(source, image_posix, additional);
    }
}
