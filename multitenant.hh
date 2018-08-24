/*
 * Copyright (C) 2018 ScyllaDB
 */

/*
 * This file is part of Scylla.
 *
 * Scylla is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Scylla is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Scylla.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <seastar/core/scheduling.hh>
#include <seastar/core/sstring.hh>
#include <unordered_map>

#include "seastarx.hh"

struct tenant_config {
    scheduling_group sched_group;
};

struct multitenancy_config {
    tenant_config default_tenant;
    std::unordered_map<sstring, tenant_config> tenants;  // role -> tenant

    tenant_config get_config_for_tentant(const sstring& name) const; // returns default if not found
};
