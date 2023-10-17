/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   controls.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 13:29:16 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 15:10:54 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <unordered_map>

namespace gbmu
{
    enum class control_name
    {
        Up,
        Down,
        Left,
        Right,
        A,
        B,
        Start,
        Select,

        Pause,
        Resume,
        Stop,

        Speed,
    };

    class controls
    {
    private:
        static std::unordered_map<std::string, control_name> const CONTROL_NAMES;
        static std::unordered_map<control_name, std::string> const CONTROL_NAMES_REVERSE;

    private:
        std::unordered_map<int, control_name> keys;

    public:
        void load(std::string const &path);
        void save(std::string const &path) const;
    };
}
