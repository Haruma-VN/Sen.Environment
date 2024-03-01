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

        /**
         * Shell callback
         * @param argument - command and arguments
         */

        export function callback(argument: [string, ...string]): string;
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

        /**
         * Arguments passed to Kernel
         */

        declare const arguments: Array<string>;

        /**
         * Test method
         */

        export function test(...params: Array<string>): void;

        /**
         * JS XML Support
         */

        declare namespace XML {
            /**
             * All XML Document are designed to be Object after deserialize
             */

            export interface XMLDocument {
                [x: string]: unknown;
            }

            /**
             * --------------------------------------------------
             * JavaScript XML Deserialize Support
             * @param source - XML String to parse
             * @returns - XML Document
             * --------------------------------------------------
             */

            export function deserialize<T extends XMLDocument>(source: string): T;

            /**
             * --------------------------------------------------
             * JavaScript XML Deserialize file Support
             * @param source - source file
             * @returns - XML Document
             * --------------------------------------------------
             */

            export function deserialize_fs<T extends XMLDocument>(source: string): T;

            /**
             * --------------------------------------------------
             * JavaScript XML Serialize Support
             * @param source - source object
             * @returns - XML String
             * --------------------------------------------------
             */

            export function serialize<T extends XMLDocument>(source: T): string;

            /**
             * --------------------------------------------------
             * JavaScript XML Serialize File System Support
             * @param destination - destination file
             * @param xml - XML object
             * --------------------------------------------------
             */

            export function serialize_fs<T extends XMLDocument>(destination: string, xml: T): void;
        }

        declare namespace Language {
            export function load_language(source: string): void;

            export function get(key: string): string;
        }

        /**
         * JS Operating System
         */

        declare namespace OperatingSystem {
            /**
             * --------------------------------------------------
             * @returns: Get current OS
             * --------------------------------------------------
             */

            export function current(): string;
            /**
             * --------------------------------------------------
             * @returns: Get current Architecture
             * --------------------------------------------------
             */

            export function architecture(): string;
        }

        /**
         * JavaScript Array Buffer
         */

        declare namespace ArrayBuffer {
            /**
             * --------------------------------------------------
             * JavaScript read file as buffer
             * @param source - source file
             * @returns - ArrayBuffer of source file after read
             * --------------------------------------------------
             */

            export function open(source: string): ArrayBuffer;

            /**
             * --------------------------------------------------
             * JavaScript write file as buffer
             * @param destination - source file
             * @param data - Array Buffer to write
             * @returns - undefined
             * --------------------------------------------------
             */

            export function out(destination: string, data: ArrayBuffer): void;

            /**
             * Random bytes in ArrayBuffer
             * @param value - ArrayBuffer
             */

            export function random(value: ArrayBuffer): void;

            /**
             * Fill bytes in ArrayBuffer
             * @param array - ArrayBuffer
             * @param value - Value to fill
             */

            export function fill(array: ArrayBuffer, value: bigint): void;
        }

        /**
         * JavaScript call OS
         */

        declare namespace Process {
            /**
             * --------------------------------------------------
             * JavaScript run process method
             * @param command - command to execute
             * --------------------------------------------------
             */

            export function run(command: string): void;

            /**
             * --------------------------------------------------
             * JavaScript run process method
             * @param command - command to execute
             * @returns - string after execute command
             * --------------------------------------------------
             */

            export function execute(command: string): string;

            /**
             * --------------------------------------------------
             * JavaScript test environment
             * @param command - what to check in path env
             * @returns: true or false
             * --------------------------------------------------
             */

            export function is_exists_in_path_environment(command: string): boolean;
        }

        /**
         * JavaScript Image Support
         */

        declare namespace Image {
            /**
             * --------------------------------------------------
             * JavaScript Scale Image Method
             * @param source - source file
             * @param destination - destination file
             * @param percentage - percentage
             * @returns - Image after scale
             * --------------------------------------------------
             */

            export function scale_fs(source: string, destination: string, percentage: number): void;

            /**
             * --------------------------------------------------
             * JavaScript Resize Image Method
             * @param source - source file
             * @param destination - destination file
             * @param percentage - percentage
             * @returns - Image after resize
             * --------------------------------------------------
             */

            export function resize_fs(source: string, destination: string, percentage: number): void;

            /**
             * --------------------------------------------------
             * JavaScript Rotate Image Method
             * @param source - source file
             * @param destination - destination file
             * @param percentage - percentage
             * @returns - Image after rotate
             * --------------------------------------------------
             */

            export function rotate_fs(source: string, destination: string, percentage: number): void;

            /**
             * --------------------------------------------------
             * JavaScript Transparent Image Method
             * @param destination - destination file
             * @param width - width
             * @param height - height
             * @returns - Blank image
             * --------------------------------------------------
             */

            export function transparent_fs(destination: string, width: number, height: number): void;

            /**
             * --------------------------------------------------
             * JavaScript Rectangle Struct Image Method
             * --------------------------------------------------
             */

            export interface Rectangle {
                x: number;
                y: number;
                width: number;
                height: number;
            }

            /**
             * --------------------------------------------------
             * JavaScript Rectangle has IOBase
             * --------------------------------------------------
             */

            export interface RectangleFileIO extends Rectangle {
                destination: string;
            }
            /**
             * --------------------------------------------------
             * JavaScript Composite Image Method
             * @param source - source file
             * @param destination - destination file
             * @param rectangle - rectangle
             * --------------------------------------------------
             */

            export function composite_fs(source: string, destination: string, rectangle: Rectangle): void;

            /**
             * --------------------------------------------------
             * JavaScript Composite Images Method
             * @param source - source file
             * @param data - rectangle
             * --------------------------------------------------
             */

            export function composite_multiple_fs(source: string, data: Array<RectangleFileIO>): void;

            /**
             * --------------------------------------------------
             * JavaScript Composite Images Method ASYNC
             * @param source - source file
             * @param data - rectangle
             * --------------------------------------------------
             */

            export function composite_multiple_fs_asynchronous(source: string, data: Array<RectangleFileIO>): void;
            /**
             * --------------------------------------------------
             * JavaScript Open Image Method
             * @param source - source file
             * @returns - Struct
             * --------------------------------------------------
             */

            export function open(source: string): Sen.Kernel.Dimension.Image;

            /**
             * --------------------------------------------------
             * JavaScript Write Image Method
             * @param destination - destination file
             * @param image - image struct
             * @returns - after write image
             * --------------------------------------------------
             */

            export function write(source: string, image: Sen.Kernel.Dimension.Image): void;

            /**
             * --------------------------------------------------
             * JavaScript Join & Write Image Method
             * @param destination - Destination to write
             * @param dimension - Dimension
             * @param data - Data to write
             * --------------------------------------------------
             */

            export function join_png(destination: string, dimension: Kernel.Dimension.Structure, data: Array<Kernel.Dimension.Image>): void;

            /**
             * --------------------------------------------------
             * JavaScript Join Image Method
             * @param dimension - Dimension
             * @param data - Data to write
             * @returns image data after join
             * --------------------------------------------------
             */

            export function join(dimension: Kernel.Dimension.Structure, data: Array<Kernel.Dimension.Image>): Kernel.Dimension.Image;
        }

        /**
         * JavaScript Thread
         */

        declare namespace Thread {
            /**
             * --------------------------------------------------
             * JavaScript Thread sleep
             * @param time - time to sleep
             * @returns: undefined
             * --------------------------------------------------
             */

            export function sleep(time: bigint): void;

            /**
             * --------------------------------------------------
             * JavaScript time count
             * @returns: current timestamps
             * --------------------------------------------------
             */

            export function now(): number;
        }

        /**
         * NodeJS Path Implementation
         */

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

            /**
             * Format used for format
             */

            export interface Format {
                base: string;
                dir: string;
            }

            /**
             * --------------------------------------------------
             * JavaScript format
             * @param format - format
             * @returns: path
             * --------------------------------------------------
             */

            export function format(format: Format): string;

            /**
             * --------------------------------------------------
             * JavaScript normalize path
             * @param source - path
             * @returns: normalized path
             * --------------------------------------------------
             */

            export function normalize(source: string): string;

            /**
             * --------------------------------------------------
             * JavaScript resolve path
             * @param source - path
             * @returns: resolved path
             * --------------------------------------------------
             */

            export function resolve(source: string): string;

            /**
             * --------------------------------------------------
             * JavaScript extension name
             * @param source - path
             * @returns: extension name
             * --------------------------------------------------
             */

            export function extname(source: string): string;

            /**
             * --------------------------------------------------
             * JavaScript Is Absolute path
             * @param source - path
             * @returns: extension name
             * --------------------------------------------------
             */

            export function is_absolute(source: string): boolean;

            /**
             * --------------------------------------------------
             * JavaScript relative path
             * @param from - path
             * @param to - path
             * @returns: resolved path
             * --------------------------------------------------
             */

            export function relative(from: string, to: string): string;
            /**
             * --------------------------------------------------
             * JavaScript base without extension
             * @param source - path
             * @returns: basename without ext
             * --------------------------------------------------
             */

            export function base_without_extension(source: string): string;

            /**
             * --------------------------------------------------
             * JavaScript ignore extension
             * @param source - path
             * @returns: full path except extension
             * --------------------------------------------------
             */

            export function except_extension(source: string): string;
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

            export function print(title: string, message: string, color: Script.Definition.Console.Color): void;
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
             * @param destination - Destination file
             * @param obj - Object to serialize
             * @param indent - How many indentation
             * @param ensure_ascii - Ensure ASCII
             * --------------------------------------------------
             */

            export function serialize_fs<T>(destination: string, obj: T, indent: number, ensure_ascii: boolean): void;
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
             * This method will read the file
             * @param source - Source file to read
             * --------------------------------------------------
             */

            export function read_file_encode_with_utf16le(source: string): string;
            /**
             * --------------------------------------------------
             * This method will write the file
             * @param destination - Destination file to write
             * @param content - Write content
             * --------------------------------------------------
             */

            export function write_file(destination: string, content: string): void;
            /**
             * --------------------------------------------------
             * This method will write the file
             * @param destination - Destination file to write
             * @param content - Write content
             * --------------------------------------------------
             */

            export function write_file_encode_with_utf16le(destination: string, content: string): void;

            /**
             * --------------------------------------------------
             * This method will create directory
             * @param destination - Directory to create
             * @returns: created directory
             * --------------------------------------------------
             */

            export function create_directory(destination: string): void;

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
             * --------------------------------------------------
             * JavaScript read whole directory implementation
             * @param source - source directory
             * @returns full directory read (only file)
             * --------------------------------------------------
             */

            export function read_directory(source: string): Array<string>;

            /**
             * --------------------------------------------------
             * JavaScript read directory implementation
             * @param source - source directory
             * @returns  read current
             * --------------------------------------------------
             */
            export function read_current_directory(source: string): Array<string>;

            /**
             * --------------------------------------------------
             * JavaScript read directory implementation
             * @param source - source directory
             * @returns read current directory but only take file
             * --------------------------------------------------
             */
            export function read_directory_only_file(source: string): Array<string>;
            /**
             * --------------------------------------------------
             * JavaScript read directory implementation
             * @param source - source directory
             * @returns read current directory but only take dir
             * --------------------------------------------------
             */
            export function read_directory_only_directory(source: string): Array<string>;

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
             * JavaScript MD5 Hashing methods
             */

            declare namespace FNV {
                /**
                 * --------------------------------------------------
                 * JavaScript hash method
                 * @param str - string to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash(str: string): bigint;

                /**
                 * --------------------------------------------------
                 * JavaScript hash method for file
                 * @param source - source file to hash
                 * @returns: hashed string
                 * --------------------------------------------------
                 */

                export function hash_fs(source: string): bigint;
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

                /**
                 * --------------------------------------------------
                 * JavaScript Base64 decoding method as async
                 * @param argument - Argument to call
                 * @returns: async call
                 * --------------------------------------------------
                 */

                export function decode_fs_as_multiple_threads<source extends string, destination extends string>(argument: Array<[source, destination]>): void;

                /**
                 * --------------------------------------------------
                 * JavaScript Base64 encoding method as async
                 * @param argument - Argument to call
                 * @returns: async call
                 * --------------------------------------------------
                 */

                export function encode_fs_as_multiple_threads<source extends string, destination extends string>(argument: Array<[source, destination]>): void;
            }
        }

        /**
         * JavaScript Compression from Kernel
         */

        declare namespace Compression {
            /**
             * Zip Support from Kernel
             */

            declare namespace Zip {
                /**
                 * Zip compression from Kernel
                 */

                declare namespace Compress {
                    /**
                     * --------------------------------------------------
                     * JS Zip compression support
                     * @param source - source files
                     * @param destination - destination
                     * @param root - root escape character
                     * --------------------------------------------------
                     */

                    export function file(source: Array<string>, destination: string, root?: string): void;

                    /**
                     * --------------------------------------------------
                     * JS Zip compression support
                     * @param source - source files
                     * @param destination - destination
                     * --------------------------------------------------
                     */

                    export function directory(source: string, destination: string): void;
                }

                /**
                 * Zip Uncompression from Kernel
                 */

                declare namespace Uncompress {
                    /**
                     * --------------------------------------------------
                     * JS Zip uncompression support
                     * @param source - source file
                     * @param destination - destination
                     * --------------------------------------------------
                     */

                    export function process(source: string, destination: string): void;
                }
            }

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

                /**
                 *
                 * @param view - StreamView
                 * @returns Stream
                 */

                export function uncompress(data: BinaryView): BinaryView;

                /**
                 *
                 * @param view - StreamView
                 * @param level - level
                 * @returns Stream
                 */

                export function compress(data: BinaryView, level: Script.Definition.Zlib.Level): BinaryView;
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

                export function compress_fs(source: string, destination: string, level: Script.Zlib.Level): void;

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

            export const script: string;
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

                export function encode_fs(source: string, destination: string, format: Sen.Script.Support.Texture.Format): void;

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

                export function decode_fs(source: string, destination: string, width: bigint, height: bigint, format: Sen.Script.Support.Texture.Format): void;
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
                     * @param source - source file
                     * @param destination - destination file
                     * @returns - decoded file
                     * --------------------------------------------------
                     */
                    export function decode_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript RTON Decrypt method for file
                     * @param source - source file
                     * @param destination - destination file
                     * @param key - key
                     * @param iv - iv
                     * @returns - decoded file
                     * --------------------------------------------------
                     */
                    export function decrypt_fs(source: string, destination: string, key: string, iv: string): void;
                    /**
                     * --------------------------------------------------
                     * JavaScript RTON Encrypt method for file
                     * @param source - source file
                     * @param destination - destination file
                     * @param key - key
                     * @param iv - iv
                     * @returns - encrypted file
                     * --------------------------------------------------
                     */
                    export function encrypt_fs(source: string, destination: string, key: string, iv: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript RTON Decrypt method for file
                     * @param source - source file
                     * @param destination - destination file
                     * @param key - key
                     * @param iv - iv
                     * @returns - decoded file
                     * --------------------------------------------------
                     */
                    export function decrypt_and_decode_fs(source: string, destination: string, key: string, iv: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript RTON Decrypt method for file
                     * @param source - source file
                     * @param destination - destination file
                     * @param key - key
                     * @param iv - iv
                     * @returns - decoded file
                     * --------------------------------------------------
                     */
                    export function encode_and_encrypt_fs(source: string, destination: string, key: string, iv: string): void;
                    /**
                     * --------------------------------------------------
                     * JavaScript RTON Decode method as async
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function decode_fs_as_multiple_threads(...params: Array<[string, string]>): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript RTON Encode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: encoded file
                     * --------------------------------------------------
                     */
                    export function encode_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript RTON Encode method as async
                     * @returns: encoded file
                     * --------------------------------------------------
                     */
                    export function encode_fs_as_multiple_threads(...params: Array<[string, string]>): void;
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
                    export function uncompress_fs(source: string, destination: string, use_64_bit_variant: boolean): void;

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
                 * Render Effects Support
                 */

                declare namespace RenderEffects {
                    /**
                     * --------------------------------------------------
                     * JavaScript RenderEffects Decode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function decode_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript RenderEffects Encode method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: encoded file
                     * --------------------------------------------------
                     */
                    export function encode_fs(source: string, destination: string): void;
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
                 * Crypt-Data Support
                 */

                declare namespace CryptData {
                    /**
                     * --------------------------------------------------
                     * JavaScript Crypt-Data decrypt for file
                     * @param source - source file
                     * @param destination - destination file
                     * @param key - key
                     * @returns: decrypted file
                     * --------------------------------------------------
                     */
                    export function decrypt_fs(source: string, destination: string, key: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript Crypt-Data encrypt for file
                     * @param source - source file
                     * @param destination - destination file
                     * @param key - key
                     * @returns: encrypted file
                     * --------------------------------------------------
                     */
                    export function encrypt_fs(source: string, destination: string, key: string): void;
                }

                /**
                 * CharacterFontWidget2 Support
                 */

                declare namespace CharacterFontWidget2 {
                    /**
                     * --------------------------------------------------
                     * JavaScript CharacterFontWidget2 decode for file
                     * @param source - source file
                     * @param destination - destination file
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function decode_fs(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript CharacterFontWidget2 encode for file
                     * @param source - source file
                     * @param destination - destination file
                     * @returns: encoded file
                     * --------------------------------------------------
                     */
                    export function encode_fs(source: string, destination: string): void;
                }

                /**
                 * Particles Support
                 */

                declare namespace Particles {
                    /**
                     * --------------------------------------------------
                     * JavaScript Particles decode for file
                     * @param source - source file
                     * @param destination - destination file
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function decode_fs(source: string, destination: string): void;
                    /**
                     * --------------------------------------------------
                     * JavaScript Particles decode for file
                     * @param source - source file
                     * @param destination - destination file
                     * @returns: decoded file
                     * --------------------------------------------------
                     */
                    export function uncompress_and_encode(source: string, destination: string): void;

                    /**
                     * --------------------------------------------------
                     * JavaScript Particles encode for file
                     * @param source - source file
                     * @param destination - destination file
                     * @returns: encoded file
                     * --------------------------------------------------
                     */
                    export function encode_fs(source: string, destination: string): void;
                    /**
                     * --------------------------------------------------
                     * JavaScript Particles encode for file
                     * @param source - source file
                     * @param destination - destination file
                     * @returns: encoded file
                     * --------------------------------------------------
                     */
                    export function encode_and_compress_fs(source: string, destination: string): void;
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
                    export function decode_fs(source: string, destination: string, key: string, iv: string, use_64_bit_variant: boolean): void;

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
                    export function encode_fs(source: string, destination: string, key: string, iv: string, use_64_bit_variant: boolean): void;
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
                     * Compression flag
                     */

                    export type flag = 0n | 1n | 2n | 3n;

                    /**
                     * Res-Info
                     */

                    export interface ResourceStreamGroupResInfo extends Record<string, unknown> {
                        id: bigint;
                        format: bigint;
                        width: bigint;
                        height: bigint;
                        pitch: bigint;
                        alpha_size?: bigint;
                        alpha_format?: bigint;
                    }

                    export interface PTXInfo extends Record<string, unknown> {
                        path: string;
                        ptx_info?: ResourceStreamGroupResInfo;
                    }

                    /**
                     * RSG Packet info
                     */

                    export interface ResourceStreamGroupPacketInfo extends Record<string, unknown> {
                        compression_flags: flag;
                        res: Array<ResourceStreamGroupResInfo>;
                    }

                    /**
                     * Group-Info
                     */

                    export interface ResourceStreamGroupInfo extends Record<string, unknown> {
                        category: null | bigint;
                        packet_info: ResourceStreamGroupPacketInfo;
                    }

                    /**
                     * Group-Pool
                     */

                    export interface ResourceStreamGroupPool extends Record<string, unknown> {
                        is_composite: boolean;
                        subgroup: Record<string, ResourceStreamGroupInfo>;
                    }

                    /**
                     * Manifest
                     */

                    export interface Manifest extends Record<string, unknown> {
                        version: bigint;
                        group: Record<string, ResourceStreamGroupPool>;
                    }

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
                     * JavaScript RSB Unpack method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: unpacked file
                     * --------------------------------------------------
                     */
                    export function unpack_for_modding_fs(source: string, destination: string): Manifest;

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
                     * Version
                     */

                    export type Version = 3n | 4n;

                    /**
                     * Compression flag
                     */

                    export type CompressionFlags = 0n | 1n | 2n | 3n;

                    /**
                     * PTX Info
                     */

                    export type PTXInfo = {
                        id: bigint;
                        width: bigint;
                        height: bigint;
                    };

                    /**
                     * ResInfo
                     */

                    export type ResInfo = {
                        path: string;
                        ptx_info?: PTXInfo;
                    };

                    /**
                     * Definition file
                     */

                    export interface Definition {
                        version: Version;
                        compression_flags: CompressionFlags;
                        res: Array<ResInfo>;
                    }

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
                     * JavaScript RSG Unpack method for file
                     * @param source : source file
                     * @param destination : destination file
                     * @returns: unpacked file
                     * --------------------------------------------------
                     */
                    export function unpack_modding(source: string, destination: string): Definition;

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

                    export function convert_fs(source: string, destination: string, layout: Sen.Script.Support.PopCap.ResourceGroup): void;

                    /**
                     * Image Resource Group
                     */

                    export interface ResourceContainsImage {
                        slot: bigint;
                        id: string;
                        path: Array<string> | string;
                        type: "Image";
                    }

                    /**
                     * Atlas container
                     */

                    export interface ResourceContainsAtlas extends ResourceContainsImage {
                        atlas: boolean;
                        width: bigint;
                        height: bigint;
                    }

                    /**
                     * Sprite container
                     */

                    export interface ResourceContainsSprite extends ResourceContainsImage {
                        parent: string;
                        ax: bigint;
                        ay: bigint;
                        aw: bigint;
                        ah: bigint;
                        x?: bigint;
                        y?: bigint;
                        cols?: bigint;
                        rows?: bigint;
                    }

                    /**
                     * SubGroup that contains Image file
                     */

                    export interface ResourceSubgroup {
                        id: string;
                        type: "simple";
                        parent: string;
                        res: string;
                        resources: Array<ResourceContainsImage>;
                    }
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

                    export type Layout = "string" | "array";

                    export type Nullable<T> = null | T;

                    export interface Common {
                        type: unknown;
                    }

                    export interface Atlas extends Common {
                        type: string;
                        packet: Record<string, AtlasBinding>;
                    }

                    export interface AtlasBinding {
                        type: string;
                        path: string;
                        dimension: {
                            width: bigint;
                            height: bigint;
                        };
                        data: Record<string, Sprite>;
                    }

                    export interface Default {
                        ax: bigint;
                        ay: bigint;
                        aw: bigint;
                        ah: bigint;
                        x: bigint;
                        y: bigint;
                        cols?: bigint;
                        rows?: bigint;
                    }

                    export interface Sprite {
                        type: string;
                        path: string;
                        default: Default;
                    }

                    export interface Binary extends Common {
                        type: null;
                        packet: {
                            type: "File";
                            data: Record<string, CommonBinaryData>;
                        };
                    }

                    export interface CommonBinaryData {
                        type: string;
                        path: string;
                        forceOriginalVectorSymbolSize?: boolean;
                        srcpath?: string;
                    }

                    export interface CompositeShell {
                        is_composite: boolean;
                        subgroup: Record<string, Common>;
                    }

                    export interface Structure {
                        expand_path: Layout;
                        groups: Record<string, CompositeShell>;
                    }
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

        /**
         * Dimension Support
         */

        declare namespace Dimension {
            /**
             * Dimension struct
             */

            export interface Structure {
                /**
                 * Width of the image
                 */
                width: bigint;
                /**
                 * Height of the image
                 */
                height: bigint;
                /**
                 * Calculate the area
                 */
                area(): bigint;
                /**
                 * Calculate the circumference
                 */
                circumference(): bigint;
            }

            /**
             * Get the instance of the Structure
             * @param width - width provider
             * @param height - height provider
             */

            export function instance(width: bigint, height: bigint): Sen.Kernel.Dimension.Structure;

            /**
             * Dimension struct
             */

            export interface Image extends Structure {
                source_file?: string;
                bit_depth: bigint;
                color_type: bigint;
                interlace_type: bigint;
                channels: bigint;
                rowbytes: bigint;
                data: ArrayBuffer;
                [Symbol.iterator]: () => Iterator<this>;
            }
        }

        /**
         * Kernel Boolean Wrapper
         */

        declare class Boolean {
            /**
             * Value
             */

            private _value: boolean;

            /**
             * Getter
             */

            public get value(): boolean;

            /**
             * Setter
             */

            public set value(value: boolean): void;

            /**
             * JS Boolean Constructor
             * @param value - Boolean value
             */

            public constructor(value: boolean): void;

            /**
             * Get true instance
             */

            public static true(): Boolean;

            /**
             * Get false instance
             */

            public static false(): Boolean;
        }

        /**
         * Kernel String Wrapper
         */

        declare class String {
            /**
             * Value
             */

            private _value: string;

            /**
             * Getter
             */

            public get value(): string;

            /**
             * Setter
             */

            public set value(value: string): void;

            /**
             * JS String Constructor
             * @param value - String value
             */

            public constructor(value: string): void;

            /*
             * JS Get instance
             *
             */

            public instance(value: string): String;
        }

        /**
         * Size
         */

        declare class Size {
            /* Value */

            private _value: bigint;

            /**
             * Constructor
             * @param value - Value
             */

            public constructor(value: bigint): void;

            /**
             * Getter
             */

            public get value(): bigint;

            /**
             * Setter
             */

            public set value(value: bigint): void;

            /**
             * Get instance
             */

            public static instance(value: bigint): Size;
        }

        /**
         * Character
         */

        declare class Character {
            /* Value */

            private _value: bigint;

            /**
             * Constructor
             * @param value - Value
             */

            public constructor(value: bigint): void;

            /**
             * Getter
             */

            public get value(): bigint;

            /**
             * Setter
             */

            public set value(value: bigint): void;

            /**
             * Get instance
             */

            public static instance(value: bigint): Character;
        }

        /**
         * BinaryView
         */

        declare class BinaryView {
            /* Value */

            private _value: ArrayBuffer;

            /**
             * Constructor
             * @param value - Value
             */

            public constructor(value: ArrayBuffer): void;

            /**
             * Getter
             */

            public get value(): ArrayBuffer;

            /**
             * Setter
             */

            public set value(value: ArrayBuffer): void;

            /**
             * Get instance
             */

            public static instance(): BinaryView;

            /***
             * Get size
             */

            public size(): bigint;

            /***
             * Get capacity
             */

            public capacity(): bigint;

            /**
             * Make ArrayBuffer
             * @param from - from value
             * @param to - to value
             */

            public sub(from: bigint, to: bigint): ArrayBuffer;

            /**
             * Allocating memory
             * @param value - amount to allocate
             */

            public allocate(value: bigint): void;

            /**
             * convert BinaryView to DataStreamView
             */

            public stream_view(): DataStreamView;

            /**
             * convert BinaryView to DataStreamViewUseBigEndian
             */

            public big_stream_view(): DataStreamViewUseBigEndian;
        }

        /**
         * Kernel DataView
         */

        declare class DataStreamView {
            /**
             * Current write position
             */

            public write_position: bigint;

            /**
             * Current read position
             */

            public read_position: bigint;

            /**
             *
             * @param source - Source file to process or make a new DataView writer
             */

            public constructor(source?: string): void;

            /**
             * Get the size of current Stream
             */

            public size(): bigint;

            /**
             *
             * @param value - JS String value
             */

            public fromString(value: string): void;

            /**
             * Stream capacity
             */

            public capacity(): bigint;

            /**
             *
             * @param value - Value to increase
             */

            public reserve(value: bigint): void;

            /**
             * cast current DataView to JS Uint8Array
             */

            public toUint8Array(): Uint8Array;

            /**
             * cast current DataView to JS ArrayBuffer
             */

            public toArrayBuffer(): ArrayBuffer;

            /**
             * cast current DataView to JS Uint8Array
             */
            public getUint8Array(from: bigint, to: bigint): Uint8Array;

            /**
             * cast current DataView to JS ArrayBuffer
             */

            public getArrayBuffer(from: bigint, to: bigint): ArrayBuffer;

            /**
             * Convert current stream to string
             */

            public toString(): string;

            /**
             *
             * @param destination - output path
             */

            public out_file(destination: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeUint8(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeUint8(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeUint16(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeUint16(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeUint32(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeUint32(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeUint64(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeUint64(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeInt8(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeInt8(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeInt16(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeInt16(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeInt32(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeInt32(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeInt64(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeInt64(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeArrayBuffer(value: ArrayBuffer): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeUint8Array(value: Uint8Array): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeArrayBuffer(value: ArrayBuffer, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeUint8Array(value: Uint8Array, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeFloat(value: number): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeFloat(value: number, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeDouble(value: number): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeDouble(value: number, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeVarInt32(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeVarInt32(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeVarInt64(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeVarInt64(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeZigZag32(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeZigZag32(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeZigZag64(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeZigZag64(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeString(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeString(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringFourByte(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringFourByte(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeNull(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeNull(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeBoolean(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeBoolean(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByUint8(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByUint8(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByUint16(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByUint16(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByUint32(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByUint32(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByInt8(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByInt8(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByInt16(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByInt16(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByInt32(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByInt32(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByEmpty(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByEmpty(value: string, position: bigint): void;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readUint8(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readUint8(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readUint16(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readUint16(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readUint24(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readUint24(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readUint32(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readUint32(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readUint64(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readUint64(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readInt8(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readInt8(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readInt16(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readInt16(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readInt24(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readInt24(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readInt32(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readInt32(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readInt64(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readInt64(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readFloat(): number;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readFloat(position: bigint): number;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readDouble(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readDouble(position: bigint): number;

            /**
             * DataStreamView reader
             * @param size - Size to read
             * @returns Value after read
             */

            public readString(size: bigint): string;

            /**
             * DataStreamView reader
             * @param size - Size to read
             * @param position - Position to read
             * @returns Value after read
             */

            public readString(size: bigint, position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByUint8(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByUint8(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByUint16(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByUint16(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByUint24(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByUint24(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByUint32(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByUint32(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByUint64(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByUint64(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByInt8(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByInt8(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByInt16(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByInt16(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByInt24(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByInt24(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByInt32(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByInt32(position: bigint): string;
            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByVarInt32(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByVarInt32(position: bigint): string;
            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByEmpty(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByEmpty(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByInt64(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByInt64(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readVarInt32(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readVarInt32(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readVarInt64(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readVarInt64(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readVarUint32(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readVarUint32(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readVarUint64(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readVarUint64(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readZigZag32(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readZigZag32(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readZigZag64(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readZigZag64(position: bigint): bigint;

            /**
             * DataStreamView close
             * @returns Close current stream
             */

            public close(): void;
        }

        /**
         * DataStreamView but use big endian
         */

        declare class DataStreamViewUseBigEndian {
            /**
             * Current write position
             */

            public write_position: bigint;

            /**
             * Current read position
             */

            public read_position: bigint;

            /**
             *
             * @param source - Source file to process or make a new DataView writer
             */

            public constructor(source?: string): void;

            /**
             * Get the size of current Stream
             */

            public size(): bigint;

            /**
             *
             * @param value - JS String value
             */

            public fromString(value: string): void;

            /**
             * Stream capacity
             */

            public capacity(): bigint;

            /**
             *
             * @param value - Value to increase
             */

            public reserve(value: bigint): void;

            /**
             * cast current DataView to JS Uint8Array
             */

            public toUint8Array(): Uint8Array;

            /**
             * cast current DataView to JS ArrayBuffer
             */

            public toArrayBuffer(): ArrayBuffer;

            /**
             * cast current DataView to JS Uint8Array
             */
            public getUint8Array(from: bigint, to: bigint): Uint8Array;

            /**
             * cast current DataView to JS ArrayBuffer
             */

            public getArrayBuffer(from: bigint, to: bigint): ArrayBuffer;

            /**
             * Convert current stream to string
             */

            public toString(): string;

            /**
             *
             * @param destination - output path
             */

            public out_file(destination: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeUint8(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeUint8(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeUint16(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeUint16(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeUint32(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeUint32(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeUint64(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeUint64(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeInt8(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeInt8(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeInt16(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeInt16(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeInt32(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeInt32(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeInt64(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeInt64(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeArrayBuffer(value: ArrayBuffer): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeUint8Array(value: Uint8Array): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeArrayBuffer(value: ArrayBuffer, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeUint8Array(value: Uint8Array, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeFloat(value: number): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeFloat(value: number, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeDouble(value: number): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeDouble(value: number, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeVarInt32(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeVarInt32(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeVarInt64(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeVarInt64(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeZigZag32(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeZigZag32(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeZigZag64(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeZigZag64(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeString(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeString(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringFourByte(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringFourByte(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeNull(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeNull(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeBoolean(value: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeBoolean(value: bigint, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByUint8(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByUint8(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByUint16(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByUint16(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByUint32(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByUint32(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByInt8(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByInt8(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByInt16(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByInt16(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByInt32(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByInt32(value: string, position: bigint): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             */

            public writeStringByEmpty(value: string): void;

            /**
             * DataStreamView writer
             * @param value - Value to write
             * @param position - Position to write
             */

            public writeStringByEmpty(value: string, position: bigint): void;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readUint8(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readUint8(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readUint16(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readUint16(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readUint24(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readUint24(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readUint32(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readUint32(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readUint64(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readUint64(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readInt8(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readInt8(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readInt16(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readInt16(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readInt24(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readInt24(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readInt32(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readInt32(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readInt64(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readInt64(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readFloat(): number;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readFloat(position: bigint): number;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readDouble(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readDouble(position: bigint): number;

            /**
             * DataStreamView reader
             * @param size - Size to read
             * @returns Value after read
             */

            public readString(size: bigint): string;

            /**
             * DataStreamView reader
             * @param size - Size to read
             * @param position - Position to read
             * @returns Value after read
             */

            public readString(size: bigint, position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByUint8(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByUint8(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByUint16(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByUint16(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByUint24(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByUint24(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByUint32(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByUint32(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByUint64(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByUint64(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByInt8(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByInt8(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByInt16(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByInt16(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByInt24(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByInt24(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByInt32(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByInt32(position: bigint): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByVarInt32(position: bigint): string;
            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByEmpty(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByEmpty(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readStringByInt64(): string;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readStringByInt64(position: bigint): string;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readVarInt32(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readVarInt32(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readVarInt64(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readVarInt64(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readVarUint32(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readVarUint32(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readVarUint64(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readVarUint64(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readZigZag32(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readZigZag32(position: bigint): bigint;

            /**
             * DataStreamView reader
             * @returns Value after read
             */

            public readZigZag64(): bigint;

            /**
             * DataStreamView reader
             * @param position - Position to read
             * @returns Value after read
             */

            public readZigZag64(position: bigint): bigint;

            /**
             * DataStreamView close
             * @returns Close current stream
             */

            public close(): void;
        }

        /**
         * Miscellaneous method
         */

        declare namespace Miscellaneous {
            /**
             * JS Deep clone
             * @param object - Any JS Value, but should be JS Object
             */

            export function make_copy<T extends any>(object: T): T;

            /**
             * UTF16 Support
             * @param value - ArrayBuffer
             * @returns JS String
             */

            export function cast_ArrayBuffer_to_JS_WideString(value: ArrayBuffer): string;

            /**
             * UTF8 Support
             * @param value - ArrayBuffer
             * @returns JS String
             */

            export function cast_ArrayBuffer_to_JS_String(value: ArrayBuffer): string;

            /**
             *
             * @param value - JS String
             */

            export function cast_movable_String_to_ArrayBuffer(value: string): ArrayBuffer;
        }
    }
}
