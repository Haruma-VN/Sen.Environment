// ignore_for_file: file_names

import 'package:engine/Components/Models/Controller/FilePicker.dart';
import 'package:engine/Components/View/InputField.dart';
import 'package:flutter/material.dart';

class InputFile extends StatefulWidget {
  const InputFile({
    super.key,
    required this.placeholder,
    required this.isDirectory,
    required this.controller,
  });

  final String? placeholder;

  final TextEditingController? controller;

  final bool isDirectory;

  @override
  State<InputFile> createState() => _InputFileState();
}

class _InputFileState extends State<InputFile> {
  IconData _makeIcon() {
    return widget.isDirectory
        ? Icons.upload_outlined
        : Icons.upload_file_outlined;
  }

  Future<String?> _callback() async {
    return widget.isDirectory
        ? await BasicFilePicker.pickDirectory()
        : await BasicFilePicker.pickFile();
  }

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.only(
        top: 5.0,
        bottom: 8.0,
        left: 0.0,
        right: 0.0,
      ),
      child: Row(
        children: [
          InputField(
            placeholder: widget.placeholder,
            controller: widget.controller,
            icon: IconButton(
              onPressed: () async {
                var result = await _callback();
                if (result != null) {
                  widget.controller!.text = result;
                }
                return;
              },
              tooltip: widget.placeholder,
              icon: Icon(
                _makeIcon(),
                size: 18,
              ),
            ),
          ),
        ],
      ),
    );
  }
}
