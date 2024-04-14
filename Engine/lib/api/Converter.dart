// ignore_for_file: unused_import, file_names

import 'dart:ffi';
import 'dart:typed_data';
import 'package:engine/api/Interface.dart';
import 'package:ffi/ffi.dart';
import 'dart:convert' as convert;

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

  static Pointer<Utf8> utf8ArrayToCString(Uint8List units) {
    final Pointer<Uint8> result = calloc<Uint8>(units.length + 1);
    final Uint8List list = result.asTypedList(units.length + 1);
    list.setAll(0, units);
    list[units.length] = 0;
    return result.cast<Utf8>();
  }

  static Uint8List toUint8List(String str) {
    return convert.utf8.encode(str);
  }
}
