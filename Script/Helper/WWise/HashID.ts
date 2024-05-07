namespace Sen.Script.Helper.WWise.HashID {
    export function execute() {
        // TODO : Add localization
        let input: string = undefined!;
        const destination = new Kernel.UInteger32();
        while (input !== "") {
            Console.argument("input string");
            input = Kernel.Console.readline();
            Kernel.Support.WWise.SoundBank.hash(input!, destination);
            Console.obtained(`${input}`);
            Console.finished(`${destination.value}`);
        }
        return;
    }
}
Sen.Script.Helper.WWise.HashID.execute();
