import 'package:flutter/material.dart';
import 'package:modding/screen/animation_viewer/visual_helper.dart';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';

// ignore: must_be_immutable
class MediaScreen extends StatelessWidget {
  final List<String> sprite;
  final List<String> image;
  final List<String> media;

  void Function()? updateUI;

  MediaScreen({
    super.key,
    required this.sprite,
    required this.image,
    required this.media,
  });

  @override
  Widget build(BuildContext context) {
    final los = AppLocalizations.of(context)!;
    return DefaultTabController(
      initialIndex: 0,
      length: 3,
      child: Scaffold(
        appBar: AppBar(
          automaticallyImplyLeading: false,
          centerTitle: true,
          backgroundColor: Colors.transparent,
          bottom: PreferredSize(
            preferredSize: const Size.fromHeight(48.0),
            child: Container(
              decoration: BoxDecoration(
                border: Border(
                  bottom: BorderSide(
                    color: Theme.of(context).dividerColor,
                    width: 2.0,
                  ),
                ),
              ),
              child: TabBar(
                indicatorColor: Theme.of(context).colorScheme.secondary,
                tabs: <Widget>[
                  Tab(
                    icon: const Icon(Icons.folder_outlined),
                    text: los.media,
                  ),
                  Tab(
                    icon: const Icon(Icons.folder_outlined),
                    text: los.image,
                  ),
                  Tab(
                    icon: const Icon(Icons.folder_outlined),
                    text: los.sprite,
                  ),
                ],
              ),
            ),
          ),
        ),
        body: Padding(
          padding: const EdgeInsets.all(8.0),
          child: TabBarView(
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
      itemBuilder: (context, index) => Card(
        margin: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
        elevation: 4,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(12),
        ),
        child: ListTile(
          contentPadding: const EdgeInsets.all(12),
          leading: VisualHelper.imageSource[index] == null
              ? const Icon(Icons.broken_image_outlined, size: 40)
              : ClipRRect(
                  borderRadius: BorderRadius.circular(8),
                  child: Image(
                    image: VisualHelper.imageSource[index]!,
                    width: 40,
                    height: 40,
                    fit: BoxFit.cover,
                  ),
                ),
          title:
              Text(media[index], style: Theme.of(context).textTheme.titleSmall),
        ),
      ),
    );
  }
}

// ignore: must_be_immutable
class _ImagePage extends StatefulWidget {
  _ImagePage({
    required this.image,
    required this.updateUI,
  });

  final List<String> image;

  void Function()? updateUI;

  @override
  State<_ImagePage> createState() => _ImagePageState();
}

class _ImagePageState extends State<_ImagePage> {
  @override
  Widget build(BuildContext context) {
    return ListView.builder(
      itemCount: widget.image.length,
      itemBuilder: (context, index) => Card(
        margin: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
        elevation: 4,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(12),
        ),
        child: ListTile(
          contentPadding: const EdgeInsets.all(12),
          leading: const Icon(Icons.image_outlined, size: 40),
          title: Text(widget.image[index],
              style: Theme.of(context).textTheme.titleSmall),
          trailing: Checkbox(
            value: VisualHelper.selectImageList[index],
            onChanged: (bool? value) {
              setState(
                () {
                  VisualHelper.selectImageList[index] =
                      value ?? !VisualHelper.selectImageList[index];
                },
              );
              widget.updateUI?.call();
            },
          ),
        ),
      ),
    );
  }
}

// ignore: must_be_immutable
class _SpritePage extends StatefulWidget {
  _SpritePage({
    required this.sprite,
    this.updateUI,
  });

  final List<String> sprite;

  void Function()? updateUI;

  @override
  State<_SpritePage> createState() => _SpritePageState();
}

class _SpritePageState extends State<_SpritePage> {
  @override
  Widget build(BuildContext context) {
    return ListView.builder(
      itemCount: widget.sprite.length,
      itemBuilder: (context, index) => Card(
        margin: const EdgeInsets.symmetric(horizontal: 16, vertical: 8),
        elevation: 4,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(12),
        ),
        child: ListTile(
          contentPadding: const EdgeInsets.all(12),
          leading: const Icon(Icons.image_outlined, size: 40),
          title: Text(widget.sprite[index],
              style: Theme.of(context).textTheme.titleSmall),
          trailing: Checkbox(
            value: VisualHelper.selectSpriteList[index],
            onChanged: (bool? value) {
              setState(() {
                VisualHelper.selectSpriteList[index] =
                    value ?? !VisualHelper.selectSpriteList[index];
              });
              widget.updateUI?.call();
            },
          ),
        ),
      ),
    );
  }
}
