import 'package:fluent_ui/fluent_ui.dart';

class WinShellComboBox<T> extends StatelessWidget {
  const WinShellComboBox({
    super.key,
    required this.items,
    required this.value,
    required this.onPressed,
  });

  final List<T> items;

  final T value;

  final void Function(T?) onPressed;

  @override
  Widget build(BuildContext context) {
    return ComboBox<T>(
      value: this.value,
      onChanged: this.onPressed,
      items: this.items.map((e) {
        return ComboBoxItem(
          value: e,
          child: Text(e.toString()),
        );
      }).toList(),
    );
  }
}
