// ignore_for_file: empty_constructor_bodies, unnecessary_this

import 'dart:ffi';
import 'package:ffi/ffi.dart';

final class StringView extends Struct {
  @Uint64()
  external int size;

  external Pointer<Utf8> value;
}

final class StringList extends Struct {
  external Pointer<StringView> value;

  @Uint64()
  external int size;
}

typedef ShellCallbackCView = StringView Function(
  StringList list,
  Pointer<Void> proxy,
);

typedef ShellCallbackDartView = StringView Function(
  StringList list,
  Pointer<Never> proxy,
);

typedef KernelExecuteCAPI = Int32 Function(
  Pointer<StringView> script,
  Pointer<StringList> arguments,
  Pointer<NativeFunction<ShellCallbackCView>> mCallback,
);

typedef KernelExecuteDartAPI = int Function(
  Pointer<StringView> script,
  Pointer<StringList> arguments,
  Pointer<NativeFunction<ShellCallbackCView>> mCallback,
);
