//  SuperTux
//  Copyright (C) 2006 Matthias Braun <matze@braunis.de>
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.

#ifndef HEADER_SUPERTUX_OBJECT_GAMEOBJS_HPP
#define HEADER_SUPERTUX_OBJECT_GAMEOBJS_HPP

#include <memory>

#include "math/vector.hpp"
#include "supertux/game_object.hpp"
#include "supertux/timer.hpp"
#include "video/color.hpp"

/* Bounciness of distros: */
#define NO_BOUNCE 0
#define BOUNCE 1

class Sprite;

class BouncyCoin : public GameObject
{
public:
  BouncyCoin(const Vector& pos, bool emerge=false);
  ~BouncyCoin();
  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);

private:
  std::auto_ptr<Sprite> sprite;
  Vector position;
  Timer timer;
  float emerge_distance;

private:
  BouncyCoin(const BouncyCoin&);
  BouncyCoin& operator=(const BouncyCoin&);
};

class BrokenBrick : public GameObject
{
public:
  BrokenBrick(std::auto_ptr<Sprite> sprite, const Vector& pos, const Vector& movement);
  ~BrokenBrick();

  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);

private:
  Timer timer;
  std::auto_ptr<Sprite> sprite;
  Vector position;
  Vector movement;

private:
  BrokenBrick(const BrokenBrick&);
  BrokenBrick& operator=(const BrokenBrick&);
};

class FloatingText : public GameObject
{
  static Color text_color;
public:
  FloatingText(const Vector& pos, const std::string& text_);
  FloatingText(const Vector& pos, int s);  // use this for score, for instance

  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);

private:
  Vector position;
  std::string text;
  Timer timer;
};

class SmokeCloud : public GameObject
{
public:
  SmokeCloud(const Vector& pos);
  ~SmokeCloud();

  virtual void update(float elapsed_time);
  virtual void draw(DrawingContext& context);

private:
  std::auto_ptr<Sprite> sprite;
  Timer timer;
  Vector position;

private:
  SmokeCloud(const SmokeCloud&);
  SmokeCloud& operator=(const SmokeCloud&);
};

#endif

/* EOF */