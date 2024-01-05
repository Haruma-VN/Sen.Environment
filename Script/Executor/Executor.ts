namespace Sen.Script.Executor {
    // The base need to be an object that is declared later

    export type Base = {};

    // Method Executor should implement direct forward, batch forward and async forward

    export interface MethodExecutor<Argument extends Base, BatchArgument extends Base, AsyncArgument extends Base> {
        id: string;
        configuration_file: string;
        direct_forward: (argument: Argument) => void;
        batch_forward: (argument: BatchArgument) => void;
        async_forward?: (argument: AsyncArgument) => void;
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

    export const clock: Clock = new Clock();

    /**
     *
     * All methods are assigned here as key | value
     * Key: must be the id of the typical module
     * Value: the worker
     *
     */

    const methods: Map<string, MethodExecutor<Base, Base, Base>> = new Map();

    /**
     * ----------------------------------------------------------
     * JavaScript Implementation of Executor
     * @param worker - Here, we assign worker with the typical
     * object that has been declared
     * @returns - and so, the typical method will be assign
     * if the key is not found
     * ----------------------------------------------------------
     */

    export function push_as_module<Argument extends Base, BatchArgument extends Base, AsyncArgument extends Base>(worker: MethodExecutor<Argument, BatchArgument, AsyncArgument>): void {
        const primary_id: string = worker.id!;
        delete (worker as any).id;
        if (methods.get(primary_id) !== undefined) {
            throw new Error(`${primary_id} is already existed`);
        }
        methods.set(primary_id, worker as MethodExecutor<Base, Base, Base>);
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

    export function run_as_module<Argument extends Base>(id: string, argument: Argument, forward_type: Sen.Script.Executor.Forward): void {
        const worker: MethodExecutor<Base, Base, Base> | undefined = methods.get(id);
        if (worker === undefined) {
            throw new Error(`Method ${id} not found`);
        }
        Sen.Script.Console.send(Sen.Script.Setting.Language.format(Sen.Script.Setting.Language.get(`method_loaded`), Sen.Script.Setting.Language.get(id)!), Definition.Console.Color.GREEN);
        const current_data: Argument = Sen.Kernel.JSON.deserialize_fs<Argument>(worker.configuration_file);
        if (Object.keys(current_data).length !== 0) {
            argument = current_data;
        }
        switch (forward_type) {
            case Forward.ASYNC: {
                if (worker.async_forward === undefined) {
                    throw new Error(Setting.Language.format(Setting.Language.get(`method_does_not_support_async_implementation`), id));
                }
                worker.async_forward(argument);
                break;
            }
            case Forward.BATCH: {
                worker.batch_forward(argument);
                break;
            }
            case Forward.DIRECT: {
                worker.direct_forward(argument);
                break;
            }
            default: {
                throw new Error(Setting.Language.format(Setting.Language.get(`method_does_not_support_async_implementation`), forward_type));
            }
        }
        clock.stop_safe();
        Sen.Script.Console.send(`${Sen.Script.Setting.Language.get(`execution_time`)}: ${clock.duration.toFixed(3)}s`, Sen.Script.Definition.Console.Color.GREEN);
        return;
    }
}
