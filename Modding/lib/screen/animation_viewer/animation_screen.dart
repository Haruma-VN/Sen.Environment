import 'dart:io';

import 'package:desktop_drop/desktop_drop.dart';
import 'package:flutter/material.dart';
import 'package:modding/screen/animation_viewer/label_screen.dart';
import 'package:modding/screen/animation_viewer/media_screen.dart';
import 'package:modding/screen/animation_viewer/visual_helper.dart';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';

class AnimationScreen extends StatefulWidget {
  const AnimationScreen({
    super.key,
    required this.hasFile,
    required this.onUploadFile,
    required this.onDragFile,
    required this.mediaScreen,
    required this.labelScreen,
  });

  final bool hasFile;

  final void Function() onUploadFile;

  final void Function(String value) onDragFile;

  final MediaScreen mediaScreen;

  final LabelScreen labelScreen;

  @override
  State<AnimationScreen> createState() => _AnimationScreenState();
}

class _AnimationScreenState extends State<AnimationScreen>
    with SingleTickerProviderStateMixin {
  double xOffset = 0.0;
  double yOffset = 0.0;
  bool _dragging = false;
  double scale = 1.0;

  late Widget? _animationVisual;
  late AnimationController _animationController;
  bool _isPause = false;

  Widget _uploadFile() {
    final los = AppLocalizations.of(context)!;
    return InkWell(
      splashColor: Colors.blue.withAlpha(30),
      onTap: widget.onUploadFile,
      child: Center(
        child: _dragging
            ? Text(los.drop_file_to_upload)
            : Text(los.upload_file_to_continue),
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
          if (VisualHelper.hasAnimation) {
            VisualHelper.workingFrameRate =
                VisualHelper.animation.frameRate.toDouble();
          }
        }
      },
      child: data,
    );
  }

  void _loadWorkingSprite(int index) {
    setState(() {
      final labelInfo = VisualHelper.labelInfo[VisualHelper.currentLabel]!;
      final duration = ((labelInfo.endIndex - labelInfo.startIndex) /
              VisualHelper.workingFrameRate *
              1000)
          .toInt();
      _animationController.duration = Duration(milliseconds: duration);
      _animationVisual =
          VisualHelper.visualizeSprite(index, _animationController);
      _animationVisual = SizedBox.fromSize(
        size: Size(VisualHelper.animation.size.width,
            VisualHelper.animation.size.height),
        child: UnconstrainedBox(
          child: SizedOverflowBox(
            alignment: AlignmentDirectional.topStart,
            size: Size(VisualHelper.animation.size.width,
                VisualHelper.animation.size.height),
            child: _animationVisual,
          ),
        ),
      );
      if (!_isPause) {
        _animationController.repeat();
      }
    });
  }

  void _resetUI() {
    setState(() {});
  }

  @override
  void initState() {
    super.initState();
    _animationController = AnimationController(vsync: this);
    _animationVisual = null;
    widget.mediaScreen.updateUI = _resetUI;
    widget.labelScreen.updateUI = _resetUI;
  }

  @override
  void dispose() {
    _animationController.dispose();
    super.dispose();
    return;
  }

  Widget _painterOrUpload() {
    if (VisualHelper.hasAnimation && VisualHelper.hasMedia) {
      _loadWorkingSprite(VisualHelper.animation.sprite.length);
      return ClipRRect(
        borderRadius: BorderRadius.circular(20),
        child: Transform(
            transform:
                VisualHelper.transformMatrixFromVariant([xOffset, yOffset]),
            child: Transform.scale(
              scale: scale,
              child: _animationVisual,
            )),
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
    final los = AppLocalizations.of(context)!;
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
                    Text(los.x),
                    Expanded(
                      child: Slider(
                        value: xOffset,
                        min: -800,
                        max: 800,
                        onChanged: (value) {
                          setState(
                            () {
                              xOffset = value;
                            },
                          );
                        },
                      ),
                    ),
                    IconButton(
                      onPressed: () {
                        scale *= 2;
                        if (scale > 300) {
                          scale = 300;
                        }
                        setState(() {});
                      },
                      icon: const Icon(Icons.zoom_in),
                    ),
                  ],
                ),
              ),
              Container(
                margin: const EdgeInsets.symmetric(horizontal: 16.0),
                child: Row(
                  children: [
                    Text(los.y),
                    Expanded(
                      child: Slider(
                        value: yOffset,
                        min: -400,
                        max: 400,
                        onChanged: (value) {
                          setState(() {
                            yOffset = value;
                          });
                        },
                      ),
                    ),
                    IconButton(
                      onPressed: () {
                        scale /= 2;
                        if (scale < 0.1) {
                          scale = 0.1;
                        }
                        setState(() {});
                      },
                      icon: const Icon(Icons.zoom_out),
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
                onPressed: () {
                  VisualHelper.workingFrameRate /= 2;
                  if (VisualHelper.workingFrameRate <= 0) {
                    VisualHelper.workingFrameRate = 1;
                  }
                  setState(() {});
                },
              ),
              IconButton(
                icon: !_isPause
                    ? const Icon(Icons.play_arrow)
                    : const Icon(Icons.pause),
                onPressed: () {
                  if (!_isPause) {
                    _isPause = true;
                    _animationController.stop();
                  } else {
                    _isPause = false;
                    _animationController.forward();
                  }
                  setState(() {});
                },
              ),
              IconButton(
                icon: const Icon(Icons.arrow_forward),
                onPressed: () {
                  VisualHelper.workingFrameRate *= 2;
                  if (VisualHelper.workingFrameRate > 240) {
                    VisualHelper.workingFrameRate = 240;
                  }
                  setState(() {});
                },
              ),
            ],
          ),
        ),
      ],
    );
  }
}
