import 'package:flutter/material.dart';
import 'package:modding/model/log_message.dart';

class LogProvider with ChangeNotifier {
  final List<LogMessage> _list = [];

  List<LogMessage> get list => _list;

  void empty() {
    _list.clear();
    notifyListeners();
  }

  void add(LogMessage message) {
    _list.add(message);
    notifyListeners();
  }
}
