declare namespace Sen {
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

            export function print(title: string, color: Script.Console.Color): void;
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
         * Kernel File System export to Script
         * Notice: Kernel file system are based on C++ Standard file system <fstream>
         * Kernel must implement these functions, Script only need to call
         */

        declare namespace FileSystem {
            /**
             * --------------------------------------------------
             * This method will read the file based on the path provided
             * @param source - Source file to read
             * --------------------------------------------------
             */

            export function read_file(source: string): string;
            /**
             * --------------------------------------------------
             * This method will write the file based on the path provided
             * @param destination - Destination file to write
             * @param content - Write content
             * --------------------------------------------------
             */

            export function write_file(destination: string, content: string): string;
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

        declare namespace Support {
            declare namespace PopCap {
                declare namespace RTON {
                    export function decode_fs(source: string, destination: string): void;
                }
            }
        }
    }
}
