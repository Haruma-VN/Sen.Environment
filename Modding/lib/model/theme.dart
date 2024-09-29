import 'package:flutter/cupertino.dart' as cupertino;
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

class CupertinoDesign {
  static const cupertino.CupertinoThemeData darkTheme =
      cupertino.CupertinoThemeData(
    brightness: cupertino.Brightness.dark,
    primaryColor: cupertino.CupertinoColors.systemPurple,
    scaffoldBackgroundColor: cupertino.CupertinoColors.black,
    textTheme: cupertino.CupertinoTextThemeData(
      textStyle: cupertino.TextStyle(fontFamily: 'SanFrancisco'),
    ),
  );

  static const cupertino.CupertinoThemeData lightTheme =
      cupertino.CupertinoThemeData(
    brightness: cupertino.Brightness.light,
    primaryColor: cupertino.CupertinoColors.systemPurple,
    textTheme: cupertino.CupertinoTextThemeData(
      textStyle: cupertino.TextStyle(fontFamily: 'SanFrancisco'),
    ),
  );
}
