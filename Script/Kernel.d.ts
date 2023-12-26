// Declaration of Kernel in JavaScript
// Kernel version: 1
declare namespace Sen {
    /**
     * Shell interactive
     */

    declare namespace Shell {
        /**
         * Shell version
         */

        export const version: number;

        /**
         * Current Shell is gui or console
         */

        export const is_gui: boolean;
    }

    /**
     * Kernel methods are exported here
     */

    declare namespace Kernel {
        /**
         * --------------------------------------------------
         * Kernel version
         * --------------------------------------------------
         */

        declare const version: number;

        declare namespace Path {
            /**
             * --------------------------------------------------
             * JavaScript join path
             * @param args - Paths to join
             * @returns: New path after join
             * --------------------------------------------------
             */

            export function join(...args: Array<string>): string;

            /**
             * --------------------------------------------------
             * JavaScript basename
             * Example: "/Users/Refsnes/demo_path.js" ->
             * "demo_path.js"
             * @param source - Path
             * @returns: File name
             *
             * --------------------------------------------------
             */

            export function basename(source: string): string;

            /**
             * --------------------------------------------------
             * JavaScript delimiter
             * @returns: Delimiter of current OS
             *
             * --------------------------------------------------
             */

            export function delimiter(): string;

            /**
             * --------------------------------------------------
             * JavaScript dirname
             * Example: "/Users/Refsnes/demo_path.js" ->
             * "/Users/Refsnes/"
             * @param source - Path
             * @returns: File name
             *
             * --------------------------------------------------
             */

            export function dirname(source: string): string;

            // todo
        }

        /**
         * Console methods from Kernel export to Script through Callback interactive with Shell
         * Shell methods must implementing this, Script only need to call
         */

        declare namespace Console {
            /**
             * --------------------------------------------------
             * This method print the message to the debug console
             * @param title - The title of what you want to print
             * @param color - Color message
             * --------------------------------------------------
             */

            export function print(title: string, color: Script.Definition.Console.Color): void;
            /**
             * --------------------------------------------------
             * This method print the message to the debug console
             * @param title - The title of what you want to print
             * --------------------------------------------------
             */

            export function print(title: string): void;

            /***
             * --------------------------------------------------
             * @returns: Input result
             * --------------------------------------------------
             */

            export function readline(): string;
        }

        /**
         * Custom JSON Deserialize/Serialize
         */

        declare namespace JSON {
            /**
             * --------------------------------------------------
             * JavaScript JSON Deserializer
             * int -> bigint, float/double -> number
             * @param str - JSON String to deserialize
             * @returns: object of generic type
             * --------------------------------------------------
             */

            export function deserialize<T>(str: string): T;

            /**
             * --------------------------------------------------
             * JavaScript JSON File Deserializer
             * int -> bigint, float/double -> number
             * @param source - JSON path to deserialize
             * @returns: object of generic type
             * --------------------------------------------------
             */

            export function deserialize_fs<T>(source: string): T;

            /**
             * --------------------------------------------------
             * JavaScript JSON Serializer
             * @param obj - Object to serialize
             * @param indent - How many indentation
             * @param ensure_ascii - Ensure ASCII
             * --------------------------------------------------
             */

            export function serialize<T>(obj: T, indent: number, ensure_ascii: boolean): string;

            /**
             * --------------------------------------------------
             * JavaScript JSON File Serializer
             * @param desitnation - Destination file
             * @param obj - Object to serialize
             * @param indent - How many indentation
             * @param ensure_ascii - Ensure ASCII
             * --------------------------------------------------
             */

            export function serialize_fs<T>(desitnation: string, obj: T, indent: number, ensure_ascii: boolean): void;
        }

        /**
         * Kernel File System export to Script
         * Notice: Kernel file system are based on C++ Standard file system <fstream>
         * Kernel must implement these functions, Script only need to call
         */

        declare namespace FileSystem {
            /**
             * --------------------------------------------------
             * This method will read the file
             * @param source - Source file to read
             * --------------------------------------------------
             */

            export function read_file(source: string): string;
            /**
             * --------------------------------------------------
             * This method will write the file
             * @param destination - Destination file to write
             * @param content - Write content
             * --------------------------------------------------
             */

            export function write_file(destination: string, content: string): string;

            /**
             * --------------------------------------------------
             * This method will create directory
             * @param destination - Directory to create
             * @returns: created directory
             * --------------------------------------------------
             */

            export function create_directory(desitnation: string): void;

            /**
             * --------------------------------------------------
             * This method will check if a path is a file
             * @param source - path
             * @returns: boolean value
             * --------------------------------------------------
             */

            export function is_file(source: string): boolean;

            /**
             * --------------------------------------------------
             * This method will check if a path is a directory
             * @param source - path
             * @returns: boolean value
             * --------------------------------------------------
             */

            export function is_directory(source: string): boolean;

            /**
             * JS Operation for File System
             */

            declare namespace Operation {
                /**
                 * --------------------------------------------------
                 * This method will rename a file
                 * @param source - path
                 * @param destination - path
                 * --------------------------------------------------
                 */

                export function rename(source: string, destination: string): void;

                /**
                 * --------------------------------------------------
                 * This method will copy a file
                 * @param source - path
                 * @param destination - path
                 * --------------------------------------------------
                 */

                export function copy(source: string, destination: string): void;

                /**
                 * --------------------------------------------------
                 * This method will copy a file
                 * @param source - path
                 * --------------------------------------------------
                 */

                export function remove(source: string): void;
            }
        }

        /**
         * JavaScript Native Handler from Kernel
         */

        declare namespace JavaScript {
            /**
             * --------------------------------------------------
             * @param code: code to execute
             * @returns JS Value after evaluate
             * --------------------------------------------------
             */

            export function evaluate<T>(code: string): T;

            /**
             * --------------------------------------------------
             * @param source : source file to execute
             * @returns JS Value after evaluate
             * --------------------------------------------------
             */

            export function evaluate_fs<T>(source: string): T;
        }

        /**
         * JavaScript Kernel interactive : Encryption
         */

        declare namespace Encryption {
            /**
             * JavaScript MD5 Hashing methods
             */

            declare namespace MD5 {
                /**
                 * --------------------------------------------------
                 * JavaScript hash method
                 * @param str - string to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash(str: string): string;

                /**
                 * --------------------------------------------------
                 * JavaScript hash method for file
                 * @param source - source file to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash_fs(source: string): string;
            }

            /**
             * JavaScript SHA224 Hashing methods
             */

            declare namespace SHA224 {
                /**
                 * --------------------------------------------------
                 * JavaScript hash method
                 * @param str - string to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash(str: string): string;

                /**
                 * --------------------------------------------------
                 * JavaScript hash method for file
                 * @param source - source file to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash_fs(source: string): string;
            }

            /**
             * JavaScript SHA256 Hashing methods
             */

            declare namespace SHA256 {
                /**
                 * --------------------------------------------------
                 * JavaScript hash method
                 * @param str - string to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash(str: string): string;

                /**
                 * --------------------------------------------------
                 * JavaScript hash method for file
                 * @param source - source file to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash_fs(source: string): string;
            }

            /**
             * JavaScript SHA384 Hashing methods
             */

            declare namespace SHA384 {
                /**
                 * --------------------------------------------------
                 * JavaScript hash method
                 * @param str - string to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash(str: string): string;

                /**
                 * --------------------------------------------------
                 * JavaScript hash method for file
                 * @param source - source file to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash_fs(source: string): string;
            }

            /**
             * JavaScript SHA512 Hashing methods
             */

            declare namespace SHA512 {
                /**
                 * --------------------------------------------------
                 * JavaScript hash method
                 * @param str - string to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash(str: string): string;

                /**
                 * --------------------------------------------------
                 * JavaScript hash method for file
                 * @param source - source file to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash_fs(source: string): string;
            }

            /**
             * JavaScript XOR Hashing methods
             */

            declare namespace XOR {
                /**
                 * --------------------------------------------------
                 * JavaScript hash method
                 * @param plain - string to encrypt
                 * @param key - key to encrypt
                 * @returns: encrypted string
                 * --------------------------------------------------
                 */

                export function encrypt(str: string, key: string): string;

                /**
                 * --------------------------------------------------
                 * JavaScript encrypt method for file
                 * @param source - source file to encrypt
                 * @param destination - output file
                 * @param key - key to encrypt
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function encrypt_fs(source: string, destination: string, key: string): string;
            }

            /**
             * JavaScript Base64 encode/decode methods
             */

            declare namespace Base64 {
                /**
                 * --------------------------------------------------
                 * JavaScript Base64 encoding method
                 * @param str - string to encode
                 * @returns: encoded string
                 * --------------------------------------------------
                 */

                export function encode(str: string): string;

                /**
                 * --------------------------------------------------
                 * JavaScript Base64 decoding method
                 * @param str - string to encode
                 * @returns: decoded string
                 * --------------------------------------------------
                 */

                export function decode(str: string): string;

                /**
                 * --------------------------------------------------
                 * JavaScript Base64 encoding method
                 * @param source - source file
                 * @param destination - destination file
                 * @returns: file has been encoded with base64
                 * --------------------------------------------------
                 */

                export function encode_fs(source: string, destination: string): void;

                /**
                 * --------------------------------------------------
                 * JavaScript Base64 decodng method
                 * @param source - source file
                 * @param destination - destination file
                 * @returns: file has been decoded with base64
                 * --------------------------------------------------
                 */

                export function decode_fs(source: string, destination: string): void;
            }
        }

        /**
         * JavaScript Compression from Kernel
         */

        declare namespace Compression {
            /**
             * ZLib compression from Kernel
             */

            declare namespace Zlib {
                /**
                 * --------------------------------------------------
                 * JavaScript Zlib compression method for file
                 * @param source : source file
                 * @param destination : destination file
                 * @param level : level of compression
                 * @returns: compressed file
                 * --------------------------------------------------
                 */

                export function compress_fs(source: string, destination: string, level: Script.Zlib.Level): void;

                /**
                 * --------------------------------------------------
                 * JavaScript Zlib uncompression method for file
                 * @param source : source file
                 * @param destination : destination file
                 * @returns: compressed file
                 * --------------------------------------------------
                 */

                export function uncompress_fs(source: string, destination: string): void;
            }

            /**
             * Gzip compression from Kernel
             */

            declare namespace Gzip {
                /**
                 * --------------------------------------------------
                 * JavaScript Gzip compression method for file
                 * @param source : source file
                 * @param destination : destination file
                 * @returns: compressed file
                 * --------------------------------------------------
                 */

                export function compress_fs(source: string, destination: string): void;

                /**
                 * --------------------------------------------------
                 * JavaScript Gzip uncompression method for file
                 * @param source : source file
                 * @param destination : destination file
                 * @returns: compressed file
                 * --------------------------------------------------
                 */

                export function uncompress_fs(source: string, destination: string): void;
            }

            /**
             * Bzip2 compression from Kernel
             */

            declare namespace Bzip2 {
                /**
                 * --------------------------------------------------
                 * JavaScript Bzip2 compression method for file
                 * @param source : source file
                 * @param destination : destination file
                 * @returns: compressed file
                 * --------------------------------------------------
                 */

                export function compress_fs(source: string, destination: string): void;

                /**
                 * --------------------------------------------------
                 * JavaScript Bzip2 uncompression method for file
                 * @param source : source file
                 * @param destination : destination file
                 * @returns: compressed file
                 * --------------------------------------------------
                 */

                export function uncompress_fs(source: string, destination: string): void;
            }
        }

        /**
         * JavaScript home declaration
         */

        declare namespace Home {
            /**
             * Parent directories of script
             */

            export const script_parent: string;
        }

        /**
         * Kernel support declaration
         */

        declare namespace Support {
            /**
             * Texture Support
             */

            declare namespace Texture {
                /**
                 * --------------------------------------------------
                 * JavaScript Texture Encode Support
                 * @param source - Source file
                 * @param destination - Destination file
                 * @param format - Format
                 * @returns: Encoded file
                 * --------------------------------------------------
                 */

                export function encode_fs(
                    source: string,
                    destination: string,
                    format: Sen.Script.Support.Texture.Format,
                ): void;

                /**
                 * --------------------------------------------------
                 * JavaScript Texture Decode Support
                 * @param source - Source file
                 * @param destination - Destination file
                 * @param width - width
                 * @param height - height
                 * @param format - Format
                 * @returns: Decoded file
                 * --------------------------------------------------
                 */

                export function decode_fs(
                    source: string,
                    destination: string,
                    width: bigint,
                    height: bigint,
                    format: Sen.Script.Support.Texture.Format,
                ): void;
            }

            /**
             * PopCap Support declaration
             */

            declare namespace PopCap {
                /**
                 * Reflectation Object Notaion Support
                 */

                declare namespace RTON {
                    /**
                     * --------------------------------------------------
                     * JavaScript RTON Decode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function decode_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript RTON Encode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: encoded file
                     * --------------------------------------------------
                     */
                    export function encode_fs(source: string, destination: string): void;
                }

                /**
                 * Zlib Support
                 */

                declare namespace Zlib {
                    /**
                     * --------------------------------------------------
                     * JavaScript Zlib Uncompress method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @param use_64_bit_variant : use the 64bit variant
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function uncompress_fs(
                        source: string,
                        destination: string,
                        use_64_bit_variant: boolean,
                    ): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript Zlib Compress method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @param use_64_bit_variant : use the 64bit variant
                     * @returns: encoded file
                     * --------------------------------------------------
                     */
                    export function compress_fs(source: string, destination: string, use_64_bit_variant: boolean): void;
                }

                /**
                 * New Type Object Notaion Support
                 */

                declare namespace Newton {
                    /**
                     * --------------------------------------------------
                     * JavaScript Newton Decode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function decode_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript Newton Encode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: encoded file
                     * --------------------------------------------------
                     */
                    export function encode_fs(source: string, destination: string): void;
                }

                /**
                 * Compiled Text
                 */

                declare namespace CompiledText {
                    /**
                     * --------------------------------------------------
                     * JavaScript Compiled Text Decode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @param key : key
                     * @param iv : iv
                     * @param use_64_bit_variant :use the 64 bit variant
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function decode_fs(
                        source: string,
                        destination: string,
                        key: string,
                        iv: string,
                        use_64_bit_variant: boolean,
                    ): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript Compiled Text Encode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @param key : key
                     * @param iv : iv
                     * @param use_64_bit_variant :use the 64 bit variant
                     * @returns: encoded file
                     * --------------------------------------------------
                     */
                    export function encode_fs(
                        source: string,
                        destination: string,
                        key: string,
                        iv: string,
                        use_64_bit_variant: boolean,
                    ): void;
                }

                /**
                 * PopCap Animation decode method
                 */

                declare namespace Animation {
                    /**
                     * --------------------------------------------------
                     * JavaScript PAM Decode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function decode_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript PAM Encode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: encoded file
                     * --------------------------------------------------
                     */
                    export function encode_fs(source: string, destination: string): void;
                }

                /**
                 * PopCap Resource Stream Bundle Support
                 */

                declare namespace RSB {
                    /**
                     * --------------------------------------------------
                     * JavaScript RSB Unpack method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: unpacked file
                     * --------------------------------------------------
                     */
                    export function unpack_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript RSB Pack method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: packed file
                     * --------------------------------------------------
                     */
                    export function pack_fs(source: string, destination: string): void;
                }

                /**
                 * PopCap Resource Stream Group Support
                 */

                declare namespace RSG {
                    /**
                     * --------------------------------------------------
                     * JavaScript RSG Unpack method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: unpacked file
                     * --------------------------------------------------
                     */
                    export function unpack_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript RSG Pack method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: packed file
                     * --------------------------------------------------
                     */
                    export function pack_fs(source: string, destination: string): void;
                }

                /**
                 * Resource Group Support
                 */

                declare namespace ResourceGroup {
                    /**
                     * --------------------------------------------------
                     * JavaScript Split Resource Group Support
                     * @param source - Source file
                     * @param destination - Destination file
                     * --------------------------------------------------
                     */

                    export function split_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript Merge Resource Group Support
                     * @param source - Source file
                     * @param destination - Destination file
                     * --------------------------------------------------
                     */

                    export function merge_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript Convert Resource Group Support
                     * @param source - Source file
                     * @param destination - Destination file
                     * @param layout - Destination file
                     * --------------------------------------------------
                     */

                    export function convert_fs(
                        source: string,
                        destination: string,
                        layout: Sen.Script.Support.PopCap.ResourceGroup,
                    ): void;
                }

                /**
                 * Res Info Support
                 */

                declare namespace ResInfo {
                    /**
                     * --------------------------------------------------
                     * JavaScript Split Res Info Support
                     * @param source - Source file
                     * @param destination - Destination file
                     * --------------------------------------------------
                     */

                    export function split_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript Merge Res Info Support
                     * @param source - Source file
                     * @param destination - Destination file
                     * --------------------------------------------------
                     */

                    export function merge_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript Convert Res Info Support
                     * @param source - Source file
                     * @param destination - Destination file
                     * @param layout - Destination file
                     * --------------------------------------------------
                     */

                    export function convert_fs(source: string, destination: string): void;
                }
            }

            /**
             * WWise Support
             */

            declare namespace WWise {
                /**
                 * WWise Sound Bank Support
                 */

                declare namespace SoundBank {
                    /**
                     * --------------------------------------------------
                     * JavaScript WWise Decode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function decode_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript WWise Decode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function encode_fs(source: string, destination: string): void;
                }
            }
        }
    }
}