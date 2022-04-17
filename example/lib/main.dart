import 'package:flutter/material.dart';
import 'package:quick_device_watcher/quick_device_watcher.dart';

void main() {
  runApp(MyHome());
}

class MyHome extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: Scaffold(
        appBar: AppBar(
          title: const Text('Plugin example app'),
        ),
        body: MyApp(),
      ),
    );
  }
}

class MyApp extends StatefulWidget {
  @override
  _MyAppState createState() => _MyAppState();
}

class _MyAppState extends State<MyApp> {
  @override
  Widget build(BuildContext context) {
    return _buildColumn();
  }

  void log(String info) {
    ScaffoldMessenger.of(context).showSnackBar(SnackBar(content: Text(info)));
  }

  Widget _buildColumn() {
    return Column(
      children: [
        _start_stop(),
      ],
    );
  }

  Widget _start_stop() {
    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceEvenly,
      children: [
        ElevatedButton(
          child: Text('startWatch'),
          onPressed: () async {
            await QuickDeviceWatcher.startWatch();
            log('startWatch');
          },
        ),
        ElevatedButton(
          child: Text('stopWatch'),
          onPressed: () async {
            await QuickDeviceWatcher.stopWatch();
            log('stopWatch');
          },
        ),
      ],
    );
  }
}
