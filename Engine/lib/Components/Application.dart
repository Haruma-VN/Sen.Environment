// ignore_for_file: file_names, prefer_const_literals_to_create_immutables
import 'package:engine/Components/AppBar/AppBarView.dart';
import 'package:engine/Components/Models/Console/ConsoleView.dart';
import 'package:engine/Components/Models/Console/Launcher.dart';
import 'package:engine/Components/Models/Theme.dart';
import 'package:engine/Components/NavigationBar/NavigationBarView.dart';
import 'package:engine/Components/NavigationBar/NavigationItem.dart';
import 'package:engine/Components/common.dart';
import 'package:engine/Api/Kernel.dart';
import 'package:flutter/material.dart';

class Application extends StatefulWidget {
  const Application({super.key});

  @override
  State<Application> createState() => _ApplicationState();
}

class _ApplicationState extends State<Application> {
  int currentPage = 0;
  List<WidgetCallback> pages = [
    (BuildContext context, ScrollController scrollView, Launcher launcher) =>
        ConsoleView(
          context: context,
          scrollController: scrollView,
          launcher: launcher,
        ),
    (_, __, ___) => SliverToBoxAdapter(
          child: Container(),
        ),
    (_, __, ___) => SliverToBoxAdapter(
          child: Container(),
        ),
  ];

  final ScrollController customScrollView = ScrollController();

  @override
  Widget build(BuildContext context) {
    var launcher = Launcher(
      kernel: Kernel.open(
        "D:/Code/Sen.Environment/Kernel/build/kernel/Release/kernel.dll",
      ),
    );
    final myPage = pages[currentPage](context, customScrollView, launcher);
    return MaterialApp(
      theme: BasicTheme.lightMode,
      darkTheme: BasicTheme.darkMode,
      themeMode: ThemeMode.light,
      debugShowCheckedModeBanner: false,
      home: Builder(
        builder: (context) => Scaffold(
          backgroundColor: Theme.of(context).scaffoldBackgroundColor,
          body: Stack(
            children: [
              Container(
                padding: const EdgeInsets.only(
                  bottom: 80.0,
                ),
                child: CustomScrollView(
                  controller: customScrollView,
                  slivers: <Widget>[
                    AppBarView(
                      appTitle: Common.applicationName,
                      titlePadding: const EdgeInsets.only(
                        left: 20.0,
                        bottom: 15.0,
                      ),
                      height: 100.0,
                    ),
                    myPage,
                  ],
                ),
              ),
              myPage is ConsoleView
                  ? Positioned(
                      bottom: 0,
                      left: 0,
                      right: 0,
                      child: Padding(
                        padding: const EdgeInsets.only(
                          top: 5.0,
                          right: 10.0,
                          left: 10.0,
                          bottom: 16.0,
                        ),
                        child: launcher,
                      ),
                    )
                  : Container(),
            ],
          ),
          bottomNavigationBar: NavigationBarView(
              currentPage: currentPage,
              models: const [
                NavigationItem("Console", Icon(Icons.terminal_outlined)),
                NavigationItem("Setting", Icon(Icons.settings_outlined)),
              ],
              onPressed: (int newPage) =>
                  setState(() => currentPage = newPage)),
        ),
      ),
    );
  }
}
