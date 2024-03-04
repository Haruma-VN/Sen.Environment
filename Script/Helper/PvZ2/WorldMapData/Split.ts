namespace Sen.Script.Helper.PvZ2.WorldMapData.Split {
    /**
     * Bounding Rect
     */

    export interface MBoundingRect {
        mX: bigint;
        mY: bigint;
        mWidth: bigint;
        mHeight: bigint;
    }

    /**
     * Event List
     */

    export interface MEventList {
        m_position: MPosition;
        m_eventType: string;
        m_eventSubType: string;
        m_name: string;
        m_dataString: string;
        m_displayText: string;
        m_worldMapTutorialVisibleWhen: string;
        m_levelNodeType: string;
        m_eventId: string;
        m_parentEvent: string;
        m_unlockedFrom: string;
        m_completedNarrationID: string;
        m_isArtFlipped: string;
        m_visibleFrom: string;
        m_imageID?: bigint;
        m_scaleX?: bigint;
        m_scaleY?: bigint;
    }

    /**
     * Map Piece
     */

    export interface MMapPiece {
        m_position: MPosition;
        m_imageID: bigint;
        m_eventType: string;
        m_eventSubType: string;
        m_worldMapTutorialVisibleWhen: string;
        m_levelNodeType: string;
        m_drawLayer: bigint;
        m_scaleX?: bigint;
        m_scaleY?: bigint;
        m_displayText: string;
        m_parallaxLayer?: bigint;
        m_eventId: string;
        m_isArtFlipped: string;
        m_rotationAngle?: bigint;
    }

    /**
     * Position
     */

    export interface MPosition {
        x: bigint;
        y: bigint;
    }

    /**
     * Object data
     */

    export interface Objdata {
        m_mapPieces: Array<MMapPiece>;
        m_eventList: Array<MEventList>;
        m_worldName: string;
        m_creationTime: bigint;
        m_resGroupID: bigint;
        m_boundingRect: MBoundingRect;
        m_worldId: bigint;
        m_version: bigint;
    }

    /**
     * Object
     */

    export interface Object {
        uid: string;
        objclass: string;
        objdata: Objdata;
    }

    /**
     * World map data
     */

    export interface WorldMapData {
        version: bigint;
        objects: Array<Object>;
    }

    /**
     * Definition
     */

    export type Definition = {
        version: bigint;
        objects: Record<string, RecordData>;
    };

    export type RecordData = {
        m_worldId: bigint;
        m_resGroupID: bigint;
    };

    /**
     *
     * @param worldmap_data - world map data
     * @param output_directory - output directory
     * @returns splitted
     */

    export function process(worldmap_data: WorldMapData, destination: string): void {
        const definition: Definition = {
            version: 1n,
            objects: {},
        };
        if (!("objects" in worldmap_data)) {
            throw new Error(Kernel.Language.get("object_cannot_be_null_in_worldmap_data"));
        }
        const standard: Array<string> = Object.keys(worldmap_data.objects);
        const unique: Array<string> = [...new Set(standard)];
        if (standard.length > unique.length) {
            throw new Error(Kernel.Language.get("map_name_is_duplicated"));
        }
        Kernel.FileSystem.create_directory(`${destination}/worldmap`);
        for (const worldmap of worldmap_data.objects) {
            const map_name: string = worldmap.objdata.m_worldName;
            const record_data: RecordData = {
                m_worldId: worldmap.objdata.m_worldId,
                m_resGroupID: worldmap.objdata.m_resGroupID,
            };
            delete (worldmap as any).objdata.m_worldName;
            delete (worldmap as any).objdata.m_worldId;
            delete (worldmap as any).objdata.m_resGroupID;
            Kernel.JSON.serialize_fs<Object>(`${destination}/worldmap/${map_name}.json`, worldmap, 1, false);
            definition.objects[map_name] = record_data;
        }
        Kernel.JSON.serialize_fs(`${destination}/map_list.json`, definition, 1, false);
        return;
    }

    /**
     * Evaluate method
     * @returns
     */

    export function execute(): void {
        // to do
        const source: string = "";
        process(Kernel.JSON.deserialize_fs<WorldMapData>(source), `${Kernel.Path.except_extension(source)}.map_list`);
        return;
    }
}
Sen.Script.Helper.PvZ2.WorldMapData.Split.execute();
