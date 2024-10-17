import 'package:flutter/material.dart';
import 'package:modding/screen/home_screen.dart';
import 'package:modding/screen/setting_screen.dart';
import 'dart:io';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';

class RootScreen extends StatefulWidget {
  const RootScreen({super.key, required this.title});

  final String title;

  @override
  State<RootScreen> createState() => _RootScreenState();
}

class _RootScreenState extends State<RootScreen> {
  int _currentPageIndex = 0;

  final _labelBehavior = NavigationDestinationLabelBehavior.alwaysShow;

  final List<Widget> _destinations = const [
    HomeScreen(),
    SettingScreen(),
  ];

  void _changeScreen(int index) {
    setState(() {
      _currentPageIndex = index;
    });
  }

  Widget? _makeNavigationBar() {
    final los = AppLocalizations.of(context)!;
    if (Platform.isAndroid || Platform.isIOS) {
      return NavigationBar(
        labelBehavior: _labelBehavior,
        selectedIndex: _currentPageIndex,
        onDestinationSelected: _changeScreen,
        destinations: <Widget>[
          NavigationDestination(
            icon: const Icon(Icons.home_outlined),
            selectedIcon: const Icon(Icons.home_filled),
            label: los.home,
          ),
          NavigationDestination(
            icon: const Icon(Icons.settings_outlined),
            selectedIcon: const Icon(Icons.settings),
            label: los.settings,
          ),
        ],
      );
    }
    return null;
  }

  Widget _makeNavigationRail() {
    final los = AppLocalizations.of(context)!;
    if (Platform.isWindows || Platform.isLinux || Platform.isMacOS) {
      return NavigationRail(
        selectedIndex: _currentPageIndex,
        onDestinationSelected: (index) {
          setState(() {
            _currentPageIndex = index;
          });
        },
        labelType: NavigationRailLabelType.all,
        destinations: [
          NavigationRailDestination(
            icon: const Icon(Icons.home_outlined),
            selectedIcon: const Icon(Icons.home_filled),
            label: Text(los.home),
          ),
          NavigationRailDestination(
            icon: const Icon(Icons.settings_outlined),
            selectedIcon: const Icon(Icons.settings),
            label: Text(los.settings),
          ),
        ],
      );
    }
    return const SizedBox.shrink();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Row(
        children: [
          _makeNavigationRail(),
          Expanded(
            child: _destinations[_currentPageIndex],
          ),
        ],
      ),
      bottomNavigationBar: _makeNavigationBar(),
    );
  }
}
