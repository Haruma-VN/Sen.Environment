// ignore_for_file: file_names, prefer_const_constructors, unnecessary_this, must_be_immutable, unused_import
import 'dart:async';
import 'dart:ffi';

import 'package:engine/Api/Converter.dart';
import 'package:engine/Api/Interface.dart';
import 'package:engine/Components/Message/Message.dart';
import 'package:engine/Components/Message/MessageWrapper.dart';
import 'package:engine/Components/Models/Console/Launcher.dart';
import 'package:engine/Components/Models/Console/MessageProvider.dart';
import 'package:engine/Components/Models/Controller/NotificationService.dart';
import 'package:engine/Api/Kernel.dart';
import 'package:engine/Api/Shell.dart';
import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';
import 'package:provider/provider.dart';

class ConsoleView extends StatefulWidget with ChangeNotifier implements Shell {
  ConsoleView({
    super.key,
    required this.context,
    required this.scrollController,
    required this.launcher,
  }) {
    Kernel.gui = this;
  }

  final ScrollController scrollController;

  final Launcher launcher;

  @override
  State<ConsoleView> createState() => _ConsoleViewState();

  final BuildContext context;

  @override
  void clearMessage() {
    Provider.of<MessageModel>(context, listen: false).clearMessage();
    notifyListeners();
    return;
  }

  @override
  void sendMessage(String message) async {
    Provider.of<MessageModel>(context, listen: false).sendMessage(message);
    await scrollController.animateTo(
      scrollController.position.maxScrollExtent,
      duration: Duration(milliseconds: 100),
      curve: Curves.easeOut,
    );
    notifyListeners();
    return;
  }

  @override
  void changeLoadingStatus() {
    Provider.of<MessageModel>(context, listen: false).changeLoadingStatus();
    notifyListeners();
    return;
  }

  @override
  void notify() {
    notifyListeners();
    return;
  }

  @override
  void sendMessageWithSubtitle(String title, String message) async {
    Provider.of<MessageModel>(context, listen: false)
        .sendMessageWithSubtitle(title, message);
    await scrollController.animateTo(
      scrollController.position.maxScrollExtent,
      duration: Duration(milliseconds: 100),
      curve: Curves.easeOut,
    );
    notifyListeners();
    return;
  }

  @override
  void sendMessageWithSubtitleAndColor(
    String title,
    String message,
    String color,
  ) async {
    Provider.of<MessageModel>(context, listen: false)
        .sendMessageWithSubtitleAndColor(title, message, color);
    await scrollController.animateTo(
      scrollController.position.maxScrollExtent,
      duration: Duration(milliseconds: 100),
      curve: Curves.easeOut,
    );
    notifyListeners();
    return;
  }

  @override
  void pushNotification(String message) {
    NotificationService.push('Sen: Environment', message);
    return;
  }

  @override
  void setFinishedState() {
    Provider.of<MessageModel>(context, listen: false).setFinishedState();
    notifyListeners();
    return;
  }

  @override
  void inputStringState() {
    Provider.of<MessageModel>(context, listen: false).inputStringState();
    notifyListeners();
    return;
  }

  @override
  void setIdle() {
    Provider.of<MessageModel>(context, listen: false).setIdle();
    notifyListeners();
    return;
  }

  @override
  Future<void> execute(Pointer<CStringView> arg) async {
    var completer = Completer<String?>();
    this.inputStringState();
    launcher.completer = completer;
    var e = await completer.future;
    this.sendMessage(e!);
    this.setIdle();
    var units = CStringConverter.toUint8List(e);
    var utf8Str = CStringConverter.utf8ListToCString(units);
    arg.ref
      ..size = units.length
      ..value = utf8Str;
    await WidgetsBinding.instance.endOfFrame;
    return;
  }

  @override
  Future<void> inputEnumeration(
      Pointer<CStringView> dest, List<String> arguments) async {
    var completer = Completer<String?>();
    Provider.of<MessageModel>(context, listen: false)
        .inputEnumeration(dest, arguments);
    notifyListeners();
    Launcher.enumeration = arguments;
    launcher.completer = completer;
    var e = (await completer.future)!;
    this.sendMessage(e);
    this.setIdle();
    var units = CStringConverter.toUint8List(e);
    var utf8Str = CStringConverter.utf8ListToCString(units);
    dest.ref
      ..size = units.length
      ..value = utf8Str;
    await WidgetsBinding.instance.endOfFrame;
    return;
  }

  @override
  Future<void> inputBoolean(Pointer<CStringView> arg) async {
    var completer = Completer<String?>();
    Provider.of<MessageModel>(context, listen: false).inputBoolean(arg);
    launcher.completer = completer;
    var e = await completer.future;
    this.setIdle();
    this.sendMessage(e! == '1' ? "True" : "False");
    var units = CStringConverter.toUint8List(e);
    var utf8Str = CStringConverter.utf8ListToCString(units);
    arg.ref
      ..size = units.length
      ..value = utf8Str;
    await WidgetsBinding.instance.endOfFrame;
    return;
  }
}

class _ConsoleViewState extends State<ConsoleView> {
  Widget _makeExpanderCard(BuildContext context, MessageWrapper e) {
    if (e.message != null && e.message!.isNotEmpty) {
      return Row(
        children: [
          Expanded(
            child: Text(
              e.message!,
              style: Theme.of(context).textTheme.bodyMedium,
            ),
          ),
        ],
      );
    }
    return Container();
  }

  @override
  Widget build(BuildContext context) {
    return SliverList(
      delegate: SliverChildListDelegate(
        [
          ...Provider.of<MessageModel>(context).messages.map(
                (MessageWrapper e) => Container(
                  margin: const EdgeInsets.only(
                    left: 5.0,
                    right: 5.0,
                  ),
                  child: Message(
                    title: Column(
                      children: <Widget>[
                        Row(
                          children: [
                            IgnorePointer(child: Icon(e.icon, size: 18.0)),
                            const SizedBox(width: 10.0),
                            Expanded(
                              child: Text(
                                e.title,
                                style: Theme.of(context).textTheme.bodyLarge,
                              ),
                            ),
                          ],
                        ),
                        const SizedBox(height: 3.0),
                        _makeExpanderCard(context, e)
                      ],
                    ),
                    baseColor: e.color != null
                        ? e.color!
                        : Theme.of(context).colorScheme.surface,
                    padding: const EdgeInsets.symmetric(
                      vertical: 8.0,
                      horizontal: 5.0,
                    ),
                  ),
                ),
              ),
        ],
      ),
    );
  }
}
