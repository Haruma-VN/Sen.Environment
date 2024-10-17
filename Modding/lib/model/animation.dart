typedef Transform = List<double>;

typedef Color = List<double>;

typedef Matrix = List<double>;

class FrameNode {
  int index;
  int duration;
  int resource;
  bool sprite;
  int? firstFrame;
  Transform transform;
  Color color;

  FrameNode({
    required this.index,
    required this.duration,
    required this.resource,
    required this.sprite,
    this.firstFrame,
    required this.transform,
    required this.color,
  });

  factory FrameNode.fromJson(Map<String, dynamic> json) {
    return FrameNode(
      index: (json['index']),
      duration: (json['duration']),
      resource: json['resource'],
      sprite: json['sprite'],
      firstFrame: json['first_frame'] != null ? (json['first_frame']) : null,
      transform: List<double>.from(json['transform']),
      color: List<double>.from(json['color']),
    );
  }
}

typedef FrameNodeList = List<FrameNode>;

typedef FrameNodeStructure = Map<int, FrameNodeList>;

enum State {
  stateNull,
  stateFalse,
  stateTrue,
}

class Model {
  State state;
  int resource;
  bool sprite;
  Transform transform;
  Color color;
  int frameStart;
  int frameDuration;
  int? index;

  Model({
    required this.state,
    required this.resource,
    required this.sprite,
    required this.transform,
    required this.color,
    required this.frameStart,
    required this.frameDuration,
    this.index,
  });

  factory Model.fromJson(Map<String, dynamic> json) {
    return Model(
      state: State.values[json['state']],
      resource: json['resource'],
      sprite: json['sprite'],
      transform: List<double>.from(json['transform']),
      color: List<double>.from(json['color']),
      frameStart: (json['frame_start']),
      frameDuration: (json['frame_duration']),
      index: json['index'] != null ? (json['index']) : null,
    );
  }
}

class SexyAnimation {
  int version;
  int frameRate;
  AnimationPosition position;
  AnimationSize size;
  List<AnimationImage> image;
  List<AnimationSprite> sprite;
  AnimationSprite mainSprite;

  SexyAnimation({
    required this.version,
    required this.frameRate,
    required this.position,
    required this.size,
    required this.image,
    required this.sprite,
    required this.mainSprite,
  });

  factory SexyAnimation.fromJson(Map<String, dynamic> json) {
    return SexyAnimation(
      version: (json['version']),
      frameRate: (json['frame_rate']),
      position: AnimationPosition.fromJson(json['position']),
      size: AnimationSize.fromJson(json['size']),
      image: (json['image'] as List)
          .map((i) => AnimationImage.fromJson(i))
          .toList(),
      sprite: (json['sprite'] as List)
          .map((i) => AnimationSprite.fromJson(i))
          .toList(),
      mainSprite: AnimationSprite.fromJson(json['main_sprite']),
    );
  }
}

class AnimationPosition {
  double x;
  double y;

  AnimationPosition({
    required this.x,
    required this.y,
  });

  factory AnimationPosition.fromJson(Map<String, dynamic> json) {
    return AnimationPosition(
      x: json['x'].toDouble(),
      y: json['y'].toDouble(),
    );
  }
}

class AnimationSize {
  double width;
  double height;

  AnimationSize({
    required this.width,
    required this.height,
  });

  factory AnimationSize.fromJson(Map<String, dynamic> json) {
    return AnimationSize(
      width: json['width'].toDouble(),
      height: json['height'].toDouble(),
    );
  }
}

class AnimationImage {
  String path;
  String id;
  AnimationDimension dimension;
  List<double> transform;

  AnimationImage({
    required this.path,
    required this.id,
    required this.dimension,
    required this.transform,
  });

  factory AnimationImage.fromJson(Map<String, dynamic> json) {
    return AnimationImage(
      path: json['path'],
      id: json['id'],
      dimension: AnimationDimension.fromJson(json['dimension']),
      transform: List<double>.from(json['transform']),
    );
  }
}

class AnimationDimension {
  int width;
  int height;

  AnimationDimension({
    required this.width,
    required this.height,
  });

  factory AnimationDimension.fromJson(Map<String, dynamic> json) {
    return AnimationDimension(
      width: json['width'],
      height: json['height'],
    );
  }
}

class AnimationSprite {
  String name;
  AnimationWorkArea workArea;
  List<AnimationFrame> frame;

  AnimationSprite({
    required this.name,
    required this.workArea,
    required this.frame,
  });

  factory AnimationSprite.fromJson(Map<String, dynamic> json) {
    return AnimationSprite(
      name: json['name'],
      workArea: AnimationWorkArea.fromJson(json['work_area']),
      frame: (json['frame'] as List)
          .map((i) => AnimationFrame.fromJson(i))
          .toList(),
    );
  }
}

class AnimationWorkArea {
  int start;
  int duration;

  AnimationWorkArea({
    required this.start,
    required this.duration,
  });

  factory AnimationWorkArea.fromJson(Map<String, dynamic> json) {
    return AnimationWorkArea(
      start: (json['start']),
      duration: (json['duration']),
    );
  }
}

class AnimationFrame {
  String label;
  bool stop;
  List<AnimationCommand> command;
  List<int> remove;
  List<AnimationAppend> append;
  List<AnimationMove> change;

  AnimationFrame({
    required this.label,
    required this.stop,
    required this.command,
    required this.remove,
    required this.append,
    required this.change,
  });

  factory AnimationFrame.fromJson(Map<String, dynamic> json) {
    return AnimationFrame(
      label: json['label'],
      stop: json['stop'],
      command: (json['command'] as List)
          .map((i) => AnimationCommand.fromJson(i))
          .toList(),
      remove:
          (json['remove'] as List).map((i) => int.parse(i.toString())).toList(),
      append: (json['append'] as List)
          .map((i) => AnimationAppend.fromJson(i))
          .toList(),
      change: (json['change'] as List)
          .map((i) => AnimationMove.fromJson(i))
          .toList(),
    );
  }
}

class AnimationCommand {
  String command;
  String argument;

  AnimationCommand({
    required this.command,
    required this.argument,
  });

  factory AnimationCommand.fromJson(Map<String, dynamic> json) {
    return AnimationCommand(
      command: json['command'],
      argument: json['argument'],
    );
  }
}

class AnimationAppend {
  int index;
  String name;
  int resource;
  bool sprite;
  bool additive;
  int preloadFrame;
  double timeScale;

  AnimationAppend({
    required this.index,
    required this.name,
    required this.resource,
    required this.sprite,
    required this.additive,
    required this.preloadFrame,
    required this.timeScale,
  });

  factory AnimationAppend.fromJson(Map<String, dynamic> json) {
    return AnimationAppend(
      index: (json['index']),
      name: json['name'],
      resource: (json['resource']),
      sprite: json['sprite'],
      additive: json['additive'],
      preloadFrame: (json['preload_frame']),
      timeScale: json['time_scale'].toDouble(),
    );
  }
}

class AnimationMove {
  int index;
  List<double> transform;
  Color? color;
  List<double>? sourceRectangle;
  int spriteFrameNumber;

  AnimationMove({
    required this.index,
    required this.transform,
    this.color,
    this.sourceRectangle,
    required this.spriteFrameNumber,
  });

  factory AnimationMove.fromJson(Map<String, dynamic> json) {
    return AnimationMove(
      index: (json['index']),
      transform: List<double>.from(json['transform']),
      color: json['color'] != null ? List<double>.from(json['color']) : null,
      sourceRectangle: json['source_rectangle'] != null
          ? List<double>.from(json['source_rectangle'])
          : null,
      spriteFrameNumber: (json['sprite_frame_number']),
    );
  }
}
