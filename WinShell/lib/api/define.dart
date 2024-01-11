// ignore_for_file: empty_constructor_bodies, unnecessary_this

import 'dart:ffi';
import 'package:ffi/ffi.dart';

final class StringView extends Struct {
  @Int32()
  external int size;

  external Pointer<Utf8> value;
}

final class StringList extends Struct {
  external Pointer<StringView> value;

  @Int32()
  external int size;
}

typedef ShellCallbackCView = Pointer<StringView> Function(
  Pointer<StringList> list,
);

typedef ShellCallbackDartView = Pointer<StringView> Function(
  Pointer<StringList> list,
);

typedef KernelExecuteCAPI = Int32 Function(
  Pointer<StringView> argument,
  Pointer<NativeFunction<ShellCallbackCView>> mCallback,
);

typedef KernelExecuteDartAPI = int Function(
  Pointer<StringView> argument,
  Pointer<NativeFunction<ShellCallbackDartView>> mCallback,
);
