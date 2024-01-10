// ignore_for_file: empty_constructor_bodies, unnecessary_this

import 'dart:ffi';
import 'package:ffi/ffi.dart';

final class CStringView extends Struct {
  @Uint64()
  external int size;

  external Pointer<Utf8> value;
}

final class CStringList extends Struct {
  @Uint64()
  external int size;

  external Pointer<CStringView> value;
}

typedef ShellCallbackCView = Pointer<CStringView> Function(
  Pointer<CStringList> list,
);

typedef ShellCallbackDartView = Pointer<CStringView> Function(
  Pointer<CStringList> list,
);

typedef KernelExecuteCAPI = Int32 Function(Pointer<CStringView> argument,
    Pointer<NativeFunction<ShellCallbackCView>> mCallback);

typedef KernelExecuteDartAPI = int Function(Pointer<CStringView> argument,
    Pointer<NativeFunction<ShellCallbackDartView>> mCallback);
