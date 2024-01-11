// ignore_for_file: unused_import

import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:winshell/api/define.dart';

class CStringConverter {
  static String toDartString(Pointer<StringView> stringview) {
    var charCodes =
        stringview.ref.value.cast<Uint8>().asTypedList(stringview.ref.size);
    return String.fromCharCodes(charCodes);
  }

  static List<String> toList(StringList list) {
    var result = <String>[];
    for (var i = 0; i < list.size; i++) {
      result.add(list.value.elementAt(i).ref.value.toDartString());
    }
    return result;
  }

  static Pointer<StringView> toStringView(String str) {
    var units = str.toNativeUtf8();
    var size = str.length;
    var stringView = calloc<StringView>();
    stringView.ref
      ..value = units
      ..size = size;

    return stringView;
  }
}
