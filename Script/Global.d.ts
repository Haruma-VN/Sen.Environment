/**
 * Redefine JS function
 */

namespace globalThis {
    interface Array<T> {
        /**
         * This method is similar to length
         */
        size(): number;
        /**
         *
         * @param num - Index to get
         * @returns - If find, return the element otherwise return undefined
         */
        at(num: number): T | undefined;
    }
}
