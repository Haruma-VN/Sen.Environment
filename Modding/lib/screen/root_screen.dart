import 'package:flutter/material.dart';
import 'package:material_symbols_icons/symbols.dart';
import 'package:modding/screen/home_screen.dart';
import 'package:modding/screen/setting_screen.dart';
import 'dart:io';

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
    if (Platform.isAndroid || Platform.isIOS) {
      return NavigationBar(
        labelBehavior: _labelBehavior,
        selectedIndex: _currentPageIndex,
        onDestinationSelected: _changeScreen,
        destinations: const <Widget>[
          NavigationDestination(
            icon: Icon(Icons.home_outlined),
            selectedIcon: Icon(Icons.home_filled),
            label: 'Home',
          ),
          NavigationDestination(
            icon: Icon(Icons.settings_outlined),
            selectedIcon: Icon(Icons.settings),
            label: 'Settings',
          ),
        ],
      );
    }
    return null;
  }

  Widget _makeNavigationRail() {
    if (Platform.isWindows || Platform.isLinux || Platform.isMacOS) {
      return NavigationRail(
        selectedIndex: _currentPageIndex,
        onDestinationSelected: (index) {
          setState(() {
            _currentPageIndex = index;
          });
        },
        labelType: NavigationRailLabelType.all,
        destinations: const [
          NavigationRailDestination(
            icon: Icon(Icons.home_outlined),
            selectedIcon: Icon(Icons.home_filled),
            label: Text('Home'),
          ),
          NavigationRailDestination(
            icon: Icon(Icons.settings_outlined),
            selectedIcon: Icon(Icons.settings),
            label: Text('Settings'),
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
