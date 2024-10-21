import 'dart:io';
import 'package:flutter/material.dart';
import 'package:modding/model/build_distribution.dart';
import 'package:modding/model/theme.dart';
import 'package:modding/provider/log_provider.dart';
import 'package:modding/provider/recent_provider.dart';
import 'package:modding/provider/setting_provider.dart';
import 'package:modding/screen/root_screen.dart';
import 'package:modding/service/android_service.dart';
import 'package:modding/service/notification_service.dart';
import 'package:provider/provider.dart';
import 'package:window_manager/window_manager.dart';
import 'package:flutter_localizations/flutter_localizations.dart';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';
import 'package:dynamic_color/dynamic_color.dart';

Future<void> main(
  List<String> arguments,
) async {
  WidgetsFlutterBinding.ensureInitialized();
  if (Platform.isWindows || Platform.isLinux || Platform.isMacOS) {
    await windowManager.setTitleBarStyle(TitleBarStyle.normal);
    await WindowManager.instance.ensureInitialized();
    await windowManager.setMinimumSize(const Size(1280, 720));
    await windowManager.center();
    await windowManager.waitUntilReadyToShow();
    await windowManager.show();
    await windowManager.focus();
  }
  await NotificationService.initialize();
  if (Platform.isAndroid) {
    AndroidService.initialize();
  }
  runApp(
    MultiProvider(
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
    ),
  );
}

class Application extends StatelessWidget {
  const Application({super.key});

  @override
  Widget build(BuildContext context) {
    return DynamicColorBuilder(
      builder: (lightDynamic, darkDynamic) => MaterialApp(
        debugShowCheckedModeBanner: false,
        title: BuildDistribution.kApplicationName,
        theme: MaterialDesign.lightTheme.copyWith(
          colorScheme: lightDynamic,
        ),
        darkTheme: MaterialDesign.darkTheme.copyWith(
          colorScheme: darkDynamic,
        ),
        themeMode: Provider.of<SettingProvider>(context).themeData,
        home: const RootScreen(title: BuildDistribution.kApplicationName),
        localizationsDelegates: const [
          AppLocalizations.delegate,
          GlobalMaterialLocalizations.delegate,
          GlobalWidgetsLocalizations.delegate,
          GlobalCupertinoLocalizations.delegate,
        ],
        supportedLocales: const [
          Locale('en'),
          Locale('vi'),
          Locale('es'),
        ],
        locale: Locale(Provider.of<SettingProvider>(context).locale),
      ),
    );
  }
}
