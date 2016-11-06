#include <dbus-cxx.h>
#include <unistd.h>

/**
 * Interface specification from: https://redmine.move2space.de/projects/move2/wiki/ADCS_SW_Interface
 * Dummy implementation.
 */

std::string _name = "DEPLOY";
int _dummy_value;

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
  ret = conn->request_name( "moveii.deploy", DBUS_NAME_FLAG_REPLACE_EXISTING );
  if (DBUS_REQUEST_NAME_REPLY_PRIMARY_OWNER != ret) return 1;

  // create an object on us
  DBus::Object::pointer object = conn->create_object("/moveii/deploy");

  // publish our methods
  object->create_method<void, int>("moveii.deploy", "set_dummy", sigc::ptr_fun(set_dummy));
  object->create_method<void>("moveii.deploy", "increment_dummy", sigc::ptr_fun(increment_dummy));
  object->create_method<int>("moveii.deploy", "get_dummy", sigc::ptr_fun(get_dummy));

  // create an object on ADCS
  DBus::ObjectProxy::pointer adcs, com;
  adcs = conn->create_object_proxy("moveii.adcs", "/moveii/adcs");
  com = conn->create_object_proxy("moveii.com", "/moveii/com");

  //method proxy for adcs, works as a real method, but goes through dbus to work
  DBus::MethodProxy<void>& adcs_flashADCS = *(adcs->create_method<void>("moveii.adcs", "flashADCS"));

  //method proxy for com
  DBus::MethodProxy<void>& com_increment_dummy = *(com->create_method<void>("moveii.com", "increment_dummy"));


  while(true) {
    sleep(3);
    std::cout << _name << ": telling to flash for ADCS" << std::endl;
    adcs_flashADCS();
    sleep(2);
    std::cout << _name << ": telling to increment dummy for COM" << std::endl;
    com_increment_dummy();
  }
  
  return 0;
}
