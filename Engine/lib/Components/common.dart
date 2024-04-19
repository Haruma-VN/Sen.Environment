import 'package:engine/Components/Models/Console/Launcher.dart';
import 'package:flutter/material.dart';

typedef WidgetCallback = Widget Function(
    BuildContext context, ScrollController scrollView, Launcher launcher);

class Common {
  static const String applicationName = "Sen.Environment: GUI";

  static const int version = 1;

  static const bool isGUI = true;
}
