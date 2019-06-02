#include <sys/select.h>
#include <unistd.h>

#include <algorithm>
#include <iostream>
#include <string>

#include "i3ipc++/ipc-util.hpp"
#include "i3ipc++/ipc.hpp"

namespace {

void printWorkspaceInfo(
    const std::vector<std::shared_ptr<i3ipc::workspace_t>>& workspaces,
    const std::string& current_mode) {
  std::cout << "%{A5:i3-msg workspace next:}";
  std::cout << "%{A4:i3-msg workspace prev:}";
  for (const auto& ws : workspaces) {
    std::cout << "%{A:i3-msg workspace " << ws->name << ":}%{+u}";
    if (ws->urgent) {
      std::cout << "%{U#bf700f}";
    } else if (ws->focused) {
      std::cout << "%{U#0f70bf}";
    } else if (ws->visible) {
      std::cout << "%{U#CCCCCC}";
    } else {
      std::cout << "%{U#888888}";
    }
    std::cout << " " << ws->name << " %{-u}%{A}";
  }
  std::cout << "%{U-}%{A}%{A}";
  if (current_mode != "default") {
    std::cout << " %{F#000000}%{B#AA0000}" << current_mode << "%{B-}%{F-}";
  }
}

bool workspaceCompare(const std::shared_ptr<i3ipc::workspace_t>& a,
                      const std::shared_ptr<i3ipc::workspace_t>& b) {
  return a->rect.x < b->rect.x;
}

int main_() {
  i3ipc::connection conn;
  auto workspaces = conn.get_workspaces();
  std::stable_sort(workspaces.begin(), workspaces.end(), workspaceCompare);
  // We have to assume 'default' here because i3 has no way to poll the current
  // mode; we just have to wait for the first event to get accurate info.
  std::string current_mode{"default"};

  conn.signal_workspace_event.connect([&workspaces,
                                       &conn](const i3ipc::workspace_event_t&) {
    workspaces = conn.get_workspaces();
    std::stable_sort(workspaces.begin(), workspaces.end(), workspaceCompare);
  });
  conn.signal_mode_event.connect([&current_mode](const i3ipc::mode_t& mode) {
    current_mode = mode.change;
  });
  conn.subscribe(i3ipc::ET_WORKSPACE | i3ipc::ET_MODE);
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
    printWorkspaceInfo(workspaces, current_mode);
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
