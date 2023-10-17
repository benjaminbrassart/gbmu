/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/16 13:02:57 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 14:04:56 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/renderer.hpp"
#include "gbmu/mmu.hpp"

#include <iostream>

namespace gbmu
{
    int const renderer::WIN_WIDTH = 160;
    int const renderer::WIN_HEIGHT = 144;
    char const renderer::WIN_TITLE[] = "gbmu";

    renderer::renderer() :
        window(this->_create_window()),
        joypads()
    {
    }

    renderer::~renderer()
    {
        glfwDestroyWindow(this->window);
    }

    void renderer::render(mmu const &)
    {
    }

    void renderer::poll_events(mmu const &)
    {
        glfwPollEvents();
    }

    void renderer::set_title(std::string const &title)
    {
        glfwSetWindowTitle(this->window, title.c_str());
    }

    GLFWwindow *renderer::_create_window()
    {
        static int const JOYPAD_IDS[] {
            GLFW_JOYSTICK_1,
            GLFW_JOYSTICK_2,
            GLFW_JOYSTICK_3,
            GLFW_JOYSTICK_4,
            GLFW_JOYSTICK_5,
            GLFW_JOYSTICK_6,
            GLFW_JOYSTICK_7,
            GLFW_JOYSTICK_8,
            GLFW_JOYSTICK_9,
            GLFW_JOYSTICK_10,
            GLFW_JOYSTICK_11,
            GLFW_JOYSTICK_12,
            GLFW_JOYSTICK_13,
            GLFW_JOYSTICK_14,
            GLFW_JOYSTICK_15,
            GLFW_JOYSTICK_16,
        };

        glfwInit();

        auto win = glfwCreateWindow(
            WIN_WIDTH, WIN_HEIGHT,
            WIN_TITLE,
            nullptr,
            nullptr
        );

        glfwSetWindowUserPointer(win, this);
        glfwSetWindowAttrib(win, GLFW_RESIZABLE, GLFW_FALSE);

        this->joypads.reserve(16);

        for (auto jid : JOYPAD_IDS)
        {
            auto jname = glfwGetGamepadName(jid);

            this->joypads.push_back(jname);
            std::cout << "Gamepad " << (jid + 1) << ": " << (jname == nullptr ? "(null)" : jname) << std::endl;
        }

        glfwSetKeyCallback(win, [](GLFWwindow *win, int key, int sc, int action, int mods) {
            auto r = reinterpret_cast<renderer *>(glfwGetWindowUserPointer(win));

            r->key_callback(win, key, sc, action, mods);
        });

        glfwSetDropCallback(win, [](GLFWwindow *win, int count, char const **paths) {
            auto r = reinterpret_cast<renderer *>(glfwGetWindowUserPointer(win));

            r->drop_callback(win, count, paths);
        });

        return win;
    }

    void renderer::key_callback(GLFWwindow *, int key, int sc, int action, int mods)
    {
        std::cout << std::dec
            << "key: " << key << std::endl
            << "sc: " << sc << std::endl
            << "action: " << action << std::endl
            << "mods: " << mods << std::endl
            << std::endl;
    }

    void renderer::drop_callback(GLFWwindow *, int count, char const **paths)
    {
        for (int i = 0; i < count; i += 1)
        {
            std::cout << "Drag and drop file: " << paths[i] << std::endl;
        }
    }
}
