// ignore_for_file: file_names, must_be_immutable

import 'package:engine/Components/Models/Console/MessageProvider.dart';
import 'package:engine/Components/Models/Console/State.dart';
import 'package:engine/Api/Kernel.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:engine/Components/ProgressBar/ProgressBar.dart';

class Launcher extends StatefulWidget {
  Launcher({
    super.key,
    required this.kernel,
  });

  final Kernel kernel;

  final textEditingController = TextEditingController();

  static Function()? onPressed;

  @override
  State<Launcher> createState() => _LauncherState();
}

class _LauncherState extends State<Launcher> {
  Widget _exchangeState(MyState state) {
    switch (state) {
      case MyState.launch:
        {
          return const Text('LAUNCH');
        }
      case MyState.finish:
        {
          return const Text('FINISHED');
        }
      default:
        {
          return Container();
        }
    }
  }

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
                onPressed: !(Provider.of<MessageModel>(context, listen: true)
                        .isRunning
                        .value)
                    ? () {
                        setState(() {
                          widget.kernel.run(
                              "D:/Code/Sen.Environment/Script/build/main.js",
                              "d",
                              ["D:/打包开始/main.850.com.ea.game.pvz2_row.obb"]);
                        });
                      }
                    : () {
                        setState(() {
                          Provider.of<MessageModel>(context, listen: false)
                              .isRunning
                              .value = false;
                        });
                      },
                child: Container(
                  padding: const EdgeInsets.all(16.0),
                  child: Row(
                    children: [
                      _exchangeState(Provider.of<MessageModel>(context).state)
                    ],
                  ),
                ),
              ),
            )
          ],
        )
      ],
    );
  }
}
