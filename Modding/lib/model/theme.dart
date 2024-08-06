import 'package:flutter/material.dart';

final ThemeData darkTheme = ThemeData(
  brightness: Brightness.dark,
  fontFamily: 'GoogleSans',
  colorScheme: ColorScheme.fromSeed(
    seedColor: Colors.deepPurpleAccent,
    brightness: Brightness.dark,
  ).copyWith(
    surface: Colors.black,
    onSurface: Colors.white,
  ),
  scaffoldBackgroundColor: Colors.black,
  useMaterial3: true,
);

final ThemeData lightTheme = ThemeData(
  fontFamily: 'GoogleSans',
  colorScheme: ColorScheme.fromSeed(
    seedColor: Colors.deepPurple,
    brightness: Brightness.light,
  ),
  useMaterial3: true,
);
