// ignore_for_file: file_names, unnecessary_this

import 'package:engine/Components/NavigationBar/NavigationItem.dart';
import 'package:flutter/material.dart';

class NavigationBarView extends BottomNavigationBar {
  final List<NavigationItem> models;

  final int currentPage;

  final Function(int)? onPressed;

  final Color? color;

  static BottomNavigationBarItem _makeDest(NavigationItem e) {
    return BottomNavigationBarItem(
      icon: e.icon,
      label: e.name,
    );
  }

  NavigationBarView({
    Key? key,
    this.color,
    required this.models,
    required this.onPressed,
    required this.currentPage,
  }) : super(
          currentIndex: currentPage,
          key: key,
          items: models.map((e) => _makeDest(e)).toList(),
          onTap: onPressed,
          backgroundColor: color,
        );
}
