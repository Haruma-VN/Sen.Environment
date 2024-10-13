import 'package:flutter/material.dart';
import 'package:material_symbols_icons/material_symbols_icons.dart';

class LabelScreen extends StatefulWidget {
  const LabelScreen({
    super.key,
    required this.label,
  });

  final List<String> label;

  @override
  State<LabelScreen> createState() => _LabelScreenState();
}

class _LabelScreenState extends State<LabelScreen> {
  @override
  Widget build(BuildContext context) {
    return ListView.builder(
      itemCount: widget.label.length,
      itemBuilder: (context, index) => Card(
        child: ListTile(
          leading: const Icon(Symbols.animation),
          trailing: IconButton(
            icon: const Icon(Symbols.play_arrow),
            onPressed: () {
              // TODO : Add on play event
            },
          ),
          title: Text(widget.label[index]),
        ),
      ),
    );
  }
}
