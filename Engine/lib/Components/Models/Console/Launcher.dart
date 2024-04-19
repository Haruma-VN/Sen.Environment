// ignore_for_file: file_names, must_be_immutable, use_build_context_synchronously, avoid_init_to_null

import 'dart:async';

import 'package:engine/Components/Models/Console/MessageProvider.dart';
import 'package:engine/Components/Models/Console/State.dart';
import 'package:engine/Api/Kernel.dart';
import 'package:engine/Components/Models/Controller/FilePicker.dart';
import 'package:engine/Components/View/DropDownList.dart';
import 'package:engine/Components/View/InputField.dart';
import 'package:engine/Components/View/RadioButton.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
import 'package:engine/Components/ProgressBar/ProgressBar.dart';

class Launcher extends StatefulWidget {
  Launcher({
    super.key,
    required this.kernel,
    this.completer,
  });

  final Kernel kernel;

  final textEditingController = TextEditingController();

  Completer? completer = null;

  static List<String>? enumeration = null;

  @override
  State<Launcher> createState() => _LauncherState();
}

class _LauncherState extends State<Launcher> {
  Widget _exchangeState(MyState state) {
    switch (state) {
      case MyState.launch:
        {
          return const Center(
            child: Text('LAUNCH'),
          );
        }
      case MyState.finish:
        {
          return const Text('FINISHED');
        }
      case MyState.inputString:
        {
          return Expanded(
            child: Row(
              children: [
                InputField(
                  controller: widget.textEditingController,
                  placeholder: 'Input...',
                ),
                IconButton(
                  onPressed: () async {
                    showDialog(
                      context: context,
                      builder: (BuildContext context) {
                        return AlertDialog(
                          title: const Text('Option'),
                          content: Row(children: [
                            TextButton(
                              onPressed: () async {
                                var result = await BasicFilePicker.pickFile();
                                if (result != null) {
                                  widget.textEditingController.text = result;
                                }
                                Navigator.of(context).pop();
                              },
                              child: const Text('FILE'),
                            ),
                            TextButton(
                              onPressed: () async {
                                var result =
                                    await BasicFilePicker.pickDirectory();
                                if (result != null) {
                                  widget.textEditingController.text = result;
                                }
                                Navigator.of(context).pop();
                              },
                              child: const Text('DIRECTORY'),
                            )
                          ]),
                        );
                      },
                    );
                  },
                  icon: const Icon(Icons.file_upload_outlined),
                ),
                IconButton(
                  onPressed: () {
                    var value = widget.textEditingController.text;
                    widget.textEditingController.text = '';
                    widget.completer!.complete(value);
                  },
                  icon: const Icon(Icons.send_outlined),
                ),
              ],
            ),
          );
        }
      case MyState.inputEnumeration:
        {
          var value = Launcher.enumeration![0];
          var list = DropDownListView(
            items: Launcher.enumeration!,
            value: value,
          );
          return Expanded(
              child: Row(
            children: [
              Expanded(child: list),
              IconButton(
                onPressed: () {
                  var value = list.value;
                  widget.completer!.complete(value);
                },
                icon: const Icon(Icons.send_outlined),
              ),
            ],
          ));
        }
      case MyState.inputBoolean:
        {
          var radioButton = RadioButton(
            options: const ['YES', 'NO'],
            isRowProvider: true,
          );
          return Expanded(
              child: Row(
            children: [
              Expanded(child: radioButton),
              IconButton(
                onPressed: () {
                  var value = radioButton.currentOption;
                  widget.completer!.complete(value == 'YES' ? '1' : '2');
                },
                icon: const Icon(Icons.send_outlined),
              ),
            ],
          ));
        }
      case MyState.idle:
        {
          return Container();
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
                onPressed:
                    (Provider.of<MessageModel>(context, listen: true).state ==
                            MyState.launch)
                        ? () {
                            setState(() {
                              widget.kernel.run(
                                  "D:/Code/Sen.Environment/Script/build/main.js",
                                  "d", []);
                            });
                          }
                        : null,
                child: Container(
                  padding: Provider.of<MessageModel>(context, listen: true)
                          .isRunning
                          .value
                      ? const EdgeInsets.all(4.0)
                      : const EdgeInsets.all(16.0),
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
