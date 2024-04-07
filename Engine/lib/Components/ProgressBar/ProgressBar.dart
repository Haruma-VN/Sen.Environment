// ignore_for_file: file_names

import 'package:flutter/material.dart';

class ProgressBar extends StatefulWidget {
  const ProgressBar({
    super.key,
    required this.value,
  });

  @override
  State<ProgressBar> createState() => _ProgressBarState();

  final double? value;
}

class _ProgressBarState extends State<ProgressBar> {
  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.only(
        top: 5.0,
        bottom: 5.0,
        left: 10.0,
        right: 10.0,
      ),
      child: LinearProgressIndicator(
        minHeight: 1.0,
        value: widget.value,
      ),
    );
  }
}
