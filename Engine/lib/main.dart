import 'dart:io';

import 'package:engine/Components/Application.dart';
import 'package:engine/Components/Models/Console/MessageProvider.dart';
import 'package:engine/Components/Models/Controller/NotificationService.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';
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
  await NotificationService.initialize();
  runApp(
    ChangeNotifierProvider(
      create: (context) => MessageModel(),
      child: const MyApp(),
    ),
  );
}

class MyApp extends StatelessWidget {
  const MyApp({super.key});

  @override
  Widget build(BuildContext context) {
    return const Application();
  }
}
