// ignore_for_file: file_names

class BasicException {
  final String message;

  final StackTrace stackTrace;

  static BasicException from(String message, StackTrace stackTrace) {
    return BasicException(message: message, stackTrace: stackTrace);
  }

  const BasicException({required this.message, required this.stackTrace});
}
