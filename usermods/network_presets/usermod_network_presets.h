#pragma once

#include "wled.h"

#define NETWORK_PRESET_CONNECT 100
#define NETWORK_PRESET_DISCONNECT bootPreset

class UsermodNetworkPresets : public Usermod {
private:
  bool _connected;

public:
  void setup() { _connected = false; }

  void loop() {
    if (_connected && !Network.isConnected()) {
      _connected = false;
      applyPresetFromPlaylist(NETWORK_PRESET_DISCONNECT);
    }
  }

  void connected() {
    _connected = true;
    applyPresetFromPlaylist(NETWORK_PRESET_CONNECT);
  }
};