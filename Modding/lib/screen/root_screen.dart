import 'package:flutter/material.dart';
import 'package:modding/provider/setting_provider.dart';
import 'package:modding/screen/home_screen.dart';
import 'package:modding/screen/setting_screen.dart';
import 'dart:io';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';
import 'package:modding/service/android_service.dart';
import 'package:provider/provider.dart';

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

  Future<void> _displayAllowDialog() async {
    final los = AppLocalizations.of(context)!;
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: Text(los.android_request),
        content: Text(los.android_storage_access_permission_required),
        actions: [
          TextButton(
            onPressed: () {
              Navigator.of(context).pop();
            },
            child: Text(los.go_to_settings),
          ),
        ],
      ),
    );
  }

  void _requestAndroidPermissionFirstTime() {
    if (!Platform.isAndroid) return;
    Future.sync(
      () async {
        final provider = Provider.of<SettingProvider>(context, listen: false);
        if (!provider.requestedPermission) {
          if (!(await AndroidService.checkStoragePermission())) {
            await _displayAllowDialog();
            await AndroidService.requestStoragePermission();
          }
        }
        provider.setRequestedPermission(true);
      },
    );
  }

  @override
  Widget build(BuildContext context) {
    _requestAndroidPermissionFirstTime();
    return Scaffold(
      appBar: AppBar(
        title: Text(widget.title),
      ),
      body: Row(
        children: [
          _makeNavigationRail(),
          Expanded(
            child: AnimatedSwitcher(
              duration: const Duration(
                milliseconds: 100,
              ),
              child: _destinations[_currentPageIndex],
              transitionBuilder: (child, animation) {
                return FadeTransition(
                  opacity: animation,
                  child: child,
                );
              },
            ),
          ),
        ],
      ),
      bottomNavigationBar: _makeNavigationBar(),
    );
  }
}
