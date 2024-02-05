namespace Sen.Script.Executor {
    // The base need to be an object that is declared later

    export type Base = {
        [x: string]: unknown;
    } & {
        source: unknown;
    };

    // Base Configuration need to be inherited

    export interface Configuration {
        [x: string]: unknown;
    }

    export type MethodType = "file" | "directory" | "any" | "files";

    // Method Executor should implement direct forward, batch forward and async forward

    export interface MethodExecutor<
        Argument extends Sen.Script.Executor.Base,
        BatchArgument extends Sen.Script.Executor.Base,
        AsyncArgument extends Sen.Script.Executor.Base,
        Configuration extends Sen.Script.Executor.Configuration,
    > {
        id: string;
        configuration_file: string;
        direct_forward: (argument: Argument) => void;
        batch_forward?: (argument: BatchArgument) => void;
        async_forward?: (argument: AsyncArgument) => void;
        is_enabled: boolean;
        configuration: Configuration;
        filter: [MethodType, RegExp] | [MethodType, ...Array<RegExp>];
    }

    /**
     * Forwarder typical type
     */

    export enum Forward {
        DIRECT,
        BATCH,
        ASYNC,
    }

    /**
     * Clock need to be initialized during the runtime.
     * Clock will calculate everything
     */

    export const clock: Sen.Script.Clock = new Clock();

    /**
     *
     * All methods are assigned here as key | value
     * Key: must be the id of the typical module
     * Value: the worker
     *
     */

    const methods: Map<string, Sen.Script.Executor.MethodExecutor<Sen.Script.Executor.Base, Sen.Script.Executor.Base, Sen.Script.Executor.Base, Sen.Script.Executor.Configuration>> = new Map();

    /**
     * ----------------------------------------------------------
     * JavaScript Implementation of Executor
     * @param worker - Here, we assign worker with the typical
     * object that has been declared
     * @returns - and so, the typical method will be assign
     * if the key is not found
     * ----------------------------------------------------------
     */

    export function push_as_module<
        Argument extends Sen.Script.Executor.Base,
        BatchArgument extends Sen.Script.Executor.Base,
        AsyncArgument extends Sen.Script.Executor.Base,
        Configuration extends Sen.Script.Executor.Configuration,
    >(worker: MethodExecutor<Argument, BatchArgument, AsyncArgument, Configuration>): void {
        const primary_id: string = worker.id!;
        delete (worker as any).id;
        if (methods.get(primary_id) !== undefined) {
            throw new Error(`${primary_id} is already existed`);
        }
        methods.set(primary_id, worker as MethodExecutor<Base, Base, Base, Configuration>);
        return;
    }

    /**
     * ----------------------------------------------------------
     * JavaScript Executor Implement
     * @param argument - Argument to query
     * @param key - Key
     * @param defined_value - If not, this val will assign to it
     * @returns
     * ----------------------------------------------------------
     */

    export function defined_or_default<Argument extends Sen.Script.Executor.Base, T>(argument: Argument, key: string, defined_value: T): void {
        if ((argument as any & Argument)[key] === undefined) {
            (argument as any & Argument)[key] = defined_value;
        }
        return;
    }

    /**
     * ----------------------------------------------------------
     * JavaScript Executor Implement
     * @param argument - Argument to query
     * @param key - Key
     * @param defined_value - If not, this val will assign to it
     * @returns
     * ----------------------------------------------------------
     */

    export function argument_load<Argument extends Sen.Script.Executor.Base, Configuration extends Sen.Script.Executor.Configuration>(
        argument: Argument,
        key: string,
        configuration: Configuration,
        rule: Array<bigint> | Array<[bigint, string, string]>,
        title: string,
    ): void {
        Sen.Script.Console.argument(title);
        if ((argument as any & Argument)[key] !== undefined) {
            Sen.Kernel.Console.print(`    ${(argument as any & Argument)[key]}`);
            return;
        }
        if ((configuration as any)[key] === "?") {
            return configurate_or_input(argument, key, rule as Array<[bigint, string, string]>);
        }
        if ((configuration as any)[key] !== "?") {
            Sen.Kernel.Console.print(`    ${(configuration as any)[key]}`);
            (argument as any & Argument)[key] = (configuration as any)[key];
        }
        return;
    }

    export function input_integer(rule: Array<bigint>): bigint {
        let input: string = undefined!;
        while (true) {
            input = Sen.Kernel.Console.readline();
            if (/^\d+$/.test(input) && (rule as Array<bigint>).includes(BigInt(input))) {
                break;
            }
            Console.error(Sen.Kernel.Language.get("js.invalid_input_value"));
        }
        return BigInt(input);
    }

    /**
     * ----------------------------------------------------------
     * JavaScript Executor Implement
     * @param argument - Argument to query
     * @param key - Key
     * @param defined_value - If not, this val will assign to it
     * @returns
     * ----------------------------------------------------------
     */

    export function configurate_or_input<Argument extends Sen.Script.Executor.Base, T>(argument: Argument, key: string, rule: Array<bigint> | Array<[bigint, string, string]>): void {
        if ((argument as any & Argument)[key] === undefined) {
            if (typeof rule[0] === "object") {
                const new_rule: Array<bigint> = [];
                rule.forEach((e: [bigint, string] & any) => {
                    Sen.Kernel.Console.print(`    ${e[0]}. ${e[2]}`);
                    new_rule.push(e[0]);
                });
                (argument as any)[key] = (rule as Array<[bigint, string, string]>)[Number(input_integer(new_rule) - 1n)][1];
                return;
            }
            if (typeof rule[0] === "string") {
                (argument as any)[key] = Sen.Kernel.Console.readline();
                return;
            }
            if (typeof rule[0] === "bigint") {
                (argument as any)[key] = input_integer(rule as Array<bigint>);
                return;
            }
        }
        return;
    }

    export function test([type, method]: [MethodType, RegExp], source: string): boolean {
        let is_valid: boolean = undefined!;
        switch (type) {
            case "file": {
                is_valid = Kernel.FileSystem.is_file(source);
                break;
            }
            case "directory": {
                is_valid = Kernel.FileSystem.is_directory(source);
                break;
            }
        }
        Console.send(`Regex: ${method}: After test: ${method.test(source)}`);
        return true;
    }

    export function test_array([type, ...method]: [MethodType, ...Array<RegExp>], source: Array<string>): boolean {
        let is_valid: boolean = true;
        switch (type) {
            case "file": {
                is_valid = source.every((e) => Kernel.FileSystem.is_file(e));
                break;
            }
            case "directory": {
                is_valid = source.every((e) => Kernel.FileSystem.is_directory(e));
                break;
            }
        }
        return is_valid && method.every((e) => source.some((i) => e.test(i)));
    }

    /**
     * ----------------------------------------------------------
     * JavaScript Implementation of Runner
     * @param id - Here, we call the id. If the id is assigned
     * as a method, the method will be called instantly
     * @param argument - Provide arguments
     * @param forward_type - Forwarder type
     * @returns - Launch if the method found.
     * If an error is thrown, it mean the id is not assigned
     * ----------------------------------------------------------
     */

    export function run_as_module<Argument extends Sen.Script.Executor.Base>(id: string, argument: Argument, forward_type: Sen.Script.Executor.Forward): void {
        const worker: Sen.Script.Executor.MethodExecutor<Sen.Script.Executor.Base, Sen.Script.Executor.Base, Sen.Script.Executor.Base, Sen.Script.Executor.Configuration> | undefined = methods.get(id);
        if (worker === undefined) {
            throw new Error(Sen.Script.format(Sen.Kernel.Language.get("js.method_not_found"), id));
        }
        worker.configuration = Sen.Kernel.JSON.deserialize_fs<Configuration>(worker.configuration_file);
        Sen.Script.Console.display(`${Sen.Kernel.Language.get("method_loaded")}`, Sen.Kernel.Language.get(id), Sen.Script.Definition.Console.Color.GREEN);
        switch (forward_type) {
            case Sen.Script.Executor.Forward.ASYNC: {
                if (worker.async_forward === undefined) {
                    throw new Error(Sen.Script.format(Sen.Kernel.Language.get("method_does_not_support_async_implementation"), id));
                }
                worker.async_forward(argument);
                break;
            }
            case Sen.Script.Executor.Forward.BATCH: {
                if (worker.batch_forward === undefined) {
                    throw new Error(Sen.Script.format(Sen.Kernel.Language.get("method_does_not_support_batch_implementation"), id));
                }
                worker.batch_forward(argument);
                break;
            }
            case Sen.Script.Executor.Forward.DIRECT: {
                worker.direct_forward(argument);
                break;
            }
            default: {
                throw new Error(Sen.Script.format(Sen.Kernel.Language.get("js.method_does_not_execute")));
            }
        }
        Sen.Script.Executor.clock.stop_safe();
        Sen.Script.Console.send(`${Sen.Kernel.Language.get("execution_time")}: ${Sen.Script.Executor.clock.duration.toFixed(3)}s`, Sen.Script.Definition.Console.Color.GREEN);
        return;
    }

    export function display_argument(argument: string | string[]): void {
        if (is_string(argument)) {
            Console.send(`${Sen.Kernel.Language.get("execution_argument")}:`, Definition.Console.Color.CYAN);
            Kernel.Console.print(`    ${argument}`);
        } else {
            Console.send(`${Sen.Kernel.Language.get("execution_argument")}:`, Definition.Console.Color.CYAN);
            argument.forEach((e) => Kernel.Console.print(`    ${e}`));
        }
        return;
    }

    export function execute<Argument extends Base>(argument: Argument, id: string, forward: Forward): void {
        try {
            run_as_module<Argument>(id, argument, forward);
        } catch (e: any) {
            Console.error(e);
        }
        return;
    }

    export function load_module<Argument extends Base>(argument: Argument): void {
        const modules: Map<bigint, string> = new Map<bigint, string>();
        Console.send(argument.source);
        const query = (
            callback: (([type, method]: [MethodType, RegExp], source: string) => boolean) | (([type, method]: [MethodType, ...Array<RegExp>], source: Array<string>) => boolean),
            filter: [MethodType, RegExp | Array<RegExp>],
            source: string | string[],
            method_name: string,
        ) => {
            if (callback(filter as [MethodType, RegExp], source as string & string[])) {
                modules.set(BigInt(modules.size) + 1n, method_name);
            }
            return;
        };
        methods.forEach((worker, method_name) => {
            if (is_string(argument.source)) {
                query(test, worker.filter as [MethodType, RegExp], argument.source, method_name);
            }
            if (is_array(argument.source)) {
                query(test_array, worker.filter as [MethodType, RegExp], argument.source, method_name);
            }
        });
        display_argument(argument.source as string | string[]);
        Console.send(`${Sen.Kernel.Language.get("execution_argument")}: ${Kernel.Language.get("js.input_an_method_to_start")}`, Definition.Console.Color.CYAN);
        modules.forEach((name: string, num: bigint) => {
            Kernel.Console.print(`    ${num}. ${Kernel.Language.get(name)}`);
        });
        const view: Array<bigint> = Array.from(modules.keys());
        switch (view.length) {
            case 0: {
                Console.error(Kernel.Language.get("js.argument_ignored"));
                break;
            }
            case 1: {
                execute<Argument>(argument, modules.get(view[0])!, Forward.DIRECT);
                break;
            }
            default: {
                const input_value: bigint = input_integer(view);
                execute<Argument>(argument, modules.get(input_value)!, Forward.DIRECT);
            }
        }
        return;
    }
}
