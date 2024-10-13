import 'package:flutter/material.dart';
import 'package:material_symbols_icons/material_symbols_icons.dart';
import 'package:modding/model/item.dart';
import 'package:modding/provider/setting_provider.dart';
import 'package:modding/screen/animation_viewer/main_screen.dart';
import 'package:modding/screen/js_pick.dart';
import 'package:modding/screen/method_picker.dart';
import 'package:modding/screen/shell_screen.dart';
import 'package:provider/provider.dart';

class HomeScreen extends StatefulWidget {
  const HomeScreen({super.key});

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  final List<Item> items = [
    Item(
      title: 'Shell',
      description: 'Use the Shell module',
      icon: const Icon(Symbols.terminal_rounded, size: 50),
    ),
    Item(
      title: 'Method Picker',
      description: 'Select method to use from categories of methods',
      icon: const Icon(Symbols.package_2, size: 50),
    ),
    Item(
      title: 'JS Execute',
      description: 'Run built-in JS',
      icon: const Icon(Symbols.javascript_rounded, size: 50),
    ),
    Item(
      title: 'Animation Viewer',
      description: 'View animation',
      icon: const Icon(Symbols.animated_images, size: 50),
    ),
  ];

  void _initWidget({
    required SettingProvider settingProvider,
  }) {
    _initShellWidget(
      holderPath: settingProvider.toolChain,
    );
    _initMethodPicker(
      holderPath: settingProvider.toolChain,
    );
    _initJSModule(
      holderPath: settingProvider.toolChain,
    );
    _initAnimationViewer();
  }

  void _initAnimationViewer() {
    items[3].onWidget = () {
      return const AnimationViewer();
    };
  }

  void _initShellWidget({
    required String holderPath,
  }) {
    items[0].onWidget = () {
      return ShellScreen(holderPath: holderPath, arguments: const []);
    };
  }

  void _initJSModule({
    required String holderPath,
  }) {
    items[2].onWidget = () {
      return JsPick(holder: holderPath);
    };
  }

  void _initMethodPicker({
    required String holderPath,
  }) {
    items[1].onWidget = () {
      return const MethodPicker();
    };
  }

  @override
  Widget build(BuildContext context) {
    final settingProvider = Provider.of<SettingProvider>(context);
    _initWidget(settingProvider: settingProvider);
    final screenWidth = MediaQuery.of(context).size.width;
    const itemWidth = 250.0;
    int crossAxisCount = (screenWidth / itemWidth).floor();
    return Padding(
      padding: const EdgeInsets.all(8.0),
      child: GridView.builder(
        gridDelegate: SliverGridDelegateWithFixedCrossAxisCount(
          crossAxisCount: crossAxisCount,
          childAspectRatio: 1.0,
        ),
        itemCount: items.length,
        itemBuilder: (context, index) {
          final item = items[index];
          return Card(
            clipBehavior: Clip.hardEdge,
            child: InkWell(
              splashColor: Colors.blue.withAlpha(30),
              onTap: settingProvider.isValid
                  ? () {
                      Navigator.push(
                        context,
                        MaterialPageRoute(
                          builder: (context) => item.onWidget(),
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
                    ),
                    const SizedBox(height: 4),
                    Text(
                      item.description,
                      textAlign: TextAlign.center,
                      maxLines: 2,
                      overflow: TextOverflow.ellipsis,
                    ),
                    const SizedBox(height: 15),
                    settingProvider.isValid
                        ? Container()
                        : const Text('Toolchain is invalid'),
                  ],
                ),
              ),
            ),
          );
        },
      ),
    );
  }
}
