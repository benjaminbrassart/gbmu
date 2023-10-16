/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/16 11:56:45 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/16 13:23:51 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>

#include <GLFW/glfw3.h>
#include <GL/gl.h>

namespace gbmu
{
    class mmu;

    class renderer
    {
    private:
        static int const WIN_WIDTH;
        static int const WIN_HEIGHT;
        static char const WIN_TITLE[];

    private:
        GLFWwindow *window;

    public:
        renderer();
        ~renderer();

    public:
        void render(mmu const &mmu);

        void poll_events(mmu const &mmu);

        void set_title(std::string const &title);

    private:
        static GLFWwindow *_create_window();

        static void key_callback(GLFWwindow *window, int key, int sc, int action, int mods);
    };
}
