// ignore_for_file: file_names

import 'package:engine/Components/Models/Console/MessageProvider.dart';
import 'package:engine/api/Kernel.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:engine/Components/ProgressBar/ProgressBar.dart';

class Launcher extends StatefulWidget {
  const Launcher({
    super.key,
    required this.kernel,
  });

  final Kernel kernel;

  @override
  State<Launcher> createState() => _LauncherState();
}

class _LauncherState extends State<Launcher> {
  int index = 0;

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        ProgressBar(
          value:
              Provider.of<MessageModel>(context).isRunning.value ? null : 0.0,
        ),
        Row(
          children: [
            Expanded(
              child: ElevatedButton(
                onPressed: () {
                  setState(() {
                    widget.kernel.run(
                        "D:/Code/Sen.Environment/Script/build/main.js",
                        "d", []);
                    ++index;
                  });
                },
                child: Container(
                  padding: const EdgeInsets.all(16.0),
                  child: Provider.of<MessageModel>(context).isRunning.value
                      ? const Text("FINISHED")
                      : const Text("LAUNCH"),
                ),
              ),
            )
          ],
        )
      ],
    );
  }
}
