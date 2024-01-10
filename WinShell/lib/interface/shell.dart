// ignore_for_file: must_be_immutable

import 'package:fluent_ui/fluent_ui.dart';
import 'package:winshell/interface/application.dart';

class WinShell extends StatefulWidget {
  WinShell({
    super.key,
    required this.topIndex,
    required this.items,
  });

  int topIndex;

  List<NavigationPaneItem> items;

  @override
  State<WinShell> createState() => _WinShellState();
}

class _WinShellState extends State<WinShell> {
  @override
  Widget build(BuildContext context) {
    return NavigationView(
      appBar: const NavigationAppBar(
        title: Text(Application.app_name),
      ),
      pane: NavigationPane(
        selected: widget.topIndex,
        onChanged: (index) => setState(() => widget.topIndex = index),
        displayMode: PaneDisplayMode.open,
        items: widget.items,
        footerItems: [
          // to do : add setting
        ],
      ),
    );
  }
}
