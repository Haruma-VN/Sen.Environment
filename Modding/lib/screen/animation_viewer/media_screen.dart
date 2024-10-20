import 'package:flutter/material.dart';
import 'package:modding/screen/animation_viewer/visual_helper.dart';

// ignore: must_be_immutable
class MediaScreen extends StatelessWidget {
  final List<String> sprite;

  final List<String> image;

  final List<String> media;

  late void Function() updateUI;

  MediaScreen({
    super.key,
    required this.sprite,
    required this.image,
    required this.media,
  });

  @override
  Widget build(BuildContext context) {
    return DefaultTabController(
      initialIndex: 0,
      length: 3,
      child: Scaffold(
        appBar: AppBar(
          automaticallyImplyLeading: false,
          bottom: const TabBar(
            tabs: <Widget>[
              Tab(
                icon: Icon(Icons.folder_outlined),
                text: 'Media',
              ),
              Tab(
                icon: Icon(Icons.folder_outlined),
                text: 'Image',
              ),
              Tab(
                icon: Icon(Icons.folder_outlined),
                text: 'Sprite',
              ),
            ],
          ),
        ),
        body: TabBarView(
          children: <Widget>[
            _MediaPage(media: media),
            _ImagePage(
              image: image,
              updateUI: updateUI,
            ),
            _SpritePage(
              sprite: sprite,
              updateUI: updateUI,
            ),
          ],
        ),
      ),
    );
  }
}

class _MediaPage extends StatelessWidget {
  const _MediaPage({
    required this.media,
  });

  final List<String> media;

  @override
  Widget build(BuildContext context) {
    return ListView.builder(
      itemCount: media.length,
      itemBuilder: (context, index) => ListTile(
        leading: VisualHelper.imageSource[index] == null
            ? const Icon(Icons.broken_image_outlined)
            : Image(
                image: VisualHelper.imageSource[index]!,
                width: 40,
                height: 40,
              ),
        title: Text(media[index]),
      ),
    );
  }
}

class _ImagePage extends StatefulWidget {
  const _ImagePage({
    required this.image,
    required this.updateUI,
  });

  final List<String> image;

  final void Function() updateUI;

  @override
  State<_ImagePage> createState() => _ImagePageState();
}

class _ImagePageState extends State<_ImagePage> {
  @override
  Widget build(BuildContext context) {
    return ListView.builder(
      itemCount: widget.image.length,
      itemBuilder: (context, index) => ListTile(
        leading: const Icon(Icons.image_outlined),
        title: Text(widget.image[index]),
        trailing: Checkbox(
          value: VisualHelper.selectImageList[index],
          onChanged: (bool? value) {
            setState(() {
              VisualHelper.selectImageList[index] =
                  value ?? !VisualHelper.selectImageList[index];
            });
            widget.updateUI();
          },
        ),
      ),
    );
  }
}

class _SpritePage extends StatefulWidget {
  const _SpritePage({
    required this.sprite,
    required this.updateUI,
  });

  final List<String> sprite;

  final void Function() updateUI;

  @override
  State<_SpritePage> createState() => _SpritePageState();
}

class _SpritePageState extends State<_SpritePage> {
  @override
  Widget build(BuildContext context) {
    return ListView.builder(
      itemCount: widget.sprite.length,
      itemBuilder: (context, index) => ListTile(
        leading: const Icon(Icons.image_outlined),
        title: Text(widget.sprite[index]),
        trailing: Checkbox(
            value: VisualHelper.selectSpriteList[index],
            onChanged: (bool? value) {
              setState(() {
                VisualHelper.selectSpriteList[index] =
                    value ?? !VisualHelper.selectSpriteList[index];
              });
              widget.updateUI();
            }),
      ),
    );
  }
}
