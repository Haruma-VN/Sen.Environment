// ignore_for_file: file_names

import 'package:flutter/material.dart';

class AppBarView extends SliverAppBar {
  final double? height;

  final String appTitle;

  final EdgeInsetsGeometry? titlePadding;

  AppBarView({
    super.key,
    required this.appTitle,
    this.titlePadding,
    this.height,
  }) : super(
          expandedHeight: height,
          floating: false,
          pinned: true,
          flexibleSpace: LayoutBuilder(
            builder: ((BuildContext context, BoxConstraints constraints) {
              return FlexibleSpaceBar(
                title: Text(
                  appTitle,
                  style: Theme.of(context).textTheme.titleLarge,
                ),
                titlePadding: titlePadding,
                collapseMode: CollapseMode.parallax,
              );
            }),
          ),
        );
}
