declare namespace Sen {
    /**
     * Kernel methods are exported here
     */

    declare namespace Kernel {
        /**
         * Console methods from Kernel export to Script through Callback interactive with Shell
         * Shell methods must implementing this, Script only need to call
         */

        declare namespace Console {
            /**
             * --------------------------------------------------
             * This method print the message to the debug console
             * @param title - The title of what you want to print
             * @param message - Message what you want to print
             * @param color - Color message
             * --------------------------------------------------
             */

            export function print(title: string, message: string, color: Script.Color): void;
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

        declare namespace Home {
            export const script_parent: string;
        }
    }
}
