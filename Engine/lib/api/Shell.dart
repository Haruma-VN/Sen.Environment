// ignore: file_names
abstract class Shell {
  // API interact
  void sendMessage(String message);

  void sendMessageWithSubtitle(String title, String message);

  void sendMessageWithSubtitleAndColor(
      String title, String message, String color);

  void clearMessage();

  void changeLoadingStatus();

  void notify();
}
