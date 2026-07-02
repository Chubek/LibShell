#include "LibShell-Posix.hpp"

#include <iostream>
#include <memory>
#include <string>
#include <string_view>

namespace lsh::cli {
Result<ir::Program> parse_line(std::string_view line);
}

namespace {

void print_diagnostic(const lsh::Diagnostic& diagnostic) {
    std::cerr << "libsh: " << diagnostic.message;
    if (!diagnostic.path.empty()) {
        std::cerr << " (" << diagnostic.path << ')';
    }
    std::cerr << '\n';
}

int run_line(lsh::Shell& shell, std::string_view line) {
    auto program = lsh::cli::parse_line(line);
    if (!program) {
        print_diagnostic(program.error());
        return 2;
    }
    auto report = shell.run(program.value());
    if (!report) {
        print_diagnostic(report.error());
        return 1;
    }
    for (const auto& diagnostic : report.value().diagnostics) {
        print_diagnostic(diagnostic);
    }
    return report.value().status.code;
}

} // namespace

int main(int argc, char** argv) {
    lsh::Shell shell {std::make_shared<lsh::LocalExecutor>()};
    shell.set_command_substitution_parser(
        [](std::string_view line) { return lsh::cli::parse_line(line); });
    if (argc > 1) {
        std::string line;
        for (int index = 1; index < argc; ++index) {
            if (!line.empty()) {
                line.push_back(' ');
            }
            line += argv[index];
        }
        return run_line(shell, line);
    }

    std::string line;
    while (std::cout << "libsh> " && std::getline(std::cin, line)) {
        if (line == "exit" || line == "quit") {
            break;
        }
        if (line.empty()) {
            continue;
        }
        (void)run_line(shell, line);
    }
    return 0;
}
