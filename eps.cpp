#include <dbus-cxx.h>
#include <unistd.h>

/**
 * Interface specification from: https://redmine.move2space.de/projects/move2/wiki/ADCS_SW_Interface
 * Dummy implementation.
 */

std::string _name = "EPS";
int _channels[10] = {0};
int _watchdog_limit = 0;

void restartSatellite() {
  std::cout << _name << ": restarting sattelite" << std::endl;
}

void switchPDMn(int ch, int state) {
  if (ch >= 0 || ch <= 9) {
    std::cout << _name << ": switching PDM on channel " << ch << " to " << state << std::endl;
    _channels[ch] = state;
  } else {
    // maybe some return value?
    std::cout << _name << ": cannot switch PDM - channel out of bounds" << std::endl;
  }
}

void setWatchdogLimit(int limit) {
  std::cout << _name << ": setting watchdog limit to " << limit << std::endl;
  _watchdog_limit = limit;
}

void resetWD() {
  std::cout << _name << ": resetting WD" << std::endl;
}

void resetEPS() {
  std::cout << _name << ": resetting EPS" << std::endl;
}

void initPDMs() {
  std::cout << _name << ": initializing PDMs" << std::endl;
}

void setPDMinitState(int ch, int state) {
  if (ch >= 0 || ch <= 9) {
    std::cout << _name << ": setting PDM initial state to " << state << " on " << ch << std::endl;
    _channels[ch] = state;
  } else {
    std::cout << _name << ": cannot init PDM - channel out of bounds" << std::endl; 
  }
}

int main()
{
  int ret;

  DBus::init();
  
  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();
  DBus::Connection::pointer conn = dispatcher->create_connection(DBus::BUS_SESSION);
  ret = conn->request_name( "moveii.eps", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;

  // create an object on us
  DBus::Object::pointer object = conn->create_object("/moveii/eps");

  // publish our methods
  object->create_method<void>("moveii.eps", "restartSatellite", sigc::ptr_fun(restartSatellite));
  object->create_method<void, int ,int >("moveii.eps", "switchPDMn", sigc::ptr_fun(switchPDMn));
  object->create_method<void, int>("moveii.eps", "setWatchdogLimit", sigc::ptr_fun(setWatchdogLimit));
  object->create_method<void>("moveii.eps", "resetWD", sigc::ptr_fun(resetWD));
  object->create_method<void>("moveii.eps", "resetEPS", sigc::ptr_fun(resetEPS));
  object->create_method<void>("moveii.eps", "initPDMs", sigc::ptr_fun(initPDMs));
  object->create_method<void, int, int>("moveii.eps", "setPDMinitState", sigc::ptr_fun(setPDMinitState));
  // create an object on ADCS
  DBus::ObjectProxy::pointer adcs, com, deploy;
  adcs = conn->create_object_proxy("moveii.adcs", "/moveii/adcs");
  com = conn->create_object_proxy("moveii.com", "/moveii/com");
  deploy = conn->create_object_proxy("moveii.deploy", "/moveii/deploy");

  //method proxy for adcs, works as a real method, but goes through dbus to work
  DBus::MethodProxy<void, std::string>& adcs_setTLE = *(adcs->create_method<void, std::string>("moveii.adcs", "setTLE"));

  //method proxy for com
  DBus::MethodProxy<void, int>& com_set_dummy = *(com->create_method<void, int>("moveii.com", "set_dummy"));

  //method proxy for deploy
  DBus::MethodProxy<int>& deploy_get_dummy = *(deploy->create_method<int>("moveii.deploy", "get_dummy"));

  while(true) {
    sleep(5);
    std::cout << _name << ": telling to set TLE to \"MOVEII TO EARTH, MOVEII TO EARTH\" for ADCS" << std::endl;
    adcs_setTLE("MOVEII TO EARTH, MOVEII TO EARTH");
    sleep(4);
    std::cout << _name << ": telling to set dummy to 1 for com" << std::endl;
    com_set_dummy(1);
    sleep(3);
    std::cout << _name << ": asking for dummy from deploy" << std::endl;
    std::cout << _name << ": got " << deploy_get_dummy() << std::endl;
  }
  
  return 0;
}
