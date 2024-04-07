// ignore_for_file: file_names

import 'package:flutter/material.dart';

class InputField extends StatefulWidget {
  const InputField({
    super.key,
    required this.placeholder,
    this.controller,
    this.icon,
  });

  final String? placeholder;

  final Widget? icon;

  final TextEditingController? controller;

  @override
  State<InputField> createState() => _InputFieldState();
}

class _InputFieldState extends State<InputField> {
  @override
  void dispose() {
    super.dispose();
    if (widget.controller != null) {
      widget.controller!.dispose();
    }
    return;
  }

  @override
  Widget build(BuildContext context) {
    return Expanded(
      child: Padding(
        padding: const EdgeInsets.only(
          left: 10.0,
          right: 16.0,
          bottom: 2.0,
        ),
        child: TextField(
          controller: widget.controller,
          decoration: InputDecoration(
            border: const OutlineInputBorder(),
            labelText: widget.placeholder,
            suffixIcon: widget.icon,
          ),
        ),
      ),
    );
  }
}
