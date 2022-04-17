#include "include/quick_device_watcher/quick_device_watcher_plugin.h"

// This must be included before many other Windows headers.
#include <windows.h>
#include <dbt.h>

#include <flutter/method_channel.h>
#include <flutter/plugin_registrar_windows.h>
#include <flutter/standard_method_codec.h>

#include <map>
#include <memory>
#include <sstream>

namespace {

HWND GetRootWindow(flutter::FlutterView *view) {
  return ::GetAncestor(view->GetNativeWindow(), GA_ROOT);
}

class QuickDeviceWatcherPlugin : public flutter::Plugin {
 public:
  static void RegisterWithRegistrar(flutter::PluginRegistrarWindows *registrar);

  QuickDeviceWatcherPlugin(flutter::PluginRegistrarWindows *registrar);

  virtual ~QuickDeviceWatcherPlugin();

 private:
  // Called when a method is called on this plugin's channel from Dart.
  void HandleMethodCall(
      const flutter::MethodCall<flutter::EncodableValue> &method_call,
      std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result);

  std::optional<LRESULT> HandleWindowProc(
      HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);

  flutter::PluginRegistrarWindows *registrar_;

  int window_proc_id_ = -1;

  HDEVNOTIFY hDeviceNotify_ = NULL;
};

// static
void QuickDeviceWatcherPlugin::RegisterWithRegistrar(
    flutter::PluginRegistrarWindows *registrar) {
  auto channel =
      std::make_unique<flutter::MethodChannel<flutter::EncodableValue>>(
          registrar->messenger(), "quick_device_watcher",
          &flutter::StandardMethodCodec::GetInstance());

  auto plugin = std::make_unique<QuickDeviceWatcherPlugin>(registrar);

  channel->SetMethodCallHandler(
      [plugin_pointer = plugin.get()](const auto &call, auto result) {
        plugin_pointer->HandleMethodCall(call, std::move(result));
      });

  registrar->AddPlugin(std::move(plugin));
}

QuickDeviceWatcherPlugin::QuickDeviceWatcherPlugin(flutter::PluginRegistrarWindows *registrar)
    : registrar_(registrar) {
  window_proc_id_ = registrar_->RegisterTopLevelWindowProcDelegate(
    [this](HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
      return HandleWindowProc(hwnd, message, wparam, lparam);
    });
}

QuickDeviceWatcherPlugin::~QuickDeviceWatcherPlugin() {
  registrar_->UnregisterTopLevelWindowProcDelegate(window_proc_id_);
}

void QuickDeviceWatcherPlugin::HandleMethodCall(
    const flutter::MethodCall<flutter::EncodableValue> &method_call,
    std::unique_ptr<flutter::MethodResult<flutter::EncodableValue>> result) {
  if (method_call.method_name().compare("startWatch") == 0) {
    OutputDebugString(L"QuickDeviceWatcher: startWatch");

    DEV_BROADCAST_DEVICEINTERFACE db = {sizeof(DEV_BROADCAST_DEVICEINTERFACE), DBT_DEVTYP_DEVICEINTERFACE};
    hDeviceNotify_ = RegisterDeviceNotification(GetRootWindow(registrar_->GetView()),
      &db, DEVICE_NOTIFY_WINDOW_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES);

    if (hDeviceNotify_ != NULL) {
      result->Success(nullptr);
    } else {
      OutputDebugString(L"QuickDeviceWatcher: RegisterDeviceNotification error");
      result->Error(nullptr);
    }
  } else if (method_call.method_name().compare("stopWatch") == 0) {
    OutputDebugString(L"QuickDeviceWatcher: stopWatch");

    if (hDeviceNotify_ == NULL) {
      result->Success(nullptr);
    } else if (UnregisterDeviceNotification(hDeviceNotify_)) {
      hDeviceNotify_ = NULL;
      result->Success(nullptr);
    } else {
      OutputDebugString(L"QuickDeviceWatcher: UnregisterDeviceNotification error");
      result->Error(nullptr);
    }
  } else {
    result->NotImplemented();
  }
}

std::optional<LRESULT> QuickDeviceWatcherPlugin::HandleWindowProc(
    HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam) {
  std::optional<LRESULT> result;

  if (message == WM_DEVICECHANGE &&
    (wparam == DBT_DEVICEARRIVAL || wparam == DBT_DEVICEREMOVECOMPLETE)) {
    DEV_BROADCAST_HDR *hdr = (DEV_BROADCAST_HDR*)lparam;
    if (hdr->dbch_devicetype != DBT_DEVTYP_DEVICEINTERFACE) {
      return result;
    }

    DEV_BROADCAST_DEVICEINTERFACE *db = (DEV_BROADCAST_DEVICEINTERFACE*)hdr;
    if (wparam == DBT_DEVICEARRIVAL) {
      OutputDebugString(L"QuickDeviceWatcher: DBT_DEVICEARRIVAL\n");
      OutputDebugString(db->dbcc_name);
    } else if (wparam == DBT_DEVICEREMOVECOMPLETE) {
      OutputDebugString(L"QuickDeviceWatcher: DBT_DEVICEREMOVECOMPLETE\n");
      OutputDebugString(db->dbcc_name);
    }
    result = 0;
  }

  return result;
}

}  // namespace

void QuickDeviceWatcherPluginRegisterWithRegistrar(
    FlutterDesktopPluginRegistrarRef registrar) {
  QuickDeviceWatcherPlugin::RegisterWithRegistrar(
      flutter::PluginRegistrarManager::GetInstance()
          ->GetRegistrar<flutter::PluginRegistrarWindows>(registrar));
}
