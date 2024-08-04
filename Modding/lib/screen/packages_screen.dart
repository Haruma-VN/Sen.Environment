import 'dart:convert';
import 'dart:io';
import 'package:flutter/material.dart';
import 'package:flutter_simple_treeview/flutter_simple_treeview.dart';
import 'package:modding/model/log_message.dart';
import 'package:modding/model/tree_node_data.dart';
import 'package:modding/provider/filter_provider.dart';
import 'package:modding/provider/item_provider.dart';
import 'package:modding/provider/log_provider.dart';
import 'package:modding/provider/setting_provider.dart';
import 'package:modding/screen/shell_screen.dart';
import 'package:modding/service/file_service.dart';
import 'package:provider/provider.dart';
import 'package:json_editor_flutter/json_editor_flutter.dart';

class PackagesScreen extends StatefulWidget {
  const PackagesScreen({super.key});

  @override
  State<PackagesScreen> createState() => _PackagesScreenState();
}

class _PackagesScreenState extends State<PackagesScreen> {
  late TreeController _treeController;

  @override
  void initState() {
    _treeController = TreeController(allNodesExpanded: false);
    super.initState();
  }

  @override
  void dispose() {
    super.dispose();
  }

  TreeNode _buildTreeNode(TreeNodeData data) {
    return TreeNode(
      content: _buildNodeContent(data),
      children: data.children.map(_buildTreeNode).toList(),
    );
  }

  Widget _buildNodeContent(TreeNodeData data) {
    return Row(
      children: [
        Icon(
          data.isFile ? Icons.insert_drive_file_outlined : Icons.folder,
          color: data.isFile ? null : Colors.yellow,
        ),
        const SizedBox(width: 10),
        Text(data.name),
        const SizedBox(width: 10),
        if (data.isFile) _buildMenuAnchor(data),
      ],
    );
  }

  Widget _buildMenuAnchor(TreeNodeData data) {
    return MenuAnchor(
      builder:
          (BuildContext context, MenuController controller, Widget? child) {
        return IconButton(
          onPressed: () {
            if (controller.isOpen) {
              controller.close();
            } else {
              controller.open();
            }
          },
          icon: const Icon(Icons.more_horiz_outlined),
          tooltip: 'Option',
        );
      },
      menuChildren: _buildMenuItems(data),
    );
  }

  final _canBeTriggered = [
    'Forward',
  ];

  final _rtonMethods = [
    'Decode',
    'Decrypt',
    'Encrypt',
    'Decrypt and Decode',
  ];

  final _jsonMethods = [
    'Encode',
    'Encode and Encrypt',
    'Edit',
  ];

  List<Widget> _buildMenuItems(TreeNodeData data) {
    List<String> methods;
    if (data.name.toLowerCase().endsWith('rton')) {
      methods = [..._canBeTriggered, ..._rtonMethods];
    } else {
      methods = [..._canBeTriggered, ..._jsonMethods];
    }
    return methods
        .map((e) => MenuItemButton(
              onPressed: () => _handleMenuItemClick(e, data),
              child: Text(e),
            ))
        .toList();
  }

  void _disposeScreen() {
    if (Navigator.of(context).canPop()) {
      Navigator.of(context).pop();
    }
  }

  void _showSaveFinish() async {
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Your file has been saved'),
        actions: [
          TextButton(
            onPressed: () {
              _disposeScreen();
            },
            child: const Text('Okay'),
          ),
        ],
      ),
    );
  }

  String _exchangeMethod(String item) {
    Map<String, String> data = {
      'Encode': 'popcap.rton.encode',
      'Decode': 'popcap.rton.decode',
      'Encrypt': 'popcap.rton.encrypt',
      'Decrypt': 'popcap.rton.decrypt',
      'Encode and Encrypt': 'popcap.rton.encode_and_encrypt',
      'Decrypt and Decode': 'popcap.rton.decrypt_and_decode',
    };
    return data[item] ?? '?';
  }

  void _handleMenuItemClick(String item, TreeNodeData data) async {
    final logProvider = Provider.of<LogProvider>(context, listen: false);
    logProvider.add(LogMessage(
      title: item,
      subtitle: data.path,
      time: DateTime.now(),
    ));
    switch (item) {
      case 'Edit':
        {
          await _editFile(data.path);
          break;
        }
      case 'Forward':
        {
          await _executeShell([data.path]);
          break;
        }
      default:
        {
          await _executeShell(
              ['-source', data.path, '-method', _exchangeMethod(item)]);
          break;
        }
    }
  }

  Future<void> _executeShell(
    List<String> arguments,
  ) async {
    await showDialog(
      context: context,
      builder: (context) => ShellScreen(
        shellPath:
            Provider.of<SettingProvider>(context, listen: false).toolChain,
        arguments: arguments,
      ),
    );
  }

  Future<void> _editFile(String filePath) async {
    var jsonStr = File(filePath).readAsStringSync();
    var value = jsonDecode(jsonStr);

    await Navigator.of(context).push(
      MaterialPageRoute(
        builder: (context) => Scaffold(
          appBar: AppBar(
            title: const Text('Sen: Modding Environment'),
            actions: [
              IconButton(
                onPressed: () async {
                  await _showSaveDialog(
                    onPressed: () {
                      File(filePath).writeAsStringSync(
                        const JsonEncoder.withIndent('\t').convert(value),
                      );
                      _disposeScreen();
                      _showSaveFinish();
                    },
                  );
                },
                icon: const Icon(Icons.save_outlined),
              ),
            ],
          ),
          body: JsonEditor(
            onChanged: (newValue) {
              value = newValue;
            },
            json: jsonStr,
          ),
        ),
      ),
    );
  }

  Future<void> _showSaveDialog({
    required void Function()? onPressed,
  }) async {
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Would you like to save your file?'),
        actions: [
          TextButton(
            onPressed: onPressed,
            child: const Text('Yes'),
          ),
          TextButton(
            onPressed: () {
              _disposeScreen();
            },
            child: const Text('No'),
          ),
        ],
      ),
    );
  }

  TreeNodeData _buildTree(List<String> paths) {
    final provider = Provider.of<ItemProvider>(context);
    var root = TreeNodeData(name: 'root', children: [], path: '');
    for (var path in paths) {
      var parts = path.replaceAll('\\', '/').split('/');
      var current = root;
      var currentPath = provider.path.replaceAll('\\', '/');
      for (var i = 0; i < parts.length; ++i) {
        var part = parts[i];
        currentPath = '$currentPath/$part';
        var isFile = FileService.isFile(currentPath);
        var child = current.children.firstWhere((node) => node.name == part,
            orElse: () {
          var newNode = TreeNodeData(
            name: part,
            isFile: isFile,
            children: [],
            path: currentPath,
          );
          current.children.add(newNode);
          return newNode;
        });
        current = child;
      }
    }
    return root;
  }

  @override
  Widget build(BuildContext context) {
    final itemProvider = Provider.of<ItemProvider>(context);
    final filterProvider = Provider.of<FilterProvider>(context);
    List<String> data = [];
    if (itemProvider.path != '') {
      data = FileService.readDirectory(
        source: '${itemProvider.path}/packages',
        recursive: true,
      ).map((e) => e.replaceAll('${itemProvider.path}/', '')).toList();
    }
    if (filterProvider.value != 'disabled') {
      data = data.where((e) {
        return e.toLowerCase().endsWith(filterProvider.value);
      }).toList();
    }
    var root = _buildTree(data);
    return SingleChildScrollView(
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          SingleChildScrollView(
            child: TreeView(
              treeController: _treeController,
              nodes: root.children.map(_buildTreeNode).toList(),
            ),
          )
        ],
      ),
    );
  }
}
