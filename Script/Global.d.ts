/**
 * Redefine JS function
 */

namespace globalThis {
    /**
     * ES2023 : Adapt
     */

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

        /**
         * Reverse an array
         */

        toReversed(): Array<T>;

        /**
         *
         * @param compareFn - Callback
         */

        toSorted(compareFn?: (a: T, b: T) => number): Array<T>;

        /**
         *
         * @param from - Index
         * @param to - Index
         * @param params - Additional parameters
         */

        toSpliced(from: number, to: number, ...params: Array<T>): Array<T>;

        /**
         *
         * @param index - Index to change
         * @param value - Value to change
         */

        with(index: number, value: number): Array<T>;
    }

    /**
     * ES2023 : Adapt
     */

    interface ObjectConstructor {
        /**
         *
         * @param obj - Any
         * @param propKey - Property key
         */

        hasOwn(obj: {}, propKey: PropertyKey): boolean;
    }

    /**
     * ES2023 : Adapt
     */

    interface StringConstructor {
        /**
         *
         * @param num - Index to get
         * @returns - If find, return the element otherwise return undefined
         */
        at(num: number): T | undefined;
    }

    /**
     * ES2023 : Adapt
     */

    interface String {
        /**
         *
         * @param num - Index to get
         * @returns - If find, return the element otherwise return undefined
         */
        at(num: number): string;
    }
}
