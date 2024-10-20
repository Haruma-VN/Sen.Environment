import 'package:flutter/material.dart';
import 'package:modding/model/animation.dart' as model;
import 'package:modding/service/file_service.dart';
import 'dart:collection';
import 'dart:math' as math;

final Matrix4 initialMatrix = Matrix4.identity();

final model.Color initialColor = [1.0, 1.0, 1.0, 1.0];

class LabelInfo {
  int startIndex;
  int endIndex;

  LabelInfo({required this.startIndex, required this.endIndex});
}

class VisualHelper {
  static late model.SexyAnimation animation;

  static List<ImageProvider?> imageSource = [];

  static bool hasAnimation = false;

  static bool hasMedia = false;

  static int workingSpriteIndex = 0;

  static double workingFrameRate = 30;

  static String currentLabel = "main";

  static Map<String, LabelInfo> labelInfo = {};

  static List<bool> selectImageList = [];

  static List<bool> selectSpriteList = [];

  static Future<void> loadAnimation(String path) async {
    dispose();
    animation = model.SexyAnimation.fromJson(
      await FileService.readJson(source: path),
    );
    return;
  }

  static void loadImageSource(
    String directory,
  ) {
    for (var image in animation.image) {
      var file = '$directory/${image.path}.png';
      var source = null as MemoryImage?;
      if (FileService.isFile(file)) {
        source = MemoryImage(FileService.readBuffer(source: file));
      }
      imageSource.add(source);
    }
    return;
  }

  static Matrix4 transformMatrixFromVariant(model.Transform transform) {
    final matrix = Matrix4.identity();
    switch (transform.length) {
      case 2:
        {
          matrix[12] = transform[0];
          matrix[13] = transform[1];
          break;
        }
      case 3:
        {
          final cos = math.cos(transform[0]);
          final sin = math.sin(transform[0]);
          matrix[0] = cos;
          matrix[1] = sin;
          matrix[4] = -sin;
          matrix[5] = cos;
          matrix[12] = transform[1];
          matrix[13] = transform[2];
          break;
        }
      case 6:
        {
          matrix[0] = transform[0];
          matrix[1] = transform[1];
          matrix[4] = transform[2];
          matrix[5] = transform[3];
          matrix[12] = transform[4];
          matrix[13] = transform[5];
          break;
        }
      default:
        break;
    }
    return matrix;
  }

  static ColorFilter makeColor(
    model.Color value,
  ) {
    var result = ColorFilter.mode(
        Color.fromARGB(
          (255.0 * value[3]).toInt(),
          (255.0 * value[0]).toInt(),
          (255.0 * value[1]).toInt(),
          (255.0 * value[2]).toInt(),
        ),
        BlendMode.modulate);
    return result;
  }

  static Color colorFromVariant(model.Color color) {
    return Color.fromRGBO((color[0] * 255).round(), (color[1] * 255).round(),
        (color[2] * 255).round(), color[3]);
  }

  static model.AnimationImage selectImage(int index) {
    var result = null as model.AnimationImage?;
    if (0 <= index && index < animation.image.length) {
      result = animation.image[index];
    } else {
      throw Exception();
    }
    return result;
  }

  static model.AnimationSprite selectSprite(
    int index,
  ) {
    var result = null as model.AnimationSprite?;
    if (0 <= index && index < animation.sprite.length) {
      result = animation.sprite[index];
    } else if (index == animation.sprite.length) {
      result = animation.mainSprite;
    } else {
      throw Exception();
    }
    return result;
  }

  //-----------------------

  static Widget visualizeImage(int index) {
    final image = selectImage(index);
    return Visibility(
        visible: VisualHelper.selectImageList[index],
        child: Transform(
            transform: transformMatrixFromVariant(image.transform),
            child: imageSource[index] == null
                ? Text('Missing ${image.path}')
                : Image(
                    image: imageSource[index]!,
                    width: image.dimension.width.toDouble(),
                    height: image.dimension.height.toDouble(),
                    fit: BoxFit.fill)));
  }

  static Widget visualizeSprite(
      int index, AnimationController animationController) {
    final sprite = selectSprite(index);
    final isMainSprite = index == animation.sprite.length;
    final layerList = SplayTreeMap<int, _VisualLayer>();
    var frameIndex = 0;

    for (final frame in sprite.frame) {
      for (final removeIndex in frame.remove) {
        var layer = layerList[removeIndex]!;
        layer.isRemoved = true;
      }
      for (var action in frame.append) {
        if (layerList.containsKey(action.index)) {
          throw Exception();
        }
        var layer = layerList[action.index] = _VisualLayer();
        var subController = isMainSprite
            ? animationController.drive(IntTween(
                begin: labelInfo[currentLabel]!.startIndex,
                end: labelInfo[currentLabel]!.endIndex - 1))
            : animationController.drive(
                IntTween(begin: 0, end: sprite.frame.length - 1),
              );
        layer.view = AnimatedBuilder(
          animation: subController,
          child: !action.sprite
              ? visualizeImage(action.resource)
              : visualizeSprite(action.resource, animationController),
          builder: (context, child) {
            var index = subController.value;
            var property = layer.property[index];
            if (property == null) {
              return const SizedBox();
            }
            return ColorFiltered(
              colorFilter: property.$2,
              child: Transform(
                transform: property.$1,
                child: child,
              ),
            );
          },
        );
        layer.property = List.filled(sprite.frame.length, null);
        layer.property[frameIndex] = (initialMatrix, makeColor(initialColor));
        layer.isRemoved = false;
        layer.isChanged = true;
      }
      for (var action in frame.change) {
        var layer = layerList[action.index]!;
        if (layer.isChanged) {
          layer.property[frameIndex] = (
            transformMatrixFromVariant(action.transform),
            action.color != null
                ? makeColor(action.color!)
                : layer.property[frameIndex]!.$2,
          );
        } else {
          layer.property[frameIndex] = (
            transformMatrixFromVariant(action.transform),
            action.color != null
                ? makeColor(action.color!)
                : layer.property[frameIndex - 1]!.$2,
          );
        }
        layer.isChanged = true;
      }
      for (var layer in layerList.values) {
        if (layer.isRemoved) {
          continue;
        }
        if (layer.isChanged) {
          layer.isChanged = false;
          continue;
        }
        layer.property[frameIndex] = layer.property[frameIndex - 1]!;
      }
      frameIndex++;
    }

    if (isMainSprite) {
      return Stack(
        fit: StackFit.passthrough,
        children: layerList.values.map((value) => value.view).toList(),
      );
    } else {
      return Visibility(
        visible: VisualHelper.selectSpriteList[index],
        child: Stack(
          fit: StackFit.passthrough,
          children: layerList.values.map((value) => value.view).toList(),
        ),
      );
    }
  }

  static dispose() {
    hasAnimation = false;
    hasMedia = false;
    workingFrameRate = 30;
    selectImageList = [];
    selectSpriteList = [];
    imageSource = [];
    labelInfo = {};
    currentLabel = "main";
  }
}

class _VisualLayer {
  late Widget view;
  late List<(Matrix4, ColorFilter)?> property;
  late bool isRemoved;
  late bool isChanged;
}
