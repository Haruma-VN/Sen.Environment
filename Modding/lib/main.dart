import 'dart:io';

import 'package:flutter/cupertino.dart';
import 'package:flutter/material.dart';
import 'package:modding/model/theme.dart';
import 'package:modding/provider/filter_provider.dart';
import 'package:modding/provider/log_provider.dart';
import 'package:modding/provider/manifest_provider.dart';
import 'package:modding/provider/recent_provider.dart';
import 'package:modding/provider/setting_provider.dart';
import 'package:modding/screen/root_screen.dart';
import 'package:provider/provider.dart';
import 'package:modding/provider/item_provider.dart';
import 'package:window_manager/window_manager.dart';

Future<void> main(
  List<String> arguments,
) async {
  WidgetsFlutterBinding.ensureInitialized();
  if (Platform.isWindows || Platform.isLinux || Platform.isMacOS) {
    await WindowManager.instance.ensureInitialized();
    await windowManager.center();
    await windowManager.waitUntilReadyToShow();
    await windowManager.show();
  }
  runApp(MultiProvider(
    providers: [
      ChangeNotifierProvider(
        create: (_) => ItemProvider(),
      ),
      ChangeNotifierProvider(
        create: (_) => LogProvider(),
      ),
      ChangeNotifierProvider(
        create: (_) => FilterProvider(),
      ),
      ChangeNotifierProvider(
        create: (_) => RecentProvider(),
      ),
      ChangeNotifierProvider(
        create: (_) => ManifestProvider(),
      ),
      ChangeNotifierProvider(
        create: (_) => SettingProvider(),
      ),
    ],
    child: const Application(),
  ));
}

class Application extends StatelessWidget {
  const Application({super.key});

  @override
  Widget build(BuildContext context) {
    if (Platform.isIOS || Platform.isMacOS) {
      return const CupertinoApp(
        debugShowCheckedModeBanner: false,
        title: 'Sen: Environment',
        theme: CupertinoDesign.lightTheme,
        home: RootScreen(title: 'Sen: Environment'),
      );
    } else {
      return MaterialApp(
        debugShowCheckedModeBanner: false,
        title: 'Sen: Environment',
        theme: MaterialDesign.lightTheme,
        darkTheme: MaterialDesign.darkTheme,
        themeMode: Provider.of<SettingProvider>(context).themeData,
        home: const RootScreen(title: 'Sen: Environment'),
      );
    }
  }
}
