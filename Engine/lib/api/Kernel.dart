// ignore_for_file: unused_import, unnecessary_this, prefer_is_empty, avoid_print, no_leading_underscores_for_local_identifiers, constant_identifier_names, non_constant_identifier_names, file_names

import 'dart:convert';
import 'dart:ffi';
import 'package:engine/Base/Exception.dart';
import 'package:engine/api/Converter.dart';
import 'package:engine/api/Interface.dart';
import 'package:engine/api/Shell.dart';
import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';

class Kernel {
  static const version = 1;

  late DynamicLibrary dylib;

  static late Shell gui;

  static void callback(
    Pointer<CStringList> list,
    Pointer<CStringView> destination,
    Pointer<Never> proxy,
  ) {
    var result = CStringConverter.toList(list.ref);
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

  final String kernelPath;

  Kernel.open({required this.kernelPath}) {
    this.dylib = DynamicLibrary.open(kernelPath);
  }

  Future<BasicException?> run(
    String scriptPath,
    String currentShell,
    List<String> additionalArguments,
  ) async {
    gui.changeLoadingStatus();
    gui.clearMessage();
    final KernelExecuteDartAPI execute = this
        .dylib
        .lookupFunction<KernelExecuteCAPI, KernelExecuteDartAPI>('execute');
    final script = calloc<CStringView>(1);
    script.ref
      ..value = scriptPath.toNativeUtf8()
      ..size = scriptPath.length;
    final argumentList = [
      currentShell,
      kernelPath,
      scriptPath,
      ...additionalArguments,
    ];
    final argument = calloc<CStringView>(argumentList.length);
    for (var i = 0; i < argumentList.length; ++i) {
      final e = argumentList[i];
      argument.elementAt(i).ref
        ..size = e.length
        ..value = e.toNativeUtf8();
    }
    final arguments = calloc<CStringList>(1);
    arguments.ref
      ..size = argumentList.length
      ..value = argument;
    BasicException? error;
    try {
      execute(
        script,
        arguments,
        Pointer.fromFunction(Kernel.callback),
      );
    } catch (e, s) {
      error = BasicException.from(e.toString(), s);
    }
    for (var i = 0; i < argumentList.length; ++i) {
      calloc.free(argument.elementAt(i).ref.value.cast<Int8>());
    }
    calloc.free(arguments);
    calloc.free(script.ref.value);
    calloc.free(script);
    gui.changeLoadingStatus();
    return error;
  }
}
