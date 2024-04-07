// ignore_for_file: file_names

import 'package:flutter/material.dart';

class Message extends StatelessWidget {
  final Widget title;
  final double? height;
  final double? width;
  final Color baseColor;
  final EdgeInsetsGeometry? padding;

  const Message({
    Key? key,
    required this.title,
    this.height,
    this.width,
    this.padding,
    this.baseColor = Colors.teal,
  }) : super(key: key);

  @override
  Widget build(BuildContext context) {
    return Card(
      elevation: 1,
      color: Color.alphaBlend(
          baseColor.withOpacity(0.4), Theme.of(context).colorScheme.surface),
      child: Container(
        height: height,
        width: width,
        padding: padding,
        alignment: Alignment.centerLeft,
        child: Container(
          margin: const EdgeInsets.only(
            left: 10.0,
            top: 2.0,
          ),
          child: SingleChildScrollView(
            child: title,
          ),
        ),
      ),
    );
  }
}
