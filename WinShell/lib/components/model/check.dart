// ignore_for_file: unnecessary_this

import 'package:fluent_ui/fluent_ui.dart';

class WinShellCheckBox extends StatelessWidget {
  const WinShellCheckBox({
    super.key,
    required this.value,
    required this.onPressed,
  });

  final bool value;

  final void Function(bool? value) onPressed;

  @override
  Widget build(BuildContext context) {
    return Checkbox(
      checked: this.value,
      onChanged: this.onPressed,
    );
  }
}
