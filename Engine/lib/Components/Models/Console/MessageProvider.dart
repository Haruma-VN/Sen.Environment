// ignore_for_file: prefer_final_fields, file_names

import 'dart:ffi';

import 'package:engine/Api/Interface.dart';
import 'package:engine/Base/Boolean.dart';
import 'package:engine/Components/Message/MessageWrapper.dart';
import 'package:engine/Components/Models/Console/State.dart';
import 'package:engine/Api/Shell.dart';
import 'package:flutter/material.dart';

class MessageModel extends ChangeNotifier implements Shell {
  List<MessageWrapper> _messages = [];

  Boolean _isRunning = Boolean(false);

  List<MessageWrapper> get messages => _messages;

  Boolean get isRunning => _isRunning;

  MyState _state = MyState.launch;

  MyState get state => _state;

  set state(value) => _state = value;

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
  void pushNotification(String message) {
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

  @override
  void setFinishedState() {
    _state = MyState.launch;
    notify();
    return;
  }

  @override
  void inputStringState() {
    _state = MyState.inputString;
    notify();
    return;
  }

  @override
  Future<void> execute(Pointer<CStringView> arg) async {
    notify();
  }

  @override
  void setIdle() {
    _state = MyState.idle;
    notify();
    return;
  }

  @override
  Future<void> inputEnumeration(
    Pointer<CStringView> dest,
    List<String> arguments,
  ) async {
    _state = MyState.inputEnumeration;
    notify();
    return;
  }

  @override
  Future<void> inputBoolean(Pointer<CStringView> dest) async {
    _state = MyState.inputBoolean;
    notify();
    return;
  }
}
