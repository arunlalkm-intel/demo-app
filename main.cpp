#include <boost/asio/io_context.hpp>
#include <boost/asio/spawn.hpp>

#include <sdbusplus/asio/connection.hpp>
#include <sdbusplus/asio/object_server.hpp>
#include <sdbusplus/asio/sd_event.hpp>
#include <sdbusplus/bus.hpp>
#include <sdbusplus/exception.hpp>
#include <sdbusplus/server.hpp>
#include <sdbusplus/timer.hpp>

#include <chrono>
#include <ctime>
#include <iostream>
#include <variant>

std::string testFunction(const int32_t& callCount) 
{
    return "success: " + std::to_string(callCount);
}

int main()
{
    // setup connection to dbus
    boost::asio::io_context io;
    auto conn = std::make_shared<sdbusplus::asio::connection>(io);

    // make object server
    conn->request_name("dbus.test.server");
    auto server = sdbusplus::asio::object_server(conn);

    // add interface 
    std::shared_ptr<sdbusplus::asio::dbus_interface> iface =
        server.add_interface("/test/object", "test.interface");

    // add properties
    iface->register_property("intVal", 33,
        sdbusplus::asio::PropertyPermission::readWrite);

    // add properties with specialized callbacks
    iface->register_property("valWithRange", 66,
        // custom set
        [](const int& req, int& propertyValue) {
            if (req >= 50 && req <= 100)
            {
                propertyValue = req;
                return true;
            }

            return false;
        });

    // add properties (vector)
    std::vector<std::string> myStringVec = { "some", "test", "data" };
    iface->register_property("myStringVec", myStringVec,
        sdbusplus::asio::PropertyPermission::readWrite);

    // add method
    iface->register_method("testMethod", testFunction);

    // initialize interface
    iface->initialize();

    io.run();

    return 0;
}
