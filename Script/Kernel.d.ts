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

        declare namespace JavaScript {
            export function evaluate(source: string): any;
            export function evaluate_fs(source: string): any;
        }
    }
}
