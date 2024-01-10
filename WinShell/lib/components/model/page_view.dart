// ignore_for_file: unnecessary_this

import 'package:fluent_ui/fluent_ui.dart';

class WinShellPageView extends StatelessWidget {
  const WinShellPageView({
    super.key,
    required this.text,
    required this.child,
  });

  final Widget child;

  final String text;

  @override
  Widget build(BuildContext context) {
    return SizedBox(
      width: 100.0,
      child: Button(
        child: Text(this.text),
        onPressed: () {
          Navigator.of(context).push(
            FluentDialogRoute(
              builder: (context) => this.child,
              context: context,
            ),
          );
        },
      ),
    );
  }
}
