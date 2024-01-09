// ignore_for_file: unnecessary_this

import 'package:fluent_ui/fluent_ui.dart';

class InputField extends StatelessWidget {
  const InputField({
    super.key,
    required this.label,
    required this.placeHolder,
  });

  final String label;
  final String placeHolder;

  @override
  Widget build(BuildContext context) {
    return InfoLabel(
      label: label,
      child: TextBox(
        placeholder: placeHolder,
        expands: false,
      ),
    );
  }
}

class InputFieldWithIcon extends InputField {
  const InputFieldWithIcon({
    super.key,
    required super.label,
    required super.placeHolder,
    required this.iconBegin,
    required this.iconEnd,
  });

  final IconData iconBegin;

  final IconData iconEnd;

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        IgnorePointer(
          child: Icon(this.iconBegin),
        ),
        super.build(context),
        IgnorePointer(
          child: Icon(this.iconEnd),
        ),
      ],
    );
  }
}
