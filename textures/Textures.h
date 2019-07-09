/*
 * Textures.h
 *
 *  Created on: 9th of July 2019.
 *  Author: vahit.k
 */

#ifndef TEXTURES_TEXTURES_H_
#define TEXTURES_TEXTURES_H_

#pragma once

#include "Common.h"

#include <memory>

std::unique_ptr<ITexture> MakeTextureSolid(Size size, char pixel);

std::unique_ptr<ITexture> MakeTextureCheckers(Size size, char pixel1,
                                              char pixel2);

std::unique_ptr<ITexture> MakeTextureCow();

#endif /* TEXTURES_TEXTURES_H_ */
