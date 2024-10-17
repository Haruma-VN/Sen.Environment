import 'dart:io';
import 'package:flutter/material.dart';
import 'package:modding/model/theme.dart';
import 'package:modding/provider/log_provider.dart';
import 'package:modding/provider/recent_provider.dart';
import 'package:modding/provider/setting_provider.dart';
import 'package:modding/screen/root_screen.dart';
import 'package:provider/provider.dart';
import 'package:window_manager/window_manager.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';

Future<void> main(
  List<String> arguments,
) async {
  WidgetsFlutterBinding.ensureInitialized();
  if (Platform.isWindows || Platform.isLinux || Platform.isMacOS) {
    await WindowManager.instance.ensureInitialized();
    await windowManager.setMinimumSize(const Size(400, 400));
    await windowManager.center();
    await windowManager.waitUntilReadyToShow();
    await windowManager.show();
  }
  runApp(MultiProvider(
    providers: [
      ChangeNotifierProvider(
        create: (_) => LogProvider(),
      ),
      ChangeNotifierProvider(
        create: (_) => RecentProvider(),
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
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      title: 'Sen: Environment',
      theme: MaterialDesign.lightTheme,
      darkTheme: MaterialDesign.darkTheme,
      themeMode: Provider.of<SettingProvider>(context).themeData,
      home: const RootScreen(title: 'Sen: Environment'),
      localizationsDelegates: const [
        AppLocalizations.delegate,
        GlobalMaterialLocalizations.delegate,
        GlobalWidgetsLocalizations.delegate,
        GlobalCupertinoLocalizations.delegate,
      ],
      supportedLocales: const [
        Locale('en'),
        Locale('vi'),
      ],
      locale: Locale(Provider.of<SettingProvider>(context).locale),
    );
  }
}
