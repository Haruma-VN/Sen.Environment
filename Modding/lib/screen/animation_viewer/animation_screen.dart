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
    if (mounted) {
      setState(() {});
    }
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
    widget.mediaScreen.updateUI = null;
    widget.labelScreen.updateUI = null;
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
            child: ConstrainedBox(
              constraints: BoxConstraints(
                maxWidth: MediaQuery.of(context).size.width * 0.6,
                maxHeight: MediaQuery.of(context).size.height * 0.6,
              ),
              child: FittedBox(
                fit: BoxFit.contain,
                child: _animationVisual,
              ),
            ),
          ),
        ),
      );
    } else {
      return ClipRRect(
        clipBehavior: Clip.hardEdge,
        child: _dragAndDropWidget(_uploadFile()),
      );
    }
  }

  Widget _buildSliderRow({
    required String label,
    required double value,
    required double min,
    required double max,
    required ValueChanged<double> onChanged,
  }) {
    return Card(
      child: Row(
        children: [
          const SizedBox(width: 16),
          Text(
            label,
            style: Theme.of(context)
                .textTheme
                .titleSmall!
                .copyWith(fontWeight: FontWeight.bold),
          ),
          const SizedBox(width: 8.0),
          Expanded(
            child: Tooltip(
              message: value.toStringAsFixed(2).toString(),
              child: Slider(
                value: value.clamp(min, max),
                min: min,
                max: max,
                onChanged: onChanged,
              ),
            ),
          ),
        ],
      ),
    );
  }

  void _zoomInAction() {
    scale *= 2;
    if (scale > 300) {
      scale = 300;
    }
    setState(() {});
  }

  void _zoomOutAction() {
    scale /= 2;
    if (scale < 0.1) {
      scale = 0.1;
    }
    setState(() {});
  }

  Widget _buildContainer() {
    return Expanded(
      child: Container(
        width: double.infinity,
        margin: const EdgeInsets.symmetric(horizontal: 16.0),
        decoration: BoxDecoration(
          borderRadius: BorderRadius.circular(20),
          border: Border.all(
            color: Theme.of(context).brightness == Brightness.dark
                ? Colors.white.withOpacity(0.3)
                : Colors.black.withOpacity(0.3),
            width: 2,
          ),
          gradient: LinearGradient(
            colors: Theme.of(context).brightness == Brightness.dark
                ? [Colors.grey[900]!, Colors.grey[800]!]
                : [Colors.white, Colors.grey[200]!],
            begin: Alignment.topLeft,
            end: Alignment.bottomRight,
          ),
          boxShadow: [
            BoxShadow(
              color: Theme.of(context).brightness == Brightness.dark
                  ? Colors.black.withOpacity(0.5)
                  : Colors.grey.withOpacity(0.3),
              blurRadius: 10,
              offset: const Offset(0, 5),
            ),
          ],
        ),
        child: _painterOrUpload(),
      ),
    );
  }

  @override
  Widget build(BuildContext context) {
    final los = AppLocalizations.of(context)!;
    return Column(
      children: [
        _buildContainer(),
        Padding(
          padding: const EdgeInsets.all(16.0),
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              _buildSliderRow(
                label: los.x,
                value: xOffset,
                min: -800,
                max: 800,
                onChanged: (value) {
                  setState(() {
                    xOffset = value;
                  });
                },
              ),
              const SizedBox(height: 4.0),
              const Divider(),
              const SizedBox(height: 4.0),
              _buildSliderRow(
                label: los.y,
                value: yOffset,
                min: -400,
                max: 400,
                onChanged: (value) {
                  setState(() {
                    yOffset = value;
                  });
                },
              ),
            ],
          ),
        ),
        Card(
          margin: const EdgeInsets.only(bottom: 16.0, left: 16.0, right: 16.0),
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(12),
          ),
          elevation: 4,
          child: Padding(
            padding: const EdgeInsets.all(12.0),
            child: Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                IconButton(
                  onPressed: _zoomOutAction,
                  icon: const Icon(Icons.zoom_out_outlined),
                  tooltip: 'Zoom Out',
                ),
                Tooltip(
                  message: 'Backward',
                  child: IconButton(
                    icon: const Icon(Icons.arrow_back_outlined),
                    onPressed: () {
                      VisualHelper.workingFrameRate /= 2;
                      if (VisualHelper.workingFrameRate <= 0) {
                        VisualHelper.workingFrameRate = 1;
                      }
                      setState(() {});
                    },
                  ),
                ),
                Tooltip(
                  message: _isPause ? 'Play' : 'Stop',
                  child: IconButton(
                    icon: !_isPause
                        ? const Icon(Icons.play_arrow_outlined)
                        : const Icon(Icons.pause_outlined),
                    onPressed: () {
                      setState(() {
                        _isPause = !_isPause;
                        if (_isPause) {
                          _animationController.stop();
                        } else {
                          _animationController.forward();
                        }
                      });
                    },
                  ),
                ),
                Tooltip(
                  message: 'Forward',
                  child: IconButton(
                    icon: const Icon(Icons.arrow_forward_outlined),
                    onPressed: () {
                      VisualHelper.workingFrameRate *= 2;
                      if (VisualHelper.workingFrameRate > 240) {
                        VisualHelper.workingFrameRate = 240;
                      }
                      setState(() {});
                    },
                  ),
                ),
                IconButton(
                  onPressed: _zoomInAction,
                  icon: const Icon(Icons.zoom_in_outlined),
                  tooltip: 'Zoom In',
                ),
              ],
            ),
          ),
        ),
      ],
    );
  }
}
