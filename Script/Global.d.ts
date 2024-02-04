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

        /**
         * Check if current array is empty
         */
        empty(): boolean;

        toReversed(): Array<T>;
        toSorted(compareFn?: (a: T, b: T) => number): Array<T>;
        toSpliced(from: number, to: number, ...params: Array<T>): Array<T>;
        with(index: number, value: number): Array<T>;
    }
}
