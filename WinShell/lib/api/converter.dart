// ignore_for_file: unused_import

import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:winshell/api/interface.dart';

class CStringConverter {
  /// Convert C String to Dart String

  static String toDartString(Pointer<CStringView> stringview) {
    var charCodes =
        stringview.ref.value.cast<Uint8>().asTypedList(stringview.ref.size);
    return String.fromCharCodes(charCodes);
  }

  /// Convert CStringList to DartList

  static List<String> toList(CStringList list) {
    var result = <String>[];
    for (var i = 0; i < list.size; ++i) {
      result.add(list.value.elementAt(i).ref.value.toDartString());
    }
    return result;
  }

  /// Convert Dart String to CStringView

  static Pointer<CStringView> toStringView(String str) {
    var stringView = calloc<CStringView>();
    stringView.ref
      ..value = str.toNativeUtf8()
      ..size = str.length;
    return stringView;
  }
}
