// ignore_for_file: unnecessary_this

import 'package:fluent_ui/fluent_ui.dart';

class WinShellInputField extends StatelessWidget {
  const WinShellInputField({
    super.key,
    required this.label,
    required this.placeHolder,
    required this.controller,
  });

  final String label;
  final String placeHolder;
  final TextEditingController controller;

  @override
  Widget build(BuildContext context) {
    return InfoLabel(
      label: label,
      child: TextBox(
        controller: this.controller,
        placeholder: this.placeHolder,
        expands: false,
      ),
    );
  }
}

class WinShellInputFieldWithIcon extends WinShellInputField {
  const WinShellInputFieldWithIcon({
    super.key,
    required super.label,
    required super.placeHolder,
    required super.controller,
    required this.iconBegin,
    required this.iconEnd,
  });

  final IconData iconBegin;

  final IconData iconEnd;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(10.0),
      child: Row(
        crossAxisAlignment: CrossAxisAlignment.center,
        children: [
          IgnorePointer(
            child: Padding(
              padding: const EdgeInsets.fromLTRB(0.0, 20.0, 0.0, 0.0),
              child: Icon(
                this.iconBegin,
              ),
            ),
          ),
          const SizedBox(
            width: 10.0,
          ),
          Flexible(
            child: super.build(context),
          ),
          const SizedBox(
            width: 10.0,
          ),
          IgnorePointer(
            child: Padding(
              padding: const EdgeInsets.fromLTRB(0.0, 20.0, 0.0, 0.0),
              child: Icon(
                this.iconEnd,
              ),
            ),
          ),
        ],
      ),
    );
  }
}
