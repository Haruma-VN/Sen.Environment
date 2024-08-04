import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';

class SettingProvider with ChangeNotifier {
  String _theme = 'system';

  bool _sendNotification = false;

  String _toolchain = '';

  String get theme => _theme;

  bool get sendNotification => _sendNotification;

  String get toolChain => _toolchain;

  ThemeMode get themeData {
    final Map<String, ThemeMode> exchanger = {
      'system': ThemeMode.system,
      'dark': ThemeMode.dark,
      'light': ThemeMode.light,
    };
    return exchanger[_theme] ?? ThemeMode.system;
  }

  SettingProvider() {
    _loadTheme();
    _loadOnNotification();
    _loadToolChain();
  }

  void setTheme(String value) async {
    _theme = value;
    await _saveTheme();
    notifyListeners();
  }

  void setNotification(bool value) async {
    _sendNotification = value;
    await _saveOnNotification();
    notifyListeners();
  }

  void setToolChain(String value) async {
    _toolchain = value;
    await _saveToolChain();
    notifyListeners();
  }

  Future<void> _saveTheme() async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString('theme', _theme);
    return;
  }

  Future<void> _loadTheme() async {
    final prefs = await SharedPreferences.getInstance();
    _theme = prefs.getString('theme') ?? 'system';
    notifyListeners();
    return;
  }

  Future<void> _saveToolChain() async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString('toolchain', _toolchain);
    return;
  }

  Future<void> _loadToolChain() async {
    final prefs = await SharedPreferences.getInstance();
    _toolchain = prefs.getString('toolchain') ?? '';
    notifyListeners();
    return;
  }

  Future<void> _saveOnNotification() async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setBool('sendNotification', _sendNotification);
    return;
  }

  Future<void> _loadOnNotification() async {
    final prefs = await SharedPreferences.getInstance();
    _sendNotification = prefs.getBool('sendNotification') ?? false;
    notifyListeners();
    return;
  }
}
