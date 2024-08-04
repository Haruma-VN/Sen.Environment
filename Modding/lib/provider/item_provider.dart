import 'package:flutter/material.dart';

class ItemProvider with ChangeNotifier {
  String _path = '';

  String get path => _path;

  set path(String value) {
    _path = value;
    notifyListeners();
  }
}
