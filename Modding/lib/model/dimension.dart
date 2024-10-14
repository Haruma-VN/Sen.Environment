class Dimension {
  int width;
  int height;
  Dimension({
    required this.width,
    required this.height,
  });

  factory Dimension.empty({
    width = 0,
    height = 0,
  }) {
    throw UnimplementedError();
  }
}
