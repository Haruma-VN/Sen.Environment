import 'package:engine/Components/Application.dart';
import 'package:engine/Components/Models/Console/MessageProvider.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

void main(
  List<String> arguments,
) {
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
