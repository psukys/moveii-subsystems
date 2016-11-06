#include <dbus-cxx.h>
#include <unistd.h>

/**
 * Interface specification from: https://redmine.move2space.de/projects/move2/wiki/ADCS_SW_Interface
 * Dummy implementation.
 */

std::string _name = "ADCS";
uint8_t _mode;
uint16_t _coilconfig;
std::string _tle;
float _time;

void rebootADCS() {
  std::cout << _name << ": rebooting ADCS!" << std::endl;
}

uint8_t setMode(uint8_t Mode) {
  std::cout << _name << ": setting mode to " << Mode << std::endl;
  _mode = Mode;
  // return mode set?
  return _mode;
}

uint16_t setCoilConfig(uint16_t CoilConfig) {
  std::cout << _name << ": setting coil configuration to " << CoilConfig << std::endl;
  _coilconfig = CoilConfig;
  // return coil config set?
  return _coilconfig;
}

void setTLE(std::string TLE) {
  std::cout << _name << ": setting TLE to " << TLE << std::endl;
  _tle = TLE;
}

void updateClock(float Time) {
  std::cout << _name << ": updating clock to " << Time << std::endl;
  _time = Time;
}

void flashADCS() {
  std::cout << _name << ": flashing ADCS!" << std::endl;
}

int main()
{
  int ret;

  DBus::init();
  
  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();
  DBus::Connection::pointer conn = dispatcher->create_connection(DBus::BUS_SESSION);
  ret = conn->request_name( "moveii.adcs", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;

  // create an object on us
  DBus::Object::pointer object = conn->create_object("/moveii/adcs");

  // publish our methods
  object->create_method<void>("moveii.adcs", "rebootADCS", sigc::ptr_fun(rebootADCS));
  object->create_method<uint8_t, uint8_t>("moveii.adcs", "setMode", sigc::ptr_fun(setMode));
  object->create_method<uint16_t, uint16_t>("moveii.adcs", "setCoilConfig", sigc::ptr_fun(setCoilConfig));
  object->create_method<void, std::string>("moveii.adcs", "setTLE", sigc::ptr_fun(setTLE));
  object->create_method<void, float>("moveii.adcs", "updateClock", sigc::ptr_fun(updateClock));
  object->create_method<void>("moveii.adcs", "flashADCS", sigc::ptr_fun(flashADCS));

  while(true) {}
  
  return 0;
}
