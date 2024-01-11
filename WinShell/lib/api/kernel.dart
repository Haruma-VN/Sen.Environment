// ignore_for_file: unused_import, unnecessary_this, prefer_is_empty, avoid_print

import 'dart:convert';
import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:fluent_ui/fluent_ui.dart';
import 'package:winshell/api/define.dart';
import 'package:winshell/api/converter.dart';

class Kernel {
  late DynamicLibrary dylib;

  static Pointer<StringView> debug(
    Pointer<StringList> list,
  ) {
    var result = CStringConverter.toList(list);
    assert(result.length >= 1, "result must be greater or equals 1");
    switch (result[0]) {
      case 'input':
        {
          print("input");
          break;
        }
      case 'display':
        {
          print('display');
          break;
        }
      case 'version':
        {
          return CStringConverter.toStringView('1');
        }
      case 'is_gui':
        {
          return CStringConverter.toStringView('1');
        }
    }
    return CStringConverter.toStringView('');
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
    calloc.free(script);
    return result;
  }
}
