import 'package:flutter/material.dart';

ThemeData darkTheme = ThemeData(
  brightness: Brightness.dark,
  fontFamily: 'GoogleSans',
  colorScheme: ColorScheme.fromSeed(
      seedColor: Colors.deepPurple, brightness: Brightness.dark),
  useMaterial3: true,
);

ThemeData lightTheme = ThemeData(
  fontFamily: 'GoogleSans',
  colorScheme: ColorScheme.fromSeed(seedColor: Colors.deepPurple),
  useMaterial3: true,
);
