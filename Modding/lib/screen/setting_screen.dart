import 'dart:io';

import 'package:flutter/material.dart';
import 'package:modding/provider/setting_provider.dart';
import 'package:modding/service/android_service.dart';
import 'package:modding/service/file_service.dart';
import 'package:provider/provider.dart';

class SettingScreen extends StatefulWidget {
  const SettingScreen({super.key});

  @override
  State<SettingScreen> createState() => _SettingScreenState();
}

class _SettingScreenState extends State<SettingScreen> {
  bool _hasPermission = false;

  @override
  void initState() {
    super.initState();
    Future.sync(() async {
      var hasPermission = await _checkDefaultPermission();
      setState(() {
        _hasPermission = hasPermission;
      });
    });
  }

  void _onChangeTheme() async {
    final settingProvider = Provider.of<SettingProvider>(
      context,
      listen: false,
    );
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Theme'),
        content: Column(
          crossAxisAlignment: CrossAxisAlignment.end,
          mainAxisSize: MainAxisSize.min,
          children: [
            _buildThemeOptionTile(
              settingProvider: settingProvider,
              title: 'System',
              value: 'system',
            ),
            _buildThemeOptionTile(
              settingProvider: settingProvider,
              title: 'Light',
              value: 'light',
            ),
            _buildThemeOptionTile(
              settingProvider: settingProvider,
              title: 'Dark',
              value: 'dark',
            ),
            const SizedBox(height: 10),
            _onCloseButton(),
          ],
        ),
      ),
    );
  }

  Widget _onCloseButton() {
    return TextButton(
      onPressed: () {
        Navigator.of(context).pop();
      },
      child: const Text('Okay'),
    );
  }

  Widget _buildThemeOptionTile({
    required SettingProvider settingProvider,
    required String title,
    required String value,
  }) {
    return ListTile(
      title: Text(title),
      leading: Radio<String>(
        value: value,
        groupValue: settingProvider.theme,
        onChanged: (String? value) {
          if (value == null) return;
          settingProvider.setTheme(value);
        },
      ),
    );
  }

  Widget _buildNotificationOption({
    required SettingProvider settingProvider,
    required String title,
    required bool value,
  }) {
    return ListTile(
      title: Text(title),
      leading: Radio<bool>(
        value: value,
        groupValue: settingProvider.sendNotification,
        onChanged: (bool? value) {
          if (value == null) {
            return;
          }
          settingProvider.setNotification(value);
        },
      ),
    );
  }

  void _onChangeNotification() async {
    final settingProvider = Provider.of<SettingProvider>(
      context,
      listen: false,
    );
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Show notification'),
        actions: [
          _buildNotificationOption(
            settingProvider: settingProvider,
            title: 'Enabled',
            value: true,
          ),
          _buildNotificationOption(
            settingProvider: settingProvider,
            title: 'Disabled',
            value: false,
          ),
          const SizedBox(height: 10),
          _onCloseButton(),
        ],
      ),
    );
  }

  void _onCheckValidator() {
    final settingProvider = Provider.of<SettingProvider>(
      context,
      listen: false,
    );
    if (settingProvider.toolChain.isNotEmpty) {
      settingProvider.setIsValid(
        _existKernel(settingProvider.toolChain) &&
            _existScript(settingProvider.toolChain),
      );
    }
  }

  bool _existKernel(String path) {
    if (Platform.isAndroid) {
      return true;
    }
    if (Platform.isWindows) {
      return FileService.isFile('$path/Kernel.dll');
    }
    if (Platform.isLinux) {
      return FileService.isFile('$path/libKernel.so');
    }
    if (Platform.isIOS || Platform.isMacOS) {
      return FileService.isFile('$path/libKernel.dylib');
    }
    return false;
  }

  Future<bool> _checkDefaultPermission() async {
    if (Platform.isAndroid) {
      return await AndroidService.checkStoragePermission();
    }
    return true;
  }

  bool _existScript(String path) {
    return FileService.isFile('$path/Script/main.js');
  }

  void _onChangeToolChain() async {
    final settingProvider = Provider.of<SettingProvider>(
      context,
      listen: false,
    );
    final controller = TextEditingController(text: settingProvider.toolChain);
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: const Text('Toolchain'),
        actions: [
          Row(
            children: [
              Expanded(
                child: TextField(
                  controller: controller,
                  onChanged: (String value) {
                    controller.text = value;
                    settingProvider.setToolChain(controller.text);
                    _onCheckValidator();
                  },
                ),
              ),
              Tooltip(
                message: 'Upload directory',
                child: IconButton(
                  onPressed: () async {
                    final directory = await FileService.uploadDirectory();
                    if (directory == null) {
                      return;
                    }
                    controller.text = directory;
                    settingProvider.setToolChain(directory);
                    _onCheckValidator();
                  },
                  icon: const Icon(Icons.drive_folder_upload_outlined),
                ),
              ),
            ],
          ),
          const SizedBox(height: 10),
          _onCloseButton(),
        ],
      ),
    );
    controller.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final settingProvider = Provider.of<SettingProvider>(context);
    toolchainPath() => settingProvider.toolChain == ''
        ? 'Not specified'
        : settingProvider.toolChain;
    return Container(
      margin: const EdgeInsets.all(8.0),
      padding: const EdgeInsets.all(8.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          const Text('Default setting'),
          const SizedBox(height: 15),
          ListTile(
            leading: const Icon(Icons.dark_mode_outlined),
            title: const Text('Theme'),
            onTap: _onChangeTheme,
          ),
          const SizedBox(height: 10),
          ListTile(
            leading: const Icon(Icons.translate_outlined),
            title: const Text('Language'),
            onTap: () {},
          ),
          const SizedBox(height: 10),
          const ListTile(
            leading: Icon(Icons.person_2_outlined),
            title: Text('Author'),
            subtitle: Text('Haruma'),
          ),
          const Divider(),
          const Text('Application setting'),
          const SizedBox(height: 15),
          ListTile(
            leading: const Icon(Icons.notifications_outlined),
            title: const Text('Send notification'),
            onTap: _onChangeNotification,
          ),
          const SizedBox(height: 10),
          ListTile(
            leading: const Icon(Icons.storage_outlined),
            title: const Text('Storage permission'),
            subtitle:
                _hasPermission ? const Text('Granted') : const Text('Denied'),
            onTap: !_hasPermission
                ? () async {
                    await AndroidService.requestStoragePermission();
                  }
                : null,
          ),
          const SizedBox(height: 10),
          ListTile(
            leading: const Icon(Icons.build_outlined),
            title: const Text('Toolchain'),
            subtitle: Text(toolchainPath()),
            onTap: _onChangeToolChain,
          ),
        ],
      ),
    );
  }
}
