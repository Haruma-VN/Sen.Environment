class TreeNodeData {
  String name;
  bool isFile;
  List<TreeNodeData> children;
  final String path;

  TreeNodeData({
    required this.name,
    this.isFile = false,
    required this.children,
    required this.path,
  });
}
