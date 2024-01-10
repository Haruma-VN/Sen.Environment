// ignore_for_file: unnecessary_this

import 'package:fluent_ui/fluent_ui.dart';

class WinShellMessage extends StatelessWidget {
  const WinShellMessage({
    super.key,
    required this.title,
    required this.message,
    required this.severity,
  });

  final String title;

  final String message;

  final InfoBarSeverity severity;

  @override
  Widget build(BuildContext context) {
    return InfoBar(
      title: Text(this.title),
      content: Text(this.message),
      severity: this.severity,
      isLong: true,
    );
  }
}
