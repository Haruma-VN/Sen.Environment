namespace Sen.Script.Executor.Methods.Data.MD5.Hash {
    /**
     * Argument for the current method
     */

    export interface Argument extends Base {
        source: string;
    }

    /**
     * Argument for batch method
     */

    export interface BatchArgument extends Base {
        directory: string;
    }

    /**
     * Configuration file if needed
     */

    export interface Configuration extends Base {}

    /**
     * ----------------------------------------------
     * JavaScript forward method, this method need
     * to be evaluated during script loading time
     * ----------------------------------------------
     */

    export function forward(): void {
        push_as_module<Argument, BatchArgument, Base>({
            id: `data.md5.hash`,
            configuration_file: Home.query(`~/Executor/Configuration/data.md5.hash.json`),
            direct_forward({ source }) {
                Sen.Script.Console.finished(Setting.Language.get(`result`), Sen.Kernel.Encryption.MD5.hash_fs(source));
                return;
            },
            batch_forward({ directory }) {
                Sen.Kernel.FileSystem.read_directory(directory)
                    .filter((path: string) => Sen.Kernel.FileSystem.is_file(path))
                    .forEach((source: string) => {
                        Sen.Script.Console.display(Setting.Language.get(`processing_file`), source, Definition.Console.Color.CYAN);
                        this.direct_forward({ source });
                    });
                return;
            },
        });
        return;
    }
}
Sen.Script.Executor.Methods.Data.MD5.Hash.forward();
