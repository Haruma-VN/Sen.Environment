import 'dart:convert';

class ScriptData {
  final int kernel;
  final int script;
  final List<Holder> data;

  ScriptData({
    required this.kernel,
    required this.script,
    required this.data,
  });

  Map<String, dynamic> toJson() {
    return {
      'kernel': kernel,
      'script': script,
      'data': data.map((holder) => holder.toJson()).toList(),
    };
  }

  String toJsonString() {
    return jsonEncode(toJson());
  }

  factory ScriptData.fromJson(Map<String, dynamic> json) {
    return ScriptData(
      kernel: json['kernel'],
      script: json['script'],
      data:
          (json['data'] as List).map((item) => Holder.fromJson(item)).toList(),
    );
  }
}

class Holder {
  final String path;
  final String name;
  final String description;

  Holder({
    required this.path,
    required this.name,
    required this.description,
  });

  factory Holder.fromJson(Map<String, dynamic> json) {
    return Holder(
      path: json['path'],
      name: json['name'],
      description: json['description'],
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'path': path,
      'name': name,
      'description': description,
    };
  }
}
