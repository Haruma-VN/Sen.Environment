// ignore_for_file: file_names

import 'package:flutter/material.dart';

class BasicTheme {
  static final ThemeData lightMode = ThemeData(
    brightness: Brightness.light,
    colorScheme: null,
    colorSchemeSeed: null,
    fontFamily: 'GoogleSans',
    scaffoldBackgroundColor:
        Color.alphaBlend(Colors.pinkAccent.withOpacity(0.05), Colors.white),
  );

  static final ThemeData darkMode = ThemeData(
    brightness: Brightness.dark,
    colorScheme: null,
    colorSchemeSeed: null,
    fontFamily: 'GoogleSans',
    scaffoldBackgroundColor: Colors.black,
  );
}
