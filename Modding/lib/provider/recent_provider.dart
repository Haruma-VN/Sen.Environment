import 'package:flutter/material.dart';
import 'package:shared_preferences/shared_preferences.dart';

class RecentProvider with ChangeNotifier {
  List<String> _recentFiles = [];

  List<String> get recentFiles => _recentFiles;

  RecentProvider() {
    _loadRecentFiles();
  }

  void addFile(String filePath) async {
    if (!_recentFiles.contains(filePath)) {
      _recentFiles.add(filePath);
      await _saveRecentFiles();
      notifyListeners();
    }
    return;
  }

  void removeFile(String filePath) async {
    _recentFiles.remove(filePath);
    await _saveRecentFiles();
    notifyListeners();
    return;
  }

  void clearFiles() async {
    _recentFiles.clear();
    await _saveRecentFiles();
    notifyListeners();
    return;
  }

  Future<void> _saveRecentFiles() async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setStringList('recentFiles', _recentFiles);
    return;
  }

  Future<void> _loadRecentFiles() async {
    final prefs = await SharedPreferences.getInstance();
    _recentFiles = prefs.getStringList('recentFiles') ?? [];
    notifyListeners();
    return;
  }
}
