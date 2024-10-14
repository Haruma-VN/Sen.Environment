import 'dart:io';

import 'package:flutter/material.dart';
import 'package:modding/screen/animation_viewer/animation_screen.dart';
import 'package:modding/screen/animation_viewer/label_screen.dart';
import 'package:modding/screen/animation_viewer/media_screen.dart';
import 'package:modding/service/file_service.dart';
import 'package:modding/model/animation.dart' as animation;

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

  bool _hasFile = false;

  late animation.SexyAnimation _animation;

  void _onUploadFile() async {
    final file = await FileService.uploadFile();
    if (file != null) {
      _animation = animation.SexyAnimation.fromJson(
        await FileService.readJson(source: file),
      );
      setState(() {
        _hasFile = true;
        _updateScreens();
      });
    }
  }

  void _onDragFile(String file) async {
    _animation = animation.SexyAnimation.fromJson(
      await FileService.readJson(source: file),
    );
    setState(() {
      _hasFile = true;
      _updateScreens();
    });
  }

  void _updateScreens() {
    _screen = <Widget>[
      AnimationScreen(
        key: ValueKey(_hasFile),
        hasFile: _hasFile,
        onUploadFile: _onUploadFile,
        onDragFile: _onDragFile,
      ),
      LabelScreen(label: _label),
      MediaScreen(sprite: _sprite, image: _image, media: _media),
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
  }

  late List<Widget> _screen;

  void _onItemTapped(int index) {
    setState(() {
      _selectedIndex = index;
    });
  }

  Widget? _navigationBar() {
    if (Platform.isMacOS || Platform.isWindows || Platform.isLinux) {
      return null;
    }
    return BottomNavigationBar(
      items: const <BottomNavigationBarItem>[
        BottomNavigationBarItem(
          icon: Icon(Icons.home),
          label: 'Home',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.label_outline),
          label: 'Label',
        ),
        BottomNavigationBarItem(
          icon: Icon(Icons.image_outlined),
          label: 'Media',
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
      return Row(
        children: [
          Flexible(
            flex: 2,
            child: Container(
              margin: const EdgeInsets.only(left: 8.0, bottom: 8.0),
              decoration: BoxDecoration(
                borderRadius: BorderRadius.circular(20),
                border: Border.all(
                  color: MediaQuery.of(context).platformBrightness ==
                          Brightness.dark
                      ? Colors.white
                      : Colors.black,
                  width: 2,
                ),
              ),
              child: _screen[1],
            ),
          ),
          Flexible(flex: 15, child: _screen[0]),
          Flexible(
            flex: 3,
            child: Container(
              margin: const EdgeInsets.only(left: 12.0, bottom: 12.0),
              child: _screen[2],
            ),
          ),
        ],
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Animation Viewer'),
      ),
      body: _buildUI(),
      bottomNavigationBar: _navigationBar(),
    );
  }
}
