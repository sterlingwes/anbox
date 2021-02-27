/*
 * Copyright (C) 2016 Simon Fels <morphis@gravedo.de>
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 3, as published
 * by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranties of
 * MERCHANTABILITY, SATISFACTORY QUALITY, or FITNESS FOR A PARTICULAR
 * PURPOSE.  See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <signal.h>
// #include <sys/prctl.h>

#include "anbox/android/ip_config_builder.h"
#include "anbox/utils.h"
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include <fstream>
#include <sstream>

constexpr const char *default_container_ip_address{"192.168.250.2"};
constexpr const std::uint32_t default_container_ip_prefix_length{24};
constexpr const char *default_host_ip_address{"192.168.250.1"};
constexpr const char *default_dns_server{"8.8.8.8"};

namespace fs = boost::filesystem;

namespace anbox {
Daemon::Daemon()
    : cmd{cli::Name{"anbox"}, cli::Usage{"anbox"},
          cli::Description{"The Android in a Box runtime"}} {

}

int Daemon::Run(const std::vector<std::string> &arguments) try {
  
  std::string container_network_address_;
  std::string container_network_gateway_;
  std::vector<std::string> container_network_dns_servers_;

  anbox_android::IpConfigBuilder ip_conf;
  ip_conf.set_version(anbox_android::IpConfigBuilder::Version::Version2);
  ip_conf.set_assignment(anbox_android::IpConfigBuilder::Assignment::Static);

  std::string address = default_container_ip_address;
  std::uint32_t ip_prefix_length = default_container_ip_prefix_length;
  if (!container_network_address_.empty()) {
    auto tokens = anbox::utils::string_split(container_network_address_, '/');
    if (tokens.size() == 1 || tokens.size() == 2)
      address = tokens[0];
    if (tokens.size() == 2)
      ip_prefix_length = atoi(tokens[1].c_str());
  }
  ip_conf.set_link_address(address, ip_prefix_length);

  std::string gateway = default_host_ip_address;
  if (!container_network_gateway_.empty())
    gateway = container_network_gateway_;
  ip_conf.set_gateway(gateway);

  if (container_network_dns_servers_.size() > 0)
    ip_conf.set_dns_servers(container_network_dns_servers_);
  else
    ip_conf.set_dns_servers({default_dns_server});

  ip_conf.set_id(0);

  std::vector<std::uint8_t> buffer(512);
  anbox_common::BinaryWriter writer(buffer.begin(), buffer.end());
  const auto size = ip_conf.write(writer);

  // const auto data_ethernet_path = fs::path("data") / "misc" / "ethernet";
  // const auto ip_conf_dir = SystemConfiguration::instance().data_dir() / data_ethernet_path;
  // if (!fs::exists(ip_conf_dir))
  //   fs::create_directories(ip_conf_dir);

  // We have to walk through the created directory hierachy now and
  // ensure the permissions are set correctly. Otherwise the Android
  // system will fail to boot as it isn't allowed to write anything
  // into these directories. As previous versions of Anbox which were
  // published to our users did this incorrectly we need to check on
  // every startup if those directories are still owned by root and
  // if they are we move them over to the unprivileged user.

  // auto path = SystemConfiguration::instance().data_dir();
  // for (auto iter = data_ethernet_path.begin(); iter != data_ethernet_path.end(); iter++) {
  //   path /= *iter;

  //   struct stat st;
  //   if (stat(path.c_str(), &st) < 0) {
  //     WARNING("Cannot retrieve permissions of path %s", path);
  //     continue;
  //   }

  //   if (st.st_uid != 0 && st.st_gid != 0)
  //     continue;

  //   if (::chown(path.c_str(), unprivileged_uid, unprivileged_uid) < 0)
  //     WARNING("Failed to set owner for path '%s'", path);
  // }

  const auto ip_conf_dir = "/Users/wjohnson/Code/anbox/ipconfig/data/misc/ethernet";
  const auto ip_conf_path = "/Users/wjohnson/Code/anbox/ipconfig/data/misc/ethernet/ipconfig.txt";
  // if (fs::exists(ip_conf_path))
  //   fs::remove(ip_conf_path);

  std::ofstream f(ip_conf_path, std::ofstream::binary);
  if (f.is_open()) {
    f.write(reinterpret_cast<const char*>(buffer.data()), size);
    f.close();
  }
  // } else {
  //   ERROR("Failed to write IP configuration. Network functionality will not be available.");
  // }

} catch (std::exception &err) {
  ERROR("%s", err.what());

  return EXIT_FAILURE;
}
}  // namespace anbox
