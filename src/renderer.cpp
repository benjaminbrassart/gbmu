/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/16 13:02:57 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/16 13:25:13 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "gbmu/renderer.hpp"
#include "gbmu/mmu.hpp"

namespace gbmu
{
    int const renderer::WIN_WIDTH = 160;
    int const renderer::WIN_HEIGHT = 144;
    char const renderer::WIN_TITLE[] = "gbmu";

    renderer::renderer() :
        window(renderer::_create_window())
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
        glfwInit();

        auto win = glfwCreateWindow(
            WIN_WIDTH, WIN_HEIGHT,
            WIN_TITLE,
            nullptr,
            nullptr
        );

        glfwSetWindowAttrib(win, GLFW_RESIZABLE, GLFW_FALSE);

        return win;
    }
}
