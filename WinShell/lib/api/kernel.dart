// ignore_for_file: unused_import, unnecessary_this, prefer_is_empty, avoid_print

import 'dart:convert';
import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:fluent_ui/fluent_ui.dart';
import 'package:winshell/api/define.dart';
import 'package:winshell/api/converter.dart';

class Kernel {
  late DynamicLibrary dylib;

  static StringView debug(
    StringList list,
  ) {
    var result = CStringConverter.toList(list);
    assert(result.length >= 1, "result must be greater or equals 1");
    switch (result[0]) {
      case 'display':
        {
          assert(result.length >= 2, "argument must be greater than 2");
          switch (result.length) {
            case 2:
              {
                print(result[1]);
                break;
              }
            case 3:
              {
                print(result[1]);
                if (result[2] != '') {
                  print(result[2]);
                }
                break;
              }
            case 4:
              {
                print(result[1]);
                if (result[2] != '') {
                  print(result[2]);
                }
                break;
              }
          }
          break;
        }
      case 'input':
        {
          print('input');
          break;
        }
      case 'version':
        {
          var versionPointer = CStringConverter.toStringView('1');
          var reference = versionPointer.ref;
          calloc.free(versionPointer);
          versionPointer = nullptr;
          return reference;
        }
      case 'is_gui':
        {
          var guiPointer = CStringConverter.toStringView('1');
          var reference = guiPointer.ref;
          calloc.free(guiPointer);
          guiPointer = nullptr;
          return reference;
        }
    }
    var unknownPointer = CStringConverter.toStringView('');
    var reference = unknownPointer.ref;
    calloc.free(unknownPointer);
    unknownPointer = nullptr;
    return reference;
  }

  Kernel() {
    this.dylib = DynamicLibrary.open(
      'D:/Code/Sen.Environment/Kernel/build/kernel/Release/kernel.dll',
    );
  }

  Kernel.open(path) {
    this.dylib = DynamicLibrary.open(path);
  }

  int execute() {
    const String scriptPath = 'D:/Code/Sen.Environment/Script/build/main.js';
    final KernelExecuteDartAPI executeMethod = this
        .dylib
        .lookupFunction<KernelExecuteCAPI, KernelExecuteDartAPI>('execute');
    Pointer<StringView> script = calloc<StringView>(scriptPath.length);
    script.ref
      ..value = scriptPath.toNativeUtf8()
      ..size = scriptPath.length;
    int result = 0;
    try {
      result = executeMethod(
        script,
        Pointer.fromFunction(Kernel.debug),
      );
    } catch (e) {
      print(e);
      debugPrintStack();
    }
    calloc.free(script.ref.value);
    calloc.free(script);
    return result;
  }
}
