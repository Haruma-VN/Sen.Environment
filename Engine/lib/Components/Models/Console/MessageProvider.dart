// ignore_for_file: prefer_final_fields, file_names

import 'package:engine/Base/Boolean.dart';
import 'package:engine/Components/Message/MessageWrapper.dart';
import 'package:engine/api/Shell.dart';
import 'package:flutter/material.dart';

class MessageModel extends ChangeNotifier implements Shell {
  List<MessageWrapper> _messages = [];

  Boolean _isRunning = Boolean(false);

  List<MessageWrapper> get messages => _messages;

  Boolean get isRunning => _isRunning;

  @override
  void sendMessage(String message) {
    _messages.add(MessageWrapper(title: message, icon: Icons.circle_outlined));
    notify();
    return;
  }

  @override
  void clearMessage() {
    _messages.clear();
    notify();
    return;
  }

  @override
  void changeLoadingStatus() {
    _isRunning.value = !_isRunning.value;
    notify();
    return;
  }

  @override
  void notify() {
    notifyListeners();
    return;
  }

  @override
  void sendMessageWithSubtitle(String title, String message) {
    _messages.add(MessageWrapper(
        title: title, icon: Icons.circle_outlined, message: message));
    notify();
    return;
  }

  Color _exchangeColor(String color) {
    return switch (color) {
      'red' => Colors.red,
      'green' => Colors.green,
      'yellow' => Colors.yellow,
      'cyan' => Colors.cyan,
      'default' => Colors.transparent,
      _ => Colors.amber,
    };
  }

  @override
  void sendMessageWithSubtitleAndColor(
    String title,
    String message,
    String color,
  ) {
    _messages.add(
      MessageWrapper(
        title: title,
        icon: Icons.circle_outlined,
        message: message,
        color: _exchangeColor(color),
      ),
    );
    notify();
    return;
  }
}
