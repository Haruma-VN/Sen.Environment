namespace Sen.Script.Executor {
    // The base need to be an object that is declared later

    export type Base = {
        [x: string]: unknown;
    };

    // Base Configuration need to be inherited

    export interface Configuration {
        [x: string]: unknown;
    }

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
            throw new Error(Sen.Script.Setting.format(Sen.Kernel.Language.get("js.method_not_found"), id));
        }
        worker.configuration = Sen.Kernel.JSON.deserialize_fs<Configuration>(worker.configuration_file);
        Sen.Script.Console.display(`${Sen.Kernel.Language.get(`method_loaded`)}`, Sen.Kernel.Language.get(id), Sen.Script.Definition.Console.Color.GREEN);
        switch (forward_type) {
            case Sen.Script.Executor.Forward.ASYNC: {
                if (worker.async_forward === undefined) {
                    throw new Error(Sen.Script.Setting.format(Sen.Kernel.Language.get(`method_does_not_support_async_implementation`), id));
                }
                worker.async_forward(argument);
                break;
            }
            case Sen.Script.Executor.Forward.BATCH: {
                if (worker.batch_forward === undefined) {
                    throw new Error(Sen.Script.Setting.format(Sen.Kernel.Language.get(`method_does_not_support_batch_implementation`), id));
                }
                worker.batch_forward(argument);
                break;
            }
            case Sen.Script.Executor.Forward.DIRECT: {
                worker.direct_forward(argument);
                break;
            }
            default: {
                throw new Error(Sen.Script.Setting.format(Sen.Kernel.Language.get(`js.method_does_not_execute`)));
            }
        }
        Sen.Script.Executor.clock.stop_safe();
        Sen.Script.Console.send(`${Sen.Kernel.Language.get(`execution_time`)}: ${Sen.Script.Executor.clock.duration.toFixed(3)}s`, Sen.Script.Definition.Console.Color.GREEN);
        return;
    }
}
