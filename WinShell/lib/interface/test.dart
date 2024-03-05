import 'package:flutter/material.dart';
import 'package:winshell/api/kernel.dart';
import 'package:winshell/api/shell.dart';

class CView extends StatefulWidget {
  CView({
    super.key,
    required this.kernel,
  });

  final List<String> list = [];

  final Kernel kernel;

  @override
  State<CView> createState() => _CViewStage();
}

class _CViewStage extends State<CView> implements Shell {
  @override
  Widget build(BuildContext context) {
    Kernel.gui = this;
    return Scaffold(
      body: ListView.separated(
        itemBuilder: (BuildContext context, int index) {
          return ListTile(
            title: Text(widget.list[index]),
          );
        },
        separatorBuilder: (context, index) => const Divider(),
        itemCount: widget.list.length,
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: () => widget.kernel.execute(),
        child: const Icon(Icons.message),
      ),
    );
  }

  @override
  void sendMessage(String message) {
    setState(() {
      widget.list.add(message);
    });
  }
}
