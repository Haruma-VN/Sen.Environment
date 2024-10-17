import 'dart:convert';
import 'dart:io';
import 'dart:typed_data';

import 'package:file_picker/file_picker.dart';
import 'package:modding/service/android_service.dart';

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

  static Uint8List readBuffer({
    required String source,
  }) {
    var file = File(source);
    return file.readAsBytesSync();
  }

  static void writeBuffer({
    required String source,
    required Uint8List data,
  }) {
    var file = File(source);
    file.writeAsBytesSync(data);
    return;
  }

  static dynamic readJson({
    required String source,
  }) {
    return jsonDecode(readFile(source: source));
  }

  static Future<String?> uploadDirectory() async {
    var directory = null as String?;
    if (Platform.isAndroid) {
      directory = await AndroidService.pickDirectoryFromDocument();
    } else {
      directory = await FilePicker.platform.getDirectoryPath();
    }
    if (directory == null || directory.isEmpty) {
      return null;
    }
    return directory;
  }

  static Future<String?> uploadFile() async {
    if (Platform.isAndroid) {
      return await AndroidService.pickFileFromDocument();
    }
    return await _uploadFilePicker();
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
