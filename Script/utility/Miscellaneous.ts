namespace Sen.Script {
    /**
     * --------------------------------------------------
     * JavaScript custom formatter
     * @param str - string to format
     * @param args - arguments
     * @returns formatted string
     * --------------------------------------------------
     */

    export function format(str: string, ...args: Array<any>): string {
        for (const arg of args) {
            str = str.replace("{}", arg.toString());
        }
        return str;
    }

    /**
     *
     * @param value - any JS Value
     * @returns
     */

    export function is_boolean(value: unknown): value is boolean {
        return typeof value === "boolean";
    }

    /**
     *
     * @param value - any JS Value
     * @returns
     */

    export function is_bigint(value: unknown): value is bigint {
        return typeof value === "bigint";
    }

    /**
     *
     * @param value - any JS Value
     * @returns
     */

    export function is_number(value: unknown): value is number {
        return typeof value === "number";
    }

    /**
     *
     * @param value - any JS Value
     * @returns
     */

    export function is_string(value: unknown): value is string {
        return typeof value === "string";
    }

    /**
     *
     * @param value - any JS Value
     * @returns
     */

    export function is_object(value: unknown): value is object {
        return typeof value === "object";
    }

    /**
     *
     * @param value - any JS Value
     * @returns
     */

    export function is_actual_object(value: unknown): value is Record<any, any> {
        return is_object(value) && value.constructor.name === "Object";
    }

    /**
     *
     * @param value - any JS Value
     * @returns
     */

    export function is_array(value: unknown): value is Array<any> {
        return is_object(value) && Array.isArray(value);
    }

    /**
     *
     * @param value - Any JS Value
     * @returns
     */

    export function debug(value: unknown): void {
        if (is_object(value)) {
            Console.send(Kernel.JSON.serialize(value, 1, false));
            return;
        }
        Console.send(value);
        return;
    }

    /**
     *
     * @param condition - Conditional to assert
     * @param message - Message if assert failed
     * @returns
     */

    export function assert(condition: boolean, message?: string): void {
        if (!condition) {
            throw new Error(message);
        }
        return;
    }
}
