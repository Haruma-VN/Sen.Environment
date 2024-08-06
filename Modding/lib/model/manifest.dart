class Manifest {
  int version;
  int textureFormatVersion;
  int textureFormatCategory;
  ResourceInfo resourceInfo;
  PackageInfo packageInfo;
  List<String> group;

  Manifest({
    required this.version,
    required this.textureFormatVersion,
    required this.textureFormatCategory,
    required this.resourceInfo,
    required this.packageInfo,
    required this.group,
  });

  factory Manifest.fromJson(Map<String, dynamic> json) {
    return Manifest(
      version: json['version'],
      textureFormatVersion: json['texture_information_version'],
      textureFormatCategory: json['texture_format_category'],
      resourceInfo: ResourceInfo.fromJson(json['resource_info']),
      packageInfo: PackageInfo.fromJson(json['package_info']),
      group: List<String>.from(json['group']),
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'version': version,
      'texture_information_version': textureFormatVersion,
      'texture_format_category': textureFormatCategory,
      'resource_info': resourceInfo.toJson(),
      'package_info': packageInfo.toJson(),
      'group': group,
    };
  }
}

class ResourceInfo {
  String expandPath;
  bool useNewton;
  String? resourceAdditionalName;

  ResourceInfo({
    required this.expandPath,
    required this.useNewton,
    required this.resourceAdditionalName,
  });

  factory ResourceInfo.fromJson(Map<String, dynamic> json) {
    return ResourceInfo(
      expandPath: json['expand_path'],
      useNewton: json['use_new_type_resource'],
      resourceAdditionalName: json['resource_additional_name'],
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'expand_path': expandPath,
      'use_new_type_resource': useNewton,
      'resource_additional_name': resourceAdditionalName,
    };
  }
}

class PackageInfo {
  bool rtonIsEncrypted;
  bool autoConvertJsonIfExist;

  PackageInfo({
    required this.rtonIsEncrypted,
    required this.autoConvertJsonIfExist,
  });

  factory PackageInfo.fromJson(Map<String, dynamic> json) {
    return PackageInfo(
      rtonIsEncrypted: json['rton_is_encrypted'],
      autoConvertJsonIfExist: json['auto_convert_jsons_exist'],
    );
  }

  Map<String, dynamic> toJson() {
    return {
      'rton_is_encrypted': rtonIsEncrypted,
      'auto_convert_jsons_exist': autoConvertJsonIfExist,
    };
  }
}
