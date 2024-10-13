import 'dart:convert';
import 'dart:io';

import 'package:file_picker/file_picker.dart';

class FileService {
  static List<String> readDirectory({
    required String source,
    required bool recursive,
  }) {
    final dir = Directory(source);
    final list = dir.listSync(recursive: recursive);
    List<String> result = [];
    for (var ripe in list) {
      result.add(ripe.path);
    }
    return result;
  }

  static isDirectory(String source) {
    return Directory(source).existsSync();
  }

  static isFile(String source) {
    return File(source).existsSync();
  }

  static void writeFile({
    required String source,
    required String data,
  }) {
    final file = File(source);
    file.writeAsStringSync(data);
    return;
  }

  static String readFile({
    required String source,
  }) {
    return File(source).readAsStringSync();
  }

  static dynamic readJson({
    required String source,
  }) {
    return jsonDecode(readFile(source: source));
  }

  static Future<String?> uploadDirectory() async {
    var directory = await FilePicker.platform.getDirectoryPath();
    if (directory == null || directory.isEmpty) {
      return null;
    }
    return directory;
  }

  static Future<String?> uploadFile() async {
    if (Platform.isAndroid) {
      // TODO : Handle this
      return null;
    }
    return _uploadFilePicker();
  }

  static Future<String?> _uploadFilePicker() async {
    var result = await FilePicker.platform.pickFiles();
    if (result == null) {
      return null;
    } else {
      return result.files.single.path!;
    }
  }
}