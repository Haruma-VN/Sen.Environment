// ignore_for_file: empty_constructor_bodies, unnecessary_this, file_names

import 'dart:ffi';
import 'package:ffi/ffi.dart';

final class CStringView extends Struct {
  @Size()
  external int size;

  external Pointer<Utf8> value;
}

final class CStringList extends Struct {
  external Pointer<CStringView> value;

  @Size()
  external int size;
}

typedef ShellCallbackCView = Void Function(
  Pointer<CStringList> list,
  Pointer<CStringView> destination,
  Pointer<Void> proxy,
);

typedef ShellCallbackDartView = void Function(
  Pointer<CStringList> list,
  Pointer<CStringView> destination,
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
