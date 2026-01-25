#include "NvsStorage.h"

namespace Blinds {
    namespace Infrastructure {

        NvsStorage::NvsStorage() {}

        void NvsStorage::savePosition(int32_t position) {
            _prefs.begin("blinds", false);
            _prefs.putInt("pos", position);
            _prefs.end();
        }

        int32_t NvsStorage::loadPosition() {
            _prefs.begin("blinds", true);
            int32_t pos = _prefs.getInt("pos", -1);
            _prefs.end();
            return pos;
        }

        void NvsStorage::saveConfig(const std::string& configJson) {
            _prefs.begin("blinds", false);
            _prefs.putString("config", configJson.c_str());
            _prefs.end();
        }

        std::string NvsStorage::loadConfig() {
            _prefs.begin("blinds", true);
            String cfg = _prefs.getString("config", "");
            _prefs.end();
            return std::string(cfg.c_str());
        }
    }
}
