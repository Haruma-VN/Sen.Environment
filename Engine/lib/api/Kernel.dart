// ignore_for_file: unused_import, unnecessary_this, prefer_is_empty, avoid_print, no_leading_underscores_for_local_identifiers, constant_identifier_names, non_constant_identifier_names, file_names, avoid_init_to_null, depend_on_referenced_packages

import 'dart:async';
import 'dart:convert';
import 'dart:ffi';
import 'dart:io';
import 'dart:isolate';
import 'package:engine/Base/Exception.dart';
import 'package:engine/api/Converter.dart';
import 'package:engine/api/Interface.dart';
import 'package:engine/api/Shell.dart';
import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';
import 'package:async/async.dart';

class Kernel {
  static const version = 1;

  static DynamicLibrary? dylib = null;

  static late Shell gui;

  static SendPort? _sendPort = null;

  static void _callback(
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
                _sendPort!.send([result[1]]);
                break;
              }
            case 3:
              {
                _sendPort!.send([result[1], result[2]]);
                break;
              }
            case 4:
              {
                _sendPort!.send([result[1], result[2]]);
                break;
              }
          }
          break;
        }
      case 'input':
        {
          var e = "1";
          destination.ref
            ..size = e.length
            ..value = e.toNativeUtf8();
          _sendPort!.send([null]);
          break;
        }
      case 'version':
        {
          var _version = version.toString();
          destination.ref
            ..size = _version.length
            ..value = _version.toNativeUtf8();
          _sendPort!.send([null]);
          break;
        }
      case 'is_gui':
        {
          var _is_gui = 1.toString();
          destination.ref
            ..size = _is_gui.length
            ..value = _is_gui.toNativeUtf8();
          _sendPort!.send([null]);
          break;
        }
    }
    sleep(const Duration(milliseconds: 10));
    return;
  }

  static _sub(
    List<dynamic> isolateData,
  ) async {
    var mainSendPort = isolateData[0] as SendPort;
    var kernelPath = isolateData[1];
    _sendPort = mainSendPort;
    dylib = DynamicLibrary.open(kernelPath);
    var execute = dylib!
        .lookupFunction<KernelExecuteCAPI, KernelExecuteDartAPI>('execute');
    var subReceivePort = ReceivePort();
    var subStreamQueue = StreamQueue<dynamic>(subReceivePort);
    mainSendPort.send(subReceivePort.sendPort);
    var subEvent = await subStreamQueue.next as List<dynamic>;
    var scriptPath = subEvent[0] as String;
    var currentShell = subEvent[1] as String;
    var additionalArguments = subEvent[2] as List<String>;
    var script = calloc<CStringView>(1);
    var scriptNativeString = CStringConverter.toUint8List(scriptPath);
    var scriptPointer = CStringConverter.utf8ArrayToCString(scriptNativeString);
    script.ref
      ..value = scriptPointer
      ..size = scriptNativeString.length;
    var argumentList = [
      currentShell,
      kernelPath,
      scriptPath,
      ...additionalArguments,
    ];
    var argument = calloc<CStringView>(argumentList.length);
    for (var i = 0; i < argumentList.length; ++i) {
      var currentArgument = CStringConverter.toUint8List(argumentList[i]);
      var currentArgumentPointer =
          CStringConverter.utf8ArrayToCString(currentArgument);
      argument.elementAt(i).ref
        ..size = currentArgument.length
        ..value = currentArgumentPointer;
    }
    var arguments = calloc<CStringList>(1);
    arguments.ref
      ..size = argumentList.length
      ..value = argument;
    execute(
      script,
      arguments,
      Pointer.fromFunction(_callback),
    );
    for (var i = 0; i < argumentList.length; ++i) {
      calloc.free(argument.elementAt(i).ref.value.cast<Int8>());
    }
    calloc.free(arguments.ref.value);
    calloc.free(arguments);
    calloc.free(script.ref.value);
    calloc.free(script);
    mainSendPort.send(null);
    _sendPort = null;
    return;
  }

  static String? _kernelPath = null;

  Kernel.open(String kernelPath) {
    _kernelPath = kernelPath;
    dylib = DynamicLibrary.open(_kernelPath!);
  }

  void sendMessage(List<dynamic> packet) {
    for (var e in packet) {
      if (e != null && (e as String).isNotEmpty) {
        gui.sendMessage(e);
      }
    }
    return;
  }

  Future<void> run(
    String scriptPath,
    String currentShell,
    List<String> additionalArguments,
  ) async {
    gui.changeLoadingStatus();
    gui.clearMessage();
    var mainReceivePort = ReceivePort();
    var mainStreamQueue = StreamQueue<dynamic>(mainReceivePort);
    await Isolate.spawn(_sub, [mainReceivePort.sendPort, _kernelPath]);
    var subSendPort = await mainStreamQueue.next as SendPort;
    subSendPort.send([scriptPath, currentShell, additionalArguments]);
    while (await mainStreamQueue.hasNext) {
      var mainEvent = await mainStreamQueue.next as List<dynamic>?;
      if (mainEvent == null) {
        mainEvent = await mainStreamQueue.next as List<dynamic>;
        sendMessage(mainEvent);
        break;
      } else {
        sendMessage(mainEvent);
      }
    }
    await mainStreamQueue.cancel();
    return;
  }
}
