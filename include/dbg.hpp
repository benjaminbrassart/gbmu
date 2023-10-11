/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dbg.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbrassar <bbrassar@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/08 10:20:37 by bbrassar          #+#    #+#             */
/*   Updated: 2023/10/11 13:59:10 by bbrassar         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#pragma once

#define _str(Str) #Str
#define str(Str) _str(Str)

#define dbg(What) (std::cout << "[DEBUG] " << __FILE__ << ":" << __LINE__ << ": " << str(What) << std::endl, What)
