#include <sys/select.h>
#include <sys/time.h>

#include <chrono>
#include <iostream>

#include "i3ipc++/ipc.hpp"

int main() {
  i3ipc::connection conn;

  auto t1 = std::chrono::steady_clock::now();
  auto workspaces = conn.get_workspaces();
  for (const auto &ws : workspaces) {
    std::cout << ws->name;
    if (ws->focused) {
      std::cout << " focused";
    }
    if (ws->visible) {
      std::cout << " visible";
    }
    std::cout << std::endl;
  }
  auto t2 = std::chrono::steady_clock::now();

  std::cout
      << "ms: "
      << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()
      << std::endl;

  conn.signal_workspace_event.connect([](const i3ipc::workspace_event_t &ev) {
    std::cout << "workspace event" << std::endl;
    if (ev.current) {
      std::cout << ev.current->name << std::endl;
    }
  });
  conn.subscribe(i3ipc::ET_WORKSPACE);
  conn.connect_event_socket();

  int32_t event_fd = conn.get_event_socket_fd();
  struct timeval timeout {
    0, 500000
  };
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(event_fd, &readfds);
  t1 = std::chrono::steady_clock::now();
  while (select(event_fd + 1, &readfds, nullptr, nullptr, &timeout) != -1) {
    t2 = std::chrono::steady_clock::now();
    std::cout << "elapsed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1)
                     .count()
              << "ms" << std::endl;
    timeout.tv_sec = 0;
    timeout.tv_usec = 500000;
    if (!FD_ISSET(event_fd, &readfds)) {
      std::cout << "timeout" << std::endl;
      FD_SET(event_fd, &readfds);
      t1 = std::chrono::steady_clock::now();
      continue;
    }
    std::cout << "event" << std::endl;
    conn.handle_event();
    t1 = std::chrono::steady_clock::now();
  }

  return 0;
}
