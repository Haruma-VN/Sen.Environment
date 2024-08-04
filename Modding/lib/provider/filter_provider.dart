import 'package:flutter/material.dart';

class FilterProvider with ChangeNotifier {
  String _value = 'disabled';

  String get value => _value;

  set value(String v) {
    _value = v;
    notifyListeners();
  }
}
