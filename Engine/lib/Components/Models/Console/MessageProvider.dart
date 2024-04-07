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
    notifyListeners();
    return;
  }

  @override
  void clearMessage() {
    _messages.clear();
    notifyListeners();
    return;
  }

  @override
  void changeLoadingStatus() {
    _isRunning.value = !_isRunning.value;
    notifyListeners();
    return;
  }
}
