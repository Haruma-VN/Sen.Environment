namespace Sen.Script.Helper.Debugger.TestModdingMethod {
    export function execute() {
        let source = Console.path("input rsb file", "file");
        let destination = `${Kernel.Path.except_extension(source)}.bundle`;
        Kernel.Support.Miscellaneous.unpack_packet_contain_resource(source, destination);
        return;
    }
}
Sen.Script.Helper.Debugger.TestModdingMethod.execute();
