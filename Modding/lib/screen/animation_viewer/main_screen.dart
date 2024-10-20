import 'dart:io';

import 'package:flutter/material.dart';
import 'package:modding/screen/animation_viewer/animation_screen.dart';
import 'package:modding/screen/animation_viewer/label_screen.dart';
import 'package:modding/screen/animation_viewer/media_screen.dart';
import 'package:modding/service/file_service.dart';
import 'package:modding/screen/animation_viewer/visual_helper.dart';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';

class AnimationViewer extends StatefulWidget {
  const AnimationViewer({super.key});

  @override
  State<AnimationViewer> createState() => _AnimationViewerState();
}

class _AnimationViewerState extends State<AnimationViewer> {
  int _selectedIndex = 0;

  late List<String> _sprite;
  late List<String> _image;
  late List<String> _media;
  late List<String> _label;
  late TextEditingController _controller;

  Future<void> _onUploadMedia() async {
    final los = AppLocalizations.of(context)!;
    await showDialog(
      context: context,
      builder: (context) => AlertDialog(
        title: Text(los.upload_media),
        content: Row(
          children: [
            const Icon(Icons.abc_outlined),
            Expanded(
              child: TextField(
                controller: _controller,
              ),
            ),
            IconButton(
              onPressed: () async {
                final directory = await FileService.uploadDirectory();
                if (directory != null) {
                  _controller.text = directory;
                }
              },
              icon: const Icon(Icons.drive_folder_upload_outlined),
            ),
          ],
        ),
        actions: [
          TextButton(
            onPressed: () {
              VisualHelper.loadImageSource(_controller.text);
              VisualHelper.hasMedia = true;
              Navigator.of(context).pop();
            },
            child: Text(los.okay),
          ),
        ],
      ),
    );
  }

  void _loadMedia() {
    for (final image in VisualHelper.animation.image) {
      _image.add(image.path);
      _media.add('${image.path}.png');
      VisualHelper.selectImageList.add(true);
    }
    for (final sprite in VisualHelper.animation.sprite) {
      _sprite.add(sprite.name);
      VisualHelper.selectSpriteList.add(true);
    }
    var labelName = "main";
    _label.add("main");
    VisualHelper.labelInfo[labelName] = LabelInfo(
        startIndex: 0,
        endIndex: VisualHelper.animation.mainSprite.frame.length - 1);
    for (int frameIndex = 0;
        frameIndex < VisualHelper.animation.mainSprite.frame.length;
        ++frameIndex) {
      final frameLabelName =
          VisualHelper.animation.mainSprite.frame[frameIndex].label;
      if (frameLabelName != "" && frameLabelName != labelName) {
        labelName = frameLabelName;
        VisualHelper.labelInfo[labelName] =
            LabelInfo(startIndex: frameIndex, endIndex: frameIndex);
        _label.add(labelName);
      }
      ++VisualHelper.labelInfo[labelName]!.endIndex;
    }
  }

  void _onUploadFile() async {
    final file = await FileService.uploadFile();
    if (file != null) {
      await VisualHelper.loadAnimation(file);
      VisualHelper.hasAnimation = true;
      await _onUploadMedia();
      _loadMedia();
      setState(() {
        _updateScreens();
      });
    }
  }

  void _onDragFile(String file) async {
    await VisualHelper.loadAnimation(file);
    VisualHelper.hasAnimation = true;
    _onUploadMedia();
    _loadMedia();
    setState(() {
      _updateScreens();
    });
  }

  void _updateScreens() {
    final mediaScreen = MediaScreen(
      sprite: _sprite,
      image: _image,
      media: _media,
    );
    final labelScreen = LabelScreen(label: _label);
    _screen = <Widget>[
      AnimationScreen(
        key: ValueKey(VisualHelper.hasAnimation),
        onUploadFile: _onUploadFile,
        onDragFile: _onDragFile,
        hasFile: false,
        mediaScreen: mediaScreen,
        labelScreen: labelScreen,
      ),
      labelScreen,
      mediaScreen,
    ];
  }

  @override
  void initState() {
    super.initState();
    _sprite = [];
    _image = [];
    _media = [];
    _label = [];
    _updateScreens();
    _controller = TextEditingController();
  }

  @override
  void dispose() {
    _controller.dispose();
    super.dispose();
    VisualHelper.dispose();
  }

  late List<Widget> _screen;

  void _onItemTapped(int index) {
    setState(() {
      _selectedIndex = index;
    });
  }

  Widget? _navigationBar() {
    final los = AppLocalizations.of(context)!;
    if (Platform.isMacOS || Platform.isWindows || Platform.isLinux) {
      return null;
    }
    return BottomNavigationBar(
      items: <BottomNavigationBarItem>[
        BottomNavigationBarItem(
          icon: const Icon(Icons.home),
          label: los.home,
        ),
        BottomNavigationBarItem(
          icon: const Icon(Icons.label_outline),
          label: los.label,
        ),
        BottomNavigationBarItem(
          icon: const Icon(Icons.image_outlined),
          label: los.media,
        ),
      ],
      currentIndex: _selectedIndex,
      onTap: _onItemTapped,
    );
  }

  Widget _buildUI() {
    if (Platform.isAndroid || Platform.isIOS) {
      return _screen[_selectedIndex];
    } else {
      return Padding(
        padding: const EdgeInsets.all(8.0),
        child: Row(
          children: [
            Flexible(
              flex: 4,
              child: Container(
                margin: const EdgeInsets.only(left: 8.0, bottom: 8.0),
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
                child: Padding(
                  padding: const EdgeInsets.all(8.0),
                  child: _screen[1],
                ),
              ),
            ),
            Flexible(flex: 12, child: _screen[0]),
            Flexible(
              flex: 4,
              child: Container(
                margin: const EdgeInsets.only(left: 12.0, bottom: 12.0),
                child: _screen[2],
              ),
            ),
          ],
        ),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    final los = AppLocalizations.of(context)!;
    return Scaffold(
      appBar: AppBar(
        title: Text(los.animation_viewer),
      ),
      body: _buildUI(),
      bottomNavigationBar: _navigationBar(),
    );
  }
}
