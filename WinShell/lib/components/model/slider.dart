// ignore_for_file: unnecessary_this, non_constant_identifier_names

import 'package:fluent_ui/fluent_ui.dart';

class WinShellSlider extends StatefulWidget {
  const WinShellSlider({
    super.key,
    required this.value,
    required this.minValue,
    required this.maxValue,
  });
  final double value;
  final double minValue;
  final double maxValue;

  @override
  State<WinShellSlider> createState() => _WinShellSliderState();
}

class _WinShellSliderState extends State<WinShellSlider> {
  @override
  void initState() {
    super.initState();
    this.value = widget.value;
    this.minValue = widget.minValue;
    this.maxValue = widget.maxValue;
  }

  late double value;

  late double minValue;

  late double maxValue;

  @override
  Widget build(BuildContext context) {
    return Slider(
      label: '${value.toInt()}',
      value: value,
      onChanged: (v) => setState(() => value = v),
      min: this.minValue,
      max: this.maxValue,
    );
  }
}
