namespace Sen.Script.Executor.Methods.JS.Evaluate {
    /**
     * Argument for the current method
     */

    export interface Argument extends Sen.Script.Executor.Base {
        source: string;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Sen.Script.Executor.Base {
        directory: string;
    }

    /**
     * Async support
     */

    export interface AsyncArgument<source extends string, destination extends string> extends Sen.Script.Executor.Base {
        parameter: Array<string>;
    }

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Sen.Script.Executor.Configuration {}

    /**
     * Detail
     */

    export namespace Detail {
        /**
         * If an argument is input, will be stored here
         */

        export const argument: Array<string> = undefined!;
    }

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        Sen.Script.Executor.push_as_module<
            Sen.Script.Executor.Methods.JS.Evaluate.Argument,
            Sen.Script.Executor.Methods.JS.Evaluate.BatchArgument,
            Sen.Script.Executor.Methods.JS.Evaluate.AsyncArgument<string, string>,
            Sen.Script.Executor.Methods.JS.Evaluate.Configuration
        >({
            id: "js.evaluate",
            configuration_file: Home.query("~/Executor/Configuration/js.evaluate.json"),
            direct_forward(argument: Argument): void {
                is_valid_source(argument, false);
                Console.obtained(argument.source);
                clock.start_safe();
                const result: string | undefined = Kernel.JavaScript.evaluate_fs(argument.source) as unknown & string;
                Console.display(Kernel.Language.get("js.process.done"), result, Definition.Console.Color.GREEN);
                clock.stop_safe();
                return;
            },
            batch_forward(argument: BatchArgument): void {
                return basic_batch(this, argument, false);
            },
            is_enabled: true,
            configuration: undefined!,
            filter: ["file", /(.+)\.js$/i],
        });
        return;
    }
}

Sen.Script.Executor.Methods.JS.Evaluate.forward();
