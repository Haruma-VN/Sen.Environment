// ignore_for_file: file_names, must_be_immutable

import 'package:flutter/material.dart';

class DropDownListView extends StatefulWidget {
  DropDownListView({
    super.key,
    required this.items,
    required this.value,
  });

  final List<String> items;

  String value;

  @override
  State<DropDownListView> createState() => _DropDownListViewState();
}

class _DropDownListViewState extends State<DropDownListView> {
  @override
  Widget build(BuildContext context) {
    return DropdownButton<String>(
      value: widget.value,
      isExpanded: true,
      focusColor: Colors.transparent,
      borderRadius: const BorderRadius.all(Radius.circular(10)),
      underline: Container(),
      items: widget.items
          .map((e) => DropdownMenuItem<String>(
                value: e,
                child: Text(e),
              ))
          .toList(),
      onChanged: (String? newValue) {
        setState(() {
          widget.value = newValue!;
        });
      },
    );
  }
}
