import 'package:flutter/material.dart';
import 'package:modding/model/manifest.dart';

class ManifestProvider with ChangeNotifier {
  late Manifest _manifest;

  Manifest get manifest => _manifest;

  set manifest(Manifest value) {
    _manifest = value;
  }
}
