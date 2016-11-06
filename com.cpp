#include <dbus-cxx.h>
#include <unistd.h>

/**
 * Interface specification from: https://redmine.move2space.de/projects/move2/wiki/ADCS_SW_Interface
 * Dummy implementation.
 */

std::string _name = "COM";
int _dummy_value = 0;

void set_dummy(int dummy) {
  std::cout << _name << ": setting dummy to "<< dummy << std::endl;
  _dummy_value = dummy;
}

void increment_dummy() {
  _dummy_value++;
  std::cout << _name << ": dummy incremented to " << _dummy_value << std::endl;
}

int get_dummy() {
  std::cout << _name << ": dummy requested, sending (" << _dummy_value << ")!" << std::endl;
  return _dummy_value;
}

int main()
{
  int ret;

  DBus::init();
  
  DBus::Dispatcher::pointer dispatcher = DBus::Dispatcher::create();
  DBus::Connection::pointer conn = dispatcher->create_connection(DBus::BUS_SESSION);
  ret = conn->request_name( "moveii.com", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;

  // create an object on us
  DBus::Object::pointer object = conn->create_object("/moveii/com");

  // publish our methods
  object->create_method<void, int>("moveii.com", "set_dummy", sigc::ptr_fun(set_dummy));
  object->create_method<void>("moveii.com", "increment_dummy", sigc::ptr_fun(increment_dummy));
  object->create_method<int>("moveii.com", "get_dummy", sigc::ptr_fun(get_dummy));

  // create an object on ADCS
  DBus::ObjectProxy::pointer moveii;
  moveii = conn->create_object_proxy("moveii.adcs", "/moveii/adcs");
  
  //method proxy for adcs, works as a real method, but goes through dbus to work
  DBus::MethodProxy<void>& adcs_reboot = *(moveii->create_method<void>("moveii.adcs", "rebootADCS"));


  while(true) {
    sleep(3);
    std::cout << _name << ": telling to reboot for ADCS" << std::endl;
    adcs_reboot();
  }
  
  return 0;
}
