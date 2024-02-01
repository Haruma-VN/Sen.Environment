namespace Sen.Script.Support.PopCap.Animation {
    // sprite_document
    export interface SpriteDocument extends Kernel.XML.XMLDocument {
        DOMSymbolItem: {
            "@attributes": Attributes;
            timeline: {
                DOMTimeline: SpriteDomTimeline;
            };
        };
    }

    export interface Matrix {
        "@attributes": {
            a: string;
            b: string;
            c: string;
            d: string;
            tx: string;
            ty: string;
        };
    }

    export interface Color {
        "@attributes": {
            redMultiplier: string;
            greenMultiplier: string;
            blueMultiplier: string;
            alphaMultiplier: string;
        };
    }

    export interface DOMSymbolInstanceAttributes {
        libraryItemName: string;
        firstFrame?: string;
        symbolType: string;
        loop: string;
    }

    export interface DOMSymbolInstance {
        "@attributes": DOMSymbolInstanceAttributes;
        matrix: {
            Matrix: Matrix;
        };
        color?: {
            Color: Color;
        };
    }

    export type SpriteDomFrameProperty = SpriteDomFrame | SpriteDomFrame[];

    export interface SpriteDomFrame {
        "@attributes": {
            index: string;
            duration: string;
        };
        elements: {
            DOMSymbolInstance: DOMSymbolInstance;
        } | null;
    }

    export interface SpriteFrame {
        DOMFrame: SpriteDomFrameProperty;
    }

    export type SpriteFrameProperty = SpriteFrame | SpriteFrame[];

    export interface SpriteDomLayer {
        "@attributes": {
            name: string;
        };
        frames: SpriteFrameProperty;
    }

    export type DomLayerProperty = SpriteDomLayer | SpriteDomLayer[];

    export interface SpriteLayers {
        DOMLayer: DomLayerProperty;
    }

    export type SpriteLayersProperty = SpriteLayers | SpriteLayers[];

    export interface SpriteDomTimeline {
        "@attributes": {
            name: string;
        };
        layers: SpriteLayersProperty;
    }

    // image_document
    export interface ImageDocument extends Kernel.XML.XMLDocument {
        DOMSymbolItem: {
            "@attributes": Attributes;
            timeline: {
                DOMTimeline: DomTimeline;
            };
        };
    }

    export interface DOMBitmapInstance {
        "@attributes": {
            libraryItemName: string;
        };
        matrix: {
            Matrix: Matrix;
        };
    }

    export interface DomFrame {
        "@attributes": {
            index: string;
        };
        elements: {
            DOMBitmapInstance: DOMBitmapInstance;
        };
    }

    export interface DomLayer {
        frames: {
            DOMFrame: DomFrame;
        };
    }

    export interface DomTimeline {
        "@attributes": {
            name: string;
        };
        layers: {
            DOMLayer: DomLayer;
        };
    }

    export type xmlns_xsi = "http://www.w3.org/2001/XMLSchema-instance";

    export type xmlns = "http://ns.adobe.com/xfl/2008/";

    export interface Attributes {
        "xmlns:xsi": xmlns_xsi;
        xmlns: xmlns;
        name: string;
        symbolType: string;
    }

    // dom_document

    export interface DOMDocument extends Kernel.XML.XMLDocument {
        DOMDocument: {
            "@attributes": DocumentAttributes;
            folder: DomFolderItem;
            media: DomBitmapItem;
            symbols: {
                Include: AttributesSymbolsItem[];
            };
            timelines: {
                DOMTimeline: DocumentDOMTimeline;
            };
        };
    }

    export interface DocumentDOMTimeline {
        "@attributes": {
            name: string;
        };
        layers: {
            DOMLayer: DocumentDomLayer[];
        };
    }

    export interface DocumentDomLayer {
        "@attributes": {
            name: string;
        };
        frames: {
            DOMFrame: DocumentDomFrameProperty;
        };
    }

    export interface DocumentDomSymbolInstance {
        "@attributes": {
            libraryItemName: string;
            symbolType: string;
            loop: string;
        };
    }

    export type Actionscript = Record<string, string> | Record<string, string>[];

    export interface ActionscriptProperty {
        script: Actionscript;
    }

    export interface DocumentDomFrame {
        "@attributes": {
            index: string;
            duration?: string;
            name?: string;
            labelType?: string;
        };
        Actionscript?: ActionscriptProperty;
        elements?: {
            DOMSymbolInstance: DocumentDomSymbolInstance;
        } | null;
    }

    export type DocumentDomFrameProperty = DocumentDomFrame | DocumentDomFrame[];

    export type XflVersion = "2.971";

    export interface DocumentAttributes {
        "xmlns:xsi": xmlns_xsi;
        xmlns: xmlns;
        frameRate: string;
        width: string;
        height: string;
        xflVersion: XflVersion;
    }

    export interface AttributesFolderItem {
        "@attributes": {
            name: string;
            isExpanded: string;
        };
    }

    export interface DomFolderItem {
        DOMFolderItem: AttributesFolderItem[];
    }

    export interface AttributesBitmapItem {
        "@attributes": {
            name: string;
            href: string;
        };
    }

    export interface DomBitmapItem {
        DOMBitmapItem: AttributesBitmapItem[];
    }

    export interface AttributesSymbolsItem {
        "@attributes": {
            href: string;
        };
    }

    // extra_info
    export type ExtraInfo = Record<string, Sen.Script.Support.PopCap.Animation.Structure.AnimationSize>;

    export interface Model {
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

    export type FrameNodeList = Record<string, FrameNode[]>;

    export interface Action {
        start_index: bigint;
        duration: bigint;
        frame_index: bigint[];
    }

    export type ActionList = Record<string, Action>;
}
