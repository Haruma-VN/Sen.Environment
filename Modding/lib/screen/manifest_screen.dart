import 'dart:convert';

import 'package:flutter/material.dart';
import 'package:material_symbols_icons/material_symbols_icons.dart';
import 'package:modding/model/manifest.dart';
import 'package:modding/provider/item_provider.dart';
import 'package:modding/provider/manifest_provider.dart';
import 'package:modding/service/file_service.dart';
import 'package:provider/provider.dart';

class ManifestScreen extends StatefulWidget {
  const ManifestScreen({super.key});

  @override
  State<ManifestScreen> createState() => _ManifestScreenState();
}

class _ManifestScreenState extends State<ManifestScreen> {
  Manifest? _manifest;

  void _parseManifest() {
    if (_manifest != null) {
      return;
    }
    final itemProvider = Provider.of<ItemProvider>(context);
    final manifestProvider = Provider.of<ManifestProvider>(
      context,
      listen: false,
    );
    if (itemProvider.path == '') {
      throw Exception('manifest not found');
    }
    try {
      _manifest = Manifest.fromJson(
        jsonDecode(
          FileService.readFile(
            source: '${itemProvider.path}/data.json',
          ),
        ),
      );
      manifestProvider.manifest = _manifest!;
    } catch (e) {
      throw Exception(e);
    }
  }

  String _exchangeDisplayValue(String value) {
    final destination = {
      'string': 'Since 10.4+ version in PvZ2 Official',
      'array': 'From 1.4 to 10.3 in PvZ2 Official and PvZ2 Chinese',
    };
    return destination[value]!;
  }

  String _exchangeTextureFormatCategory(int index) {
    final exchangeTable = {
      0: 'Android',
      1: 'iOS',
      2: 'Android Chinese',
    };
    return exchangeTable[index]!;
  }

  Widget _buildHeaderInfo() {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        ListTile(
          title: const Text('Version number'),
          trailing: Text(
            _manifest!.version.toString(),
            style: Theme.of(context).textTheme.labelLarge,
          ),
          leading: const Icon(Symbols.deployed_code),
        ),
        const SizedBox(height: 10),
        ListTile(
          title: const Text('Texture format version'),
          trailing: DropdownButton<int>(
            value: _manifest!.textureFormatVersion,
            onChanged: (int? value) {
              setState(() {
                _manifest!.textureFormatVersion = value!;
              });
            },
            items: [0, 1, 2].map<DropdownMenuItem<int>>((int value) {
              return DropdownMenuItem<int>(
                value: value,
                child: Text(value.toString()),
              );
            }).toList(),
          ),
          leading: const Icon(Symbols.deployed_code),
        ),
        const SizedBox(height: 10),
        ListTile(
          title: const Text('Texture format category'),
          trailing: DropdownButton<int>(
            value: _manifest!.textureFormatVersion,
            onChanged: (int? value) {
              setState(() {
                _manifest!.textureFormatVersion = value!;
              });
            },
            items: [0, 1, 2].map<DropdownMenuItem<int>>((int value) {
              return DropdownMenuItem<int>(
                value: value,
                child: Text(value.toString()),
              );
            }).toList(),
          ),
        ),
      ],
    );
  }

  Widget _buildResourceInfo() {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      mainAxisSize: MainAxisSize.min,
      children: [
        ListTile(
          title: const Text('Resources Convert Style'),
          trailing: DropdownButton<String>(
            value: _manifest!.resourceInfo.expandPath,
            onChanged: (String? value) {
              setState(() {
                _manifest!.resourceInfo.expandPath = value!;
              });
            },
            items: ['string', 'array']
                .map<DropdownMenuItem<String>>((String value) {
              return DropdownMenuItem<String>(
                value: value,
                child: Text(
                  _exchangeDisplayValue(value),
                  softWrap: true,
                  overflow: TextOverflow.visible,
                ),
              );
            }).toList(),
          ),
        ),
        ListTile(
          title: const Text('Use Newton file'),
          trailing: Switch(
            value: _manifest!.resourceInfo.useNewton,
            onChanged: (bool value) {
              setState(() {
                _manifest!.resourceInfo.useNewton = value;
              });
            },
          ),
        ),
      ],
    );
  }

  Widget _buildPackageInfo() {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        ListTile(
          title: const Text('RTON is encrypted'),
          trailing: Switch(
            value: _manifest!.packageInfo.rtonIsEncrypted,
            onChanged: (bool value) {
              setState(() {
                _manifest!.packageInfo.rtonIsEncrypted = value;
              });
            },
          ),
        ),
        ListTile(
          title:
              const Text('Automatically convert JSON inside Packages if found'),
          trailing: Switch(
            value: _manifest!.packageInfo.autoConvertJsonIfExist,
            onChanged: (bool value) {
              setState(() {
                _manifest!.packageInfo.autoConvertJsonIfExist = value;
              });
            },
          ),
        ),
      ],
    );
  }

  void _onSaveFinish({
    required String title,
  }) async {
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: Text(title),
        actions: [
          TextButton(
            onPressed: () {
              Navigator.of(context).pop();
            },
            child: const Text('Okay'),
          ),
        ],
      ),
    );
  }

  Widget _buildGroupInfo({
    String? groupSearch,
  }) {
    late List<String> group;
    if (groupSearch != null) {
      group = _manifest!.group
          .where((e) => e.toLowerCase().contains(groupSearch.toLowerCase()))
          .toList();
    } else {
      group = _manifest!.group;
    }
    return Container(
      decoration: BoxDecoration(
        borderRadius: BorderRadius.circular(10),
        border: Border.all(color: Colors.blueAccent),
      ),
      child: ConstrainedBox(
        constraints: const BoxConstraints(
          maxHeight: 400,
        ),
        child: ListView.builder(
          itemCount: group.length,
          itemBuilder: (context, index) => Card(
            child: ListTile(
              leading: const Icon(Icons.insert_drive_file_outlined),
              title: Row(
                children: [
                  Expanded(
                    child: Text(group[index]),
                  ),
                  IconButton(
                    icon: const Icon(Icons.info_outlined),
                    onPressed: () => _onInfoPressed(
                      _manifest!.group.indexOf(group[index]),
                    ),
                  ),
                  const SizedBox(width: 10),
                  IconButton(
                    icon: const Icon(Icons.edit_outlined),
                    onPressed: () => _onEditPressed(
                      _manifest!.group.indexOf(group[index]),
                    ),
                  ),
                  const SizedBox(width: 10),
                  IconButton(
                    icon: const Icon(Icons.delete_outline),
                    onPressed: () => _onDeletePressed(
                      _manifest!.group.indexOf(group[index]),
                    ),
                  ),
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }

  void _onInfoPressed(int index) async {
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: Text(_manifest!.group[index]),
        actions: [
          TextButton(
            onPressed: () {
              Navigator.of(context).pop();
            },
            child: const Text('Okay'),
          ),
        ],
      ),
    );
  }

  void _onEditPressed(int index) async {
    final controller = TextEditingController(text: _manifest!.group[index]);
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Edit packet name'),
        actions: [
          TextField(
            controller: controller,
          ),
          const SizedBox(height: 10),
          TextButton(
            onPressed: () {
              Navigator.of(context).pop();
              if (controller.text.isEmpty) {
                _onSaveFinish(title: 'Packet name cannot be empty');
                return;
              }
              setState(() {
                _manifest!.group[index] = controller.text;
              });
              _onSaveFinish(title: 'Save finish!');
            },
            child: const Text('Finish'),
          ),
        ],
      ),
    );
    controller.dispose();
  }

  void _onDeletePressed(int index) async {
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: Text('Are you sure want to delete ${_manifest!.group[index]}?'),
        actions: [
          Row(
            mainAxisAlignment: MainAxisAlignment.end,
            children: [
              TextButton(
                onPressed: () {
                  Navigator.of(context).pop();
                  _onSaveFinish(
                    title: '${_manifest!.group[index]} has been deleted!',
                  );
                  setState(() {
                    _manifest!.group.removeAt(index);
                  });
                },
                child: const Text('Yes'),
              ),
              TextButton(
                onPressed: () {
                  Navigator.of(context).pop();
                  _onSaveFinish(
                    title: '${_manifest!.group[index]} has not been deleted!',
                  );
                },
                child: const Text('No'),
              ),
            ],
          )
        ],
      ),
    );
  }

  void _onSearchGroupData() async {
    final controller = TextEditingController();
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Group to find'),
        actions: [
          TextField(
            controller: controller,
          ),
          TextButton(
            onPressed: () {
              if (controller.text.isNotEmpty) {
                setState(() {
                  _searchGroup = controller.text;
                });
              }
              Navigator.of(context).pop();
            },
            child: const Text('Okay'),
          )
        ],
      ),
    );
    controller.dispose();
  }

  void _onRemoveSearchData() {
    if (_searchGroup == null) {
      _onSaveFinish(title: 'No search data to remove!');
      return;
    }
    setState(() {
      _searchGroup = null;
    });
    _onSaveFinish(title: 'Search data has been removed!');
  }

  Widget _toolchainButton({
    required void Function()? onPressed,
    required String tooltip,
    required IconData icon,
  }) {
    return Tooltip(
      message: tooltip,
      child: IconButton(
        onPressed: onPressed,
        icon: Icon(icon),
      ),
    );
  }

  void _onCheckDuplicate() async {
    final List<String> duplicate = _findDuplicates(_manifest!.group);

    await _showDuplicateDialog(context, duplicate);
  }

  List<String> _findDuplicates(List<String> items) {
    final Set<String> unique = <String>{};
    final List<String> duplicate = [];
    for (var str in items) {
      if (!unique.add(str)) {
        duplicate.add(str);
      }
    }
    return duplicate;
  }

  Widget _duplicateText(List<String> duplicate) {
    return duplicate.isEmpty
        ? const Text('No duplicate found')
        : Text('Found ${duplicate.length} duplicate');
  }

  Widget _duplicateValue(List<String> duplicate) {
    return duplicate.isEmpty
        ? const SizedBox.shrink()
        : SingleChildScrollView(
            child: Column(
              mainAxisAlignment: MainAxisAlignment.center,
              children: duplicate
                  .map(
                    (e) => Center(
                      child: Text(e),
                    ),
                  )
                  .toList(),
            ),
          );
  }

  Future<void> _showDuplicateDialog(
      BuildContext context, List<String> duplicate) async {
    return showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: _duplicateText(duplicate),
        actions: [
          Column(
            mainAxisAlignment: MainAxisAlignment.center,
            crossAxisAlignment: CrossAxisAlignment.end,
            children: [
              _duplicateValue(duplicate),
              const SizedBox(height: 15),
              TextButton(
                onPressed: () {
                  Navigator.of(context).pop();
                },
                child: const Text('Okay'),
              ),
            ],
          ),
        ],
      ),
    );
  }

  void _onSortData() {
    setState(() {
      _manifest!.group.sort();
    });
    _onSaveFinish(title: 'Group has been sorted!');
  }

  Widget _buildToolchainForGroup() {
    return Row(
      children: [
        _toolchainButton(
          tooltip: 'Sort by alphabetically',
          icon: Icons.sort_by_alpha_outlined,
          onPressed: _onSortData,
        ),
        _toolchainButton(
          tooltip: 'Check duplicates',
          icon: Icons.bug_report_outlined,
          onPressed: _onCheckDuplicate,
        ),
        _toolchainButton(
          tooltip: 'Search',
          icon: Icons.search_outlined,
          onPressed: _onSearchGroupData,
        ),
        _toolchainButton(
          tooltip: 'Remove search data',
          icon: Icons.close_outlined,
          onPressed: _onRemoveSearchData,
        ),
      ],
    );
  }

  String? _searchGroup;

  @override
  Widget build(BuildContext context) {
    try {
      _parseManifest();
      return Padding(
        padding: const EdgeInsets.all(8.0),
        child: SingleChildScrollView(
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              const Text('Header information'),
              const SizedBox(height: 10),
              _buildHeaderInfo(),
              const Divider(),
              const Text('Resource information'),
              _buildResourceInfo(),
              const Divider(),
              const Text('Packages information'),
              _buildPackageInfo(),
              const Divider(),
              const Text('Group information'),
              const SizedBox(height: 10),
              _buildToolchainForGroup(),
              const SizedBox(height: 10),
              _buildGroupInfo(groupSearch: _searchGroup),
            ],
          ),
        ),
      );
    } catch (e) {
      return Center(
        child: Text(e.toString()),
      );
    }
  }
}
