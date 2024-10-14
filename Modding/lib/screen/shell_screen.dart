import 'dart:async';
import 'dart:ffi';
import 'dart:io';
import 'dart:isolate';
import 'package:desktop_drop/desktop_drop.dart';
import 'package:ffi/ffi.dart';
import 'package:flutter/material.dart';
import 'package:material_symbols_icons/material_symbols_icons.dart';
import 'package:modding/model/api.dart';
import 'package:async/async.dart';
import 'package:modding/model/build_distribution.dart';
import 'package:modding/model/message.dart';
import 'package:modding/provider/setting_provider.dart';
import 'package:modding/service/file_service.dart';
import 'package:modding/service/pointer_service.dart';
import 'package:modding/widget/radio_button.dart';
import 'package:provider/provider.dart';

class ShellScreen extends StatefulWidget {
  final String holderPath;
  final List<String> arguments;

  const ShellScreen({
    super.key,
    required this.holderPath,
    required this.arguments,
  });

  @override
  State<ShellScreen> createState() => _ShellScreenState();
}

class _ShellScreenState extends State<ShellScreen> {
  TextEditingController? _inputController;
  late ScrollController _scrollController;
  bool _finished = false;
  late List<Message> _outputData;
  static SendPort? _sendPort;
  static DynamicLibrary? _dylib;
  String _stage = '';
  late Completer<String?> _completer;
  late List<String> _enumeration;
  late String _value;
  late Timer _timer;
  int _secondsPassed = 0;

  @override
  void initState() {
    _inputController = TextEditingController();
    _scrollController = ScrollController();
    _outputData = [];
    super.initState();
    _startTimer();
    _run(widget.arguments);
  }

  void _startTimer() {
    _timer = Timer.periodic(const Duration(seconds: 1), (Timer t) {
      if (mounted) {
        setState(() {
          _secondsPassed++;
        });
      }
    });
  }

  void _resetTimeCount() {
    setState(() {
      _secondsPassed = 0;
    });
  }

  static int _callback(
    Pointer<CStringList> list,
    Pointer<CStringView> destination,
  ) {
    var result = PointerService.toList(list.ref);
    assert(result.isNotEmpty, "result must be greater or equals 1");
    var command = result.removeAt(0);

    switch (command) {
      case 'display':
        _handleDisplayMessageCommand(result);
        break;
      case 'input':
        _handleInputCommand(destination);
        break;
      case 'input_boolean':
        _handleInputBooleanCommand(destination);
        break;
      case 'input_enumeration':
        _handleInputEnumerationCommand(result, destination);
        break;
      case 'version':
        _handleVersionCommand(destination);
        break;
      case 'is_gui':
        _handleIsGuiCommand(destination);
        break;
      case 'push_notification':
        _handlePushNotificationCallback(result);
        break;
      case 'finish':
        _handleFinishCallback();
        break;
      default:
        _handleUnexpectedCallback(result);
        break;
    }
    sleep(const Duration(milliseconds: 10));
    return 0;
  }

  static void _handleUnexpectedCallback(List<String> command) {
    _sendPort!.send(['unexpected', ...command]);
  }

  static void _handlePushNotificationCallback(List<String> result) {
    assert(result.length >= 2);
    _sendPort!.send(['push_notification', result[0], result[1]]);
  }

  static void _handleFinishCallback() {
    _sendPort!.send(['finish']);
  }

  static void _handleDisplayMessageCommand(List<String> result) {
    assert(result.isNotEmpty, "argument must be greater than 1");

    switch (result.length) {
      case 1:
        _sendPort!.send(['display', result[0]]);
        break;
      case 2:
        _sendPort!.send(['display', result[0], result[1]]);
        break;
      case 3:
        _sendPort!.send(['display', result[0], result[1], result[2]]);
        break;
    }
  }

  static void _handleInputCommand(Pointer<CStringView> destination) {
    var state = calloc<Bool>();
    state.value = false;
    _sendPort!.send(['input_string', state.address, destination.address]);

    while (!state.value) {
      sleep(const Duration(milliseconds: 10));
    }

    calloc.free(state);
  }

  static void _handleInputBooleanCommand(Pointer<CStringView> destination) {
    var state = calloc<Bool>();
    state.value = false;
    _sendPort!.send(['input_boolean', state.address, destination.address]);

    while (!state.value) {
      sleep(const Duration(milliseconds: 10));
    }

    calloc.free(state);
  }

  static void _handleInputEnumerationCommand(
      List<String> result, Pointer<CStringView> destination) {
    var state = calloc<Bool>();
    state.value = false;
    _sendPort!.send([
      'input_enumeration',
      state.address,
      destination.address,
      [...result]
    ]);

    while (!state.value) {
      sleep(const Duration(milliseconds: 10));
    }

    calloc.free(state);
  }

  static void _handleVersionCommand(Pointer<CStringView> destination) {
    var version = BuildDistribution.version.toString();
    destination.ref
      ..size = version.length
      ..value = version.toNativeUtf8();
    _sendPort!.send([null]);
  }

  static void _handleIsGuiCommand(Pointer<CStringView> destination) {
    var isGui = 1.toString();
    destination.ref
      ..size = isGui.length
      ..value = isGui.toNativeUtf8();
    _sendPort!.send([null]);
  }

  static Future<void> _sub(
    List<dynamic> isolateData,
  ) async {
    final mainSendPort = isolateData[0] as SendPort;
    final kernelPath = isolateData[1];
    _sendPort = mainSendPort;
    _dylib = DynamicLibrary.open(kernelPath);
    final execute = _dylib!
        .lookupFunction<KernelExecuteCAPI, KernelExecuteDartAPI>('execute');
    final subReceivePort = ReceivePort();
    final subStreamQueue = StreamQueue<dynamic>(subReceivePort);
    mainSendPort.send(subReceivePort.sendPort);
    final subEvent = await subStreamQueue.next as List<dynamic>;
    final scriptPath = subEvent[0] as String;
    final currentShell = subEvent[1] as String;
    final additionalArguments = subEvent[2] as List<String>;
    final script = calloc<CStringView>(1);
    final scriptNativeString = PointerService.toUint8List(scriptPath);
    final scriptPointer = PointerService.utf8ListToCString(scriptNativeString);
    script.ref
      ..value = scriptPointer
      ..size = scriptNativeString.length;
    final argumentList = [
      currentShell,
      kernelPath,
      scriptPath,
      ...additionalArguments,
    ];
    final argument = calloc<CStringView>(argumentList.length);
    for (var i = 0; i < argumentList.length; ++i) {
      final currentArgument = PointerService.toUint8List(argumentList[i]);
      final currentArgumentPointer =
          PointerService.utf8ListToCString(currentArgument);
      (argument + i).ref
        ..size = currentArgument.length
        ..value = currentArgumentPointer;
    }
    final arguments = calloc<CStringList>(1);
    arguments.ref
      ..size = argumentList.length
      ..value = argument;
    final result = execute(
      script,
      arguments,
      Pointer.fromFunction(_callback),
    );
    assert(result == 0);
    for (var i = 0; i < argumentList.length; ++i) {
      calloc.free((argument + i).ref.value.cast<Int8>());
    }
    calloc.free(arguments.ref.value);
    calloc.free(arguments);
    calloc.free(script.ref.value);
    calloc.free(script);
    mainSendPort.send(null);
    _sendPort = null;
    return;
  }

  String get _formattedTime {
    final hours = _secondsPassed ~/ 3600;
    final minutes = (_secondsPassed % 3600) ~/ 60;
    final seconds = _secondsPassed % 60;
    return '${_twoDigits(hours)}:${_twoDigits(minutes)}:${_twoDigits(seconds)}';
  }

  String _twoDigits(int n) {
    return n.toString().padLeft(2, '0');
  }

  @override
  void dispose() {
    _inputController?.dispose();
    _scrollController.dispose();
    super.dispose();
  }

  void _exit() {
    Navigator.of(context).pop();
  }

  void _scrollToBottom() {
    if (_scrollController.hasClients) {
      _scrollController.animateTo(
        _scrollController.position.maxScrollExtent,
        duration: const Duration(milliseconds: 300),
        curve: Curves.easeOut,
      );
    }
  }

  void _queryCommand(List<dynamic> event) {
    if (event.isEmpty) return;
    var command = event.removeAt(0);
    if (command == null) return;
    switch (command) {
      case 'display':
        _handleDisplayCommand(event);
        break;
      case 'push_notification':
        _handlePushNotificationCommand(event);
        break;
      case 'finish':
        _handleFinishCommand();
        break;
    }
  }

  void _handlePushNotificationCommand(List<dynamic> event) {
    if (event.isEmpty) return;
    var message = event[0];
    _pushNotification(message);
  }

  void _handleFinishCommand() {
    _setFinishState();
  }

  void _handleDisplayCommand(List<dynamic> event) {
    if (event.isEmpty) return;

    switch (event.length) {
      case 1:
        _sendDirectMessage(event[0]);
        break;
      case 2:
        _sendMessageWithSubtitle(event[0], event[1]);
        break;
      case 3:
        _sendMessageWithSubtitleAndColor(event[0], event[1], event[2]);
        break;
    }
  }

  Color _getColorBasedOnTheme() {
    return Theme.of(context).brightness == Brightness.dark
        ? Colors.grey
        : Colors.pinkAccent.withOpacity(0.05);
  }

  Color _exchangeColor(String color) {
    return switch (color) {
      'red' => Colors.red,
      'green' => Colors.green,
      'yellow' => Colors.yellow,
      'cyan' => Colors.cyan,
      'default' => _getColorBasedOnTheme(),
      _ => Colors.amber,
    };
  }

  void _sendDirectMessage(String message) {
    setState(() {
      _outputData.add(Message(title: message));
      _scrollToBottom();
    });
  }

  void _sendMessageWithSubtitle(String message, String subtitle) {
    setState(() {
      _outputData.add(Message(title: message, subtitle: subtitle));
    });
  }

  void _sendMessageWithSubtitleAndColor(
    String title,
    String subtitle,
    String color,
  ) {
    setState(() {
      _outputData.add(Message(title: title, subtitle: subtitle, color: color));
      _scrollToBottom();
    });
  }

  void _pushNotification(String message) {
    // TODO
  }

  void _setFinishState() {
    setState(() {
      _finished = true;
      _scrollToBottom();
    });
  }

  void _setPendingJob() {
    setState(() {
      _finished = false;
    });
  }

  void _clearMessage() {
    setState(() {
      _outputData.clear();
    });
  }

  Future<void> _inputString(
    Pointer<CStringView> destination,
  ) async {
    final completer = Completer<String?>();
    setState(() {
      _stage = 'input_string';
      _completer = completer;
    });
    final e = await completer.future;
    setState(() {
      _stage = 'any';
      _outputData.add(
        Message(title: 'User provided', subtitle: e, color: 'default'),
      );
    });
    final units = PointerService.toUint8List(e!);
    final utf8Str = PointerService.utf8ListToCString(units);
    destination.ref
      ..size = units.length
      ..value = utf8Str;
    _scrollToBottom();
    await WidgetsBinding.instance.endOfFrame;
  }

  Future<void> _inputEnumeration(
    Pointer<CStringView> destination,
    List<String> restStatement,
  ) async {
    final completer = Completer<String?>();
    setState(() {
      _enumeration = restStatement;
      _value = _enumeration[0];
      _stage = 'input_enumeration';
      _completer = completer;
    });
    final e = await completer.future;
    setState(() {
      _stage = 'any';
      _outputData.add(
        Message(
          title: 'User provided',
          subtitle: e,
          color: 'default',
        ),
      );
    });
    final units = PointerService.toUint8List(e!);
    final utf8Str = PointerService.utf8ListToCString(units);
    destination.ref
      ..size = units.length
      ..value = utf8Str;
    _scrollToBottom();
    await WidgetsBinding.instance.endOfFrame;
  }

  Future<void> _inputBoolean(
    Pointer<CStringView> destination,
  ) async {
    final completer = Completer<String?>();
    setState(() {
      _stage = 'input_boolean';
      _completer = completer;
    });
    final e = await completer.future;
    setState(() {
      _stage = 'any';
      _outputData.add(
        Message(
          title: 'User provided',
          subtitle: e,
          color: 'default',
        ),
      );
    });
    final units = PointerService.toUint8List(e!);
    final utf8Str = PointerService.utf8ListToCString(units);
    destination.ref
      ..size = units.length
      ..value = utf8Str;
    _scrollToBottom();
    await WidgetsBinding.instance.endOfFrame;
  }

  String _exchangeKernelPath({required SettingProvider provider}) {
    if (Platform.isAndroid) {
      return 'libKernel.so';
    }
    if (Platform.isWindows) {
      return '${provider.toolChain}/kernel.dll';
    }
    if (Platform.isLinux) {
      return '${provider.toolChain}/libKernel.so';
    }
    if (Platform.isMacOS || Platform.isIOS) {
      return '${provider.toolChain}/Kernel.dylib';
    }
    throw Exception('unsupported platform');
  }

  void _run(
    List<String> additionalArguments,
  ) async {
    final settingProvider =
        Provider.of<SettingProvider>(context, listen: false);
    _setPendingJob();
    _clearMessage();
    final mainReceivePort = ReceivePort();
    final mainStreamQueue = StreamQueue<dynamic>(mainReceivePort);
    await Isolate.spawn(_sub, [
      mainReceivePort.sendPort,
      _exchangeKernelPath(provider: settingProvider),
    ]);
    final subSendPort = await mainStreamQueue.next as SendPort;
    subSendPort.send([
      '${settingProvider.toolChain}/script/main.js',
      (settingProvider.toolChain),
      additionalArguments
    ]);
    while (await mainStreamQueue.hasNext) {
      var mainEvent = await mainStreamQueue.next as List<dynamic>?;
      if (mainEvent == null) {
        mainEvent = await mainStreamQueue.next as List<dynamic>;
        break;
      } else {
        if (mainEvent[0] != null &&
            (mainEvent[0] as String).contains('input')) {
          final callbackState = Pointer<Bool>.fromAddress(mainEvent[1]);
          final destination = Pointer<CStringView>.fromAddress(mainEvent[2]);
          statement:
          switch (mainEvent[0]) {
            case 'input_string':
              {
                await _inputString(destination);
                break statement;
              }
            case 'input_enumeration':
              {
                final restStatement = (mainEvent[3] as List<String>);
                await _inputEnumeration(destination, restStatement);
                break statement;
              }
            case 'input_boolean':
              {
                await _inputBoolean(destination);
                break statement;
              }
          }
          callbackState.value = true;
        } else {
          _queryCommand(mainEvent);
        }
      }
    }
    _setFinishedState();
    await mainStreamQueue.cancel();
    return;
  }

  void _setFinishedState() {
    setState(() {
      _finished = true;
      _scrollToBottom();
    });
  }

  Widget _makeStage() {
    if (_finished) {
      return _buildFinishedStage();
    }
    switch (_stage) {
      case 'input_string':
        return _buildInputStringStage();
      case 'input_enumeration':
        return _buildInputEnumerationStage();
      case 'input_boolean':
        return _buildInputBooleanStage();
      default:
        return const SizedBox.shrink();
    }
  }

  Widget _buildFinishedStage() {
    _timer.cancel();
    return Column(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        Text('Elapsed Time: $_formattedTime'),
        const SizedBox(height: 10),
        const Text('Execute again?'),
        const SizedBox(height: 10),
        Row(
          children: [
            Expanded(
              child: ElevatedButton(
                onPressed: () {
                  setState(() {
                    _outputData.clear();
                  });
                  _resetTimeCount();
                  _startTimer();
                  _run(widget.arguments);
                },
                child: const Padding(
                  padding: EdgeInsets.all(16.0),
                  child: Text('Yes'),
                ),
              ),
            ),
            const SizedBox(width: 10),
            Expanded(
              child: ElevatedButton(
                onPressed: _exit,
                child: const Padding(
                  padding: EdgeInsets.all(16.0),
                  child: Text('No'),
                ),
              ),
            ),
          ],
        ),
      ],
    );
  }

  void _onUploadFile() async {
    var result = await FileService.uploadFile();
    if (result != null && _inputController != null) {
      _inputController!.text = result;
    }
  }

  void _onUploadDirectory() async {
    var result = await FileService.uploadDirectory();
    if (result != null && _inputController != null) {
      _inputController!.text = result;
    }
  }

  void _onSelect() {
    showModalBottomSheet(
      context: context,
      builder: (BuildContext context) {
        return Wrap(
          children: [
            ListTile(
              leading: const Icon(Icons.upload_file),
              title: const Text('Upload File'),
              onTap: () {
                _onUploadFile();
                Navigator.of(context).pop();
              },
            ),
            ListTile(
              leading: const Icon(Icons.folder),
              title: const Text('Select Directory'),
              onTap: () {
                _onUploadDirectory();
                Navigator.of(context).pop();
              },
            ),
            ListTile(
              leading: const Icon(Icons.history_outlined),
              title: const Text('Recent files'),
              onTap: () {},
            ),
            ListTile(
              leading: const Icon(Icons.cancel),
              title: const Text('Cancel'),
              onTap: () {
                Navigator.pop(context);
              },
            ),
          ],
        );
      },
    );
  }

  Widget _buildInputStringStage() {
    return Row(
      children: [
        Expanded(
          child: TextField(
            controller: _inputController,
            onSubmitted: (e) => _onSendString(),
          ),
        ),
        IconButton(
          onPressed: _onSelect,
          icon: const Icon(Symbols.radio_button_checked_rounded),
        ),
        IconButton(
          onPressed: _onSendString,
          icon: const Icon(Icons.send_outlined),
        ),
      ],
    );
  }

  void _onSendString() {
    final inputData = _inputController!.text;
    _completer.complete(inputData);
    _inputController!.text = '';
    setState(() {
      _outputData.add(Message(
        title: inputData,
      ));
    });
  }

  Widget _buildInputEnumerationStage() {
    final list = DropdownButton<String>(
      value: _value,
      isExpanded: true,
      focusColor: Colors.transparent,
      borderRadius: const BorderRadius.all(Radius.circular(10)),
      underline: Container(),
      items: _enumeration
          .map((e) => DropdownMenuItem<String>(
                value: e,
                child: Text(e),
              ))
          .toList(),
      onChanged: (String? newValue) {
        if (newValue == null) return;
        setState(() {
          _value = newValue;
        });
      },
    );
    return Row(
      children: [
        Expanded(child: list),
        IconButton(
          onPressed: _onSendEnumeration,
          icon: const Icon(Icons.send_outlined),
        ),
      ],
    );
  }

  void _onSendEnumeration() {
    _completer.complete(_value);
  }

  Widget _buildInputBooleanStage() {
    final radioButton = RadioButton(
      options: const ['YES', 'NO'],
      isRowProvider: true,
    );
    return Row(
      children: [
        Expanded(child: radioButton),
        IconButton(
          onPressed: () => _onSendBoolean(radioButton.currentOption),
          icon: const Icon(Icons.send_outlined),
        ),
      ],
    );
  }

  void _onSendBoolean(String currentOption) {
    final value = currentOption;
    _completer.complete(value == 'YES' ? '1' : '2');
  }

  Color? _color(Message e) {
    return e.color != null ? _exchangeColor(e.color!).withOpacity(0.42) : null;
  }

  Widget? _subtitle(Message e) {
    return e.subtitle != null && e.subtitle!.isNotEmpty
        ? SelectableText(e.subtitle!)
        : null;
  }

  Widget? _leading() {
    if (_finished) {
      return null;
    }
    return switch (_stage) {
      'input_string' => const Icon(Symbols.text_fields),
      'input_enumeration' => const Icon(Symbols.list),
      'input_boolean' => const Icon(Symbols.radio_button_unchecked),
      _ => const SizedBox.shrink(),
    };
  }

  Widget _text(String message) {
    return SelectableText(message);
  }

  Widget _messageDisplay(Message e) {
    return Padding(
      padding: const EdgeInsets.symmetric(
        horizontal: 8,
      ),
      child: Card(
        color: _color(e),
        child: ListTile(
          leading: const Icon(Symbols.terminal),
          title: _text(e.title),
          subtitle: _subtitle(e),
        ),
      ),
    );
  }

  Widget _containerWrap({
    required Widget child,
  }) {
    return Container(
      margin: const EdgeInsets.only(
        left: 16.0,
        right: 16.0,
        bottom: 8.0,
        top: 8.0,
      ),
      child: child,
    );
  }

  Widget _timePassed() {
    return Text(
      _formattedTime,
      style: const TextStyle(fontSize: 12),
    );
  }

  Widget _makeUserStage() {
    if (_finished) {
      return _containerWrap(child: _makeStage());
    }
    return _containerWrap(
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          _timePassed(),
          const SizedBox(height: 4),
          _loadingState(),
          const SizedBox(height: 4),
          Card(
            child: ListTile(
              leading: _leading(),
              title: _makeStage(),
            ),
          ),
        ],
      ),
    );
  }

  Widget _loadingState() {
    if (!_finished) {
      return const LinearProgressIndicator();
    }
    return const SizedBox.shrink();
  }

  Widget _buildUI() {
    return Column(
      children: [
        Expanded(
          child: SingleChildScrollView(
            controller: _scrollController,
            child: Column(
              children: [
                ..._outputData.map(
                  (e) => _messageDisplay(e),
                ),
              ],
            ),
          ),
        ),
        _makeUserStage(),
      ],
    );
  }

  Widget _buildDragAndDrop(Widget data) {
    if (Platform.isAndroid || Platform.isIOS) {
      return data;
    }
    return DropTarget(
      onDragDone: (details) {
        if (details.files.isNotEmpty) {
          var file = details.files.first;
          _inputController?.text = file.path;
        }
      },
      child: data,
    );
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Shell'),
      ),
      body: _buildDragAndDrop(
        _buildUI(),
      ),
    );
  }
}
