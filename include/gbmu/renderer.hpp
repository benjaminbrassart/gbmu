/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   renderer.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/16 11:56:45 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/17 14:03:11 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#include <string>
#include <vector>

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
        std::vector<char const *> joypads;

    public:
        renderer();
        ~renderer();

    public:
        void render(mmu const &mmu);

        void poll_events(mmu const &mmu);

        void set_title(std::string const &title);

    private:
        GLFWwindow *_create_window();

        void key_callback(GLFWwindow *win, int key, int sc, int action, int mods);
        void drop_callback(GLFWwindow *win, int count, char const **paths);
    };
}
