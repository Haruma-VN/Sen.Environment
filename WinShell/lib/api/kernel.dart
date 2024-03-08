// ignore_for_file: unused_import, unnecessary_this, prefer_is_empty, avoid_print, no_leading_underscores_for_local_identifiers, constant_identifier_names, non_constant_identifier_names

import 'dart:convert';
import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:fluent_ui/fluent_ui.dart';
import 'package:winshell/api/interface.dart';
import 'package:winshell/api/converter.dart';
import 'package:winshell/api/shell.dart';
import 'package:winshell/interface/test.dart';

class Kernel {
  static const version = 1;

  late DynamicLibrary dylib;

  static late Shell gui;

  static void debug(
    Pointer<CStringList> list,
    Pointer<CStringView> destination,
    Pointer<Never> proxy,
  ) {
    var result = CStringConverter.toList(list.ref);
    debugPrint(result.toString());
    assert(result.length >= 1, "result must be greater or equals 1");
    switch (result[0]) {
      case 'display':
        {
          assert(result.length >= 2, "argument must be greater than 2");
          switch (result.length) {
            case 2:
              {
                gui.sendMessage(result[1]);
                break;
              }
            case 3:
              {
                gui.sendMessage(result[1]);
                if (result[2] != '') {
                  gui.sendMessage(result[2]);
                }
                break;
              }
            case 4:
              {
                gui.sendMessage(result[1]);
                if (result[2] != '') {
                  gui.sendMessage(result[2]);
                }
                break;
              }
          }
          break;
        }
      case 'input':
        {
          gui.sendMessage('input');
          print('input');
          break;
        }
      case 'version':
        {
          var _version = version.toString();
          destination.ref
            ..size = _version.length
            ..value = _version.toNativeUtf8();
          return;
        }
      case 'is_gui':
        {
          var _is_gui = 1.toString();
          destination.ref
            ..size = _is_gui.length
            ..value = _is_gui.toNativeUtf8();
          return;
        }
    }
    return;
  }

  static void test() {}

  Kernel() {
    this.dylib = DynamicLibrary.open(
      'D:/Code/Sen.Environment/Kernel/build/kernel/Release/kernel.dll',
    );
  }

  static var kernelPath =
      "D:/Code/Sen.Environment/Kernel/build/kernel/Release/kernel.dll";

  Kernel.open(path) {
    this.dylib = DynamicLibrary.open(path);
  }

  int execute() {
    gui.clearMessage();
    const String scriptPath = 'D:/Code/Sen.Environment/Script/build/main.js';
    final KernelExecuteDartAPI executeMethod = this
        .dylib
        .lookupFunction<KernelExecuteCAPI, KernelExecuteDartAPI>('execute');
    var script = calloc<CStringView>(1);
    script.ref
      ..value = scriptPath.toNativeUtf8()
      ..size = scriptPath.length;
    var result = 0;
    var view = [
      './test.exe',
      kernelPath,
      scriptPath,
      "D:/TwinStar/script/utility/Console.js",
    ];
    var argument = calloc<CStringView>(view.length);
    for (var i = 0; i < view.length; ++i) {
      var e = view[i];
      argument.elementAt(i).ref
        ..size = e.length
        ..value = e.toNativeUtf8();
    }
    var arguments = calloc<CStringList>(1);
    arguments.ref
      ..size = view.length
      ..value = argument;
    try {
      result = executeMethod(
        script,
        arguments,
        Pointer.fromFunction(Kernel.debug),
      );
    } catch (e) {
      print(e);
      debugPrintStack();
    }
    for (var i = 0; i < view.length; ++i) {
      calloc.free(argument.elementAt(i).ref.value.cast<Int8>());
    }
    calloc.free(arguments);
    calloc.free(script.ref.value);
    calloc.free(script);
    return result;
  }
}
