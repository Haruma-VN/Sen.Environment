import 'dart:ffi';
import 'dart:typed_data';
import 'package:ffi/ffi.dart';
import 'package:modding/model/api.dart';
import 'dart:convert' as convert;

class PointerService {
  static String toDartString(Pointer<CStringView> str) {
    var charCodes = str.ref.value.cast<Uint8>().asTypedList(str.ref.size);
    return String.fromCharCodes(charCodes);
  }

  static List<String> toList(CStringList list) {
    var result = <String>[];
    for (var i = 0; i < list.size; ++i) {
      result.add((list.value + i).ref.value.toDartString());
    }
    return result;
  }

  static Pointer<Utf8> utf8ListToCString(Uint8List units) {
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
