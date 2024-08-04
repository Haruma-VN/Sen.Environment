import 'dart:convert';
import 'package:delightful_toast/delight_toast.dart';
import 'package:delightful_toast/toast/components/toast_card.dart';
import 'package:delightful_toast/toast/utils/enums.dart';
import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:modding/model/log_message.dart';
import 'package:modding/provider/filter_provider.dart';
import 'package:modding/provider/item_provider.dart';
import 'package:modding/provider/log_provider.dart';
import 'package:modding/provider/manifest_provider.dart';
import 'package:modding/provider/recent_provider.dart';
import 'package:modding/screen/home_screen.dart';
import 'package:modding/screen/manifest_screen.dart';
import 'package:modding/screen/packages_screen.dart';
import 'package:modding/screen/packet_screen.dart';
import 'package:modding/screen/setting_screen.dart';
import 'package:modding/service/file_service.dart';
import 'dart:io';
import 'package:provider/provider.dart';

class RootScreen extends StatefulWidget {
  const RootScreen({super.key, required this.title});

  final String title;

  @override
  State<RootScreen> createState() => _RootScreenState();
}

class _RootScreenState extends State<RootScreen> {
  int _currentPageIndex = 0;

  final _labelBehavior = NavigationDestinationLabelBehavior.alwaysShow;

  final List<Widget> _destinations = const [
    HomeScreen(),
    PackagesScreen(),
    PacketScreen(),
    ManifestScreen(),
    SettingScreen(),
  ];

  void _changeScreen(int index) {
    setState(() {
      _currentPageIndex = index;
    });
  }

  Widget? _makeNavigationBar() {
    if (Platform.isAndroid || Platform.isIOS) {
      return NavigationBar(
        labelBehavior: _labelBehavior,
        selectedIndex: _currentPageIndex,
        onDestinationSelected: _changeScreen,
        destinations: const <Widget>[
          NavigationDestination(
            icon: Icon(Icons.home_outlined),
            label: 'Home',
          ),
          NavigationDestination(
            icon: Icon(Icons.check_box_outline_blank),
            label: 'Packages',
          ),
          NavigationDestination(
            icon: Icon(Icons.file_copy_outlined),
            label: 'Packet',
          ),
          NavigationDestination(
            icon: Icon(Icons.data_object_outlined),
            label: 'Manifest',
          ),
          NavigationDestination(
            icon: Icon(Icons.settings_outlined),
            label: 'Settings',
          ),
        ],
      );
    }
    return null;
  }

  void _filterOnClick() async {
    await showDialog(
      context: context,
      builder: (context) => _buildFilterDialog(context),
    );
  }

  Widget _buildFilterDialog(BuildContext context) {
    final filterProvider = Provider.of<FilterProvider>(context);
    return AlertDialog(
      title: const Text('Filter'),
      actions: [
        ..._buildFilterOptions(filterProvider),
        TextButton(
          onPressed: () {
            Navigator.of(context).pop();
          },
          child: const Text('Finish'),
        ),
      ],
    );
  }

  List<Widget> _buildFilterOptions(FilterProvider filterProvider) {
    return ['disabled', 'rton', 'json']
        .map(
          (e) => RadioListTile(
            title: Text(e),
            value: e,
            groupValue: filterProvider.value,
            onChanged: (value) {
              if (value == null) {
                return;
              }
              filterProvider.value = value;
            },
          ),
        )
        .toList();
  }

  void _showSnackBar({
    required String title,
    Color? color,
  }) {
    DelightToastBar(
      autoDismiss: true,
      position: DelightSnackbarPosition.bottom,
      builder: (context) => ToastCard(
        color: color,
        leading: const Icon(Icons.done_outline, size: 30),
        title: Text(
          title,
          style: const TextStyle(fontWeight: FontWeight.w500, fontSize: 14),
        ),
      ),
    ).show(context);
    return;
  }

  void _onUploadDirectory() {
    final logProvider = Provider.of<LogProvider>(context, listen: false);
    final itemProvider = Provider.of<ItemProvider>(context, listen: false);
    final recentProvider = Provider.of<RecentProvider>(context, listen: false);
    logProvider.empty();
    logProvider.add(LogMessage(
      title: 'Upload modding directory',
      time: DateTime.now(),
    ));
    if (itemProvider.path != '') {
      _showSnackBar(
        title: 'loaded: ${itemProvider.path}',
        color: Colors.green.withOpacity(0.85),
      );
      logProvider.add(LogMessage(
        title: 'Uploaded modding directory',
        subtitle: itemProvider.path,
        time: DateTime.now(),
      ));
      recentProvider.addFile(itemProvider.path);
    }
  }

  void _recentOnClick() async {
    await showDialog(
      context: context,
      builder: (context) => _buildRecentDialog(context),
    );
  }

  Widget _buildRecentDialog(BuildContext context) {
    final recentProvider = Provider.of<RecentProvider>(context);
    final itemProvider = Provider.of<ItemProvider>(context);

    return AlertDialog(
      title: const Text('Recent directory'),
      content: SingleChildScrollView(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: _buildRecentFileList(recentProvider, itemProvider),
        ),
      ),
      actions: [
        _buildDialogActions(context),
      ],
    );
  }

  List<Widget> _buildRecentFileList(
    RecentProvider recentProvider,
    ItemProvider itemProvider,
  ) {
    return recentProvider.recentFiles
        .map(
          (e) => Column(
            children: [
              Row(
                children: [
                  Expanded(child: Text(e)),
                  const Spacer(),
                  TextButton(
                    onPressed: () {
                      itemProvider.path = e;
                      _onUploadDirectory();
                      Navigator.of(context).pop();
                    },
                    child: const Text('Load'),
                  ),
                ],
              ),
              const SizedBox(height: 10.0),
              const Divider(),
            ],
          ),
        )
        .toList();
  }

  void _showConfirmClearDialog() async {
    await showDialog(
      context: context,
      builder: (context) => _buildClearConfirmDialog(context),
    );
  }

  Widget _buildClearConfirmDialog(BuildContext context) {
    return AlertDialog(
      title: const Text('Would you like to clear all the data?'),
      content: _buildDialogContent(context),
      actions: _buildConfirmDialogActions(context),
    );
  }

  Widget _buildDialogContent(BuildContext context) {
    return const Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      mainAxisSize: MainAxisSize.min,
      children: [
        Text(
          'Once the directory are cleared, you are no longer chance to get them back, are you sure want to continue?',
        ),
        SizedBox(height: 10),
      ],
    );
  }

  void _onFinish({
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

  List<Widget> _buildConfirmDialogActions(BuildContext context) {
    return [
      TextButton(
        onPressed: () {
          final recentProvider = Provider.of<RecentProvider>(
            context,
            listen: false,
          );
          recentProvider.clearFiles();
          Navigator.of(context).pop();
          _onFinish(title: 'All recent directory has been deleted');
        },
        child: const Text('Yes'),
      ),
      TextButton(
        onPressed: () {
          Navigator.of(context).pop();
          _onFinish(title: 'All recent directory has not been deleted');
        },
        child: const Text('No'),
      ),
    ];
  }

  Widget _buildDialogActions(BuildContext context) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.end,
      children: [
        TextButton(
          onPressed: () {
            Navigator.of(context).pop();
            _showConfirmClearDialog();
          },
          child: const Text('Clear all'),
        ),
        const SizedBox(width: 10),
        TextButton(
          onPressed: () {
            Navigator.of(context).pop();
          },
          child: const Text('Finish'),
        ),
      ],
    );
  }

  void _saveOnClick() async {
    final itemProvider = Provider.of<ItemProvider>(context, listen: false);
    final manifestProvider =
        Provider.of<ManifestProvider>(context, listen: false);
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Would you like to save the file?'),
        actions: [
          TextButton(
            onPressed: () {
              Navigator.of(context).pop();
              FileService.writeFile(
                source: '${itemProvider.path}/data.json',
                data: const JsonEncoder.withIndent('\t')
                    .convert(manifestProvider.manifest),
              );
              _onFinish(title: 'The file has been saved!');
            },
            child: const Text('Yes'),
          ),
          TextButton(
            onPressed: () {
              Navigator.of(context).pop();
            },
            child: const Text('No'),
          ),
        ],
      ),
    );
  }

  void _showFinishedDialog(String destination) async {
    await showDialog(
      context: context,
      barrierDismissible: false,
      builder: (context) {
        return AlertDialog(
          title: const Text('Saving file completed'),
          actions: [
            Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                _DialogContent(destination: destination),
                _DialogActions(
                  onOkayPressed: () {
                    Navigator.of(context).pop();
                  },
                ),
              ],
            ),
          ],
        );
      },
    );
  }

  void _exportLog() async {
    final controller = TextEditingController();
    final logProvider = Provider.of<LogProvider>(context, listen: false);
    String? destination;
    await showDialog(
      context: context,
      builder: (context) {
        return AlertDialog(
          title: const Text('Export log'),
          actions: [
            _ExportDialogActions(
              controller: controller,
              onFilePicked: (filePath) {
                if (filePath != null) {
                  destination = filePath;
                  controller.text = filePath;
                }
              },
              onSavePressed: () {
                Navigator.of(context).pop();
                if (destination == null) {
                  _onFinish(title: 'Export log destination cannot be empty');
                  return;
                }
                _saveLog(logProvider, destination!);
              },
              onCancelPressed: () {
                Navigator.of(context).pop();
              },
            ),
          ],
        );
      },
    );

    controller.dispose();
  }

  void _saveLog(LogProvider logProvider, String destination) {
    final buffer = StringBuffer();
    const idWidth = 5;
    const timeWidth = 25;
    const commandWidth = 50;
    buffer.writeln(
      '${'ID'.padRight(idWidth)}\t${'Time'.padRight(timeWidth)}\t${'Command'.padRight(commandWidth)}\tMessage',
    );
    for (var i = 0; i < logProvider.list.length; i++) {
      final e = logProvider.list[i];
      final timeString = e.time.toIso8601String().padRight(timeWidth);
      buffer.writeln(
          '${i.toString().padRight(idWidth)}\t$timeString\t${e.title.padRight(commandWidth)}\t${e.subtitle}');
    }
    final logMessage = buffer.toString();
    FileService.writeFile(source: destination, data: logMessage);
    _showFinishedDialog(destination);
  }

  List<Widget>? _actions() {
    switch (_currentPageIndex) {
      case 0:
        return _homePageAction();
      case 1:
        return _packagesPageAction();
      case 3:
        return _manifestPageAction();
      default:
        return null;
    }
  }

  Widget _iconButtonWithTooltip({
    required void Function()? onPressed,
    required IconData icon,
    required String tooltip,
  }) {
    return Tooltip(
      message: tooltip,
      child: IconButton(
        onPressed: onPressed,
        icon: Icon(icon),
      ),
    );
  }

  List<Widget> _homePageAction() {
    return [
      _iconButtonWithTooltip(
        icon: Icons.bug_report_outlined,
        onPressed: _exportLog,
        tooltip: 'Export log',
      ),
      _iconButtonWithTooltip(
        icon: Icons.history_outlined,
        onPressed: _recentOnClick,
        tooltip: 'Recent',
      ),
    ];
  }

  List<Widget> _packagesPageAction() {
    return [
      _iconButtonWithTooltip(
        icon: Icons.filter_list_outlined,
        onPressed: _filterOnClick,
        tooltip: 'Filter',
      ),
    ];
  }

  List<Widget> _manifestPageAction() {
    return [
      _iconButtonWithTooltip(
        icon: Icons.save_outlined,
        onPressed: _saveOnClick,
        tooltip: 'Save',
      ),
    ];
  }

  Widget _makeNavigationRail() {
    if (Platform.isWindows || Platform.isLinux || Platform.isMacOS) {
      return NavigationRail(
        selectedIndex: _currentPageIndex,
        onDestinationSelected: (index) {
          setState(() {
            _currentPageIndex = index;
          });
        },
        labelType: NavigationRailLabelType.all,
        destinations: const [
          NavigationRailDestination(
            icon: Icon(Icons.home_outlined),
            selectedIcon: Icon(Icons.home_filled),
            label: Text('Home'),
          ),
          NavigationRailDestination(
            icon: Icon(Icons.check_box_outline_blank),
            selectedIcon: Icon(Icons.check_box),
            label: Text('Packages'),
          ),
          NavigationRailDestination(
            icon: Icon(Icons.file_copy_outlined),
            selectedIcon: Icon(Icons.file_copy),
            label: Text('Packet'),
          ),
          NavigationRailDestination(
            icon: Icon(Icons.data_object_outlined),
            selectedIcon: Icon(Icons.data_object),
            label: Text('Manifest'),
          ),
          NavigationRailDestination(
            icon: Icon(Icons.settings_outlined),
            selectedIcon: Icon(Icons.settings),
            label: Text('Settings'),
          ),
        ],
      );
    }
    return const SizedBox.shrink();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
        actions: _actions(),
      ),
      body: Row(
        children: [
          _makeNavigationRail(),
          Expanded(
            child: _destinations[_currentPageIndex],
          ),
        ],
      ),
      bottomNavigationBar: _makeNavigationBar(),
    );
  }
}

class _DialogContent extends StatelessWidget {
  final String destination;

  const _DialogContent({required this.destination});

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text('File saved: $destination'),
        const SizedBox(height: 15),
      ],
    );
  }
}

class _DialogActions extends StatelessWidget {
  final VoidCallback onOkayPressed;

  const _DialogActions({required this.onOkayPressed});

  @override
  Widget build(BuildContext context) {
    return TextButton(
      onPressed: onOkayPressed,
      child: const Text('Okay'),
    );
  }
}

class _ExportDialogContent extends StatelessWidget {
  final TextEditingController controller;
  final ValueChanged<String?> onFilePicked;

  const _ExportDialogContent({
    required this.controller,
    required this.onFilePicked,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Row(
          children: [
            Expanded(
              child: TextField(
                controller: controller,
              ),
            ),
            IconButton(
              onPressed: () async {
                var outputFile = await FilePicker.platform.saveFile(
                  dialogTitle: 'Select an output path',
                  fileName: 'log_${DateTime.now().microsecondsSinceEpoch}.txt',
                );
                onFilePicked(outputFile);
              },
              icon: const Icon(Icons.file_upload_outlined),
            ),
          ],
        ),
        const SizedBox(height: 10),
      ],
    );
  }
}

class _ExportDialogActions extends StatelessWidget {
  final VoidCallback onSavePressed;
  final VoidCallback onCancelPressed;

  final TextEditingController controller;
  final ValueChanged<String?> onFilePicked;

  const _ExportDialogActions({
    required this.onSavePressed,
    required this.onCancelPressed,
    required this.controller,
    required this.onFilePicked,
  });

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        _ExportDialogContent(
          controller: controller,
          onFilePicked: onFilePicked,
        ),
        Row(
          children: [
            TextButton(
              onPressed: onSavePressed,
              child: const Text('Save'),
            ),
            TextButton(
              onPressed: onCancelPressed,
              child: const Text('Finished'),
            ),
          ],
        ),
      ],
    );
  }
}
