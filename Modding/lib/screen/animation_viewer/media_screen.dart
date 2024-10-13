import 'package:flutter/material.dart';

class MediaScreen extends StatelessWidget {
  final List<String> sprite;

  final List<String> image;

  final List<String> media;

  const MediaScreen({
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
            _ImagePage(image: image),
            _SpritePage(sprite: sprite),
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
        leading: const Icon(Icons.image_outlined),
        title: Text(media[index]),
      ),
    );
  }
}

class _ImagePage extends StatelessWidget {
  const _ImagePage({
    required this.image,
  });

  final List<String> image;

  @override
  Widget build(BuildContext context) {
    return ListView.builder(
      itemCount: image.length,
      itemBuilder: (context, index) => ListTile(
        leading: const Icon(Icons.image_outlined),
        title: Text(image[index]),
        trailing: Checkbox(value: true, onChanged: (e) {}),
      ),
    );
  }
}

class _SpritePage extends StatelessWidget {
  const _SpritePage({
    required this.sprite,
  });

  final List<String> sprite;

  @override
  Widget build(BuildContext context) {
    return ListView.builder(
      itemCount: sprite.length,
      itemBuilder: (context, index) => ListTile(
        leading: const Icon(Icons.image_outlined),
        title: Text(sprite[index]),
        trailing: Checkbox(value: true, onChanged: (e) {}),
      ),
    );
  }
}
