class LogMessage {
  final String title;
  final String? subtitle;
  final DateTime time;

  const LogMessage({
    required this.title,
    required this.time,
    this.subtitle,
  });
}
