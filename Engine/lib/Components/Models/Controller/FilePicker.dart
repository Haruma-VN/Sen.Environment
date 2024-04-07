// ignore_for_file: file_names

import 'package:file_picker/file_picker.dart';

class BasicFilePicker {
  static Future<String?> pickFile() async {
    var result = await FilePicker.platform.pickFiles();
    if (result != null) {
      return result.files.single.path!;
    } else {
      return null;
    }
  }

  static Future<String?> pickDirectory() async {
    return await FilePicker.platform.getDirectoryPath();
  }
}
