import 'dart:io';

import 'package:desktop_drop/desktop_drop.dart';
import 'package:flutter/material.dart';
import 'package:modding/screen/animation_viewer/animation_painter.dart';

class AnimationScreen extends StatefulWidget {
  const AnimationScreen({
    super.key,
    required this.hasFile,
    required this.onUploadFile,
    required this.onDragFile,
  });

  final bool hasFile;

  final void Function() onUploadFile;

  final void Function(String value) onDragFile;

  @override
  State<AnimationScreen> createState() => _AnimationScreenState();
}

class _AnimationScreenState extends State<AnimationScreen> {
  double xOffset = 0.0;
  double yOffset = 0.0;
  bool _dragging = false;

  Widget _uploadFile() {
    return InkWell(
      splashColor: Colors.blue.withAlpha(30),
      onTap: widget.onUploadFile,
      child: Center(
        child: _dragging
            ? const Text('Drop file to upload...')
            : const Text('Upload file to continue...'),
      ),
    );
  }

  Widget _dragAndDropWidget(Widget data) {
    if (Platform.isAndroid || Platform.isIOS) {
      return data;
    }
    return DropTarget(
      onDragEntered: (details) {
        setState(() {
          _dragging = true;
        });
      },
      onDragExited: (details) {
        setState(() {
          _dragging = false;
        });
      },
      onDragDone: (details) {
        if (details.files.isNotEmpty) {
          var file = details.files.first;
          widget.onDragFile(file.path);
        }
      },
      child: data,
    );
  }

  Widget _painterOrUpload() {
    if (widget.hasFile) {
      return ClipRRect(
        borderRadius: BorderRadius.circular(20),
        child: CustomPaint(
          painter: AnimationPainter(x: xOffset, y: yOffset),
        ),
      );
    } else {
      return ClipRRect(
        clipBehavior: Clip.hardEdge,
        child: _dragAndDropWidget(_uploadFile()),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Expanded(
          child: Container(
            width: double.infinity,
            margin: const EdgeInsets.symmetric(horizontal: 16.0),
            decoration: BoxDecoration(
              borderRadius: BorderRadius.circular(20),
              border: Border.all(
                color:
                    MediaQuery.of(context).platformBrightness == Brightness.dark
                        ? Colors.white
                        : Colors.black,
                width: 2,
              ),
            ),
            child: _painterOrUpload(),
          ),
        ),
        Padding(
          padding: const EdgeInsets.all(8.0),
          child: Column(
            children: [
              Container(
                margin: const EdgeInsets.symmetric(horizontal: 16.0),
                child: Row(
                  children: [
                    const Text('X'),
                    Expanded(
                      child: Slider(
                        value: xOffset,
                        min: 0,
                        max: 1000,
                        onChanged: (value) {
                          setState(() {
                            xOffset = value;
                          });
                        },
                      ),
                    ),
                  ],
                ),
              ),
              Container(
                margin: const EdgeInsets.symmetric(horizontal: 16.0),
                child: Row(
                  children: [
                    const Text('Y'),
                    Expanded(
                      child: Slider(
                        value: yOffset,
                        min: 0,
                        max: 200,
                        onChanged: (value) {
                          setState(() {
                            yOffset = value;
                          });
                        },
                      ),
                    ),
                  ],
                ),
              ),
            ],
          ),
        ),
        Container(
          margin: const EdgeInsets.only(bottom: 8.0),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              IconButton(
                icon: const Icon(Icons.arrow_back),
                onPressed: () {},
              ),
              IconButton(
                icon: const Icon(Icons.play_arrow),
                onPressed: () {},
              ),
              IconButton(
                icon: const Icon(Icons.arrow_forward),
                onPressed: () {},
              ),
            ],
          ),
        ),
      ],
    );
  }
}
