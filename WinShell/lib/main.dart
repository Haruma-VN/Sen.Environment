// ignore_for_file: unnecessary_this

import 'package:fluent_ui/fluent_ui.dart';
import 'package:flutter/material.dart';
import 'package:winshell/api/kernel.dart';
import 'package:winshell/components/model/page_view.dart';
import 'package:winshell/components/model/text.dart';
import 'package:winshell/components/model/theme.dart';
import 'package:winshell/interface/shell.dart';
//import 'package:winshell/interface/shell.dart';
import 'package:winshell/interface/test.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  int that = 1;

  static TextEditingController controller = TextEditingController();

  List<NavigationPaneItem> items = [
    PaneItem(
      icon: const Icon(FluentIcons.home),
      title: const Text('Home'),
      body: Container(),
    ),
    PaneItemSeparator(),
    PaneItemExpander(
      icon: const Icon(FluentIcons.package),
      title: const Text('JavaScript'),
      body: Container(),
      items: [
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('Evaluate'),
          body: Column(
            children: [
              WinShellInputFieldWithIcon(
                label: 'Source file',
                placeHolder: 'Input source file',
                iconBegin: FluentIcons.js,
                iconEnd: FluentIcons.upload,
                controller: controller,
              ),
              WinShellPageView(
                  text: "Execute",
                  child: MaterialApp(
                    debugShowCheckedModeBanner: false,
                    home: Scaffold(body: Container()),
                  ))
            ],
          ),
        ),
      ],
    ),
  ];

  int topIndex = 0;

  Kernel kernel = Kernel();

  @override
  Widget build(BuildContext context) {
    return FluentApp(
      debugShowCheckedModeBanner: false,
      themeMode: ThemeMode.system,
      darkTheme: WinShellTheme.darkTheme,
      theme: WinShellTheme.lightTheme,
      home: CView(
        kernel: this.kernel,
      ),
    );
  }
}
