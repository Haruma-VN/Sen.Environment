// ignore_for_file: non_constant_identifier_names, unnecessary_this

import 'package:fluent_ui/fluent_ui.dart';

class WinShellButton extends StatelessWidget {
  const WinShellButton({
    super.key,
    required this.text,
    required this.onPressed,
    required this.iconBegin,
    required this.iconEnd,
    this.height,
    this.width,
  });
  final IconData iconBegin;
  final IconData iconEnd;
  final String text;
  final void Function() onPressed;
  final double? width, height;

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        IgnorePointer(
          child: Icon(this.iconBegin),
        ),
        SizedBox(
          width: this.width,
          height: this.height,
          child: Button(
            onPressed: this.onPressed,
            child: Text(this.text),
          ),
        ),
        IgnorePointer(
          child: Icon(this.iconEnd),
        ),
      ],
    );
  }
}
