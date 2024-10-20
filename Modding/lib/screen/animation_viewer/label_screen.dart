import 'package:flutter/material.dart';
import 'package:material_symbols_icons/material_symbols_icons.dart';
import 'package:modding/screen/animation_viewer/visual_helper.dart';

// ignore: must_be_immutable
class LabelScreen extends StatefulWidget {
  LabelScreen({
    super.key,
    required this.label,
  });

  final List<String> label;

  void Function()? updateUI;

  @override
  State<LabelScreen> createState() => _LabelScreenState();
}

class _LabelScreenState extends State<LabelScreen> {
  @override
  Widget build(BuildContext context) {
    return ListView.builder(
      itemCount: widget.label.length,
      itemBuilder: (context, index) => Tooltip(
        message: widget.label[index],
        child: Card(
          child: ListTile(
            leading: const Icon(Symbols.animation),
            trailing: IconButton(
              icon: const Icon(Symbols.play_arrow),
              onPressed: () {
                setState(() {
                  VisualHelper.currentLabel = widget.label[index];
                });
                widget.updateUI?.call();
              },
            ),
            title: Text(widget.label[index]),
          ),
        ),
      ),
    );
  }
}
