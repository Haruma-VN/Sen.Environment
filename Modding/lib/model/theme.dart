import 'package:flutter/material.dart' as material;

class MaterialDesign {
  static final material.ThemeData darkTheme = material.ThemeData(
    brightness: material.Brightness.dark,
    fontFamily: 'GoogleSans',
    colorScheme: material.ColorScheme.fromSeed(
      seedColor: material.Colors.deepPurpleAccent,
      brightness: material.Brightness.dark,
    ).copyWith(
      surface: material.Colors.black,
      onSurface: material.Colors.white,
    ),
    scaffoldBackgroundColor: material.Colors.black,
    useMaterial3: true,
    dialogTheme: material.DialogTheme(
      backgroundColor: material.Colors.grey.shade900,
    ),
  );

  static final material.ThemeData lightTheme = material.ThemeData(
    fontFamily: 'GoogleSans',
    colorScheme: material.ColorScheme.fromSeed(
      seedColor: material.Colors.deepPurple,
      brightness: material.Brightness.light,
    ),
    useMaterial3: true,
  );
}
