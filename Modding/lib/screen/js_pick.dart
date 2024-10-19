import 'package:flutter/material.dart';
import 'package:modding/model/script.dart';
import 'package:modding/screen/shell/shell_screen.dart';
import 'package:modding/service/file_service.dart';
import 'package:flutter_gen/gen_l10n/app_localizations.dart';
import 'package:page_transition/page_transition.dart';

class JsPick extends StatefulWidget {
  const JsPick({
    super.key,
    required this.holder,
  });

  final String holder;

  @override
  State<JsPick> createState() => _JsPickState();
}

class _JsPickState extends State<JsPick> {
  late ScriptData _data;

  @override
  void initState() {
    _data = ScriptData.fromJson(
      FileService.readJson(
          source: '${widget.holder}/Script/Helper/script.json'),
    );
    super.initState();
  }

  @override
  Widget build(BuildContext context) {
    final los = AppLocalizations.of(context)!;
    return Scaffold(
      appBar: AppBar(
        title: Text(los.js_page),
      ),
      body: Padding(
        padding: const EdgeInsets.all(8.0),
        child: ListView.builder(
          itemCount: _data.data.length,
          itemBuilder: (context, index) {
            final item = _data.data[index];
            return Card(
              child: ListTile(
                leading: const Icon(Icons.javascript_outlined, size: 28),
                title: Text(item.name),
                subtitle: Text(item.description),
                onTap: () {
                  Navigator.of(context).push(
                    PageTransition(
                      duration: const Duration(milliseconds: 300),
                      type: PageTransitionType.rightToLeft,
                      child: ShellScreen(
                        holderPath: widget.holder,
                        arguments: [
                          '-method',
                          'js.evaluate',
                          '-source',
                          item.path.replaceFirst(
                            '.',
                            '${widget.holder}/Script/Helper',
                          ),
                        ],
                      ),
                    ),
                  );
                },
              ),
            );
          },
        ),
      ),
    );
  }
}
