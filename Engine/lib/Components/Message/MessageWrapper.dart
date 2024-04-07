// ignore_for_file: file_names

import 'package:flutter/material.dart';

class MessageWrapper {
  final String title;
  final String? message;
  final Color? color;
  final IconData icon;

  const MessageWrapper({
    required this.title,
    required this.icon,
    this.color,
    this.message,
  });
}
