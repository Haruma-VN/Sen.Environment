import 'package:flutter/material.dart';

typedef WidgetCallback = Widget Function(
    BuildContext context, ScrollController scrollView);

class Common {
  static const String applicationName = "Sen.Environment: GUI";

  static const int version = 1;

  static const bool isGUI = true;
}
