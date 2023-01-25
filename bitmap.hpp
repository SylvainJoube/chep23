/*
 * This file is part of covfie, a part of the ACTS project
 *
 * Copyright (c) 2022 CERN
 *
 * This Source Code Form is subject to the terms of the Mozilla Public License,
 * v. 2.0. If a copy of the MPL was not distributed with this file, You can
 * obtain one at http://mozilla.org/MPL/2.0/.
 */

#pragma once

#include <string>
#include <kwk/kwk.hpp>

// Compute the exact type of our data storage
using img_type = kwk::make_table_t<kwk::int8_, kwk::_2D>;

void render_bitmap(img_type const& img, std::string fname);

