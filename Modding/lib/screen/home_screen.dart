import 'package:delightful_toast/delight_toast.dart';
import 'package:delightful_toast/toast/components/toast_card.dart';
import 'package:delightful_toast/toast/utils/enums.dart';
import 'package:flutter/material.dart';
import 'package:modding/model/log_message.dart';
import 'package:modding/provider/item_provider.dart';
import 'package:modding/provider/log_provider.dart';
import 'package:modding/provider/recent_provider.dart';
import 'package:provider/provider.dart';
import 'package:file_picker/file_picker.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  late TextEditingController _moddingDirectoryController;

  @override
  void initState() {
    _moddingDirectoryController = TextEditingController();
    super.initState();
  }

  @override
  void dispose() {
    _moddingDirectoryController.dispose();
    super.dispose();
  }

  Future<void> _onUpload() async {
    final logProvider = Provider.of<LogProvider>(context, listen: false);
    final provider = Provider.of<ItemProvider>(context, listen: false);
    final recentProvider = Provider.of<RecentProvider>(context, listen: false);
    final result = await FilePicker.platform.getDirectoryPath();
    logProvider.empty();
    logProvider.add(
        LogMessage(title: 'Upload modding directory', time: DateTime.now()));
    if (result != null) {
      provider.path = result;
      _showSnackBar(
        title: 'loaded: $result',
        color: Colors.green.withOpacity(0.85),
      );
      logProvider.add(LogMessage(
        title: 'Uploaded modding directory',
        subtitle: result,
        time: DateTime.now(),
      ));
      recentProvider.addFile(result);
    } else {
      logProvider.add(
        LogMessage(
          title: 'Failed upload modding directory',
          time: DateTime.now(),
        ),
      );
    }
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

  List<Widget> _moddingDirectory() {
    return [
      const Text('Modding directory'),
      Row(
        children: [
          Expanded(
            child: TextField(
              controller: _moddingDirectoryController,
            ),
          ),
          IconButton(
            onPressed: _onUpload,
            icon: const Icon(Icons.upload_outlined),
          ),
        ],
      )
    ];
  }

  Widget? _subtitle(LogMessage e) {
    return e.subtitle != null ? Text(e.subtitle!) : null;
  }

  @override
  Widget build(BuildContext context) {
    final itemProvider = Provider.of<ItemProvider>(context);
    final logProvider = Provider.of<LogProvider>(context);
    _moddingDirectoryController.text = itemProvider.path;
    return Padding(
      padding: const EdgeInsets.all(8.0),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          ..._moddingDirectory(),
          const SizedBox(height: 15.0),
          Expanded(
            child: Container(
              decoration: BoxDecoration(
                borderRadius: BorderRadius.circular(10),
                border: Border.all(color: Colors.blueAccent),
              ),
              child: ListView(
                children: logProvider.list
                    .map(
                      (e) => Card(
                        child: ListTile(
                          leading: const Icon(Icons.work_history_outlined),
                          title: Text(e.title),
                          subtitle: _subtitle(e),
                        ),
                      ),
                    )
                    .toList(),
              ),
            ),
          ),
        ],
      ),
    );
  }
}
