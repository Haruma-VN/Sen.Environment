import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:modding/provider/setting_provider.dart';
import 'package:provider/provider.dart';

class SettingScreen extends StatefulWidget {
  const SettingScreen({super.key});

  @override
  State<SettingScreen> createState() => _SettingScreenState();
}

class _SettingScreenState extends State<SettingScreen> {
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
          if (value == null) return;
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
                    settingProvider.setToolChain(value);
                  },
                ),
              ),
              Tooltip(
                message: 'Upload directory',
                child: IconButton(
                  onPressed: () async {
                    var directory =
                        await FilePicker.platform.getDirectoryPath();
                    if (directory == null || directory.isEmpty) {
                      return;
                    }
                    controller.text = directory;
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
    return Column(
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
          onTap: () {},
        ),
        const SizedBox(height: 10),
        ListTile(
          leading: const Icon(Icons.build_outlined),
          title: const Text('Toolchain'),
          subtitle: Text(toolchainPath()),
          onTap: _onChangeToolChain,
        ),
      ],
    );
  }
}
