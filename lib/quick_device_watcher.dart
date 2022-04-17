
import 'dart:async';

import 'package:flutter/services.dart';

class QuickDeviceWatcher {
  static const MethodChannel _channel =
      const MethodChannel('quick_device_watcher');

  static Future<String?> get platformVersion async {
    final String? version = await _channel.invokeMethod('getPlatformVersion');
    return version;
  }
}
