// Copyright (c) 2019-2020 Tom Hancocks
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include "target/target.hpp"
#include "diagnostic/fatal.hpp"
#include "parser/file.hpp"

// MARK: - Constructors

kdl::target::target()
    : m_dst_root("."), m_dst_file("result")
{

}

// MARK: - Container Management

auto kdl::target::add_type_container(const build_target::type_container container) -> void
{
    m_type_containers.emplace_back(container);
}

auto kdl::target::type_container_count() const -> std::size_t
{
    return m_type_containers.size();
}

auto kdl::target::type_container_at(const int i) const -> build_target::type_container
{
    return m_type_containers[i];
}

auto kdl::target::type_container_named(const kdl::lexeme name) const -> build_target::type_container
{
    for (auto c : m_type_containers) {
        if (c.name() == name.text()) {
            return c;
        }
    }
    log::fatal_error(name, 1, "Missing definition for type '" + name.text() + "'");
}


// MARK: - Destination Paths

auto kdl::target::set_dst_path(const std::string dst_path) -> void
{
    auto path = dst_path;
    std::string filename;

    if (kdl::file::exists(path) && !kdl::file::is_directory(path)) {
        while (path.substr(path.size() - 1) != "/") {
            filename = path.substr(path.size() - 1) + filename;
            path.pop_back();
        }
    }
    else if (kdl::file::exists(path) && kdl::file::is_directory(path)) {
        filename = "result";
    }
    else {
        while (path.substr(path.size() - 1) != "/") {
            filename = path.substr(path.size() - 1) + filename;
            path.pop_back();
        }

        if (!kdl::file::exists(path)) {
            kdl::file::is_directory(path);
        }
    }

    // Make sure the end of the path is a name.
    if (path.substr(path.size() - 1) == "/") {
        path.pop_back();
    }

    m_dst_root = path;
    m_dst_file = filename;
}

// MARK: - Scenario Paths

auto kdl::target::set_scenario_root(std::string_view path) -> void
{
    m_scenario_root = std::string(path);

    if (m_scenario_root.substr(m_scenario_root.size() - 1) == "/") {
        m_scenario_root.pop_back();
    }
}

auto kdl::target::scenario_manifest(std::string_view scenario_name) -> std::string
{
    auto path = kdl::file::resolve_tilde(m_scenario_root) + "/" + std::string(scenario_name);
    if (!kdl::file::exists(path) && !kdl::file::is_directory(path)) {
        std::cerr << "Could not find scenario named: '" + std::string(scenario_name) + "'." << std::endl;
        exit(1);
    }

    path.append("/manifest.kdl");
    if (!kdl::file::exists(path)) {
        std::cerr << "Scenario '" + std::string(scenario_name) + "' is missing a 'manifest.kdl' file." << std::endl;
        exit(1);
    }

    return path;
}

// MARK: - Source Paths

auto kdl::target::set_src_root(const std::string src_root) -> void
{
    auto path = src_root;
    // This needs to be a directory. Check if the path provided is a KDL file. If it is truncate the file name.
    // If there is a terminating /, then truncate it too.
    if (path.substr(path.size() - 4) == ".kdl") {
        while (path.substr(path.size() - 1) != "/") {
            path.pop_back();
        }
    }

    if (path.substr(path.size() - 1) == "/") {
        path.pop_back();
    }

    // Save the path.
    m_src_root = path;
}

auto kdl::target::resolve_src_path(const std::string path) const -> std::string
{
    // TODO: Improve this so it actually makes more sense.
    std::string rpath("@rpath"); // Root Path (Location of Input File)
    std::string spath("@spath"); // Scenario Path (Location of Scenario Type Definitions)
    std::string opath("@opath"); // Output Path (Location of Target Output)

    if (path.substr(0, rpath.size()) == rpath) {
        return m_src_root + path.substr(rpath.size());
    }

    return path;
}

// MARK: - Resource Management

auto kdl::target::add_resource(const build_target::resource_instance resource) -> void
{
    m_file.add_resource(resource.type_code(), resource.id(), resource.name(), resource.assemble());
}

// MARK: - Saving

auto kdl::target::target_file_path() const -> std::string
{
    auto path = m_dst_root;

    if (path.substr(path.size() - 1) != "/") {
        path += "/";
    }
    path += m_dst_file;

    switch (m_format) {
        case graphite::rsrc::file::format::classic:
            path += ".ndat";
            break;
        case graphite::rsrc::file::format::extended:
            path += ".kdat";
            break;
        case graphite::rsrc::file::format::rez:
            path += ".rez";
            break;
    }

    return path;
}

auto kdl::target::set_output_file(const std::string file) -> void
{
    m_dst_file = file;
}

auto kdl::target::save() -> void
{
    m_file.write(target_file_path(), m_format);
}