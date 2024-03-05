// ignore_for_file: empty_constructor_bodies, unnecessary_this

import 'dart:ffi';
import 'package:ffi/ffi.dart';

final class CStringView extends Struct {
  @Uint64()
  external int size;

  external Pointer<Utf8> value;
}

final class CStringList extends Struct {
  external Pointer<CStringView> value;

  @Uint64()
  external int size;
}

typedef ShellCallbackCView = CStringView Function(
  CStringList list,
  Pointer<Void> proxy,
);

typedef ShellCallbackDartView = CStringView Function(
  CStringList list,
  Pointer<Never> proxy,
);

typedef KernelExecuteCAPI = Int32 Function(
  Pointer<CStringView> script,
  Pointer<CStringList> arguments,
  Pointer<NativeFunction<ShellCallbackCView>> mCallback,
);

typedef KernelExecuteDartAPI = int Function(
  Pointer<CStringView> script,
  Pointer<CStringList> arguments,
  Pointer<NativeFunction<ShellCallbackCView>> mCallback,
);
