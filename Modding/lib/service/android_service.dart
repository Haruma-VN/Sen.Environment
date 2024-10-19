import 'package:flutter/services.dart';

class AndroidService {
  static const MethodChannel _methodChannel = MethodChannel(
    'com.haruma.sen.environment',
  );

  static List<String>? _arguments;

  static List<String>? get arguments => _arguments;

  static void initialize() {
    _methodChannel.setMethodCallHandler(_handleMethod);
  }

  static Future<void> _handleMethod(
    MethodCall call,
  ) async {
    switch (call.method) {
      case 'onResourcesReceived':
        _arguments = List<String>.from(call.arguments);
        break;
      default:
        throw UnimplementedError();
    }
  }

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
