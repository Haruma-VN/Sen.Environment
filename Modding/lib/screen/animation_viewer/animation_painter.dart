import 'package:flutter/material.dart';
import 'package:modding/model/animation.dart' as animation;
import 'dart:math' as math;

class AnimationPainter extends CustomPainter {
  final double x;
  final double y;

  AnimationPainter({required this.x, required this.y});

  animation.Matrix variantToMatrix(animation.Transform transform) {
    switch (transform.length) {
      case 2:
        {
          return [1.0, 0.0, 0.0, 1.0, transform[0], transform[1]];
        }
      case 3:
        {
          final cos = math.cos(transform[0]);
          final sin = math.sin(transform[0]);
          return [cos, sin, -sin, cos, transform[1], transform[2]];
        }
      case 6:
        {
          return [...transform];
        }
      default:
        {
          throw Exception('invalid convert');
        }
    }
  }

  animation.Color mixColor(animation.Color source, animation.Color change) {
    return [
      change[0] * source[0],
      change[1] * source[1],
      change[2] * source[2],
      change[3] * source[3],
    ];
  }

  animation.Matrix mixMatrix(animation.Matrix source, animation.Matrix change) {
    return [
      change[0] * source[0] + change[2] * source[1],
      change[1] * source[0] + change[3] * source[1],
      change[0] * source[2] + change[2] * source[3],
      change[1] * source[2] + change[3] * source[3],
      change[0] * source[4] + change[2] * source[5] + change[4],
      change[1] * source[4] + change[3] * source[5] + change[5],
    ];
  }

  @override
  void paint(Canvas canvas, Size size) {
    var xStart = 10 + x;
    var yStart = 10 + y;
    if (xStart < 0) xStart = 0;
    if (yStart < 0) yStart = 0;

    final paint = Paint()
      ..color = Colors.blue
      ..style = PaintingStyle.stroke
      ..strokeWidth = 4;
    canvas.drawRect(
      Rect.fromLTWH(xStart, yStart, size.width - 1000, size.height - 200),
      paint,
    );
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) {
    return true;
  }
}
