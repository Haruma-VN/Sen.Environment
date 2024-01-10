// ignore_for_file: unnecessary_this

import 'package:fluent_ui/fluent_ui.dart';

class WinShellInfoField extends StatelessWidget {
  const WinShellInfoField({
    super.key,
    required this.header,
    required this.child,
  });

  final String header;

  final String child;

  @override
  Widget build(BuildContext context) {
    return Expander(
      header: Text(this.header),
      content: SizedBox(
        height: 300,
        child: SingleChildScrollView(
          child: Text(this.child),
        ),
      ),
    );
  }
}
