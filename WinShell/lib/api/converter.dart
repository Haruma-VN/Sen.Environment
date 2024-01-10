// ignore_for_file: unused_import

import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:winshell/api/define.dart';

class CStringConverter {
  static String toDartString(Pointer<CStringView> stringview) {
    var charCodes =
        stringview.ref.value.cast<Uint8>().asTypedList(stringview.ref.size);
    return String.fromCharCodes(charCodes);
  }

  static List<String> toList(Pointer<CStringList> list) {
    var result = <String>[];
    for (var i = 0; i < list.ref.size; i++) {
      result.add(list.ref.value.elementAt(i).ref.value.toDartString());
    }
    return result;
  }

  static Pointer<CStringView> toCStringView(String str) {
    var units = str.toNativeUtf8();
    var size = str.length;
    var cstringview = calloc<CStringView>();
    cstringview.ref
      ..value = units
      ..size = size;

    return cstringview;
  }
}
