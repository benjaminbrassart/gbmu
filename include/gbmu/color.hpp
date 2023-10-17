/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   color.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/17 18:27:14 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 18:50:36 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <iostream>

namespace gbmu
{
    struct color
    {
    private:
        char const *_color;
        std::string _message;

    private:
        color(char const *color, std::string const message) :
            _color(color),
            _message(message)
        {
        }

    public:
        inline static color red(std::string const &s)
        {
            return color("\033[0;31m", s);
        }

        inline static color green(std::string const &s)
        {
            return color("\033[0;32m", s);
        }

        inline static color yellow(std::string const &s)
        {
            return color("\033[0;33m", s);
        }

    public:
        friend std::ostream &operator<<(std::ostream &stream, color const &color);
    };

    std::ostream &operator<<(std::ostream &stream, color const &color)
    {
        return stream << color._color << color._message << "\033[0m";
    }
}
