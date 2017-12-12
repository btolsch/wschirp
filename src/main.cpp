#include <sys/select.h>
#include <unistd.h>

#include <iostream>
#include <string>

#include "i3ipc++/ipc-util.hpp"
#include "i3ipc++/ipc.hpp"

namespace {

void printWorkspaceInfo(i3ipc::connection& conn) {
  auto workspaces = conn.get_workspaces();
  std::cout << "%{A5:i3-msg workspace next:}";
  std::cout << "%{A4:i3-msg workspace prev:}";
  for (const auto& ws : workspaces) {
    std::cout << "%{A:i3-msg workspace " << ws->name << ":}%{+u}";
    if (ws->urgent) {
      std::cout << "%{U#bf700f}";
    } else if (ws->focused) {
      std::cout << "%{U#0f70bf}";
    } else if (!ws->visible) {
      std::cout << "%{U#888888}";
    }
    std::cout << " " << ws->name << " %{-u}%{A}";
  }
  std::cout << "%{U-}%{A}%{A}";
}

int main_() {
  i3ipc::connection conn;

  conn.subscribe(i3ipc::ET_WORKSPACE);
  conn.connect_event_socket();

  int32_t event_fd = conn.get_event_socket_fd();
  fd_set readfds;
  FD_ZERO(&readfds);
  FD_SET(STDIN_FILENO, &readfds);
  FD_SET(event_fd, &readfds);
  std::string stdin_line;
  while (select(event_fd + 1, &readfds, nullptr, nullptr, nullptr) != -1) {
    const bool stdin_set = FD_ISSET(STDIN_FILENO, &readfds);
    const bool event_set = FD_ISSET(event_fd, &readfds);
    FD_SET(STDIN_FILENO, &readfds);
    FD_SET(event_fd, &readfds);
    if (stdin_set) {
      std::getline(std::cin, stdin_line);
      if (stdin_line.length() == 0) {
        return 0;
      }
    }
    if (event_set) {
      conn.handle_event();
    }
    printWorkspaceInfo(conn);
    std::cout << stdin_line << std::endl;
  }

  return 0;
}

}  // namespace

int main() {
  while (true) {
    try {
      return main_();
    } catch (i3ipc::ipc_error&) {
      // Wait for i3 to return if it's reloading.
      sleep(1);
    }
  }
}
