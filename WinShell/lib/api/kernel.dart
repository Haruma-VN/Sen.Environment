// ignore_for_file: unused_import, unnecessary_this, prefer_is_empty, avoid_print

import 'dart:convert';
import 'dart:ffi';
import 'package:ffi/ffi.dart';
import 'package:fluent_ui/fluent_ui.dart';
import 'package:winshell/api/define.dart';
import 'package:winshell/api/converter.dart';
import 'package:winshell/interface/test.dart';

class Kernel {
  late DynamicLibrary dylib;

  static late CViewStage gui;

  static StringView debug(
    StringList list,
    Pointer<Never> proxy,
  ) {
    var result = CStringConverter.toList(list);
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
    const String scriptPath = 'D:/Code/Sen.Environment/Script/build/main.js';
    final KernelExecuteDartAPI executeMethod = this
        .dylib
        .lookupFunction<KernelExecuteCAPI, KernelExecuteDartAPI>('execute');
    Pointer<StringView> script = calloc<StringView>(1);
    script.ref
      ..value = scriptPath.toNativeUtf8()
      ..size = scriptPath.length;
    int result = 0;
    List<String> view = [
      './test.exe',
      kernelPath,
      scriptPath,
      "D:/test/ZombieSkycityZombossGroup_1536.sprite/ipad3_10.8.1_main.rsb.bundle/packet/__MANIFESTGROUP__.rsg",
    ];
    Pointer<StringView> argument = calloc<StringView>(view.length);
    for (var i = 0; i < view.length; ++i) {
      var e = view[i];
      argument.elementAt(i).ref
        ..size = e.length
        ..value = e.toNativeUtf8();
    }
    Pointer<StringList> arguments = calloc<StringList>(1);
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
