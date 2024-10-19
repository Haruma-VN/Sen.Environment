// ignore_for_file: must_be_immutable

import 'package:flutter/material.dart';

class RadioButton extends StatefulWidget {
  RadioButton({
    super.key,
    required this.options,
    this.isRowProvider = true,
  }) {
    _currentOption = options[0];
  }

  final List<String> options;

  late String _currentOption;

  String get currentOption => _currentOption;

  final bool isRowProvider;

  @override
  State<RadioButton> createState() => _RadioButtonState();
}

class _RadioButtonState extends State<RadioButton> {
  Widget _buildRow() {
    return Row(
      children: [
        ...widget.options.map(
          (e) => Expanded(
            child: RadioListTile(
              title: Text(e),
              value: e,
              groupValue: widget._currentOption,
              onChanged: (ex) {
                setState(
                  () {
                    widget._currentOption = ex.toString();
                  },
                );
              },
            ),
          ),
        ),
      ],
    );
  }

  Widget _buildColumn() {
    return Column(
      children: [
        ...widget.options.map(
          (e) => RadioListTile(
            title: Text(e),
            value: e,
            groupValue: widget._currentOption,
            onChanged: (ex) {
              setState(() {
                widget._currentOption = ex.toString();
              });
            },
          ),
        ),
      ],
    );
  }

  @override
  Widget build(BuildContext context) {
    return widget.isRowProvider ? _buildRow() : _buildColumn();
  }
}
