import 'dart:io';
import 'package:path/path.dart' as path;

class PlatformService {
  static void revealInExplorer(String filePath) {
    if (Platform.isWindows) {
      const executable = 'explorer';
      final arguments = [
        '/select,',
        path.absolute(filePath).replaceAll('/', '\\')
      ];
      Process.run(executable, arguments);
    } else if (Platform.isMacOS) {
      const executable = 'open';
      final arguments = ['-R', path.absolute(filePath)];
      Process.run(executable, arguments);
    } else if (Platform.isLinux) {
      const executable = 'xdg-open';
      final arguments = [path.absolute(filePath)];
      Process.run(executable, arguments);
    }
  }
}
