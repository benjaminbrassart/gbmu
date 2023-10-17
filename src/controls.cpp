/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   controls.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 15:02:14 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 15:15:00 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/controls.hpp"

#include <fstream>
#include <string>
#include <sstream>

namespace gbmu
{
    std::unordered_map<std::string, control_name> const controls::CONTROL_NAMES {
        {"Up", control_name::Up},
        {"Down", control_name::Down},
        {"Left", control_name::Left},
        {"Right", control_name::Right},
        {"A", control_name::A},
        {"B", control_name::B},
        {"Start", control_name::Start},
        {"Select", control_name::Select},
        {"Pause", control_name::Pause},
        {"Resume", control_name::Resume},
        {"Stop", control_name::Stop},
        {"Speed", control_name::Speed},
    };

    std::unordered_map<control_name, std::string> const controls::CONTROL_NAMES_REVERSE {
        {control_name::Up, "Up"},
        {control_name::Down, "Down"},
        {control_name::Left, "Left"},
        {control_name::Right, "Right"},
        {control_name::A, "A"},
        {control_name::B, "B"},
        {control_name::Start, "Start"},
        {control_name::Select, "Select"},
        {control_name::Pause, "Pause"},
        {control_name::Resume, "Resume"},
        {control_name::Stop, "Stop"},
        {control_name::Speed, "Speed"},
    };

    void controls::load(std::string const &path)
    {
        decltype(this->keys) new_keys;
        std::ifstream f(path);

        if (!f)
        {
            // could not open file for reading
            throw; // TODO throw proper exception
        }

        std::string line;
        int key;
        std::string ct_name;

        while (std::getline(f, line))
        {
            std::stringstream ss;

            ss << line;
            ss >> key >> ct_name;

            if (!ss.eof() || ss.fail())
            {
                // invalid line format
                throw; // TODO throw proper exception
            }

            auto it = controls::CONTROL_NAMES.find(ct_name);

            if (it == controls::CONTROL_NAMES.end())
            {
                // not such control name
                throw; // TODO throw proper exception
            }

            auto res = new_keys.insert(std::make_pair(key, it->second));

            if (!res.second)
            {
                // duplicated control
                throw; // TODO throw proper exception
            }
        }

        this->keys = new_keys;
    }
}
