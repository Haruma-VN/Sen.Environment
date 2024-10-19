import 'dart:io';
import 'package:flutter_local_notifications/flutter_local_notifications.dart';
import 'package:local_notifier/local_notifier.dart';
import 'package:modding/model/build_distribution.dart';

class NotificationService {
  static FlutterLocalNotificationsPlugin? _flutterLocalNotificationsPlugin;

  static Future<void> initialize() async {
    if (Platform.isWindows) {
      await localNotifier.setup(
        appName: BuildDistribution.kApplicationName,
        shortcutPolicy: ShortcutPolicy.requireCreate,
      );
    }
    if (Platform.isLinux ||
        Platform.isMacOS ||
        Platform.isAndroid ||
        Platform.isIOS) {
      _flutterLocalNotificationsPlugin = FlutterLocalNotificationsPlugin();
      var initializationSettings = InitializationSettings(
        android: const AndroidInitializationSettings(
          '@mipmap/launcher_icon',
        ),
        iOS: const DarwinInitializationSettings(),
        macOS: const DarwinInitializationSettings(),
        linux: LinuxInitializationSettings(
          defaultActionName: '',
          defaultIcon: AssetsLinuxIcon(
            'assets/images/logo.png',
          ),
        ),
      );
      await _flutterLocalNotificationsPlugin!.initialize(
        initializationSettings,
        onDidReceiveNotificationResponse: null,
      );
    }
    return;
  }

  static Future<void> push(
    String title,
    String description,
  ) async {
    if (Platform.isWindows) {
      await LocalNotification(
        title: title,
        body: description,
      ).show();
    }
    if (Platform.isLinux ||
        Platform.isMacOS ||
        Platform.isAndroid ||
        Platform.isIOS) {
      await _flutterLocalNotificationsPlugin!.show(
        DateTime.now().millisecondsSinceEpoch % 0x80000000,
        title,
        description,
        const NotificationDetails(
          android: AndroidNotificationDetails(
            'com.haruma.sen.modding.notification_channel.main',
            'Main',
          ),
          iOS: DarwinNotificationDetails(),
          macOS: DarwinNotificationDetails(),
          linux: LinuxNotificationDetails(),
        ),
      );
    }
    return;
  }
}
