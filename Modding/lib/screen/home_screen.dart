import 'dart:io';

import 'package:flutter/material.dart';
import 'package:material_symbols_icons/material_symbols_icons.dart';
import 'package:modding/model/item.dart';
import 'package:modding/provider/setting_provider.dart';
import 'package:modding/screen/animation_viewer/main_screen.dart';
import 'package:modding/screen/js_pick.dart';
import 'package:modding/screen/method_picker.dart';
import 'package:modding/screen/shell/shell_screen.dart';
import 'package:page_transition/page_transition.dart';
import 'package:provider/provider.dart';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  late List<Item> items;

  @override
  void initState() {
    super.initState();
  }

  void _initWidget({
    required SettingProvider settingProvider,
  }) {
    _initShellWidget();
    _initMethodPicker();
    _initJSModule(holder: settingProvider.toolChain);
    _initAnimationViewer();
  }

  void _initAnimationViewer() {
    items[3].onWidget = () {
      return const AnimationViewer();
    };
  }

  void _initShellWidget() {
    items[0].onWidget = () {
      return const ShellScreen(arguments: []);
    };
  }

  void _initJSModule({
    required String holder,
  }) {
    items[2].onWidget = () {
      return JsPick(
        holder: holder,
      );
    };
  }

  void _initMethodPicker() {
    items[1].onWidget = () {
      return const MethodPicker();
    };
  }

  Widget _buildUI() {
    final los = AppLocalizations.of(context)!;
    final settingProvider = Provider.of<SettingProvider>(context);
    if (Platform.isMacOS || Platform.isLinux || Platform.isWindows) {
      final screenWidth = MediaQuery.of(context).size.width;
      const itemWidth = 250.0;
      final crossAxisCount = (screenWidth / itemWidth).floor();
      return GridView.builder(
        gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
          crossAxisCount: crossAxisCount,
          childAspectRatio: 1.0,
        ),
        itemCount: items.length,
        itemBuilder: (context, index) {
          final item = items[index];
          return Tooltip(
            message: item.title,
            child: Card(
              clipBehavior: Clip.none,
              child: InkWell(
                splashColor: Colors.blue.withAlpha(30),
                onTap: settingProvider.isValid
                    ? () {
                        Navigator.push(
                          context,
                          PageTransition(
                            duration: const Duration(milliseconds: 300),
                            type: PageTransitionType.rightToLeft,
                            child: item.onWidget(),
                          ),
                        );
                      }
                    : null,
                child: Container(
                  margin: const EdgeInsets.symmetric(horizontal: 8.0),
                  child: Column(
                    mainAxisAlignment: MainAxisAlignment.center,
                    children: [
                      item.icon,
                      const SizedBox(height: 8),
                      Text(
                        item.title,
                        textAlign: TextAlign.center,
                        maxLines: 4,
                        style: const TextStyle(fontWeight: FontWeight.bold),
                      ),
                      const SizedBox(height: 4),
                      Text(
                        item.description,
                        textAlign: TextAlign.center,
                        maxLines: 4,
                        overflow: TextOverflow.ellipsis,
                      ),
                      const SizedBox(height: 15),
                      settingProvider.isValid
                          ? Container()
                          : Text(los.toolchain_is_invalid),
                    ],
                  ),
                ),
              ),
            ),
          );
        },
      );
    } else {
      return ListView.builder(
        itemCount: items.length,
        itemBuilder: (context, index) {
          final item = items[index];
          return Padding(
            padding: const EdgeInsets.symmetric(vertical: 4.0),
            child: Card(
              clipBehavior: Clip.hardEdge,
              child: InkWell(
                splashColor: Colors.blue.withAlpha(30),
                onTap: settingProvider.isValid
                    ? () {
                        Navigator.push(
                          context,
                          PageTransition(
                            duration: const Duration(milliseconds: 300),
                            type: PageTransitionType.rightToLeft,
                            child: item.onWidget(),
                          ),
                        );
                      }
                    : null,
                child: ListTile(
                  leading: item.icon,
                  title: Text(
                    item.title,
                    maxLines: 4,
                  ),
                  subtitle: Column(
                    crossAxisAlignment: CrossAxisAlignment.start,
                    children: [
                      const SizedBox(height: 2),
                      Text(
                        item.description,
                        maxLines: 2,
                        overflow: TextOverflow.ellipsis,
                      ),
                      const SizedBox(height: 8),
                      settingProvider.isValid
                          ? Container()
                          : Text(los.toolchain_is_invalid),
                    ],
                  ),
                  onTap: settingProvider.isValid
                      ? () {
                          Navigator.push(
                            context,
                            PageTransition(
                              duration: const Duration(milliseconds: 300),
                              type: PageTransitionType.rightToLeft,
                              child: item.onWidget(),
                            ),
                          );
                        }
                      : null,
                ),
              ),
            ),
          );
        },
      );
    }
  }

  void _initItem() {
    final los = AppLocalizations.of(context)!;
    items = [
      Item(
        title: los.shell,
        description: los.shell_description,
        icon: const Icon(Symbols.terminal_rounded, size: 50),
      ),
      Item(
        title: los.method_picker,
        description: los.method_picker_description,
        icon: const Icon(Symbols.package_2, size: 50),
      ),
      Item(
        title: los.js_execute,
        description: los.js_execute_description,
        icon: const Icon(Symbols.javascript_rounded, size: 50),
      ),
      Item(
        title: los.animation_viewer,
        description: los.animation_viewer_description,
        icon: const Icon(Symbols.animated_images, size: 50),
      ),
    ];
  }

  @override
  Widget build(BuildContext context) {
    final settingProvider = Provider.of<SettingProvider>(context);
    _initItem();
    _initWidget(settingProvider: settingProvider);
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 8.0),
      child: _buildUI(),
    );
  }
}
