import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

class AndroidService {
  static const MethodChannel _methodChannel = MethodChannel(
    'com.haruma.sen.environment',
  );

  static Future<bool> requestStoragePermission() async {
    final result = await _methodChannel.invokeMethod<bool>(
      'request_storage_permission',
    );
    return result!;
  }

  static Future<bool> checkStoragePermission() async {
    final result = await _methodChannel.invokeMethod<bool>(
      'check_storage_permission',
    );
    return result!;
  }

  static Future<String?> pickFileFromDocument() async {
    final result = await _methodChannel.invokeMethod<String?>(
      'pick_file',
    );
    return result!;
  }

  static Future<String?> pickDirectoryFromDocument() async {
    final result = await _methodChannel.invokeMethod<String?>(
      'pick_directory',
    );
    return result!;
  }
}
