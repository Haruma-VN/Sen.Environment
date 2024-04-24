// ignore_for_file: unused_import, file_names

import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:engine/Api/Interface.dart';

abstract class Shell {
  // API interact
  void sendMessage(String message);

  void sendMessageWithSubtitle(String title, String message);

  void sendMessageWithSubtitleAndColor(
      String title, String message, String color);

  void clearMessage();

  void changeLoadingStatus();

  void notify();

  void pushNotification(String message);

  void setFinishedState();

  void inputStringState();

  Future<void> execute(Pointer<CStringView> arg);

  Future<void> inputEnumeration(
    Pointer<CStringView> dest,
    List<String> arguments,
  );

  Future<void> inputBoolean(
    Pointer<CStringView> dest,
  );

  void setIdle();
}
