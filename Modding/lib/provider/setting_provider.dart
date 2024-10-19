import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';

class SettingProvider with ChangeNotifier {
  String _theme = 'system';

  String _locale = 'en';

  bool _sendNotification = false;

  String _toolchain = '';

  bool _isValid = false;

  String get theme => _theme;

  bool get sendNotification => _sendNotification;

  String get locale => _locale;

  String get toolChain => _toolchain;

  bool get isValid => _isValid;

  bool _requestedPermission = false;

  bool get requestedPermission => _requestedPermission;

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
    _loadValid();
    _loadLocale();
    _loadRequestedPermission();
    notifyListeners();
  }

  void setTheme(String value) async {
    _theme = value;
    await _saveTheme();
    notifyListeners();
  }

  void setIsValid(bool value) async {
    _isValid = value;
    await _saveIsValid();
    notifyListeners();
  }

  void setRequestedPermission(bool value) async {
    _requestedPermission = value;
    await _saveRequestedPermission();
    notifyListeners();
  }

  void setLocale(String value) async {
    _locale = value;
    await _saveLocale();
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

  Future<void> _saveLocale() async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString('locale', _locale);
    return;
  }

  Future<void> _loadTheme() async {
    final prefs = await SharedPreferences.getInstance();
    _theme = prefs.getString('theme') ?? 'system';
    return;
  }

  Future<void> _loadLocale() async {
    final prefs = await SharedPreferences.getInstance();
    _locale = prefs.getString('locale') ?? 'en';
    return;
  }

  Future<void> _loadValid() async {
    final prefs = await SharedPreferences.getInstance();
    _isValid = prefs.getBool('isValid') ?? false;
    return;
  }

  Future<void> _loadRequestedPermission() async {
    final prefs = await SharedPreferences.getInstance();
    _requestedPermission = prefs.getBool('requestedPermission') ?? false;
    return;
  }

  Future<void> _saveToolChain() async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString('toolchain', _toolchain);
    return;
  }

  Future<void> _saveIsValid() async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setBool('isValid', _isValid);
    return;
  }

  Future<void> _saveRequestedPermission() async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setBool('requestedPermission', _requestedPermission);
    return;
  }

  Future<void> _loadToolChain() async {
    final prefs = await SharedPreferences.getInstance();
    _toolchain = prefs.getString('toolchain') ?? '';
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
    return;
  }
}
