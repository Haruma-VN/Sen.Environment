namespace Sen.Script.Support.PopCap.LawnStrings.Convert {
    /**
     * Detail
     */

    /**
     *
     * @param text - from lawnstrings text
     * @returns list
     */

    export function make_list(text: string): Array<string> {
        const text_list: Array<string> = text.split("\n");
        const arr: Array<string> = new Array<string>();
        for (let i: number = 0; i < text_list.length; ++i) {
            if (/^\[.*\]\s*$/.test(text_list[i])) {
                arr.push(text_list[i].slice(1, -1));
                let destination: number = -1;
                find_next: for (let j: number = i + 1; j < text_list.length; ++j) {
                    if (/^\[.*\]\s*$/.test(text_list[j])) {
                        destination = j;
                        break find_next;
                    }
                }
                let text_ripe: string = "";
                if (destination !== -1) {
                    for (let k: number = i + 1; k < destination - 1; ++k) {
                        test_case: switch (text_list[k]) {
                            case "": {
                                text_ripe = text_ripe.concat("\r\n");
                                break test_case;
                            }
                            default: {
                                if (text_ripe !== "") {
                                    text_ripe = text_ripe.concat("\r\n");
                                }
                                text_ripe = text_ripe.concat(text_list[k]);
                                break test_case;
                            }
                        }
                        ++i;
                    }
                } else {
                    find_last: for (let j: number = text_list.length - 1; j > i; --j) {
                        if (text_list[j] === "") {
                            destination = j;
                            break find_last;
                        }
                    }
                    for (let k: number = i + 1; k < destination - 1; ++k) {
                        switch (text_list[k]) {
                            case "": {
                                text_ripe = text_ripe.concat("\r\n");
                                break;
                            }
                            default: {
                                text_ripe = text_ripe.concat(text_list[k]);
                            }
                        }
                        ++i;
                    }
                }
                arr.push(text_ripe);
            }
        }
        return arr;
    }
}
