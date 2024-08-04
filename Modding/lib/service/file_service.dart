import 'dart:io';

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
}
