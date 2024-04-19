// ignore_for_file: unused_import, unnecessary_this, prefer_is_empty, avoid_print, no_leading_underscores_for_local_identifiers, constant_identifier_names, non_constant_identifier_names, file_names, avoid_init_to_null, depend_on_referenced_packages

import 'dart:async';
import 'dart:convert';
import 'dart:ffi';
import 'dart:io';
import 'dart:isolate';
import 'package:engine/Api/Interface.dart';
import 'package:engine/Base/BasicString.dart';
import 'package:engine/Base/Boolean.dart';
import 'package:engine/Base/Exception.dart';
import 'package:engine/Components/Models/Console/Launcher.dart';
import 'package:engine/Api/Converter.dart';
import 'package:engine/Api/Shell.dart';
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
    var command = result.removeAt(0);
    switch (command) {
      case 'display':
        {
          assert(result.length >= 1, "argument must be greater than 1");
          switch (result.length) {
            case 1:
              {
                _sendPort!.send(['display', result[0]]);
                break;
              }
            case 2:
              {
                _sendPort!.send(['display', result[0], result[1]]);
                break;
              }
            case 3:
              {
                _sendPort!.send(['display', result[0], result[1], result[2]]);
                break;
              }
          }
          break;
        }
      case 'input':
        {
          var state = calloc<Bool>();
          state.value = false;
          _sendPort!.send(['input_string', state.address, destination.address]);
          while (!state.value) {
            sleep(const Duration(milliseconds: 10));
          }
          calloc.free(state);
          break;
        }
      case 'input_boolean':
        {
          var state = calloc<Bool>();
          state.value = false;
          _sendPort!
              .send(['input_boolean', state.address, destination.address]);
          while (!state.value) {
            sleep(const Duration(milliseconds: 10));
          }
          calloc.free(state);
          break;
        }
      case 'input_enumeration':
        {
          var state = calloc<Bool>();
          state.value = false;
          _sendPort!.send([
            'input_enumeration',
            state.address,
            destination.address,
            [...result]
          ]);
          while (!state.value) {
            sleep(const Duration(milliseconds: 10));
          }
          calloc.free(state);
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
      case 'push_notification':
        {
          assert(result.length >= 2);
          _sendPort!.send(['push_notification', result[0], result[1]]);
          break;
        }
      case 'finish':
        {
          _sendPort!.send(['finish']);
          break;
        }
    }
    sleep(const Duration(milliseconds: 10));
    return;
  }

  static Future<void> _sub(
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
    var scriptPointer = CStringConverter.utf8ListToCString(scriptNativeString);
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
          CStringConverter.utf8ListToCString(currentArgument);
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

  void queryCommand(List<dynamic> event) {
    var first = event.removeAt(0);
    if (first == null) {
      return;
    }
    switch (first) {
      case 'display':
        {
          _sendMessage(event);
          break;
        }
      case 'push_notification':
        {
          _pushNotification(event[0]);
          break;
        }
      case 'finish':
        {
          _setFinishState();
          break;
        }
      case 'input':
        {
          _setInputString();
          break;
        }
    }

    return;
  }

  void _setInputString() {
    gui.inputStringState();
    return;
  }

  void _setFinishState() {
    gui.setFinishedState();
    gui.changeLoadingStatus();
    return;
  }

  void _pushNotification(String message) {
    gui.pushNotification(message);
    return;
  }

  void _sendMessage(List<dynamic> packet) {
    switch (packet.length) {
      case 1:
        {
          gui.sendMessage(packet[0]);
          break;
        }
      case 2:
        {
          gui.sendMessageWithSubtitle(packet[0], packet[1]);
          break;
        }
      case 3:
        {
          gui.sendMessageWithSubtitleAndColor(packet[0], packet[1], packet[2]);
          break;
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
        break;
      } else {
        if (mainEvent[0] != null &&
            (mainEvent[0] as String).contains('input')) {
          var callbackState = Pointer<Bool>.fromAddress(mainEvent[1]);
          var destination = Pointer<CStringView>.fromAddress(mainEvent[2]);
          statement:
          switch (mainEvent[0]) {
            case 'input_string':
              {
                await gui.execute(destination);
                break statement;
              }
            case 'input_enumeration':
              {
                var restStatement = (mainEvent[3] as List<String>);
                await gui.inputEnumeration(destination, restStatement);
                break statement;
              }
            case 'input_boolean':
              {
                await gui.inputBoolean(destination);
                break statement;
              }
          }
          callbackState.value = true;
        } else {
          queryCommand(mainEvent);
        }
      }
    }
    gui.setFinishedState();
    await mainStreamQueue.cancel();
    return;
  }
}
