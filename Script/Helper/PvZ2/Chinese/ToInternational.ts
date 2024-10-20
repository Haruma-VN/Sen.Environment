// namespace Sen.Script.Helper.PvZ2.Chinese.ToInternational {
//     export type Dummy = Record<string, Array<string>>;

//     export interface Prop {
//         international_manifest: Kernel.Support.PopCap.RSB.Manifest;
//         chinese_manifest: Kernel.Support.PopCap.RSB.Manifest;
//         international_packet: string;
//         chinese_packet: string;
//         dummy: Dummy;
//         compression_flag: Kernel.Support.PopCap.RSG.CompressionFlags;
//         chinese_res_info: Kernel.Support.PopCap.ResInfo.Structure;
//         international_res_info: Kernel.Support.PopCap.ResInfo.Structure;
//     }

//     export function watch(manifest: Kernel.Support.PopCap.RSB.Manifest, dump: Dummy): void {
//         Object.keys(manifest.group).forEach(function watch_packet(group): void {
//             dump[group] = [];
//             for (const subgroup of Object.keys(manifest.group[group].subgroup)) {
//                 dump[group].push(subgroup);
//             }
//         });
//         return;
//     }

//     export function process_common_rsg(source: string, destination: string, compression_flag: Kernel.Support.PopCap.RSG.CompressionFlags): void {
//         const unpacked_source: string = `${Kernel.Path.except_extension(source)}.packet`;
//         Kernel.Support.PopCap.RSG.unpack_fs(source, unpacked_source);
//         const packet_info: Kernel.Support.PopCap.RSG.Definition = Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.RSG.Definition>(`${unpacked_source}/packet.json`);
//         packet_info.compression_flags = compression_flag;
//         Kernel.JSON.serialize_fs<Kernel.Support.PopCap.RSG.Definition>(`${unpacked_source}/packet.json`, packet_info, 1, false);
//         Kernel.Support.PopCap.RSG.pack_fs(unpacked_source, destination);
//         Kernel.FileSystem.Operation.remove_all(unpacked_source);
//         return;
//     }

//     export function process_texture_rsg(source: string, destination: string, compression_flag: Kernel.Support.PopCap.RSG.CompressionFlags): void {
//         const unpacked_source: string = `${Kernel.Path.except_extension(source)}.packet`;
//         Kernel.Support.PopCap.RSG.unpack_fs(source, unpacked_source);
//         const packet_info: Kernel.Support.PopCap.RSG.Definition = Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.RSG.Definition>(`${unpacked_source}/packet.json`);
//         packet_info.compression_flags = compression_flag;
//         packet_info.res.forEach(function process_texture(e): void {
//             const raw_file: string = `${unpacked_source}/res/${e.path}`;
//             const image_file: string = raw_file.replace(/(((\.ptx)))$/i, ".png");
//             Kernel.Support.Texture.decode_fs(raw_file, image_file, e.ptx_info!.width, e.ptx_info!.height, Support.Texture.Format.RGBA_PVRTC_4BPP);
//             Kernel.Support.Texture.encode_fs(image_file, raw_file, Support.Texture.Format.RGB_ETC1_A_8);
//         });
//         Kernel.JSON.serialize_fs<Kernel.Support.PopCap.RSG.Definition>(`${unpacked_source}/packet.json`, packet_info, 1, false);
//         Kernel.Support.PopCap.RSG.pack_fs(unpacked_source, destination);
//         Kernel.FileSystem.Operation.remove_all(unpacked_source);
//         return;
//     }

//     export function process({ chinese_manifest, chinese_packet, dummy, international_manifest, international_packet, compression_flag, chinese_res_info, international_res_info }: Prop): void {
//         for (const [composite_group, subgroups] of Object.entries(dummy)) {
//             Console.send(`${Kernel.Language.get("script.helper.pvz2.chinese.to_international.in_queue")}: ${composite_group}`, Definition.Console.Color.CYAN);
//             if (international_manifest.group[composite_group] !== undefined) {
//                 throw new Error(format(Kernel.Language.get("script.helper.pvz2.chinese.to_international.composite_group_already_existed"), composite_group));
//             }
//             if (chinese_manifest.group[composite_group] === undefined) {
//                 throw new Error(format(Kernel.Language.get("script.helper.pvz2.chinese.to_international.composite_group_is_not_found_in_chinese_manifest"), composite_group));
//             }
//             international_manifest.group[composite_group] = {
//                 is_composite: chinese_manifest.group[composite_group].is_composite,
//                 // reference is cheaper, but if necessary can make copy with Kernel.Miscellaneous.make_copy(chinese_manifest.group[composite_group].subgroup)
//                 subgroup: chinese_manifest.group[composite_group].subgroup,
//             };
//             // copy chinese res-info to international res-info, because we don't need to make any copy
//             international_res_info.groups[composite_group] = chinese_res_info.groups[composite_group];
//             subgroups.forEach(function process_subgroup(subgroup): void {
//                 const current_node = international_manifest.group[composite_group].subgroup[subgroup].packet_info;
//                 current_node.compression_flags = compression_flag;
//                 if (chinese_manifest.group[composite_group].subgroup[subgroup].category[0] === 0n) {
//                     process_common_rsg(`${chinese_packet}/${subgroup}.rsg`, `${international_packet}/${subgroup}.rsg`, current_node.compression_flags);
//                 } else {
//                     process_texture_rsg(`${chinese_packet}/${subgroup}.rsg`, `${international_packet}/${subgroup}.rsg`, current_node.compression_flags);
//                     current_node.res.forEach(function rewrite_information(e): void {
//                         delete e.ptx_info!.alpha_format;
//                         delete e.ptx_info!.alpha_size;
//                         e.ptx_info!.format = 147n;
//                     });
//                 }
//             });
//         }
//         return;
//     }

//     export function execute(): void {
//         const international_bundle: string = Console.path(Kernel.Language.get("script.helper.pvz2.chinese.to_international.input_international_bundle"), "directory");
//         const chinese_bundle: string = Console.path(Kernel.Language.get("script.helper.pvz2.chinese.to_international.input_chinese_bundle"), "directory");
//         Console.argument(Kernel.Language.get("script.helper.pvz2.chinese.to_international.parsing_international_manifest"));
//         const international_manifest: Kernel.Support.PopCap.RSB.Manifest = Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.RSB.Manifest>(`${international_bundle}/manifest.json`);
//         Console.finished(Kernel.Language.get("script.helper.pvz2.chinese.to_international.done"));
//         Console.argument(Kernel.Language.get("script.helper.pvz2.chinese.to_international.parsing_chinese_manifest"));
//         const chinese_manifest: Kernel.Support.PopCap.RSB.Manifest = Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.RSB.Manifest>(`${chinese_bundle}/manifest.json`);
//         Console.finished(Kernel.Language.get("script.helper.pvz2.chinese.to_international.done"));
//         {
//             const dummy: Dummy = {};
//             watch(chinese_manifest, dummy);
//             const ripe_file = `${chinese_bundle}/group.json`;
//             Kernel.JSON.serialize_fs<Dummy>(ripe_file, dummy, 1, false);
//             Console.finished(`${Kernel.Language.get("script.helper.pvz2.chinese.to_international.dumped_file")}: ${ripe_file}`);
//         }
//         const ripe_file: string = `${international_bundle}/group.json`;
//         Kernel.JSON.serialize_fs<Dummy>(ripe_file, {}, 1, false);
//         Console.finished(`${Kernel.Language.get("script.helper.pvz2.chinese.to_international.to_port_2c_content_edit_this")}: ${ripe_file}`);
//         Console.argument(Kernel.Language.get("script.helper.pvz2.chinese.to_international.waiting"));
//         Kernel.Console.readline();
//         Console.argument(Kernel.Language.get("script.helper.pvz2.chinese.to_international.parsing_international_res_info"));
//         const international_res_info: Kernel.Support.PopCap.ResInfo.Structure = Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.ResInfo.Structure>(`${international_bundle}/res.json`);
//         Console.argument(Kernel.Language.get("script.helper.pvz2.chinese.to_international.parsing_chinese_res_info"));
//         const chinese_res_info: Kernel.Support.PopCap.ResInfo.Structure = Kernel.JSON.deserialize_fs<Kernel.Support.PopCap.ResInfo.Structure>(`${chinese_bundle}/res.json`);
//         process({
//             chinese_manifest: chinese_manifest,
//             chinese_packet: `${chinese_bundle}/packet`,
//             dummy: Kernel.JSON.deserialize_fs<Dummy>(ripe_file),
//             international_manifest: international_manifest,
//             international_packet: `${international_bundle}/packet`,
//             chinese_res_info: chinese_res_info,
//             international_res_info: international_res_info,
//             compression_flag: 3n,
//         });
//         Console.finished(Kernel.Language.get("script.helper.pvz2.chinese.to_international.all_process_has_been_finished"));
//         Kernel.JSON.serialize_fs<Kernel.Support.PopCap.RSB.Manifest>(`${international_bundle}/manifest.json`, international_manifest, 1, false);
//         Kernel.JSON.serialize_fs<Kernel.Support.PopCap.ResInfo.Structure>(`${international_bundle}/res.json`, international_res_info, 1, false);
//         return;
//     }
// }

// Sen.Script.Helper.PvZ2.Chinese.ToInternational.execute();
