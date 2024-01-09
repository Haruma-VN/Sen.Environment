import 'package:fluent_ui/fluent_ui.dart';
import 'package:winshell/components/model/button.dart';
import 'package:winshell/interface/application.dart';

void main() {
  runApp(const MyApp());
}

class MyApp extends StatefulWidget {
  const MyApp({super.key});

  @override
  State<MyApp> createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  int that = 1;

  List<NavigationPaneItem> items = [
    PaneItem(
      icon: const Icon(FluentIcons.home),
      title: const Text('Home'),
      body: Container(),
    ),
    PaneItemSeparator(),
    PaneItemExpander(
      icon: const Icon(FluentIcons.issue_tracking),
      title: const Text('PopCap RSB'),
      body: Container(),
      items: [
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('Unpack'),
          body: Container(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('Pack'),
          body: Container(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('Unpack for Modding'),
          body: Container(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('Pack for Modding'),
          body: Container(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('Unpack by loose constraints'),
          body: Container(),
        ),
        PaneItem(
          infoBadge: const Text('New'),
          icon: const Icon(FluentIcons.package),
          title: const Text('Unpack Flash'),
          body: Container(),
        ),
      ],
    ),
    PaneItemExpander(
      icon: const Icon(FluentIcons.disable_updates),
      title: const Text('PopCap RSG'),
      body: Container(),
      items: [
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('Unpack'),
          body: Container(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('Pack'),
          body: WinShellButton(
            text: "Hello World",
            iconBegin: FluentIcons.a_a_d_logo,
            iconEnd: FluentIcons.a_t_p_logo,
            onPressed: () => debugPrint("hello world"),
          ),
        ),
      ],
    ),
    PaneItemExpander(
      icon: const Icon(FluentIcons.account_management),
      title: const Text('PopCap Animation'),
      body: Container(),
      items: [
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('Decode'),
          body: Container(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('Encode'),
          body: Container(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('To Flash'),
          body: Container(),
        ),
        PaneItem(
          icon: const Icon(FluentIcons.package),
          title: const Text('From Flash'),
          body: Container(),
        ),
      ],
    ),
  ];

  int topIndex = 0;

  @override
  Widget build(BuildContext context) {
    return FluentApp(
      debugShowCheckedModeBanner: false,
      themeMode: ThemeMode.system,
      home: NavigationView(
        appBar: const NavigationAppBar(
          title: Text(Application.app_name),
        ),
        pane: NavigationPane(
          selected: topIndex,
          onChanged: (index) => setState(() => topIndex = index),
          displayMode: PaneDisplayMode.open,
          items: items,
          footerItems: [
            PaneItem(
              icon: const Icon(FluentIcons.settings),
              title: const Text('Settings'),
              body: Container(),
            ),
            PaneItemAction(
              icon: const Icon(FluentIcons.add),
              title: const Text('Add New Item'),
              onTap: () {
                // Your Logic to Add New `NavigationPaneItem`
                items.add(
                  PaneItem(
                    icon: const Icon(FluentIcons.new_folder),
                    title: const Text('New Item'),
                    body: const Center(
                      child: Text(
                        'This is a newly added Item',
                      ),
                    ),
                  ),
                );
                setState(() {});
              },
            ),
          ],
        ),
      ),
    );
  }
}
