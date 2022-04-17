import 'dart:async';

import 'package:flutter/services.dart';

class QuickDeviceWatcher {
  static const MethodChannel _channel =
      const MethodChannel('quick_device_watcher');

  static Future<void> startWatch() => _channel.invokeMethod('startWatch');

  static Future<void> stopWatch() => _channel.invokeMethod('stopWatch');
}
