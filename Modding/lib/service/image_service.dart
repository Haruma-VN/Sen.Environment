import 'package:modding/model/dimension.dart';
import 'package:modding/service/file_service.dart';
import 'package:image/image.dart';

/// Document : https://github.com/brendan-duncan/image/blob/main/doc/README.md

class ImageService {
  static void saveImage(
    String source,
    Image data,
  ) {
    FileService.writeBuffer(source: source, data: encodePng(data));
    return;
  }

  static void resizeImage(
    String source,
    Dimension newDimension,
    String destination,
  ) {
    final bytes = FileService.readBuffer(source: source);
    final image = decodeImage(bytes)!;
    final result = copyResize(
      image,
      width: newDimension.width,
      height: newDimension.height,
    );
    saveImage(
      destination,
      result,
    );
    return;
  }

  static Image readImage(
    String source,
  ) =>
      decodeImage(
        FileService.readBuffer(
          source: source,
        ),
      )!;

  static void cropImage({
    required Image original,
    required int x,
    required int y,
    required int width,
    required int height,
    required String destination,
  }) {
    final result = copyCrop(
      original,
      x: x,
      y: y,
      width: width,
      height: height,
    );
    saveImage(destination, result);
    return;
  }
}
